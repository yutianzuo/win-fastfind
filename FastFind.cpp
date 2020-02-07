// FastFind.cpp : main source file for FastFind.exe
//

#include "stdafx.h"

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>

#include "resource.h"

#include "aboutdlg.h"
#include "MainDlg.h"

CAppModule _Module;
CString g_cstr_foldername;
bool g_b_starthide;

BOOL IsInstanceExist()
{
	HANDLE h_mutex = CreateMutex(NULL, TRUE, L"360FastFindSingle");
	if (!h_mutex)
		return FALSE;

	if (GetLastError() != ERROR_ALREADY_EXISTS)
		return FALSE;

	HWND hWnd = ::FindWindow(L"#32770", L"360搜索");
	if (!hWnd)
	{
		return TRUE;
	}
	::ShowWindow(hWnd, SW_RESTORE);
	::SetForegroundWindow(hWnd);
	::BringWindowToTop(hWnd);
	::SetActiveWindow(hWnd);
	::SetForegroundWindow(hWnd);


	if (g_cstr_foldername.GetLength())
	{
		ConvertToLenPath(g_cstr_foldername);
		g_cstr_foldername.MakeLower();
		COPYDATASTRUCT CopyData;
		CopyData.dwData = 'FAST';
		CopyData.cbData = (g_cstr_foldername.GetLength() + 1) * sizeof(WCHAR);
		CopyData.lpData = (void*)((LPCWSTR)g_cstr_foldername);
		::SendMessage(hWnd, WM_COPYDATA, NULL, (LPARAM)&CopyData);
	}
	else
	{
		COPYDATASTRUCT CopyData;
		CopyData.dwData = 'FAST';
		CopyData.cbData = 0;
		CopyData.lpData = (void*)NULL;
		::SendMessage(hWnd, WM_COPYDATA, NULL, (LPARAM)&CopyData);
	}

	return TRUE;
}

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CMainDlg dlgMain;

	if(dlgMain.Create(NULL) == NULL)
	{
		ATLTRACE(_T("Main dialog creation failed!\n"));
		return 0;
	}

	dlgMain.ShowWindow(nCmdShow);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}
