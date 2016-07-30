#include "stdafx.h"
#include "ImageProcess.h"



ImageProcess::ImageProcess(void)
{
	this->pImgDiff = NULL;
	this->pImg = NULL;
	this->ImgColorFlag = -1;

	// Init FreeImage.
	FreeImage_Initialise();
}


ImageProcess::~ImageProcess(void)
{
	for (int i = 0; i < this->QaImages.size(); i++) {
		if (this->QaImages[i] != NULL)
			cvReleaseImage(&this->QaImages[i]);
	}

	// Destory FreeImage
	FreeImage_DeInitialise();
}

IplImage* imgA;
void TrackBarFunc(int val)  
{  
	if (val != 0) {
		double scale=val * 0.01;
		IplImage* dst = 0;
		CvSize dst_size;
		dst_size.width = (int)(imgA->width * scale);  
		dst_size.height = (int)(imgA->height * scale);

		dst=cvCreateImage(dst_size,imgA->depth,imgA->nChannels);
		cvResize(imgA,dst,CV_INTER_LINEAR);
		cvShowImage("image",dst);
	}
} 

void ImageProcess::DrawDifferent(IplImage* pImg)
{
	if (this->ImgDiffP.size() == 0)
		return;

	IplImage* pDiffImg = NULL;
	CvSize SizeImg = cvGetSize(pImg);
	int iChannel = -1;
	if (this->ImgColorFlag <= 0) {
		iChannel = 1;
	} else {
		iChannel = 3;
	}
	pDiffImg = cvCreateImage(SizeImg, IPL_DEPTH_8U, iChannel);	
	RgbImage  imgDiff(pDiffImg);
	BwImage  imgDiffP(pDiffImg);

	vector<CvPoint>::iterator ImgPIterator;
	ImgPointV temImgP;
	for (ImgPIterator = this->ImgDiffP.begin(); ImgPIterator != this->ImgDiffP.end(); ++ImgPIterator) {
		CvPoint DiffP = *ImgPIterator;
		if (this->ImgColorFlag == 0) {
			imgDiffP[DiffP.y][DiffP.x] = 255;
		}
		else {
			imgDiff[DiffP.y][DiffP.x].r = 255;
			imgDiff[DiffP.y][DiffP.x].g = 0;
			imgDiff[DiffP.y][DiffP.x].b = 0;
		}
	}
	this->pImgDiff = pDiffImg;
	//cvSaveImage("E:\\f.png", pDiffImg);

}

bool ImageProcess::CombineInsideImageDiff(IplImage** pCombineImg)
{
	IplImage* pImg;
	int iChannel = -1;
	
	if (this->QaImages.size() != 2) return NULL;

	IplImage* pImg0 = this->QaImages[0];
	IplImage* pImg2 = this->QaImages[1];
	this->DrawDifferent(pImg0);
	// if pImgDiff is NULL, means this compare is from Perceptual hash algorithm.
	if (this->pImgDiff == NULL) return NULL;

	if (this->ImgColorFlag > 0)
		iChannel = 3;
	else
		iChannel = 1;

	int maxWidth = MAX(pImg0->width, pImg2->width);
	int maxHeight = MAX(pImg0->height, pImg2->height);
	pImg = cvCreateImage(cvSize(maxWidth*3, maxHeight), IPL_DEPTH_8U, iChannel);
	cvSet (pImg, CV_RGB(0, 0, 0));
	CvRect ROIRect ;//= {0, 0, pImg0->width, pImg0->height};
	ROIRect.x = 0;
	ROIRect.y = 0;
	ROIRect.width = pImg0->width;
	ROIRect.height = pImg0->height;

	cvSetImageROI(pImg, ROIRect);
	cvCopy(pImg0, pImg);
	cvResetImageROI(pImg);

	//ROIRect = cvRect(pImg0->width, 0, pImg1->width, pImg1->height);
	ROIRect = cvRect(maxWidth, 0, maxWidth, maxHeight);
	cvSetImageROI(pImg, ROIRect);
	cvCopy(this->pImgDiff, pImg);
	cvResetImageROI(pImg);

	ROIRect = cvRect(maxWidth * 2, 0, maxWidth, maxHeight);
	cvSetImageROI(pImg,ROIRect);
	cvCopy(pImg2,pImg);
	cvResetImageROI(pImg);
	
	*pCombineImg = pImg;
	if (pCombineImg != NULL) {
		return true;
	} else {
		return false;
	}
}

bool ImageProcess::CombineImageDiff(IplImage* pImg0, IplImage* pImg2, IplImage** pCombineImg) {
	IplImage* pImg;
	int iChannel = -1;

	this->DrawDifferent(pImg0);
	// if pImgDiff is NULL, means this compare is from Perceptual hash algorithm.
	if (this->pImgDiff == NULL) return NULL;

	if (this->ImgColorFlag > 0)
		iChannel = 3;
	else
		iChannel = 1;

	int maxWidth = MAX(pImg0->width, pImg2->width);
	int maxHeight = MAX(pImg0->height, pImg2->height);
	pImg = cvCreateImage(cvSize(maxWidth*3, maxHeight), IPL_DEPTH_8U, iChannel);
	cvSet (pImg, CV_RGB(0, 0, 0));
	//CvRect ROIRect = {0, 0, pImg0->width, pImg0->height};
	CvRect ROIRect ;//= {0, 0, pImg0->width, pImg0->height};
	ROIRect.x = 0;
	ROIRect.y = 0;
	ROIRect.width = pImg0->width;
	ROIRect.height = pImg0->height;

	cvSetImageROI(pImg, ROIRect);
	cvCopy(pImg0, pImg);
	cvResetImageROI(pImg);

	//ROIRect = cvRect(pImg0->width, 0, pImg1->width, pImg1->height);
	ROIRect = cvRect(maxWidth, 0, maxWidth, maxHeight);
	cvSetImageROI(pImg, ROIRect);
	cvCopy(this->pImgDiff, pImg);
	cvResetImageROI(pImg);

	ROIRect = cvRect(maxWidth * 2, 0, maxWidth, maxHeight);
	cvSetImageROI(pImg,ROIRect);
	cvCopy(pImg2,pImg);
	cvResetImageROI(pImg);

	*pCombineImg = pImg;
	if (pCombineImg != NULL) {
		return true;
	} else {
		return false;
	}
}

IplImage* ImageProcess::CombineImage(IplImage* pImg0, IplImage* pImg1)
{
	CvSize size0 = cvGetSize(pImg0);
	size0.height *= 3;
	size0.width *= 3;

	int combineWidth = MAX(pImg0->width, pImg1->width);
	int combineHeight = MAX(pImg0->height, pImg1->height);

	IplImage* pImg;
	pImg = cvCreateImage(cvSize(combineWidth * 2, combineHeight), IPL_DEPTH_8U, 3);

	cvSet (pImg, CV_RGB(0,20,0) );

	//CvRect ROIRect = {0, 0, pImg0->width, pImg0->height};
	CvRect ROIRect ;//= {0, 0, pImg0->width, pImg0->height};
	ROIRect.x = 0;
	ROIRect.y = 0;
	ROIRect.width = pImg0->width;
	ROIRect.height = pImg0->height;

 	cvSetImageROI(pImg, ROIRect);
	cvCopy(pImg0, pImg);
	cvResetImageROI(pImg);

	ROIRect=cvRect(pImg0->width, 0, pImg1->width, pImg1->height);
	cvSetImageROI(pImg,ROIRect);
	cvCopy(pImg1,pImg);
	cvResetImageROI(pImg);

	//this->QaImages.push_back(pImg);
	return pImg;
}


void ImageProcess::ShowImage(IplImage* pImg)
{
	cvDestroyWindow("Image Scroll Bar"); 
	//seperate
	cvNamedWindow("image", 1);
	cvShowImage("image", pImg);
	_TrackBarData TBData={"TrackBar",100,100};
	//cvSet (pDiffImg, CV_RGB(0,0,100) );
	imgA = pImg;
	cvCreateTrackbar( TBData.Name, "image", &TBData.Pos, TBData.MaxValue , TrackBarFunc);  
	//cvShowImage("pImg1", pImg1);
	cvWaitKey(0);
	//_DEBUG_INFO("ShowImage end.\n");
}


