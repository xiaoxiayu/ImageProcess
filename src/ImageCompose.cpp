#include "ImageCompose.h"


void Compose(ArgValues InVal) {
	if (strncmp(InVal.ImgOutName, "\n", 1) == 0) {
		return;
	}
	ImageProcess ImgProc = ImageProcess();

	bool bLoad = false;
	bLoad = ImgProc.LoadImageInternalFI(InVal.ImgName0, InVal.ImgName1, InVal.Color);
	if (!bLoad) {
		return;
	}

	if (strncmp(InVal.CompareSize, "\n", 1) != 0)
		ImgProc.ReSetImageSize(InVal.CompareSize);

	IplImage* pImg0 = NULL;
	IplImage* pImg1 = NULL;
	ImgProc.GetImages(&pImg0, &pImg1);

	bool ret = ImgProc.CombineImageOutput(pImg0, pImg1, InVal.ImgOutName);
	if (ret) {
		printf("%lf", 1.000000);
	} else {
		printf("%lf", 0.000000);
	}
}