#pragma once

#define CODE_PAGE_IDENTIFIERS_GBK	936

// ����ܼ�ȫ����Ϣ����������Ͱ�ȫ��ʿ��װĿ¼������ mid
class SOM_NO_VTABLE ISomAppInfo : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_SOMAPPINFO };

	STDMETHOD (Get360Path)(ISomString** szPath) PURE;			// ����App Paths ��ð�ȫ��ʿ�İ�װ·��
	STDMETHOD (GetSomPath)(ISomString** szPath) PURE;			// �������ܼҵİ�װ·��
	STDMETHOD (GetComputerID)(ISomString** szMID) PURE;			// ��ü������Ӳ��Ψһ ID
	STDMETHOD (GetUIDString)(ISomString** szUIDString) PURE;	// ��� UID �ַ���
	STDMETHOD_(DWORD, GetUID)() PURE;							// ��� UID
	STDMETHOD (GetOEMURL)(ISomString** szOEMURL) PURE;			// ��� OEM URL
	STDMETHOD (GetOEMTitle)(ISomString** szOEMTitle) PURE;		// ��� OEM TITLE
	STDMETHOD (GetPID)(ISomString** szPID) PURE;				// ��ü������OEMID

	STDMETHOD_(BOOL, GetDebugPrivilege)() PURE;					// �򿪵���Ȩ��
	STDMETHOD_(BOOL, HaveInternetConnection)() PURE;			// �ж��Ƿ�����������ͨ��
};

// http ���� (���� get, post)
class SOM_NO_VTABLE IHttpDownload : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_HTTPDOWNLOAD };

	// pBuf�е��ַ�����ansi��ʽ�� ���pEventΪNULL �򲻶�ȡ������Ϣ
	STDMETHOD_(BOOL, DownloadFileByPost)(LPCWSTR lpURL, void* pBuf, DWORD dwSize, LPCWSTR lpFileName, ISomAsyncResult* pEvent) PURE;
	STDMETHOD_(BOOL, DownloadFileByGet)(LPCWSTR lpURL, LPCWSTR lpFileName, ISomAsyncResult* pEvent) PURE;

	// pBuf�е��ַ�����ansi��ʽ�� ����ɹ� 
	// �Ὣ���������ISomAsyncResult�ṹ��ISomBinaryBuffer��
	// ���pEventΪNULL �򲻶�ȡ������Ϣ
	STDMETHOD_(BOOL, DownloadBufByPost)(LPCWSTR lpURL, void* pBuf, DWORD dwSize, ISomAsyncResult* pEvent) PURE;
	STDMETHOD_(BOOL, DownloadBufByGet)(LPCWSTR lpURL, ISomAsyncResult* pEvent) PURE;
};

// pdown ��װ
// sunhai //////////////////////////////////////////////////////////////////////////////////////

enum DOWNLOAD_STATE
{
	download_state_undownload			= 0,			// ����GetInRecycleFlagΪTRUE����ʾ���������У��������������С�
	download_state_downloading			= 3,			// ��������
	download_state_downloadpause		= 4,			// ��������
	download_state_waitingfordownload	= 6,			// ��������
	download_state_installing			= 7,			// �����ع���
	download_state_waitingforinstall	= 8,			// �����ع���
	download_state_waitingforinstall_x	= 9,			// �����ع���
	download_state_downloadprepare		= 10,			// ��������

	download_state_downloadSuccess		= 11,			// 2010-03-26  ���سɹ�
														// �������ͨ��

	download_state_allinrecycle			= MAXWORD - 12,	// ö�ٻ���վ
	download_state_alldownloaded		= MAXWORD - 11,	// ö��������
	download_state_alldownloading		= MAXWORD - 10,	// ö��������
};

enum DOWNLOAD_NOTIFY_ACTION
{
	download_notify_statuschanged		= 1,			// �������״̬�ı�
	download_notify_actionchanged		= 2,			// �����Ϊ����ı䣬�������С�-�������ء�-������վ
};

enum DOWNLOAD_ERROR_CODE
{
	download_error_ok						= 0,
	download_error_nourlinfo				= 1,
	download_error_CallbackFaild			= 2,
	download_error_CallbackUnkwn			= 3,
	download_error_CallbackTmout			= 4,
	download_error_DLDirNotExist			= 5,
	download_error_DLDirNotWritable			= 6,
	download_error_DLDirNotEnoughSpace		= 7,
	download_error_PPStartTaskError			= 8,
	download_error_UserManualCancel			= 9,
	download_error_QueryDlInfoFailed		= 10,

	download_error_UrlInfoIsNull			= 11,
	download_error_UrlInfoError				= 12,
	download_error_UrlInfoNotXml			= 13,
	download_error_CreateXmlDocError		= 14,
	download_error_DownloadUrlInfoBufferError	= 15,
	download_error_Downloadfromcache		= 16,
	download_error_DownloadModuleError		= 17,
	download_error_restorefromrecycle		= 18,
	download_error_modulenotinitorfaild		= 19,
	download_error_dlurlmodulenotwork		= 20,

	download_error_UserManualPause			= 21,

	download_error_DataStructIsNull			= 22,
};

enum DOWNLOAD_TAB_TYPE
{
	download_tab_ess					= 1,
	download_tab_bao					= 2,
	download_tab_upt					= 3,
	download_tab_mbl					= 4,
};

enum DOWNLOAD_CONFIRM_TYPE
{
	download_confirm_plugin				= 1,
	download_status_alldown				= 2,
};

// 2010-03-26 �������������ʱ���˳�����
// �������ͨ��
// DWORD ���� ����������ʱ �������� 
#define  DOWNLOAD_EXIT_MODE			L"Options\\ExitMode"
#define  DOWNLOAD_EXIT_MODE_UNKNOWN			0	
#define  DOWNLOAD_EXIT_MODE_INDIRECTEXIT	1	// ֱ���˳�
#define  DOWNLOAD_EXIT_MODE_TRAY			2	// �����̺�̨����

// BOOL���ͣ�TRUEΪĬ��ֵ��TRUE������غ�������װ��װ�������ֶ���װ
#define  DOWNLOAD_INSTALLAFTERDOWNLOADED			L"Options\\DownloadInstall"

// CLEARTEMP_CONDITION���ͣ�cleartemp_per_weekΪĬ��ֵ����ʾɾ�������ļ���ʱ��
#define  DOWNLOAD_CLEARTEMPFILECONDITION			L"Options\\DelOldFilePerWeek"
enum CLEARTEMP_CONDITION
{
	cleartemp_nodelete = 0,
	cleartemp_per_week,
	cleartemp_afterdl,
};

