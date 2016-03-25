#include "stdafx.h"
#include "ParseParam.h"


ParseParam::ParseParam(void)
{
	this->argc = 0;
	this->argv = 0;
	CleanVal();
	gCtlFlag = 0x000000;
}

ParseParam::ParseParam(int argc, char **argv)
{
	this->argc = argc;
	this->argv = argv;
	CleanVal();
	gCtlFlag = 0x000000;
}

ParseParam::~ParseParam(void)
{
}

void ParseParam::HelpInfo(void)
{
	printf("/*********************************************************************/\n");
	printf("* @ FXQA Image Process: Ver.2.1. \n");
	printf("* @ OpenCV: Ver.2.4.3. QT5.0.1. FreeImage3.16.0\n");
	printf("* @ Author: xiaoxia_yu.\n");
	printf("* @ Date: 2013/10/24.\n");
	printf("* *********************************************************************/\n");
	printf("* Function: Image and pdf compare.\n");
	printf("* @ Parameter Description:\n");
	printf("*   -c: Compare image or pdf. After -c should be followed by two files. Essential.\n");
	printf("*   -f: To force uniform size.\n");
	printf("*   -i: Combine image use with -o\n");
	printf("*   -n: Re-set the image size.Rules(x, y, width, height), By * separated.\n");
	printf("*   -p: Image mode. 0 to set grey images, 1 to set color images.\n");
	printf("*   -e: Comparison accuracy. Default is 0.\n");
	printf("*   -a: Different adjust.\n");
	printf("*   -o: Output images. \n");
	printf("*   -s: Show images. \n");
	printf("*   -h: Show This. \n");
	printf("* @ eg. -c 1.png 2.png -n \"20*30*500*600\" -p 1 -e 2 -o 3.png -s\n");
	printf("* *********************************************************************/\n");
	printf("* Function: ScreenShot. \n");
	printf("* @ Parameter Description:\n");
	printf("* @ -g: Set to screen shot mode. Essential.\n");
	printf("* @ -c: Compare image after screen shot, Parameters refer to the above. \n");
	printf("* @ -w: To a screenshot of the application class name. \n");
	printf("* @ -m: Re-set the screen shot size.Rules(x, y, width, height), By * separated.Default is full screen.\n");
	printf("* @ -o: Output images. \n");
	printf("* @ eg. -g -o e:\\screenshot.png \n");
	printf("* @ eg. -g -c E:\\3.png -w AcrobatSDIWindow -m 10*10*700*800 -n 10*10*10*10 -o E:\different.png -s\n");
	printf("* @ eg. -g -c -w App0ClassOrTitle*App1ClassOrTitle -m 10*10*700*800*20*20*700*800 -n 10*10*200*400 -o e:\\different.png \n");
	printf("/*********************************************************************/\n");
}

void ParseParam::SetGetOptStr(char* optstr)
{
	this->optstr = optstr;
}

int ParseParam::CallGetOpt(void)
{
	return getopt(this->argc, this->argv, this->optstr);
}

ArgValues ParseParam::GetResult(void)
{
	for (int i = 0; i < this->ArgValue.ActionFlag.size(); i++) {
		if (this->ArgValue.ActionFlag[i] == FXQA_FUNC_COMPAREIMG)
			if (this->ArgValue.Color ==  '\0')
				this->ArgValue.Color = "1";
	}
	
	return this->ArgValue;
}

void ParseParam::CleanVal(void)
{
	this->ArgValue.ActionFlag.clear();
	this->ArgValue.Color = "\n";
	this->ArgValue.ErrLimit = "\n";
	this->ArgValue.ImgName0 = "\n";
	this->ArgValue.ImgName1 = "\n";
	this->ArgValue.ImgOutName = "\n";
	this->ArgValue.CompareSize = "\n";
	this->ArgValue.AdjustOffset = "\n";
	this->ArgValue.ProgramVal = "\n";
	this->ArgValue.ScreenShotSize = "\n";
	this->ArgValue.ShowImg = false;
	this->ArgValue.bForceChangeSize = false;
}

