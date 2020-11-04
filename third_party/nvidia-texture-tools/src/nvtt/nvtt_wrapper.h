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

#ifndef NVTT_WRAPPER_H
#define NVTT_WRAPPER_H

// Function linkage
#if NVTT_SHARED

#if defined _WIN32 || defined WIN32 || defined __NT__ || defined __WIN32__ || defined __MINGW32__
#	ifdef NVTT_EXPORTS
#		define NVTT_API __declspec(dllexport)
#	else
#		define NVTT_API __declspec(dllimport)
#	endif
#endif

#if defined __GNUC__ >= 4
#	ifdef NVTT_EXPORTS
#		define NVTT_API __attribute__((visibility("default")))
#	endif
#endif

#endif // NVTT_SHARED

#if !defined NVTT_API
#	define NVTT_API
#endif

#define NVTT_VERSION 20102

#ifdef __cplusplus
typedef struct nvtt::InputOptions NvttInputOptions;
typedef struct nvtt::CompressionOptions NvttCompressionOptions;
typedef struct nvtt::OutputOptions NvttOutputOptions;
typedef struct nvtt::Compressor NvttCompressor;
#else
typedef struct NvttInputOptions NvttInputOptions;
typedef struct NvttCompressionOptions NvttCompressionOptions;
typedef struct NvttOutputOptions NvttOutputOptions;
typedef struct NvttCompressor NvttCompressor;
#endif

/// Supported compression formats.
typedef enum
{
	// No compression.
	NVTT_Format_RGB,
	NVTT_Format_RGBA = NVTT_Format_RGB,

	// DX9 formats.
	NVTT_Format_DXT1,
	NVTT_Format_DXT1a,	// DXT1 with binary alpha.
	NVTT_Format_DXT3,
	NVTT_Format_DXT5,
	NVTT_Format_DXT5n,	// Compressed HILO: R=1, G=y, B=0, A=x
	
	// DX10 formats.
	NVTT_Format_BC1 = NVTT_Format_DXT1,
	NVTT_Format_BC1a = NVTT_Format_DXT1a,
	NVTT_Format_BC2 = NVTT_Format_DXT3,
	NVTT_Format_BC3 = NVTT_Format_DXT5,
	NVTT_Format_BC3n = NVTT_Format_DXT5n,
	NVTT_Format_BC4,
	NVTT_Format_BC5,
	
	NVTT_Format_DXT1n,   // Not supported.
	NVTT_Format_CTX1,    // Not supported.

	NVTT_Format_BC6,
	NVTT_Format_BC7,

	NVTT_Format_BC3_RGBM,

	NVTT_Format_ETC1,
	NVTT_Format_ETC2_R,
	NVTT_Format_ETC2_RG,
	NVTT_Format_ETC2_RGB,
	NVTT_Format_ETC2_RGBA,
	NVTT_Format_ETC2_RGB_A1,

	NVTT_Format_ETC2_RGBM,

	NVTT_Format_PVR_2BPP_RGB,     // Using PVR textools.
	NVTT_Format_PVR_4BPP_RGB,
	NVTT_Format_PVR_2BPP_RGBA,
	NVTT_Format_PVR_4BPP_RGBA,
} NvttFormat;

/// Quality modes.
typedef enum
{
	NVTT_Quality_Fastest,
	NVTT_Quality_Normal,
	NVTT_Quality_Production,
	NVTT_Quality_Highest,
} NvttQuality;

/// Wrap modes.
typedef enum
{
	NVTT_WrapMode_Clamp,
	NVTT_WrapMode_Repeat,
	NVTT_WrapMode_Mirror,
} NvttWrapMode;

/// Texture types.
typedef enum
{
	NVTT_TextureType_2D,
	NVTT_TextureType_Cube,
	TextureType_3D,
	TextureType_Array,
} NvttTextureType;

