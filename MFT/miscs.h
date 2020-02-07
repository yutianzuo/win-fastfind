/*
 * Copyright(C) 2009,yutianzuo (All rights reserved.)
 * 
 * FileDes  : miscs.h
 * Abstract : file and directory functions
 * Version  : 1.0 
 * Author   : yutianzuo
 * Date     : 2009-5-12
 * 
 */
#ifndef __MISCS__H__TIMOTHY__
#define __MISCS__H__TIMOTHY__
//------------------------------------------------------------------------
#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif
#include <atlbase.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <tlhelp32.h>
#include <Shellapi.h>
#include <ShObjIdl.h>
#include <ShlGuid.h>
#include <shlobj.h>
#include <Psapi.h>
#include <Sddl.h>

#include "string_x.h"

#pragma warning(disable:4311)
#pragma warning(disable:4312)
#pragma warning(disable:4996)

#ifndef FUNCTION_BEGIN
#define FUNCTION_BEGIN do{
#endif

#ifndef FUNCTION_LEAVE
#define FUNCTION_LEAVE break
#endif

#ifndef FUNCTION_END
#define FUNCTION_END }while(false)
#endif
#pragma comment(lib, "Psapi.lib")


typedef struct
{
	unsigned int n_width;
	unsigned int n_heigh;
}tag_image_info;

inline 
int dir_file_exist(const wchar_t* wsz_file)
{
	stringxw wstr_file;
	if (!wsz_file || (wstr_file.assign(wsz_file)).size() == 0)
	{
		return 0;
	}
	wstr_file.erase(0, wstr_file.find_first_not_of(L' '));
	wstr_file.erase(wstr_file.find_last_not_of(L' ') + 1);
	wstr_file.erase(wstr_file.find_last_not_of(L'\\') + 1);

	struct _stat buf;
	int ret = _wstat(wstr_file.c_str(), &buf);
	if (-1 == ret) //does not exist 
	{
		return -1;
	}
	if (_S_IFREG & buf.st_mode) //file
	{
		return 1;
	}
	if (_S_IFDIR & buf.st_mode) //dir
	{
		return 2;
	}
	return 0; //other type
}



inline 
bool mkdir_recursively(const wchar_t* wsz_path_dir) //input should be a directory full path
{
	if (!wsz_path_dir)
	{
		return false;
	}
	stringxw wstr_path(wsz_path_dir);
	std::vector<stringxw> vec_part;
	if (0 != wstr_path.split_string(L"\\", vec_part))
	{
		if (0 != wstr_path.split_string(L"/", vec_part))
		{
			return false;
		}
	}
	if (1  == vec_part.size())
	{
		return true;
	}
	stringxw wstr_head = vec_part[0];
	for (std::size_t i = 1; i < vec_part.size(); ++i)
	{
		wstr_head += L"\\";
		wstr_head += vec_part[i];
		if (2 != dir_file_exist(wstr_head.c_str()))
		{
			if (-1 == _wmkdir(wstr_head.c_str()))
			{
				return false;
			}
		}
	}
	return true;
}

inline 
bool mkfile(const wchar_t* wsz_path_file) //input should be a file full path
{
	if (!wsz_path_file)
	{
		return false;
	}
	if (1 == dir_file_exist(wsz_path_file))
	{
		return true;
	}
	stringxw wstr_path(wsz_path_file);
	std::vector<stringxw> vec_part;
	if (0 != wstr_path.split_string(L"\\", vec_part))
	{
		if (0 != wstr_path.split_string(L"/", vec_part))
		{
			return false;
		}
	}
	if (1 == vec_part.size())
	{
		return false;
	}
	stringxw wstr_dir;
	for (std::size_t i = 0; i < vec_part.size() - 1; ++i)
	{
		wstr_dir += vec_part[i];
		wstr_dir += L"\\";
	}
	if (!mkdir_recursively(wstr_dir.c_str()))
		return false;
	wstr_dir += vec_part[vec_part.size() - 1];
	FILE* file_temp = NULL;
	if (NULL == (file_temp = _wfopen(wstr_dir.c_str(), L"w")))
	{
		return false;
	}
	if (file_temp)
	{
		fclose(file_temp);
	}
	return true;
}

inline
bool get_user_path(std::wstring& wstr_ini_path, bool current_or_all = true) //current_or_all=true means current user
{
	wchar_t wsz_temp[MAX_PATH] = {0};
	int nFlag = CSIDL_COMMON_APPDATA;
	if (current_or_all)
		nFlag = CSIDL_APPDATA;
	HRESULT hr = SHGetFolderPathW(NULL, nFlag | CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, wsz_temp);
	if (SUCCEEDED(hr))
	{
		wstr_ini_path = wsz_temp;
	}

	if (FAILED(hr) && current_or_all)
	{
		wchar_t wsz_ev_tmp[MAX_PATH] = {0};
		DWORD dw_ret = ::ExpandEnvironmentStringsW(L"%appdata%", wsz_ev_tmp, MAX_PATH);
		if (0 == dw_ret)
		{
			return false;
		}
		wchar_t* wsz_ev_new_tmp = NULL;
		if (dw_ret >= MAX_PATH)
		{
			wsz_ev_new_tmp = new (std::nothrow) wchar_t[dw_ret + 1];
			if (!wsz_ev_new_tmp)
			{
				return false;
			}
			memset(wsz_ev_new_tmp, 0, (dw_ret + 1) * sizeof(wchar_t));
			::ExpandEnvironmentStringsW(L"%appdata%", wsz_ev_new_tmp, dw_ret + 1);
			wstr_ini_path = wsz_ev_new_tmp;
			delete [] wsz_ev_new_tmp;
		}
		else
		{
			wstr_ini_path = wsz_ev_tmp;
		}
	}

	if (wstr_ini_path[wstr_ini_path.size() - 1] != L'\\')
	{
		wstr_ini_path += L'\\';
	}
	return true;
}

