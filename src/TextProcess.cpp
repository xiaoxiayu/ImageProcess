
#include "TextProcess.h"
#include <tesseract/api/baseapi.h>
#include <allheaders.h>

#include "ImageCompare.h"
#include "TextLabelOCR.h"

void FXQAFunc_OCR(int argc, char **argv, ArgValues InVal) {
	LabelOCR labelOcr;
	vector<Mat> possible_labels, label_1;

	cv::Mat image;
	if (strcmp(InVal.ScreenShotSize , "\n") != 0) {
		IplImage* pImgGet = GetScreenImg(argc, argv, NULL, InVal);
		if(pImgGet != NULL) {
			image = pImgGet;
		}
	} else if (strcmp(InVal.ImgName0 , "\n") != 0) {
		image = cv::imread(InVal.ImgName0);
	}
	
	possible_labels.push_back(image);

	Mat gray;
	cvtColor(possible_labels[0], gray, COLOR_RGB2GRAY);
	Mat p= gray.reshape(1, 1);
	p.convertTo(p, CV_32FC1); // CV_32FC1

	label_1.push_back(possible_labels[0]);

	if (InVal.ShowImg) {
		labelOcr.SetShowImage();
	}

	std::vector<std::string> recString = labelOcr.runRecognition(label_1, 1);
	std::vector<std::string>::iterator itor = recString.begin();
	for (; itor != recString.end(); itor++) {
		std::cout << *itor << std::endl;
	}

}
