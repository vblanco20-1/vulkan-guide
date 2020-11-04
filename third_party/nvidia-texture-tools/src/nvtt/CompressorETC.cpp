
#include "CompressorETC.h"

#include "nvmath/Vector.inl"
#include "nvmath/Matrix.inl"
#include "nvmath/Color.inl"
#include "nvcore/Utils.h"    // clamp

//#define HAVE_RGETC 0
//#define HAVE_ETCPACK 0 // Only enable in OSX for debugging.

#if HAVE_RGETC
#include "rg_etc1.h"
#endif

#if HAVE_ETCPACK
// From etcpack.cxx
extern void decompressBlockETC2(unsigned int block_part1, unsigned int block_part2, uint8 *img, int width, int height, int startx, int starty);
extern void decompressBlockAlpha(uint8* data, uint8* img, int width, int height, int ix, int iy);
extern void decompressBlockAlpha16bit(uint8* data, uint8* img, int width, int height, int ix, int iy);
extern int formatSigned;
#endif

#define assert nvCheck

using namespace nv;

// TODO:
// - Accurate rounding of signed 3-bit components.
// - Range based table selection.
// - Slower try all options table selection?
// - Trivial selector assignment.
// * Base point optimization.
// * Brute force base point optimization.
// - Enumerate and evaluate all clusters.
// - Brute force planar mode endpoint refinement. For each color try two rounding directions (8 tests).
// - T & H modes decompression.

union BlockETC {
    // Definitions from EtcLib/EtcBlock4x4EncodingBits.h
    struct Individual {
        uint red2 : 4;      // byte 0
        uint red1 : 4;
        uint green2 : 4;    // byte 1
        uint green1 : 4;
        uint blue2 : 4;     // byte 2
        uint blue1 : 4;
        uint flip : 1;      // byte 3
        uint diff : 1;
        uint cw2 : 3;
        uint cw1 : 3;
        uint selectors;     // bytes 4-7
    };
    NV_COMPILER_CHECK(sizeof(BlockETC::Individual) == 64/8);

    struct Differential {
        uint dred2 : 3;     // byte 0
        uint red1 : 5;
        uint dgreen2 : 3;   // byte 1
        uint green1 : 5;
        uint dblue2 : 3;    // byte 2
        uint blue1 : 5;
        uint flip : 1;      // byte 3
        uint diff : 1;
        uint cw2 : 3;
        uint cw1 : 3;
        uint selectors;     // bytes 4-7
    };
    NV_COMPILER_CHECK(sizeof(Differential) == 64/8);

    struct T {
        uint red1b : 2;     // byte 0
        uint detect2 : 1;
        uint red1a : 2;
        uint detect1 : 3;
        uint blue1 : 4;     // byte 1
        uint green1 : 4;
        uint green2 : 4;    // byte 2
        uint red2 : 4;
        uint db : 1;        // byte 3
        uint diff : 1;
        uint da : 2;
        uint blue2 : 4;
        uint selectors;     // bytes 4-7
    };
    NV_COMPILER_CHECK(sizeof(T) == 64/8);

    struct H {
        uint green1a : 3;   // byte 0
        uint red1 : 4;
        uint detect1 : 1;
        uint blue1b : 2;    // byte 1
        uint detect3 : 1;
        uint blue1a : 1;
        uint green1b : 1;
        uint detect2 : 3;
        uint green2a : 3;   // byte 2
        uint red2 : 4;
        uint blue1c : 1;
        uint db : 1;        // byte 3
        uint diff : 1;
        uint da : 1;
        uint blue2 : 4;
        uint green2b : 1;
        uint selectors;     // bytes 4-7
    };
    NV_COMPILER_CHECK(sizeof(H) == 64/8);

    struct Planar {
        uint originGreen1 : 1;  // byte 0
        uint originRed : 6;
        uint detect1 : 1;
        uint originBlue1 : 1;   // byte 1
        uint originGreen2 : 6;
        uint detect2 : 1;
        uint originBlue3 : 2;   // byte 2
        uint detect4 : 1;
        uint originBlue2 : 2;
        uint detect3 : 3;
        uint horizRed2 : 1;     // byte 3
        uint diff : 1;
        uint horizRed1 : 5;
        uint originBlue4 : 1;
        uint horizBlue1: 1;     // byte 4
        uint horizGreen : 7;
        uint vertRed1 : 3;      // byte 5
        uint horizBlue2 : 5;
        uint vertGreen1 : 5;    // byte 6
        uint vertRed2 : 3;
        uint vertBlue : 6;      // byte 7
        uint vertGreen2 : 2;
    };
    NV_COMPILER_CHECK(sizeof(Planar) == 64/8);


    uint64 data64;
    uint32 data32[2];
    uint8 data8[8];
    Individual individual;
    Differential differential;
    T t;
    H h;
    Planar planar;
};
NV_COMPILER_CHECK(sizeof(BlockETC) == 64/8);



static const int etc_intensity_modifiers[8][4] = {
    { -8,  -2,   2,   8 },
    { -17,  -5,  5,  17 },
    { -29,  -9,   9,  29 },
    {  -42, -13, 13,  42 },
    { -60, -18, 18,  60 },
    { -80, -24, 24,  80 },
    { -106, -33, 33, 106 },
    { -183, -47, 47, 183 }
};

static const int etc_intensity_range[8] = {
    16, 34, 58, 84, 120, 160, 212, 366
};

static const int etc_th_distances[8] = { 3, 6, 11, 16, 23, 32, 41, 64 };

static const uint8 etc_selector_scramble[] = { 3, 2, 0, 1 };
static const uint8 etc_selector_unscramble[] = { 2, 3, 1, 0 };


static float midpoints4[16];
NV_AT_STARTUP(
    for (int i = 0; i < 15; i++) {
        float f0 = float(((i+0) << 4) | ((i+0) >> 4)) / 255.0f;
        float f1 = float(((i+1) << 4) | ((i+1) >> 4)) / 255.0f;
        midpoints4[i] = (f0 + f1) * 0.5f;
    }
    midpoints4[15] = 1.0f;
);

static const float midpoints5[32] = {
    0.015686f, 0.047059f, 0.078431f, 0.111765f, 0.145098f, 0.176471f, 0.207843f, 0.241176f, 0.274510f, 0.305882f, 0.337255f, 0.370588f, 0.403922f, 0.435294f, 0.466667f, 0.5f,
    0.533333f, 0.564706f, 0.596078f, 0.629412f, 0.662745f, 0.694118f, 0.725490f, 0.758824f, 0.792157f, 0.823529f, 0.854902f, 0.888235f, 0.921569f, 0.952941f, 0.984314f, 1.0f
};

//static const float midpoints6[64];
//static const float midpoints7[128];



// ETC2 Modes:
// - ETC1:
//  - two partitions (flip modes): 2*(4x2, 2x4)
//  - two base colors stored as 444+444 or 555+333
//  - two 3 bit intensity modifiers
// - T Mode. 2 colors 444, 3 bit intensity modifiers, 2 bit indices.
// - H Mode. 2 colors 444, 3 bit intensity modifiers, 2 bit indices.
// - Planar mode: 3 colors 676

struct ETC_Data {
    enum Mode {
        Mode_ETC1,
        Mode_T,
        Mode_H,
        Mode_Planar,
    } mode;

    // @@ It may make more sense to store bit-expanded or even float colors here.
    union {
        struct {
            uint16 color0;      // 444 or 555
            uint16 color1;      // 444 or 333
            uint8 table0;       // 3 bits
            uint8 table1;       // 3 bits
            bool flip;          // partition mode
            bool diff;          // color encoding
        } etc;
        struct {
            uint16 color0;      // 444
            uint16 color1;      // 444
            uint8 table;        // 3 bits
        } t, h;
        struct {
            uint8 ro, go, bo;   // 676
            uint8 rh, gh, bh;   // 676
            uint8 rv, gv, bv;   // 676
        } planar;
    };

    uint8 selector[16];         // 2 bit indices (32 bits)
};

struct ETC_Solution {
    float error = NV_FLOAT_MAX;
    ETC_Data data;
};


struct ETC_Options {
    //bool fast_flip_mode_selection = false;
    bool use_rg_etc = true;
    bool enable_etc2 = true;
    bool use_planar = true;
    bool use_t_mode = true;
    bool use_h_mode = true;
    bool onebit_alpha = false;
    Vector3 color_weights = Vector3(1);
    
    //int8 eac_search_radius = 1;  // [0-3]
    //bool eac_11bit_mode = false;
};

/*static*/ float compress_etc(Vector4 input_colors[16], float input_weights[16], const ETC_Options & options, BlockETC * output);


struct BlockEAC {
    uint base : 8;
    uint table : 4;
    uint multiplier : 4;
    uint selectors0 : 8;
    uint selectors1 : 8;
    
    uint selectors2 : 8;
    uint selectors3 : 8;
    uint selectors4 : 8;
    uint selectors5 : 8;
};
NV_COMPILER_CHECK(sizeof(BlockEAC) == 64/8);

struct BlockETC_EAC {
    BlockEAC eac;
    BlockETC etc;
};
NV_COMPILER_CHECK(sizeof(BlockETC_EAC) == 128/8);

// EAC:
// 8 bit base code word
// 4 bit multiplier
// 4 bit table index
// 16 * 3 bit indices.

struct EAC_Data {
    uint8 alpha;        // 8 bits
    uint8 multiplier;   // 4 bits
    uint8 table_index;  // 4 bits
    uint8 selector[16]; // 3 bit indices
};

struct EAC_Solution {
    float error = NV_FLOAT_MAX;
    EAC_Data data;
};

struct EAC_Options {
    int search_radius = 1;      // 0 = fast, 1 = medium, 2 = slow
    bool use_11bit_mode = false;
};


