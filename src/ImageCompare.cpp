
#include "stdafx.h"
#include <stdio.h>

#include "ParseParam.h"
#include "ImageProcess.h"
//#include "TextProcess.h"
#include "ScreenSnapshot.h"

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
void Compare(ArgValues InVal)
{
	ImageProcess ImgProc;
	int fileType0 = ImgProc.GetFileType(InVal.ImgName0);
	int fileType1 = ImgProc.GetFileType(InVal.ImgName0);
	if (fileType0 != fileType1) {
		printf("ERROR: Image type different.");
		return;
	}
	bool bLoad = false;
	if (fileType0 == FXQA_IMAGE_GIF || fileType0 == FXQA_IMAGE_TIFF) {
		int frameCount0 = -1;
		bLoad = ImgProc.LoadMulImageFI(InVal.ImgName0, InVal.ImgName1, NULL, &frameCount0, InVal.Color);
		if (!bLoad) {
			printf("ERROR: Frame count different.");
			return;
		}
		IplImage** frameImg0 = new IplImage*[frameCount0]; 
		bLoad = ImgProc.LoadMulImageFI(InVal.ImgName0, InVal.ImgName1, frameImg0, &frameCount0, InVal.Color);
		int frameCount1 = -1;
		bLoad = ImgProc.LoadMulImageFI(InVal.ImgName1, InVal.ImgName1, NULL, &frameCount1, InVal.Color);
		IplImage** frameImg1 = new IplImage*[frameCount1]; 
		bLoad = ImgProc.LoadMulImageFI(InVal.ImgName1, InVal.ImgName1, frameImg1, &frameCount1, InVal.Color);
		//ImgProc.ShowImage(frameImg1[1]);
		if (strncmp(InVal.CompareSize, "\n", 1) != 0)
			ImgProc.ReSetImageSize(InVal.CompareSize);
		int errFrameIndex = -1;
		double ImgSimilary = ImgProc.ImageMulCompare(frameImg0, frameImg1, frameCount0, &errFrameIndex);
		if (ImgSimilary < 1) {
			printf("%lf, frame %d", ImgSimilary, errFrameIndex);
		} else {
			printf("1.000000");
		}
		//ImgProc.ShowImage(pImg);
		IplImage* pImg = NULL;
		ImgProc.CombineImageDiff(frameImg0[errFrameIndex], frameImg1[errFrameIndex], &pImg);
		for (int i = 0; i < InVal.ActionFlag.size(); i++) {
			if (InVal.ImgOutName != '\0' && pImg != NULL && strncmp(InVal.ImgOutName, "\n", 1) != 0) {
				ImgProc.ImageOutPut(InVal.ImgOutName, pImg);
			}
			if (InVal.ShowImg == true) {
				ImgProc.ShowImage(pImg);
			}
		}
		cvReleaseImage(&pImg);
		for (int frameIndex = 0; frameIndex < frameCount0; frameIndex++) {
			cvReleaseImage(&frameImg0[frameIndex]);
			cvReleaseImage(&frameImg1[frameIndex]);
		}
		delete[] frameImg0;
		delete[] frameImg1;
		// 		ImgProc.ShowImage(frameImg[0]);
		// 		ImgProc.ShowImage(frameImg[9]);
		return;
	} else {
		bLoad = ImgProc.LoadImageInternalFI(InVal.ImgName0, InVal.ImgName1, InVal.Color);
		if (!bLoad) {
			return;
		}
		// Image compare
		if (strncmp(InVal.CompareSize, "\n", 1) != 0)
			ImgProc.ReSetImageSize(InVal.CompareSize);
		
		double ImgSimilary = ImgProc.ImageCompare(InVal.ErrLimit, InVal.AdjustOffset, InVal.bForceChangeSize);
		printf("%lf", ImgSimilary);

		IplImage* pImg = NULL;
		ImgProc.CombineInsideImageDiff(&pImg);
		//ImgProc.ShowImage(pImg);
		for (int i = 0; i < InVal.ActionFlag.size(); i++) {
			if (InVal.ImgOutName != '\0' && pImg != NULL && strncmp(InVal.ImgOutName, "\n", 1) != 0) {
				ImgProc.ImageOutPut(InVal.ImgOutName, pImg);
			}
			if (InVal.ShowImg == true) {
				ImgProc.ShowImage(pImg);
			}
		}
		cvReleaseImage(&pImg);
	}
#if defined(_PDF_PROCESS)
	else {											// File compare.
// 		PdfProcess* pdfer = new PdfProcess("E:/1.pdf");
// 		void* pdfBuf = NULL;
//		pdfer->GetRenderBuf(&pdfBuf);

		bool isEqual = false;
		FileProcess* FileProc = new FileProcess;
		bool ret = FileProc->FileIsEqualString(InVal.ImgName0, InVal.ImgName1, &isEqual);
		if (ret == false) {
			printf("Image not found, or file format error.\n");
			return;
		}
		char* difBuf0 = NULL;
		char* difBuf1 = NULL;
		int difPos = -1;
		ret = FileProc->GetDifferent(&difBuf0, &difBuf1, &difPos);
		
		if (isEqual == false) {
#ifdef _DEBUG
 			char difBuf2[11] ;
 			memcpy(difBuf2, difBuf0, 10);
 			char difBuf3[11] ;
 			memcpy(difBuf3, difBuf1, 10);
 			fwrite(difBuf2, 1, 10, stdout);
#endif
			printf("0.000000 Pos: %d\n", difPos);

		}
		ret = FileProc->PdfIsEqualRender(InVal.ImgName0, InVal.ImgName1, &isEqual);
		if (isEqual == true) {
			printf("1.000000\n");
		} else {
			char* difStr = NULL;
			//FileProc->GetDifferent(&difStr);
			for (int i = 0; i < InVal.ActionFlag.size(); i++) {
				if (InVal.ImgOutName != '\0') {
					ImgProc.ImageDiffBufOutPut(FileProc->m_curImgInfo0.width, FileProc->m_curImgInfo0.height, 
									FileProc->m_curImgInfo1.width, FileProc->m_curImgInfo1.height,
									FileProc->m_curImgInfo0.imgBuf, FileProc->m_curImgInfo1.imgBuf,
									InVal.ImgOutName);

				}
			}
		}
		delete FileProc;
	}
#endif // _PDF_PROCESS
}