inline 
bool get_user_ini_path(std::wstring& wstr_ini_path, const wchar_t* wsz_file_path, bool current_or_all = true) //current_or_all=true means current user, wsz_file_path should be "kingsoft\\config\\xxx.conf"
{
	if (!get_user_path(wstr_ini_path, current_or_all))
		return false;
	stringxw wstr_part_file(wsz_file_path);
	wstr_part_file.trim();
	wstr_part_file.trim_left(L'\\');
	wstr_part_file.trim_left(L'/');
	wstr_part_file.replace_all(L'/', L'\\');
	wstr_ini_path += wstr_part_file;
	if (!mkfile(wstr_ini_path.c_str()))
	{
		return false;
	}
	return true;
}


inline
bool get_file_size(const wchar_t* wsz_file_path, unsigned long& file_size)
{
	if (!wsz_file_path)
	{
		file_size = 0;
		return false;
	}
	if (1 != dir_file_exist(wsz_file_path))
	{
		return false;
	}
	struct _stat buf;
	int ret = _wstat(wsz_file_path, &buf);
	if (-1 == ret)
	{
		return false;
	}
	file_size = buf.st_size;
	return true;
}

inline 
bool get_file_sizex64(const wchar_t* wsz_file_path, __int64& file_size)
{
	if (!wsz_file_path)
	{
		file_size = 0;
		return false;
	}
// 	if (1 != dir_file_exist(wsz_file_path))
// 	{
// 		return false;
// 	}
	struct _stati64 buf;
	int ret = _wstati64(wsz_file_path, &buf);
	if (-1 == ret)
	{
		return false;
	}
	file_size = buf.st_size;
	return true;
}

inline 
bool is_dir_safe(const std::wstring& wstr_path_in)
{
	bool b_ret = true;
	FUNCTION_BEGIN;
	wchar_t wsz_tmp[_MAX_PATH] = {0};
	if (0 == ::GetWindowsDirectoryW(wsz_tmp, _MAX_PATH))
	{
		b_ret = false;
		FUNCTION_LEAVE;
	}
	stringxw wstr_window_dir(wsz_tmp);
	wstr_window_dir.to_lower();
	stringxw wstr_window_sys = wstr_window_dir + L"\\system32";
	stringxw wstr_window_sys2 = wstr_window_dir + L"\\system32\\";
	stringxw wstr_tmp = wstr_path_in;
	wstr_tmp.to_lower();
	stringxw wstr_root = wstr_window_dir.substr(0, wstr_window_dir.find_first_of(L':') + 1);
	stringxw wstr_root2 = wstr_root + L"\\";
	if (wstr_tmp == wstr_root || wstr_tmp == wstr_root2 || wstr_tmp == wstr_window_dir || wstr_tmp == (wstr_window_dir += L"\\")
		|| wstr_tmp == wstr_window_sys || wstr_tmp == wstr_window_sys2)
	{
		b_ret = false;
		FUNCTION_LEAVE;
	}
	FUNCTION_END;
	return b_ret;
}

inline
int convert_file_size(stringxw& wstr_size)
{
	int n_ret = 0;
	FUNCTION_BEGIN;
	__int64 n64_size = _wtoi64(wstr_size.c_str());
	if (((n64_size / (1024 * 1024 * 1024)) / 1024)) //TB
	{
		wstr_size.format(L"%d TB", ((n64_size / (1024 * 1024 * 1024)) / 1024));
	}
	else if (n64_size / (1024 * 1024 * 1024)) //GB
	{
		wstr_size.format(L"%d GB", n64_size / (1024 * 1024 * 1024));
	}
	else if (n64_size / (1024 * 1024)) //MB
	{
		long double db_tmp = (long double)n64_size;
		db_tmp = db_tmp / (1024 * 1024);
		wstr_size.format(L"%3.1f MB", db_tmp);
	}
	else if (n64_size / 1024) //KB
	{
		long double db_tmp = (long double)n64_size;
		db_tmp = db_tmp / (1024);
		wstr_size.format(L"%3.1f KB", db_tmp);
	}
	else //Bytes
	{
		wstr_size += L" Bytes";
	}
	FUNCTION_END;
	return n_ret;
}

typedef struct
{
	stringxw wstr_create_time;
	stringxw wstr_access_time;
	stringxw wstr_modify_time;
	stringxw wstr_create_time_detail;
	stringxw wstr_file_size;
}tag_file_info;