double mx = 0, my = 0;  
int dx = 0, dy = 0, horizBar_x = 0, vertiBar_y = 0; 
bool clickVertiBar = false, clickHorizBar = false, needScroll = false; 
CvRect rect_bar_horiz, rect_bar_verti;
void mouse_callback( int event, int x, int y, int flags, void* param )  
{  
	if (needScroll) {
		switch( event ) {
		case CV_EVENT_LBUTTONDOWN:  
			mx = x, my = y;  
			dx = 0, dy = 0;  
			// �������ʱ��궨λ��ˮƽ������������  
			if (x >= rect_bar_horiz.x && x <= rect_bar_horiz.x+rect_bar_horiz.width  
				&& y >= rect_bar_horiz.y && y<= rect_bar_horiz.y+rect_bar_horiz.height) {
				clickHorizBar = true;  
			}  
			// �������ʱ��궨λ�ڴ�ֱ������������  
			if (x >= rect_bar_verti.x && x <= rect_bar_verti.x+rect_bar_verti.width  
				&& y >= rect_bar_verti.y && y<= rect_bar_verti.y+rect_bar_verti.height) {
				clickVertiBar = true;  
			}  
			break;   
		case CV_EVENT_MOUSEMOVE:   
			if (clickHorizBar) {
				dx = fabs(x-mx) > 1 ? (int)(x-mx) : 0;  
				dy = 0;  
			}  
			if (clickVertiBar) {
				dx = 0;  
				dy = fabs(y-my) > 1 ? (int)(y-my) : 0;  
			}  
			mx = x, my = y;  
			break;    
		case CV_EVENT_LBUTTONUP:   
			mx = x, my = y;  
			dx = 0, dy = 0;  
			clickHorizBar = false;  
			clickVertiBar = false;  
			break;    
		default:  
			dx = 0, dy = 0;  
			break;  
		}  
	}  
}  

void ImageProcess::ShowImageScroll(char* title, IplImage* src_img,   
	int winWidth, int winHeight) // ��ʾ���ڴ�СĬ��Ϊ 1400��700  
{  
	IplImage* dst_img;  
	CvRect  rect_dst,   // ��������Ч��ͼ����ʾ����  
		rect_src;   // ����ͼ���Ӧ��Դͼ���е�����  
	int imgWidth = src_img->width,   
		imgHeight = src_img->height,  
		barWidth = 25;  // �������Ŀ�ȣ����أ�  
	double  scale_w = (double)imgWidth/(double)winWidth,    // Դͼ���봰�ڵĿ�ȱ�ֵ  
		scale_h = (double)imgHeight/(double)winHeight;  // Դͼ���봰�ڵĸ߶ȱ�ֵ  

	if(scale_w<1)   
		winWidth = imgWidth+barWidth;  
	if(scale_h<1)   
		winHeight = imgHeight+barWidth;  

	int showWidth = winWidth, showHeight = winHeight; // rect_dst �Ŀ�͸�  
	int src_x = 0, src_y = 0;   // Դͼ���� rect_src �����Ͻ�λ��  
	int horizBar_width = 0, horizBar_height = 0,  
		vertiBar_width = 0, vertiBar_height = 0;  

	needScroll = scale_w>1.0 || scale_h>1.0 ? TRUE : FALSE;  
	// ��ͼ������趨�Ĵ��ڴ�С������ʾ������  
	if(needScroll) {
		dst_img = cvCreateImage(cvSize(winWidth, winHeight),src_img->depth, src_img->nChannels);  
		cvZero(dst_img);  
		// Դͼ���ȴ��ڴ��ڿ�ȣ�����ʾˮƽ������  
		if(scale_w > 1.0) {
			showHeight = winHeight - barWidth;  
			horizBar_width = (int)((double)winWidth/scale_w);  
			horizBar_height = winHeight-showHeight;  
			horizBar_x = min(  
				max(0,horizBar_x+dx),  
				winWidth-horizBar_width);  
			rect_bar_horiz = cvRect(  
				horizBar_x,   
				showHeight+1,   
				horizBar_width,   
				horizBar_height);  
			// ��ʾˮƽ������  
			cvRectangleR(dst_img, rect_bar_horiz, cvScalarAll(255), -1);  
		}   
		// Դͼ��߶ȴ��ڴ��ڸ߶ȣ�����ʾ��ֱ������  
		if(scale_h > 1.0) {
			showWidth = winWidth - barWidth;  
			vertiBar_width = winWidth-showWidth;  
			vertiBar_height = (int)((double)winHeight/scale_h);  
			vertiBar_y = min(  
				max(0,vertiBar_y+dy),   
				winHeight-vertiBar_height);  
			rect_bar_verti = cvRect(  
				showWidth+1,   
				vertiBar_y,   
				vertiBar_width,   
				vertiBar_height);  
			// ��ʾ��ֱ������  
			cvRectangleR(dst_img, rect_bar_verti, cvScalarAll(255), -1);  
		}  

		showWidth = min(showWidth,imgWidth);  
		showHeight = min(showHeight,imgHeight);  
		// ���ô�����ʾ��� ROI  
		rect_dst = cvRect(0, 0, showWidth, showHeight);  
		cvSetImageROI(dst_img, rect_dst);  
		// ����Դͼ��� ROI  
		src_x = (int)((double)horizBar_x*scale_w);  
		src_y = (int)((double)vertiBar_y*scale_h);  
		src_x = min(src_x, imgWidth-showWidth);  
		src_y = min(src_y, imgHeight-showHeight);  
		rect_src = cvRect(src_x, src_y, showWidth, showHeight);  
		cvSetImageROI(src_img, rect_src);  
		// ��Դͼ�����ݸ��Ƶ�������ʾ��  
		cvCopy(src_img, dst_img);  

		cvResetImageROI(dst_img);  
		cvResetImageROI(src_img);  
		// ��ʾͼ��͹�����  
		cvShowImage(title,dst_img);  

		cvReleaseImage(&dst_img);  
	}  
	// Դͼ��С���趨���ڣ���ֱ����ʾͼ���޹�����  
	else {
		cvShowImage(title, src_img);  
	}     
}  

bool ImageProcess::PointInDifRect(CvPoint pt)
{
	int nLen = DifRegArray.size();
	if(nLen <= 0) return false;

	CvRect rect;
	vector<CvRect>::iterator DiffRegIter;
	for (DiffRegIter = DifRegArray.begin(); DiffRegIter != DifRegArray.end(); ++DiffRegIter) {
		//rect = m_DifRegArray.GetAt(i);
		rect = *DiffRegIter;
// 		if(rect.Contains(pt.x,pt.y)) 
// 			return TRUE;
		if (rect.x == pt.x && rect.y == pt.y) return true;

	}
	return false;
}

CvRect ImageProcess::GetBoundBox(ImgPointV &array)
{
	CvRect rect;
	rect.x = rect.y = rect.width = rect.height = 0;
	//rect.left = rect.top = rect.right = rect.bottom = 0;
	int nLen = array.size();
	if (nLen <= 0) return rect;

	CvPoint pt1, pt2;
	//pt1 = array.GetAt(0);
	pt1 = array[0];
	int left = pt1.x;
	int right = pt1.x;
	int top =pt1.y;
	int bottom = pt1.y;

	for (int i=1; i<nLen; i++) {
		pt2 = array[i];
		if(pt2.x < left) left = pt2.x;
		if(pt2.x > right) right = pt2.x;
		if(pt2.y < top ) top = pt2.y;
		if(pt2.y > bottom) bottom = pt2.y;
	}

	rect.x = left;
	rect.height = top;
	rect.y = right;
	rect.width = bottom;

	return rect;
}

