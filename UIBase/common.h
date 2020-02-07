#pragma once

// GetWindowFamily 的参数 flag 的定义
#define Windows_MSStr		0x00000001
#define Windows_CDVersion	0x00000002
#define Windows_BitInfo		0x00000004
#define Windows_BuildNo		0x00000008
#define Windows_ForDownload 0x00000010

// GetWindowFamily 当flag为Windows_ForDownload时的返回值
#define	WINDOWS_UNKNOWN				0
#define	WINDOWS_32S					1
#define WINDOWS_NT3					2
#define	WINDOWS_95					3
#define	WINDOWS_98					4
#define WINDOWS_98SE				5
#define	WINDOWS_ME					6
#define WINDOWS_NT4					7
#define	WINDOWS_2000				8
#define	WINDOWS_XP					9
#define WINDOWS_2003				10
#define WINDOWS_VISTA				11
#define WINDOWS_WIN7				12

// //Windows版本相关
class SOM_NO_VTABLE IWinVersion : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_WINVERSION };

	STDMETHOD_(BOOL, IsWindows2K)() PURE;
	STDMETHOD_(BOOL, IsWindowsXp)() PURE;
	STDMETHOD_(BOOL, IsWindowsVistaLater)() PURE;
	STDMETHOD_(BOOL, GetWindowFamily)(DWORD flag, ISomString** strSystem) PURE;
};

class SOM_NO_VTABLE ISomHash : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_SOMHASH };

	STDMETHOD_(BOOL, GetFileMD5)(LPCWSTR lpFile, ISomString** szOutputMd5) PURE;
	STDMETHOD_(BOOL, GetBufferMD5String)(LPBYTE lpBuffer, DWORD dwSize, ISomString** szOutputMd5) PURE;
	STDMETHOD_(BOOL, GetBufferMD5)(LPBYTE lpBuffer, DWORD dwSize, ISomBinaryBuffer** ppOutputMd5) PURE;
};

class SOM_NO_VTABLE ISomKernel : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_SOMKERNEL };

};

class SOM_NO_VTABLE IPathParser : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_PATHPARSER };

	STDMETHOD_(BOOL, ExpandEnvironmentStrings)(LPCWSTR strString, ISomString** strValue) PURE;
	STDMETHOD_(BOOL, PathCanonicalize)(LPCWSTR lpPath, ISomString** appPath) PURE;
	STDMETHOD_(BOOL, SplitPath)(LPCWSTR lpPath, ISomString** appPath, ISomString** appName) PURE;
	STDMETHOD_(BOOL, CommandLineCanonicalize)(LPCWSTR lpCommandLine, ISomString** appPath, ISomString** appParams) PURE;
	STDMETHOD_(BOOL, GetPFDirectory)(ISomString** szPath) PURE;				// 获得 Program Files 的路径
	STDMETHOD (GetArgumentsFilePath)(LPCWSTR szString, ISomString** szOutString, BOOL bArg) PURE;
};

class SOM_NO_VTABLE ISomRegistry : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_SOMREGISTRY };

	// b64Key: 该参数只在64位系统下有有意义  
	// TRUE 表示是否枚举的是64位系统对象的注册表项 否则表示枚举32位系统对应的注册表项
	STDMETHOD_(BOOL, RegEnum)(LPCWSTR strName, BOOL b64Key, ISomStringList** valueVec) PURE;
	STDMETHOD_(BOOL, RegRead)(LPCWSTR strName, BOOL b64Key, ISomString** strValue) PURE;
	STDMETHOD_(BOOL, RegReadDWORD)(LPCWSTR strName, BOOL b64Key, DWORD* pdwValue) PURE;
	STDMETHOD_(BOOL, RegWrite)(LPCWSTR strName, BOOL b64Key, LPCWSTR strValue, BOOL bExpand) PURE;
	STDMETHOD_(BOOL, RegWriteDWORD)(LPCWSTR strName, BOOL b64Key, DWORD dwValue) PURE;
	STDMETHOD_(BOOL, RegDeleteValue)(LPCWSTR strName, BOOL b64Key) PURE;
	STDMETHOD_(BOOL, RegDeleteKey)(LPCWSTR strName, BOOL b64Key) PURE;
};

