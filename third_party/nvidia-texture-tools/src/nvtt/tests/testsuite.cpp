// Copyright (c) 2009-2011 Ignacio Castano <castano@gmail.com>
// Copyright (c) 2007-2009 NVIDIA Corporation -- Ignacio Castano <icastano@nvidia.com>
// 
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

#include <nvtt/nvtt.h>
#include <nvimage/Image.h>
#include <nvimage/ImageIO.h>
#include <nvimage/BlockDXT.h>
#include <nvimage/ColorBlock.h>
#include <nvcore/Ptr.h>
#include <nvcore/Debug.h>
#include <nvcore/StrLib.h>
#include <nvcore/StdStream.h>
#include <nvcore/TextWriter.h>
#include <nvcore/FileSystem.h>
#include <nvcore/Timer.h>

#include <stdlib.h> // free
#include <string.h> // memcpy

#include "../tools/cmdline.h"

using namespace nv;

#define ARRAY_SIZE(a) sizeof(a)/sizeof(a[0])

// Kodak image set
static const char * s_kodakImageSet[] = {
    "kodim01.png",
    "kodim02.png",
    "kodim03.png",
    "kodim04.png",
    "kodim05.png",
    "kodim06.png",
    "kodim07.png",
    "kodim08.png",
    "kodim09.png",
    "kodim10.png",
    "kodim11.png",
    "kodim12.png",
    "kodim13.png",
    "kodim14.png",
    "kodim15.png",
    "kodim16.png",
    "kodim17.png",
    "kodim18.png",
    "kodim19.png",
    "kodim20.png",
    "kodim21.png",
    "kodim22.png",
    "kodim23.png",
    "kodim24.png",
};

// Waterloo image set
static const char * s_waterlooImageSet[] = {
    "clegg.png",
    "frymire.png",
    "lena.png",
    "monarch.png",
    "peppers.png",
    "sail.png",
    "serrano.png",
    "tulips.png",
};

// Epic image set
static const char * s_epicImageSet[] = {
    "Bradley1.png",
    "Gradient.png",
    "MoreRocks.png",
    "Wall.png",
    "Rainbow.png",
    "Text.png",
};

// Farbrausch
static const char * s_farbrauschImageSet[] = {
    "t.2d.pn02.png",
    "t.aircondition.01.png",
    "t.bricks.02.png",
    "t.bricks.05.png",
    "t.concrete.cracked.01.png",
    "t.envi.colored02.png",
    "t.envi.colored03.png",
    "t.font.01.png",
    "t.sewers.01.png",
    "t.train.03.png",
    "t.yello.01.png",
};

// Lugaru
static const char * s_lugaruImageSet[] = {
    "lugaru-blood.png",
    "lugaru-bush.png",
    "lugaru-cursor.png",
    "lugaru-hawk.png",
};

// Quake3
static const char * s_quake3ImageSet[] = {
    "q3-blocks15cgeomtrn.tga",
    "q3-blocks17bloody.tga",
    "q3-dark_tin2.tga",
    "q3-fan_grate.tga",
    "q3-fan.tga",
    "q3-metal2_2.tga",
    "q3-panel_glo.tga",
    "q3-proto_fence.tga",
    "q3-wires02.tga",
};

static const char * s_witnessImageSet[] = {
    "applebark.tga",
    "grass-01.tga",
    "brownRock.tga",
    "rock-01.tga",
    "rock-02.tga",
    "Lao-picture.tga",
    "laser-base.tga",
    "skydome.tga",
    "speaker.tga",
    "specRuin-base.tga",
    "vault.tga",
    "specRuin-puzzle.tga"
};

static const char * s_witnessLmapImageSet[] = {
    "hallway.dds",
    "windmill.dds",
    "tunnel.dds",
    "theater.dds",
    "tower.dds",
    "hub.dds",
    "mine.dds",
    "archway.dds",
    "hut.dds",
    "shaft.dds",
};

static const char * s_normalMapImageSet[] = {
    "01_dot1.png",
    "02_dot2.png",
    "03_dot3.png",
    "04_dot4.png",
    "05_lumpy.png",
    "06_voronoi.png",
    "07_turtle.png",
    "08_normalmap.png",
    "09_metal.png",
    "10_skin.png",
    "11_onetile.png",
    "12_barrel.png",
    "13_arcade.png",
    "14_tentacle.png",
    "15_chest.png",
    "16_face.png",
};


