#pragma once

#ifndef _IMAGEPROCESS_H_
#define _IMAGEPROCESS_H_

#include "ParseParam.h"
#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>
#include <opencv/cv.h>

#include "FreeImage.h"

#define FXQA_IMAGE_BMP			0
#define FXQA_IMAGE_PNG			1
#define FXQA_IMAGE_JPEG			2
#define FXQA_IMAGE_GIF			3
#define FXQA_IMAGE_TIFF			4
#define FXQA_IMAGE_JP2			5
#define FXQA_IMAGE_UNKNOWN		-1

using namespace cv;
typedef vector<CvPoint> ImgPointV; 
typedef vector<CvRect> ImgPRectV; 
typedef vector<IplImage*> ImageV; 

template<class T> class Image
{
private:
	IplImage* imgp;
public:
	Image(IplImage* img=0) {imgp=img;}
	~Image(){imgp=0;}
	void operator=(IplImage* img) {imgp=img;}
	inline T* operator[](const int rowIndx) {
		return ((T *)(imgp->imageData + rowIndx*imgp->widthStep));}
}; 

typedef struct{
	unsigned char b,g,r;
} RgbPixel; 

typedef struct{
	float b,g,r;
} RgbPixelFloat; 

typedef Image<RgbPixel>       RgbImage;
typedef Image<RgbPixelFloat>  RgbImageFloat;
typedef Image<unsigned char>  BwImage;
typedef Image<float>          BwImageFloat;

   
void						DEBUG_SHOW_IMAGE(IplImage* pImg0, IplImage* pImg1);

struct _TrackBarData  
{  
	char *Name; 
	int Pos; 
	int MaxValue;
};  
// _TrackBarData TBData={"TrackBar",127,255};
void TrackBarFunc(int val); 
void mouse_callback( int event, int x, int y, int flags, void* param );
class ImageProcess
{
public:
	ImageProcess(void);
	~ImageProcess(void);

	ImgPointV					ImgDiffP;
	ImgPRectV					DifRegArray;
	IplImage*					pImg;
	IplImage*					pImgDiff;
	CvRect						m_reUnion;
	ImageV						QaImages;
	int							ImgColorFlag;

	int							GetFileType(const char* ImgName);
	bool						LoadImageInternalFI(const char* imgName0, const char* imgName1, char* ColorFlag = "1", int loadFlag = 0);
	bool						LoadMulImageFI(const char* imgName0, const char* imgName1, 
											   IplImage** pImgs, int* frameCount, char* ColorFlag = "1", int loadFlag = 0);
	bool						LoadImageCV(char* ImgName0 = "\n", char* ImgName1 = "\n", char* ColorFlag = "1");
	bool						LoadImageCV(IplImage* pImg0, char* ImgName1 = "\n", char* ColorFlag = "1");
	bool						LoadImageCV(IplImage* pImg0, IplImage* pImg1, char* ColorFlag = "1");
	
	IplImage*					FIBITMAP2IplImage(FIBITMAP* dib, int colorFlag);
	
	double						ImageCompare(char* ErrLimit = "0", char* PixelOffset = "0", bool bForceChangeSize = false);
	double						ImageMulCompare(IplImage** pImgs0, IplImage** pImgs1, int frameCount, int* errFrameIndex, 
												char* ErrLimit = "0", char* PixelOffset = "0");
	double						ImageRgbCompare(char* ErrLimit = "0", char* PixelOffset = "0");
	double						ImageRgbCompare(IplImage* pImg0, IplImage* pImg1, int iColorFlag = -1, char* ErrLimit = "0", char* PixelOffset = "0");
	double						ImagePerceptualHashCompare(char* ErrLimit = "0", char* PixelOffset = "0");

	int							ImageOutPut(char* ImgOutName, IplImage* pImg);
	bool						ImageDiffBufOutPut(int width0, int height0, int width1, int height1, void* imgBuf0, void* imgBuf1, char* ImgOutName);
	int							CombineImageOutput(IplImage* pImg0, IplImage* pImg1, const char* outPutName);
	void						ShowImage(IplImage* pImg);

	bool						CutImage(IplImage* pImage, CvRect* cutSize, IplImage*& pOutImage);

	bool						ImageBufToIplImage(int width, int height, void* buf, IplImage** pImg, int bitCount=-1, int sizeImg=-1);
	bool						ConvertToGrayImage(const IplImage* pImage, IplImage* pGrayImage);
	bool						ConvertToBinaryImage(const IplImage* pGrayImage, int thresholdVal, IplImage* pBinaryImage);
	bool						ReSetImageSize(char* StrSize);
	bool						ReSetImageSize(CvRect* imgSize);
	bool						CombineInsideImageDiff(IplImage** pCombineImg);
	bool						CombineImageDiff(IplImage* pImg0, IplImage* pImg2, IplImage** pCombineImg);
	int							GetImageContours(const IplImage* pImage, int thresholdVal, CvMemStorage* cvMStorage, CvSeq** pcvSeq);
	bool						GetPDFRenderRange(IplImage* pImg, CvRect* renderSize);
	bool						GetPDFRenderImage(IplImage* pImg0, IplImage* pImg1, CvRect* pdfRenderRange0, CvRect* pdfRenderRange1, IplImage** pdfImg0, IplImage** pdfImg1);

	bool						ImageStretchByHistogram(IplImage *src,IplImage *dst);
	bool						ImageFindImageCore(IplImage *src);

	void						GetImages(IplImage** pImg0, IplImage** pImg1);

protected:
	int64						CalcImagePerceptualHashKey(const IplImage* pImage);
	float						CompareImageKeySimilarity(int64 key1, int64 key2);
	IplImage*					CombineImage(IplImage* pImg0, IplImage* pImg1);

private:
	void						ClearCvRect(CvRect* renderSize);
	int							AtoI(char*);
	void						GetDifRect(void);
	void						DrawDifferent(IplImage* pImg);

	bool						PointInDifRect(CvPoint pt);
	CvRect						GetBoundBox(ImgPointV &array);
	void						UnionRect(CvRect re_union,int distance);
	bool						IsUnionRect(CvRect rect1, CvRect rect2, int distanc);
	void						ShowImageScroll(char* title, IplImage* src_img, int winWidth = 1400, int winHeight = 700);

	
	//static void mouse_callback( int event, int x, int y, int flags, void* param );
	//static bool needScroll = false;
};

#endif