void ImageProcess::GetDifRect(void)
{
	DifRegArray.clear(); 
	if(DifRegArray.size() >= 20000) return;

	int i=0,j=0;
	int n=20;

	int nLen = DifRegArray.size();
	if(nLen <= 0) return;

	CvPoint pt_i;
	ImgPointV tmpArr;
	ImgPointV posArr;

	while(nLen>0 && i<nLen)
	{
		tmpArr.clear();
		posArr.clear();

		pt_i = ImgDiffP[i];
		//pt_i = *DiffPosIter;//m_DifPosArray.GetAt(i);
		if (PointInDifRect(pt_i)) 
		{
			i++;
			continue;
		}
		//tmpArr.Add(pt_i);
		tmpArr.push_back(pt_i);
		for(j=i+1;j<nLen;j++)
		{
			//CFX_Point pt_j = m_DifPosArray.GetAt(j);
			CvPoint pt_j = ImgDiffP[j];
			if(abs(pt_j.x - pt_i.x) <= n && abs(pt_j.y - pt_i.y) <= n)
			{
				//tmpArr.Add(pt_j);
				tmpArr.push_back(pt_j);
			}
			else 
				posArr.push_back(pt_j);
			//}
		}
		if(tmpArr.size() >= 5)
		{
			CvRect rect = GetBoundBox(tmpArr);

			if(rect.width >= 2||rect.height >= 2)
			{	
				if(DifRegArray.size() > 0)
					UnionRect(rect,10);
				else
					DifRegArray.push_back(rect);
			}				
		}
// 		m_DifPosArray.RemoveAll();
// 		for(int k =0;k<posArr.GetSize();k++)
// 		{
// 			CFX_Point pt= posArr.GetAt(k);
// 			m_DifPosArray.Add(pt);
// 		}
// 		nLen = m_DifPosArray.GetSize();
// 		i = 0;
		//}
	}

}

bool ImageProcess::IsUnionRect(CvRect rect1, CvRect rect2, int distanc)
{

// 	if (rect1.Contains(rect2.left,rect2.top) ||
// 		rect1.Contains(rect2.right,rect2.top) ||
// 		rect1.Contains(rect2.left,rect2.bottom) ||
// 		rect1.Contains(rect2.right,rect2.bottom))
// 	if (rect1.x == rect2.height)
// 	{
// 		return TRUE;
// 	}
// 
// 	// 
// 	FX_RECT rect = rect1;
// 	rect.left -= distanc;
// 	if(rect.left < 0) rect.left = 0;
// 	rect.right += distanc;
// 	if(rect.right > m_nCmpBmpWidth) 
// 		rect.right = m_nCmpBmpWidth;
// 	rect.top -= distanc;
// 	if(rect.top > 0) 
// 		rect.top = 0;
// 	rect.bottom += distanc;
// 	if(rect.bottom > m_nCmpBmpHeight) 
// 		rect.bottom = m_nCmpBmpHeight;
// 
// 	if (rect.Contains(rect2.left,rect2.top) ||
// 		rect.Contains(rect2.right,rect2.top) ||
// 		rect.Contains(rect2.left,rect2.bottom) ||
// 		rect.Contains(rect2.right,rect2.bottom))
// 	{
// 		return TRUE;
// 	}

	return FALSE;
}

void ImageProcess::UnionRect(CvRect re_union,int distance)
{
	// Normalize top < bottom
	CvRect re_i;
	m_reUnion = re_union;
	int nLen = DifRegArray.size();
	bool bContinue = false;
	for(int i=0;i<nLen;i++)
	{
		re_i = DifRegArray[i];
		if(IsUnionRect(re_i,re_union,distance))
		{
// 			re_i.Union(re_union);
// 			m_reUnion = re_i;
// 			m_DifRegArray.RemoveAt(i);
// 			bContinue = TRUE;
			break;
		}
	}

	if (bContinue)
	{	
		UnionRect(m_reUnion,10);
	}else
	{
		DifRegArray.push_back(m_reUnion);
	}
}

int	ImageProcess::GetFileType(const char* ImgName) {
	FREE_IMAGE_FORMAT fileType = FIF_UNKNOWN;  
	fileType = FreeImage_GetFileType(ImgName);
	if(fileType == FIF_UNKNOWN) {  
		fileType = FreeImage_GetFIFFromFilename(ImgName);
	}  
	switch (fileType) {
	case FIF_GIF:
		return FXQA_IMAGE_GIF;
	case FIF_JPEG:
		return FXQA_IMAGE_JPEG;
	case FIF_BMP:
		return FXQA_IMAGE_BMP;
	case FIF_JP2:
		return FXQA_IMAGE_JP2;
	case FIF_TIFF:
		return FXQA_IMAGE_TIFF;
	case FIF_PNG:
		return FXQA_IMAGE_PNG;
	}
	return -1;
}

IplImage* ImageProcess::FIBITMAP2IplImage(FIBITMAP* dib, int colorFlag)  
{  
	int nClrUsed = FreeImage_GetColorsUsed(dib);  
	int nBpp = FreeImage_GetBPP(dib);  
	int height = FreeImage_GetHeight(dib);  
	int width = FreeImage_GetWidth(dib);  
	RGBQUAD* pPalette = FreeImage_GetPalette(dib);
	int nChannel = nBpp / 8;; 
	if (nChannel < 3) {
		nChannel = 3;
	}
	int colorType = FreeImage_GetColorType(dib);
	if (colorType == FIC_MINISWHITE || colorType == FIC_MINISBLACK || colorFlag == 0) {
		//nChannel = 1;
	}
	int imgType = FreeImage_GetImageType(dib);
	if (colorFlag == 6) 
		nChannel = 3;
	if(!nClrUsed && !pPalette) {  
		IplImage* iplImg = cvCreateImage(cvSize(width,height), IPL_DEPTH_8U, nChannel);     
		iplImg->origin = 1;  
		for(int y=0; y<height; y++) {  
			BYTE* pLine = (BYTE*)iplImg->imageData + y*iplImg->widthStep;  
			BYTE* psrcLine = (BYTE*)FreeImage_GetScanLine(dib, y);  
			for (int x=0; x<nChannel * width; x++) {  
				*pLine++ = *psrcLine++;  
			}  
		}  
		return iplImg;  
	} else if(pPalette) {  
		IplImage* iplImg = cvCreateImage(cvSize(width,height), IPL_DEPTH_8U, nChannel);     
		iplImg->origin = 1;  
		BYTE intensity;  
		BYTE* pIntensity = &intensity;  
		for(int y=0;y<height;y++) {  
			BYTE* pLine = (BYTE*)iplImg->imageData + y*iplImg->widthStep;  
			for (int x=0;x<width;x++) {  
				FreeImage_GetPixelIndex(dib,x,y,pIntensity);  
				pLine[x*3] = pPalette[intensity].rgbBlue;  
				pLine[x*3+1] = pPalette[intensity].rgbGreen;  
				pLine[x*3+2] = pPalette[intensity].rgbRed;  
			}  
		}  
		return iplImg;  
	} else {  
		return NULL;  
	}  
}  