class SOM_NO_VTABLE ISomFile : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_SOMFILE };

	typedef enum TargetEnum
	{
		ShortcutTarget_sucess = 0,
		ShortcutTarget_msi = 1,
		ShortcutTarget_fail = 2,
	};

	// pdwMS, pdwLS, pszVersion 都可以给空，说明不返回这项数据
	STDMETHOD_ (BOOL, GetFileVersion)(LPCWSTR strFile, DWORD* pdwMS, DWORD* pdwLS, ISomString** pszVersion, BOOL bFileVerOrProductVer) PURE;

	STDMETHOD_ (BOOL, GetFileInfo)(LPCWSTR strFile, ISomString** pszDesctiption, ISomString** pszCompany) PURE;

	STDMETHOD_ (BOOL,SHGetShortcutTarget)(PCWSTR pszFileName,ISomString **pszShortcutTarget) PURE;
	STDMETHOD_ (int,PowerGetShortTarget)(PCWSTR pszShortcut, ISomString** pszTargert,ISomString** pszProductCode,ISomString** pszFeatureId,ISomString** pszComponentCode,BOOL bOnlyMsi) PURE;


};


class SOM_NO_VTABLE IUrlParser : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_URLPARSER };

	STDMETHOD_(BOOL, FormURLEncode)(LPCWSTR strInput, ISomBinaryBuffer** strOut) PURE;
	STDMETHOD_(BOOL, FormURLEncodeA)(LPCSTR strInput, ISomBinaryBuffer** strOut) PURE;
	STDMETHOD_(BOOL, FormURLEncodeData)(PBYTE lpInput, DWORD dwLen, ISomBinaryBuffer** strOut) PURE;
};

class SOM_NO_VTABLE IZlib : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_ZLIB };
	STDMETHOD (Compress)(PBYTE dest, PDWORD destLen, PBYTE source, DWORD sourceLen, int level) PURE;
	STDMETHOD (Uncompress)(PBYTE dest, PDWORD destLen, PBYTE source, DWORD sourceLen) PURE;

	STDMETHOD (ZipFile)(LPCWSTR pszwSrc, LPCWSTR pszwZip)PURE;
	STDMETHOD (UnzipFileToDir)(LPCWSTR pszwZip, LPCWSTR pszwOutDir)PURE;

};

