
#include "TextLabelOCR.h"
#include "ImageProcess.h"

LabelOCR::LabelOCR()
	: showImages(false) {
    // constructor
    // Pass it to Tesseract API

    tess.Init(NULL, "eng", tesseract::OEM_DEFAULT);
    tess.SetVariable("tessedit_char_whitelist", "abcdefghijklmnopqrstuvwxyzABCDEFHIJKLMNOPQRSTUVWXYZ0123456789-"); //-
    tess.SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
}

LabelOCR::~LabelOCR() {
    //destructor

    tess.Clear();
    tess.End();
}

void LabelOCR::filterUndesiredChars(string &str){
    char chars[] = "?";

    for (unsigned int i = 0; i < strlen(chars); ++i)
    {
        // you need include <algorithm> to use general algorithms like std::remove()
        str.erase (std::remove(str.begin(), str.end(), chars[i]), str.end());
    }
}

void LabelOCR::preProcess(const Mat &InputImage, Mat &binImage)
{
	IplImage* iplInputImage = new IplImage(InputImage);
	IplImage* newInputImage = NULL;
	ImageProcess pro;
	int resize_ret = pro.ImageResize(iplInputImage, 2.5, newInputImage);
	if (resize_ret < 0) {
		return;
	}

	
    Mat midImage, midImage2, dst;
    Mat Morph = getStructuringElement(MORPH_CROSS,Size( 1, 1 ) );
    Mat HPKernel = (Mat_<float>(5,5) << -1.0,  -1.0, -1.0, -1.0,  -1.0,
                                        -1.0,  -1.0, -1.0, -1.0,  -1.0,
                                        -1.0,  -1.0, 25, -1.0,  -1.0,
                                        -1.0,  -1.0, -1.0, -1.0,  -1.0,
                                        -1.0,  -1.0, -1.0, -1.0,  -1.0);
    medianBlur( cv::Mat(newInputImage), dst, 1);
	filter2D(dst, midImage2, InputImage.depth(), HPKernel);
	cvtColor(midImage2, binImage, COLOR_RGB2GRAY);
	/*IplImage* temIplImg = new IplImage(midImage);
	CvSize sz;
	sz.width = temIplImg->width;
	sz.height = temIplImg->height;
	IplImage* temIplImg1 = cvCreateImage(sz, temIplImg->depth, temIplImg->nChannels);
    cvThreshold(temIplImg, temIplImg1, 60, 255, CV_THRESH_BINARY);
	binImage = temIplImg1;*/
    //threshold(binImage, binImage ,0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    //erode(binImage, binImage, 3, Point(-1, -1), 2, 1, 1);
    //morphologyEx( binImage,binImage,MORPH_CLOSE, Morph);
}

string LabelOCR::runPrediction1(const Mat &labelImage, int i){
    string t1;
    if (labelImage.empty())
        return (t1);

    Mat textImage;
    Mat drawImage = labelImage.clone();
	
    Mat midImage;
    preProcess(drawImage, textImage);

	double left_slice = 0.17;
	double labelROI_x = textImage.size().width * left_slice; // initial point x
	double labelROI_y = 0; //textImage.rows*0.76; // initial point y
	double labelROI_w = textImage.size().width * (1 - left_slice);  // width
	double labelROI_h = textImage.size().height; // heigth

	Rect labelROI(labelROI_x, labelROI_y, labelROI_w, labelROI_h);

    tess.TesseractRect( textImage.data, 1, textImage.step1(), labelROI.x, labelROI.y, labelROI.width, labelROI.height);

    // Get the text
    char* text1 = tess.GetUTF8Text();
    t1 = string(text1);
    if (t1.size() > 2)
        t1.resize(t1.size() - 2);
	
    if (showImages){
		ImageProcess tem;
		IplImage* temI = new IplImage(textImage);

		IplImage* temI2 = NULL;
		CvRect* labelROI2 = new CvRect;
		labelROI2->x = labelROI_x; labelROI2->y = labelROI_y; labelROI2->width = labelROI_w; labelROI2->height = labelROI_h;
		tem.CutImage(temI, labelROI2, temI2);
		tem.ShowImage(temI2);

    }

    return (t1);
}

string LabelOCR::runPrediction2(const Mat &labelImage, int i){

    string t1;
    if (labelImage.empty())
        return (t1);

    Mat textImage;
    Mat drawImage = labelImage.clone();

    double labelROI_x = labelImage.cols*0.15; // initial point x
    double labelROI_y = labelImage.rows*0.20; // initial point y
    double labelROI_w = labelImage.cols*0.5;  // width
    double labelROI_h = labelImage.rows*0.15; // heigth

    Rect labelROI(labelROI_x, labelROI_y, labelROI_w, labelROI_h);

    Mat midImage;
    preProcess(drawImage, textImage);


    tess.TesseractRect( textImage.data, 1, textImage.step1(), labelROI.x, labelROI.y, labelROI.width, labelROI.height);
    // Get the text
    char* text1 = tess.GetUTF8Text();
    t1 = string(text1);
    if (t1.size() > 2)
        t1.resize(t1.size() - 2);

    cout << "label_" << i << ": " << t1 << endl;

    if (showImages){
        putText(drawImage, t1, Point(labelROI.x+7, labelROI.y-5), FONT_HERSHEY_PLAIN, 1.5, Scalar(0, 0, 255), 2, 8); // CV_FONT_HERSHEY_SIMPLEX
        rectangle(drawImage, labelROI, Scalar(0, 0, 255), 2, 8, 0);
        //
        stringstream ss; ss << i;
        string str = ss.str();

        //imshow("label_"+str, labelImage);
        imshow("textImage_2_"+str, textImage);
        imshow("letters_2_"+str, drawImage);
    }

    return (t1);
}

vector<string> LabelOCR::runRecognition(const vector<Mat> &labelImage, int labelType)
{
    vector<string> output;

    output.resize(labelImage.size());

    for( size_t i = 0; i < labelImage.size(); i++ )
    {
        if ( !labelImage[i].empty() && labelType == 1)
            output[i] = runPrediction1(labelImage[i],i);
        if ( !labelImage[i].empty() && labelType == 2)
            output[i] = runPrediction2(labelImage[i],i);
    }
    return (output);
}
