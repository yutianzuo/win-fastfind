// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//  are changed infrequently
//

#pragma once

#ifndef WINVER                          // Specifies that the minimum required platform is Windows Vista.
#define WINVER 0x0501           // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT            // Specifies that the minimum required platform is Windows Vista.
#define _WIN32_WINNT 0x0501     // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINDOWS          // Specifies that the minimum required platform is Windows 98.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE                       // Specifies that the minimum required platform is Internet Explorer 7.0.
#define _WIN32_IE 0x0700        // Change this to the appropriate value to target other versions of IE.
#endif

#include <atlbase.h>
#include <atlstr.h>
#include <atlapp.h>
#include <atltypes.h>

extern CAppModule _Module;
extern CString g_cstr_foldername;
extern bool g_b_starthide;


#include "mft/miscs.h"




#if defined _M_IX86
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif


inline
void ConvertToLenPath(CString& cstr_path)
{
	WCHAR* wsz_buff = new (std::nothrow) WCHAR[MAXWORD];
	if (wsz_buff)
	{
		memset(wsz_buff, 0, MAXWORD * sizeof(WCHAR));
		::GetLongPathName(cstr_path, wsz_buff, MAXWORD);
		cstr_path = wsz_buff;
		delete [] wsz_buff;
	}	
}

class CCriticalSectionWrapper
{
public:
	CCriticalSectionWrapper()
	{
		InitializeCriticalSection(&m_cs);
	}

	virtual ~CCriticalSectionWrapper()
	{
		DeleteCriticalSection(&m_cs);
	}

	void lock()
	{
		EnterCriticalSection(&m_cs);
	}

	void unlock()
	{
		LeaveCriticalSection(&m_cs);
	}
protected:
	CRITICAL_SECTION m_cs;
};
