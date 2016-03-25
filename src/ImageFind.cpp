#include "ImageFind.h"

#include "ImageProcess.h"
#include "ScreenSnapshot.h"
#include "ImageCompare.h"

void FXQAFunc_Find(int argc, char **argv, ArgValues InVal) {
	std::string imgPath = InVal.ImgName0;
	
	cv::Mat src_img, template_img;
	cv::Mat result_mat;
	//cv::Mat debug_img;

	template_img = cv::imread(imgPath.c_str(), CV_LOAD_IMAGE_COLOR);
	if (template_img.data == NULL) {
		printf("cv::imread() failed...\n");
		return ;
	}

	QApplication app(argc, argv);
	QtSnapshot snapshot;
	snapshot.GrabWindow(InVal.CompareSize);
	src_img = snapshot.GetIplImage();
	if (src_img.data == NULL) {
		printf("cv::imread() failed...\n");
		return ;
	}

	int match_method = CV_TM_CCORR_NORMED;
	cv::matchTemplate(src_img, template_img, result_mat, match_method);
	//cv::normalize(result_mat, result_mat, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

	double minVal; double maxVal; 
	cv::Point minLoc, maxLoc, matchLoc;
	cv::minMaxLoc(result_mat, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );
	if( match_method  == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED )  matchLoc = minLoc;
	else matchLoc = maxLoc;
	
	ImageProcess img_proc;
	CvRect re = {matchLoc.x, matchLoc.y, template_img.cols, template_img.rows};
	IplImage ipl_template_img = template_img;
	IplImage ipl_res_img = src_img;

	IplImage* ipl_out_img = NULL;
	img_proc.CutImage(&ipl_res_img, &re, ipl_out_img);
	double ret_pec = img_proc.ImageRgbCompare(ipl_out_img, &ipl_template_img, 1);
	cvReleaseImage(&ipl_out_img);

	int match_x = 0, match_y = 0, match_w = 0, match_h = 0;
	if (strlen(InVal.CompareSize) == 1) {
		match_x = matchLoc.x;
		match_y = matchLoc.y;
		match_w = template_img.cols;
		match_h = template_img.rows;
	} else {
		ParseParam ParamParser;
		vector<int> SizeInfo = ParamParser.ParseInputNValue(InVal.CompareSize);
		if (SizeInfo.size() < 4) {
			std::cout << "Param ERROR: -n";
			return;
		}
		match_x = matchLoc.x + SizeInfo[0];
		match_y = matchLoc.y  + SizeInfo[1];
		match_w = template_img.cols;
		match_h = template_img.rows;
	}
	// Print info.
	std::cout << ret_pec << ":" << match_x << "*" << match_y << "*" << match_w << "*" << match_h;
		
	if (InVal.ShowImg) {
		LOGBRUSH lb;
		lb.lbStyle = BS_SOLID; 
		lb.lbColor = RGB(255,0,0); 
		lb.lbHatch = 0; 
		HGDIOBJ hPen = ExtCreatePen(PS_COSMETIC | PS_SOLID, 
			1, &lb, 0, NULL); 

		HDC hDC = GetDC(0);
		HGDIOBJ hPenOld = SelectObject(hDC, hPen); 

		MoveToEx(hDC, match_x, match_y, NULL); 
		LineTo(hDC, match_x + match_w, match_y); 

		MoveToEx(hDC, match_x, match_y, NULL); 
		LineTo(hDC, match_x, match_y + match_h); 

		MoveToEx(hDC, match_x + match_w, match_y, NULL); 
		LineTo(hDC, match_x + match_w, match_y + match_h); 

		MoveToEx(hDC, match_x, match_y + match_h, NULL); 
		LineTo(hDC, match_x + match_w, match_y + match_h); 

		SelectObject(hDC, hPenOld); 
		DeleteObject(hPen); 
	}
	
}