inline 
int get_file_info(const wchar_t* wsz_file_path, tag_file_info* p_tag_file_info) //file,directory info;include time, size...
{
	int n_ret = 0;
	FUNCTION_BEGIN;
	if (!p_tag_file_info || !wsz_file_path)
	{
		n_ret = -1;
		FUNCTION_LEAVE;
	}
// 	if (-1 == dir_file_exist(wsz_file_path))
// 	{
// 		n_ret = -2;
// 		FUNCTION_LEAVE;
// 	}
	p_tag_file_info->wstr_access_time = L"";
	p_tag_file_info->wstr_create_time = L"";
	p_tag_file_info->wstr_modify_time = L"";
	p_tag_file_info->wstr_create_time_detail = L"";
	p_tag_file_info->wstr_file_size = L"";
	WIN32_FILE_ATTRIBUTE_DATA info_tag = {0};
	DWORD dw_ret = ::GetFileAttributesExW(wsz_file_path, GetFileExInfoStandard, &info_tag);
	if (0 == dw_ret)
	{
		n_ret = -3;
		FUNCTION_LEAVE;
	}
	SYSTEMTIME sys_time = {0};
	FILETIME local_file_time = {0};
	if (::FileTimeToLocalFileTime(&info_tag.ftCreationTime, &local_file_time))
	{
		::FileTimeToSystemTime(&local_file_time, &sys_time);
		p_tag_file_info->wstr_create_time.format(L"%d-%.2d-%.2d", sys_time.wYear, sys_time.wMonth, sys_time.wDay);
		p_tag_file_info->wstr_create_time_detail.format(L"%.2d:%.2d:%.2d", sys_time.wHour, sys_time.wMinute, sys_time.wSecond);
	}
	if (::FileTimeToSystemTime(&info_tag.ftLastAccessTime, &sys_time))
	{
		p_tag_file_info->wstr_access_time.format(L"%d-%.2d-%.2d", sys_time.wYear, sys_time.wMonth, sys_time.wDay);
	}
	if (::FileTimeToSystemTime(&info_tag.ftLastWriteTime, &sys_time))
	{
		p_tag_file_info->wstr_modify_time.format(L"%d-%.2d-%.2d", sys_time.wYear, sys_time.wMonth, sys_time.wDay);
	}
	if (info_tag.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		
		p_tag_file_info->wstr_file_size = L"ÎÄ¼þ¼Ð";
	}
	else
	{
		__int64 n_file_size = 0;
		__int64 n_high = info_tag.nFileSizeHigh;
		n_high = n_high << 32;
		n_file_size |= n_high;
		n_file_size |= info_tag.nFileSizeLow;
		p_tag_file_info->wstr_file_size.format(L"%I64d", n_file_size);
		convert_file_size(p_tag_file_info->wstr_file_size);
	}

	FUNCTION_END;
	return n_ret;
}

inline 
bool get_app_path(std::wstring& wstr_app_path, HMODULE hm_handle = NULL)
{
	wchar_t wsz_temp[_MAX_PATH] = {0};
	if (0 == ::GetModuleFileNameW(hm_handle, wsz_temp, _MAX_PATH))
	{
		return false;
	}
	wstr_app_path = wsz_temp;
	std::wstring::size_type pos = 0;
	pos = wstr_app_path.find_last_of(L'\\');
	if (std::wstring::npos == pos)
	{
		return false;
	}
	wstr_app_path.erase(pos);
	wstr_app_path += L"\\";
	return true;
}

inline bool get_temp_dir(std::wstring& wstr_temp_path)
{
	wchar_t wsz_temp[_MAX_PATH] = {0};
	if (0 == ::GetTempPathW(_MAX_PATH, wsz_temp))
		return false;
	wstr_temp_path = wsz_temp;
	return true;
}


inline 
DWORD get_process_id_by_fullname(const wchar_t* wsz_target_full_path)
{
	if (!wsz_target_full_path)
	{
		return 0;
	}
	DWORD pid = 0;
	DWORD dw_processid[1024] = {0};
	DWORD dw_needed = 0;
	DWORD dw_process_counter = 0;
	if (!::EnumProcesses(dw_processid, sizeof(DWORD) * 1024, &dw_needed))
	{
		return 0;
	}
	dw_process_counter = dw_needed / sizeof(DWORD);
	for (std::size_t i = 0; i < dw_process_counter; ++i)
	{
		HANDLE h_handle = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dw_processid[i]);
		if (!h_handle)
			continue;
		HMODULE h_module = NULL;
		DWORD dw_needed  = 0;
		if (::EnumProcessModules(h_handle, &h_module, sizeof(h_module), &dw_needed)) //get first process's module
		{
			wchar_t wsz_full_path[MAX_PATH] = {0};
			::GetModuleFileNameEx(h_handle, h_module, wsz_full_path, MAX_PATH);
			stringxw wstr_full_path(wsz_target_full_path);
			if (0 == wstr_full_path.compare_no_case(wsz_full_path) && ::GetCurrentProcessId() != dw_processid[i])
			{
				::CloseHandle(h_handle);
				pid = dw_processid[i];
				break;
			}
			else
			{
				::CloseHandle(h_handle);
				continue;
			}
		}
		else
		{
			::CloseHandle(h_handle);
		}
	}
	return pid;
}

inline 
DWORD get_process_id_by_name(const wchar_t* process_name)
{
	if (!process_name)
	{
		return 0;
	}
	DWORD pid = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	PROCESSENTRY32W pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	Process32FirstW(hSnapshot, &pe);
	do
	{
		if(_wcsicmp(pe.szExeFile, process_name) == 0)
		{
			pid = pe.th32ProcessID;
			break;
		}
	}
	while(Process32NextW(hSnapshot, &pe) == TRUE);
	CloseHandle(hSnapshot);

	return pid;
}

inline 
DWORD get_process_id_by_name(const wchar_t* process_name, std::vector<DWORD>& vec_pid)
{
	if (!process_name)
	{
		return 0;
	}
	DWORD pid = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	PROCESSENTRY32W pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	Process32FirstW(hSnapshot, &pe);
	do
	{
		if(_wcsicmp(pe.szExeFile, process_name) == 0)
		{
			pid = pe.th32ProcessID;
			if (vec_pid.end() == std::find(vec_pid.begin(), vec_pid.end(), pid))
			{
				vec_pid.push_back(pid);
			}
		}
	}
	while(Process32NextW(hSnapshot, &pe) == TRUE);
	CloseHandle(hSnapshot);
	return pid;
}