// BOOL���ͣ�TRUEΪĬ��ֵ��TRUE��ʾ�����в���������ʾ��������ʾ
#define  DOWNLOAD_TIPMESSAGEBEFOREDOWNLOAD			L"Options\\TipBeforeDownload"

// �ַ������ͣ���ʾ����Ŀ¼
#define  DOWNLOAD_DOWNLOADFOLDER					L"Options\\DownloadFolder"
#define  DOWNLOAD_DEFAULTDOWNLOADFOLDER				L"Options\\DefaultDownloadFolder"

class SOM_NO_VTABLE IDownloadData : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_DOWNLOADDATA };

	// ��� DWORD
	STDMETHOD_(DWORD, GetValueAsDWORD)(LPCWSTR lpwszKey) PURE;
	// ��� LPWSTR
	STDMETHOD_(LPCWSTR, GetValueAsCWSTR)(LPCWSTR lpwszKey) PURE;

	STDMETHOD_(double, GetProgress)() PURE;
	STDMETHOD_(DOWNLOAD_STATE, GetDownloadState)() PURE;
	STDMETHOD_(DWORD, GetSoftSize)() PURE;
	STDMETHOD_(long, GetLastModifyTime)() PURE;
	STDMETHOD_(DOWNLOAD_ERROR_CODE, GetLastError)() PURE;
	STDMETHOD_(LPCWSTR, GetHintTipText)() PURE;
	STDMETHOD_(LPCWSTR, GetPluginName)() PURE;
	STDMETHOD_(LPCWSTR, GetSupportSystem)() PURE;
	STDMETHOD_(LPCWSTR, GetInstPath)() PURE;
	STDMETHOD_(LPCWSTR, GetSoftName)() PURE;

	STDMETHOD_(LPVOID, GetDownloadParam)() PURE;
	STDMETHOD_(UINT32, GetDownloadRate)() PURE;

	STDMETHOD_(HICON, GetFileIcon)() PURE;

	STDMETHOD_(BOOL, GetInRecycleFlag)() PURE;

	STDMETHOD_(BOOL, GetSupportSystemFlag)() PURE;

	STDMETHOD_(LPCWSTR, GetImgUrl)() PURE;
};

class SOM_NO_VTABLE IDownloadMgr : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_DOWNLOADMGR };

	// �û�ȷ������Ϣ�� WPARAMΪDOWNLOAD_CONFIRM_TYPE�� download_confirm_plugin���ͣ� LPARAMΪ���ID�� GetSupportSystemFlag��GetPluginName��GetHintTipText������ݸ���ҳ
	// �����ظ�����Ϣ�� WPARAMΪDOWNLOAD_CONFIRM_TYPE�� download_status_alldown���ͣ� LPARAMΪ0��ͨ��QueryAllDownloadStatus�ӿڻ�ȡ��������Ϣ
	STDMETHOD_(BOOL, Init)(HWND hMainWnd, DWORD dwMsgRet) PURE;
	STDMETHOD_(BOOL, Uninit)() PURE;

	// ֪ͨ����Ϣ�� WPARAMΪ MAKEPARAM(DOWNLOAD_NOTIFY_ACTION, DOWNLOAD_STATE) ���ͱ�ʶ�����ǰ״̬�� LPARAMΪ���ID
	STDMETHOD_(BOOL, AdviseNotifyWnd)(HWND hWnd, DWORD dwMsgRet) PURE;
	STDMETHOD_(BOOL, UnadviseNotifyWnd)(HWND hWnd) PURE;

	STDMETHOD_(BOOL, StartDownloadByID)(DWORD dwSoftID, LPCWSTR lpwszSoftName, DOWNLOAD_TAB_TYPE dwFromTab, LPCWSTR lpwszImgInfo, LPVOID lpParam) PURE;
	STDMETHOD_(BOOL, ResumeDownloadByID)(DWORD dwSoftID) PURE;
	STDMETHOD_(BOOL, RestoreDownloadByID)(DWORD dwSoftID) PURE;
	STDMETHOD_(BOOL, PauseDownloadByID)(DWORD dwSoftID, DOWNLOAD_ERROR_CODE decError = download_error_UserManualPause) PURE;
	STDMETHOD_(BOOL, CancelDownloadByID)(DWORD dwSoftID) PURE;
	STDMETHOD_(BOOL, DeleteDownloadByID)(DWORD dwSoftID) PURE;
	STDMETHOD_(BOOL, InstallDownloadSoftByID)(DWORD dwSoftID) PURE;

	STDMETHOD_(BOOL, QueryDownloadInfo)(DWORD dwSoftID, IDownloadData** lpDownloadData) PURE;

	STDMETHOD_(BOOL, ClearMobileTask)() PURE;

	STDMETHOD_(LPCWSTR, GetDefaultDlDirectory)() PURE;

	STDMETHOD_(LPCWSTR, GetCurrentDlDirectory)() PURE;

	STDMETHOD_(DOWNLOAD_ERROR_CODE, MakeSureDownloadDirectoryValid)(LPCWSTR lpwszDirectoryPath = NULL, DWORD dwSoftSizeInMB = 50) PURE;

	STDMETHOD_(BOOL, ModifyDownloadSetting)() PURE;

	STDMETHOD_(DWORD, QueryDownloadArrayByState)(DOWNLOAD_STATE dsState, ISomBinaryBuffer** lppBuffer) PURE;

	STDMETHOD_(BOOL, SetNotifyActionChange)(BOOL bNotifyActionChange) PURE;

	// *pdwDownloadStateֻ������3��״̬��
	// download_state_undownloadΪ�����أ���ʾ����������������Լ����鿴���顱�͡��������á���ť��*pdwTaskCount��ʶ������������
	// download_state_downloadingΪ�������أ���ʾ���ؽ��ȣ��Լ����鿴���顱��*pdwTaskCount��ʶ��������������
	// download_state_downloadprepareΪ׼�����أ����Ŷ������Լ����鿴���顱��*pdwTaskCount��ʶ��������������
	STDMETHOD_(BOOL, QueryAllDownloadStatus)(ISomString** lppStrDownloadStatus = NULL, DOWNLOAD_STATE* pdwDownloadState = NULL, DWORD* pdwDownloadRate = NULL, double* pdbDownloadProgress = NULL, DWORD* pdwTaskCount = NULL) PURE;

	STDMETHOD_(LPCWSTR, FormatErrorText)(DOWNLOAD_ERROR_CODE decError) PURE;

	STDMETHOD_(DWORD, QuerySoftSizeByState)(DOWNLOAD_STATE dsState) PURE;

	STDMETHOD_(BOOL, DownloadSoftUrls)(ISomDWORDList* lpSoftIDList) PURE;
};