static const int eac_intensity_modifiers[16][8] = {
    {-3, -6, -9, -15, 2, 5, 8, 14}, // 0
    {-3, -7,-10, -13, 2, 6, 9, 12}, // 1
    {-2, -5, -8, -13, 1, 4, 7, 12}, // 2
    {-2, -4, -6, -13, 1, 3, 5, 12}, // 3
    {-3, -6, -8, -12, 2, 5, 7, 11}, // 4
    {-3, -7, -9, -11, 2, 6, 8, 10}, // 5
    {-4, -7, -8, -11, 3, 6, 7, 10}, // 6
    {-3, -5, -8, -11, 2, 4, 7, 10}, // 7
    {-2, -6, -8, -10, 1, 5, 7, 9 }, // 8
    {-2, -5, -8, -10, 1, 4, 7, 9 }, // 9
    {-2, -4, -8, -10, 1, 3, 7, 9 }, // 10
    {-2, -5, -7, -10, 1, 4, 6, 9 }, // 11
    {-3, -4, -7, -10, 2, 3, 6, 9 }, // 12
    {-1, -2, -3, -10, 0, 1, 2, 9 }, // 13
    {-4, -6, -8,  -9, 3, 5, 7, 8 }, // 14
    {-3, -5, -7,  -9, 2, 4, 6, 8 }  // 15
};




static void pack_etc2_block(const ETC_Data & data, BlockETC * output_block) {
    BlockETC block;

    bool swap_colors = false;

    if (data.mode == ETC_Data::Mode_ETC1) {
        // These are the same for individual and differential blocks.
        block.individual.diff = data.etc.diff;
        block.individual.flip = data.etc.flip;
        block.individual.cw1 = data.etc.table0;
        block.individual.cw2 = data.etc.table1;

        if (data.etc.diff) {
            block.differential.red1 = data.etc.color0 >> 10;
            block.differential.dred2 = data.etc.color1 >> 6;
            block.differential.green1 = (data.etc.color0 >> 5) & 0x1F;
            block.differential.dgreen2 = (data.etc.color1 >> 3) & 0x7;
            block.differential.blue1 = data.etc.color0 & 0x1F;
            block.differential.dblue2 = data.etc.color1 & 0x7;
        }
        else {
            block.individual.red1 = data.etc.color0 >> 8;
            block.individual.red2 = data.etc.color1 >> 8;
            block.individual.green1 = (data.etc.color0 >> 4) & 0xF;
            block.individual.green2 = (data.etc.color1 >> 4) & 0xF;
            block.individual.blue1 = data.etc.color0 & 0xF;
            block.individual.blue2 = data.etc.color1 & 0xF;
        }
    }
    else if (data.mode == ETC_Data::Mode_T) {
        block.t.red1a = (data.t.color0 >> 8) >> 2;
        block.t.red1b = (data.t.color0 >> 8);
        block.t.green1 = (data.t.color0 >> 4);
        block.t.blue1 = data.t.color0;

        block.t.red2 = (data.t.color1 >> 8);
        block.t.green2 = (data.t.color1 >> 4);
        block.t.blue2 = data.t.color1;

        block.t.da = data.t.table >> 1;
        block.t.db = data.t.table;

        block.t.diff = 1;

        // create an invalid R differential to trigger T mode
        int dr = block.differential.dred2;
        if (dr >= 4) dr -= 8;
        int r = block.differential.red1 + dr;

        block.t.detect1 = 0;
        block.t.detect2 = 1;
        if (r >= 4) {
            block.t.detect1 = 7;
            block.t.detect2 = 0;
        }
    }
    else if (data.mode == ETC_Data::Mode_H) {
        bool table_lsb = data.h.table & 1;
        swap_colors = (data.h.color0 < data.h.color1) ^ !table_lsb;

        uint16 color0 = data.h.color0;
        uint16 color1 = data.h.color1;

        if (swap_colors) {
            swap(color0, color1);
        }

        block.h.red1 = (data.h.color0 >> 8);
        block.h.green1a = (data.h.color0 >> 4) >> 1;
        block.h.green1b = (data.h.color0 >> 4);
        block.h.blue1a = data.h.color0 >> 3;
        block.h.blue1b = data.h.color0 >> 1;
        block.h.blue1c = data.h.color0;

        block.h.red2 = (data.h.color1 >> 8);
        block.h.green2a = (data.h.color1 >> 4) >> 1;
        block.h.green2b = (data.h.color1 >> 4);
        block.h.blue2 = (data.h.color1 >> 8);

        block.h.da = data.h.table >> 2;
        block.h.db = data.h.table >> 1;

        block.h.diff = 1;

        // create an invalid R differential to trigger T mode
        block.h.detect1 = 0;
        block.h.detect2 = 0;
        block.h.detect3 = 0;

        int dr = block.differential.dred2;
        int dg = block.differential.dgreen2;
        if (dr >= 4) dr -= 8;
        if (dg >= 4) dg -= 8;
        int r = block.differential.red1 + dr;
        int g = block.differential.green1 + dg;

        if (r < 0 || r > 31) {
            block.h.detect1 = 1;
        }
        if (g >= 4) {
            block.h.detect2 = 7;
            block.h.detect3 = 0;
        }
        else {
            block.h.detect2 = 0;
            block.h.detect3 = 1;
        }
    }

    if (data.mode == ETC_Data::Mode_Planar) {
        // From ETCLib:
        block.planar.originRed = data.planar.ro;
        block.planar.originGreen1 = data.planar.go >> 6;
        block.planar.originGreen2 = data.planar.go;
        block.planar.originBlue1 = data.planar.bo >> 5;
        block.planar.originBlue2 = data.planar.bo >> 3;
        block.planar.originBlue3 = data.planar.bo >> 1;
        block.planar.originBlue4 = data.planar.bo;

        block.planar.horizRed1 = data.planar.rh >> 1;
        block.planar.horizRed2 = data.planar.rh;
        block.planar.horizGreen = data.planar.gh;
        block.planar.horizBlue1 = data.planar.bh >> 5;
        block.planar.horizBlue2 = data.planar.bh;

        block.planar.vertRed1 = data.planar.rv >> 3;
        block.planar.vertRed2 = data.planar.rv;
        block.planar.vertGreen1 = data.planar.gv >> 2;
        block.planar.vertGreen2 = data.planar.gv;
        block.planar.vertBlue = data.planar.bv;

        block.planar.diff = 1;

        // create valid RG differentials and an invalid B differential to trigger planar mode
        block.planar.detect1 = 0;
        block.planar.detect2 = 0;
        block.planar.detect3 = 0;
        block.planar.detect4 = 0;

        // @@ Clean this up.
        int dr = block.differential.dred2;
        int dg = block.differential.dgreen2;
        int db = block.differential.dblue2;
        if (dr >= 4) dr -= 8;
        if (dg >= 4) dg -= 8;
        if (db >= 4) db -= 8;
        int r = block.differential.red1 + dr;
        int g = block.differential.green1 + dg;
        int b = block.differential.blue1 + db;

        if (r < 0 || r > 31) {
            block.planar.detect1 = 1;
        }
        if (g < 0 || g > 31) {
            block.planar.detect2 = 1;
        }
        if (b >= 4) {
            block.planar.detect3 = 7;
            block.planar.detect4 = 0;
        }
        else {
            block.planar.detect3 = 0;
            block.planar.detect4 = 1;
        }
    }
    else {
        block.individual.selectors = 0;
        for (int i = 0; i < 16; i++) {
            uint selector = data.selector[i];
            selector = etc_selector_scramble[selector];
            block.individual.selectors |= (selector >> 1) << (i ^ 8);
            block.individual.selectors |= (selector & 1) << ((16 + i) ^ 8);
        }

        if (swap_colors) {
            block.individual.selectors ^= 0x0000FFFF;
        }
    }

    // @@ output_block is big endian, byte swap:
    *output_block = block;
}

