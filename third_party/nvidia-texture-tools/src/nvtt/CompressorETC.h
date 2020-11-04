#include "nvcore/nvcore.h"

namespace nv {

    class Vector3;
    class Vector4;
    
    void decompress_etc(const void * input_block, Vector4 output_colors[16]);
    void decompress_eac(const void * input_block, Vector4 output_colors[16], int output_channel);
    void decompress_etc_eac(const void * input_block, Vector4 output_colors[16]);

    float compress_etc1(Vector4 input_colors[16], float input_weights[16], const Vector3 & color_weights, void * output);
    float compress_etc2(Vector4 input_colors[16], float input_weights[16], const Vector3 & color_weights, void * output);
    float compress_etc2_a1(Vector4 input_colors[16], float input_weights[16], const Vector3 & color_weights, void * output);
    float compress_eac(Vector4 input_colors[16], float input_weights[16], int input_channel, int search_radius, bool use_11bit_mode, void * output);
    float compress_etc2_eac(Vector4 input_colors[16], float input_weights[16], const Vector3 & color_weights, void * output);

}


