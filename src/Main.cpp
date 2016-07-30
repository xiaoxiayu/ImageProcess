// FirstTest.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "ImageCompare.h"
#include "ImageCompose.h"
#include "ImageFind.h"
#include "TextProcess.h"
#include "common.h"

int main( int argc, char **argv )
{
	//printf("%d", LevenshteinDistance("xiaoxia_yu@foxitsoftware.com", "xiaoxia yquoxitsoftwareLom"));
	//return 1;
	ParseParam ArgvParser = ParseParam(argc, argv);

	if (argc == 1) {
		ArgvParser.HelpInfo();
		return 1;
	}

	ArgvParser.SetGetOptStr("r::g::c::f::w:n:m:p:e:i:t:o:a:sh");
	int arg_index = 0;
	while(1) {
		int c = ArgvParser.CallGetOpt();
		ArgvParser.ParseVal(c, arg_index);
		arg_index++;
		if(c == -1) break;
	}

	//TextProcess tt;
	//tt.Test1();

	ArgValues InVal = ArgvParser.GetResult();
	vector<string> VPValW;
	vector<int>	VPValN;
	IplImage* pImgGet0 = NULL;
	IplImage* pImgGet1 = NULL;
	for (int i = 0; i < InVal.ActionFlag.size(); i++) {
		switch (InVal.ActionFlag[i]) {
		case FXQA_FUNC_COMPAREIMG:
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
			Compare(InVal); break;						// Compare images, or file.
		case FXQA_FUNC_GETDESKTOPIMG:
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
		case FXQA_FUNC_COMPOSE:
			Compose(InVal);
			break;
		case FXQA_FUNC_FINDIMG:
			FXQAFunc_Find(argc, argv, InVal);
			break;
		case FXQA_FUNC_OCR:
			FXQAFunc_OCR(argc, argv, InVal);
			break;
		default:
			_DEBUG_INFO("DEFAULT  run.\n");
			break;
		}
	}
	return 0;
}



