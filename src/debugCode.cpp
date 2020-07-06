// FirstTest.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <stdio.h>

#include "ParseParam.h"
#include "ImageProcess.h"
#include "TextProcess.h"
#include "QtSnapshot.h"


#ifdef _WIN32
#include "WindowsHND.h"
#endif

/**
 * Compare Images from input, Images are usually already well on the local storage.
 * @ ImgName0 and ImgName1 in InVal from input argv.
 */
void Compare(ArgValues InVal)
{
	ImageProcess ImgProc = ImageProcess();
	bool bLoad = ImgProc.LoadImageCV(InVal.ImgName0, InVal.ImgName1, InVal.Color);
	if (bLoad) {
        if (strcmp(InVal.CompareSize, "\n") != 0)
			ImgProc.ReSetImageSize(InVal.CompareSize);

		IplImage* pImg = NULL;
		double ImgSimilary = ImgProc.ImageCompare(InVal.ErrLimit, InVal.AdjustOffset);
 		printf("%lf\n", ImgSimilary);

 		ImgProc.CombineInsideImageDiff(&pImg);

 		for (int i = 0; i < InVal.ActionFlag.size(); i++) {
 			if (InVal.ImgOutName != '\0') {
 				ImgProc.ImageOutPut(InVal.ImgOutName, pImg);
 			}
 			if (InVal.ShowImg == true) {
 				ImgProc.ShowImage(pImg);
 			}
 		}
	}
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
		if (strlen(strstr(InVal.ProgramVal, "classxxReader")) > 0
			|| strlen(strstr(InVal.ProgramVal, "classxxPhantom")) > 0
			|| strlen(strstr(InVal.ProgramVal, "AcrobatSDIWindow")) > 0) {
				IplImage* pdfImg0 = NULL;
				IplImage* pdfImg1 = NULL;

				ImgProc.GetPDFRenderRange(pImg0, &pdfRenderRange0);
				ImgProc.GetPDFRenderRange(pImg1, &pdfRenderRange1);

				ImgProc.GetPDFRenderImage(pImg0, pImg1, &pdfRenderRange0, &pdfRenderRange1, &pdfImg0, &pdfImg1);

//  				IplImage* temImg = cvCreateImage(cvSize(500, 600), pdfImg0->depth, pdfImg0->nChannels);
//  				IplImage* temImg1 = cvCreateImage(cvSize(500, 600), pdfImg1->depth, pdfImg1->nChannels);
//  				cvResize(pdfImg1, temImg);
//  				cvResize(pdfImg0, temImg1);
// 				DEBUG_SHOW_IMAGE(pdfImg0, pdfImg1);
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
	CWindowCtrl* winCtl = new CWindowCtrl;
	winCtl->ShowTestWindow(proClassName);
	Sleep(1000);

	QApplication app(argc, argv);
	QtSnapshot MyQt;
	
	if (strcmp(InVal.ScreenShotSize, "\n") == 0)
		InVal.ScreenShotSize = winCtl->GetChildWindowSize();

	bool bRet = MyQt.GrabWindow(InVal.ScreenShotSize);

	//if (strcmp(InVal.ImgName0, "-c") == 0)
	IplImage* pImgGet = NULL;
	pImgGet = MyQt.QtImgToIplImage();
	if (bRet && InVal.ImgOutName != "\n" 
		&& InVal.ActionFlag.size() == 1) {
		MyQt.SaveImage(InVal.ImgOutName);
	}
//#ifdef _DEBUG_OUTPUT_INFO_
	string tem = "E:\\";
	tem = tem + proClassName;
	tem = tem + ".png";
	MyQt.SaveImage((char*)tem.c_str());
//#endif
	winCtl->HideTestWindow(proClassName);
	delete winCtl;
	return pImgGet;
}

///////////////////////////////////////////////////////////////////////////////////////////
//just for test.
///////////////////////////////////////////////////////////////////////////////////////////

void myTest2()
{
	IplImage* src; 
	CvMoments moments; 
	CvMat *region; 
	CvPoint pt1,pt2; 
	double m00 = 0, m10, m01, mu20, mu11, mu02, inv_m00; 
	double a, b, c; 
	int xc, yc; 
	// 第一条命令行参数确定了图像的文件名。 
	if( (src=cvLoadImage("E:\\pImg0.png", 0))!= 0) 
		//if( (src=cvLoadImage("fbb.jpg", 0))!= 0) 
		//if( argc == 2 && (src=cvLoadImage(argv[1], 0))!= 0) 
	{ 
		IplImage* dst = cvCreateImage( cvGetSize(src), 8, 3 ); 
		CvMemStorage* storage = cvCreateMemStorage(0); 
		CvSeq* contour = 0; 

		cvNamedWindow( "Source", 1 ); 
		cvShowImage( "Source", src ); 
		cvWaitKey(0);

		cvThreshold( src, src, 254, 255, CV_THRESH_BINARY );//100 is the thredhold 
		cvNot( src, src ); 
		
		cvShowImage( "Source", src );
		cvWaitKey(0);
		//cvFindContours( src, storage, &contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE,cvPoint(0,0) ); 

		cvFindContours( src, storage, &contour, sizeof(CvContour),CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) ); 
		cvZero( dst ); 

		for( ; contour != 0; contour = contour->h_next ) 
		{ 
			//CvScalar color = CV_RGB( rand()&255, rand()&255, rand()&255 ); 
			CvScalar color = CV_RGB( 255, 0,0 ); 
			/* 用1替代 CV_FILLED 所指示的轮廓外形 */ 
			cvDrawContours( dst, contour, color, color, -1, CV_FILLED, 8,cvPoint(0,0) );//you can change 1 to CV_FILLED 
			contour = cvApproxPoly( contour, sizeof(CvContour), storage, CV_POLY_APPROX_DP, 3, 1 ); 
			//CvRect* r = (CvRect*)cvGetSeqElem( contour,1); 
			region=(CvMat*)contour; 
			cvMoments( region, &moments,0 ); 
			//cvMoments( &contour, &moments,0 ); 
			// cvDrawContours( cnt_img, _contours, CV_RGB(255,0,0), CV_RGB(0,255,0), _levels, 3, CV_AA, cvPoint(0,0) ); CV_FILLED 
			//////////////////////////////////////////////// 
			///////////////////////////////////////////////// 
			m00 = moments.m00; 
			m10 = moments.m10; 
			m01 = moments.m01; 
			mu11 = moments.mu11; 
			mu20 = moments.mu20; 
			mu02 = moments.mu02; 

			//if( fabs(m00) < DBL_EPSILON )break; 

			inv_m00 = 1. / m00; 
			xc = cvRound( m10 * inv_m00 ); 
			yc = cvRound( m01 * inv_m00 ); 
			a = mu20 * inv_m00; 
			b = mu11 * inv_m00; 
			c = mu02 * inv_m00; 
			///////////////// 
			pt1.x=xc-1;pt1.y=yc; 
			pt2.x=xc+1;pt2.y=yc; 
			cvLine( dst, pt1, pt2, CV_RGB(0,255,0), 2, CV_AA, 0 ); 
			pt1.x=xc;pt1.y=yc-1; 
			pt2.x=xc;pt2.y=yc+1; 
			cvLine( dst, pt1, pt2, CV_RGB(0,255,0), 2, CV_AA, 0 ); 

//  			string pointStr = "x: ";
//  			char* temStr = new char[7];
//  			memset(temStr, 0, 7);
//  			sprintf(temStr, "%d", pt1.x);
//  			pointStr += temStr;
//  
//  			pointStr += " y: ";
//  			memset(temStr, 0, 7);
//  			sprintf(temStr, "%d", pt1.y);
//  			pointStr += temStr;
//  			
//  			delete[] temStr;
 			//Mat img(600, 800, CV_8UC3, Scalar::all(0));
 	//		putText(Mat(dst), pointStr, pt1, FONT_HERSHEY_DUPLEX, 2, CV_RGB(0,255,0));
		} 

		cvNamedWindow( "Components", 1 ); 
		cvShowImage( "Components", dst ); 
		cvWaitKey(0); 
}
}
IplImage* g_pGrayImage;
IplImage* g_pBinaryImage;
CvSeq *g_pcvSeq = NULL;  
#define _TEM_TEST
void on_trackbar(int pos)  
{  
	// 转为二值图  
	//cvThreshold(g_pGrayImage, g_pBinaryImage, pos, 255, CV_THRESH_BINARY);  
// 	cvCanny(g_pGrayImage, g_pBinaryImage, pos, pos * 3, 3);
// 	// 显示二值图  
// 	cvShowImage("mytest_after", g_pBinaryImage);  
	//IplImage *pBinaryImage = cvCreateImage(cvGetSize(g_pGrayImage), IPL_DEPTH_8U, 1);  
//	cvThreshold(g_pGrayImage, pBinaryImage, pos, 255, CV_THRESH_BINARY);  
	// 显示二值图  
//	cvShowImage("mytest", pBinaryImage);  

// 	CvMemStorage* cvMStorage = cvCreateMemStorage();  
// 	// 检索轮廓并返回检测到的轮廓的个数  
#ifdef _TEM_TEST

 	IplImage *pOutlineImage = cvCreateImage(cvGetSize(g_pGrayImage), IPL_DEPTH_8U, 3);  
#endif
	int img_w = g_pGrayImage->width;
	int img_h = g_pGrayImage->height;

	int n = 0;
	int x_max = 0, y_max = 0;
	int x_min = img_w, y_min = img_h;

	IplImage *pBinaryImage = cvCreateImage(cvGetSize(g_pGrayImage), IPL_DEPTH_8U, 1); 
	CvMemStorage* cvMStorage = cvCreateMemStorage();  

	cvThreshold(g_pGrayImage, pBinaryImage, pos, 255, CV_THRESH_BINARY);

#ifndef _TEM_TEST
	for (int i = 90; i < 200; i += 10) {
		cvThreshold(g_pGrayImage, pBinaryImage, i, 255, CV_THRESH_BINARY); 
#endif	
		// 检索轮廓并返回检测到的轮廓的个数  
		int ctoCnt = cvFindContours(pBinaryImage,cvMStorage, &g_pcvSeq, 88, CV_RETR_LIST );  

		for (CvSeq* c=g_pcvSeq;c!=NULL;c=c->h_next) {
#ifdef _TEM_TEST

			cvDrawContours(
				pOutlineImage,
				c,
				CV_RGB(255,0,0),
				CV_RGB(0,255,0),
				0,
				1,
				8
				);
			CvRect tem = cvBoundingRect(c);
			printf("Contour # %d\n",n);
			cvShowImage("mytest_after",pOutlineImage);
			printf("%d elements:\n",c->total);
		
#endif
			for (int i=0;i<c->total;++i)
			{
				CvPoint* p = CV_GET_SEQ_ELEM(CvPoint,c,i);

				if (x_min > p->x) {
					if (p->x > 10)
						x_min = p->x;
				}
				if (y_min > p->y) {
					if (p->y > 1)
						y_min = p->y;
				}
				if (x_max < p->x) {
					if (p->x < (img_w - 10))
						x_max = p->x;
				}
				if (y_max < p->y) {
					if (p->y < (img_h - 1))
						y_max = p->y;
				}
			//	printf("%d, %d, %d, %d \n", x_min, y_min, x_max, y_max);
			}
			n++;
		}
#ifndef _TEM_TEST
	}
#endif
//  	cvSetImageROI(g_pGrayImage, cvRect(x_min, y_min, x_max - x_min, y_max - y_min));
//  	IplImage* newImg = cvCreateImage(cvSize(x_max - x_min, y_max - y_min), g_pGrayImage->depth, g_pGrayImage->nChannels);
//  	cvCopy(g_pGrayImage, newImg, 0);
//  	cvResetImageROI(newImg);
//  	cvSaveImage("E:\\saveafer.png", newImg);
 //	cvReleaseImage(&newImg);
#ifdef _TEM_TEST
	cvNamedWindow("mytest_binary", CV_WINDOW_AUTOSIZE); 
	cvShowImage("mytest_binary", pBinaryImage); 
 	cvShowImage("mytest_after", pOutlineImage);  

 	cvReleaseMemStorage(&cvMStorage);  
  	cvReleaseImage(&pBinaryImage);  
  	cvReleaseImage(&pOutlineImage); 
#endif
}  



