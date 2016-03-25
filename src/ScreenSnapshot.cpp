#include "stdafx.h"
#include "ScreenSnapshot.h"
#include "Charset.h"


IplImage* QtSnapshot::GetIplImage()  
{  
	QImage qImage = this->QPImgGrab.toImage();
	int width = qImage.width();  
	int height = qImage.height();  

	CvSize Size = {width, height};  
	IplImage *IplImageBuffer = cvCreateImage(Size, IPL_DEPTH_8U, 3);  
	for (int y = 0; y < height; ++y) {  
		for (int x = 0; x < width; ++x) {  
			QRgb rgb = qImage.pixel(x, y);  
			cvSet2D(IplImageBuffer, y, x, CV_RGB(qRed(rgb), qGreen(rgb), qBlue(rgb)));  
		}  
	}  
	this->OpenCVImg = IplImageBuffer;
	return IplImageBuffer;  
} 

bool QtSnapshot::GrabWindow(char* Size)
{
	int x = 0;int y = 0;
	int iWidth = QApplication::desktop()->width();
	int iHeight = QApplication::desktop()->height();

	vector<int> SizeInfo;
	if (Size != NULL) {
		ParseParam ParamParser;
		SizeInfo = ParamParser.ParseInputNValue(Size);
	}
		
	int iNSize = SizeInfo.size();
	if (iNSize != 1 && iNSize != 4 &&  iNSize != 8 && iNSize != 0)
		return false;

	if (iNSize != 1) {
		x = SizeInfo[0];y = SizeInfo[1];
		iWidth = SizeInfo[2];
		iHeight = SizeInfo[3];

		if (iNSize == 8) {
			string temSize = Size;
			memset(Size, 0, strlen(Size));
			temSize.copy(Size, temSize.length()/2, temSize.length()/2+1);
		}
	}

	QScreen* Myscreen = QGuiApplication::primaryScreen();
	QPixmap pixmap = Myscreen->grabWindow(QApplication::desktop()->winId(), 
		x, y, iWidth, iHeight);
	
	this->QPImgGrab = pixmap;
	if (pixmap.height() == NULL) return false;

	return true;
}

bool QtSnapshot::SaveImage(char* OutPutName)
{
#ifdef WIN32
	std::wstring tem = A2U(OutPutName);
	QString saveName = "";
	saveName = QString::fromStdWString(tem);
	bool bRet = this->QPImgGrab.save(saveName, "png");
#else
	bool bRet = this->QPImgGrab.save(OutPutName, "png");
#endif
	return bRet;
}

//#include <Windows.h>
//void test(WId winId, int x, int y, int w, int h )
//{
//	RECT r;
//	GetClientRect(winId, &r);
//
//	if (w < 0) w = r.right - r.left;
//	if (h < 0) h = r.bottom - r.top;
//
//#ifdef Q_WS_WINCE_WM
//	if (qt_wince_is_pocket_pc()) {
//		QWidget *widget = QWidget::find(winId);
//		if (qobject_cast<QDesktopWidget *>(widget)) {
//			RECT rect = {0,0,0,0};
//			AdjustWindowRectEx(&rect, WS_BORDER | WS_CAPTION, FALSE, 0);
//			int magicNumber = qt_wince_is_high_dpi() ? 4 : 2;
//			y += rect.top - magicNumber;
//		}
//	}
//#endif
//
//	// Create and setup bitmap
//	HDC display_dc = GetDC(0);
//	HDC bitmap_dc = CreateCompatibleDC(display_dc);
//	HBITMAP bitmap = CreateCompatibleBitmap(display_dc, w, h);
//	HGDIOBJ null_bitmap = SelectObject(bitmap_dc, bitmap);
//
//	// copy data
//	HDC window_dc = GetDC(winId);
//	BitBlt(bitmap_dc, 0, 0, w, h, window_dc, x, y, SRCCOPY
//#ifndef Q_WS_WINCE
//		| CAPTUREBLT
//#endif
//		);
//
//	// clean up all but bitmap
//	ReleaseDC(winId, window_dc);
//	SelectObject(bitmap_dc, null_bitmap);
//	DeleteDC(bitmap_dc);
//
//	QPixmap pixmap = QPixmap::fromWinHBITMAP(bitmap);
//
//	DeleteObject(bitmap);
//	ReleaseDC(0, display_dc);
//
//	return pixmap;
//}