/*
int ShowProcessIntegrityLevel() //通过进程完整性来判断进程是否高权限，system或者普通权限
{
	int n_ret = -1;
	HANDLE hToken;
	HANDLE hProcess;

	DWORD dwLengthNeeded;
	DWORD dwError = ERROR_SUCCESS;

	PTOKEN_MANDATORY_LABEL pTIL = NULL;
	LPWSTR pStringSid;
	DWORD dwIntegrityLevel;

	hProcess = GetCurrentProcess();
	if (OpenProcessToken(hProcess, TOKEN_QUERY, &hToken)) 
	{
		// Get the Integrity level.
		if (!GetTokenInformation(hToken, TokenIntegrityLevel, 
			NULL, 0, &dwLengthNeeded))
		{
			dwError = GetLastError();
			if (dwError == ERROR_INSUFFICIENT_BUFFER)
			{
				pTIL = (PTOKEN_MANDATORY_LABEL)LocalAlloc(0, 
					dwLengthNeeded);
				if (pTIL != NULL)
				{
					if (GetTokenInformation(hToken, TokenIntegrityLevel, 
						pTIL, dwLengthNeeded, &dwLengthNeeded))
					{
						UCHAR count = *GetSidSubAuthorityCount(pTIL->Label.Sid);
						dwIntegrityLevel = *GetSidSubAuthority(pTIL->Label.Sid, 
							(DWORD)(count-1));

						if (dwIntegrityLevel == SECURITY_MANDATORY_LOW_RID)
						{
							// Low Integrity
							n_ret = 1;
						}
						else if (dwIntegrityLevel >= SECURITY_MANDATORY_MEDIUM_RID && 
							dwIntegrityLevel < SECURITY_MANDATORY_HIGH_RID)
						{
							// Medium Integrity
							n_ret = 2;
						}
						else if (dwIntegrityLevel >= SECURITY_MANDATORY_HIGH_RID)
						{
							// High Integrity
							n_ret = 3;
						}
						else if (dwIntegrityLevel >= SECURITY_MANDATORY_SYSTEM_RID)
						{
							// System Integrity
							n_ret = 4;
						}
					}
					LocalFree(pTIL);
				}
			}
		}
		CloseHandle(hToken);
	}

	return n_ret;
}
*/
BOOL IsElevated( BOOL * pbElevated ) /*另一种判断是否高低权限的办法*/
{
	if( !is_vista_or_later() )
	{
		if ( pbElevated)
			*pbElevated = TRUE;
		return TRUE ;
	}
	HRESULT hResult = E_FAIL; // assume an error occured
	HANDLE hToken	= NULL;
	BOOL bRet = FALSE;
	if ( !::OpenProcessToken( 
		::GetCurrentProcess(), 
		TOKEN_QUERY, 
		&hToken ) )
	{
		return FALSE ;
	}

	TOKEN_ELEVATION te = { 0 };
	DWORD dwReturnLength = 0;

	if ( !::GetTokenInformation(
		hToken,
		TokenElevation,
		&te,
		sizeof( te ),
		&dwReturnLength ) )
	{
		DebugBreak();
	}
	else
	{
		if (dwReturnLength != sizeof( te ))
		{
			DebugBreak();
		}

		bRet = TRUE ; 

		if ( pbElevated)
			*pbElevated = (te.TokenIsElevated != 0);
	}

	::CloseHandle( hToken );

	return bRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	/*写注册表*/
	wchar_t wsz_temp[_MAX_PATH] = {0};
	if (0 == ::GetModuleFileNameW(NULL, wsz_temp, _MAX_PATH))
	{
		return false;
	}
	stringxw wstr_path(wsz_temp);
	stringxw wstr_write_value;
	wstr_write_value.format(L"\"%s\" -foldername \"%%1\"", wstr_path.c_str());
	write_register(HKEY_CLASSES_ROOT, L"Folder\\shell\\360搜索", L"", L"");
	write_register(HKEY_CLASSES_ROOT, L"Folder\\shell\\360搜索\\command", L"", L"");
	write_register(HKEY_CLASSES_ROOT, L"Folder\\shell\\360搜索\\command", L"", wstr_write_value.c_str());

	write_register(HKEY_LOCAL_MACHINE, L"software\\classes\\Folder\\shell\\360搜索", L"", L"");
	write_register(HKEY_LOCAL_MACHINE, L"software\\classes\\Folder\\shell\\360搜索\\command", L"", L"");
	write_register(HKEY_LOCAL_MACHINE, L"software\\classes\\Folder\\shell\\360搜索\\command", L"", wstr_write_value.c_str());
	/*写注册表*/
	stringxw wstr_cmd;
	wstr_cmd = lpstrCmdLine;
	wstr_cmd.to_lower();
#ifdef _DEBUG
	::OutputDebugStringW(wstr_cmd.c_str());
#endif

	if (is_vista_or_later())
	{
		BOOL b_elevate = FALSE;
		IsElevated(&b_elevate);
		if (!b_elevate)
		{
			stringxw wstr_path;
			get_app_path(wstr_path);
			wstr_path += L"fastfind.exe";
			if (wstr_cmd.find(L"autorun") != stringxw::npos)
			{
				WipeShellExecuteEx(wstr_path, L"autorun", SW_HIDE, 0, 0, true);
			}
			else
			{
				WipeShellExecuteEx(wstr_path, lpstrCmdLine, SW_SHOW, 0, 0, true);
			}
			
			return 0;
		}
	}


	if (wstr_cmd.find(L"hide") != stringxw::npos || wstr_cmd.find(L"autorun") != stringxw::npos)
	{
		g_b_starthide = true;
	}
	stringxw wstr_sub;
	if (wstr_cmd.find(L"-foldername") != stringxw::npos)
	{
		wstr_sub = wstr_cmd;
		wstr_sub.erase(0, wcslen(L"-foldername"));
		wstr_sub.trim();
		wstr_sub.trim(L'\"');
	}
	if (stringxw::npos == wstr_sub.find(L"{"))
	{
		g_cstr_foldername = wstr_sub.c_str();
		if (g_cstr_foldername.GetLength() && g_cstr_foldername[g_cstr_foldername.GetLength() - 1] != L'\\')
		{
			g_cstr_foldername += L"\\";
		}		
	}
	if (IsInstanceExist())
		return 0;



	HRESULT hRes = ::CoInitialize(NULL);
	// If you are running on NT 4.0 or higher you can use the following call instead to 
	// make the EXE free threaded. This means that calls come in on a random RPC thread.
	//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);

	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();

	return nRet;
}
