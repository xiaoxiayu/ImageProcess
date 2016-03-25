#pragma once

#ifndef _TEXTPROCESS_H_
#define _TEXTPROCESS_H_

#include "stdafx.h"
#include "ParseParam.h"
#include "Charset.h"

//OCR
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>


class TextProcess
{
public:
	TextProcess() {};
	~TextProcess() {};

	void	Test1();
};

#endif