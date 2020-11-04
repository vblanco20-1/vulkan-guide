PROJECT(nvtt)

ADD_SUBDIRECTORY(squish)

SET(NVTT_SRCS
    nvtt.h nvtt.cpp
    nvtt_wrapper.h nvtt_wrapper.cpp
    Compressor.h
    BlockCompressor.h BlockCompressor.cpp
    CompressorDX9.h CompressorDX9.cpp
    CompressorDX10.h CompressorDX10.cpp
    CompressorDX11.h CompressorDX11.cpp
    icbc.h icbc.cpp
    CompressorDXT5_RGBM.h CompressorDXT5_RGBM.cpp
    CompressorETC.h CompressorETC.cpp
    CompressorRGB.h CompressorRGB.cpp
    Context.h Context.cpp
    QuickCompressDXT.h QuickCompressDXT.cpp
    OptimalCompressDXT.h OptimalCompressDXT.cpp
    SingleColorLookup.h SingleColorLookup.cpp
    CompressionOptions.h CompressionOptions.cpp
    InputOptions.h InputOptions.cpp
    OutputOptions.h OutputOptions.cpp
    TaskDispatcher.h #TaskDispatcher.cpp
    Surface.h Surface.cpp
    CubeSurface.h CubeSurface.cpp
    cuda/CudaUtils.h cuda/CudaUtils.cpp
    cuda/CudaMath.h
    cuda/BitmapTable.h
    cuda/CudaCompressorDXT.h cuda/CudaCompressorDXT.cpp
    #${NV_SOURCE_DIR}/extern/rg_etc1_v104/rg_etc1.h ${NV_SOURCE_DIR}/extern/rg_etc1_v104/rg_etc1.cpp
    #${NV_SOURCE_DIR}/extern/etcpack/etcpack.cxx ${NV_SOURCE_DIR}/extern/etcpack/etcdec.cxx
    )

IF (CUDA_FOUND)
    ADD_DEFINITIONS(-DHAVE_CUDA)
    CUDA_COMPILE(CUDA_SRCS cuda/CompressKernel.cu)
    SET(NVTT_SRCS ${NVTT_SRCS} ${CUDA_SRCS})
    SET(LIBS ${LIBS} ${CUDA_LIBRARIES})
    INCLUDE_DIRECTORIES(${CUDA_INCLUDE_DIRS})
ENDIF (CUDA_FOUND)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${NV_SOURCE_DIR}/extern/rg_etc1_v104)

ADD_DEFINITIONS(-DNVTT_EXPORTS)
#ADD_DEFINITIONS(-DHAVE_RGETC)
#ADD_DEFINITIONS(-DHAVE_ETCPACK)

IF(NVTT_SHARED)	
    ADD_LIBRARY(nvtt SHARED ${NVTT_SRCS})
ELSE(NVTT_SHARED)
    ADD_LIBRARY(nvtt ${NVTT_SRCS})
ENDIF(NVTT_SHARED)

TARGET_LINK_LIBRARIES(nvtt ${LIBS} nvcore nvimage nvthread nvsquish bc6h bc7 nvmath rg_etc1)

INSTALL(TARGETS nvtt 
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static)

INSTALL(FILES nvtt.h nvtt_wrapper.h DESTINATION include/nvtt)


ADD_SUBDIRECTORY(tools)
ADD_SUBDIRECTORY(tests)
