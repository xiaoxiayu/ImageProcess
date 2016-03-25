#pragma once

#include "ParseParam.h"
//#include <QtCore/QCoreApplication>
//#include <QtGui/QGuiApplication>
#include <QtGui/QPixmap>
#include <QtGui/QScreen>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QTabWidget>
#include "ImageProcess.h"

class QtSnapshot:public QTabWidget
{
private:
	int argc;
	char **argv;
	QPixmap		QPImgGrab;
	IplImage*   OpenCVImg;
	
public:
	bool			GrabWindow(char* Size=NULL);
	bool			SaveImage(char* OutPutName);
	IplImage*		GetIplImage(); 
};

