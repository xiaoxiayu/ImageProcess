
#include "WindowsHND.h"


bool CALLBACK EnumTestChildrenWindow(HWND childWND, LPARAM myData)
{
	wchar_t* winTxt = new wchar_t[50];
	memset(winTxt, 0, 50);
	wchar_t* winClass = new wchar_t[50];
	memset(winClass, 0, 50);

	GetClassName(childWND, winClass, 50);
	GetWindowText(childWND, winTxt, 50);

	if (wcscmp(winTxt, L"Reader") == 0 
		|| wcscmp(winTxt, L"AVPageView") == 0
		|| wcscmp(winClass, L"Chrome_RenderWidgetHostHWND") == 0) {
		WINDOWINFO winInfo;
		GetWindowInfo(childWND, &winInfo);

		char* temNum = new char[6];

		memset(temNum, 0, 6);
		sprintf_s(temNum, 6, "%d", winInfo.rcWindow.left);
		strcpy((char*)myData, temNum);
		strcat((char*)myData, "*");

		memset(temNum, 0, 6);
		sprintf_s(temNum, 6, "%d", winInfo.rcWindow.top);
		strcat((char*)myData, temNum);
		strcat((char*)myData, "*");

		memset(temNum, 0, 6);
		sprintf_s(temNum, 6, "%d", (winInfo.rcWindow.right - winInfo.rcWindow.left));
		strcat((char*)myData, temNum);
		strcat((char*)myData, "*");

		memset(temNum, 0, 6);
		sprintf_s(temNum, 6, "%d", (winInfo.rcWindow.bottom - winInfo.rcWindow.top));
		strcat((char*)myData, temNum);
		
		delete[] winTxt;
		delete[] temNum;
		return false;
	}

	delete[] winTxt;
	delete[] winClass;
	return true;
}

void CWindowCtrl::ShowTestWindow(const char* ProgramVal)
{
	int cnt = strlen(ProgramVal);

	wchar_t* wProgramVal = new wchar_t[cnt + 1];
	wProgramVal[cnt] = 0;

	A2U(ProgramVal, wProgramVal, cnt);

	HWND myWindow = FindWindow(NULL, wProgramVal);
	if (myWindow == NULL)
		myWindow = FindWindow(wProgramVal, NULL);
	//HWND myWindow = FindWindow(NULL, L"Want that Chrome Gone? - Google Chrome");
	if (myWindow == NULL) {
		printf("%s not found.\n", ProgramVal);
		return;
	}

	WINDOWINFO winInfo;
	GetWindowInfo(myWindow, &winInfo);

	ShowWindow(myWindow, SW_MAXIMIZE);
	
	EnumChildWindows(myWindow, (WNDENUMPROC)EnumTestChildrenWindow, (LPARAM)m_strChildSize.c_str());

	SetForegroundWindow(myWindow);

	delete[] wProgramVal;
	return;
}

void CWindowCtrl::HideTestWindow(const char* ProgramVal)
{
	int cnt = strlen(ProgramVal);

	wchar_t* wProgramVal = new wchar_t[cnt + 1];
	wProgramVal[cnt] = 0;

	A2U(ProgramVal, wProgramVal, cnt);

	HWND myWindow = FindWindow(wProgramVal, NULL);

	WINDOWINFO winInfo;
	GetWindowInfo(myWindow, &winInfo);

	ShowWindow(myWindow, SW_MINIMIZE);
	
	delete[] wProgramVal;
	return;
}