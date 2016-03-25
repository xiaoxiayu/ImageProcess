
#include "Charset.h"

int U2A(const wchar_t* szU,char* szA,size_t cnt)
{
	return WideCharToMultiByte (CP_ACP, 0, szU, -1, szA, cnt, NULL, NULL) ;
}
int A2U(const char* szA,wchar_t* szU,size_t cnt)
{
	return MultiByteToWideChar (CP_ACP, 0, szA, -1, szU, cnt) ;
}
/** 
* Wide-character to string
* @param[in] wide-character
* @return string
*/
std::string  U2A(const wchar_t* szU)
{
	int nRetCode=U2A(szU,0,0);
	if(0==nRetCode)
		return std::string();
	std::string str(nRetCode-1,'\0');
	U2A(szU,(char*)(str.c_str()),nRetCode);
	return str;
}
/** 
* String to wide-character
* @param[in] string
* @return wide-character
*/
std::wstring A2U(const char* szA)
{
	int nRetCode=A2U(szA,0,0);
	if(0==nRetCode)
		return std::wstring();
	std::wstring str(nRetCode-1,'\0');
	A2U(szA,(wchar_t*)(str.c_str()),nRetCode);
	return str;
}

/** 
* Utf-8 to string
* @param[in] uft-8
* @return string
*/
string Utf8ToStr(char* Utf8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)Utf8, -1, NULL,0);
	WCHAR * Unicode = new WCHAR[len+1];
	memset(Unicode, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)Utf8, -1, Unicode, len);
	string FileName = U2A(Unicode);
	delete Unicode;
	return FileName;
}

/** 
* Utf-8 to unicode
* @param[in] uft-8
* @param[out] string
*/
void UTF_8ToUnicode(wchar_t* pOut,char *pText)
{
	char* uchar = (char *)pOut;

	uchar[1] = ((pText[0] & 0x0F) << 4) + ((pText[1] >> 2) & 0x0F);
	uchar[0] = ((pText[1] & 0x03) << 6) + (pText[2] & 0x3F);

	return;
}

/** 
* Unicode to utf8
* @param[in] utf8
* @param[out] string
*/
void UnicodeToUTF_8(wchar_t* pText, char* pOut)
{
	// 注意 WCHAR高低字的顺序,低字节在前，高字节在后
	char* pchar = (char *)pText;

	pOut[0] = (0xE0 | ((pchar[1] & 0xF0) >> 4));
	pOut[1] = (0x80 | ((pchar[1] & 0x0F) << 2)) + ((pchar[0] & 0xC0) >> 6);
	pOut[2] = (0x80 | (pchar[0] & 0x3F));

	return;
}

/** 
* Unicode to GB2312
* @param[in] unicode
* @param[out] GB2312
*/
void UnicodeToGB2312(wchar_t uData, char* pOut)
{
	WideCharToMultiByte(CP_ACP,NULL,&uData,1,pOut,sizeof(wchar_t),NULL,NULL);
	return;
}     

/** 
* GB2312 to Unicode
* @param[in] GB2312
* @param[out] unicode
*/
void Gb2312ToUnicode(char *gbBuffer, wchar_t* pOut)
{
	::MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,gbBuffer,2,pOut,1);
	return ;
}

/** 
* GB2312 to Utf8
* @param[in] GB2312
* @param[in] length
* @param[out] utf8
*/
void GB2312ToUTF_8(char *pText, int pLen, string& pOut)
{
	char buf[4];
	int nLength = pLen* 3;
	char* rst = new char[nLength];

	memset(buf,0,4);
	memset(rst,0,nLength);

	int i = 0;
	int j = 0;      
	while(i < pLen)
	{
		//如果是英文直接复制就可以
		if( *(pText + i) >= 0)
		{
			rst[j++] = pText[i++];
		}
		else
		{
			wchar_t pbuffer;
			Gb2312ToUnicode(pText+i, &pbuffer);

			UnicodeToUTF_8(&pbuffer, buf);

			unsigned short int tmp = 0;
			tmp = rst[j] = buf[0];
			tmp = rst[j+1] = buf[1];
			tmp = rst[j+2] = buf[2];    

			j += 3;
			i += 2;
		}
	}
	rst[j] = '\0';

	//返回结果
	pOut = rst;             
	delete []rst;   

	return;
}

/** 
* Utf8 to GB2312
* @param[in] utf8
* @param[in] length
* @param[out] gb2312
*/
void UTF_8ToGB2312(char *pText, int pLen, string &pOut)
{
	char * newBuf = new char[pLen];
	char Ctemp[4];
	memset(Ctemp,0,4);

	int i =0;
	int j = 0;

	while(i < pLen)
	{
		if(pText[i] > 0)
		{
			newBuf[j++] = pText[i++];                       
		}
		else                 
		{
			WCHAR Wtemp;
			UTF_8ToUnicode(&Wtemp,pText + i);

			UnicodeToGB2312(Wtemp, Ctemp);

			newBuf[j] = Ctemp[0];
			newBuf[j + 1] = Ctemp[1];

			i += 3;    
			j += 2;   
		}
	}
	newBuf[j] = '\0';
	pOut = newBuf;
	delete []newBuf;

	return; 
}