bool ImageProcess::LoadMulImageFI(const char* imgName0, const char* imgName1, IplImage** pImgs, int* frameCount,
								  char* ColorFlag, int loadFlag) {
	FREE_IMAGE_FORMAT imgType0 = FreeImage_GetFileType(imgName0);
	if(imgType0 == FXQA_IMAGE_UNKNOWN || !FreeImage_FIFSupportsReading(imgType0)) {  
		return false;
	}  
	FREE_IMAGE_FORMAT imgType1 = FreeImage_GetFileType(imgName1);
	if(imgType1 == FXQA_IMAGE_UNKNOWN || !FreeImage_FIFSupportsReading(imgType1)) {  
		return false;
	}  

	if (pImgs == NULL || *frameCount <= 0) {
		FIBITMAP* dib0 = FreeImage_Load(imgType0, imgName0, loadFlag); 
		FIBITMAP* dib1 = FreeImage_Load(imgType1, imgName1, loadFlag); 
		FIMULTIBITMAP* pMulImg0 = FreeImage_OpenMultiBitmap(imgType0, imgName0, 0, 1, 0); 
		FIMULTIBITMAP* pMulImg1 = FreeImage_OpenMultiBitmap(imgType0, imgName1, 0, 1, 0); 

		int imgCount0 = FreeImage_GetPageCount(pMulImg0);
		int imgCount1 = FreeImage_GetPageCount(pMulImg1);

		FreeImage_CloseMultiBitmap(pMulImg0);
		FreeImage_CloseMultiBitmap(pMulImg1);
		FreeImage_Unload(dib0);
		FreeImage_Unload(dib1);

		if (imgCount0 != imgCount1) {
			return false;
		}
		*frameCount = imgCount0;
		return true;
	}
	this->ImgColorFlag = this->AtoI(ColorFlag);

	FIBITMAP* dib0 = FreeImage_Load(imgType0, imgName0, loadFlag); 
	FIBITMAP* dib1 = FreeImage_Load(imgType1, imgName1, loadFlag); 

	FIMULTIBITMAP * pMulImg = FreeImage_OpenMultiBitmap(imgType0, imgName0, 0, 1, 0); 
	if (*frameCount == 0) {
		LoadImageInternalFI(imgName0, imgName1, ColorFlag);
		return true;
	}
	for (int curFrame = 0; curFrame < *frameCount; curFrame++) {
		FIBITMAP* pFrame = NULL;
		pFrame= FreeImage_LockPage(pMulImg, curFrame); 
		int width = -1, height = -1;
		width = FreeImage_GetWidth(pFrame); 
		height = FreeImage_GetHeight(pFrame);
		IplImage* pImgTem0 = FIBITMAP2IplImage(pFrame, this->ImgColorFlag);
		IplImage* pImg0 = cvCreateImage(cvSize(pImgTem0->width, pImgTem0->height), IPL_DEPTH_8U, 3);
		cvConvertImage(pImgTem0, pImg0, 1);
		pImgs[curFrame] = pImg0;
		//ShowImage(pImgTem0);
		cvReleaseImage(&pImgTem0);
		FreeImage_UnlockPage(pMulImg, pFrame, 1);
	}
	
	return true;
}

bool ImageProcess::LoadImageInternalFI(const char* imgName0, const char* imgName1, char* ColorFlag, int loadFlag) {
	FREE_IMAGE_FORMAT imgType0 = FreeImage_GetFileType(imgName0);
	if(imgType0 == FXQA_IMAGE_UNKNOWN || !FreeImage_FIFSupportsReading(imgType0)) {  
		return false;
	}  
	FREE_IMAGE_FORMAT imgType1 = FreeImage_GetFileType(imgName1);
	if(imgType1 == FXQA_IMAGE_UNKNOWN || !FreeImage_FIFSupportsReading(imgType1)) {  
		return false;
	}  

	this->ImgColorFlag = this->AtoI(ColorFlag);

	FIBITMAP* dib0 = FreeImage_Load(imgType0, imgName0, loadFlag); 
	FIBITMAP* dib1 = FreeImage_Load(imgType1, imgName1, loadFlag); 
	//FreeImage_Save(FIF_PNG, dib0, "E:/save.png");
	IplImage* pImgTem0 = FIBITMAP2IplImage(dib0, this->ImgColorFlag);
	IplImage* pImgTem1 = FIBITMAP2IplImage(dib1, this->ImgColorFlag);

	IplImage* pImg0 = NULL;
	IplImage* pImg1 = NULL; 

	pImg0 = cvCreateImage(cvSize(pImgTem0->width, pImgTem0->height), IPL_DEPTH_8U, 3);
	cvConvertImage(pImgTem0, pImg0, 1);
	pImg1 = cvCreateImage(cvSize(pImgTem1->width, pImgTem1->height), IPL_DEPTH_8U, 3);
	cvConvertImage(pImgTem1, pImg1, 1);

	cvReleaseImage(&pImgTem0);
	cvReleaseImage(&pImgTem1);
	if (pImg0 != NULL && pImg1 != NULL) {
		this->QaImages.push_back(pImg0);
		this->QaImages.push_back(pImg1);
	}

	if (this->QaImages.size() > 0)
		return true;
	else
		return false;
	return NULL;  
}

bool ImageProcess::LoadImageCV(char* ImgName0, char* ImgName1, char* ColorFlag) {
	IplImage* pImg0 = NULL;
	IplImage* pImg1 = NULL; 

	this->ImgColorFlag = this->AtoI(ColorFlag);

	if (ImgName0 == "-c")
		pImg0 = NULL;
	pImg0 = cvLoadImage(ImgName0, this->ImgColorFlag);
	pImg1 = cvLoadImage(ImgName1, this->ImgColorFlag);

	if (pImg0 != NULL && pImg1 != NULL) {
		this->QaImages.push_back(pImg0);
		this->QaImages.push_back(pImg1);
	}

	if (this->QaImages.size() > 0)
		return true;
	else
		return false;
}

bool ImageProcess::LoadImageCV(IplImage* pImg0, char* ImgName1, char* ColorFlag) {
	IplImage* pImg1 = NULL; 

	this->ImgColorFlag = this->AtoI(ColorFlag);
	pImg1 = cvLoadImage(ImgName1, this->ImgColorFlag);

	if (pImg0 != NULL && pImg1 != NULL) {
		this->QaImages.push_back(pImg0);
		this->QaImages.push_back(pImg1);
	}

	if (this->QaImages.size() > 0)
		return true;
	else
		return false;
}

bool ImageProcess::LoadImageCV(IplImage* pImg0, IplImage* pImg1, char* ColorFlag)
{
	if (pImg0 == NULL || pImg1 == NULL) return false;

	IplImage* pGrayImage0 = NULL;
	IplImage* pGrayImage1 = NULL;

	//Clear
	if (this->QaImages.size() > 0)
		this->QaImages.clear();
	
	// Get -p
	this->ImgColorFlag = this->AtoI(ColorFlag);

	if (this->ImgColorFlag == 0) {
		IplImage* pGrayImage0 = cvCreateImage(cvGetSize(pImg0), IPL_DEPTH_8U, 1);
		IplImage* pGrayImage1 = cvCreateImage(cvGetSize(pImg1), IPL_DEPTH_8U, 1);

		ConvertToGrayImage(pImg0, pGrayImage0);
		ConvertToGrayImage(pImg1, pGrayImage1);

		cvReleaseImage(&pImg0);
		cvReleaseImage(&pImg1);

		this->QaImages.push_back(pGrayImage0);
		this->QaImages.push_back(pGrayImage1);
	} else {
		this->QaImages.push_back(pImg0);
		this->QaImages.push_back(pImg1);
	}
	
	if (this->QaImages.size() > 0)
		return true;
	else
		return false;
}

bool ImageProcess::ImageDiffBufOutPut(int width0, int height0, int width1, int height1, 
								void* imgBuf0, void* imgBuf1, char* ImgOutName)
{
	int ret = -1;
	IplImage* pImg0 = NULL;
	IplImage* pImg1 = NULL;
	IplImage* pImg = NULL;

	ret = ImageBufToIplImage(width0, height0, imgBuf0, &pImg0);
	if (pImg0 == NULL) return false;

	ret = ImageBufToIplImage(width1, height1, imgBuf1, &pImg1);
	if (pImg1 == NULL) return false;

	this->QaImages.push_back(pImg0);
	this->QaImages.push_back(pImg1);

	ImageRgbCompare(pImg0, pImg1);
	CombineInsideImageDiff(&pImg);
	ImageOutPut(ImgOutName, pImg);
	
	return true;
}
bool ImageProcess::ReSetImageSize(char* StrSize)
{
	ParseParam ParamParser = ParseParam();
	int i = 0;
	int x = -1;int y = -1;
	int width = -1; int height = -1;
 	vector<int> SizeInfo = ParamParser.ParseInputNValue(StrSize);
 
 	x = SizeInfo[0]; y = SizeInfo[1];
 	width = SizeInfo[2]; height = SizeInfo[3];

 	vector<IplImage*>::iterator ImgIterator;
 	for (ImgIterator = this->QaImages.begin();
 		ImgIterator != this->QaImages.end(); ++ImgIterator) {
 		IplImage* Img = *ImgIterator;
 
 		// Check the size is out of range
 		if (Img->width < (x + width) || Img->height < (y + height)) {
			_DEBUG_INFO("Image compare range is out of range.\n");
 			return false;
		}
 
 		cvSetImageROI(Img,cvRect(x, y, width, height));
 		IplImage* dst = cvCreateImage(cvSize(width, height), Img->depth, Img->nChannels);
 		cvCopy(Img, dst, 0);
 		cvResetImageROI(Img);
 		this->QaImages[i] = dst;
 		i++;
 	}
	if (i != 0)
		return true;
	else
		return false;
}

