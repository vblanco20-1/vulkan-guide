PROJECT(nvsquish)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

SET(SQUISH_SRCS
	fastclusterfit.cpp
	fastclusterfit.h
	weightedclusterfit.cpp
	weightedclusterfit.h
	colourblock.cpp
	colourblock.h
	colourfit.cpp
	colourfit.h
	colourset.cpp
	colourset.h
	config.h
	maths.cpp
	maths.h
	simd.h
	simd_sse.h
	simd_ve.h)

ADD_LIBRARY(nvsquish STATIC ${SQUISH_SRCS})