/// Input formats.
typedef enum
{
	NVTT_InputFormat_BGRA_8UB,      // Normalized [0, 1] 8 bit fixed point.
	NVTT_InputFormat_RGBA_16F,      // 16 bit floating point.
	NVTT_InputFormat_RGBA_32F,      // 32 bit floating point.
	NVTT_InputFormat_R_32F,         // Single channel 32 bit floating point.
} NvttInputFormat;

/// Mipmap downsampling filters.
typedef enum
{
	NVTT_MipmapFilter_Box,
	NVTT_MipmapFilter_Triangle,
	NVTT_MipmapFilter_Kaiser,
} NvttMipmapFilter;

/// Extents rounding mode.
typedef enum
{
	NVTT_RoundMode_None,
	NVTT_RoundMode_ToNextPowerOfTwo,
	NVTT_RoundMode_ToNearestPowerOfTwo,
	NVTT_RoundMode_ToPreviousPowerOfTwo,
	NVTT_RoundMode_ToNextMultipleOfFour,                     // (New in NVTT 2.1)
	NVTT_RoundMode_ToNearestMultipleOfFour,                  // (New in NVTT 2.1)
	NVTT_RoundMode_ToPreviousMultipleOfFour,                 // (New in NVTT 2.1)
} NvttRoundMode;

/// Alpha mode.
typedef enum
{
	NVTT_AlphaMode_None,
	NVTT_AlphaMode_Transparency,
	NVTT_AlphaMode_Premultiplied,
} NvttAlphaMode;

// Error codes.
typedef enum
{
    NVTT_Error_Unknown,
	NVTT_Error_InvalidInput,
	NVTT_Error_UnsupportedFeature,
	NVTT_Error_CudaError,
	NVTT_Error_FileOpen,
	NVTT_Error_FileWrite,
    NVTT_Error_UnsupportedOutputFormat,
} NvttError;

// Output container format types.
typedef enum
{
    NVTT_Container_DDS,
    NVTT_Container_DDS10,
    NVTT_Container_KTX,
} NvttContainer;

typedef enum
{
	NVTT_False,
	NVTT_True,
} NvttBoolean;


