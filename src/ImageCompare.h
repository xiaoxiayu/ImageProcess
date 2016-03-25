#ifndef _IMAGE_COMPARE_H_
#define _IMAGE_COMPARE_H_

#include "stdafx.h"
#include <stdio.h>

#include "ImageProcess.h"
#if defined(_PDF_PROCESS)
#include "PdfProcess.h"
#endif // _PDF_PROCESS

#ifdef _WIN32
#include "WindowsHND.h"
#endif

/**
 * Compare Images from input, Images are usually already well on the local storage.
 * @ ImgName0 and ImgName1 in InVal from input argv.
 */
void Compare(ArgValues InVal);

/**
 * Compare Images after SrceenShot.
 * @ pImg0 from ScreenShot.
 * @ ImgName1 from local image file.
 */
void Compare(IplImage* pImg0, ArgValues InVal);

void Compare(IplImage* pImg0, IplImage* pImg1, ArgValues InVal);

// ScreenShot. use -n to set size.
IplImage* GetScreenImg(int argc, char **argv, const char* proClassName, ArgValues InVal);

#endif