static void unpack_etc2_block(const BlockETC * input_block, ETC_Data * data) {

    // @@ input_block is big endian, byte swap first:
    BlockETC block = *input_block;

    // Assume ETC1 for now.
    data->mode = ETC_Data::Mode_ETC1;

    // These are the same for individual and differential blocks.
    data->etc.diff = block.individual.diff != 0;
    data->etc.flip = block.individual.flip != 0;
    data->etc.table0 = block.individual.cw1;
    data->etc.table1 = block.individual.cw2;

    // Decode colors.
    if (data->etc.diff) {
        data->etc.color0 = U16((block.differential.red1 << 10) | (block.differential.green1 << 5) | block.differential.blue1);
        data->etc.color1 = U16((block.differential.dred2 << 6) | (block.differential.dgreen2 << 3) | block.differential.dblue2);

        // @@ Clean this up.
        int dr = block.differential.dred2;
        int dg = block.differential.dgreen2;
        int db = block.differential.dblue2;
        if (dr >= 4) dr -= 8;
        if (dg >= 4) dg -= 8;
        if (db >= 4) db -= 8;
        int r = block.differential.red1 + dr;
        int g = block.differential.green1 + dg;
        int b = block.differential.blue1 + db;

        // Detect ETC2 modes (invalid combinations).
        if (r < 0 || r > 31) {
            data->mode = ETC_Data::Mode_T;
        }
        else if (g < 0 || g > 31) {
            data->mode = ETC_Data::Mode_H;
        }
        else if (b < 0 || b > 31) {
            data->mode = ETC_Data::Mode_Planar;
        }
    }
    else {
        data->etc.color0 = U16((block.individual.red1 << 8) | (block.individual.green1 << 4) | block.individual.blue1);
        data->etc.color1 = U16((block.individual.red2 << 8) | (block.individual.green2 << 4) | block.individual.blue2);
    }

    if (data->mode == ETC_Data::Mode_T) {
        uint16 r0 = U16((block.t.red1a << 2) | block.t.red1b);
        uint16 g0 = U16(block.t.green1);
        uint16 b0 = U16(block.t.blue1);
        data->t.color0 = U16(r0 << 8) | U16(g0 << 4) | b0;

        uint16 r1 = U16(block.t.red2);
        uint16 g1 = U16(block.t.green2);
        uint16 b1 = U16(block.t.blue2);
        data->t.color1 = U16(r1 << 8) | U16(g1 << 4) | b1;

        data->t.table = U8((block.t.da << 1) | block.t.db);
    }
    else if (data->mode == ETC_Data::Mode_H) {
        uint16 r0 = U16(block.h.red1);
        uint16 g0 = U16((block.h.green1a << 1) | block.h.green1b);
        uint16 b0 = U16((block.h.blue1a << 3) | (block.h.blue1b << 1) | block.h.blue1c);
        data->h.color0 = U16(r0 << 8) | U16(g0 << 4) | b0;

        uint16 r1 = U16(block.h.red2);
        uint16 g1 = U16((block.h.green2a << 1) | block.h.green2b);
        uint16 b1 = U16(block.h.blue2);
        data->h.color1 = U16(r1 << 8) | U16(g1 << 4) | b1;

        data->h.table = U8((block.h.da << 2) | (block.h.db << 1));

        if (data->h.color0 >= data->h.color1) {
            data->h.table++;
        }
    }

    if (data->mode == ETC_Data::Mode_Planar) {
        data->planar.ro = U8(block.planar.originRed);
        data->planar.go = U8((block.planar.originGreen1 << 6) + block.planar.originGreen2);
        data->planar.bo = U8((block.planar.originBlue1 << 5) + (block.planar.originBlue2 << 3) + (block.planar.originBlue3 << 1) + block.planar.originBlue4);

        data->planar.rh = U8((block.planar.horizRed1 << 1) + block.planar.horizRed2);
        data->planar.gh = U8(block.planar.horizGreen);
        data->planar.bh = U8((block.planar.horizBlue1 << 5) + block.planar.horizBlue2);

        data->planar.rv = U8((block.planar.vertRed1 << 3) + block.planar.vertRed2);
        data->planar.gv = U8((block.planar.vertGreen1 << 2) + block.planar.vertGreen2);
        data->planar.bv = U8(block.planar.vertBlue);
    }
    else {
        // Note, selectors are arranged in columns, keep that order.
        unsigned char * selectors = (uint8 *)&block.individual.selectors;
        for (int i = 0; i < 16; i++) {
            int byte_msb = (1 - (i / 8));
            int byte_lsb = (3 - (i / 8));
            int shift = (i & 7);

            uint msb = (selectors[byte_msb] >> shift) & 1;
            uint lsb = (selectors[byte_lsb] >> shift) & 1;

            uint index = (msb << 1) | lsb;

            if (data->mode == ETC_Data::Mode_ETC1) {
                data->selector[i] = etc_selector_unscramble[index];
            }
            else {
                // No scrambling in T & H modes.
                data->selector[i] = index;
            }
        }
    }
}

static void pack_eac_block(const EAC_Data & data, BlockEAC * output_block) {

    output_block->base = data.alpha;
    output_block->table = data.table_index;
    output_block->multiplier = data.multiplier;
    
    uint64 selector_bits = 0;
    for (uint i = 0; i < 16; i++) {
        uint shift = 45 - (3 * i);
        selector_bits |= uint64(data.selector[i]) << shift;
    }
    
    output_block->selectors0 = selector_bits >> 40;
    output_block->selectors1 = selector_bits >> 32;
    output_block->selectors2 = selector_bits >> 24;
    output_block->selectors3 = selector_bits >> 16;
    output_block->selectors4 = selector_bits >> 8;
    output_block->selectors5 = selector_bits >> 0;
}

static void unpack_eac_block(const BlockEAC * input_block, EAC_Data * data) {
    
    data->alpha = input_block->base;
    data->table_index = input_block->table;
    data->multiplier = input_block->multiplier;
    
    uint64 selector_bits = 0;
    selector_bits |= uint64(input_block->selectors0) << 40;
    selector_bits |= uint64(input_block->selectors1) << 32;
    selector_bits |= uint64(input_block->selectors2) << 24;
    selector_bits |= uint64(input_block->selectors3) << 16;
    selector_bits |= uint64(input_block->selectors4) << 8;
    selector_bits |= uint64(input_block->selectors5) << 0;
    for (uint i = 0; i < 16; i++) {
        uint shift = 45 - (3 * i);
        data->selector[i] = (selector_bits >> shift) & 0x7;
    }
}



// This assumes nin > nout-nin
inline int bitexpand(uint32 bits, uint nin, uint nout) {
    assert(nout > nin);
    //assert(nout - nin > nin);
    return (bits << uint(nout - nin)) | (bits >> uint(2U * nin - nout));
}

// Integer color unpacking for decompressor.
static void unpack_color_444(uint32 packed_color, int * r, int * g, int * b) {
    int r4 = (packed_color >> 8) & 0xF;
    int g4 = (packed_color >> 4) & 0xF;
    int b4 = packed_color & 0xF;
    *r = r4 << 4 | r4;  // bitexpand(r4, 4, 8);
    *g = g4 << 4 | g4;  // bitexpand(g4, 4, 8);
    *b = b4 << 4 | b4;  // bitexpand(b4, 4, 8);
}

static Vector3 unpack_color_444(uint32 packed_color) {
    int r, g, b;
    unpack_color_444(packed_color, &r, &g, &b);
    return Vector3(float(r), float(g), float(b)) * 1.0f / 255.0f;
}

static void unpack_color_555(uint32 packed_color, int * r, int * g, int * b) {
    int r5 = (packed_color >> 10) & 0x1F;
    int g5 = (packed_color >> 5) & 0x1F;
    int b5 = packed_color & 0x1F;
    *r = (r5 << 3) | (r5 >> 2); // bitexpand(r5, 5, 8);
    *g = (g5 << 3) | (g5 >> 2); // bitexpand(g5, 5, 8);
    *b = (b5 << 3) | (b5 >> 2); // bitexpand(b5, 5, 8);
}

static Vector3 unpack_color_555(uint32 packed_color) {
    int r, g, b;
    unpack_color_555(packed_color, &r, &g, &b);
    return Vector3(float(r), float(g), float(b)) * 1.0f / 255.0f;
}

// Returns signed r,g,b without bit expansion.
static void unpack_delta_333(uint32 packed_delta, int * r, int * g, int * b) {
    *r = (packed_delta >> 6) & 7;
    *g = (packed_delta >> 3) & 7;
    *b = packed_delta & 7;
    if (*r >= 4) *r -= 8;
    if (*g >= 4) *g -= 8;
    if (*b >= 4) *b -= 8;
}

static bool unpack_color_555(uint32 packed_color, uint32 packed_delta, int * r, int * g, int * b) {
    int dc_r, dc_g, dc_b;
    unpack_delta_333(packed_delta, &dc_r, &dc_g, &dc_b);

    int r5 = int((packed_color >> 10U) & 0x1F) + dc_r;
    int g5 = int((packed_color >> 5U) & 0x1F) + dc_g;
    int b5 = int(packed_color & 0x1F) + dc_b;

    bool success = true;
    if (static_cast<uint>(r5 | g5 | b5) > 31U)
    {
       success = false;
       r5 = clamp(r5, 0, 31);
       g5 = clamp(g5, 0, 31);
       b5 = clamp(b5, 0, 31);
    }

    *r = (r5 << 3) | (r5 >> 2); // bitexpand(r5, 5, 8);
    *g = (g5 << 3) | (g5 >> 2); // bitexpand(g5, 5, 8);
    *b = (b5 << 3) | (b5 >> 2); // bitexpand(b5, 5, 8);

    return success;
}

static Vector3 unpack_color_555(uint32 packed_color, uint32 packed_delta) {
    int r, g, b;
    bool success = unpack_color_555(packed_color, packed_delta, &r, &g, &b);
    assert(success);
    return Vector3(float(r), float(g), float(b)) * 1.0f / 255.0f;
}


static void unpack_color_676(uint32 packed_color, int * r, int * g, int * b) {
    int r6 = (packed_color >> 13) & 0x3F;
    int g7 = (packed_color >> 6) & 0x7F;
    int b6 = packed_color & 0x3F;

    *r = bitexpand(r6, 6, 8);   // r << 2 | r >> 4
    *g = bitexpand(g7, 7, 8);   // g << 1 | g >> 6
    *b = bitexpand(b6, 6, 8);   // b << 2 | b >> 4
}


static uint32 pack_color_444(Vector3 color) {

    // Truncate.
    uint r = U32(ftoi_trunc(clamp(color.x * 15.0f, 0.0f, 15.0f)));
    uint g = U32(ftoi_trunc(clamp(color.y * 15.0f, 0.0f, 15.0f)));
    uint b = U32(ftoi_trunc(clamp(color.z * 15.0f, 0.0f, 15.0f)));

    // Round exactly according to 444 bit-expansion.
    r += (color.x > midpoints4[r]);
    g += (color.y > midpoints4[g]);
    b += (color.z > midpoints4[b]);

    return (r << 8) | (g << 4) | b;
}

static uint32 pack_color_555(Vector3 color) {

    // Truncate.
    uint r = U32(ftoi_trunc(clamp(color.x * 31.0f, 0.0f, 31.0f)));
    uint g = U32(ftoi_trunc(clamp(color.y * 31.0f, 0.0f, 31.0f)));
    uint b = U32(ftoi_trunc(clamp(color.z * 31.0f, 0.0f, 31.0f)));

    // Round exactly according to 555 bit-expansion.
    r += (color.x > midpoints5[r]);
    g += (color.y > midpoints5[g]);
    b += (color.z > midpoints5[b]);

    return (r << 10) | (g << 5) | b;
}