static BOOL CALLBACK EnumWindowsProcSingle(HWND hwnd, LPARAM lParam)
{
	DWORD pid = *((DWORD*)lParam);
	DWORD new_pid;
	::GetWindowThreadProcessId(hwnd, &new_pid);
	if (new_pid == pid && NULL == ::GetParent(hwnd))
	{
		*((DWORD*)lParam) = (DWORD)hwnd;
		return FALSE;
	}
	return TRUE;
}

//get the first top-level and has none owner or parent window handle(except desktop is a parent window)
inline 
HWND get_winhandle_by_proid(DWORD pid)
{
	DWORD pid_inner = pid;
	::EnumWindows(EnumWindowsProcSingle, (LPARAM)&pid_inner);
	if (pid_inner != pid)
	{
		return (HWND)pid_inner;
	}
	else
		return NULL;
}

static BOOL CALLBACK EnumWindowsProcMultiple(HWND hwnd, LPARAM lParam)
{
	std::vector<HWND>* p_vec;
	p_vec = (std::vector<HWND>*)lParam;
	DWORD pid = (DWORD)p_vec->at(0);
	DWORD new_pid;
	::GetWindowThreadProcessId(hwnd, &new_pid);
	if (new_pid == pid)
	{
		p_vec->push_back(hwnd);
		return TRUE;
	}
	return TRUE;
}

inline 
int get_winhandle_by_proid(DWORD pid, std::vector<HWND>& vec_hwnd)
{
	vec_hwnd.push_back((HWND)pid);
	::EnumWindows(EnumWindowsProcMultiple, (LPARAM)&vec_hwnd);
	vec_hwnd.erase(vec_hwnd.begin());
	return 0;
}

inline
int create_lnk(const wchar_t* wsz_real_path, const wchar_t* wsz_lnk_path, int optional = 0/*0 == nothing 1 == startup run*/)
{
	if (!wsz_real_path || !wsz_lnk_path)
	{
		return -5;
	}
	if (-1 == dir_file_exist(wsz_real_path))
	{
		return -4;
	}
	if (1 == dir_file_exist(wsz_lnk_path))
	{
		return 0;
	}
	HRESULT hr = CoInitialize(NULL);
	if (SUCCEEDED(hr))
	{
		CComPtr<IShellLink> sp_shell;
		HRESULT hr = sp_shell.CoCreateInstance(CLSID_ShellLink);
		if (SUCCEEDED(hr))
		{
			CComPtr<IPersistFile> sp_file;
			hr = sp_shell->SetPath(wsz_real_path);
			if (optional == 1)
			{
				sp_shell->SetArguments(L"auto_run");
			}
			hr = sp_shell->QueryInterface(IID_IPersistFile, (void**)&sp_file);
			if (SUCCEEDED(hr))
			{
				sp_file->Save(wsz_lnk_path, FALSE);
				sp_file.Release();
			}
			else
			{
				sp_shell.Release();
				CoUninitialize();
				return -3;
			}
			sp_shell.Release();
		}
		else
		{
			CoUninitialize();
			return -2;
		}
		CoUninitialize();
		return 0;
	}
	else
		return -1;
}

inline
int create_startup(const wchar_t* module_path)
{
	if (!module_path)
	{
		return -3;
	}
	stringxw wstr_module = module_path;
	std::vector<stringxw> vec_temp;
	wstr_module.split_string(L"\\", vec_temp);
	if (vec_temp.empty())
	{
		wstr_module.split_string(L"/", vec_temp);
		if (vec_temp.empty())
		{
			return -1;
		}
	}
	stringxw wstr_file_name = vec_temp[vec_temp.size() - 1];
	wstr_file_name.erase(wstr_file_name.find_last_of(L"."));
	wchar_t wsz_temp[MAX_PATH] = {0};
	if (FAILED(SHGetFolderPathW(NULL, CSIDL_COMMON_STARTUP | CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, wsz_temp)))
		return -2;
	wstr_file_name = stringxw(wsz_temp) + L"\\" + wstr_file_name + L".lnk";
	return create_lnk(module_path, wstr_file_name.c_str());
}

inline
int get_ini_wstring(const wchar_t* wsz_inipath, const wchar_t* wsz_section, const wchar_t* wsz_key, std::wstring& wstr_value)
{
	if (!wsz_inipath || !wsz_section || !wsz_key)
	{
		return -2;
	}
	wchar_t wsz_temp[_MAX_PATH] = {0};
	::GetPrivateProfileStringW(wsz_section, wsz_key, NULL, wsz_temp, _MAX_PATH, wsz_inipath);
	wstr_value = wsz_temp;
	if (wstr_value.empty())
	{
		return -1;
	}
	return 0;
}

inline
bool is_vista_or_later()
{
	DWORD dwVersion = GetVersion();
	DWORD dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
	DWORD dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));

	if (dwVersion < 0x80000000)	// Windows NT/2000, Whistler
	{
		if(dwWindowsMajorVersion >= 6)	// Windows vista or later
		{
			return true;
		}
	}
	return false;
}