/**
 * Compare Images after SrceenShot.
 * @ pImg0 from ScreenShot.
 * @ ImgName1 from local image file.
 */
void Compare(IplImage* pImg0, ArgValues InVal)
{
	ImageProcess ImgProc = ImageProcess();
	bool bLoad = ImgProc.LoadImageCV(pImg0, InVal.ImgName1, InVal.Color);
	if (bLoad) {
		if (InVal.CompareSize != "\n")
			ImgProc.ReSetImageSize(InVal.CompareSize);

		IplImage* pImg = NULL;
		double ImgSimilary = ImgProc.ImageCompare(InVal.ErrLimit, InVal.AdjustOffset);
		printf("%lf\n", ImgSimilary);
		ImgProc.CombineInsideImageDiff(&pImg);
		if (pImg != NULL) {
			if (InVal.ImgOutName != "\n")
				ImgProc.ImageOutPut(InVal.ImgOutName, pImg);
			if (InVal.ShowImg == true)
				ImgProc.ShowImage(pImg);
		}
	}
}

void Compare(IplImage* pImg0, IplImage* pImg1, ArgValues InVal)
{
	bool ret = false;
	bool bLoad = false;
	CvRect pdfRenderRange0, pdfRenderRange1;
	ImageProcess ImgProc = ImageProcess();

	// Get pdf render range when xx reader compare with adobe
	if (strcmp(InVal.ScreenShotSize, "\n") == 0) {
		if (strstr(InVal.ProgramVal, "classxxReader") != NULL
			|| strstr(InVal.ProgramVal, "classxxPhantom") != NULL
			|| strstr(InVal.ProgramVal, "AcrobatSDIWindow") != NULL) {
				IplImage* pdfImg0 = NULL;
				IplImage* pdfImg1 = NULL;

				ImgProc.GetPDFRenderRange(pImg0, &pdfRenderRange0);
				ImgProc.GetPDFRenderRange(pImg1, &pdfRenderRange1);

				ImgProc.GetPDFRenderImage(pImg0, pImg1, &pdfRenderRange0, &pdfRenderRange1, &pdfImg0, &pdfImg1);
#ifdef _DEBUG_OUTPUT_INFO_
 				DEBUG_SHOW_IMAGE(pdfImg0, pdfImg1);
				cvSaveImage("E:\\img0.png", pdfImg0);
				cvSaveImage("E:\\img1.png", pdfImg1);
#endif
				bLoad = ImgProc.LoadImageCV(pdfImg0, pdfImg1, InVal.Color);
		}
	} else {
		bLoad = ImgProc.LoadImageCV(pImg0, pImg1, InVal.Color);
	}
	
	if (bLoad) {
		_DEBUG_INFO("load image success.\n");

		if (InVal.CompareSize != "\n") {

			_DEBUG_INFO("reset image run.\n");
			ret = ImgProc.ReSetImageSize(InVal.CompareSize);
			_DEBUG_INFO("reset image end.\n");

			if (ret == false) return;
		}

		IplImage* pImg = NULL;

		_DEBUG_INFO("Compare image run.\n");
 		double ImgSimilary = ImgProc.ImageCompare(InVal.ErrLimit, InVal.AdjustOffset);
		printf("%lf\n", ImgSimilary);

		if (ImgSimilary != 0.00000000)
			ImgProc.CombineInsideImageDiff(&pImg);

		if (pImg != NULL) {
			if (InVal.ImgOutName != "\n")
				ImgProc.ImageOutPut(InVal.ImgOutName, pImg);
			if (InVal.ShowImg == true)
				ImgProc.ShowImage(pImg);
		} 
	
	} else {
		printf("load image failed.\n");
	}
}