bool ImageProcess::ReSetImageSize(CvRect* imgSize)
{
	int i = 0;
	int x = -1;int y = -1;
	int width = -1; int height = -1;

	x = imgSize->x; y = imgSize->y;
	width = imgSize->width; height = imgSize->height;

	vector<IplImage*>::iterator ImgIterator;
	for (ImgIterator = this->QaImages.begin();
		ImgIterator != this->QaImages.end(); ++ImgIterator) {
			IplImage* Img = *ImgIterator;

			// Check the size is out of range
			if (Img->width < (x + width) || Img->height < (y + height)) {
				_DEBUG_INFO("Image compare range is out of range.\n");
				return false;
			}

			cvSetImageROI(Img,cvRect(x, y, width, height));
			IplImage* dst = cvCreateImage(cvSize(width, height), Img->depth, Img->nChannels);
			cvCopy(Img, dst, 0);
			cvResetImageROI(Img);
			this->QaImages[i] = dst;
			i++;
	}
	if (i != 0)
		return true;
	else
		return false;
}

bool ImageProcess::CutImage(IplImage* pImage, CvRect* cutSize, IplImage*& pOutImage) {
	int i = 0;
	int x = -1;int y = -1;
	int width = -1; int height = -1;

	x = cutSize->x; y = cutSize->y;
	width = cutSize->width; height = cutSize->height;


	// Check the size is out of range
	if (pImage->width < (x + width) || pImage->height < (y + height)) {
		_DEBUG_INFO("Image compare range is out of range.\n");
		return false;
	}

	cvSetImageROI(pImage, cvRect(x, y, width, height));
	pOutImage = cvCreateImage(cvSize(width, height), pImage->depth, pImage->nChannels);
	cvCopy(pImage, pOutImage, 0);
	cvResetImageROI(pImage);

	return true;
}

int ImageProcess::AtoI(char* str)
{
	int ret = -1;
	if (strcmp(str, "\n") == 0)
		ret = 1;
	else
		ret = atoi(str);
	return ret;
}

double ImageProcess::ImageCompare(char* ErrLimit, char* PixelOffset, bool bForceChangeSize)
{
	IplImage* pImg0 = NULL;
	IplImage* pImg1 = NULL; 

	pImg0 = this->QaImages[0];
	pImg1 = this->QaImages[1];

	if (pImg0 == NULL || pImg1 == NULL)
		return -1;

	int iPImg0H = pImg0->height;
	int iPImg1H = pImg1->height;
	int iPImg0W = pImg0->width;
	int iPImg1W = pImg1->width;

	if (bForceChangeSize) {
		int iChangeW = iPImg0W, iChangeH = iPImg0H;
		IplImage* pTemChangeImg = pImg1;
		if (iPImg0W >= iPImg1W) {
			iChangeW = iPImg1W;
			iChangeH = iPImg1H;
			pTemChangeImg = pImg0;
		}
		IplImage* pChangeImage = cvCreateImage(cvSize(iChangeW, iChangeH), pTemChangeImg->depth, pTemChangeImg->nChannels);
		cvResize(pTemChangeImg, pChangeImage, CV_INTER_NN);
		//cvSaveImage("E:\\change__fffffffffffffffff.png", pChangeImage);

		if (iPImg0W >= iPImg1W) {
			cvReleaseImage(&pImg0);
			this->QaImages[0] = pChangeImage;
		} else {
			cvReleaseImage(&pImg1);
			this->QaImages[1] = pChangeImage;
		}
		
		return ImageRgbCompare(ErrLimit, PixelOffset);
	}

	if (iPImg0H != iPImg1H || iPImg0W != iPImg1W)
		return ImagePerceptualHashCompare(ErrLimit, PixelOffset);
	else
		return ImageRgbCompare(ErrLimit, PixelOffset);
}

double ImageProcess::ImageMulCompare(IplImage** pImgs0, IplImage** pImgs1, 
									 int frameCount, int* errFrameIndex, 
									 char* ErrLimit, char* PixelOffset) {
	for (int frameIndex = 0; frameIndex < frameCount; frameIndex++) {
		double cmpRet = ImageRgbCompare(pImgs0[frameIndex], pImgs1[frameIndex], this->ImgColorFlag, ErrLimit, PixelOffset);
		if (cmpRet != 1.000000) {
			*errFrameIndex = frameIndex;
			return cmpRet;
		}
	}
	return 1;
}
double ImageProcess::ImageRgbCompare(char* ErrLimit, char* PixelOffset)
{
	double DiffPer = 0;
	IplImage* pImg0 = NULL;
	IplImage* pImg1 = NULL; 
	int iErrlimit = -1;
	vector<int> iOffsetV ;
	int iOffsetX = 0;int iOffsetY = 0;

	iErrlimit = this->AtoI(ErrLimit);
	if (strcmp(PixelOffset, "\n") != 0) {
		ParseParam ParamParser = ParseParam();
		iOffsetV = ParamParser.ParseInputAValue(PixelOffset);
		//iOffset = this->AtoI(PixelOffset);
		if (iOffsetV.size() != 2) {
			printf("-a value set error.\n");
			return 0;
		}
		iOffsetX = iOffsetV[0];
		iOffsetY = iOffsetV[1];
	}

	pImg0 = this->QaImages[0];
	pImg1 = this->QaImages[1];
	if (pImg0 == NULL || pImg1 == NULL)
		return -1;

	int iPImg0H = pImg0->height;
	int iPImg1H = pImg1->height;
	int iPImg0W = pImg0->width;
	int iPImg1W = pImg1->width;

	if (iPImg0H != iPImg1H || iPImg0W != iPImg1W)
		return 0;

	long PixelCnt0 = pImg0->height * pImg0->width;
	long DiffPixelCnt = 0;

	RgbImage  img0(pImg0);
	RgbImage  img1(pImg1);
	BwImage  imgp0(pImg0);
	BwImage  imgp1(pImg1);

	for (int i = 0;i < pImg0->height;i++) {  
		for (int j = 0;j < pImg0->width;j++) {  
			int img0b = 0;int img0g = 0;int img0r = 0;
			int img1b = 0;int img1g = 0;int img1r = 0;
			int img0p = 0;int img1p = 0;

			if (this->ImgColorFlag == 1) {
				img0b = img0[i][j].b;
				img0g = img0[i][j].g;
				img0r = img0[i][j].r;
				img1b = img1[i][j].b;
				img1g = img1[i][j].g;
				img1r = img1[i][j].r;

				if ((double)fabs(double(img0b - img1b)) > iErrlimit \
					|| (double)fabs(double(img0g - img1g)) > iErrlimit \
					|| (double)fabs(double(img0r - img1r)) > iErrlimit) {
						if (iOffsetX != 0 || iOffsetY != 0) {
							int OffsetX = iOffsetX;
							int OffsetY = iOffsetY;
							if ((i == 0 || i == pImg0->height -1) && iOffsetY > 0)
								OffsetY = 0;
							if (i == 0)
								OffsetY = 0;
							img1b = img1[i + OffsetY][j + OffsetX].b;
							img1g = img1[i + OffsetY][j + OffsetX].g;
							img1r = img1[i + OffsetY][j + OffsetX].r;
							if (j != (pImg0->width - 1)) {
								if ((double)fabs(double(img0b - img1b)) > iErrlimit \
									|| (double)fabs(double(img0g - img1g)) > iErrlimit \
									|| (double)fabs(double(img0r - img1r)) > iErrlimit) {
										DiffPixelCnt ++;
										// 								printf("S0*Pos*X:%i,Y:%i, B=%i, G=%i, R=%i \n",j,i,img0b,img0g,img0r);  
										// 								printf("S1*Pos*X:%i,Y:%i, B=%i, G=%i, R=%i \n",j,i,img1b,img1g,img1r); 
										CvPoint DiffP = cvPoint(j, i);
										this->ImgDiffP.push_back(DiffP);
								}
							}

						}
						else {
							DiffPixelCnt ++;
							// 						  				printf("S0*Pos*X:%i,Y:%i, B=%i, G=%i, R=%i \n",j,i,img0b,img0g,img0r);  
							// 						  				printf("S1*Pos*X:%i,Y:%i, B=%i, G=%i, R=%i \n",j,i,img1b,img1g,img1r); 
							CvPoint DiffP = cvPoint(j, i);
							this->ImgDiffP.push_back(DiffP);
						}

				}
			}
			else if (this->ImgColorFlag == 0) {
				img0p = imgp0[i][j];
				img1p = imgp1[i][j];

				if ((double)fabs(double(img0p - img1p)) > (20*iErrlimit)) {
					if (iOffsetX != 0 || iOffsetY != 0) {
						img1p = imgp1[i + iOffsetY][j + iOffsetX];
						if (j != (pImg0->width - 1)) {
							if ((double)fabs(double(img0p - img1p)) > (20*iErrlimit)) {
								DiffPixelCnt ++;
								CvPoint DiffP = cvPoint(j, i);
								this->ImgDiffP.push_back(DiffP);
							}
						}	
					}
					else {
						DiffPixelCnt ++;
						CvPoint DiffP = cvPoint(j, i);
						this->ImgDiffP.push_back(DiffP);
					}
				}
			}
		}  
	} 
	// 	cvSaveImage("E:\\1.png", pImg0);
	// 	cvSaveImage("E:\\2.png", pImg1);
	if (DiffPixelCnt > 0) 
		DiffPer = (double)DiffPixelCnt / (double)PixelCnt0;

	else if(DiffPixelCnt == 0)
		DiffPer = 0;
	return (1 - DiffPer);
}

