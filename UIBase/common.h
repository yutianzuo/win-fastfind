#pragma once

// GetWindowFamily �Ĳ��� flag �Ķ���
#define Windows_MSStr		0x00000001
#define Windows_CDVersion	0x00000002
#define Windows_BitInfo		0x00000004
#define Windows_BuildNo		0x00000008
#define Windows_ForDownload 0x00000010

// GetWindowFamily ��flagΪWindows_ForDownloadʱ�ķ���ֵ
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

// //Windows�汾���
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
	STDMETHOD_(BOOL, GetPFDirectory)(ISomString** szPath) PURE;				// ��� Program Files ��·��
	STDMETHOD (GetArgumentsFilePath)(LPCWSTR szString, ISomString** szOutString, BOOL bArg) PURE;
};

class SOM_NO_VTABLE ISomRegistry : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_SOMREGISTRY };

	// b64Key: �ò���ֻ��64λϵͳ����������  
	// TRUE ��ʾ�Ƿ�ö�ٵ���64λϵͳ�����ע����� �����ʾö��32λϵͳ��Ӧ��ע�����
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

	// pdwMS, pdwLS, pszVersion �����Ը��գ�˵����������������
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

	// �����ݿ��ļ� ��������ݿ��ļ������� �򴴽�
	// pszPath: ���ݿ��ļ���·��
	// pHandle: �������ݿ���
	STDMETHOD(OpenDB)(char* pszPath, SQLITE_DB_HANDLE* hDB) PURE;
	// �ر����ݿ�
	// pHandle: OpenDB ���ص����ݿ���
	STDMETHOD(CloseDB)(SQLITE_DB_HANDLE hDB) PURE;

	// �첽ִ��SQL���
	// pszSQL : SQL ���
	// pfCallBack : �ص����� sql���ִ��������øú���
	// pParam: ���ø��ص������ĵ�һ��������ֵ
	// ppMsg: ������Ϣ  ����ΪNULL �������NULL ���Һ���ʧ�� �����˴�����Ϣ  Ӧ����FreeBuf�ͷŵ���ֵ ��������ڴ�й¶
	STDMETHOD(ExecSQL)(SQLITE_DB_HANDLE hDB, char* pszSQL, SQLITE_CALL_BACK_FUNC pfCallBack, void* pParam, char** ppMsg) PURE;

	// ͬ����ѯ ��ѯ���ݿ���Ϣ 
	// pszSQL: sql���
	// pppRet: ���صĽ���ַ������� ��������2�� 2�� ���ʽΪ  ����A ����B ֵA1 ֵB1 ֵA2 ֵB2  
	// pnRow: ���صļ�¼��
	// pnColumn: ���ؽ��������
	// ppMsg: ������Ϣ  ����ΪNULL �������NULL ���Һ���ʧ�� �����˴�����Ϣ  Ӧ����FreeBuf�ͷŵ���ֵ ��������ڴ�й¶ 
	STDMETHOD(QueryDBInfo)(SQLITE_DB_HANDLE hDB, char* pszSQL, char*** pppRet, int *pnRow, int *pnColumn, char** ppMsg) PURE;
	// �ͷŲ�ѯ���ݿ���Ϣ���صĽ��
	// ppResult: ִ��QueryDBInfo ���صĽ����Ϣ
	STDMETHOD(FreeDBInfo)(char** ppResult) PURE;

	// �ͷ��ɸ����ݿ���������ڴ�, ���� �����������ʧ�� �������Ϣ �������ݿ��������
	// Ӧ�õ��øú����ͷŵ� ��������ڴ�й¶
	STDMETHOD(FreeBuf)(void* pBuf) PURE;

	// ��д�����ƴ����� 
	// pSQL: SQL��� �����ǲ��뻹�Ǹ��� ����BLOB���ݵĵط���?����
	// dwBLOBIndex: ��������SQL����ж���ʺţ� ���ֵ��ʾ�ǵ�һ��? ������1��ʼ
	// pBuf: ����������
	// dwLen: ���������ݳ���
	STDMETHOD(WriteBLOB)(SQLITE_DB_HANDLE hDB, char* pSQL, DWORD dwBLOBIndex, BYTE* pBuf, DWORD dwLen) PURE;

	// ��ѯ����������
	// pSQL: ���ڲ�ѯ��SQL��� Ŀǰ��֧�ֲ�ѯ�����1������� 
	// dwBBLOBCol: Ҫ���ص�BLOB���ݵ������� ��0��ʼ
	// pBuf:����������
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

	// �Ƿ�������64λ����ϵͳ��
	STDMETHOD_(BOOL, IsRunOn64OS)() PURE;

	// �ص��ļ��ض��� ���÷�Χ�ǵ�ǰ�߳� 
	// ppOldVal: ���ص�ǰ���ض���ֵ �ڻָ�ʱʹ��
	STDMETHOD_(BOOL, DisableWow64FsRedir)(PVOID* ppOldVal) PURE;
	// �ָ��ļ��ض��� 
	// pOldVal: ��DisableWow64FsRedir�������ص�ֵ
	STDMETHOD_(BOOL, RevertWow64FsRedir)(PVOID pOldVal) PURE;
};

// ��֤360ǩ�� ���ļ�ǩ��
// 2010-05-7 ���ǩ��У��Ĺ���
// 2010-05-7 �ϳ�����ͨ��
class SOM_NO_VTABLE ISignLib : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_SIGN_LIB };

	// ��֤360ǩ��
	STDMETHOD_(BOOL, Check360Sign)(LPCWSTR pszPath) PURE;

	// ��֤�ļ�ǩ�� cpKey: ��Կ
	STDMETHOD_(BOOL, CheckFileSign)(LPCWSTR pszPath, BYTE* cpKey) PURE;
};
