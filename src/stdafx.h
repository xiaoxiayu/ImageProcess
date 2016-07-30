
#pragma once
#ifndef _STDAFX_H_
#define _STDAFX_H_


#if defined(_WIN32) || defined(_WIN64)
	#include <Windows.h>
    #include "getopt.h"
#else if defined(__linux__)
    #include <getopt.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <cctype>
#include <iostream>

//using namespace std;
#ifdef _DEBUG
#define _DEBUG_OUTPUT_INFO_
#endif
#ifdef _DEBUG_OUTPUT_INFO_
#define _DEBUG_INFO(info)	printf(##info##);
#define _DEBUG_INFO_S(info, param0)	printf("%s %s", info, param0);
#define _DEBUG_INFO_D(info, param0)	printf("%s %d", info, param0);

#else
#define _DEBUG_INFO(info)
#define _DEBUG_INFO_S(info, param0)
#define _DEBUG_INFO_D(info, param0)
#endif

#define GET_STRINGFILE_SUFFIX(path)	path.substr(path.rfind(".") + 1, path.length());
#endif