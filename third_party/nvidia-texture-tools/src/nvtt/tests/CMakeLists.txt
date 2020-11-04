
ADD_EXECUTABLE(filtertest filtertest.cpp ../tools/cmdline.h)
TARGET_LINK_LIBRARIES(filtertest nvcore nvmath nvimage nvtt)

ADD_EXECUTABLE(nvtestsuite testsuite.cpp)
TARGET_LINK_LIBRARIES(nvtestsuite nvcore nvmath nvimage nvtt)
ADD_TEST(NVTT.TestSuite.Kodak.cuda nvtestsuite -path ${NV_SOURCE_DIR}/data/testsuite -set 0 -out output-cuda-kodak)
ADD_TEST(NVTT.TestSuite.Waterloo.cuda nvtestsuite -path ${NV_SOURCE_DIR}/data/testsuite -set 1 -out output-cuda-waterloo)
ADD_TEST(NVTT.TestSuite.Epic.cuda nvtestsuite -path ${NV_SOURCE_DIR}/data/testsuite -set 2 -out output-cuda-epic)
ADD_TEST(NVTT.TestSuite.Kodak.nocuda nvtestsuite -path ${NV_SOURCE_DIR}/data/testsuite -set 0 -nocuda -out output-nocuda-kodak)
ADD_TEST(NVTT.TestSuite.Waterloo.nocuda nvtestsuite -path ${NV_SOURCE_DIR}/data/testsuite -set 1 -nocuda -out output-nocuda-waterloo)
ADD_TEST(NVTT.TestSuite.Epic.nocuda nvtestsuite -path ${NV_SOURCE_DIR}/data/testsuite -set 2 -nocuda -out output-nocuda-epic)

IF (CUDA_FOUND)
    ADD_EXECUTABLE(driverapitest driverapi.cpp)
    TARGET_LINK_LIBRARIES(driverapitest nvcore nvmath nvimage nvtt)
ENDIF (CUDA_FOUND)

ADD_EXECUTABLE(imperativeapi imperativeapi.cpp)
TARGET_LINK_LIBRARIES(imperativeapi nvcore nvmath nvimage nvtt)

ADD_EXECUTABLE(process_alpha_map process_alpha_map.cpp)
TARGET_LINK_LIBRARIES(process_alpha_map nvcore nvmath nvimage nvtt)

ADD_EXECUTABLE(cubemaptest cubemaptest.cpp)
TARGET_LINK_LIBRARIES(cubemaptest nvcore nvmath nvimage nvtt)

ADD_EXECUTABLE(nvhdrtest hdrtest.cpp)
TARGET_LINK_LIBRARIES(nvhdrtest nvcore nvimage nvtt bc6h nvmath)

#ADD_EXECUTABLE(bc1enc bc1enc.cpp)
#TARGET_LINK_LIBRARIES(bc1enc nvcore nvimage nvmath squish CMP_Core)

INSTALL(TARGETS nvtestsuite nvhdrtest DESTINATION bin)
 
#include_directories("/usr/include/ffmpeg/")
#ADD_EXECUTABLE(nvmpegenc tools/mpegenc.cpp tools/cmdline.h)
#TARGET_LINK_LIBRARIES(nvmpegenc nvcore nvmath nvimage avcodec z)
#INSTALL(TARGETS nvmpegenc DESTINATION bin)

