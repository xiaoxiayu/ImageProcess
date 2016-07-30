

#ifndef _TEXTLABLEOCR_H_
#define _TEXTLABLEOCR_H_

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cvaux.h>

#include <iostream>
#include <math.h>
#include <string.h>
#include <sstream>

#include <tesseract/api/baseapi.h>
#include <tesseract/ccutil/strngs.h>

using namespace cv;
using namespace std;

class LabelOCR {

public:
    LabelOCR();
    virtual ~LabelOCR();
    vector<string> runRecognition(const vector<Mat> &labelImage, int labelType);
    tesseract::TessBaseAPI tess;
    
	void SetShowImage() {
		showImages = true;
	}

private:
	bool showImages;

    void preProcess(const Mat &InputImage, Mat &binImage);
    string runPrediction1(const Mat &labelImage, int i);
    string runPrediction2(const Mat &labelImage, int i);
    void skeletonize(Mat& im);
    void thinningIteration(Mat& im, int iter);
    void filterUndesiredChars(string &str);

};

#endif /* _TEXTLABLEOCR_H_ */