inline
BOOL is_admin()
{ 
	HANDLE hAccessToken; 
	BYTE *InfoBuffer; 
	PTOKEN_GROUPS ptgGroups; 
	DWORD dwInfoBufferSize; 
	PSID psidAdministrators; 
	SID_IDENTIFIER_AUTHORITY siaNtAuthority = SECURITY_NT_AUTHORITY; 
	UINT i; 
	BOOL bRet = FALSE; 

	if(!OpenProcessToken(GetCurrentProcess(),TOKEN_QUERY,&hAccessToken)) 
		goto cleanup; 

	InfoBuffer = new BYTE[1024]; 
	if(!InfoBuffer) 
		goto cleanup; 

	bRet = GetTokenInformation(hAccessToken, 
		TokenGroups, 
		InfoBuffer, 
		1024, 
		&dwInfoBufferSize); 

	CloseHandle(hAccessToken); 

	if(!bRet) 
		goto cleanup; 

	if( !AllocateAndInitializeSid(&siaNtAuthority, 
		2, 
		SECURITY_BUILTIN_DOMAIN_RID, 
		DOMAIN_ALIAS_RID_ADMINS, 
		0,0,0,0,0,0,&psidAdministrators) ) 
		goto cleanup; 

	bRet = FALSE; 

	ptgGroups = (PTOKEN_GROUPS)InfoBuffer; 

	for(i = 0; i < ptgGroups->GroupCount; i++) 
	{ 
		if(EqualSid(psidAdministrators,ptgGroups-> Groups[i].Sid)) 
		{ 
			bRet = TRUE; 
			break; 
		} 
	} 

	FreeSid(psidAdministrators); 

cleanup: 
	if(InfoBuffer) 
		delete[] InfoBuffer; 

	return bRet; 
}

inline 
int is_win2000()
{
	DWORD dwVersion = GetVersion();
	DWORD dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
	DWORD dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));

	if (dwVersion < 0x80000000)	// Windows NT/2000, Whistler
	{
		if(dwWindowsMajorVersion == 5 && dwWindowsMinorVersion == 0)
		{
			return true;
		}
	}
	return false;
}


//create psa that will be used for event mutex filemapping and so on, this attributes allows the kernel object be accessed by any user's process with any privilege.
//Especially for Global namespace kernel object
//remarks: filemapping kernel object with Global namespace is special in vista or later OS, that is most of time you will fail in CreateFileMapping(a global namespace) in a 
//low or medium privilege process with ERROR_ACCESS_DENIED (a high privilege process means a admin process or a system process(which runs in session 0))
inline
PSECURITY_ATTRIBUTES create_security_attribute(void)
{
	BOOL  bRes = FALSE;
	BOOL  bRet = FALSE;

	PSECURITY_ATTRIBUTES psa = NULL;
	PSECURITY_DESCRIPTOR psd = NULL;

	psa  = (PSECURITY_ATTRIBUTES)malloc(sizeof(SECURITY_ATTRIBUTES));
	if (!psa)   
		goto cleanup;

	// DACL for your application. 
	if (!is_win2000()) //2000 later
	{
		const wchar_t string_sd[] = 
			L"D:"                  // Discretionary ACL
			L"(D;OICI;GA;;;BG)"    // Deny access to Built-in Guests
			L"(D;OICI;GA;;;AN)"    // Deny access to Anonymous Logon
			L"(A;OICI;GA;;;AU)"    // Allow full control to Authenticated Users
			L"(A;OICI;GA;;;BA)";   // Allow full control to Administrators

		ULONG size;
		bRet = ::ConvertStringSecurityDescriptorToSecurityDescriptorW(
			string_sd,
			SDDL_REVISION_1,
			&psd,
			&size
			);
		if (!bRet)  
			goto cleanup;
	}
	else //2000
	{
		psd  = (PSECURITY_DESCRIPTOR)::LocalAlloc(LMEM_FIXED, SECURITY_DESCRIPTOR_MIN_LENGTH);
		if (!psd)   goto cleanup;

		bRet = InitializeSecurityDescriptor(psd, SECURITY_DESCRIPTOR_REVISION);
		if (!bRet)  goto cleanup;

		// Set DACL to Security Descriptor
		bRet = SetSecurityDescriptorDacl(
			psd, 
			TRUE, NULL,                 // Allowing all access to the object 
			FALSE
			);
		if (!bRet) 
			goto cleanup;
	}

	psa->nLength = sizeof(SECURITY_ATTRIBUTES);
	psa->bInheritHandle = TRUE;
	psa->lpSecurityDescriptor = psd;

	bRes = TRUE;

cleanup:
	if (!bRes)
	{
		if (psd) {::LocalFree(psd);  psd = NULL;}
		if (psa) {free(psa);  psa = NULL;}
	}

	return psa;
}

inline 
int free_security_attribute(PSECURITY_ATTRIBUTES psa)
{
	if (psa)
	{
		::LocalFree(psa->lpSecurityDescriptor);
		free(psa);
	}
	return TRUE;
}

inline
int get_document_path(std::wstring& wstr_path)
{
	int n_ret = 0;
	FUNCTION_BEGIN;
	WCHAR wsz_docment[MAX_PATH] = {0};
	n_ret = ::SHGetSpecialFolderPath(NULL, wsz_docment, CSIDL_PERSONAL, TRUE);
	if (!n_ret)
	{
		n_ret = -1;
		break;
	}
	else
		n_ret = 0;
	wstr_path = wsz_docment;
	if (wstr_path[wstr_path.size() - 1] != L'\\')
	{
		wstr_path += L"\\";
	}
	FUNCTION_END;
	return n_ret;
}

inline
int get_sysrecent_path(std::wstring& wstr_path)
{
	int n_ret = 0;
	FUNCTION_BEGIN;
	WCHAR wsz_docment[MAX_PATH] = {0};
	n_ret = ::SHGetSpecialFolderPath(NULL, wsz_docment, CSIDL_RECENT, TRUE);
	if (!n_ret)
	{
		n_ret = -1;
		break;
	}
	else
		n_ret = 0;
	wstr_path = wsz_docment;
	if (wstr_path[wstr_path.size() - 1] != L'\\')
	{
		wstr_path += L"\\";
	}
	FUNCTION_END;
	return n_ret;
}

