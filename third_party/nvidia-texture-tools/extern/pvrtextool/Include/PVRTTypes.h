/******************************************************************************

 @File         PVRTTypes.h

 @Title        PVRTTypes

 @Version      

 @Copyright    Copyright (c) Imagination Technologies Limited. All Rights Reserved. Strictly Confidential.

 @Platform     ANSI compatible

 @Description  Global enums and typedefs for PVRTools

******************************************************************************/

//ACS: I split this out of PVRTGlobal.h

#ifndef _PVRTTYPES_H_
#define _PVRTTYPES_H_

/*!***************************************************************************
 Macros
*****************************************************************************/
//#include <stdio.h>

// This macro is used to check at compile time that types are of a certain size
// If the size does not equal the expected size, this typedefs an array of size 0
// which causes a compile error
#define PVRTSIZEASSERT(T, size) typedef int (sizeof_##T)[sizeof(T) == (size)]
#define PVRTCOMPILEASSERT(T, expr)	typedef int (assert_##T)[expr]


/****************************************************************************
** Integer types
****************************************************************************/

typedef char				PVRTchar8;
typedef signed char			PVRTint8;
typedef signed short		PVRTint16;
typedef signed int			PVRTint32;
typedef unsigned char		PVRTuint8;
typedef unsigned short		PVRTuint16;
typedef unsigned int		PVRTuint32;

typedef float				PVRTfloat32;

#if (defined(__int64) || defined(_WIN32))
typedef signed __int64     PVRTint64;
typedef unsigned __int64   PVRTuint64;
#elif defined(TInt64)
typedef TInt64             PVRTint64;
typedef TUInt64            PVRTuint64;
#else
typedef signed long long   PVRTint64;
typedef unsigned long long PVRTuint64;
#endif

#if __SIZEOF_WCHAR_T__  == 4 || __WCHAR_MAX__ > 0x10000
	#define PVRTSIZEOFWCHAR 4
#else
	#define PVRTSIZEOFWCHAR 2
#endif

PVRTSIZEASSERT(PVRTchar8,   1);
PVRTSIZEASSERT(PVRTint8,   1);
PVRTSIZEASSERT(PVRTuint8,  1);
PVRTSIZEASSERT(PVRTint16,  2);
PVRTSIZEASSERT(PVRTuint16, 2);
PVRTSIZEASSERT(PVRTint32,  4);
PVRTSIZEASSERT(PVRTuint32, 4);
PVRTSIZEASSERT(PVRTint64,  8);
PVRTSIZEASSERT(PVRTuint64, 8);
PVRTSIZEASSERT(PVRTfloat32, 4);

/*!**************************************************************************
@Enum   ETextureFilter
@Brief  Enum values for defining texture filtering
****************************************************************************/
enum ETextureFilter
{
	eFilter_Nearest,
	eFilter_Linear,
	eFilter_None,

	eFilter_Size,
	eFilter_Default		= eFilter_Nearest,
	eFilter_MipDefault	= eFilter_None
};

/*!**************************************************************************
@Enum   ETextureWrap
@Brief  Enum values for defining texture wrapping
****************************************************************************/
enum ETextureWrap
{
	eWrap_Clamp,
	eWrap_Repeat,

	eWrap_Size,
	eWrap_Default = eWrap_Repeat
};

/*****************************************************************************
 ACS: Handle missing assert macros.
      Maybe you needed to include PVRTGlobal.h after all?
*****************************************************************************/
#ifndef _ASSERT
# define _ASSERT(X)
#endif
#ifndef _ASSERTE
# define _ASSERTE(X)
#endif

#endif // _PVRTTYPES_H_

/*****************************************************************************
 End of file
*****************************************************************************/