class SOM_NO_VTABLE IDownloadMgr2 : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_DOWNLOADMGR2 };

	// �û�ȷ������Ϣ�� WPARAMΪDOWNLOAD_CONFIRM_TYPE�� download_confirm_plugin���ͣ� LPARAMΪ���ID�� GetSupportSystemFlag��GetPluginName��GetHintTipText������ݸ���ҳ
	// �����ظ�����Ϣ�� WPARAMΪDOWNLOAD_CONFIRM_TYPE�� download_status_alldown���ͣ� LPARAMΪ0��ͨ��QueryAllDownloadStatus�ӿڻ�ȡ��������Ϣ
	STDMETHOD_(BOOL, Init)(HWND hMainWnd, DWORD dwMsgRet) PURE;
	STDMETHOD_(BOOL, Uninit)() PURE;

	// ֪ͨ����Ϣ�� WPARAMΪ MAKEPARAM(DOWNLOAD_NOTIFY_ACTION, DOWNLOAD_STATE) ���ͱ�ʶ�����ǰ״̬�� LPARAMΪ���ID
	STDMETHOD_(BOOL, AdviseNotifyWnd)(HWND hWnd, DWORD dwMsgRet) PURE;
	STDMETHOD_(BOOL, UnadviseNotifyWnd)(HWND hWnd) PURE;

	STDMETHOD_(BOOL, StartDownloadByID)(DWORD dwSoftID, LPCWSTR lpwszSoftName, DOWNLOAD_TAB_TYPE dwFromTab, LPCWSTR lpwszImgInfo, LPVOID lpParam) PURE;
	STDMETHOD_(BOOL, ResumeDownloadByID)(DWORD dwSoftID) PURE;
	STDMETHOD_(BOOL, RestoreDownloadByID)(DWORD dwSoftID) PURE;
	STDMETHOD_(BOOL, PauseDownloadByID)(DWORD dwSoftID, DOWNLOAD_ERROR_CODE decError = download_error_UserManualPause) PURE;
	STDMETHOD_(BOOL, CancelDownloadByID)(DWORD dwSoftID) PURE;
	STDMETHOD_(BOOL, DeleteDownloadByID)(DWORD dwSoftID) PURE;
	STDMETHOD_(BOOL, InstallDownloadSoftByID)(DWORD dwSoftID) PURE;

	STDMETHOD_(BOOL, QueryDownloadInfo)(DWORD dwSoftID, IDownloadData** lpDownloadData) PURE;

	STDMETHOD_(BOOL, ClearMobileTask)() PURE;

	STDMETHOD_(LPCWSTR, GetDefaultDlDirectory)() PURE;

	STDMETHOD_(LPCWSTR, GetCurrentDlDirectory)() PURE;

	STDMETHOD_(DOWNLOAD_ERROR_CODE, MakeSureDownloadDirectoryValid)(LPCWSTR lpwszDirectoryPath = NULL, DWORD dwSoftSizeInMB = 50) PURE;

	STDMETHOD_(BOOL, ModifyDownloadSetting)() PURE;

	STDMETHOD_(DWORD, QueryDownloadArrayByState)(DOWNLOAD_STATE dsState, ISomBinaryBuffer** lppBuffer) PURE;

	STDMETHOD_(BOOL, SetNotifyActionChange)(BOOL bNotifyActionChange) PURE;

	// *pdwDownloadStateֻ������3��״̬��
	// download_state_undownloadΪ�����أ���ʾ����������������Լ����鿴���顱�͡��������á���ť��*pdwTaskCount��ʶ������������
	// download_state_downloadingΪ�������أ���ʾ���ؽ��ȣ��Լ����鿴���顱��*pdwTaskCount��ʶ��������������
	// download_state_downloadprepareΪ׼�����أ����Ŷ������Լ����鿴���顱��*pdwTaskCount��ʶ��������������
	STDMETHOD_(BOOL, QueryAllDownloadStatus)(ISomString** lppStrDownloadStatus = NULL, DOWNLOAD_STATE* pdwDownloadState = NULL, DWORD* pdwDownloadRate = NULL, double* pdbDownloadProgress = NULL, DWORD* pdwTaskCount = NULL) PURE;

	STDMETHOD_(LPCWSTR, FormatErrorText)(DOWNLOAD_ERROR_CODE decError) PURE;

	STDMETHOD_(DWORD, QuerySoftSizeByState)(DOWNLOAD_STATE dsState) PURE;

	STDMETHOD_(BOOL, DownloadSoftUrls)(ISomDWORDList* lpSoftIDList) PURE;

	STDMETHOD_(BOOL, InsertSoftUrlInfo)(DWORD dwSoftID, LPCWSTR lpwszSoftName, LPCWSTR lpwszUrl, LPCWSTR lpwszFileName, LPCWSTR lpwszInstallParam, LPCWSTR lpwszHint2, LPCWSTR lpwszPluginName, LPCWSTR lpwszSystem, LPCWSTR lpwszVersion, LPCWSTR lpwszFileSize) PURE;
};

// 2010-03-26 Ϊ֧������������ �����µ����ؽӿ�
// �������ͨ��
#define DOWNLOAD_MGR_TYPE_DOWNLOAD		0		// ���������װ��
#define DOWNLOAD_MGR_TYPE_UPDATE		1		// �������������(��������û�������� ���°�װ��)

