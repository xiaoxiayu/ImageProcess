#ifndef _PDFPROCESS_H_
#define _PDFPROCESS_H_

#include "stdafx.h"
#include "FX_GSDK.h"
#include <map>

#define BUFSIZE 10
using namespace std;

typedef struct _IMAGE_INFO_
{
	/** @brief	The width of the bitmap, in pixels. */	
	int		width;
	/** @brief	The height of the bitmap, in pixels. */	
	int		height;
	/** @brief	The number of planes for the target device. This value must be set to 1. */	
	int		planes;
	/** @brief	The number of bits per pixel (bpp). */	
	int		bitCount;
	/** @brief	The type of compression for a compressed bottom-up bitmap (top-down DIBs cannot be compressed). */	
	int		compression;
	/** @brief	The size, of the image, in bytes. */	
	int		sizeImage;
	/** @brief	The horizontal resolution, in pixels per meter, of the target device for the bitmap. */	
	int		xPelsPerMeter;
	/** @brief	The vertical resolution, in pixels per meter, of the target device for the bitmap. */	
	int		yPelsPerMeter;
	/** @brief	The number of color indices in the color table which are actually used by the bitmap. */	
	int		clrUsed;
	/** 
	 * @brief	The number of color indices which are considered important for displaying the bitmap. 
	 * 
	 * @details	If this value is zero, all colors are important. 
	 */	
	int		clrImportant;
	void*	imgBuf;
} IMAGE_INFO;

class PdfProcess;

class FileProcess
{
public:
	FileProcess();
	~FileProcess();

	bool			FileIsEqualString(char* filePath0, char* filePath1, bool* isEqual);
	bool			PdfIsEqualRender(char* pdfPath0, char* pdfPath1, bool* isEqual);
	bool			GetDifferent(char** difStr0, char** difStr1, int* difPos);

private:
	bool			CompareBuf(void* buf0, void* buf1, int sizeBuf0);

public:
	PdfProcess*		m_pdf0;
	PdfProcess*		m_pdf1;
	IMAGE_INFO		m_curImgInfo0;
	IMAGE_INFO		m_curImgInfo1;

private:
	FILE*			m_fp0;
	FILE*			m_fp1;
	int				m_difPos;
	int				m_bufSize;
};

class PdfProcess
{
public:
	PdfProcess();
	PdfProcess(char* pdfPath);
	~PdfProcess();

	bool			RenderPdf();
	bool			GetPageCount() {return m_gsdk->m_pageCount; return true;};
	bool			GetPageRenderBuf(int pageIndex, void** buf, int* sizeBuf);
public:
	map<int, IMAGE_INFO>			m_mapImgInfo;

private:
	QA_CTest_All_Util*		m_mem;
	FXGSDK3*				m_gsdk;
};
#endif // _PDFPROCESS_H_