static uint32 pack_delta_333(Vector3 delta) {
    // @@ Accurate rounding of signed 3-bit components.

    int r = ftoi_round(clamp(delta.x * 31.0f, -4.0f, 3.0f));
    int g = ftoi_round(clamp(delta.y * 31.0f, -4.0f, 3.0f));
    int b = ftoi_round(clamp(delta.z * 31.0f, -4.0f, 3.0f));

    //r += (delta.x > delta_midpoints3[r]);
    //g += (delta.y > delta_midpoints3[g]);
    //b += (delta.z > delta_midpoints3[b]);

    if (r < 0) r += 8;
    if (g < 0) g += 8;
    if (b < 0) b += 8;
    return static_cast<uint16>(b | (g << 3) | (r << 6));
}

static uint8 pack_float_6(float f) {

    // Truncate.
    uint u = U32(ftoi_trunc(clamp(f * 63.0f, 0.0f, 63.0f)));

    // Round exactly according to 6 bit-expansion.
    //u += (f > midpoints6[u]);

    float midpoint = 0.5f * (bitexpand(u, 6, 8) + bitexpand(min(u + 1, 63U), 6, 8));    // @@ Precompute.
    u += (f > midpoint);

    return U8(u);
}

static uint8 pack_float_7(float f) {

    // Truncate.
    uint u = U32(ftoi_trunc(clamp(f * 127.0f, 0.0f, 127.0f)));

    // Round exactly according to 6 bit-expansion.
    //u += (f > midpoints7[u]);

    float midpoint = 0.5f * (bitexpand(u, 7, 8) + bitexpand(min(u + 1, 127U), 7, 8));   // @@ Precompute.
    u += (f > midpoint);

    return U8(u);
}

static uint8 pack_float_6(float f, bool round_dir) {
    uint u = U32(ftoi_trunc(clamp(f * 63.0f + round_dir, 0.0f, 63.0f)));
    return U8(u);
}

static uint8 pack_float_7(float f, bool round_dir) {
    uint u = U32(ftoi_trunc(clamp(f * 127.0f + round_dir, 0.0f, 127.0f)));
    return U8(u);
}




Vector3 get_partition_color_average(const Vector4 input_colors[16], const float input_weights[16], bool flip, int partition) {
    Vector3 sum_c(0);
    float sum_w = 0;

    if (flip) {
        // Horizontal partition.
        int offset = partition ? 8 : 0;

        for (int i = 0; i < 8; i++) {
            sum_c += input_colors[i+offset].xyz() * input_weights[i+offset];
            sum_w += input_weights[i+offset];
        }
    }
    else {
        // Vertical partition.
        int offset = partition ? 2 : 0;

        for (int i = 0; i < 4; i++) {
            sum_c += input_colors[i+offset].xyz() * input_weights[i+offset];
            sum_w += input_weights[i+offset];

            sum_c += input_colors[i+offset+1].xyz() * input_weights[i+offset+1];
            sum_w += input_weights[i+offset+1];

            offset += 2;
        }
    }
    if (sum_w == 0) {
        sum_w = 1;
    }

    return sum_c * 1.0f / sum_w;
}

// Approximate partition color using average.
Vector3 base_color_average(const Vector3 colors[8]) {
    Vector3 sum_c(0);

    for (uint i = 0; i < 8; i++) {
        sum_c += colors[i];
    }

    return sum_c * 1.0f / 8.0f;
}
Vector3 base_color_average(const Vector3 colors[8], const float weights[8]) {
    Vector3 sum_c(0);
    float sum_w = 0;

    for (uint i = 0; i < 8; i++) {
        sum_c += colors[i] * weights[i];
        sum_w += weights[i];
    }

    return sum_c * 1.0f / sum_w;
}

#if 0
// Compute base color using least squares.
Vector3 base_color_least_squares(const Vector3 colors[8], int table_index, int indices[8]) {

    // Compute dot(C, I) and dot(I, I)
    Vector3 CI(0);
    float II = 0;

    for (int i = 0; i < 8; i++) {
        Vector3 C = colors[i];
        float I = etc_intensity_modifiers[table_index][indices[i]];
        CI += C * I;
        II += I * I;
    }

    return CI / II;
}

// @@ Do weighted least squares!
Vector3 base_color_least_squares(const Vector3 colors[8], const float weights[8], int table_index, int indices[8]) {

    // Compute dot(C, I) and dot(I, I)
    Vector3 CI(0);
    float II = 0;

    for (int i = 0; i < 8; i++) {
        Vector3 C = colors[i];
        float w = weights[i];
        float I = etc_intensity_modifiers[table_index][indices[i]];
        CI += C * I * w;
        II += I * I;
    }

    return CI / II;
}

// Is this any faster than the above?
Vector3 base_color_least_squares(const Vector3 colors[8], int table_index, int c0, int c1, int c2) {

    // Compute dot(C, I) and dot(I, I)
    Vector3 CI(0);

    float I0 = etc_intensity_modifiers[table_index][0];
    float I1 = etc_intensity_modifiers[table_index][1];
    float I2 = etc_intensity_modifiers[table_index][2];
    float I3 = etc_intensity_modifiers[table_index][3];

    float II = 0;
    II += c0 * I0 * I0;
    II += c1 * I1 * I1;
    II += c2 * I2 * I2;
    II += (8-c0-c1-c2) * I3 * I3;

    int i = 0;
    for (; i < c0; i++)         CI += colors[i] * I0;
    for (; i < c0+c1; i++)      CI += colors[i] * I1;
    for (; i < c0+c1+c2; i++)   CI += colors[i] * I2;
    for (; i < 8; i++)          CI += colors[i] * I3;

    return CI / II;
}

static void selectors_for_clusters(int c0, int c1, int c2, int selector[8]) {
    int i = 0;
    for (; i < c0; i++)         selector[i] = 0;
    for (; i < c0+c1; i++)      selector[i] = 1;
    for (; i < c0+c1+c2; i++)   selector[i] = 2;
    for (; i < 8; i++)          selector[i] = 3;
}

static int cluster_count(int count = 8) {
    int total = 0;

    for (uint c0 = 0; c0 <= count; c0++) {
        for (uint c1 = 0; c1 <= count-c0; c1++) {
            for (uint c2 = 0; c2 <= count-c0-c1; c2++) {
                total++;
            }
        }
    }

    // total is the number of possible cluster combinations.
    return total;
}

// Does each partition have its own table index? Or is it shared for both?


void test_all_total_orders(const Vector4 colors[8], const float weights[8], int table_index) {

    // @@ compute average luminance of each partition.


    // @@ sort colors by the luminance differences respect to partition average.

    // @@ compute luminance range, pick table index based on that. Try nearest indices also?

    // For each cluster combination:
/*
    for (uint c0 = 0; c0 <= count; c0++) {
        for (uint c1 = 0; c1 <= count-c0; c1++) {
            for (uint c2 = 0; c2 <= count-c0-c1; c2++) {

                // compute selectors.
                int selector[8];
                selectors_for_clusters(c0, c1, c2, selector);

                // compute base colors that minimize error in each partition.

                // determine error for these quantized base colors. Record best cluster combination.

            }
        }
    }
*/
}

void test_all_total_orders(const Vector4 input_colors[16], const float input_weights[16], uint count, bool flip, int table_index) {

    // Slow method is to test both flip modes.
    //test_all_total_orders(input_colors, input_weights, /*flip=*/false, int table_index);
    //test_all_total_orders(input_colors, input_weights, /*flip=*/true, int table_index);
}


// @@ How do compute the error for a given base color?

// Compute indices using range fitting / quantization of input colors?

// Compute indices using range fitting.

void test_all_clusters() {
    int count = 8; // Could be smaller.

    for (uint c0 = 0; c0 <= count; c0++) {
        Vector3 x1(0.0f);
        float w1 = 0.0f;

        for (uint c1 = 0; c1 <= count-c0; c1++) {
            Vector3 x2(0.0f);
            float w2 = 0.0f;

            for (uint c2 = 0; c2 <= count-c0-c1; c2++) {
            }
        }
    }

}

#endif








static Color32 saturate_color(int R, int G, int B) {
    Color32 c;
    c.r = U8(clamp(R, 0, 255));
    c.g = U8(clamp(G, 0, 255));
    c.b = U8(clamp(B, 0, 255));
    c.a = 255;
    return c;
}

static void get_diff_subblock_palette(uint16 packed_color, uint table_idx, Color32 palette[4]) {
    assert(table_idx < 8);

    const int * intensity_table = etc_intensity_modifiers[table_idx];

    int r, g, b;
    unpack_color_555(packed_color, &r, &g, &b);

    for (int i = 0; i < 4; i++) {
        const int y = intensity_table[i];
        palette[i] = saturate_color(r + y, g + y, b + y);
    }
}

static bool get_diff_subblock_palette(uint16 packed_color, uint16 packed_delta, uint table_idx, Color32 palette[4]) {
    assert(table_idx < 8);

    const int * intensity_table = etc_intensity_modifiers[table_idx];

    int r, g, b;
    bool success = unpack_color_555(packed_color, packed_delta, &r, &g, &b);

    for (int i = 0; i < 4; i++) {
        const int y = intensity_table[i];
        palette[i] = saturate_color(r + y, g + y, b + y);
    }

    return success;
}

static void get_abs_subblock_palette(uint16 packed_color, uint table_idx, Color32 palette[4]) {
    assert(table_idx < 8);

    const int * intensity_table = etc_intensity_modifiers[table_idx];

    int r, g, b;
    unpack_color_444(packed_color, &r, &g, &b);

    for (int i = 0; i < 4; i++) {
        const int y = intensity_table[i];
        palette[i] = saturate_color(r + y, g + y, b + y);
    }
}

static int get_selector(const ETC_Data & data, int x, int y) {
    // Note selectors are arranged in column order.
    return data.selector[x*4+y];
}
static int get_partition(const ETC_Data & data, int x, int y) {
    assert(data.mode == ETC_Data::Mode_ETC1);
    return data.etc.flip ? y > 1 : x > 1;
}