class SOM_NO_VTABLE IDownloadMgr3 : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_DOWNLOADMGR3 };

	// �û�ȷ������Ϣ�� WPARAMΪDOWNLOAD_CONFIRM_TYPE�� download_confirm_plugin���ͣ� LPARAMΪ���ID�� GetSupportSystemFlag��GetPluginName��GetHintTipText������ݸ���ҳ
	// �����ظ�����Ϣ�� WPARAMΪDOWNLOAD_CONFIRM_TYPE�� download_status_alldown���ͣ� LPARAMΪ0��ͨ��QueryAllDownloadStatus�ӿڻ�ȡ��������Ϣ
	STDMETHOD_(BOOL, Init)(HWND hMainWnd, DWORD dwMsgRet) PURE;
	STDMETHOD_(BOOL, Uninit)() PURE;

	// ֪ͨ����Ϣ�� WPARAMΪ MAKEPARAM(DOWNLOAD_NOTIFY_ACTION, DOWNLOAD_STATE) ���ͱ�ʶ�����ǰ״̬�� LPARAMΪ���ID
	STDMETHOD_(BOOL, AdviseNotifyWnd)(HWND hWnd, DWORD dwMsgRet) PURE;
	STDMETHOD_(BOOL, UnadviseNotifyWnd)(HWND hWnd) PURE;

	STDMETHOD_(BOOL, StartDownloadByID)(DWORD dwSoftID, LPCWSTR lpwszSoftName, DOWNLOAD_TAB_TYPE dwFromTab, LONG lDLType, LPCWSTR lpwszImgInfo, LPVOID lpParam) PURE;
	STDMETHOD_(BOOL, ResumeDownloadByID)(DWORD dwSoftID) PURE;
	STDMETHOD_(BOOL, RestoreDownloadByID)(DWORD dwSoftID) PURE;
	STDMETHOD_(BOOL, PauseDownloadByID)(DWORD dwSoftID, DOWNLOAD_ERROR_CODE decError = download_error_UserManualPause) PURE;
	STDMETHOD_(BOOL, CancelDownloadByID)(DWORD dwSoftID) PURE;
	STDMETHOD_(BOOL, DeleteDownloadByID)(DWORD dwSoftID) PURE;
	STDMETHOD_(BOOL, InstallDownloadSoftByID)(DWORD dwSoftID) PURE;

	STDMETHOD_(BOOL, QueryDownloadInfo)(DWORD dwSoftID, IDownloadData** lpDownloadData) PURE;

	STDMETHOD_(BOOL, ClearMobileTask)() PURE;

	STDMETHOD_(LPCWSTR, GetDefaultDlDirectory)() PURE;

	STDMETHOD_(LPCWSTR, GetCurrentDlDirectory)() PURE;

	STDMETHOD_(DOWNLOAD_ERROR_CODE, MakeSureDownloadDirectoryValid)(LPCWSTR lpwszDirectoryPath = NULL, DWORD dwSoftSizeInMB = 50) PURE;

	STDMETHOD_(BOOL, ModifyDownloadSetting)() PURE;

	STDMETHOD_(DWORD, QueryDownloadArrayByState)(DOWNLOAD_STATE dsState, ISomBinaryBuffer** lppBuffer) PURE;

	STDMETHOD_(BOOL, SetNotifyActionChange)(BOOL bNotifyActionChange) PURE;

	// *pdwDownloadStateֻ������3��״̬��
	// download_state_undownloadΪ�����أ���ʾ����������������Լ����鿴���顱�͡��������á���ť��*pdwTaskCount��ʶ������������
	// download_state_downloadingΪ�������أ���ʾ���ؽ��ȣ��Լ����鿴���顱��*pdwTaskCount��ʶ��������������
	// download_state_downloadprepareΪ׼�����أ����Ŷ������Լ����鿴���顱��*pdwTaskCount��ʶ��������������
	STDMETHOD_(BOOL, QueryAllDownloadStatus)(ISomString** lppStrDownloadStatus = NULL, DOWNLOAD_STATE* pdwDownloadState = NULL, DWORD* pdwDownloadRate = NULL, double* pdbDownloadProgress = NULL, DWORD* pdwTaskCount = NULL) PURE;

	STDMETHOD_(LPCWSTR, FormatErrorText)(DOWNLOAD_ERROR_CODE decError) PURE;

	STDMETHOD_(DWORD, QuerySoftSizeByState)(DOWNLOAD_STATE dsState) PURE;

	STDMETHOD_(BOOL, DownloadSoftUrls)(ISomDWORDList* lpSoftIDList) PURE;

	STDMETHOD_(BOOL, InsertSoftUrlInfo)(DWORD dwSoftID, LPCWSTR lpwszSoftName, LPCWSTR lpwszUrl, LPCWSTR lpwszUpUrl, LPCWSTR lpwszFileName, LPCWSTR lpwszInstallParam, LPCWSTR lpwszHint2, LPCWSTR lpwszPluginName, LPCWSTR lpwszSystem, LPCWSTR lpwszVersion, LPCWSTR lpwszFileSize) PURE;
};


// sunhai //////////////////////////////////////////////////////////////////////////////////////


// ��ٿ����ݣ�������Ϣ
class SOM_NO_VTABLE IBaikeStar : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_BAIKE_STAR };
public:
	STDMETHOD_(LPCWSTR, GetStarPoint)() PURE;
	STDMETHOD_(LPCWSTR, GetStarLink)() PURE;
	STDMETHOD_(LPCWSTR, GetStarPollNum)() PURE;
	STDMETHOD_(LPCWSTR, GetSoftName)() PURE;
	STDMETHOD_(LPCWSTR, GetCategoryName)() PURE;

	STDMETHOD_(BOOL, IsCacheData)() PURE;

};

// ��ٿ����ݣ�һ�仰�����Ϣ
class SOM_NO_VTABLE IBaikeOneWord : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_BAIKE_ONEWORD };
public:
	STDMETHOD_(LPCWSTR, GetOneWordDes)() PURE;
	STDMETHOD_(LPCWSTR, GetSoftName)() PURE;
	STDMETHOD_(LPCWSTR, GetCategoryName)() PURE;
	STDMETHOD_(BOOL, IsCacheData)() PURE;

};

// ��ٿ�����
class SOM_NO_VTABLE IBaike : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_BAIKE };
public:
	STDMETHOD_(BOOL,SubmitBaikeStarInfo)(ISomList* pSoftNameList, LPCWSTR szTabKey,ISomAsyncResult* pEvent) PURE;
	STDMETHOD_(BOOL,GetBaikeStarInfo)(LPCWSTR pName,IBaikeStar** pItem) PURE;

	STDMETHOD_(BOOL,SubmitBaikeOneWordInfo)(ISomList* pSoftNameList,LPCWSTR szTabKey,ISomAsyncResult* pEvent) PURE;
	STDMETHOD_(BOOL,GetBaikeOneWordInfo)(LPCWSTR pName,IBaikeOneWord** pItem) PURE;

	STDMETHOD_(BOOL,Init)()PURE;
	STDMETHOD_(BOOL,UnInit)()PURE;
	STDMETHOD_(BOOL,GetBaikeStarInfoFromCache)(LPCWSTR pName,IBaikeStar** pItem)PURE;
	STDMETHOD_(BOOL,GetBaikeOneWordInfoFromCache)(LPCWSTR pName,IBaikeOneWord** pItem)PURE;

};


// ��������Ϣ ��ôӰ������в�ѯ�õ�����Ϣ
class SOM_NO_VTABLE IWhiteListInfo : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_WHITELIST_INFO };

	// ��ð�ȫ����
	STDMETHOD_(DWORD, GetSecurityLever)() PURE;
	// ������� ��Ϊ��������������õ� ���������û������ ���ֵ������
	STDMETHOD_(DWORD, GetKey)() PURE;

	STDMETHOD_(BOOL, GetFileName)(ISomString** pPath) PURE;
	STDMETHOD_(BOOL, GetDescription)(ISomString** pDesp) PURE;
};

