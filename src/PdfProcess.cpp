#include "PdfProcess.h"


FileProcess::FileProcess()
{
	m_fp0 = NULL;
	m_fp1 = NULL;
	m_difPos = -1;
}

FileProcess::~FileProcess()
{
	if (m_fp0 != NULL) {
		fclose(m_fp0);
		m_fp0 = NULL;
	}
	if (m_fp1 != NULL) {
		fclose(m_fp1);
		m_fp1 = NULL;
	}
}

bool FileProcess::FileIsEqualString(char* filePath0, char* filePath1, bool* isEqual)
{
	string strFilePath = filePath0;
	string strSuffix = GET_STRINGFILE_SUFFIX(strFilePath);

	if (strSuffix.compare("pdf") != 0) return false;

	strFilePath = filePath1;
	strSuffix = GET_STRINGFILE_SUFFIX(strFilePath);

	if (strSuffix.compare("pdf") != 0) return false;

	char buf1[BUFSIZE];
	char buf2[BUFSIZE];
	m_fp0 = fopen(filePath0, "rb");
	m_fp1 = fopen(filePath1, "rb");

	if (m_fp1 == NULL || m_fp0 == NULL) return false;

	int fileSize0 = fseek(m_fp0, 0, SEEK_END);
	fileSize0 = ftell(m_fp0);
	
	int fileSize1 = fseek(m_fp1, 0, SEEK_END);
	fileSize1 = ftell(m_fp1);
	
	//if (fileSize0 != fileSize1) return false;

	fseek(m_fp1, 0, SEEK_SET);
	fseek(m_fp0, 0, SEEK_SET);

	do {
		size_t r1 = fread(buf1, 1, BUFSIZE, m_fp0);
		size_t r2 = fread(buf2, 1, BUFSIZE, m_fp1);

		if (r1 != r2 || memcmp(buf1, buf2, r1)) {
			m_difPos = ftell(m_fp0);
			*isEqual = false;
			return true;					// only get first different.
		}
	} while (!feof(m_fp0) || !feof(m_fp1));

	*isEqual = true;
	return true;
}

bool FileProcess::PdfIsEqualRender(char* pdfPath0, char* pdfPath1, bool* isEqual)
{
	PdfProcess* pdf0 = new PdfProcess(pdfPath0);
	PdfProcess* pdf1 = new PdfProcess(pdfPath1);

	int ret0 = pdf0->RenderPdf();
	int ret1 = pdf1->RenderPdf();
	if (ret0 == false || ret1 == false) {
		delete pdf0;
		delete pdf1;
		return false;
	}

	int pageCnt0 = pdf0->GetPageCount();
	int pageCnt1 = pdf1->GetPageCount();
	if (pageCnt0 != pageCnt1) {
		delete pdf0;
		delete pdf1;
		return false;
	}
	
	for (int pageIndex = 0; pageIndex < pageCnt0; pageIndex++) {
		void* buf0 = NULL;
		void* buf1 = NULL;
		int bufSize0 = -1, bufSize1 = -1;
		pdf0->GetPageRenderBuf(pageIndex, &buf0, &bufSize0);
		pdf1->GetPageRenderBuf(pageIndex, &buf1, &bufSize1);

		int ret = CompareBuf(buf0, buf1, bufSize0);
		if (ret == false) {
			printf("page_index:%d,Render compare failed.", pageIndex);
			m_curImgInfo0 = pdf0->m_mapImgInfo[pageIndex];
			m_curImgInfo1 = pdf1->m_mapImgInfo[pageIndex];
			*isEqual = false;
			return true;
		}
	}

	*isEqual = true;
	return true;
}

bool FileProcess::GetDifferent(char** difStr0, char** difStr1, int* difPos)
{
	if (m_fp0 == NULL || m_fp1 == NULL) return false;  
	//fseek(m_fp0, 0, SEEK_SET);
	fseek(m_fp0, m_difPos - BUFSIZE, SEEK_SET);
	fseek(m_fp1, m_difPos - BUFSIZE, SEEK_SET);

	char difBuf0[BUFSIZE];
	char difBuf1[BUFSIZE];
	memset(difBuf0, 0, BUFSIZE);
	memset(difBuf1, 0, BUFSIZE);

	fread(difBuf0, 1, BUFSIZE, m_fp0);
	fread(difBuf1, 1, BUFSIZE, m_fp1);
	difBuf0[BUFSIZE - 1] = 0;
	difBuf1[BUFSIZE - 1] = 0;

	*difStr0 = difBuf0;
	*difStr1 = difBuf1;
	*difPos = m_difPos - BUFSIZE;

	return true;
}

bool FileProcess::CompareBuf(void* buf0, void* buf1, int sizeBuf0)
{
	int ret = memcmp(buf0, buf1, sizeBuf0);
	if (ret != 0) return false;
	return true;
}

PdfProcess::PdfProcess()
{
	m_mem = new QA_CTest_All_Util;
	m_mem->MemoryInit_GSDK3();

	m_gsdk = new FXGSDK3;
}

PdfProcess::PdfProcess(char* pdfPath)
{
	m_mem = new QA_CTest_All_Util;
	m_mem->MemoryInit_GSDK3();
	m_mem->Unlock_GSDK3();
	m_gsdk = new FXGSDK3;

	m_gsdk->LoadDocument(pdfPath);
}

PdfProcess::~PdfProcess()
{
	m_gsdk->CloseDoc();
	delete m_gsdk;

	m_mem->DestroyLib_GSDK3();
	delete m_mem;
}

bool PdfProcess::RenderPdf()
{
	int ret = m_gsdk->CountPages();
	
	for (int i = 0; i < m_gsdk->m_pageCount; i++) {
		ret = m_gsdk->LoadPage(i);
		if (ret < 0) {
			printf("FXGSDK load page %d failed.", i);
			return false;
		}
		m_gsdk->Render();
		m_gsdk->SaveBmpToFile("E:/1.bmp");
		void* buf = NULL;
		int sizeImage = -1;
		IMAGE_INFO imgInfo;

		m_gsdk->GetBitmapInfo(&imgInfo.width, &imgInfo.height, &imgInfo.planes,
							&imgInfo.xPelsPerMeter, &imgInfo.yPelsPerMeter,
							&imgInfo.bitCount, &imgInfo.sizeImage, &imgInfo.imgBuf);

		m_mapImgInfo[i] = imgInfo;

		m_gsdk->ClosePage();
	}
	return true;
}

bool PdfProcess::GetPageRenderBuf(int pageIndex, void** buf, int* sizeBuf)
{
 	*buf = m_mapImgInfo[pageIndex].imgBuf;
 	*sizeBuf = m_mapImgInfo[pageIndex].sizeImage;
	return true;
}