double ImageProcess::ImageRgbCompare(IplImage* pImg0, IplImage* pImg1, int iColorFlag, char* ErrLimit, char* PixelOffset)
{
	double DiffPer = 0; 
	int iErrlimit = -1;
	vector<int> iOffsetV ;
	int iOffsetX = 0;int iOffsetY = 0;

	if (iColorFlag == -1) {
		iColorFlag = this->ImgColorFlag;
	}

	iErrlimit = this->AtoI(ErrLimit);
// 	if (strcmp(PixelOffset, "\n") != 0) {
// 		ParseParam ParamParser = ParseParam();
// 		iOffsetV = ParamParser.ParseInputAValue(PixelOffset);
// 		//iOffset = this->AtoI(PixelOffset);
// 		if (iOffsetV.size() != 2) {
// 			printf("-a value set error.\n");
// 			return 0;
// 		}
// 		iOffsetX = iOffsetV[0];
// 		iOffsetY = iOffsetV[1];
	//}

	if (pImg0 == NULL || pImg1 == NULL)
		return -1;

	int iPImg0H = pImg0->height;
	int iPImg1H = pImg1->height;
	int iPImg0W = pImg0->width;
	int iPImg1W = pImg1->width;

	if (iPImg0H != iPImg1H || iPImg0W != iPImg1W)
		return 0;

	long PixelCnt0 = pImg0->height * pImg0->width;
	long DiffPixelCnt = 0;

	RgbImage  img0(pImg0);
	RgbImage  img1(pImg1);
	BwImage  imgp0(pImg0);
	BwImage  imgp1(pImg1);

	for (int i = 0;i < pImg0->height;i++) {  
		for (int j = 0;j < pImg0->width;j++) {  
			int img0b = 0;int img0g = 0;int img0r = 0;
			int img1b = 0;int img1g = 0;int img1r = 0;
			int img0p = 0;int img1p = 0;

			if (iColorFlag == 1) {
				img0b = img0[i][j].b;
				img0g = img0[i][j].g;
				img0r = img0[i][j].r;
				img1b = img1[i][j].b;
				img1g = img1[i][j].g;
				img1r = img1[i][j].r;

				if ((double)fabs(double(img0b - img1b)) > iErrlimit \
					|| (double)fabs(double(img0g - img1g)) > iErrlimit \
					|| (double)fabs(double(img0r - img1r)) > iErrlimit) {
						if (iOffsetX != 0 || iOffsetY != 0) {
							int OffsetX = iOffsetX;
							int OffsetY = iOffsetY;
							if ((i == 0 || i == pImg0->height -1) && iOffsetY > 0)
								OffsetY = 0;
							if (i == 0)
								OffsetY = 0;
							img1b = img1[i + OffsetY][j + OffsetX].b;
							img1g = img1[i + OffsetY][j + OffsetX].g;
							img1r = img1[i + OffsetY][j + OffsetX].r;
							if (j != (pImg0->width - 1)) {
								if ((double)fabs(double(img0b - img1b)) > iErrlimit \
									|| (double)fabs(double(img0g - img1g)) > iErrlimit \
									|| (double)fabs(double(img0r - img1r)) > iErrlimit) {
										DiffPixelCnt ++;
										// 								printf("S0*Pos*X:%i,Y:%i, B=%i, G=%i, R=%i \n",j,i,img0b,img0g,img0r);  
										// 								printf("S1*Pos*X:%i,Y:%i, B=%i, G=%i, R=%i \n",j,i,img1b,img1g,img1r); 
										CvPoint DiffP = cvPoint(j, i);
										this->ImgDiffP.push_back(DiffP);
								}
							}

						}
						else {
							DiffPixelCnt ++;
							// 						  				printf("S0*Pos*X:%i,Y:%i, B=%i, G=%i, R=%i \n",j,i,img0b,img0g,img0r);  
							// 						  				printf("S1*Pos*X:%i,Y:%i, B=%i, G=%i, R=%i \n",j,i,img1b,img1g,img1r); 
							CvPoint DiffP = cvPoint(j, i);
							this->ImgDiffP.push_back(DiffP);
						}

				}
			}
			else if (iColorFlag == 0) {
				img0p = imgp0[i][j];
				img1p = imgp1[i][j];

				if ((double)fabs(double(img0p - img1p)) > (20*iErrlimit)) {
					if (iOffsetX != 0 || iOffsetY != 0) {
						img1p = imgp1[i + iOffsetY][j + iOffsetX];
						if (j != (pImg0->width - 1)) {
							if ((double)fabs(double(img0p - img1p)) > (20*iErrlimit)) {
								DiffPixelCnt ++;
								CvPoint DiffP = cvPoint(j, i);
								this->ImgDiffP.push_back(DiffP);
							}
						}	
					}
					else {
						DiffPixelCnt ++;
						CvPoint DiffP = cvPoint(j, i);
						this->ImgDiffP.push_back(DiffP);
					}
				}
			}
		}  
	} 
	// 	cvSaveImage("E:\\1.png", pImg0);
	// 	cvSaveImage("E:\\2.png", pImg1);
	if (DiffPixelCnt > 0) 
		DiffPer = (double)DiffPixelCnt / (double)PixelCnt0;

	else if(DiffPixelCnt == 0)
		DiffPer = 0;
	return (1 - DiffPer);
}

double ImageProcess::ImagePerceptualHashCompare(char* Errlimit, char* tem)
{
	int64 key1 = CalcImagePerceptualHashKey(this->QaImages[0]);
	int64 key2 = CalcImagePerceptualHashKey(this->QaImages[1]);

	//printf("key1 = 0x%llX, key2 = 0x%llX, Similarity = %.2fn", key1, key2, CompareImageKeySimilarity(key1, key2));

	//printf("key1 = 0x%llX, key2 = 0x%llX, Similarity = %.2fn", key1, key2, );
	return CompareImageKeySimilarity(key1, key2);

}
/** 
 * Perceptual hash algorithm
 */
