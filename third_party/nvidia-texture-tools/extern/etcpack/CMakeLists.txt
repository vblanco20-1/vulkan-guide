
SET(ETCPACK_SRCS
    etcdec.cxx
    etcpack.cxx
    image.cxx
    image.h)

add_definitions("-DPGMOUT=0")
ADD_LIBRARY(etcpack STATIC ${ETCPACK_SRCS})

