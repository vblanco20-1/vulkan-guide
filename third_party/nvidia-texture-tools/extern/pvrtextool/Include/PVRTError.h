/******************************************************************************

 @File         PVRTError.h

 @Title        PVRTError

 @Version      

 @Copyright    Copyright (c) Imagination Technologies Limited. All Rights Reserved. Strictly Confidential.

 @Platform     ANSI compatible

 @Description  

******************************************************************************/
#ifndef _PVRTERROR_H_
#define _PVRTERROR_H_

#if defined(ANDROID)
	#include <android/log.h>
#else
	#if defined(_WIN32)
		#include <windows.h>
	#else
		#include <stdio.h>
	#endif
#endif
/*!***************************************************************************
 Macros
*****************************************************************************/

/*! Outputs a string to the standard error if built for debugging. */
#if !defined(PVRTERROR_OUTPUT_DEBUG)
	#if defined(_DEBUG) || defined(DEBUG)
		#if defined(ANDROID)
			#define PVRTERROR_OUTPUT_DEBUG(A) __android_log_print(ANDROID_LOG_INFO, "PVRTools", A);
		#elif defined(_WIN32)
			#define PVRTERROR_OUTPUT_DEBUG(A) OutputDebugStringA(A);
		#else
			#define PVRTERROR_OUTPUT_DEBUG(A) fprintf(stderr,A);
		#endif
	#else
		#define PVRTERROR_OUTPUT_DEBUG(A)
	#endif
#endif


/*!***************************************************************************
 Enums
*****************************************************************************/
/*! Enum error codes */
enum EPVRTError
{
	PVR_SUCCESS = 0,
	PVR_FAIL = 1,
	PVR_OVERFLOW = 2
};

/*!***************************************************************************
 @Function			PVRTErrorOutputDebug
 @Input				format		printf style format followed by arguments it requires
 @Description		Outputs a string to the standard error.
*****************************************************************************/
void PVRTErrorOutputDebug(char const * const format, ...);

#endif // _PVRTERROR_H_

/*****************************************************************************
End of file (PVRTError.h)
*****************************************************************************/