int64 ImageProcess::CalcImagePerceptualHashKey(const IplImage* pImage)
{
	IplImage* pTheImage8X8 = cvCreateImage(cvSize(8, 8), pImage->depth, pImage->nChannels);
	IplImage* pGrayscaleImage = cvCreateImage(cvSize(8, 8), 8, 1);

	cvResize(pImage, pTheImage8X8, CV_INTER_AREA);
	cvConvertImage(pTheImage8X8, pGrayscaleImage);
	cvReleaseImage(&pTheImage8X8);

	float fElementMean = 0;
	for (int y = 0; y < 8; ++y)
	{
		for (int x = 0; x < 8; ++x)
		{
			unsigned char& cElem = *(unsigned char*)(pGrayscaleImage->imageData + x + y * pGrayscaleImage->widthStep);
			cElem = (unsigned char)(cElem / (float)255 * 64);
			fElementMean += cElem;
		}
	}

	fElementMean /= 64;
	unsigned char cElementKey = 0;
	int64 key = 0;
	unsigned char* pKeyPtr = (unsigned char*)&key;

	for (int y = 0; y < 8; ++y)
	{
		for (int x = 0; x < 8; ++x)
		{
			if (fElementMean > *(unsigned char*)(pGrayscaleImage->imageData + x + y * pGrayscaleImage->widthStep))
			{
				//小于平均值即为0。
				cElementKey <<= 1;
			}
			else
			{
				//否则即为1
				cElementKey <<= 1;
				cElementKey |= 1;
			}
		}

		pKeyPtr[y] = cElementKey;
	}

	cvReleaseImage(&pGrayscaleImage);
	return key;
}

//比较2个key的相似度，结果以1为最相似，0为不相似。
float ImageProcess::CompareImageKeySimilarity(int64 key1, int64 key2)
{
	int64 keyResult = key1 ^ key2;
	int nOneCount = 0;
	int i = 64;
	while(i--)
	{
		if ((keyResult & 1) == 1)
			nOneCount++;

		keyResult >>= 1;
	}

	_DEBUG_INFO_D("nOneCount = ", nOneCount);
	return nOneCount == 0 ? 1 : (64 - nOneCount) / (float)64;
}
//

int ImageProcess::ImageOutPut(char* ImgOutName, IplImage* pImg)
{
	if (ImgOutName == "\n")  return true;
	int iRet = cvSaveImage(ImgOutName, pImg);
	//this->ShowImage(pImg);
	return iRet;
}

int ImageProcess::CombineImageOutput(IplImage* pImg0, IplImage* pImg1, const char* outPutName)
{
	IplImage* pImg = CombineImage(pImg0, pImg1);
	int iRet = cvSaveImage(outPutName, pImg);
	return iRet;
}

bool ImageProcess::ImageBufToIplImage(int width, int height, void* buf, IplImage** pImg,
									int bitCount, int sizeImg)
{
	int coeff = 4;
	cv::Mat src( height, width, CV_8UC3, (void *) buf);
// 	cv::Mat small(height/ coeff, width/ coeff, CV_8UC1 );
// 
// 	cv::Size s_half(width/ coeff, height/ coeff);
// 	cv::resize( src, small, s_half, 1, 1, cv::INTER_LINEAR ); // resize from src to small

	//IplImage* writeImage;
	*pImg = cvCloneImage(&(IplImage)src);
 	//cvSaveImage("E:/src1.bmp", writeImage);
// 	cvReleaseImage( &writeImage );
// 
// 	writeImage=cvCloneImage(&(IplImage)small);
// 	cvSaveImage("little.bmp", writeImage);
// 	cvReleaseImage( &writeImage );
	if (pImg == NULL) return false;
	return true;
}

bool ImageProcess::ConvertToGrayImage(const IplImage* pImage, IplImage* pGrayImage)
{
	cvCvtColor(pImage, pGrayImage, CV_BGR2GRAY);  
	
	if (pGrayImage == NULL) return false;

	return true;
}

bool ImageProcess::ConvertToBinaryImage(const IplImage* pGrayImage, int thresholdVal, IplImage* pBinaryImage)
{
 	cvThreshold(pGrayImage, pBinaryImage, thresholdVal, 255, CV_THRESH_BINARY); 
 
 	if (pBinaryImage == NULL) return false;
 
 	return true;
}

int ImageProcess::GetImageContours(const IplImage* pImage, int thresholdVal, CvMemStorage* cvMStorage, CvSeq** pcvSeq)
{
	IplImage* pGrayImage = cvCreateImage(cvGetSize(pImage), IPL_DEPTH_8U, 1);

	ConvertToGrayImage(pImage, pGrayImage);

	//contrast-enhancing
	//ImageStretchByHistogram(pGrayImage, pGrayImage);

	IplImage* pBinaryImage = cvCreateImage(cvGetSize(pGrayImage), IPL_DEPTH_8U, 1);
	ConvertToBinaryImage(pGrayImage, thresholdVal, pBinaryImage);

	int ctoCnt = cvFindContours(pBinaryImage, cvMStorage, pcvSeq, 88, CV_RETR_LIST);

	cvReleaseImage(&pBinaryImage);
	cvReleaseImage(&pGrayImage);

	return ctoCnt;
}

void ImageProcess::ClearCvRect(CvRect* renderSize)
{
	renderSize->x = 0;
	renderSize->y = 0;
	renderSize->height = 0;
	renderSize->width = 0;
}

void	DEBUG_SHOW_IMAGE(IplImage* pImg0, IplImage* pImg1)
{
	cvNamedWindow("Image0", CV_WINDOW_AUTOSIZE);  
	cvShowImage("Image0", pImg0);  

	cvNamedWindow("Image1", CV_WINDOW_AUTOSIZE);  
	cvShowImage("Image1", pImg1);  

	cvWaitKey(0);
}