void myTest()
{
	IplImage *pSrcImage0 = cvLoadImage("E:\\whitepaper-pdfa.pdf - Google Chrome.png", CV_LOAD_IMAGE_UNCHANGED); 
	IplImage *pSrcImage1 = cvLoadImage("E:\\classxxReader.png", CV_LOAD_IMAGE_UNCHANGED); 


	//int isEual = strcmp(outText1, outText0);
// 
// 	ImageProcess* tem = new ImageProcess;
// 
// 	CvRect qaRec;
// 	tem->GetPDFRenderRange(pSrcImage0, &qaRec);
// 
// 	cvSetImageROI(pSrcImage0, qaRec);
// 	IplImage* newImg = cvCreateImage(cvSize(qaRec.width, qaRec.height), pSrcImage0->depth, pSrcImage0->nChannels);
// 	cvCopy(pSrcImage0, newImg, 0);
// 	cvResetImageROI(newImg);
// 	cvSaveImage("E:\\saveafer1111.png", newImg);
// 	cvReleaseImage(&newImg);

//	cvReleaseMemStorage(&cvMstorage);
 	g_pGrayImage =  cvCreateImage(cvGetSize(pSrcImage0), IPL_DEPTH_8U, 1);  
 	cvCvtColor(pSrcImage0, g_pGrayImage, CV_BGR2GRAY);  
 
 	g_pBinaryImage = cvCreateImage(cvGetSize(g_pGrayImage), IPL_DEPTH_8U, 1);  

	// 显示原图  
 	cvNamedWindow("mytest_after", CV_WINDOW_AUTOSIZE);  
//  	cvShowImage("mytest_after", g_pGrayImage);  
//  	cvWaitKey(0); 
// 	// 创建二值图窗口  
// 	cvNamedWindow("mytest_after", CV_WINDOW_AUTOSIZE);  
// 
// 	vector<Point> contour;
// 	contour.push_back(Point2f(0, 0));
// 	contour.push_back(Point2f(10, 0));
// 	contour.push_back(Point2f(10, 10));
// 	contour.push_back(Point2f(5, 4));
// 
// 	double area0 = contourArea(contour);
// 	vector<Point> approx;
// 	approxPolyDP(contour, approx, 5, true);
// 	double area1 = contourArea(approx);
// 
// 	cout << "area0 =" << area0 << endl <<
// 		"area1 =" << area1 << endl <<
// 		"approx poly vertices" << approx.size() << endl;

	// 滑动条    
	int nThreshold = 0;  
	cvCreateTrackbar("toolbar", "mytest_after", &nThreshold, 254, on_trackbar);  

	on_trackbar(1);  
// 
 	cvWaitKey(0);  
// 
// 	cvDestroyWindow("mytest_after");  
// 	cvDestroyWindow("mytest");  
// 	cvReleaseImage(&pSrcImage0);  
// 	cvReleaseImage(&g_pGrayImage);  
// 	cvReleaseImage(&g_pBinaryImage); 
}
int main( int argc, char **argv )
{
	//myTest2(); //find image center of gravity
	myTest();

	ParseParam ArgvParser = ParseParam(argc, argv);

	if (argc == 1) {
		ArgvParser.HelpInfo();
		return 1;
	}

	ArgvParser.SetGetOptStr("g::c::w:n:m:p:e:o:a:sh");
	while(1) {
		int c = ArgvParser.CallGetOpt();
		ArgvParser.ParseVal(c);
		if(c == -1) break;
	}

	ArgValues InVal = ArgvParser.GetResult();
	vector<string> VPValW;
	vector<int>	VPValN;
	IplImage* pImgGet0 = NULL;
	IplImage* pImgGet1 = NULL;
	for (int i = 0; i < InVal.ActionFlag.size(); i++) {
		switch (InVal.ActionFlag[i]) {
		case COMPAREIMG:
			_DEBUG_INFO("COMPAREIMG  run.\n");
			if (strcmp(InVal.ImgName0, "-c") == 0) {	// compare images.
				_DEBUG_INFO("-c runned.\n");
				if (strcmp(InVal.ImgName1, "-w") == 0) {
					_DEBUG_INFO("-w runned.\n");
					Compare(pImgGet0, pImgGet1, InVal); break; // Compare images. all from srceen shot.
				} else {
					Compare(pImgGet0, InVal); break;	// Compare images. one from srceen shot,other is local file.
				} 
			} 
			Compare(InVal); break;
		case GETDESKTOPIMG:
			_DEBUG_INFO("GETDESKTOPIMG  run.\n");
			VPValW = ArgvParser.ParseInputWValue(InVal.ProgramVal);
			VPValN = ArgvParser.ParseInputNValue(InVal.ScreenShotSize);
			if (VPValW.size() == 0) {
				pImgGet0 = GetScreenImg(argc, argv, NULL, InVal);
			} else if (VPValW.size() == 1) {
				pImgGet0 = GetScreenImg(argc, argv, VPValW[0].c_str(), InVal);
			} else {
				pImgGet0 = GetScreenImg(argc, argv, VPValW[0].c_str(), InVal);
				pImgGet1 = GetScreenImg(argc, argv, VPValW[1].c_str(), InVal);
			} break;
			
		default:
			_DEBUG_INFO("DEFAULT  run.\n");
			break;
		}
	}
	return 0;
}