inline
int write_register(HKEY main_key, const wchar_t* wsz_subkey, const wchar_t* wsz_value_name, DWORD dw_value)
{
	int n_ret = 0;
	HKEY hkey = 0;
	FUNCTION_BEGIN;
	if (!wsz_subkey || !wsz_value_name)
	{
		n_ret = -3;
		break;
	}
	DWORD dwRet = ::RegOpenKeyExW(main_key, wsz_subkey, 0, KEY_ALL_ACCESS, &hkey);
	if (ERROR_SUCCESS != dwRet)
	{
		dwRet = ::RegCreateKeyExW(main_key, wsz_subkey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,	&hkey, NULL);
		if (ERROR_SUCCESS != dwRet)
		{
			n_ret = -1;
			break;
		}
	}
	dwRet = ::RegSetValueExW(hkey, wsz_value_name, 0, REG_DWORD, (const BYTE*)&dw_value, sizeof(DWORD));
	if (ERROR_SUCCESS != dwRet)
	{
		n_ret = -2;
		break;
	}
	FUNCTION_END;
	if (hkey)
	{
		::RegCloseKey(hkey);
	}
	return n_ret;
}


inline
int write_register(HKEY main_key, const wchar_t* wsz_subkey, const wchar_t* wsz_value_name, const wchar_t* wsz_value)
{
	int n_ret = 0;
	HKEY hkey = 0;
	FUNCTION_BEGIN;
	if (!wsz_subkey || !wsz_value_name || !wsz_value)
	{
		n_ret = -3;
		break;
	}
	DWORD dwRet = ::RegOpenKeyExW(main_key, wsz_subkey, 0, KEY_ALL_ACCESS, &hkey);
	if (ERROR_SUCCESS != dwRet)
	{
		dwRet = ::RegCreateKeyExW(main_key, wsz_subkey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,	&hkey, NULL);
		if (ERROR_SUCCESS != dwRet)
		{
			n_ret = -1;
			break;
		}
	}
	dwRet = ::RegSetValueExW(hkey, wsz_value_name, 0, REG_SZ, (const BYTE*)wsz_value, (::wcslen(wsz_value) + 1) * sizeof(wchar_t));
	if (ERROR_SUCCESS != dwRet)
	{
		n_ret = -2;
		break;
	}
	FUNCTION_END;
	if (hkey)
	{
		::RegCloseKey(hkey);
	}
	return n_ret;
}


inline
int get_register_value(HKEY main_key, const wchar_t* wsz_subkey, const wchar_t* wsz_value_name, DWORD& dw_value)
{
	int n_ret = 0;
	HKEY hkey = 0;
	FUNCTION_BEGIN;
	if (!wsz_subkey || !wsz_value_name)
	{
		n_ret = -3;
		break;
	}
	DWORD dwRet = ::RegOpenKeyExW(main_key, wsz_subkey, 0, KEY_READ, &hkey);
	if (ERROR_SUCCESS != dwRet)
	{
		n_ret = -1;
		break;
	}
	DWORD dw_len = 512;
	BYTE byte_temp[512] = {0};
	dwRet = ::RegQueryValueExW(hkey, wsz_value_name, 0, 0, byte_temp, &dw_len);
	if (ERROR_SUCCESS != dwRet)
	{
		n_ret = -2;
		break;
	}
	std::size_t n_lenth = ::strlen((const char*)byte_temp);
	if (sizeof(DWORD) == dw_len && (0 <= n_lenth && 4 >= n_lenth))
	{
		dw_value = *((DWORD*)byte_temp);
	}
	FUNCTION_END;
	if (hkey)
	{
		::RegCloseKey(hkey);
	}
	return n_ret;
}


inline
int get_register_value(HKEY main_key, const wchar_t* wsz_subkey, const wchar_t* wsz_value_name, std::wstring& wstr_value)
{
	int n_ret = 0;
	HKEY hkey = 0;
	FUNCTION_BEGIN;
	if (!wsz_subkey || !wsz_value_name)
	{
		n_ret = -3;
		break;
	}
	DWORD dwRet = ::RegOpenKeyExW(main_key, wsz_subkey, 0, KEY_READ, &hkey);
	if (ERROR_SUCCESS != dwRet)
	{
		n_ret = -1;
		break;
	}
	DWORD dw_len = 1024;
	BYTE byte_temp[1024] = {0};
	dwRet = ::RegQueryValueExW(hkey, wsz_value_name, 0, 0, byte_temp, &dw_len);
	if (ERROR_SUCCESS != dwRet)
	{
		n_ret = -2;
		break;
	}
	wstr_value.assign((const wchar_t*)byte_temp);
	FUNCTION_END;
	if (hkey)
	{
		::RegCloseKey(hkey);
	}
	return n_ret;
}


inline
int isx64_program(DWORD dw_pid, BOOL& b_x64program)
{
	int n_ret = 0;
	HANDLE hProcess = 0;
	b_x64program = FALSE;
	FUNCTION_BEGIN;
	hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dw_pid);
	if (!hProcess)	
	{
		n_ret = -1;
		break;
	}
	typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);

	LPFN_ISWOW64PROCESS fn_iswow64process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandleW(L"kernel32"), "IsWow64Process");
	if (!fn_iswow64process)	
	{
		n_ret = -2;
		break;
	}

	if (!fn_iswow64process(hProcess, &b_x64program))
	{
		n_ret = -3;
		break;
	}

	FUNCTION_END;
	if (hProcess)
	{
		::CloseHandle(hProcess);
	}
	return n_ret;
} //to be tested