bool ImageProcess::GetPDFRenderRange(IplImage* pImg, CvRect* renderSize)
{
	if (pImg == NULL) return false;

	ClearCvRect(renderSize);

	CvMemStorage* cvMstorage = cvCreateMemStorage();

	CvSeq* pcvSeq = NULL;

	int img_w = pImg->width;
	int img_h = pImg->height;

	int n = 0;
	int x_max = 0, y_max = 0;
	int x_min = img_w, y_min = img_h;

	for (int threshold = 50; threshold < 240; threshold += 10) {
		int cnt = GetImageContours(pImg, threshold, cvMstorage, &pcvSeq);

		for (CvSeq* c = pcvSeq; c != NULL; c = c->h_next) {
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
					if (p->x < (img_w - 20))
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
	}
	renderSize->x = x_min;
	renderSize->y = y_min;
	renderSize->width = x_max - x_min;
	renderSize->height = y_max - y_min;

	cvReleaseMemStorage(&cvMstorage);
	//cvSetImageROI(g_pGrayImage, cvRect(x_min, y_min, x_max - x_min, y_max - y_min));
	if (renderSize->width == 0 || renderSize->height == 0) return false;

	return true;
}

bool ImageProcess::GetPDFRenderImage(IplImage* pImg0, IplImage* pImg1, CvRect* pdfRenderRange0, CvRect* pdfRenderRange1, IplImage** pdfImg0, IplImage** pdfImg1)
{
	if (pImg0 == NULL || pImg1 == NULL) return false;

	CvSize commonSize;
	pdfRenderRange0->width > pdfRenderRange1->width ? \
		commonSize.width = pdfRenderRange1->width : \
		commonSize.width = pdfRenderRange0->width;

	pdfRenderRange0->height > pdfRenderRange1->height ? \
		commonSize.height = pdfRenderRange1->height : \
		commonSize.height = pdfRenderRange0->height;

#ifdef _DEBUG_OUTPUT_INFO_
	commonSize.height = pdfRenderRange0->height;
	commonSize.width = pdfRenderRange0->width;
#endif
	//pdf render page 0
	cvSetImageROI(pImg0, cvRect(pdfRenderRange0->x, pdfRenderRange0->y, commonSize.width, commonSize.height));
	*pdfImg0 = cvCreateImage(cvSize(commonSize.width, commonSize.height),
		pImg0->depth, pImg0->nChannels);
	cvCopy(pImg0, *pdfImg0, 0);
	cvResetImageROI(pImg0);

#ifdef _DEBUG_OUTPUT_INFO_
	commonSize.height = pdfRenderRange1->height;
	commonSize.width = pdfRenderRange1->width;
#endif
	//pdf render page 1
	cvSetImageROI(pImg1, cvRect(pdfRenderRange1->x, pdfRenderRange1->y, commonSize.width, commonSize.height));
	*pdfImg1 = cvCreateImage(cvSize(commonSize.width, commonSize.height),
		pImg1->depth, pImg1->nChannels);
	cvCopy(pImg1, *pdfImg1, 0);
	cvResetImageROI(pImg1);

	return true;
}

bool ImageProcess::ImageStretchByHistogram(IplImage *src,IplImage *dst)
{
	//p[]存放图像各个灰度级的出现概率；
	//p1[]存放各个灰度级之前的概率和，用于直方图变换；
	//num[]存放图象各个灰度级出现的次数;

	if (src->width != dst->width) return false;

	float p[256],p1[256],num[256];
	//清空三个数组
	memset(p,0,sizeof(p));
	memset(p1,0,sizeof(p1));
	memset(num,0,sizeof(num));

	int height=src->height;
	int width=src->width;
	long wMulh = height * width;

	//求存放图象各个灰度级出现的次数
	// to do use openmp
	for(int x=0;x<width;x++)
	{
		for(int y=0;y<height;y++)
		{
			uchar v=((uchar*)(src->imageData + src->widthStep*y))[x];
			num[v]++;
		}
	}

	//求存放图像各个灰度级的出现概率
	for(int i=0;i<256;i++)
	{
		p[i]=num[i]/wMulh;
	}

	//求存放各个灰度级之前的概率和
	for(int i=0;i<256;i++)
	{
		for(int k=0;k<=i;k++)
			p1[i]+=p[k];
	}

	//直方图变换
	// to do use openmp
	for(int x=0;x<width;x++)
	{
		for(int y=0;y<height;y++)
		{
			uchar v=((uchar*)(src->imageData + src->widthStep*y))[x];
			((uchar*)(dst->imageData + dst->widthStep*y))[x]= p1[v]*255+0.5;            
		}
	}

	return true;

}

bool ImageProcess::ImageFindImageCore(IplImage* src)
{
	CvMoments moments; 
	CvMat *region; 
	CvPoint pt1,pt2; 
	double m00 = 0, m10, m01, mu20, mu11, mu02, inv_m00; 
	double a, b, c; 
	int xc, yc; 
	// 第一条命令行参数确定了图像的文件名。 
	if( src != 0) 
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
		} 

		cvNamedWindow( "Components", 1 ); 
		cvShowImage( "Components", dst ); 
		cvWaitKey(0); 
	}
	return true;
}

void ImageProcess::GetImages(IplImage** pImg0, IplImage** pImg1) {
	*pImg0 = this->QaImages[0];
	*pImg1 = this->QaImages[1];
}

int ImageProcess::ImageResize(const IplImage* src, float scale, IplImage*& desc) {
	CvSize sz;
	sz.width = src->width * scale;
	sz.height = src->height * scale;

	desc = cvCreateImage(sz, src->depth, src->nChannels);
	if (desc == NULL) {
		return -1;
	}
	cvResize(src, desc, CV_INTER_CUBIC);

	return 1;
}

//
//int BrightnessPosition = 100;
//int ContrastPosition = 100;
//
//int HistogramBins = 64;
//int HistogramBinWidth;
//float HistogramRange1[2]={0,256};
//float *HistogramRange[1]={&HistogramRange1[0]};
//
//IplImage *Image1,*Image2;
//CvHistogram *Histogram1;
//IplImage *HistogramImage;
//
//uchar LookupTableData[256];
//CvMat *LookupTableMatrix;
//IplImage *LookupTableImage;
//CvPoint Point1,Point2;
//
//
//void OnTrackbar(int Position)
//{
//	int Brightness=BrightnessPosition-100;
//	int Contrast=ContrastPosition -100;
//	double Delta;
//	double a,b;
//	int y;
//
//	//Brightness/Contrast Formula
//	if(Contrast>0)
//	{
//		Delta=127*Contrast/100;
//		a=255/(255-Delta*2);
//		b=a*(Brightness-Delta);
//
//		for(int x=0;x<256;x++)
//		{
//			y=(int)(a*x+b);
//
//			if(y<0)
//				y=0;
//			if(y>255)
//				y=255;
//
//			LookupTableData[x]=(uchar)y;
//		}
//	}
//	else
//	{
//		Delta=-128*Contrast/100;
//		a=(256-Delta*2)/255;
//		b=a*Brightness+Delta;
//
//		for(int x=0;x<256;x++)
//		{
//			y=(int)(a*x+b);
//
//			if(y<0)
//				y=0;
//			if(y>255)
//				y=255;
//
//			LookupTableData[x]=(uchar)y;
//		}
//	}
//	//End
//
//	//Look up table sketch
//	cvSetZero(LookupTableImage);
//	cvNot(LookupTableImage,LookupTableImage);
//	Point2=cvPoint(0,LookupTableData[0]);
//	for(int i=0;i<256;i++)
//	{
//		Point1=cvPoint(i,LookupTableData[i]);
//		cvLine(LookupTableImage,Point1,Point2,CV_RGB(0,0,0),3);
//		Point2=Point1;
//	}
//	cvLUT(Image1,Image2,LookupTableMatrix);
//	//End
//
//	//Gray Level Histogram
//	cvCalcHist(&Image2,Histogram1);
//	cvNormalizeHist(Histogram1,3000);
//
//	cvSetZero(HistogramImage);
//	cvNot(HistogramImage,HistogramImage);
//	HistogramBinWidth = HistogramImage->width/HistogramBins;
//	for(int i=0;i<HistogramBins;i++)
//	{
//		Point1=cvPoint(i*HistogramBinWidth,0);
//		Point2=cvPoint((i+1)*HistogramBinWidth,(int)cvQueryHistValue_1D(Histogram1,i));
//		cvRectangle(HistogramImage,Point1,Point2,CV_RGB(0,0,0),CV_FILLED);
//	}
//	//End
//
//	cvShowImage("Gray Level Histogram",HistogramImage);
//	cvShowImage("Brightness/Contrast",Image2);
//	cvShowImage("Image Enhance",LookupTableImage);
//	cvZero(Image2);
//}
//
//int main()
//{
//	Image1=cvLoadImage("E:/word3.png",0);
//	Image2=cvCloneImage(Image1);
//
//	Histogram1=cvCreateHist(1,&HistogramBins,CV_HIST_ARRAY,HistogramRange);
//	HistogramImage = cvCreateImage(cvSize(320,200),8,1);
//
//	LookupTableImage=cvCreateImage(cvSize(256,256),8,3);
//	LookupTableMatrix=cvCreateMatHeader(1,256,CV_8UC1);
//	cvSetData(LookupTableMatrix,LookupTableData,0);
//
//	LookupTableImage->origin=1;
//	HistogramImage->origin=1;
//
//	cvNamedWindow("Brightness/Contrast",1);
//	cvNamedWindow("Gray Level Histogram",1);
//	cvNamedWindow("Image Enhance",1);
//
//	cvCreateTrackbar("brightness","Brightness/Contrast",&BrightnessPosition,200,OnTrackbar);
//	cvCreateTrackbar("contrast","Brightness/Contrast",&ContrastPosition,200,OnTrackbar);
//
//	OnTrackbar(0);
//
//	cvWaitKey(0);
//}