// ������
class SOM_NO_VTABLE IWhiteList : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_WHITELIST };

	// �����ϲ�ѯ��Ϣ �첽���ú���
	// pPathList: �ļ�·���б�
	// pKeys:  ��ʾÿһ���keyֵ����
	// nKeyCnt: keyֵ�����С
	// pEvent: �ص��¼� ���ú������غ� ���¼���õ�֪ͨ ���ҷ���һ����������Ϣ�б�
	STDMETHOD_(BOOL, QueryFilesInfo)(ISomList* pPathList, int* pKeys, DWORD nKeyCnt, ISomAsyncResult* pEvent) PURE;
	
	// �ӻ����ļ��в�ѯ��Ϣ ͬ�����ú���
	// pszPath: �ļ�·��
	// ppInfo:  ����ɹ�����һ����������Ϣ���� ������
	STDMETHOD_(BOOL, QuickQueryFilesInfo)(LPCWSTR pszPath, IWhiteListInfo** ppInfo) PURE;
};

// ͼ�����
class SOM_NO_VTABLE IFileIconMgr : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_FileIconMgr };

	STDMETHOD_(HICON, QueryFileIcon)(LPCWSTR lpIconPath) PURE;
};

// ���øı�֪ͨ�ص����� 
// pParam:		Ϊע��ʱ����Ĳ���
// pszKeyPath:	�ı��Key
typedef void (*INI_NOTIFY_FUNC)(LPVOID pParam, LPCWSTR pszKeyPath);

// �����ļ�
class SOM_NO_VTABLE ISomIni : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_SOMINI };

	STDMETHOD_(BOOL, GetValueDWORD)(LPCWSTR pszPathKey, DWORD* pdwValue) PURE;
	STDMETHOD_(BOOL, SetValueDWORD)(LPCWSTR pszPathKey, DWORD dwValue) PURE;
	STDMETHOD_(BOOL, GetValueString)(LPCWSTR pszPathKey, ISomString** pStrValue) PURE; 
	STDMETHOD_(BOOL, SetValueString)(LPCWSTR pszPathKey, LPCWSTR pszVal) PURE;

	// ����INI�ļ����޸� д���ļ���
	STDMETHOD_(BOOL, Flush)() PURE;

	/*
	 * ע��keyֵ�ı�֪ͨ����
	 * pszPathKey: ��ֵ�ı���Ҫ֪ͨ��key
	 * pFunc: �ص�����
	 * pParam: �ص�ʱ�����ص������Ĳ���
	 * pDwCookie: ����һ��cookie ��ע����֪ͨʱ���õ�
	 */
	STDMETHOD_(BOOL, AdviseNotify)(LPCWSTR pszPathKey, INI_NOTIFY_FUNC pFunc, LPVOID pParam, DWORD* pDwCookie) PURE;
	/*
	 * ע��һ��keyֵ�ı�֪ͨ
	 * dwCookie:ע��ʱ���ص�ֵ
	 */
	STDMETHOD_(BOOL, UnadviseNotify)(DWORD dwCookie) PURE;
};

// SomVaribale
class SOM_NO_VTABLE ISomVariable : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_SOMVARIABLE };

	STDMETHOD_(BOOL, LoadVarXmlNode)(IXMLDOMNode* pXmlNode) PURE;
	STDMETHOD_(BOOL, ParseString)(LPCWSTR lpVariable, ISomString** outString, BOOL bExpandEnvironmentStrings = FALSE) PURE;

	STDMETHOD_(int, GetVariableCount)() PURE;
	STDMETHOD_(LPCWSTR, GetVariableById)(int id) PURE;
	STDMETHOD_(LPCWSTR, GetVariableByIndex)(int index) PURE;
};

// ���������Ϣ
#define  SOFTUPDATE_ASSIST_SHOWPERIOD			L"ShowUptMsg\\ShowPeriod"

enum SOFTUPDATE_NOTIFY_FLAG
{
	// �������������
	softupdate_notify_detected		= 0x00000001,
	// ��������������
	softupdate_notify_fillinfo		= 0x00000004,
	// ��ʼ���Ŷ���
	softupdate_notify_beginupdate	= 0x00000008,
	// �������Ŷ���
	softupdate_notify_endupdate		= 0x00000010,
	// ������ʾ��������
	softupdate_notify_updatenum		= 0x00000011,
};

enum SOFTUPDATE_DETECT_FLAG
{
	softupdate_detect_onlynum		= 0,
	softupdate_detect_fillinf		= 1,
	softupdate_detect_alldetc		= 2,
};

class SOM_NO_VTABLE ISoftUpdateData : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_SOFTUPDATEDATA };

	// ��� DWORD
	STDMETHOD_(DWORD, GetValueAsDWORD)(LPCWSTR lpwszKey) PURE;
	// ��� LPWSTR
	STDMETHOD_(LPCWSTR, GetValueAsCWSTR)(LPCWSTR lpwszKey) PURE;

	STDMETHOD_(BOOL, IsValid)() PURE;

	STDMETHOD_(DWORD, GetSoftID)() PURE;

	STDMETHOD_(LPCWSTR, GetSoftName)() PURE;
	STDMETHOD_(LPCWSTR, GetVernFile)() PURE;
	STDMETHOD_(LPCWSTR, GetIconFile)() PURE;

	STDMETHOD_(LPCWSTR, GetOldVersion)() PURE;
	STDMETHOD_(LPCWSTR, GetNewVersion)() PURE;

	STDMETHOD_(LPCWSTR, GetPublishDate)() PURE;
	STDMETHOD_(LPCWSTR, GetDescription)() PURE;

	STDMETHOD_(BOOL, GetIgnoreFlag)() PURE;

	STDMETHOD_(DWORD, GetStarNum)() PURE;
	STDMETHOD_(LPCWSTR, GetStarPoint)() PURE;
	STDMETHOD_(LPCWSTR, GetStarLink)() PURE;
	STDMETHOD_(LPCWSTR, GetStarPollNum)() PURE;

	STDMETHOD_(LPCWSTR, GetDetailUrl)() PURE;

	STDMETHOD_(BOOL, GetSupportOS)() PURE;
	STDMETHOD_(DWORD, GetSoftSize)() PURE;
	STDMETHOD_(BOOL, GetShareSoftFlag)() PURE;
	STDMETHOD_(BOOL, GetPluginSoftFlag)() PURE;
	STDMETHOD_(BOOL, GetBetaSoftFlag)() PURE;
	STDMETHOD_(BOOL, GetEnglishSoftFlag)() PURE;

	//2010-4-15 by ireton
	STDMETHOD_(BOOL, IsImportant)() PURE;
	STDMETHOD_(LPCWSTR, GetUpdatePercent)() PURE;
	STDMETHOD_(LPCWSTR, GetEid)() PURE;
};