static void decode_etc1(const ETC_Data & data, Vector4 colors[16]) {
    assert(data.mode == ETC_Data::Mode_ETC1);

    Color32 palette[2][4];

    if (data.etc.diff) {
        // Decode colors in 555+333 mode.
        get_diff_subblock_palette(data.etc.color0, data.etc.table0, palette[0]);
        get_diff_subblock_palette(data.etc.color0, data.etc.color1, data.etc.table1, palette[1]);
    }
    else {
        // Decode colors in 444,444 mode.
        get_abs_subblock_palette(data.etc.color0, data.etc.table0, palette[0]);
        get_abs_subblock_palette(data.etc.color1, data.etc.table1, palette[1]);
    }

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            colors[y*4+x] = toVector4(palette[get_partition(data, x, y)][get_selector(data, x, y)]);
        }
    }
}

static void decode_etc2_t(const ETC_Data & data, Vector4 output_colors[16]) {
    assert(data.mode == ETC_Data::Mode_T);

    int r, g, b;
    Color32 palette[4];

    int d = etc_th_distances[data.t.table];

    unpack_color_444(data.t.color0, &r, &g, &b);
    palette[0] = saturate_color(r, g, b);

    unpack_color_444(data.t.color1, &r, &g, &b);
    palette[1] = saturate_color(r + d, g + d, b + d);
    palette[2] = saturate_color(r, g, b);
    palette[3] = saturate_color(r - d, g - d, b - d);

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            output_colors[y*4+x] = toVector4(palette[get_selector(data, x, y)]);
        }
    }
}

static void decode_etc2_h(const ETC_Data & data, Vector4 output_colors[16]) {
    assert(data.mode == ETC_Data::Mode_H);

    int r, g, b;
    Color32 palette[4];

    int d = etc_th_distances[data.t.table];

    unpack_color_444(data.t.color0, &r, &g, &b);
    palette[0] = saturate_color(r + d, g + d, b + d);
    palette[1] = saturate_color(r - d, g - d, b - d);

    unpack_color_444(data.t.color1, &r, &g, &b);
    palette[2] = saturate_color(r + d, g + d, b + d);
    palette[3] = saturate_color(r - d, g - d, b - d);

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            output_colors[y*4+x] = toVector4(palette[get_selector(data, x, y)]);
        }
    }
}

static void decode_etc2_planar(const ETC_Data & data, Vector4 output_colors[16]) {
    assert(data.mode == ETC_Data::Mode_Planar);

    int ro, go, bo; // origin color
    int rh, gh, bh; // horizontal color
    int rv, gv, bv; // vertical color

    // Unpack from 676
    ro = bitexpand(data.planar.ro, 6, 8);   // r << 2 | r >> 4
    go = bitexpand(data.planar.go, 7, 8);   // g << 1 | g >> 6
    bo = bitexpand(data.planar.bo, 6, 8);

    rh = bitexpand(data.planar.rh, 6, 8);
    gh = bitexpand(data.planar.gh, 7, 8);
    bh = bitexpand(data.planar.bh, 6, 8);

    rv = bitexpand(data.planar.rv, 6, 8);
    gv = bitexpand(data.planar.gv, 7, 8);
    bv = bitexpand(data.planar.bv, 6, 8);


    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            int r = (4 * ro + x * (rh - ro) + y * (rv - ro) + 2) >> 2;
            int g = (4 * go + x * (gh - go) + y * (gv - go) + 2) >> 2;
            int b = (4 * bo + x * (bh - bo) + y * (bv - bo) + 2) >> 2;

            int idx = 4 * y + x;
            output_colors[idx].x = saturate(float(r) / 255.0f);
            output_colors[idx].y = saturate(float(g) / 255.0f);
            output_colors[idx].z = saturate(float(b) / 255.0f);
            output_colors[idx].w = 1;
        }
    }
}

static void decode_etc2(const ETC_Data & data, Vector4 colors[16]) {

    if (data.mode == ETC_Data::Mode_ETC1) {
        decode_etc1(data, colors);
    }
    else if (data.mode == ETC_Data::Mode_T) {
        decode_etc2_t(data, colors);
    }
    else if (data.mode == ETC_Data::Mode_H) {
        decode_etc2_h(data, colors);
    }
    else /*if (data.mode == ETC_Data::Mode_Planar)*/ {
        decode_etc2_planar(data, colors);
    }
}


static float get_alpha11(int base, int table, int mul, int index) {
	int elevenbase = base*8+4;
    int tabVal = eac_intensity_modifiers[table][index];
	int elevenTabVal = tabVal*8;
 
	if(mul!=0) elevenTabVal*=mul;
	else elevenTabVal/=8;
  
	//calculate sum
	int elevenbits = elevenbase+elevenTabVal;
 
	//clamp..
	if(elevenbits>=256*8) elevenbits=256*8-1;
	else if(elevenbits<0) elevenbits=0;
	//elevenbits now contains the 11 bit alpha value as defined in the spec.
 
	//extend to 16 bits before returning, since we don't have any good 11-bit file formats.
	uint16 sixteenbits = (elevenbits<<5)+(elevenbits>>6);
 
	return float(sixteenbits) / 65535.0f;
}

static float get_alpha8(int base, int table, int mul, int index) {
    int value = clamp(base + eac_intensity_modifiers[table][index] * mul, 0, 255);
    return value / 255.0f;
}




static void decode_eac_8(const EAC_Data & data, Vector4 output_colors[16], int output_channel = 3) {
    for (int i = 0; i < 16; i++) {
        int s = data.selector[4*(i%4) + i/4];
        output_colors[i].component[output_channel] = get_alpha8(data.alpha, data.table_index, data.multiplier, s);
    }
}

static void decode_eac_11(const EAC_Data & data, Vector4 output_colors[16], int output_channel = 0) {
    for (int i = 0; i < 16; i++) {
        int s = data.selector[4*(i%4) + i/4];
        output_colors[i].component[output_channel] = get_alpha11(data.alpha, data.table_index, data.multiplier, s);
    }
}




static float evaluate_mse(const Vector3 & p, const Vector3 & c, const Vector3 & w) {
    Vector3 d = (p - c) * w;
    return dot(d, d);
}

static float evaluate_rgb_mse(const Vector4 input_colors[16], const float input_weights[16], const ETC_Options & options, const ETC_Data & data) {
    // Decode data and compare?
    Vector4 colors[16];
    decode_etc2(data, colors);

    float error = 0;
    for (int i = 0; i < 16; i++) {
        error += input_weights[i] * evaluate_mse(input_colors[i].xyz(), colors[i].xyz(), options.color_weights);
    }
    return error;
}


static int select_table_index(const Vector3 & base_color, const Vector4 input_colors[16], const float input_weights[16], bool flip, int partition) {

    //float min_lum_delta = NV_FLOAT_MAX;
    float max_lum_delta = -NV_FLOAT_MAX;

    int xb = partition ? 2 : 0;
    int xe = partition ? 4 : 2;

    for (int y = 0; y < 4; y++) {
        for (int x = xb; x < xe; x++) {
            int idx = flip ? x*4 + y : y*4 + x;
            float lum_delta = dot(base_color, Vector3(1.0f/3)) - dot(input_colors[idx].xyz(), Vector3(1.0f/3));
            //min_lum_delta = min(min_lum_delta, lum_delta);
            max_lum_delta = max(max_lum_delta, fabsf(lum_delta));
        }
    }

    int best_range = -1;
    float best_error = NV_FLOAT_MAX;
    for (int i = 0; i < 8; i++) {
        float error = fabsf(etc_intensity_range[i] - 255 * max_lum_delta);
        if (error < best_error) {
            best_error = error;
            best_range = i;
        }
    }

    return best_range;
}

static float update_selectors(const Vector4 input_colors[16], const float input_weights[16], ETC_Data & data, const ETC_Options & options) {

    Color32 palette[2][4];

    if (data.etc.diff) {
        // Decode colors in 555+333 mode.
        get_diff_subblock_palette(data.etc.color0, data.etc.table0, palette[0]);
        get_diff_subblock_palette(data.etc.color0, data.etc.color1, data.etc.table1, palette[1]);
    }
    else {
        // Decode colors in 444,444 mode.
        get_abs_subblock_palette(data.etc.color0, data.etc.table0, palette[0]);
        get_abs_subblock_palette(data.etc.color1, data.etc.table1, palette[1]);
    }

    float total_error = 0;

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            int i = y*4 + x;

            float best_error = NV_FLOAT_MAX;
            int best_p = 0;

            for (int p = 0; p < 4; p++) {
                float error = evaluate_mse(toVector3(palette[get_partition(data, x, y)][p]), input_colors[i].xyz(), options.color_weights);
                if (error < best_error) {
                    best_error = error;
                    best_p = p;
                }
            }

            int s = x*4 + y;
            data.selector[s] = U8(best_p);

            total_error += best_error * input_weights[i];
        }
    }

    return total_error;
}


static void partition_input_block(const Vector4 input_colors[16], const float input_weights[16], bool flip, int partition, Vector3 output_colors[8], float output_weights[8]) {

    const int xb = partition ? 2 : 0;
    const int xe = partition ? 4 : 2;

    for (int y = 0, i = 0; y < 4; y++) {
        for (int x = xb; x < xe; x++, i++) {
            int idx = flip ? x*4 + y : y*4 + x;

            output_colors[i] = input_colors[idx].xyz();
            output_weights[i] = input_weights[idx];
        }
    }
}


struct ETC_SubBlock {
    Vector3 color;
    bool delta;
    int table;
    int indices[8];
};

static float evaluate_rgb_mse(const Vector3 colors[8], const float weights[8], const ETC_Options & options, ETC_SubBlock * sub_block) {

    // Evaluate sub block palette.
    Vector3 palette[4];
    palette[0] = sub_block->color + Vector3(etc_intensity_modifiers[sub_block->table][0] / 255.0f);
    palette[1] = sub_block->color + Vector3(etc_intensity_modifiers[sub_block->table][1] / 255.0f);
    palette[2] = sub_block->color + Vector3(etc_intensity_modifiers[sub_block->table][2] / 255.0f);
    palette[3] = sub_block->color + Vector3(etc_intensity_modifiers[sub_block->table][3] / 255.0f);

    float mse = 0;
    for (int i = 0; i < 8; i++) {
        mse += evaluate_mse(colors[i], palette[sub_block->indices[i]], options.color_weights) * weights[i];
    }

    return mse;
}