enum Mode {
    Mode_BC1,
    Mode_BC1_Alpha,
    Mode_BC2_Alpha,
    Mode_BC3_Alpha,
    Mode_BC3_YCoCg,
    Mode_BC3_RGBM,
    Mode_BC3_LUVW,
    Mode_BC3_RGBS,
    Mode_BC1_Normal,
    Mode_BC3_Normal,
    Mode_BC5_Normal,
    Mode_BC5_Normal_Stereographic,
    Mode_BC5_Normal_Paraboloid,
    Mode_BC5_Normal_Quartic,
    //Mode_BC5_Normal_DualParaboloid,
    Mode_BC6,
    Mode_BC7,
    Mode_ETC1_IC,
    Mode_ETC1_EtcLib,
    Mode_ETC2_EtcLib,
    Mode_ETC1_RgEtc,
    Mode_ETC1_Intel,
    Mode_ETC1_Ericson,
    Mode_ETC2_RGBM,
    Mode_PVR,
    Mode_Count
};
static const char * s_modeNames[] = {
    "BC1",          // Mode_BC1,
    "BC1-Alpha",    // Mode_BC1_Alpha,
    "BC2-Alpha",    // Mode_BC2_Alpha,
    "BC3-Alpha",    // Mode_BC3_Alpha,
    "BC3-YCoCg",    // Mode_BC3_YCoCg,
    "BC3-RGBM",     // Mode_BC3_RGBM,
    "BC3-LUVW",     // Mode_BC3_LUVW,
    "BC3-RGBS",     // Mode_BC3_RGBS,
    "BC1-Normal",   // Mode_BC1_Normal,
    "BC3-Normal",   // Mode_BC3_Normal,
    "BC5-Normal",   // Mode_BC5_Normal,
    "BC5-Normal-Stereographic",     // Mode_BC5_Normal_Stereographic,
    "BC5-Normal-Paraboloid",        // Mode_BC5_Normal_Paraboloid,
    "BC5-Normal-Quartic",           // Mode_BC5_Normal_Quartic,
    //"BC5-Normal-DualParaboloid",    // Mode_BC5_Normal_DualParaboloid,
    "BC6",          // Mode_BC6,
    "BC7",          // Mode_BC7,
    "ETC1-IC",
    "ETC1-EtcLib",
    "ETC2-EtcLib",
    "ETC1-RgEtc",
    "ETC1-Intel",
    "ETC1-Ericson",
    "ETC2-RGBM",
    "PVR",
};
nvStaticCheck(NV_ARRAY_SIZE(s_modeNames) == Mode_Count);

struct Test {
    const char * name;
    int count;
    Mode modes[6];
};
static Test s_imageTests[] = {
/*0*/   {"Color", 3, {Mode_BC1, Mode_BC3_YCoCg, Mode_BC3_RGBM, /*Mode_BC3_LUVW*/}},
/*1*/   {"Alpha", 3, {Mode_BC1_Alpha, Mode_BC2_Alpha, Mode_BC3_Alpha}},
/*2*/   {"Normal", 4, {Mode_BC5_Normal, Mode_BC5_Normal_Stereographic, Mode_BC5_Normal_Paraboloid, Mode_BC5_Normal_Quartic}},
/*3*/   {"Lightmap", 4, {Mode_BC1, Mode_BC3_YCoCg, Mode_BC3_RGBM, Mode_BC3_RGBS}},
/*4*/   {"HDR", 3, {Mode_ETC2_RGBM, Mode_BC3_RGBM, Mode_BC6}},
/*5*/   {"BC6", 1, {Mode_BC6}},
/*6*/   {"BC7", 1, {Mode_BC7}},
/*7*/   {"ETC", 3, {Mode_ETC1_IC, Mode_ETC1_RgEtc, Mode_ETC2_EtcLib}},
/*8*/   {"Color Mobile", 4, {Mode_PVR, Mode_ETC1_IC, Mode_ETC2_EtcLib, Mode_BC1}},
/*9*/   //{"ETC-Lightmap", 2, {Mode_BC3_RGBM, Mode_ETC_RGBM}},
};
const int s_imageTestCount = ARRAY_SIZE(s_imageTests);

enum ImageType {
    ImageType_RGB,
    ImageType_RGBA,
    ImageType_Normal,
    ImageType_HDR,
};