class SOM_NO_VTABLE ISoftUpdate : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_SOFTUPDATE };

	STDMETHOD_(BOOL, Init)() PURE;
	STDMETHOD_(BOOL, Uninit)() PURE;

	// ע���������֪ͨ������ں���Ϣ��Ϣ
	STDMETHOD_(BOOL, AdviseNotifyWnd)(HWND hWnd, DWORD dwMsgRet) PURE;
	STDMETHOD_(BOOL, UnadviseNotifyWnd)(HWND hWnd) PURE;

	// ͬ����ȡ��������������������Ѻ��Ե����������
	STDMETHOD_(DWORD, GetNeedUpdateSoftCount)() PURE;

	// �첽������������Ϣ�������Ѻ��Ե������������bQueryOnline����ΪTRUE��ʶ�������ȡ���ݣ�FALSE��ֻ�б��ػ�����Ϣ��
	// ͨ��AdviseNotifyWndע�����Ϣ֪ͨ��WPARAMΪSOFTUPDATE_NOTIFY_FLAG��ʶ֪ͨ��ɵ��ǻ�����Ϣ����������Ϣ��LPARAMΪDWORD��ʶ�������������
	// ͨ��AdviseNotifyWndע�����Ϣ֪ͨ��WPARAMΪsoftupdate_notify_beginupdate��ʶ��ʼ������LPARAMΪ0
	// ͨ��AdviseNotifyWndע�����Ϣ֪ͨ��WPARAMΪsoftupdate_notify_endupdate��ʶ����������LPARAMΪ0
	// ͨ��AdviseNotifyWndע�����Ϣ֪ͨ��WPARAMΪsoftupdate_notify_updatenum��ʶ��������������LPARAMΪDWORD��ʶ�������������
	STDMETHOD_(BOOL, DetectAllSoft)(SOFTUPDATE_DETECT_FLAG suDetectFlag) PURE;

	// ��ȡ����б�����SOFTUPDATE_QUERYLIST_FLAG������ʾ�����ͺ������������ID�б�
	STDMETHOD_(BOOL, QueryNeedUpdateSoftList)(ISomBinaryBuffer** lppSoftIDArray, BOOL bIgnoreList = FALSE) PURE;
	// ��ȡ���������Ϣ
	STDMETHOD_(BOOL, QueryItemData)(DWORD dwSoftID, ISoftUpdateData** pISoftUpdateData) PURE;

	STDMETHOD_(BOOL, GetErrorString)(ISomString** ppStrErrorSoftVern) PURE;
	STDMETHOD_(BOOL, SetIgnoreSoft)(DWORD dwSoftID, BOOL bIgnoreIt) PURE;

	// ��������װ��Ϣ������Ҫ�Ĳ������Դ�NULL
	STDMETHOD_(BOOL, CheckSoftExist)(DWORD dwSoftID, BOOL bReCheck = TRUE, BOOL* pbNeedUpdate = NULL, LPPOINT lpOldVersion = NULL, ISomString** pStrDispVersion = NULL, ISomString** pStrExecPath = NULL, ISomString** pStrIconPath = NULL) PURE;

	// ��ȡ����б����ݷ����Ѿ���װ�����ID�б�,�б�ĸ�ʽΪDWORD�飨ID��STATE��ID��STATE�������Դ����ƣ�,STATE��0δ��װ��1�Ѱ�װ��2����������ֱ�Ӵ���ҳ�档
	STDMETHOD_(BOOL, QueryInstalledSoftList)(ISomBinaryBuffer** lppSoftIDArray) PURE;

	STDMETHOD_(BOOL, TransOldIgnoredList)() PURE;
};

// ��������װ��Ϣ
class SOM_NO_VTABLE ISoftInstalledInfo : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_SOFTINSTALLEDINFO };
};

// ͳ����Ϣ

// ������ͳ��URL %s��ʾMID
#define STATISTICS_URL_MAIN L"http://softm.360safe.com/stat/?type=open&action=mainapp&"		// װ���ر�

// ����ҳ��ͳ��URL %s��ʾMID
#define STATISTICS_URL_ESS L"http://softm.360safe.com/stat/?type=open&action=page1&"		// װ���ر�
#define STATISTICS_URL_BAO L"http://softm.360safe.com/stat/?type=open&action=page8&"		// �������
#define STATISTICS_URL_UPT L"http://softm.360safe.com/stat/?type=open&action=page7&"		// �������
#define STATISTICS_URL_UNI L"http://softm.360safe.com/stat/?type=open&action=page4&"		// ���ж��
#define STATISTICS_URL_MBL L"http://softm.360safe.com/stat/?type=open&action=page11&"		// �ֻ��ر�
#define STATISTICS_URL_AUT L"http://softm.360safe.com/stat/?type=open&action=page9&"		// ��������
#define STATISTICS_URL_RUN L"http://softm.360safe.com/stat/?type=open&action=page3&"		// ��������
#define STATISTICS_URL_DEF L"http://softm.360safe.com/stat/?type=open&action=page10&"		// ����Ĭ��
#define STATISTICS_URL_POP L"http://softm.360safe.com/stat/?type=open&action=page6&"		// ������Ϸ
#define STATISTICS_URL_GJG L"http://softm.360safe.com/stat/?type=open&action=page12&"		// �߼�����

// װ���ر���������⡢�������ٵķ���ͳ��URL %s��һ����ʾ�������ƣ��ڶ�����ʾMID
#define STATISTICS_URL_CAT L"http://softm.360safe.com/stat/?type=open&action=%s&"

// ����ͳ��URL %d����Ϊ������ʾ����0(����ʾ��/1����ʾ���������װ����0���ֶ���װ��/1��������װ�����Զ�ɾ������0����ɾ����/1��һ�ܺ�ɾ����/2����װ��ɾ��������������Ƶ��0������ʾ��/1��ÿ��һ�Σ�/7��ÿ��һ�Σ����������ֿ���0����������/1����������ж�ؿ������0������ʾ��/1����ʾ����ж�ؿ�ʼ�˵�0������ʾ��/1����ʾ����ɾ�����վ����0������ɾ����/1��ɾ�����վ��
#define STATISTICS_URL_SET L"http://softm.360safe.com/stat/?type=setting&action=%d_%d_%d_%d_%d_%d_%d_%d&"

// ȡ������
#define STATISTICS_URL_CANCEL L"http://softm.360safe.com/stat/?type=canceldownload&softid=%d&"

// һ���Ż�
#define STATISTICS_URL_OPTIMIZATION L"http://softm.360safe.com/stat/?type=open&action=optimization&"