static void optimize_base_color(const Vector3 colors[8], const float weights[8], ETC_SubBlock * sub_block) {

    // @@ For a given index selection, find color that minimizes the error. RGB components are independent.

    float D_sum = 0;
    float R_sum = 0;
    float G_sum = 0;
    float B_sum = 0;
    float W_sum = 0;

    for (int i = 0; i < 8; i++) {
        float Di = etc_intensity_modifiers[sub_block->table][sub_block->indices[i]] / 255.0f;  // @@ precompute?

        D_sum += Di * weights[i];
        R_sum += colors[i].x * weights[i];
        G_sum += colors[i].y * weights[i];
        B_sum += colors[i].z * weights[i];
        W_sum += weights[i];
    }

    sub_block->color.x = (R_sum - D_sum) / W_sum;
    sub_block->color.y = (R_sum - D_sum) / W_sum;
    sub_block->color.z = (R_sum - D_sum) / W_sum;

    // @@ Estimate error (without quantization)



    // @@ Repeat for all tables?

    // @@ Given a new center, compute new indices, then update center?

}



static int reduce_colors(Vector3 * colors, float * weights, int count) {

    int n = 0;

    for (int i = 0; i < count; i++) {

        if (weights[i] == 0.0f) {
            // skip without incrementing n.
            continue;
        }

        colors[n] = colors[i];
        weights[n] = weights[i];

        // find color[j] that matches color[i]
        for (int j = i + 1; j < count; j++) {
            if (colors[i] == colors[j]) {       // @@ Compare within threshold?
                weights[n] += weights[j];
                weights[j] = 0.0f;
            }
        }

        n++;
    }

    return n;
}

// stable sort. in place.
static void sort_colors(Vector3 * colors, float * weights, int count) {
    assert(count <= 8);

    // build the list of values
    //int order[8];
    float lum[8];
    for (int i = 0; i < count; ++i) {
        //order[i] = i;
        lum[i] = colors[i].x + colors[i].y + colors[i].z;
    }

    // stable sort
    for (int i = 0; i < count; ++i) {
        for (int j = i; j > 0 && lum[j] < lum[j - 1]; --j) {
            swap(lum[j], lum[j - 1]);
            //swap(order[j], order[j - 1]);
            swap(colors[j], colors[j - 1]);
        }
    }
}



/*
float optimize_center(float colors[4][10], uniform int p, uniform int table_level)
{
    float best_center = 0;
    for (uniform int q = 0; q < 4; q++)
    {
        best_center += (colors[q][7 + p] - get_etc1_dY(table_level, q)) * colors[q][3];
    }
    best_center /= 8;

    float best_err = 0;
    for (uniform int q = 0; q < 4; q++)
    {
        float dY = get_etc1_dY(table_level, q);
        best_err += sq(clamp(best_center + dY, 0, 255) - colors[q][7 + p]) * colors[q][3];
    }

    for (uniform int branch = 0; branch < 4; branch++)
    {
        float new_center = 0;
        float sum = 0;
        for (uniform int q = 0; q < 4; q++)
        {
            if (branch <= 1 && q <= branch) continue;
            if (branch >= 2 && q >= branch) continue;
            new_center += (colors[q][7 + p] - get_etc1_dY(table_level, q)) * colors[q][3];
            sum += colors[q][3];
        }

        new_center /= sum;

        float err = 0;
        for (uniform int q = 0; q < 4; q++)
        {
            float dY = get_etc1_dY(table_level, q);
            err += sq(clamp(new_center + dY, 0, 255) - colors[q][7 + p]) * colors[q][3];
        }

        if (err < best_err)
        {
            best_err = err;
            best_center = new_center;
        }
    }

    return best_center;
}
*/




static void compress_etc1_test(const Vector4 input_colors[16], const float input_weights[16], const ETC_Options & options, ETC_Solution * result) {

    Vector3 colors[8];
    float weights[8];
    //int xrefs[8];
    ETC_SubBlock sub_block[2];

    bool best_flip = false;
    for (int flip = 0; flip <= 1; flip++) {

        partition_input_block(input_colors, input_weights, !!flip, /*partition=*/0, colors, weights);

        int count = reduce_colors(colors, weights, 8);

        //sort_colors(colors, weights);

        // @@ sort colors along luminance axis.

        //sub_block[0].color

        partition_input_block(input_colors, input_weights, !!flip, /*partition=*/1, colors, weights);

    }

    //pack_colors(sub_block[0].color, sub_block[1].color, &result->data);

    result->error = update_selectors(input_colors, input_weights, result->data, options);

}

/*void pack_colors(const Vector3 & color0, const Vector3 & color1, const ETC_Options & options, ETC_Data * data) {

    uint16 abs_c0 = U16(pack_color_444(color0));
    uint16 abs_c1 = U16(pack_color_444(color1));
    Vector3 abs_vc0 = unpack_color_444(abs_c0);
    Vector3 abs_vc1 = unpack_color_444(abs_c1);
    float abs_error = evaluate_mse(color0, abs_vc0, options.color_weights) + evaluate_mse(color1, abs_vc1, options.color_weights);

    uint16 diff_c0 = U16(pack_color_555(color0));
    Vector3 diff_vc0 = unpack_color_555(diff_c0);
    uint16 diff_d1 = U16(pack_delta_333(color1 - diff_vc0));
    Vector3 diff_vc1 = unpack_color_555(diff_c0, diff_d1);
    float diff_error = evaluate_mse(color0, diff_vc0, options.color_weights) + evaluate_mse(color1, diff_vc1, options.color_weights);

    if (diff_error < abs_error) {
        data->etc.color0 = diff_c0;
        data->etc.color1 = diff_c1;
        return diff_error;
    }
    else {
        if (abs_error < best_error) {
            best_error = abs_error;
            best_diff = false;
            best_flip = flip;
            best_c0 = abs_c0;
            best_c1 = abs_c1;
            best_vc0 = abs_vc0;
            best_vc1 = abs_vc1;
        }
    }
}*/

static void compress_etc1_range_fit(const Vector4 input_colors[16], const float input_weights[16], const ETC_Options & options, ETC_Solution * result) {

    float best_error = NV_FLOAT_MAX;
    bool best_diff = false;
    bool best_flip = false;
    uint16 best_c0 = 0;
    uint16 best_c1 = 0;
    Vector3 best_vc0;
    Vector3 best_vc1;

    for (int flip = 0; flip <= 1; flip++) {
        Vector3 color0 = get_partition_color_average(input_colors, input_weights, !!flip, /*partition=*/0);
        Vector3 color1 = get_partition_color_average(input_colors, input_weights, !!flip, /*partition=*/1);

        uint16 abs_c0 = U16(pack_color_444(color0));
        uint16 abs_c1 = U16(pack_color_444(color1));
        Vector3 abs_vc0 = unpack_color_444(abs_c0);
        Vector3 abs_vc1 = unpack_color_444(abs_c1);
        float abs_error = evaluate_mse(color0, abs_vc0, options.color_weights) + evaluate_mse(color1, abs_vc1, options.color_weights);

        uint16 diff_c0 = U16(pack_color_555(color0));
        Vector3 diff_vc0 = unpack_color_555(diff_c0);
        uint16 diff_d1 = U16(pack_delta_333(color1 - diff_vc0));
        Vector3 diff_vc1 = unpack_color_555(diff_c0, diff_d1);
        float diff_error = evaluate_mse(color0, diff_vc0, options.color_weights) + evaluate_mse(color1, diff_vc1, options.color_weights);

        if (diff_error < abs_error) {
            if (diff_error < best_error) {
                best_error = diff_error;
                best_diff = true;
                best_flip = !!flip;
                best_c0 = diff_c0;
                best_c1 = diff_d1;
                best_vc0 = diff_vc0;
                best_vc1 = diff_vc1;
            }
        }
        else {
            if (abs_error < best_error) {
                best_error = abs_error;
                best_diff = false;
                best_flip = !!flip;
                best_c0 = abs_c0;
                best_c1 = abs_c1;
                best_vc0 = abs_vc0;
                best_vc1 = abs_vc1;
            }
        }
    }


    result->data.mode = ETC_Data::Mode_ETC1;
    result->data.etc.flip = best_flip;
    result->data.etc.diff = best_diff;
    result->data.etc.table0 = select_table_index(best_vc0, input_colors, input_weights, best_flip, /*partition=*/0);
    result->data.etc.table1 = select_table_index(best_vc1, input_colors, input_weights, best_flip, /*partition=*/1);
    result->data.etc.color0 = best_c0;
    result->data.etc.color1 = best_c1;

    result->error = update_selectors(input_colors, input_weights, result->data, options);

    result->error = evaluate_rgb_mse(input_colors, input_weights, options, result->data);
}

#if HAVE_RGETC
#include "nvimage/ColorBlock.h"

void compress_etc1_rg(const Vector4 input_colors[16], const float input_weights[16], const ETC_Options & options, ETC_Solution * result) {

    rg_etc1::etc1_pack_params pack_params;
    //pack_params.m_quality = rg_etc1::cLowQuality;
    pack_params.m_quality = rg_etc1::cMediumQuality;  // @@ Select quality based on compression options. 

    ColorBlock rgba;
    for (uint i = 0; i < 16; i++) {
        rgba.color(i) = toColor32(input_colors[i]);
    }
    rgba.swizzle(2, 1, 0, 3);

    BlockETC block;
    rg_etc1::pack_etc1_block((void *)&block, (const uint *)rgba.colors(), pack_params);

    unpack_etc2_block(&block, &result->data);

    result->error = evaluate_rgb_mse(input_colors, input_weights, options, result->data);
}
#endif

