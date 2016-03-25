
#ifndef _WINDOWSHND_H_
#define _WINDOWSHND_H_

#include "stdafx.h"
#include "ParseParam.h"
#include "Charset.h"

class CWindowCtrl
{
public:
	CWindowCtrl() { m_strChildSize = "";};
	~CWindowCtrl() {};

	void ShowTestWindow(const char* ProgramVal);
	void HideTestWindow(const char* ProgramVal);

	char* GetChildWindowSize() {return (char*)m_strChildSize.c_str();};
private:
	string m_strChildSize;
};


#endif