// ����������ٽ����еġ��ر�ʱ��С���֡�
#define STATISTICS_URL_STARTTIME_ASSISTANT L"http://softm.360safe.com/stat/?type=open&action=CloseStarttimeAssistant&"

// �������С����ͳ��
#define STATISTICS_URL_SOFTUPDATE_ASSISTANT L"http://softm.360safe.com/stat/?type=SoftUptMsg&RunSoftMgr=%d&SelfExit=%d&NoLongerDisp=%d&"

// ����ʱ��С����ͳ��
#define STATISTICS_URL_SPEEDID_ASSISTANT L"http://softm.360safe.com/stat/?type=SpeedID&RunSoftMgr=%d&SelfExit=%d&NoLongerDisp=%d&"

// ����������°湦��ͳ��
#define STATISTICS_URL_NEW_FEATURE		L"http://softm.360safe.com/stat/?type=open&action=NewFeature&"

// ��������в�������ͳ��
#define STATISTICS_URL_NOLONGER_NOTIFY	L"http://softm.360safe.com/stat/?type=open&action=NolongerNotify&"

// �������������ȫ�����ͳ��
#define STATISTICS_URL_UPDATE_ALL		L"http://softm.360safe.com/stat/?type=open&action=UpdateAllSoftware&"

// 2010.03.09 ����ҳͳ�Ƶ�ַ
// 2010.03.09 ����������ͨ��
#define STATISTICS_URL_DETAIL			L"http://softm.360safe.com/stat/info.html?sid=%d&tab=%d&"

//2010.04.16 by ireton ����������ͳ�ƣ�����ͳ����������
#define	STATISTICS_URL_UPDATE_FINISH	L"http://softm.360safe.com/stat/?type=update&editionid=%s&"

// ����������ͳ�Ƶ�ַ
#define	STATISTICS_URL_START_ITEM_OPR	L"http://softm.360safe.com/stat/?type=start&class=%d&sname=%s&operate=%d&IsOptmz=%d&"

class SOM_NO_VTABLE IStatisticsMgr : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_STATISTICSMGR };

	// ��Get��ʽ�������� 
	// pszURL : ���͵ĵ�ַ
	// pEvent: �¼�����
	// ���øú�������������� ���¼����ͽ����� ��ͨ��pEvent�ӿ� ֪ͨ������ pEventΪnull ����֪ͨ
	STDMETHOD_(BOOL, SendGetInfo)(PCWSTR pszURL, ISomAsyncResult* pEvent) PURE;

	// ��Post��ʽ�������� 
	// pszURL : ���͵ĵ�ַ
	// pBuf:	���������ݵ�ַ
	// lLen:	���������ݳ���
	// pEvent: �¼�����
	// ���øú�������������� ���¼����ͽ����� ��ͨ��pEvent�ӿ� ֪ͨ������ pEventΪnull ����֪ͨs
	STDMETHOD_(BOOL, SendPostInfo)(PCWSTR pszURL, BYTE* pBuf, long lLen, ISomAsyncResult* pEvent) PURE;

	STDMETHOD_(BOOL, SendGetInfoFormat)(LPCWSTR pszURL, ...) PURE;

	STDMETHOD_(BOOL, SendGetInfoOnlyAction)(LPCWSTR pszKey) PURE;
};


enum LIVEUPDATE_NOTIFY_TYPE
{
	liveupdate_notify_pos,
	liveupdate_notify_end,
};

// ����£�����֪ͨ��
class SOM_NO_VTABLE ILiveUpdate : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_LIVEUPDATE };

	STDMETHOD_(BOOL, Init)() PURE;
	STDMETHOD_(BOOL, Uninit)() PURE;

// ���ؽ�����Ϣ��WPARAMΪLIVEUPDATE_NOTIFY_TYPE��liveupdate_notify_pos��LPARAMΪ���ؽ��ȣ����ڵ���0��С�ڵ���100��
// ���ؽ�����Ϣ��WPARAMΪLIVEUPDATE_NOTIFY_TYPE��liveupdate_notify_end��LPARAMΪ�����Ƿ�ɹ���BOOL���ͣ�
	STDMETHOD_(BOOL, AdviseNotifyWnd)(HWND hWnd, DWORD dwMsg) PURE;
	STDMETHOD_(BOOL, UnadviseNotifyWnd)(HWND hWnd) PURE;

	STDMETHOD_(BOOL, UpdateNow)() PURE;
	STDMETHOD_(BOOL, QueryUpdateResultByFileName)(LPCWSTR lpwszFileName) PURE;
};

//��������
enum ECacheCategory
{
	CACHE_CAT_RESOURCE = 0,			// ��Դ
	CACHE_CAT_XML,					// XML�ļ�
	CACHE_CAT_BAIKE_ONEWORD,		// �ٿ�һ�仰
	CACHE_CAT_BAIKE_STAR,			// �ٿ�һ����������Ϣ
	CACHE_CAT_WHITE_LIST,			// ��������Ϣ
	CACHE_CAT_START_OPTMZ,			// ����������Ϣ

	CACHE_CAT_SOFTDATA,				// ����б��������
	CACHE_CAT_PAGELIST,				// ����б�ҳ������
	CACHE_CAT_CATALOGDATA,			// ����б��������
	CACHE_CAT_CATALOGLIST,			// �������б�����

	CACHE_CAT_DOWNLOADHISTORY,		// ������ʷ��¼

	CACHE_CAT_SOFTUPDATEIGNORELIST,	// ������������б�

	CACHE_CAT_UNINST_SOFT_CATEGORY,	// ���ж�ط���

	CACHE_CAT_NET_IMAGE,			// ����ͼƬ����
	CACHE_CAT_TASK,					// ����ƻ���Ϣ
};

// ������� �ýӿ�֧�ֶ��̰߳�ȫ
class SOM_NO_VTABLE ICache : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_CACHE };

	/*
	 * �������� д ��һ�ζ������� DWORDΪkey
	 * dwCategory: ����
	 * dwKey key
	 */
	STDMETHOD(WriteBufDWORD)(DWORD dwCategory, DWORD dwKey, void* pBuf, DWORD dwBytesLen) PURE;
	STDMETHOD(ReadBufDWORD)(DWORD dwCategory, DWORD dwKey, ISomBinaryBuffer* pBryBuf) PURE;

	/*
	* �������� д ��һ�ζ������� �ַ���λkey
	* dwCategory: ����
	* pKey: key
	*/
	STDMETHOD(WriteBufSTR)(DWORD dwCategory, LPCWSTR pKey, void* pBuf, DWORD dwBytesLen) PURE;
	STDMETHOD(ReadBufSTR)(DWORD dwCategory, LPCWSTR pKey, ISomBinaryBuffer* pBryBuf) PURE;

	// ��������ջ��� ��֧�������������Ļ��� 
	// ��պ�����໺�� ���Ի��������� ����ǰ��string���Ը�Ϊdword
	STDMETHOD(ClearCache)(DWORD dwCategory) PURE;
};

