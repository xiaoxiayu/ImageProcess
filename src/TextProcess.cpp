
#include "TextProcess.h"


void TextProcess::Test1()
{  
	wchar_t buf[1000];
	int i=1000;

	GetCurrentDirectory(1000,buf);

	char *outText;
	tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
	// Initialize tesseract-ocr with English, without specifying tessdata path
	if (api->Init(NULL, "eng")) {
		fprintf(stderr, "Could not initialize tesseract.\n");
		exit(1);
	}

	// Open input image with leptonica library
	Pix *image = pixRead("E:\\fff.jpg");
	
	api->SetImage(image);
	// Get OCR result
	outText = api->GetUTF8Text();

	string tem = Utf8ToStr(outText);
	printf("OCR output:\n%s", tem.c_str());

	// Destroy used object and release memory
	api->End();
	delete [] outText;
	pixDestroy(&image);
}