static void compress_etc2_planar_solid(const Vector4 input_colors[16], const float input_weights[16], const ETC_Options & options, ETC_Solution * result) {

    Vector3 C(0);
    float W = 0;

    for (int i = 0; i < 16; i++) {
        C += input_colors[i].xyz() * input_weights[i];
        W += input_weights[i];
    }

    C /= W;

    // Convert colors to 676
    result->data.mode = ETC_Data::Mode_Planar;
    result->data.planar.ro = pack_float_6(C.x);
    result->data.planar.go = pack_float_7(C.y);
    result->data.planar.bo = pack_float_6(C.z);

    result->data.planar.rh = result->data.planar.ro;
    result->data.planar.gh = result->data.planar.go;
    result->data.planar.bh = result->data.planar.bo;

    result->data.planar.rv = result->data.planar.ro;
    result->data.planar.gv = result->data.planar.go;
    result->data.planar.bv = result->data.planar.bo;

    // Evaluate error.
    result->error = evaluate_rgb_mse(input_colors, input_weights, options, result->data);
}

// Least squares optimization of planar endpoints.
static void compress_etc2_planar_lsqr(const Vector4 input_colors[16], const float input_weights[16], const ETC_Options & options, ETC_Solution * result) {

    // Isn't this a simple least squares problem?
    // - Yes, but that doesn't take clamping and quantization into account.
    // - Solve the least squares problem, then refine endpoints?

    // This matrix is always the same! But not when using arbitrary weights!
    // This would be faster computing the matrix first, then multiplying by the weight covariance matrix.
    Matrix3 m(0);

    // For every pixel, decoder does:
    // int r = (4 * ro + x * (rh - ro) + y * (rv - ro) + 2) >> 2;

    // R(x,y) = (4 * ro + x * (rh - ro) + y * (rv - ro) + 2) / 4;

    // R(x,y) = ro * (1 - x/4 - y/4) + rh * x/4 + rv * y/4 + 1/2;

    // a = x/4
    // b = y/4
    // c = 1 - a - b
    // R(x,y) = ro * c + rh * a + rv * b + 1/2;

    float A[3 * 16];

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            float w = input_weights[4*y+x];
            //if ((x == 1 || x == 2) && (y == 1 && y == 2)) w *= 0.5;

            float a = float(x) / 4 * w;
            float b = float(y) / 4 * w;
            float c = (1 - a - b) * w;

            int i = y*4 + x;
            A[3 * i + 0] = a;
            A[3 * i + 1] = b;
            A[3 * i + 2] = c;

            /*for (int yy = 0; yy < 4; yy++) {
                for (int xx = 0; xx < 4; xx++) {
                    float ww = input_weights[4*yy+xx];
                    //if ((xx == 1 || xx == 2) && (yy == 1 && yy == 2)) ww *= 0.5;

                    float aa = float(xx) / 4 * ww;
                    float bb = float(yy) / 4 * ww;
                    float cc = (1 - aa - bb) * ww;

                    m(0,0) += a * aa;
                    m(1,0) += b * aa;
                    m(2,0) += c * aa;

                    m(0,1) += a * bb;
                    m(1,1) += b * bb;
                    m(2,1) += c * bb;

                    m(0,2) += a * cc;
                    m(1,2) += b * cc;
                    m(2,2) += c * cc;
                }
            }*/
        }
    }

    // At*A
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            float d = 0;
            for (int i = 0; i < 16; i++) {
                d += A[3*i+x] * A[3*i+y];
            }
            m(x, y) = d;
        }
    }

    // Compute right side:
    Vector3 Ca(0), Cb(0), Cc(0);

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            float a = float(x) / 4;
            float b = float(y) / 4;
            float c = 1 - a - b;

            Vector3 C = input_colors[4*y+x].xyz() - Vector3(0.5f / 255);

            Ca += C * a;
            Cb += C * b;
            Cc += C * c;
        }
    }

    // Now we have 3 equations (one for each color component).

    Vector3 R(Ca.x, Cb.x, Cc.x);
    Vector3 G(Ca.y, Cb.y, Cc.y);
    Vector3 B(Ca.z, Cb.z, Cc.z);

    Vector3 r, g, b;

    if (!solveLU(m, R, &r)) {
        result->error = NV_FLOAT_MAX;
        return;
    }
    if (!solveLU(m, G, &g)) {
        result->error = NV_FLOAT_MAX;
        return;
    }
    if (!solveLU(m, B, &b)) {
        result->error = NV_FLOAT_MAX;
        return;
    }

    Vector3 Ch(r.x, g.x, b.x);
    Vector3 Cv(r.y, g.y, b.y);
    Vector3 Co(r.z, g.z, b.z);

    // Convert colors to 676
    result->data.mode = ETC_Data::Mode_Planar;
    result->data.planar.ro = pack_float_6(Co.x);
    result->data.planar.go = pack_float_7(Co.y);
    result->data.planar.bo = pack_float_6(Co.z);

    result->data.planar.rh = pack_float_6(Ch.x);
    result->data.planar.gh = pack_float_7(Ch.y);
    result->data.planar.bh = pack_float_6(Ch.z);

    result->data.planar.rv = pack_float_6(Cv.x);
    result->data.planar.gv = pack_float_7(Cv.y);
    result->data.planar.bv = pack_float_6(Cv.z);

    // Evaluate error.
    result->error = evaluate_rgb_mse(input_colors, input_weights, options, result->data);

    bool refine_endpoints = true;
    if (refine_endpoints) {
        ETC_Solution best = *result;

        // @@ The per-component errors are not correllated, test 8 combinations 3 times.
        for (int i = 0; i < 8; i++) {
            result->data.planar.ro = pack_float_6(Co.x, (i & 1) != 0);
            result->data.planar.rh = pack_float_6(Ch.x, (i & 2) != 0);
            result->data.planar.rv = pack_float_6(Cv.x, (i & 4) != 0);

            result->error = evaluate_rgb_mse(input_colors, input_weights, options, result->data);
            if (result->error < best.error) {
                best = *result;
            }
        }

        *result = best;

        for (int i = 0; i < 8; i++) {
            result->data.planar.go = pack_float_7(Co.y, (i & 1) != 0);
            result->data.planar.gh = pack_float_7(Ch.y, (i & 2) != 0);
            result->data.planar.gv = pack_float_7(Cv.y, (i & 4) != 0);

            result->error = evaluate_rgb_mse(input_colors, input_weights, options, result->data);
            if (result->error < best.error) {
                best = *result;
            }
        }

        *result = best;

        for (int i = 0; i < 8; i++) {
            result->data.planar.bo = pack_float_6(Co.z, (i & 1) != 0);
            result->data.planar.bh = pack_float_6(Ch.z, (i & 2) != 0);
            result->data.planar.bv = pack_float_6(Cv.z, (i & 4) != 0);

            result->error = evaluate_rgb_mse(input_colors, input_weights, options, result->data);
            if (result->error < best.error) {
                best = *result;
            }
        }

        *result = best;
    }
}


static void process_input_colors(Vector4 input_colors[16]) {
    for (int i = 0; i < 16; i++) {
        input_colors[i] = saturate(input_colors[i]);
        
        // @@ Sanitize input_weights?
        //  - Avoid blocks with all zero weight.
        //  - Normalize weights to avoid too small values?
        //  - Remove NaNs, infinites, etc.
    }
}

static void process_input_alphas(Vector4 input_colors[16], int input_channel) {
    for (int i = 0; i < 16; i++) {
        input_colors[i].component[input_channel] = saturate(input_colors[i].component[input_channel]);
    }
}

static void process_input_weights(float input_weights[16]) {
    float max_weight = 0.0f;
    for (int i = 0; i < 16; i++) {
        max_weight = nv::max(max_weight, input_weights[i]);
    }
    
    const float min_weight = 0.0001f;
    
    if (max_weight <= min_weight) {
        // Handle degenerate case.
        for (int i = 0; i < 16; i++) {
            input_weights[i] = 1;
        }
    }
    else {
        for (int i = 0; i < 16; i++) {
            // Clamp to positive.
            input_weights[i] = nv::max(input_weights[i], 0.0f);

            // Flush to zero.
            if (input_weights[i] < min_weight) input_weights[i] = 0.0f;

            // Normalize.
            input_weights[i] /= max_weight;
        }
    }
}




static float compress_etc_a1(Vector4 input_colors[16], float input_weights[16], const ETC_Options & options, void * output) {
    assert(options.onebit_alpha == true);

    // Classify block.
    bool transparent_block = true;
    bool opaque_block = true;

    for (int i = 0; i < 16; i++) {
        if (input_colors[i].w != 0) transparent_block = false;
        if (input_colors[i].w != 1) opaque_block = false;
    }

    if (transparent_block) {
        // @@ Encode trivial transparent block.
        return 0;
    }

    if (opaque_block) {
        // @@ Encode block with opaque bit set. @@ Isn't this like the standard encoder?
    }
    
    // @@ Encode mixed block.
    nvCheck(false); // Not implemented!
    
    //uint8 color_rgb[16*3];
    //uint8 alpha[16];
    //uint etc_word1, etc_word2;
    //compressBlockDifferentialWithAlpha(bool isTransparent, uint8* img, uint8* alphaimg, uint8* imgdec, 4, 4, 0, 0, &etc_word1, &etc_word2);
    
    return NV_FLOAT_MAX;
}

//uint etc_blocks = 0;
//uint planar_blocks = 0;
//#include "nvthread/Atomic.h"

