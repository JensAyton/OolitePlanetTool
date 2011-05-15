/*
 *  FPMOpenEXR.h
 *  planettool
 *
 *  Created by Jens Ayton on 2011-05-15.
 *  Copyright 2011 the Oolite team. All rights reserved.
 *
 */


#ifndef INCLUDED_FPMOPENEXR_h
#define INCLUDED_FPMOPENEXR_h

#include "FloatPixMap.h"

FPM_BEGIN_EXTERN_C

typedef uint32_t FPMWriteOpenEXRFlags;	// None defined, use 0

typedef void FPMOpenEXRErrorHandler(const char *message, bool isError, void *callbackContext);


FloatPixMapRef FPMCreateWithOpenEXR(const char *path, FPMOpenEXRErrorHandler errorHandler, void *callbackContext);

bool FPMWriteOpenEXR(FloatPixMapRef pm, const char *path, FPMWriteOpenEXRFlags options, FPMOpenEXRErrorHandler errorHandler, void *callbackContext);

FPM_END_EXTERN_C
#endif	/* INCLUDED_FPMOPENEXR_h */