struct sqlite3;
typedef sqlite3* SQLITE_DB_HANDLE;
typedef int (*SQLITE_CALL_BACK_FUNC)(void*, int, char**, char**);
class SOM_NO_VTABLE ISqlite : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_SQLITE };

	// 打开数据库文件 如果该数据库文件不存在 则创建
	// pszPath: 数据库文件的路径
	// pHandle: 返回数据库句柄
	STDMETHOD(OpenDB)(char* pszPath, SQLITE_DB_HANDLE* hDB) PURE;
	// 关闭数据库
	// pHandle: OpenDB 返回的数据库句柄
	STDMETHOD(CloseDB)(SQLITE_DB_HANDLE hDB) PURE;

	// 异步执行SQL语句
	// pszSQL : SQL 语句
	// pfCallBack : 回调函数 sql语句执行完后会调用该函数
	// pParam: 设置给回调函数的第一个参数的值
	// ppMsg: 错误信息  可以为NULL 如果不是NULL 并且函数失败 返回了错误信息  应调用FreeBuf释放掉该值 否则会有内存泄露
	STDMETHOD(ExecSQL)(SQLITE_DB_HANDLE hDB, char* pszSQL, SQLITE_CALL_BACK_FUNC pfCallBack, void* pParam, char** ppMsg) PURE;

	// 同步查询 查询数据库信息 
	// pszSQL: sql语句
	// pppRet: 返回的结果字符串数组 假如结果有2行 2列 其格式为  列名A 列名B 值A1 值B1 值A2 值B2  
	// pnRow: 返回的记录数
	// pnColumn: 返回结果的列数
	// ppMsg: 错误信息  可以为NULL 如果不是NULL 并且函数失败 返回了错误信息  应调用FreeBuf释放掉该值 否则会有内存泄露 
	STDMETHOD(QueryDBInfo)(SQLITE_DB_HANDLE hDB, char* pszSQL, char*** pppRet, int *pnRow, int *pnColumn, char** ppMsg) PURE;
	// 释放查询数据库信息返回的结果
	// ppResult: 执行QueryDBInfo 返回的结果信息
	STDMETHOD(FreeDBInfo)(char** ppResult) PURE;

	// 释放由该数据库对象分配的内存, 例如 如果函数返回失败 则错误信息 是由数据库对象分配的
	// 应该调用该函数释放掉 否则会有内存泄露
	STDMETHOD(FreeBuf)(void* pBuf) PURE;

	// 读写二进制大数据 
	// pSQL: SQL语句 不管是插入还是更行 该是BLOB数据的地方用?代替
	// dwBLOBIndex: 如果插入的SQL语句有多个问号， 则该值表示是第一个? 索引从1开始
	// pBuf: 二进制数据
	// dwLen: 二进制数据长度
	STDMETHOD(WriteBLOB)(SQLITE_DB_HANDLE hDB, char* pSQL, DWORD dwBLOBIndex, BYTE* pBuf, DWORD dwLen) PURE;

	// 查询二进制数据
	// pSQL: 用于查询的SQL语句 目前是支持查询结果是1条的情况 
	// dwBBLOBCol: 要返回的BLOB数据的列索引 从0开始
	// pBuf:二进制数据
	STDMETHOD(ReadBLOB)(SQLITE_DB_HANDLE hDB, char* pSQL, DWORD dwBBLOBCol, ISomBinaryBuffer* pBuf) PURE;
};

class SOM_NO_VTABLE ILibPng : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_LIBPNG };

	STDMETHOD_(HBITMAP, Decode)(PBYTE pngdata, DWORD pnglen) PURE;
};

class SOM_NO_VTABLE IJpeg : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_JPEG };

	STDMETHOD_(HBITMAP, Decode)(PBYTE jpegdata, DWORD jpeglen) PURE;
};

enum URLMONIKER_STATUS
{
	URLMONIKER_IDLE,
	URLMONIKER_DOWNLOADING,
	URLMONIKER_DOWNLOADERROR,
	URLMONIKER_DOWNLOADCOMPLETE
};

class SOM_NO_VTABLE IUrlMoniker : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_URLMONIKER };

	STDMETHOD (Download)(LPCWSTR url, PBYTE pPostData, DWORD dwPostLen, ISomAsyncResult* pAsyncResult) PURE;
	STDMETHOD (StopDownload)() PURE;
	STDMETHOD_(URLMONIKER_STATUS, DownloadStatus)() PURE;
	STDMETHOD_(DWORD, GetDataLength)() PURE;
	STDMETHOD_(PBYTE, GetData)() PURE;
};

class SOM_NO_VTABLE IWOW64 : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_WOW64 };

	// 是否运行在64位操作系统上
	STDMETHOD_(BOOL, IsRunOn64OS)() PURE;

	// 关掉文件重定向 作用范围是当前线程 
	// ppOldVal: 返回当前的重定向值 在恢复时使用
	STDMETHOD_(BOOL, DisableWow64FsRedir)(PVOID* ppOldVal) PURE;
	// 恢复文件重定向 
	// pOldVal: 调DisableWow64FsRedir函数返回的值
	STDMETHOD_(BOOL, RevertWow64FsRedir)(PVOID pOldVal) PURE;
};

// 验证360签名 和文件签名
// 2010-05-7 添加签名校验的功能
// 2010-05-7 邢超评审通过
class SOM_NO_VTABLE ISignLib : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_SIGN_LIB };

	// 验证360签名
	STDMETHOD_(BOOL, Check360Sign)(LPCWSTR pszPath) PURE;

	// 验证文件签名 cpKey: 公钥
	STDMETHOD_(BOOL, CheckFileSign)(LPCWSTR pszPath, BYTE* cpKey) PURE;
};