class ICache2 : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_CACHE2 };

	/*
	* �������� д ��һ�ζ������� DWORDΪkey
	* dwCategory: ����
	* dwKey key
	* bCompress : �Ƿ���Ҫѹ��
	* dwParam : �ڻ����ж��һ������
	*/
	STDMETHOD(WriteBufDWORD)(DWORD dwCategory, DWORD dwKey, void* pBuf, DWORD dwBytesLen, BOOL bCompress, DWORD dwParam) PURE;
	STDMETHOD(ReadBufDWORD)(DWORD dwCategory, DWORD dwKey, ISomBinaryBuffer* pBryBuf, DWORD* pdwParam) PURE;

	/*
	* �������� д ��һ�ζ������� �ַ���λkey
	* dwCategory: ����
	* pKey: key
	*/
	STDMETHOD(WriteBufSTR)(DWORD dwCategory, LPCWSTR pKey, void* pBuf, DWORD dwBytesLen, BOOL bCompress, DWORD dwParam) PURE;
	STDMETHOD(ReadBufSTR)(DWORD dwCategory, LPCWSTR pKey, ISomBinaryBuffer* pBryBuf, DWORD* pdwParam) PURE;

	// ��������ջ��� ��֧�������������Ļ��� 
	// ��պ�����໺�� ���Ի��������� ����ǰ��string���Ը�Ϊdword
	STDMETHOD(ClearCache)(DWORD dwCategory) PURE;

	// �رջ��� 
	STDMETHOD(CloseCache)() PURE;
};

const int IMG_MGR_ERROR_PATH_NOT_EXIST = 1;		// ·�������ڴ���
class SOM_NO_VTABLE IImageMgr : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_IMAGE_MGR };

	// �첽���� ����ͼƬ ���Զ�����pszPath����������������ͼƬ���Ǽ��ر���ͼƬ
	// ���ú����̷��� ͼƬ������� ������pAsynRet ��ʾ������� ���pAsynRetΪnull ��֪ͨ
	// Ȼ���ٵ��øú�����ȡͼƬ ��ʱ����pAsynRetλnull
	STDMETHOD_(HBITMAP, LoadImage)(LPCWSTR pszPath, ISomAsyncResult* pAsynRet) PURE;

	// ͬLoadImageһ�� ֻ�����Ǽ���ͼ��
	// ���Ҫ����ȡ.exe�ļ��еĵڼ���ͼ�� ��pszPath�����","�ټ�������  
	// bMaxIcon: �Ƿ��Ǵ�ͼ�� TRUE : 32 * 32 FALSE: 16 * 16
	STDMETHOD_(HICON, LoadIcon)(LPCWSTR pszPath, BOOL bMaxIcon, ISomAsyncResult* pAsynRet) PURE;
};

class SOM_NO_VTABLE IImageMgr2 : public IImageMgr
{
public:
	enum { PLUGINID = SOMPLUGIN_IMAGE_MGR2};

	// �첽���� ����ͼƬ ���Զ�����pszPath����������������ͼƬ���Ǽ��ر���ͼƬ
	// pdwErrCode��ʾ�����ԭ�� 0: ��ʾû�д��� 1:��ʾ·����Ч
	// ���ú����̷��� ͼƬ������� ������pAsynRet ��ʾ������� ���pAsynRetΪnull ��֪ͨ
	// Ȼ���ٵ��øú�����ȡͼƬ ��ʱ����pAsynRetλnull
	STDMETHOD_(HICON, LoadIcon2)(LPCWSTR pszPath, BOOL bMaxIcon, DWORD* pdwErrCode, ISomAsyncResult* pAsynRet) PURE;
};

class SOM_NO_VTABLE IFileShell : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_FILESHELL };

	STDMETHOD_(BOOL, LoadFile)(LPCWSTR lpwszPath, ISomString** lppStrXML) PURE;

};

// �������øı�֪ͨ�ص����� 
// pParam:		Ϊע��ʱ����Ĳ���
// dwParam:		����
typedef void (*SPEED_ASSIST_NOTIFY_FUNC)(LPVOID pParam, DWORD dwParam);

// ����ʱ��С���ֶ���
class SOM_NO_VTABLE ISpeedAssist : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_SPEED_ASSIST };

	// ���¿���ʱ��
	STDMETHOD_(BOOL, UpdateSpeedTime)() PURE;

	// ��ñ��εĿ���ʱ�� 0: ��ʾʧ�� ��0:����ʱ��
	STDMETHOD_(DWORD, GetSpeedTime)() PURE;


	// ���� ����Ƿ���ʾ����С����
	STDMETHOD_(BOOL, IsShowAssist)(BOOL* pbIsShow) PURE;
	STDMETHOD_(BOOL, SetShowAssist)(BOOL bShow) PURE;

	STDMETHOD_(BOOL, AdviseNotify)(SPEED_ASSIST_NOTIFY_FUNC pFunc, LPVOID pParam, DWORD* pDwCookie) PURE;
	/*
	* ע��һ��keyֵ�ı�֪ͨ
	* dwCookie:ע��ʱ���ص�ֵ
	*/
	STDMETHOD_(BOOL, UnadviseNotify)(DWORD dwCookie) PURE;
};

// ����ʱ��С���ֶ���
class SOM_NO_VTABLE ISpeedAssist2 : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_SPEED_ASSIST2 };

	// ���¿���ʱ��
	STDMETHOD_(BOOL, UpdateSpeedTime)() PURE;

	// ��ñ��εĿ���ʱ�� 0: ��ʾʧ�� ��0:����ʱ��
	STDMETHOD_(DWORD, GetSpeedTime)() PURE;


	// ���� ����Ƿ���ʾ����С����
	STDMETHOD_(BOOL, IsShowAssist)(BOOL* pbIsShow) PURE;
	STDMETHOD_(BOOL, SetShowAssist)(BOOL bShow) PURE;

	STDMETHOD_(BOOL, AdviseNotify)(SPEED_ASSIST_NOTIFY_FUNC pFunc, LPVOID pParam, DWORD* pDwCookie) PURE;
	/*
	* ע��һ��keyֵ�ı�֪ͨ
	* dwCookie:ע��ʱ���ص�ֵ
	*/
	STDMETHOD_(BOOL, UnadviseNotify)(DWORD dwCookie) PURE;

	STDMETHOD_(BOOL, WriteSpeedTimeStamp)() PURE;
};