void ParseParam::ParseVal(int c, int i)
{
//	this->CleanVal();
	switch(c) {
	case 'c':
		if (i == 0) {
			this->ArgValue.ActionFlag.push_back(FXQA_FUNC_COMPAREIMG);
			this->ArgValue.ImgName0 = this->argv[2];
			this->ArgValue.ImgName1 = this->argv[3];
		}
		break;
	case 'g':
		this->ArgValue.ActionFlag.push_back(FXQA_FUNC_GETDESKTOPIMG);
		break;
	case 'o':
		this->ArgValue.ImgOutName = optarg;
		break;
	case 's':
		this->ArgValue.ShowImg = true;
		break;
	case 'p':
		this->ArgValue.Color = optarg;
		break;
	case 'e':
		this->ArgValue.ErrLimit = optarg;
		break;
	case 'n':
		this->ArgValue.CompareSize = optarg;
		break;
	case 'm':
		this->ArgValue.ScreenShotSize = optarg;
		break;
	case 'h':
		this->HelpInfo();
		break;
	case 'a':
		this->ArgValue.AdjustOffset = optarg;
		break;	
	case 'i':
		this->ArgValue.ActionFlag.push_back(FXQA_FUNC_COMPOSE);
		this->ArgValue.ImgName0 = this->argv[2];
		this->ArgValue.ImgName1 = this->argv[3];
		break;	
 	case 'w':
 		this->ArgValue.ProgramVal = optarg;
 		break;
	case 'f':
		if (i == 0) {
			this->ArgValue.ActionFlag.push_back(FXQA_FUNC_FINDIMG);
			this->ArgValue.ImgName0 = this->argv[2];
		} else {
			this->ArgValue.bForceChangeSize = true;
		}
		break;
	}
}

vector<int> ParseParam::ParseInputNValue(char* StrSize)
{
	vector<int> Sizes;
	string strFull = StrSize;
	string strTem = strFull;
	string strNum = "";
	int pos = 0;
	while (1) {
		int sizeNum = -1;
		int iNumLen = strTem.find("*");
		if (iNumLen == -1) {
			SSCANF(strTem.c_str(), "%i", &sizeNum);
			Sizes.push_back(sizeNum);
			break;
		}

		strNum = strTem.substr(0, iNumLen);
		SSCANF(strNum.c_str(), "%i", &sizeNum);
		Sizes.push_back(sizeNum);

		strTem = strTem.substr(iNumLen + 1);
	}
	return Sizes;
}

vector<int> ParseParam::ParseInputAValue(char* StrSize)
{
	vector<int> Sizes;
	char *pi, *p;
	char *po = (char *)malloc(sizeof(char)*5);
	char *pDst = (char *)malloc(sizeof(char)*5);

	memset(po, 0, strlen(po));
	int nSrcLen;

	nSrcLen = strlen(StrSize);
	pi = StrSize;
	int x = -1;
	while (1) {
		p = strstr(pi, "*");
		if (p == NULL) {
			char* tem = strstr(pi, "+");
			x = atoi(pi);
			if (tem == NULL)
				x = -x;
			if (abs(x) < 10)
				Sizes.push_back(x);
			break;
		}
		int len = nSrcLen - strlen(p);

		memcpy(po, pi, len);
		char* tem = strstr(po, "+");
		x = atoi(po);
		if (tem == NULL)
			x = -x;
		//  		if (x == 0)
		//  			return Sizes;
		if (abs(x) < 10)
			Sizes.push_back(x);
		p++;
		nSrcLen = strlen(p);
		pi = p;
		memset(po, 0, strlen(po));
	}
	return Sizes;
}

vector<string>	ParseParam::ParseInputWValue(char* StrInput)
{
	vector<string> VProgramVal;
	string strStore = "";
	string temStore = "";

	if (strcmp(StrInput, "\n") == 0)
		return VProgramVal;

	strStore = StrInput;
	int splitPos = strStore.find("*");

	if (splitPos == -1) {
		VProgramVal.push_back(strStore);
		return VProgramVal;
	}

	temStore = strStore.substr(0, splitPos);
	VProgramVal.push_back(temStore);

	temStore = strStore.substr(splitPos + 1, strStore.length());
	VProgramVal.push_back(temStore);

	return VProgramVal;
}