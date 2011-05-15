/*
 *  FPMOpenEXR.cpp
 *  planettool
 *
 *  Created by Jens Ayton on 2011-05-15.
 *  Copyright 2011 the Oolite team. All rights reserved.
 *
 */

#include "FPMOpenEXR.h"
#include "FPMImageOperations.h"

#include <ImfInputFile.h>
#include <ImfOutputFile.h>
#include <ImfChannelList.h>
#include <cassert>

using namespace std;
using namespace Imf;
using namespace Imath;


FloatPixMapRef FPMCreateWithOpenEXR(const char *path, FPMOpenEXRErrorHandler errorHandler, void *callbackContext)
{
	FloatPixMapRef pm = NULL;
	
	assert(sizeof(FPMColor) == 4 * sizeof(float));
	
	try
	{
		InputFile file(path);
		Header header = file.header();
		Box2i dataWindow = header.dataWindow();
		
		// FIXME: crop to display window.
		int width  = dataWindow.max.x - dataWindow.min.x + 1;
		int height = dataWindow.max.y - dataWindow.min.y + 1;
		
		pm = FPMCreateC(width, height);
		if (pm == NULL)
		{
			if (errorHandler != NULL)  errorHandler("Not enough memory.", true, callbackContext);
			return NULL;
		}
		
		FPMColor *basePx = FPMGetBufferPointer(pm);
		size_t xStride = sizeof *basePx;
		size_t yStride = FPMGetRowByteCount(pm);
		
		FrameBuffer buffer;
		buffer.insert("R",
					  Slice(FLOAT,
							(char *)&basePx->r,
							xStride,
							yStride,
							1, 1,
							0.0));
		
		buffer.insert("G",
					  Slice(FLOAT,
							(char *)&basePx->g,
							xStride,
							yStride,
							1, 1,
							0.0));
		
		buffer.insert("B",
					  Slice(FLOAT,
							(char *)&basePx->b,
							xStride,
							yStride,
							1, 1,
							0.0));
		
		buffer.insert("A",
					  Slice(FLOAT,
							(char *)&basePx->a,
							xStride,
							yStride,
							1, 1,
							1.0));
		
		file.setFrameBuffer(buffer);
		file.readPixels(dataWindow.min.y, dataWindow.max.y);
	}
	catch (std::exception &e)
	{
		if (errorHandler != NULL)  errorHandler(e.what(), true, callbackContext);
		FPMRelease(&pm);
	}
	catch (...)
	{
		if (errorHandler != NULL)  errorHandler("Unknown error", true, callbackContext);
		FPMRelease(&pm);
	}
	
	return pm;
}


bool FPMWriteOpenEXR(FloatPixMapRef pm, const char *path, FPMWriteOpenEXRFlags options, FPMOpenEXRErrorHandler errorHandler, void *callbackContext)
{
	bool success = false;
	if (pm == NULL)  return false;
	
	try
	{
		FPMDimension width = FPMGetWidth(pm), height = FPMGetHeight(pm);
		Header header(width, height);
		
		ChannelList &channels = header.channels();
		
		FPMColor min, max;
		FPMFindExtremes(pm, &min, &max);
		bool haveAlpha = (min.a != 1.0 || max.a != 1.0);
		
		FPMColor *basePx = FPMGetBufferPointer(pm);
		size_t xStride = sizeof *basePx;
		size_t yStride = FPMGetRowByteCount(pm);
		
		channels.insert("R", Channel(FLOAT));
		FrameBuffer buffer;
		buffer.insert("R",
					  Slice(FLOAT,
							(char *)&basePx->r,
							xStride,
							yStride,
							1, 1,
							0.0));
		
		channels.insert("G", Channel(FLOAT));
		buffer.insert("G",
					  Slice(FLOAT,
							(char *)&basePx->g,
							xStride,
							yStride,
							1, 1,
							0.0));
		
		channels.insert("B", Channel(FLOAT));
		buffer.insert("B",
					  Slice(FLOAT,
							(char *)&basePx->b,
							xStride,
							yStride,
							1, 1,
							0.0));
		
		if (haveAlpha)
		{
			channels.insert("A", Channel(FLOAT, 1, 1, true));
			buffer.insert("A",
						  Slice(FLOAT,
								(char *)&basePx->a,
								xStride,
								yStride,
								1, 1,
								1.0));
		}
		
		OutputFile file(path, header);
		file.setFrameBuffer(buffer);
		file.writePixels(height);
	}
	catch (std::exception &e)
	{
		if (errorHandler != NULL)  errorHandler(e.what(), true, callbackContext);
		success = false;
	}
	catch (...)
	{
		if (errorHandler != NULL)  errorHandler("Unknown error", true, callbackContext);
		success = false;
	}
	
	return success;
}
