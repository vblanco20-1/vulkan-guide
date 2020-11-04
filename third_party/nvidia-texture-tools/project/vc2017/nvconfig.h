#ifndef NV_CONFIG
#define NV_CONFIG

//#cmakedefine HAVE_UNISTD_H
//#define HAVE_STDARG_H
//#cmakedefine HAVE_SIGNAL_H
//#cmakedefine HAVE_EXECINFO_H
//#define HAVE_MALLOC_H

#if defined(_OPENMP)
#define HAVE_OPENMP
#endif

#define NV_HAVE_STBIMAGE
/*#if !defined(_M_X64)
//#define NV_HAVE_FREEIMAGE
#define NV_HAVE_PNG
#define NV_HAVE_JPEG
#define NV_HAVE_TIFF
#endif*/

#endif // NV_CONFIG