// ScreenShot. use -n to set size.
IplImage* GetScreenImg(int argc, char **argv, const char* proClassName, ArgValues InVal)
{
#ifdef WIN32
	CWindowCtrl* winCtl = new CWindowCtrl;
	if (proClassName != NULL) {
		//winCtl->ShowTestWindow(proClassName);
		Sleep(1000);
	}
#endif // WIN32
	QApplication app(argc, argv);
	QtSnapshot MyQt;

#ifdef WIN32
	if (strcmp(InVal.ScreenShotSize, "\n") == 0)
		InVal.ScreenShotSize = winCtl->GetChildWindowSize();
#endif // WIN32

	bool bRet = MyQt.GrabWindow(InVal.ScreenShotSize);

	if (bRet == false) return NULL;

	//if (strcmp(InVal.ImgName0, "-c") == 0)
	IplImage* pImgGet = NULL;
	pImgGet = MyQt.GetIplImage();
	if (bRet && InVal.ImgOutName != "\n" 
		&& InVal.ActionFlag.size() == 1) {
		MyQt.SaveImage(InVal.ImgOutName);
	}
#ifdef _DEBUG_OUTPUT_INFO_
// 	string tem = "E:\\";
// 	tem = tem + proClassName;
// 	tem = tem + ".png";
// 	MyQt.SaveImage((char*)tem.c_str());
#endif
#ifdef WIN32
	if (proClassName != NULL) {
		//winCtl->HideTestWindow(proClassName);
	}
	delete winCtl;
#endif // WIN32
	return pImgGet;
}