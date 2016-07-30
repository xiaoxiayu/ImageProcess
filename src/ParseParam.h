#pragma once

#ifndef _PARASEPARAM_H_
#define _PARASEPARAM_H_

#ifdef _MSC_VER
#define SSCANF sscanf_s
#else
#define SSCANF sscanf
#endif

/** 
* @ Define for first flag
* @ 
**/
#define FXQA_FUNC_CTLFLAG				0
#define FXQA_FUNC_COMPAREIMG			1
#define FXQA_FUNC_GETDESKTOPIMG			2
#define FXQA_FUNC_COMPOSE				3
#define FXQA_FUNC_FINDIMG				4
#define FXQA_FUNC_OCR					5

/** 
* @ Define for second flag
* @ 
**/
#define FXQA_COLORIMG				0x0001
#define FXQA_OUTPUTIMG				0x0010
#define FXQA_SHOWIMG				0x0100

/**@ActionFlag;
* @16bit, 0000 0000 0000 0000;
* @Low 4 bit, Fist control flag, eg. -c, -p;
* @Low 8 bit, Second control flag, -lunkuo;
* @High 4 bit, Third control flag, eg. -o;
* @High 8 bit, Fouth control flag, eg. -s;
**/
typedef struct ArgvParamNValue
{
	// 16bit,Low 4bit to set first flag
	std::vector<int>				ActionFlag;
	
	// For image compare
	char*					Color;
	char*					ErrLimit;
	char*					ImgName0;
	char*					ImgName1;
	char*					ImgOutName;
	char*					CompareSize;
	char*					AdjustOffset;
	bool					ShowImg;

	//For screen shot
	char*					ProgramVal;
	char*					ScreenShotSize;

	bool					bForceChangeSize;

	// OCR compare text
	char*					OcrCompareText;
}ArgValues;

class ParseParam
{
public:
	ParseParam(void);
	ParseParam(int argc, char **argv);
	~ParseParam(void);
private:
	int						argc;
	char					**argv;
	char*					optstr;
	int						gCtlFlag;
	

public:
	ArgValues					ArgValue; 

	void						SetGetOptStr(char* optstr);
	int							CallGetOpt(void);
	void						ParseVal(int, int);
	ArgValues					GetResult(void);
	void						CleanVal(void);
	void						HelpInfo(void);

	std::vector<int>					ParseInputAValue(char* StrSize);
	std::vector<int>					ParseInputNValue(char* StrSize);
	std::vector<std::string>				ParseInputWValue(char* StrInput);
};

#endif