inline 
int isx64_os()
{
	int n_ret = 0;
	FUNCTION_BEGIN;
	typedef UINT(WINAPI * fnGetSystemWow64DirectoryW)(LPWSTR lpBuffer, UINT uSize);
	fnGetSystemWow64DirectoryW pAddr = (fnGetSystemWow64DirectoryW)GetProcAddress(GetModuleHandleW(L"kernel32"), "GetSystemWow64DirectoryW");
	if (!pAddr) 
	{
		n_ret = -1;
		break;
	}
	wchar_t buf[MAX_PATH + 1] = {0};
	if (!pAddr(buf, MAX_PATH))
	{
		if (GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)
		{
			n_ret = -2;
			break;
		}
	}
	FUNCTION_END;
	return n_ret;
}

inline 
int get_ie_tmp_dir(std::wstring& wstr_id_tmp_dir)
{
	int n_ret = 0;
	wstr_id_tmp_dir = L"";
	FUNCTION_BEGIN;
	wchar_t wsz_temp[_MAX_PATH] = {0};
	BOOL b_ret = SHGetSpecialFolderPathW(NULL, wsz_temp, CSIDL_INTERNET_CACHE, FALSE);
	if (! b_ret)
	{
		n_ret = -1;
		FUNCTION_LEAVE;
	}
	wstr_id_tmp_dir = wsz_temp;
	FUNCTION_END;
	return n_ret;
}

/* IsTextUTF8
*
* UTF-8 is the encoding of Unicode based on Internet Society RFC2279
*
* Basicly:
* 0000 0000-0000 007F - 0xxxxxxx  (ascii converts to 1 octet!)
* 0000 0080-0000 07FF - 110xxxxx 10xxxxxx    ( 2 octet format)
* 0000 0800-0000 FFFF - 1110xxxx 10xxxxxx 10xxxxxx (3 octet format)
* (this keeps going for 32 bit unicode) 
* 
*
* Return value:  TRUE, if the text is in UTF-8 format.
*                FALSE, if the text is not in UTF-8 format.
*                We will also return FALSE is it is only 7-bit ascii, so the right code page
*                will be used.
*
*                Actually for 7 bit ascii, it doesn't matter which code page we use, but
*                notepad will remember that it is utf-8 and "save" or "save as" will store
*                the file with a UTF-8 BOM.  Not cool.
*/
inline
BOOL IsTextUTF8( LPSTR lpstrInputStream, INT iLen )
{
	INT   i;
	DWORD cOctets;  // octets to go in this UTF-8 encoded character
	UCHAR chr;
	BOOL  bAllAscii= TRUE;
	cOctets= 0;
	for( i=0; i < iLen; i++ )
	{
		chr= *(lpstrInputStream+i);

		if( (chr&0x80) != 0 ) bAllAscii= FALSE;

		if( cOctets == 0 )  
		{
			//
			// 7 bit ascii after 7 bit ascii is just fine.  Handle start of encoding case.
			//
			if( chr >= 0x80 ) 
			{  
				//
				// count of the leading 1 bits is the number of characters encoded
				//
				do 
				{
					chr <<= 1;
					cOctets++;
				}
				while( (chr&0x80) != 0 );

				cOctets--;                        // count includes this character
				if( cOctets == 0 ) return FALSE;  // must start with 11xxxxxx
			}
		}
		else {
			// non-leading bytes must start as 10xxxxxx
			if( (chr&0xC0) != 0x80 ) 
			{
				return FALSE;
			}
			cOctets--;                           // processed another octet in encoding
		}
	}

	//
	// End of text.  Check for consistency.
	//

	if( cOctets > 0 ) {   // anything left over at the end is an error
		return FALSE;
	}

	if( bAllAscii ) {     // Not utf-8 if all ascii.  Forces caller to use code pages for conversion
		return FALSE;
	}

	return TRUE;
}

inline 
bool SystemPoweroff(bool b_poweroff = true)
{
	HANDLE hToken; 
	TOKEN_PRIVILEGES tkp; 

	if (!::OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		return false; 

	::LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid); 

	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 

	::AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

	if (::GetLastError() != ERROR_SUCCESS)
		return false; 
	if (b_poweroff)
	{
		if (!::ExitWindowsEx(EWX_POWEROFF, SHTDN_REASON_MAJOR_OPERATINGSYSTEM | SHTDN_REASON_FLAG_PLANNED))
			return false; 
	}
	else
	{
		if (!::ExitWindowsEx(EWX_REBOOT, SHTDN_REASON_MAJOR_OPERATINGSYSTEM | SHTDN_REASON_FLAG_PLANNED))
			return false; 
	}
	return true;
}