#ifdef __cplusplus
extern "C" {
#endif

// Callbacks
typedef void (* nvttErrorHandler)(NvttError e);
typedef void (* nvttBeginImageHandler)(int size, int width, int height, int depth, int face, int miplevel);
typedef bool (* nvttOutputHandler)(const void * data, int size);
typedef void (* nvttEndImageHandler)();


// InputOptions class.
NVTT_API NvttInputOptions * nvttCreateInputOptions();
NVTT_API void nvttDestroyInputOptions(NvttInputOptions * inputOptions);

NVTT_API void nvttSetInputOptionsTextureLayout(NvttInputOptions * inputOptions, NvttTextureType type, int w, int h, int d, int arraySize);
NVTT_API void nvttResetInputOptionsTextureLayout(NvttInputOptions * inputOptions);
NVTT_API NvttBoolean nvttSetInputOptionsMipmapData(NvttInputOptions * inputOptions, const void * data, int w, int h, int d, int face, int mipmap);
NVTT_API void nvttSetInputOptionsFormat(NvttInputOptions * inputOptions, NvttInputFormat format);
NVTT_API void nvttSetInputOptionsAlphaMode(NvttInputOptions * inputOptions, NvttAlphaMode alphaMode);
NVTT_API void nvttSetInputOptionsGamma(NvttInputOptions * inputOptions, float inputGamma, float outputGamma);
NVTT_API void nvttSetInputOptionsWrapMode(NvttInputOptions * inputOptions, NvttWrapMode mode);
NVTT_API void nvttSetInputOptionsMipmapFilter(NvttInputOptions * inputOptions, NvttMipmapFilter filter);
NVTT_API void nvttSetInputOptionsMipmapGeneration(NvttInputOptions * inputOptions, NvttBoolean enabled, int maxLevel);
NVTT_API void nvttSetInputOptionsKaiserParameters(NvttInputOptions * inputOptions, float width, float alpha, float stretch);
NVTT_API void nvttSetInputOptionsNormalMap(NvttInputOptions * inputOptions, NvttBoolean b);
NVTT_API void nvttSetInputOptionsConvertToNormalMap(NvttInputOptions * inputOptions, NvttBoolean convert);
NVTT_API void nvttSetInputOptionsHeightEvaluation(NvttInputOptions * inputOptions, float redScale, float greenScale, float blueScale, float alphaScale);
NVTT_API void nvttSetInputOptionsNormalFilter(NvttInputOptions * inputOptions, float sm, float medium, float big, float large);
NVTT_API void nvttSetInputOptionsNormalizeMipmaps(NvttInputOptions * inputOptions, NvttBoolean b);
NVTT_API void nvttSetInputOptionsMaxExtents(NvttInputOptions * inputOptions, int dim);
NVTT_API void nvttSetInputOptionsRoundMode(NvttInputOptions * inputOptions, NvttRoundMode mode);


// CompressionOptions class.
NVTT_API NvttCompressionOptions * nvttCreateCompressionOptions();
NVTT_API void nvttDestroyCompressionOptions(NvttCompressionOptions * compressionOptions);

NVTT_API void nvttSetCompressionOptionsFormat(NvttCompressionOptions * compressionOptions, NvttFormat format);
NVTT_API void nvttSetCompressionOptionsQuality(NvttCompressionOptions * compressionOptions, NvttQuality quality);
NVTT_API void nvttSetCompressionOptionsColorWeights(NvttCompressionOptions * compressionOptions, float red, float green, float blue, float alpha);
NVTT_API void nvttSetCompressionOptionsPixelFormat(NvttCompressionOptions * compressionOptions, unsigned int bitcount, unsigned int rmask, unsigned int gmask, unsigned int bmask, unsigned int amask);
NVTT_API void nvttSetCompressionOptionsQuantization(NvttCompressionOptions * compressionOptions, NvttBoolean colorDithering, NvttBoolean alphaDithering, NvttBoolean binaryAlpha, int alphaThreshold);


// OutputOptions class.
NVTT_API NvttOutputOptions * nvttCreateOutputOptions();
NVTT_API void nvttDestroyOutputOptions(NvttOutputOptions * outputOptions);

NVTT_API void nvttSetOutputOptionsFileName(NvttOutputOptions * outputOptions, const char * fileName);
NVTT_API void nvttSetOutputOptionsOutputHeader(NvttOutputOptions * outputOptions, NvttBoolean b);
NVTT_API void nvttSetOutputOptionsContainer(NvttOutputOptions * outputOptions, NvttContainer containerFormat);
NVTT_API void nvttSetOutputOptionsSrgbFlag(NvttOutputOptions * outputOptions, NvttBoolean b);
NVTT_API void nvttSetOutputOptionsErrorHandler(NvttOutputOptions * outputOptions, nvttErrorHandler errorHandler);
NVTT_API void nvttSetOutputOptionsOutputHandler(NvttOutputOptions * outputOptions, nvttBeginImageHandler beginImageHandler, nvttOutputHandler outputHandler, nvttEndImageHandler endImageHandler);


// Compressor class.
NVTT_API NvttCompressor * nvttCreateCompressor();
NVTT_API void nvttDestroyCompressor(NvttCompressor * compressor);

NVTT_API void nvttEnableCudaAcceleration(NvttCompressor * compressor, NvttBoolean b);
NVTT_API NvttBoolean nvttIsCudaAccelerationEnabled(const NvttCompressor* compressor);
NVTT_API NvttBoolean nvttCompress(const NvttCompressor * compressor, const NvttInputOptions * inputOptions, const NvttCompressionOptions * compressionOptions, const NvttOutputOptions * outputOptions);
NVTT_API int nvttEstimateSize(const NvttCompressor * compressor, const NvttInputOptions * inputOptions, const NvttCompressionOptions * compressionOptions);


// Global functions.
NVTT_API const char * nvttErrorString(NvttError e);
NVTT_API unsigned int nvttVersion();


#ifdef __cplusplus
} // extern "C"
#endif

#endif // NVTT_WRAPPER_H