static float compress_etc(Vector4 input_colors[16], float input_weights[16], const ETC_Options & options, void * output) {
    assert(options.onebit_alpha == false);
    
    ETC_Solution result;
    compress_etc1_range_fit(input_colors, input_weights, options, &result);

    if (options.use_rg_etc) {
#if HAVE_RGETC
        ETC_Solution rg_result;
        compress_etc1_rg(input_colors, input_weights, options, &rg_result);
        if (rg_result.error < result.error) {
            result = rg_result;
        }
#else
        // @@ Print warning?
#endif
    }

    if (options.enable_etc2) {
        if (options.use_planar) {
            ETC_Solution planar_result;
            compress_etc2_planar_lsqr(input_colors, input_weights, options, &planar_result);

            if (planar_result.error < result.error) {
                result = planar_result;
                //nv::atomicIncrement(&planar_blocks);
            }
            else {
                //nv::atomicIncrement(&etc_blocks);
            }
        }
        if (options.use_t_mode) {
            // @@
        }
        if (options.use_h_mode) {
            // @@
        }
    }

    pack_etc2_block(result.data, (BlockETC *)output);

    return result.error;
}


// Range search EAC compressor, slightly modified from ETCLib.
float compress_eac_range_search(Vector4 input_colors[16], float input_weights[16], int input_channel, const EAC_Options & options, void * output) {

    // Find alpha range
    float min_a = 1.0f;
    float max_a = 0.0f;
    for (uint i = 0; i < 16; i++) {
        float a = input_colors[i].component[input_channel];
        min_a = nv::min(min_a, a);
        max_a = nv::max(max_a, a);
    }
    const float range_a = max_a - min_a;

    EAC_Solution best;
    best.error = NV_FLOAT_MAX;

    // try each modifier table entry
    static const uint MODIFIER_TABLE_ENTRYS = 16;
    for (uint t = 0; t < MODIFIER_TABLE_ENTRYS; t++) {
        static const uint MIN_VALUE_SELECTOR = 3;
        static const uint MAX_VALUE_SELECTOR = 7;
    
        const float fTableEntryCenter = (float)-eac_intensity_modifiers[t][MIN_VALUE_SELECTOR];
        const float fTableEntryRange = (float)eac_intensity_modifiers[t][MAX_VALUE_SELECTOR] - eac_intensity_modifiers[t][MIN_VALUE_SELECTOR];
        const float fCenterRatio = fTableEntryCenter / fTableEntryRange;
    
        const int center = ftoi_round(255.0f * (min_a + fCenterRatio * range_a));
        const int min_base = max(0, center - options.search_radius);
        const int max_base = min(center + options.search_radius, 255);
    
        for (int base = min_base; base <= max_base; base++) {
            int range_multiplier = ftoi_round(255 * range_a / fTableEntryRange);
            const int min_multiplier = clamp(range_multiplier - options.search_radius, 1, 15);
            const int max_multiplier = clamp(range_multiplier + options.search_radius, 1, 15);
        
            for (int multiplier = min_multiplier; multiplier <= max_multiplier; multiplier++) {
            
                // find best selector for each pixel
                float block_error = 0;
                uint best_selector[16];
                for (uint i = 0; i < 16; i++) {
                
                    float best_error_a = NV_FLOAT_MAX;
                
                    static const uint ALPHA_SELECTOR_BITS = 3;
                    static const uint ALPHA_SELECTORS = 1 << ALPHA_SELECTOR_BITS;
                    for (uint s = 0; s < ALPHA_SELECTORS; s++) {
                        float alpha;
                        if (options.use_11bit_mode) {
                            alpha = get_alpha11(base, t, multiplier, s);
                        }
                        else {
                            alpha = get_alpha8(base, t, multiplier, s);
                        }
                    
                        float error_a = alpha - input_colors[i].component[input_channel];
                        error_a = error_a * error_a;
                    
                        if (error_a < best_error_a) {
                            best_error_a = error_a;
                            best_selector[i] = s;
                        }
                    }
                
                    block_error += best_error_a * input_weights[i];
                    if (block_error > best.error) {
                        break;  // Don't waste more time.
                    }
                }
            
                if (block_error < best.error) {
                    best.error = block_error;
                
                    best.data.alpha = base;
                    best.data.multiplier = multiplier;
                    best.data.table_index = t;
                    for (uint i = 0; i < 16; i++) {
                        // Flip selectors.
                        best.data.selector[i] = best_selector[4*(i%4) + i/4];
                    }
                }
            }
        }
    }

    pack_eac_block(best.data, (BlockEAC *)output);

    return best.error;
}




// Public API:

void nv::decompress_etc(const void * input_block, Vector4 output_colors[16]) {
#if 1 // Our code
    ETC_Data data;
    unpack_etc2_block((const BlockETC *)input_block, &data);

    decode_etc2(data, output_colors);

#elif HAVE_RGETC && 0

    Color32 colors[16];
    rg_etc1::unpack_etc1_block(input_block, &colors->u);

    for (int i = 0; i < 16; i++) {
        output_colors[i].x = colors[i].b * (1.0f / 255.0f);
        output_colors[i].y = colors[i].g * (1.0f / 255.0f);
        output_colors[i].z = colors[i].r * (1.0f / 255.0f);
        output_colors[i].w = colors[i].a * (1.0f / 255.0f);
    }

#elif HAVE_ETCPACK // Use etcpack for reference.
    const BlockETC * block = (const BlockETC *)input_block;

    uint8 colors[3*16];
    uint part1 = POSH_SwapU32(block->data32[0]);
    uint part2 = POSH_SwapU32(block->data32[1]);
    decompressBlockETC2(part1, part2, colors, 4, 4, 0, 0);

    for (int i = 0; i < 16; i++) {
        output_colors[i].x = colors[3*i+0] * (1.0f / 255.0f);
        output_colors[i].y = colors[3*i+1] * (1.0f / 255.0f);
        output_colors[i].z = colors[3*i+2] * (1.0f / 255.0f);
        output_colors[i].w = 1.0f;
    }
#endif
}

void nv::decompress_eac(const void * input_block, Vector4 output_colors[16], int output_channel) {
    nvCheck(output_channel >= 0 && output_channel < 4);
    
#if 1
    EAC_Data data;
    unpack_eac_block((const BlockEAC *)input_block, &data);
    decode_eac_11(data, output_colors, output_channel);
    
#elif HAVE_ETCPACK
    // Use etcpack for reference.
    formatSigned = 0;

    uint16 alphas[16];
    decompressBlockAlpha16bit((uint8*)input_block, (uint8*)alphas, 4, 4, 0, 0);

    for (int i = 0; i < 16; i++) {
        uint16 alpha = POSH_SwapU16(alphas[i]);
        output_colors[i].component[output_channel] = alpha * (1.0f / 65535.0f);
    }
#endif
}

void nv::decompress_etc_eac(const void * input, Vector4 output_colors[16]) {
#if 1
    BlockETC_EAC * input_block = (BlockETC_EAC *)input;

    ETC_Data etc;
    unpack_etc2_block(&input_block->etc, &etc);
    decode_etc2(etc, output_colors);

    EAC_Data eac;
    unpack_eac_block(&input_block->eac, &eac);
    decode_eac_8(eac, output_colors, 3);

#elif HAVE_ETCPACK
    // Use etcpack for reference.
    uint8 colors[4*16];
    decompressBlockAlpha((uint8*)input_block, colors, 4, 4, 0, 0);

    for (int i = 0; i < 16; i++) {
        output_colors[i].x = colors[4*i+0] * (1.0f / 255.0f);
        output_colors[i].y = colors[4*i+1] * (1.0f / 255.0f);
        output_colors[i].z = colors[4*i+2] * (1.0f / 255.0f);
        output_colors[i].w = colors[4*i+3] * (1.0f / 255.0f);
    }
#endif
}

float nv::compress_etc1(Vector4 input_colors[16], float input_weights[16], const Vector3 & color_weights, void * output) {
    
    process_input_colors(input_colors);
    
    // @@ Use same options for all blocks?
    ETC_Options options;
    options.use_rg_etc = true;
    options.enable_etc2 = false;
    options.use_t_mode = false;
    options.use_h_mode = false;
    options.use_planar = false;
    options.color_weights = color_weights;

    return compress_etc(input_colors, input_weights, options, output);
}

float nv::compress_etc2(Vector4 input_colors[16], float input_weights[16], const Vector3 & color_weights, void * output) {
    
    process_input_colors(input_colors);
    process_input_weights(input_weights);
    
    ETC_Options options;
    options.use_rg_etc = true;
    options.enable_etc2 = true;
    options.use_t_mode = false; // @@ Not implemented.
    options.use_h_mode = false; // @@ Not implemented.
    options.use_planar = true;
    options.color_weights = color_weights;

    return compress_etc(input_colors, input_weights, options, output);
}

float nv::compress_etc2_a1(Vector4 input_colors[16], float input_weights[16], const Vector3 & color_weights, void * output) {
    
    process_input_colors(input_colors);
    process_input_weights(input_weights);
    
    ETC_Options options;
    options.use_rg_etc = true;
    options.enable_etc2 = true;
    options.use_t_mode = false; // @@ Not implemented.
    options.use_h_mode = false; // @@ Not implemented.
    options.use_planar = true;
    options.onebit_alpha = true;
    options.color_weights = color_weights;
    
    return compress_etc_a1(input_colors, input_weights, options, output);
}


float nv::compress_eac(Vector4 input_colors[16], float input_weights[16], int input_channel, int search_radius, bool use_11bit_mode, void * output) {
    nvCheck(input_channel >= 0 && input_channel < 4);
    
    process_input_alphas(input_colors, input_channel);
    process_input_weights(input_weights);
    
    EAC_Options options;
    options.search_radius = search_radius;
    options.use_11bit_mode = use_11bit_mode;
    
    return compress_eac_range_search(input_colors, input_weights, input_channel, options, output);
}

float nv::compress_etc2_eac(Vector4 input_colors[16], float input_weights[16], const Vector3 & color_weights, void * output) {
    BlockETC_EAC * output_block = (BlockETC_EAC *)output;
    float error = compress_etc2(input_colors, input_weights, color_weights, &output_block->etc);
    error += compress_eac(input_colors, input_weights, /*input_channel=*/3, /*search_radius=*/1, /*use_11bit_mode=*/false, &output_block->eac);
    return error;
}
















