
#include "stdafx.h"

#ifndef _CHARSET_H_
#define _CHARSET_H_



int U2A(const wchar_t* szU,char* szA,size_t cnt);
int A2U(const char* szA,wchar_t* szU,size_t cnt);

/** 
* Wide-character to string
* @param[in] wide-character
* @return string
*/
std::string  U2A(const wchar_t* szU);
/** 
* String to wide-character
* @param[in] string
* @return wide-character
*/
std::wstring A2U(const char* szA);

/** 
* Utf-8 to string
* @param[in] uft-8
* @return string
*/
std::string Utf8ToStr(char* Utf8);

/** 
* Utf-8 to unicode
* @param[in] uft-8
* @param[out] string
*/
void UTF_8ToUnicode(wchar_t* pOut,char *pText);

/** 
* Unicode to utf8
* @param[in] utf8
* @param[out] string
*/
void UnicodeToUTF_8(wchar_t* pText, char* pOut);

/** 
* Unicode to GB2312
* @param[in] unicode
* @param[out] GB2312
*/
void UnicodeToGB2312(wchar_t uData, char* pOut);  

/** 
* GB2312 to Unicode
* @param[in] GB2312
* @param[out] unicode
*/
void Gb2312ToUnicode(char *gbBuffer, wchar_t* pOut);

/** 
* GB2312 to Utf8
* @param[in] GB2312
* @param[in] length
* @param[out] utf8
*/
void GB2312ToUTF_8(char *pText, int pLen, std::string& pOut);


/** 
* Utf8 to GB2312
* @param[in] utf8
* @param[in] length
* @param[out] gb2312
*/
void UTF_8ToGB2312(char *pText, int pLen, std::string &pOut);
#endif //#define _CHARSET_H_