struct ImageSet
{
    const char * name;
    const char * basePath;
    const char ** fileNames;
    int fileCount;
    ImageType type;
};
static ImageSet s_imageSets[] = {
    {"Kodak",       "kodak",        s_kodakImageSet,        ARRAY_SIZE(s_kodakImageSet),        ImageType_RGB},     // 0
    {"Waterloo",    "waterloo",     s_waterlooImageSet,     ARRAY_SIZE(s_waterlooImageSet),     ImageType_RGB},     // 1
    {"Epic",        "epic",         s_epicImageSet,         ARRAY_SIZE(s_epicImageSet),         ImageType_RGB},     // 2
    {"Farbraush",   "farbrausch",   s_farbrauschImageSet,   ARRAY_SIZE(s_farbrauschImageSet),   ImageType_RGB},     // 3
    {"Lugaru",      "lugaru",       s_lugaruImageSet,       ARRAY_SIZE(s_lugaruImageSet),       ImageType_RGBA},    // 4
    {"Quake3",      "quake3",       s_quake3ImageSet,       ARRAY_SIZE(s_quake3ImageSet),       ImageType_RGBA},    // 5
    {"Witness",     "witness",      s_witnessImageSet,      ARRAY_SIZE(s_witnessImageSet),      ImageType_RGB},     // 6
    {"Lightmap",    "lightmap",     s_witnessLmapImageSet,  ARRAY_SIZE(s_witnessLmapImageSet),  ImageType_HDR},     // 7
    {"Normal",      "id_tnmap",     s_normalMapImageSet,    ARRAY_SIZE(s_normalMapImageSet),    ImageType_Normal},  // 8
	// !!!UNDONE: more HDR image sets
};
const int s_imageSetCount = ARRAY_SIZE(s_imageSets);


struct MyOutputHandler : public nvtt::OutputHandler
{
    MyOutputHandler() : m_data(NULL), m_ptr(NULL) {}
    ~MyOutputHandler()
    {
        free(m_data);
    }

    virtual void beginImage(int size, int width, int height, int depth, int face, int miplevel)
    {
        m_size = size;
        m_width = width;
        m_height = height;
        free(m_data);
        m_data = (unsigned char *)malloc(size);
        m_ptr = m_data;
    }

    virtual void endImage()
    {
    }

    virtual bool writeData(const void * data, int size)
    {
        memcpy(m_ptr, data, size);
        m_ptr += size;
        return true;
    }

    nvtt::Surface decompress(Mode mode, nvtt::Format format, nvtt::Decoder decoder)
    {
        nvtt::Surface img;
        img.setImage2D(format, decoder, m_width, m_height, m_data);
        return img;
    }

    int m_size;
    int m_width;
    int m_height;
    unsigned char * m_data;
    unsigned char * m_ptr;
};

enum ErrorMode {
    ErrorMode_RMSE,
    ErrorMode_CieLab,
    ErrorMode_AngularRMSE
};