inline
void WINAPI SetRegPermission(LPCTSTR KeyStr)
{
	HKEY hKey = 0;
	SID_IDENTIFIER_AUTHORITY sia = SECURITY_NT_AUTHORITY;
	PSID pAdministratorsSid = NULL;
	SECURITY_DESCRIPTOR sd;
	PACL pDacl = NULL;
	DWORD dwAclSize;
	LONG lRetCode;
	BOOL bRet;

	lRetCode = RegOpenKeyEx(HKEY_LOCAL_MACHINE, KeyStr,	0, WRITE_DAC, &hKey);
	//open key
	// prepare a Sid representing any Interactively logged-on user
	//
	// 	bRet = AllocateAndInitializeSid(
	// 		&sia,
	// 		1,
	// 		SECURITY_INTERACTIVE_RID,
	// 		0, 0, 0, 0, 0, 0, 0,
	// 		&pInteractiveSid
	// 		);

	//
	// preprate a Sid representing the well-known admin group
	//
	bRet = AllocateAndInitializeSid(
		&sia,
		2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0,
		&pAdministratorsSid
		);

	//
	// compute size of new acl
	//
	dwAclSize = sizeof(ACL) + sizeof(ACCESS_ALLOWED_ACE) + GetLengthSid(pAdministratorsSid);

	//
	// allocate storage for Acl
	//
	pDacl = (PACL)HeapAlloc(GetProcessHeap(), 0, dwAclSize);

	bRet = InitializeAcl(pDacl, dwAclSize, ACL_REVISION);

	//
	// grant the Interactive Sid KEY_READ access to the perf key
	//
	// 	bRet = AddAccessAllowedAce(
	// 		pDacl,
	// 		ACL_REVISION,
	// 		KEY_ALL_ACCESS,
	// 		pInteractiveSid
	// 		);

	//
	// grant the Administrators Sid KEY_ALL_ACCESS access to the perf key
	//
	bRet = AddAccessAllowedAce(
		pDacl,
		ACL_REVISION,
		KEY_ALL_ACCESS,
		pAdministratorsSid
		);

	bRet = InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);

	bRet = SetSecurityDescriptorDacl(&sd, TRUE, pDacl, FALSE);

	//
	// apply the security descriptor to the registry key
	//
	lRetCode = RegSetKeySecurity(
		hKey,
		(SECURITY_INFORMATION)DACL_SECURITY_INFORMATION,
		&sd
		);

	//clean up
	RegCloseKey(hKey);
	RegCloseKey(HKEY_LOCAL_MACHINE);

	//
	// free allocated resources
	//
	if(pDacl != NULL)
		HeapFree(GetProcessHeap(), 0, pDacl);

	if(pAdministratorsSid != NULL)
		FreeSid(pAdministratorsSid);
} 

#ifndef SEE_MASK_FORCENOIDLIST
#define SEE_MASK_FORCENOIDLIST 0x1000
#endif

inline
void WipeShellExecuteEx(LPCTSTR lpFile, LPCTSTR pCommandLine, int nShow,  DWORD dwWaitInputIdle = 0, HANDLE* lphProcess = NULL, bool b_need_admin = false)
{
    SHELLEXECUTEINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cbSize = sizeof(si);
    si.lpFile = lpFile;
    si.lpParameters = pCommandLine;
    si.nShow = nShow;
    si.fMask = (SEE_MASK_FLAG_NO_UI | SEE_MASK_FORCENOIDLIST);
    si.lpVerb = L"open";
    if (is_vista_or_later() && b_need_admin)
    {
        si.lpVerb = L"runas";
    }

    if (dwWaitInputIdle || lphProcess)
        si.fMask |= SEE_MASK_NOCLOSEPROCESS;

    ::ShellExecuteEx(&si);

    if (dwWaitInputIdle)
        WaitForInputIdle(si.hProcess, dwWaitInputIdle);

    if(lphProcess)
        *lphProcess = si.hProcess;
    else
        CloseHandle(si.hProcess);
}


class none_copy_base
{
protected:
	none_copy_base()
	{
		;
	}
	~none_copy_base()
	{
		;
	}
private:
	none_copy_base(const none_copy_base&)
	{
		;
	}
	const none_copy_base& operator = (const none_copy_base&)
	{
		;
	}
};

class stl_fstream_locale : public none_copy_base
{
public:
	stl_fstream_locale()
	{
		m_loc_lang = std::locale::global(std::locale(""));
	}
	~stl_fstream_locale()
	{
		std::locale::global(m_loc_lang);
	}
private:
	std::locale m_loc_lang;
};

//when your app runs under system session while you want to gain current user's profile dir,use this class
class different_user_switch : public none_copy_base
{
public:
	different_user_switch() : m_h_token(NULL)
	{
		if (!_get_token(m_h_token) && m_h_token && ::ImpersonateLoggedOnUser(m_h_token))
		{
			;
		}
	}
	~different_user_switch()
	{
		if (m_h_token)
		{
			::RevertToSelf();
			::CloseHandle(m_h_token);
			m_h_token = NULL;
		}
	}
private:
	int _get_token(HANDLE &h_token)
	{
		int n_ret = 0;
		HANDLE h_process = NULL;
		FUNCTION_BEGIN;
		PROCESSENTRY32W pe32 = {0};
		h_process = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (h_process == INVALID_HANDLE_VALUE)
		{
			n_ret = -1;
			break;
		}
		pe32.dwSize = sizeof(PROCESSENTRY32);
		if (::Process32First(h_process, &pe32))
		{
			do
			{
				if(!::_wcsicmp(pe32.szExeFile, L"explorer.exe"))
				{
					HANDLE h_process_inner = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pe32.th32ProcessID);
					if (!h_process_inner)
					{
						n_ret = -3;
						break;
					}
					if (!::OpenProcessToken(h_process_inner, TOKEN_ALL_ACCESS, &h_token))
					{
						n_ret = -4;
						::CloseHandle(h_process_inner);
						break;
					}
					::CloseHandle(h_process_inner);
					break;
				}
			}
			while (::Process32Next(h_process, &pe32));
		}
		else
		{
			n_ret = -2;
			break;
		}
		FUNCTION_END;
		if (h_process)
		{
			::CloseHandle(h_process);
		}
		return n_ret;
	}

	HANDLE m_h_token;
};

//------------------------------------------------------------------------
#endif