int main(int argc, char *argv[])
{
    MyAssertHandler assertHandler;
    MyMessageHandler messageHandler;

    const uint version = nvtt::version();
    const uint major = version / 100 / 100;
    const uint minor = (version / 100) % 100;
    const uint rev = version % 100;

    printf("NVIDIA Texture Tools %u.%u.%u - Copyright NVIDIA Corporation 2007\n\n", major, minor, rev);

    int setIndex = 0;
    int testIndex = 0;
    int errorMode = 0;
    bool fast = false;
    bool nocuda = false;
    bool showHelp = false;
    nvtt::Decoder decoder = nvtt::Decoder_D3D10;
    Path basePath = "";
    const char * outPath = "output";
    const char * regressPath = NULL;

    // Parse arguments.
    for (int i = 1; i < argc; i++)
    {
        if (strcmp("-set", argv[i]) == 0)
        {
            if (i+1 < argc && argv[i+1][0] != '-') {
                setIndex = atoi(argv[i+1]);

                for (int j = 0; j < s_imageSetCount; j++) {
                    if (strCaseDiff(s_imageSets[j].name, argv[i+1]) == 0) {
                        setIndex = j;
                        break;
                    }
                }

                i++;
            }
        }
        else if (strcmp("-test", argv[i]) == 0)
        {
            if (i+1 < argc && argv[i+1][0] != '-') {
                testIndex = atoi(argv[i+1]);
                i++;
            }
        }
        else if (strcmp("-dec", argv[i]) == 0)
        {
            if (i+1 < argc && argv[i+1][0] != '-') {
                decoder = (nvtt::Decoder)atoi(argv[i+1]);
                i++;
            }
        }
        else if (strcmp("-err", argv[i]) == 0)
        {
            if (i+1 < argc && argv[i+1][0] != '-') {
                errorMode = atoi(argv[i+1]);
                i++;
            }
        }
        else if (strcmp("-fast", argv[i]) == 0)
        {
            fast = true;
        }
        else if (strcmp("-nocuda", argv[i]) == 0)
        {
            nocuda = true;
        }
        else if (strcmp("-help", argv[i]) == 0)
        {
            showHelp = true;
        }
        else if (strcmp("-path", argv[i]) == 0)
        {
            if (i+1 < argc && argv[i+1][0] != '-') {
                basePath = argv[i+1];
                i++;
            }
        }
        else if (strcmp("-out", argv[i]) == 0)
        {
            if (i+1 < argc && argv[i+1][0] != '-') {
                outPath = argv[i+1];
                i++;
            }
        }
        else if (strcmp("-regress", argv[i]) == 0)
        {
            if (i+1 < argc && argv[i+1][0] != '-') {
                regressPath = argv[i+1];
                i++;
            }
        }
        else
        {
            printf("Warning: unrecognized option \"%s\"\n", argv[i]);
        }
    }

    // Validate inputs.
    if (testIndex >= s_imageTestCount) {
        printf("Invalid test %d\n", testIndex);
        return 0;
    }
    if (setIndex >= s_imageSetCount) {
        printf("Invalid image set %d\n", setIndex);
        return 0;
    }

    if (showHelp)
    {
        printf("usage: nvtestsuite [options]\n\n");

        printf("Input options:\n");
        printf("  -path <path>   \tInput image path.\n");
        printf("  -regress <path>\tRegression directory.\n");
        printf("  -set [0:%d]     \tImage set.\n", s_imageSetCount-1);
        for (int i = 0; i < s_imageSetCount; i++) {
            printf("    %i:           \t%s.\n", i, s_imageSets[i].name);
        }
        printf("  -test [0:%d]    \tCompression tests to run.\n", s_imageTestCount);
        for (int i = 0; i < s_imageTestCount; i++) {
            printf("    %i:           \t%s.\n", i, s_imageTests[i].name);
        }
        printf("  -dec [0:2]     \tDecompressor.\n");
        printf("    0:           \tReference D3D10 (default).\n");
        printf("    1:           \tReference D3D9.\n");
        printf("    2:           \tNVIDIA.\n");

        printf("Compression options:\n");
        printf("  -fast          \tFast compression.\n");
        printf("  -nocuda        \tDo not use cuda compressor.\n");

        printf("Output options:\n");
        printf("  -out <path>    \tOutput directory.\n");
        printf("  -err [0:2]     \tError mode.\n");
        printf("    0:           \tRMSE (default).\n");
        printf("    1:           \tCieLab.\n");
        printf("    2:           \tAngular RMSE.\n");

        return 1;
    }

    nvtt::CompressionOptions compressionOptions;
    compressionOptions.setFormat(nvtt::Format_BC1);
    if (fast)
    {
        compressionOptions.setQuality(nvtt::Quality_Fastest);
    }
    else
    {
        compressionOptions.setQuality(nvtt::Quality_Normal);
        //compressionOptions.setQuality(nvtt::Quality_Production);
    }
    //compressionOptions.setExternalCompressor("ati");
    //compressionOptions.setExternalCompressor("squish");
    //compressionOptions.setExternalCompressor("d3dx");
    //compressionOptions.setExternalCompressor("stb");

    
    const ImageSet & set = s_imageSets[setIndex];
    const Test & test = s_imageTests[testIndex];


    nvtt::OutputOptions outputOptions;
    outputOptions.setOutputHeader(false);

    MyOutputHandler outputHandler;
    outputOptions.setOutputHandler(&outputHandler);

    nvtt::Context context;
    context.enableCudaAcceleration(!nocuda);

    if (basePath.length() > 0) {
        basePath.appendSeparator();
    }
    basePath.append(set.basePath);

    FileSystem::changeDirectory(basePath.str());
    FileSystem::createDirectory(outPath);

    //Path csvFileName;
    //csvFileName.format("%s/result-%d.csv", outPath, setIndex);
    //StdOutputStream csvStream(csvFileName.str());
    //TextWriter csvWriter(&csvStream);

    Path graphFileName;
    //graphFileName.format("%s/chart_%s_CIE-Lab.txt", outPath, test.name);
    graphFileName.format("%s/chart_%s_RMSE.txt", outPath, test.name);
    StdOutputStream graphStream(graphFileName.str());
    TextWriter graphWriter(&graphStream);

    graphWriter << "http://chart.apis.google.com/chart?";

    // Graph size.
    graphWriter << "chs=480x240";

    // Graph type: line
    graphWriter << "&cht=lc";

    // Margins.
    graphWriter << "&chma=30,10,10|0,40";

    // Grid lines.
    graphWriter << "&chxt=x,y&chxtc=0,-1000|1,-1000";

    // Labels on the left side.
    if (errorMode == ErrorMode_RMSE) {
        graphWriter << "&chxr=0,1," << set.fileCount << ",1|1,0,0.03,0.01";
    }
    else if (errorMode == ErrorMode_CieLab) {
       graphWriter << "&chxr=0,1," << set.fileCount << ",1|1,4,22,1";
    }
    else if (errorMode == ErrorMode_AngularRMSE) {
        graphWriter << "&chxr=0,1," << set.fileCount << ",1|1,0,0.2,0.02";      // 0.05,0.01
    }

    // Labels at the bottom.
    graphWriter << "&chdlp=b";

    // Line colors.
    graphWriter << "&chco=";
    for (int t = 0; t < test.count; t++)
    {
        const char * colors[] = {
            "3D7930", "952826", "3D1FC1",
            "FF9900", "999999", "999999", // pick other colors...
        };
        graphWriter << colors[t];
        if (t != test.count-1) graphWriter << ",";
    }

    // Line width.
    graphWriter << "&chls=";
    for (int t = 0; t < test.count; t++)
    {
        graphWriter << "2";
        if (t != test.count-1) graphWriter << "|";
    }

    // Data ranges.
    graphWriter << "&chds=";
    for (int t = 0; t < test.count; t++)
    {
        if (errorMode == ErrorMode_RMSE) {
            graphWriter << "0,0.05";
        }
        else if (errorMode == ErrorMode_CieLab) {
            graphWriter << "4,22";
        }
        else if (errorMode == ErrorMode_AngularRMSE) {
            graphWriter << "0,0.05";
        }
        if (t != test.count-1) graphWriter << ",";
    }

    // Legends.
    graphWriter << "&chdl=";
    for (int t = 0; t < test.count; t++)
    {
        graphWriter << s_modeNames[test.modes[t]];
        if (t != test.count-1) graphWriter << "|";
    }

    // Title
    if (errorMode == ErrorMode_RMSE) {
        graphWriter << "&chtt=" << set.name << "%20-%20" << test.name << "%20-%20RMSE";
    }
    else if (errorMode == ErrorMode_CieLab) {
        graphWriter << "&chtt=" << set.name << "%20-%20" << test.name << "%20-%20CIE-Lab";
    }
    else if (errorMode == ErrorMode_AngularRMSE) {
        graphWriter << "&chtt=" << set.name << "%20-%20" << test.name << "%20-%20Angular RMSE";
    }


    Timer timer;
    //int failedTests = 0;
    //float totalDiff = 0;

    nvtt::Surface img;

    printf("Running test '%s' with set '%s'\n", test.name, set.name);

    graphWriter << "&chd=t:";

    for (int t = 0; t < test.count; t++)
    {
        float totalCompressionTime = 0;
        float totalError = 0;

        Mode mode = test.modes[t];

        nvtt::Format format;
        const char * compressor_name = NULL;
        if (mode == Mode_BC1 || mode == Mode_BC1_Alpha || mode == Mode_BC1_Normal || mode == Mode_BC3_RGBS) {
            format = nvtt::Format_BC1;
        }
        else if (mode == Mode_BC3_Alpha || mode == Mode_BC3_YCoCg || mode == Mode_BC3_LUVW) {
            format = nvtt::Format_BC3;
        }
        else if (mode == Mode_BC3_Normal) {
            format = nvtt::Format_BC3n;
        }
        else if (mode == Mode_BC5_Normal || mode == Mode_BC5_Normal_Stereographic || mode == Mode_BC5_Normal_Paraboloid || mode == Mode_BC5_Normal_Quartic) {
            format = nvtt::Format_BC5;
        }
        else if (mode == Mode_BC3_RGBM) {
            format = nvtt::Format_BC3_RGBM;
        }
        else if (mode == Mode_BC6)
        {
            format = nvtt::Format_BC6;
        }
        else if (mode == Mode_BC7)
        {
            format = nvtt::Format_BC7;
        }
        else if (mode == Mode_ETC1_IC)
        {
            format = nvtt::Format_ETC1;
        }
        else if (mode == Mode_ETC1_EtcLib)
        {
            format = nvtt::Format_ETC1;
            compressor_name = "etclib";
        }
        else if (mode == Mode_ETC2_EtcLib)
        {
            format = nvtt::Format_ETC2_RGB;
            compressor_name = "etclib";
        }
        else if (mode == Mode_ETC1_RgEtc)
        {
            format = nvtt::Format_ETC1;
            compressor_name = "rg_etc";
        }
        else if (mode == Mode_ETC1_Ericson)
        {
            format = nvtt::Format_ETC1;
            compressor_name = "etcpack";
        }
        else if (mode == Mode_ETC1_Intel)
        {
            format = nvtt::Format_ETC1;
            compressor_name = "intel";
        }
        else if (mode == Mode_ETC2_RGBM)
        {
            format = nvtt::Format_ETC2_RGBM;
        }
        else if (mode == Mode_PVR)
        {
            format = nvtt::Format_PVR_4BPP_RGB;
        }
        else
        {
            nvUnreachable();
        }
        
        compressionOptions.setFormat(format);
        if (compressor_name) compressionOptions.setExternalCompressor(compressor_name);

        if (set.type == ImageType_RGBA) {
            img.setAlphaMode(nvtt::AlphaMode_Transparency);
        }
        else if (set.type == ImageType_Normal) { 
            img.setNormalMap(true);
        }
        else if (set.type == ImageType_HDR) { // Lightmap's alpha channel is coverage.
            img.setAlphaMode(nvtt::AlphaMode_Transparency);
        }

        // Create output directory.
        Path outputFilePath;
        outputFilePath.format("%s/%s", outPath, s_modeNames[test.modes[t]]);
        FileSystem::createDirectory(outputFilePath.str());


        printf("Processing Mode: %s\n", s_modeNames[test.modes[t]]);
        for (int i = 0; i < set.fileCount; i++)
        {
            if (!img.load(set.fileNames[i]))
            {
                printf("Input image '%s' not found.\n", set.fileNames[i]);
                return EXIT_FAILURE;
            }
            float color_range = 0.0f;

            if (img.isNormalMap()) {
                img.normalizeNormalMap();
            }
            /*if (set.type == ImageType_HDR) {
                img.scaleBias(0, 1.0f/4.0f, 0.0f); img.clamp(0);
                img.scaleBias(1, 1.0f/4.0f, 0.0f); img.clamp(1);
                img.scaleBias(2, 1.0f/4.0f, 0.0f); img.clamp(2);
                img.toGamma(2);
            }*/

            nvtt::Surface tmp = img;
            if (mode == Mode_BC1) {
                if (set.type == ImageType_HDR) {
                    /*for (int i = 0; i < 3; i++) {
                        tmp.scaleBias(i, 0.25f, 0);
                        tmp.clamp(i);
                    }*/
                }
            }
            if (mode == Mode_BC3_YCoCg) {
                tmp.setAlphaMode(nvtt::AlphaMode_None);
                if (set.type == ImageType_HDR) {
                    /*for (int i = 0; i < 3; i++) {
                        tmp.scaleBias(i, 1.0f/4.0f, 0);
                        tmp.clamp(i);
                    }*/
                }
                tmp.toYCoCg();          // Y=3, Co=0, Cg=1
                tmp.blockScaleCoCg();   // Co=0, Cg=1, Scale=2, ScaleBits = 5

                tmp.scaleBias(0, 123.0f/255.0f, 123.0f/255.0f); tmp.clamp(0, 0, 246.0f/255.0f); // -1->0, 0->123, 1->246
                tmp.scaleBias(1, 125.0f/255.0f, 125.0f/255.0f); tmp.clamp(1, 0, 250.0f/255.0f); // -1->0, 0->125, 1->250

                //tmp.scaleBias(0, 0.5f, 0.5f); tmp.clamp(0);
                //tmp.scaleBias(1, 0.5f, 0.5f); tmp.clamp(1);

                tmp.clamp(2);
                tmp.clamp(3);
            }
            else if (mode == Mode_BC3_RGBM || mode == Mode_ETC2_RGBM) {
                float r, g, b;
                tmp.range(0, NULL, &r);
                tmp.range(1, NULL, &g);
                tmp.range(2, NULL, &b);
                color_range = max3(r, g, b);
                printf("color range = %f\n", color_range);

                tmp.setAlphaMode(nvtt::AlphaMode_Transparency);

                const float max_color_range = 16.0f;

                if (color_range > max_color_range) {
                    color_range = max_color_range;
                }

                for (int i = 0; i < 3; i++) {
                    tmp.scaleBias(i, 1.0f / color_range, 0.0f);
                }
                tmp.toneMap(nvtt::ToneMapper_Linear, /*parameters=*/NULL); // Clamp without changing the hue.

                // Clamp alpha.
                tmp.clamp(3);

                // To gamma.
                tmp.toGamma(2);

                compressionOptions.setRGBMThreshold(0.2f);
            }
            else if (mode == Mode_BC3_LUVW) {
                tmp.setAlphaMode(nvtt::AlphaMode_None);
                if (set.type == ImageType_HDR) {
                    tmp.toLUVW(8.0f);
                }
                else {
                    tmp.toLUVW();
                }
            }
            else if (mode == Mode_BC3_RGBS) {
                //tmp.toJPEGLS();
                //tmp.scaleBias(0, 123.0f/255.0f, 123.0f/255.0f); tmp.clamp(0, 0, 246.0f/255.0f); // -1->0, 0->123, 1->246
                //tmp.scaleBias(2, 123.0f/255.0f, 123.0f/255.0f); tmp.clamp(0, 0, 246.0f/255.0f); // -1->0, 0->123, 1->246

                // Not helping...
                //tmp.blockLuminanceScale(0.1f);
                /*tmp.toYCoCg();
                tmp.scaleBias(0, 0.5, 0.5);
                tmp.scaleBias(1, 0.5, 0.5);
                tmp.swizzle(0, 3, 1, 4); // Co Cg 1 Y -> Co Y Cg 1
                tmp.copyChannel(img, 3); // Restore alpha channel for weighting.*/
            }
            else if (mode == Mode_BC5_Normal) {
                tmp.transformNormals(nvtt::NormalTransform_Orthographic);
            }
            else if (mode == Mode_BC5_Normal_Stereographic) {
                tmp.transformNormals(nvtt::NormalTransform_Stereographic);
            }
            else if (mode == Mode_BC5_Normal_Paraboloid) {
                tmp.transformNormals(nvtt::NormalTransform_Paraboloid);
            }
            else if (mode == Mode_BC5_Normal_Quartic) {
                tmp.transformNormals(nvtt::NormalTransform_Quartic);
            }
            /*else if (mode == Mode_BC5_Normal_DualParaboloid) {
                tmp.transformNormals(nvtt::NormalTransform_DualParaboloid);
            }*/


            printf("Compressing: \t'%s'\n", set.fileNames[i]);

            timer.start();

            context.compress(tmp, 0, 0, compressionOptions, outputOptions);

            timer.stop();
            printf("  Time:  \t%.3f sec\n", timer.elapsed());
            totalCompressionTime += timer.elapsed();

            nvtt::Surface img_out = outputHandler.decompress(mode, format, decoder);
            img_out.setAlphaMode(img.alphaMode());
            img_out.setNormalMap(img.isNormalMap());

            if (mode == Mode_BC1) {
                if (set.type == ImageType_HDR) {
                    /*for (int i = 0; i < 3; i++) {
                        img_out.scaleBias(i, 4.0f, 0);
                    }*/
                }
            }
            else if (mode == Mode_BC3_YCoCg) {
                img_out.scaleBias(0, 255.0f/123, -1.0f); // 0->-1, 123->0, 246->1
                img_out.scaleBias(1, 255.0f/125, -1.0f); // 0->-1, 125->0, 150->1

                //img_out.scaleBias(0, 2.0f, -1.0f);
                //img_out.scaleBias(1, 2.0f, -1.0f);
                
                img_out.fromYCoCg();
                img_out.clamp(0);
                img_out.clamp(1);
                img_out.clamp(2);
                if (set.type == ImageType_HDR) {
                    /*for (int i = 0; i < 3; i++) {
                        img_out.scaleBias(i, 4.0f, 0);
                    }*/
                }
            }
            else if (mode == Mode_BC3_RGBM || mode == Mode_ETC2_RGBM) {
                /*if (set.type == ImageType_HDR) {
                    //img_out.fromRGBM(3.0f);
                    img_out.fromRGBM(range);
                    img_out.toLinear(2.0f);
                }
                else {
                    img_out.fromRGBM();
                }*/

                img_out.fromRGBM(1.0f, 0.2f);
                img_out.toLinear(2);

                for (int i = 0; i < 3; i++) {
                    img_out.scaleBias(i, color_range, 0.0f);
                }

                img_out.copyChannel(img, 3);          // Copy alpha channel from source.
                img_out.setAlphaMode(nvtt::AlphaMode_Transparency);
            }
            else if (mode == Mode_BC3_LUVW) {
                if (set.type == ImageType_HDR) {
                    img_out.fromLUVW(8.0f);
                }
                else {
                    img_out.fromLUVW();
                }
            }
            else if (mode == Mode_BC3_RGBS) {
                //img_out.scaleBias(0, 255.0f/123, -1.0f);
                //img_out.scaleBias(2, 255.0f/123, -1.0f);
                //img_out.fromJPEGLS();
                /*img_out.swizzle(0, 2, 4, 1);    // Co Y Cg 1 - > Co Cg 1 Y
                img_out.scaleBias(0, 1.0, -0.5);
                img_out.scaleBias(1, 1.0, -0.5);
                img_out.fromYCoCg();*/
            }
            else if (mode == Mode_BC5_Normal) {
                img_out.reconstructNormals(nvtt::NormalTransform_Orthographic);
            }
            else if (mode == Mode_BC5_Normal_Stereographic) {
                img_out.reconstructNormals(nvtt::NormalTransform_Stereographic);
            }
            else if (mode == Mode_BC5_Normal_Paraboloid) {
                img_out.reconstructNormals(nvtt::NormalTransform_Paraboloid);
            }
            else if (mode == Mode_BC5_Normal_Quartic) {
                img_out.reconstructNormals(nvtt::NormalTransform_Quartic);
            }
            /*else if (mode == Mode_BC5_Normal_DualParaboloid) {
                tmp.transformNormals(nvtt::NormalTransform_DualParaboloid);
            }*/

            nvtt::Surface diff = nvtt::diff(img, img_out, 1.0f);

            //bool residualCompression = (set.type == ImageType_HDR);
            bool residualCompression = (mode == Mode_BC3_RGBS);
            if (residualCompression)
            {
                float residualScale = 8.0f;
                nvtt::Surface residual = diff;
                for (int j = 0; j < 3; j++) {
                    residual.scaleBias(j, residualScale, 0.5); // @@ The residual scale is fairly arbitrary.
                    residual.clamp(j);
                }
                residual.toGreyScale(1, 1, 1, 0);

                /*outputFileName.format("%s/%s", outputFilePath.str(), set.fileNames[i]);
                outputFileName.stripExtension();
                outputFileName.append("_residual.png");
                residual.save(outputFileName.str());*/

                nvtt::CompressionOptions residualCompressionOptions;
                residualCompressionOptions.setFormat(nvtt::Format_BC4);
                residualCompressionOptions.setQuality(nvtt::Quality_Production);
                
                context.compress(residual, 0, 0, compressionOptions, outputOptions);

                nvtt::Surface residual_out = outputHandler.decompress(mode, format, decoder);

                /*outputFileName.format("%s/%s", outputFilePath.str(), set.fileNames[i]);
                outputFileName.stripExtension();
                outputFileName.append("_residual_out.png");
                residual_out.save(outputFileName.str());*/

                residual_out.scaleBias(0, 1.0f/residualScale, -0.5f/residualScale);
                residual_out.scaleBias(1, 1.0f/residualScale, -0.5f/residualScale);
                residual_out.scaleBias(2, 1.0f/residualScale, -0.5f/residualScale);

                img_out.addChannel(residual_out, 0, 0, -1.0f); img_out.clamp(0);
                img_out.addChannel(residual_out, 1, 1, -1.0f); img_out.clamp(1);
                img_out.addChannel(residual_out, 2, 2, -1.0f); img_out.clamp(2);
            }

            /*if (set.type == ImageType_HDR)
            {
                Path outputFileName;
                outputFileName.format("%s/%s", outPath, set.fileNames[i]);
                outputFileName.stripExtension();
                if (set.type == ImageType_HDR) outputFileName.append(".dds");
                else outputFileName.append(".tga");
                if (!img.save(outputFileName.str()))
                {
                    printf("Error saving file '%s'.\n", outputFileName.str());
                }
            }*/

            // Output compressed image.
            Path outputFileName;
            outputFileName.format("%s/%s", outputFilePath.str(), set.fileNames[i]);
            outputFileName.stripExtension();
            if (set.type == ImageType_HDR) outputFileName.append(".dds");
            else outputFileName.append(".tga");
            if (!img_out.save(outputFileName.str(), set.type == ImageType_RGBA, set.type == ImageType_HDR))
            {
                printf("Error saving file '%s'.\n", outputFileName.str());
            }

            // Output RMSE.
            float error;
            if (errorMode == ErrorMode_RMSE) {
                error = nvtt::rmsError(img, img_out);
            }
            else if (errorMode == ErrorMode_CieLab) {
                error = nvtt::cieLabError(img, img_out);
            }
            else if (errorMode == ErrorMode_AngularRMSE) {
                error = nvtt::angularError(img, img_out);
            }

            totalError += error;
            printf("  Error: \t%.4f\n", error);

            graphWriter << error;
            if (i != set.fileCount-1) graphWriter << ",";


            // Output diff.
            for (int j = 0; j < 3; j++) {
                diff.scaleBias(j, 4.0f, 0.0f); 
                diff.abs(j);
                diff.clamp(j);
            }

            outputFileName.format("%s/%s", outputFilePath.str(), set.fileNames[i]);
            outputFileName.stripExtension();
            outputFileName.append("_diff.tga");
            diff.save(outputFileName.str());


            // Output csv file
            //csvWriter << "\"" << fileNames[i] << "\"," << rmse << "\n";

            /*if (regressPath != NULL)
            {
                Path regressFileName;
                regressFileName.format("%s/%s", regressPath, fileNames[i]);
                regressFileName.stripExtension();
                regressFileName.append(".png");

                nvtt::Surface img_reg;
                if (!img_reg.load(regressFileName.str()))
                {
                    printf("Regression image '%s' not found.\n", regressFileName.str());
                    return EXIT_FAILURE;
                }

                float rmse_reg = rmsError(img, img_reg);

                float diff = rmse_reg - rmse;
                totalDiff += diff;

                const char * text = "PASSED";
                if (equal(diff, 0)) text = "PASSED";
                else if (diff < 0) {
                    text = "FAILED";
                    failedTests++;
                }

                printf("  Diff: \t%.4f (%s)\n", diff, text);
            }*/

            fflush(stdout);
        }

        totalError /= set.fileCount;

        printf("Total Results:\n");
        printf("  Total Compression Time:\t%.3f sec\n", totalCompressionTime);
        printf("  Average Error:         \t%.4f\n", totalError);

        if (t != test.count-1) graphWriter << "|";
    }

    /*if (regressPath != NULL)
    {
        printf("Regression Results:\n");
        printf("  Diff: %.4f\n", totalDiff);
        printf("  %d/%d tests failed.\n", failedTests, fileCount);
    }*/

    return EXIT_SUCCESS;
}

