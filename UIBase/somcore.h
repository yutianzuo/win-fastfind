#pragma once

#define CODE_PAGE_IDENTIFIERS_GBK	936

// 软件管家全局信息，如主程序和安全卫士安装目录，机器 mid
class SOM_NO_VTABLE ISomAppInfo : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_SOMAPPINFO };

	STDMETHOD (Get360Path)(ISomString** szPath) PURE;			// 根据App Paths 获得安全卫士的安装路径
	STDMETHOD (GetSomPath)(ISomString** szPath) PURE;			// 获得软件管家的安装路径
	STDMETHOD (GetComputerID)(ISomString** szMID) PURE;			// 获得计算机的硬件唯一 ID
	STDMETHOD (GetUIDString)(ISomString** szUIDString) PURE;	// 获得 UID 字符串
	STDMETHOD_(DWORD, GetUID)() PURE;							// 获得 UID
	STDMETHOD (GetOEMURL)(ISomString** szOEMURL) PURE;			// 获得 OEM URL
	STDMETHOD (GetOEMTitle)(ISomString** szOEMTitle) PURE;		// 获得 OEM TITLE
	STDMETHOD (GetPID)(ISomString** szPID) PURE;				// 获得计算机的OEMID

	STDMETHOD_(BOOL, GetDebugPrivilege)() PURE;					// 打开调试权限
	STDMETHOD_(BOOL, HaveInternetConnection)() PURE;			// 判断是否网络连接是通的
};

// http 下载 (包括 get, post)
class SOM_NO_VTABLE IHttpDownload : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_HTTPDOWNLOAD };

	// pBuf中的字符串是ansi格式的 如果pEvent为NULL 则不读取返回信息
	STDMETHOD_(BOOL, DownloadFileByPost)(LPCWSTR lpURL, void* pBuf, DWORD dwSize, LPCWSTR lpFileName, ISomAsyncResult* pEvent) PURE;
	STDMETHOD_(BOOL, DownloadFileByGet)(LPCWSTR lpURL, LPCWSTR lpFileName, ISomAsyncResult* pEvent) PURE;

	// pBuf中的字符串是ansi格式的 如果成功 
	// 会将结果保存在ISomAsyncResult结构中ISomBinaryBuffer里
	// 如果pEvent为NULL 则不读取返回信息
	STDMETHOD_(BOOL, DownloadBufByPost)(LPCWSTR lpURL, void* pBuf, DWORD dwSize, ISomAsyncResult* pEvent) PURE;
	STDMETHOD_(BOOL, DownloadBufByGet)(LPCWSTR lpURL, ISomAsyncResult* pEvent) PURE;
};

// pdown 封装
// sunhai //////////////////////////////////////////////////////////////////////////////////////

enum DOWNLOAD_STATE
{
	download_state_undownload			= 0,			// 根据GetInRecycleFlag为TRUE，表示在垃圾箱中，否则在已下载中。
	download_state_downloading			= 3,			// 正在下载
	download_state_downloadpause		= 4,			// 正在下载
	download_state_waitingfordownload	= 6,			// 正在下载
	download_state_installing			= 7,			// 已下载管理
	download_state_waitingforinstall	= 8,			// 已下载管理
	download_state_waitingforinstall_x	= 9,			// 已下载管理
	download_state_downloadprepare		= 10,			// 正在下载

	download_state_downloadSuccess		= 11,			// 2010-03-26  下载成功
														// 管炜评审通过

	download_state_allinrecycle			= MAXWORD - 12,	// 枚举回收站
	download_state_alldownloaded		= MAXWORD - 11,	// 枚举已下载
	download_state_alldownloading		= MAXWORD - 10,	// 枚举正下载
};

enum DOWNLOAD_NOTIFY_ACTION
{
	download_notify_statuschanged		= 1,			// 软件下载状态改变
	download_notify_actionchanged		= 2,			// 软件行为分类改变，正在运行《-》已下载《-》回收站
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

// 2010-03-26 定义有软件下载时的退出类型
// 管炜评审通过
// DWORD 类型 有下载任务时 怎样处理 
#define  DOWNLOAD_EXIT_MODE			L"Options\\ExitMode"
#define  DOWNLOAD_EXIT_MODE_UNKNOWN			0	
#define  DOWNLOAD_EXIT_MODE_INDIRECTEXIT	1	// 直接退出
#define  DOWNLOAD_EXIT_MODE_TRAY			2	// 在托盘后台下载

// BOOL类型，TRUE为默认值，TRUE软件下载后立即安装安装，否则手动安装
#define  DOWNLOAD_INSTALLAFTERDOWNLOADED			L"Options\\DownloadInstall"

// CLEARTEMP_CONDITION类型，cleartemp_per_week为默认值，表示删除下载文件的时机
#define  DOWNLOAD_CLEARTEMPFILECONDITION			L"Options\\DelOldFilePerWeek"
enum CLEARTEMP_CONDITION
{
	cleartemp_nodelete = 0,
	cleartemp_per_week,
	cleartemp_afterdl,
};

// BOOL类型，TRUE为默认值，TRUE表示遇到有插件等情况提示，否则不提示
#define  DOWNLOAD_TIPMESSAGEBEFOREDOWNLOAD			L"Options\\TipBeforeDownload"

// 字符串类型，表示下载目录
#define  DOWNLOAD_DOWNLOADFOLDER					L"Options\\DownloadFolder"
#define  DOWNLOAD_DEFAULTDOWNLOADFOLDER				L"Options\\DefaultDownloadFolder"

class SOM_NO_VTABLE IDownloadData : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_DOWNLOADDATA };

	// 获得 DWORD
	STDMETHOD_(DWORD, GetValueAsDWORD)(LPCWSTR lpwszKey) PURE;
	// 获得 LPWSTR
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

	// 用户确认用消息， WPARAM为DOWNLOAD_CONFIRM_TYPE的 download_confirm_plugin类型， LPARAM为软件ID， GetSupportSystemFlag、GetPluginName和GetHintTipText结果传递给网页
	// 总下载更新消息， WPARAM为DOWNLOAD_CONFIRM_TYPE的 download_status_alldown类型， LPARAM为0，通过QueryAllDownloadStatus接口获取总下载信息
	STDMETHOD_(BOOL, Init)(HWND hMainWnd, DWORD dwMsgRet) PURE;
	STDMETHOD_(BOOL, Uninit)() PURE;

	// 通知用消息， WPARAM为 MAKEPARAM(DOWNLOAD_NOTIFY_ACTION, DOWNLOAD_STATE) 类型标识软件当前状态， LPARAM为软件ID
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

	// *pdwDownloadState只有以下3种状态：
	// download_state_undownload为无下载，显示已下载软件个数，以及“查看详情”和“下载设置”按钮，*pdwTaskCount标识已下载任务数
	// download_state_downloading为正在下载，显示下载进度，以及“查看详情”，*pdwTaskCount标识正在下载任务数
	// download_state_downloadprepare为准备下载，播放动画，以及“查看详情”，*pdwTaskCount标识正在下载任务数
	STDMETHOD_(BOOL, QueryAllDownloadStatus)(ISomString** lppStrDownloadStatus = NULL, DOWNLOAD_STATE* pdwDownloadState = NULL, DWORD* pdwDownloadRate = NULL, double* pdbDownloadProgress = NULL, DWORD* pdwTaskCount = NULL) PURE;

	STDMETHOD_(LPCWSTR, FormatErrorText)(DOWNLOAD_ERROR_CODE decError) PURE;

	STDMETHOD_(DWORD, QuerySoftSizeByState)(DOWNLOAD_STATE dsState) PURE;

	STDMETHOD_(BOOL, DownloadSoftUrls)(ISomDWORDList* lpSoftIDList) PURE;
};

class SOM_NO_VTABLE IDownloadMgr2 : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_DOWNLOADMGR2 };

	// 用户确认用消息， WPARAM为DOWNLOAD_CONFIRM_TYPE的 download_confirm_plugin类型， LPARAM为软件ID， GetSupportSystemFlag、GetPluginName和GetHintTipText结果传递给网页
	// 总下载更新消息， WPARAM为DOWNLOAD_CONFIRM_TYPE的 download_status_alldown类型， LPARAM为0，通过QueryAllDownloadStatus接口获取总下载信息
	STDMETHOD_(BOOL, Init)(HWND hMainWnd, DWORD dwMsgRet) PURE;
	STDMETHOD_(BOOL, Uninit)() PURE;

	// 通知用消息， WPARAM为 MAKEPARAM(DOWNLOAD_NOTIFY_ACTION, DOWNLOAD_STATE) 类型标识软件当前状态， LPARAM为软件ID
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

	// *pdwDownloadState只有以下3种状态：
	// download_state_undownload为无下载，显示已下载软件个数，以及“查看详情”和“下载设置”按钮，*pdwTaskCount标识已下载任务数
	// download_state_downloading为正在下载，显示下载进度，以及“查看详情”，*pdwTaskCount标识正在下载任务数
	// download_state_downloadprepare为准备下载，播放动画，以及“查看详情”，*pdwTaskCount标识正在下载任务数
	STDMETHOD_(BOOL, QueryAllDownloadStatus)(ISomString** lppStrDownloadStatus = NULL, DOWNLOAD_STATE* pdwDownloadState = NULL, DWORD* pdwDownloadRate = NULL, double* pdbDownloadProgress = NULL, DWORD* pdwTaskCount = NULL) PURE;

	STDMETHOD_(LPCWSTR, FormatErrorText)(DOWNLOAD_ERROR_CODE decError) PURE;

	STDMETHOD_(DWORD, QuerySoftSizeByState)(DOWNLOAD_STATE dsState) PURE;

	STDMETHOD_(BOOL, DownloadSoftUrls)(ISomDWORDList* lpSoftIDList) PURE;

	STDMETHOD_(BOOL, InsertSoftUrlInfo)(DWORD dwSoftID, LPCWSTR lpwszSoftName, LPCWSTR lpwszUrl, LPCWSTR lpwszFileName, LPCWSTR lpwszInstallParam, LPCWSTR lpwszHint2, LPCWSTR lpwszPluginName, LPCWSTR lpwszSystem, LPCWSTR lpwszVersion, LPCWSTR lpwszFileSize) PURE;
};

// 2010-03-26 为支持增量包下载 定义新的下载接口
// 管炜评审通过
#define DOWNLOAD_MGR_TYPE_DOWNLOAD		0		// 下载软件安装包
#define DOWNLOAD_MGR_TYPE_UPDATE		1		// 下载软件升级包(如果该软件没有升级包 则下安装包)

class SOM_NO_VTABLE IDownloadMgr3 : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_DOWNLOADMGR3 };

	// 用户确认用消息， WPARAM为DOWNLOAD_CONFIRM_TYPE的 download_confirm_plugin类型， LPARAM为软件ID， GetSupportSystemFlag、GetPluginName和GetHintTipText结果传递给网页
	// 总下载更新消息， WPARAM为DOWNLOAD_CONFIRM_TYPE的 download_status_alldown类型， LPARAM为0，通过QueryAllDownloadStatus接口获取总下载信息
	STDMETHOD_(BOOL, Init)(HWND hMainWnd, DWORD dwMsgRet) PURE;
	STDMETHOD_(BOOL, Uninit)() PURE;

	// 通知用消息， WPARAM为 MAKEPARAM(DOWNLOAD_NOTIFY_ACTION, DOWNLOAD_STATE) 类型标识软件当前状态， LPARAM为软件ID
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

	// *pdwDownloadState只有以下3种状态：
	// download_state_undownload为无下载，显示已下载软件个数，以及“查看详情”和“下载设置”按钮，*pdwTaskCount标识已下载任务数
	// download_state_downloading为正在下载，显示下载进度，以及“查看详情”，*pdwTaskCount标识正在下载任务数
	// download_state_downloadprepare为准备下载，播放动画，以及“查看详情”，*pdwTaskCount标识正在下载任务数
	STDMETHOD_(BOOL, QueryAllDownloadStatus)(ISomString** lppStrDownloadStatus = NULL, DOWNLOAD_STATE* pdwDownloadState = NULL, DWORD* pdwDownloadRate = NULL, double* pdbDownloadProgress = NULL, DWORD* pdwTaskCount = NULL) PURE;

	STDMETHOD_(LPCWSTR, FormatErrorText)(DOWNLOAD_ERROR_CODE decError) PURE;

	STDMETHOD_(DWORD, QuerySoftSizeByState)(DOWNLOAD_STATE dsState) PURE;

	STDMETHOD_(BOOL, DownloadSoftUrls)(ISomDWORDList* lpSoftIDList) PURE;

	STDMETHOD_(BOOL, InsertSoftUrlInfo)(DWORD dwSoftID, LPCWSTR lpwszSoftName, LPCWSTR lpwszUrl, LPCWSTR lpwszUpUrl, LPCWSTR lpwszFileName, LPCWSTR lpwszInstallParam, LPCWSTR lpwszHint2, LPCWSTR lpwszPluginName, LPCWSTR lpwszSystem, LPCWSTR lpwszVersion, LPCWSTR lpwszFileSize) PURE;
};


// sunhai //////////////////////////////////////////////////////////////////////////////////////


// 大百科数据，星星信息
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

// 大百科数据，一句话简介信息
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

// 大百科数据
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


// 白名单信息 获得从白名单中查询得到的信息
class SOM_NO_VTABLE IWhiteListInfo : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_WHITELIST_INFO };

	// 获得安全级别
	STDMETHOD_(DWORD, GetSecurityLever)() PURE;
	// 获得索引 是为方便外面检索设置的 如果调用则没有设置 则该值无意义
	STDMETHOD_(DWORD, GetKey)() PURE;

	STDMETHOD_(BOOL, GetFileName)(ISomString** pPath) PURE;
	STDMETHOD_(BOOL, GetDescription)(ISomString** pDesp) PURE;
};

// 白名单
class SOM_NO_VTABLE IWhiteList : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_WHITELIST };

	// 从网上查询信息 异步调用函数
	// pPathList: 文件路径列表
	// pKeys:  表示每一项的key值数组
	// nKeyCnt: key值数组大小
	// pEvent: 回调事件 调用函数返回后 该事件会得到通知 并且返回一个白名单信息列表
	STDMETHOD_(BOOL, QueryFilesInfo)(ISomList* pPathList, int* pKeys, DWORD nKeyCnt, ISomAsyncResult* pEvent) PURE;
	
	// 从缓存文件中查询信息 同步调用函数
	// pszPath: 文件路径
	// ppInfo:  如果成功创建一个白名单信息对象 并返回
	STDMETHOD_(BOOL, QuickQueryFilesInfo)(LPCWSTR pszPath, IWhiteListInfo** ppInfo) PURE;
};

// 图标管理
class SOM_NO_VTABLE IFileIconMgr : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_FileIconMgr };

	STDMETHOD_(HICON, QueryFileIcon)(LPCWSTR lpIconPath) PURE;
};

// 配置改变通知回调函数 
// pParam:		为注册时传入的参数
// pszKeyPath:	改变的Key
typedef void (*INI_NOTIFY_FUNC)(LPVOID pParam, LPCWSTR pszKeyPath);

// 配置文件
class SOM_NO_VTABLE ISomIni : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_SOMINI };

	STDMETHOD_(BOOL, GetValueDWORD)(LPCWSTR pszPathKey, DWORD* pdwValue) PURE;
	STDMETHOD_(BOOL, SetValueDWORD)(LPCWSTR pszPathKey, DWORD dwValue) PURE;
	STDMETHOD_(BOOL, GetValueString)(LPCWSTR pszPathKey, ISomString** pStrValue) PURE; 
	STDMETHOD_(BOOL, SetValueString)(LPCWSTR pszPathKey, LPCWSTR pszVal) PURE;

	// 将对INI文件的修改 写到文件中
	STDMETHOD_(BOOL, Flush)() PURE;

	/*
	 * 注册key值改变通知函数
	 * pszPathKey: 其值改变需要通知的key
	 * pFunc: 回调函数
	 * pParam: 回调时传给回调函数的参数
	 * pDwCookie: 返回一个cookie 在注销该通知时会用到
	 */
	STDMETHOD_(BOOL, AdviseNotify)(LPCWSTR pszPathKey, INI_NOTIFY_FUNC pFunc, LPVOID pParam, DWORD* pDwCookie) PURE;
	/*
	 * 注销一个key值改变通知
	 * dwCookie:注册时返回的值
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

// 软件升级信息
#define  SOFTUPDATE_ASSIST_SHOWPERIOD			L"ShowUptMsg\\ShowPeriod"

enum SOFTUPDATE_NOTIFY_FLAG
{
	// 填充基本数据完成
	softupdate_notify_detected		= 0x00000001,
	// 填充网络数据完成
	softupdate_notify_fillinfo		= 0x00000004,
	// 开始播放动画
	softupdate_notify_beginupdate	= 0x00000008,
	// 结束播放动画
	softupdate_notify_endupdate		= 0x00000010,
	// 更新显示升级数字
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

	// 获得 DWORD
	STDMETHOD_(DWORD, GetValueAsDWORD)(LPCWSTR lpwszKey) PURE;
	// 获得 LPWSTR
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

	// 注册软件升级通知软件窗口和消息信息
	STDMETHOD_(BOOL, AdviseNotifyWnd)(HWND hWnd, DWORD dwMsgRet) PURE;
	STDMETHOD_(BOOL, UnadviseNotifyWnd)(HWND hWnd) PURE;

	// 同步获取升级软件个数（不包含已忽略的升级软件）
	STDMETHOD_(DWORD, GetNeedUpdateSoftCount)() PURE;

	// 异步检测软件升级信息（包括已忽略的升级软件），bQueryOnline参数为TRUE标识从网络获取数据，FALSE则只有本地基本信息，
	// 通过AdviseNotifyWnd注册的消息通知，WPARAM为SOFTUPDATE_NOTIFY_FLAG标识通知完成的是基本信息还是网络信息，LPARAM为DWORD标识升级软件的数量
	// 通过AdviseNotifyWnd注册的消息通知，WPARAM为softupdate_notify_beginupdate标识开始动画，LPARAM为0
	// 通过AdviseNotifyWnd注册的消息通知，WPARAM为softupdate_notify_endupdate标识结束动画，LPARAM为0
	// 通过AdviseNotifyWnd注册的消息通知，WPARAM为softupdate_notify_updatenum标识更新升级数量，LPARAM为DWORD标识升级软件的数量
	STDMETHOD_(BOOL, DetectAllSoft)(SOFTUPDATE_DETECT_FLAG suDetectFlag) PURE;

	// 获取软件列表，根据SOFTUPDATE_QUERYLIST_FLAG返回提示升级和忽略升级的软件ID列表
	STDMETHOD_(BOOL, QueryNeedUpdateSoftList)(ISomBinaryBuffer** lppSoftIDArray, BOOL bIgnoreList = FALSE) PURE;
	// 获取软件升级信息
	STDMETHOD_(BOOL, QueryItemData)(DWORD dwSoftID, ISoftUpdateData** pISoftUpdateData) PURE;

	STDMETHOD_(BOOL, GetErrorString)(ISomString** ppStrErrorSoftVern) PURE;
	STDMETHOD_(BOOL, SetIgnoreSoft)(DWORD dwSoftID, BOOL bIgnoreIt) PURE;

	// 检测软件安装信息，不需要的参数可以传NULL
	STDMETHOD_(BOOL, CheckSoftExist)(DWORD dwSoftID, BOOL bReCheck = TRUE, BOOL* pbNeedUpdate = NULL, LPPOINT lpOldVersion = NULL, ISomString** pStrDispVersion = NULL, ISomString** pStrExecPath = NULL, ISomString** pStrIconPath = NULL) PURE;

	// 获取软件列表，根据返回已经安装的软件ID列表,列表的格式为DWORD组（ID，STATE，ID，STATE，……以此类推）,STATE是0未安装，1已安装，2需升级，可直接传入页面。
	STDMETHOD_(BOOL, QueryInstalledSoftList)(ISomBinaryBuffer** lppSoftIDArray) PURE;

	STDMETHOD_(BOOL, TransOldIgnoredList)() PURE;
};

// 检测软件安装信息
class SOM_NO_VTABLE ISoftInstalledInfo : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_SOFTINSTALLEDINFO };
};

// 统计信息

// 主程序统计URL %s表示MID
#define STATISTICS_URL_MAIN L"http://softm.360safe.com/stat/?type=open&action=mainapp&"		// 装机必备

// 功能页面统计URL %s表示MID
#define STATISTICS_URL_ESS L"http://softm.360safe.com/stat/?type=open&action=page1&"		// 装机必备
#define STATISTICS_URL_BAO L"http://softm.360safe.com/stat/?type=open&action=page8&"		// 软件宝库
#define STATISTICS_URL_UPT L"http://softm.360safe.com/stat/?type=open&action=page7&"		// 软件升级
#define STATISTICS_URL_UNI L"http://softm.360safe.com/stat/?type=open&action=page4&"		// 软件卸载
#define STATISTICS_URL_MBL L"http://softm.360safe.com/stat/?type=open&action=page11&"		// 手机必备
#define STATISTICS_URL_AUT L"http://softm.360safe.com/stat/?type=open&action=page9&"		// 开机加速
#define STATISTICS_URL_RUN L"http://softm.360safe.com/stat/?type=open&action=page3&"		// 正在运行
#define STATISTICS_URL_DEF L"http://softm.360safe.com/stat/?type=open&action=page10&"		// 设置默认
#define STATISTICS_URL_POP L"http://softm.360safe.com/stat/?type=open&action=page6&"		// 热门游戏
#define STATISTICS_URL_GJG L"http://softm.360safe.com/stat/?type=open&action=page12&"		// 高级工具

// 装机必备、软件宝库、开机加速的分类统计URL %s第一个表示分类名称，第二个表示MID
#define STATISTICS_URL_CAT L"http://softm.360safe.com/stat/?type=open&action=%s&"

// 设置统计URL %d依次为下载提示设置0(不提示）/1（提示）、软件安装设置0（手动安装）/1（立即安装）、自动删除设置0（不删除）/1（一周后删除）/2（安装后删除）、升级助手频率0（不提示）/1（每天一次）/7（每周一次）、开机助手开关0（不弹出）/1（弹出）、卸载控制面板0（不显示）/1（显示）、卸载开始菜单0（不显示）/1（显示）、删入回收站设置0（永久删除）/1（删入回收站）
#define STATISTICS_URL_SET L"http://softm.360safe.com/stat/?type=setting&action=%d_%d_%d_%d_%d_%d_%d_%d&"

// 取消下载
#define STATISTICS_URL_CANCEL L"http://softm.360safe.com/stat/?type=canceldownload&softid=%d&"

// 一键优化
#define STATISTICS_URL_OPTIMIZATION L"http://softm.360safe.com/stat/?type=open&action=optimization&"

// 点击开机加速界面中的”关闭时间小助手“
#define STATISTICS_URL_STARTTIME_ASSISTANT L"http://softm.360safe.com/stat/?type=open&action=CloseStarttimeAssistant&"

// 软件升级小助手统计
#define STATISTICS_URL_SOFTUPDATE_ASSISTANT L"http://softm.360safe.com/stat/?type=SoftUptMsg&RunSoftMgr=%d&SelfExit=%d&NoLongerDisp=%d&"

// 开机时间小助手统计
#define STATISTICS_URL_SPEEDID_ASSISTANT L"http://softm.360safe.com/stat/?type=SpeedID&RunSoftMgr=%d&SelfExit=%d&NoLongerDisp=%d&"

// 软件升级中新版功能统计
#define STATISTICS_URL_NEW_FEATURE		L"http://softm.360safe.com/stat/?type=open&action=NewFeature&"

// 软件升级中不再提醒统计
#define STATISTICS_URL_NOLONGER_NOTIFY	L"http://softm.360safe.com/stat/?type=open&action=NolongerNotify&"

// 软件升级中升级全部软件统计
#define STATISTICS_URL_UPDATE_ALL		L"http://softm.360safe.com/stat/?type=open&action=UpdateAllSoftware&"

// 2010.03.09 详情页统计地址
// 2010.03.09 万立新评审通过
#define STATISTICS_URL_DETAIL			L"http://softm.360safe.com/stat/info.html?sid=%d&tab=%d&"

//2010.04.16 by ireton 软件升级完成统计，用来统计升级比例
#define	STATISTICS_URL_UPDATE_FINISH	L"http://softm.360safe.com/stat/?type=update&editionid=%s&"

// 设置启动项统计地址
#define	STATISTICS_URL_START_ITEM_OPR	L"http://softm.360safe.com/stat/?type=start&class=%d&sname=%s&operate=%d&IsOptmz=%d&"

class SOM_NO_VTABLE IStatisticsMgr : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_STATISTICSMGR };

	// 以Get方式发送数据 
	// pszURL : 发送的地址
	// pEvent: 事件对象
	// 调用该函数后会立即返回 等事件发送结束后 会通过pEvent接口 通知调用者 pEvent为null 则不用通知
	STDMETHOD_(BOOL, SendGetInfo)(PCWSTR pszURL, ISomAsyncResult* pEvent) PURE;

	// 以Post方式发送数据 
	// pszURL : 发送的地址
	// pBuf:	二进制数据地址
	// lLen:	二进制数据长度
	// pEvent: 事件对象
	// 调用该函数后会立即返回 等事件发送结束后 会通过pEvent接口 通知调用者 pEvent为null 则不用通知s
	STDMETHOD_(BOOL, SendPostInfo)(PCWSTR pszURL, BYTE* pBuf, long lLen, ISomAsyncResult* pEvent) PURE;

	STDMETHOD_(BOOL, SendGetInfoFormat)(LPCWSTR pszURL, ...) PURE;

	STDMETHOD_(BOOL, SendGetInfoOnlyAction)(LPCWSTR pszKey) PURE;
};


enum LIVEUPDATE_NOTIFY_TYPE
{
	liveupdate_notify_pos,
	liveupdate_notify_end,
};

// 库更新（包括通知）
class SOM_NO_VTABLE ILiveUpdate : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_LIVEUPDATE };

	STDMETHOD_(BOOL, Init)() PURE;
	STDMETHOD_(BOOL, Uninit)() PURE;

// 下载进度消息，WPARAM为LIVEUPDATE_NOTIFY_TYPE的liveupdate_notify_pos，LPARAM为下载进度（大于等于0，小于等于100）
// 下载进度消息，WPARAM为LIVEUPDATE_NOTIFY_TYPE的liveupdate_notify_end，LPARAM为更新是否成功（BOOL类型）
	STDMETHOD_(BOOL, AdviseNotifyWnd)(HWND hWnd, DWORD dwMsg) PURE;
	STDMETHOD_(BOOL, UnadviseNotifyWnd)(HWND hWnd) PURE;

	STDMETHOD_(BOOL, UpdateNow)() PURE;
	STDMETHOD_(BOOL, QueryUpdateResultByFileName)(LPCWSTR lpwszFileName) PURE;
};

//缓冲种类
enum ECacheCategory
{
	CACHE_CAT_RESOURCE = 0,			// 资源
	CACHE_CAT_XML,					// XML文件
	CACHE_CAT_BAIKE_ONEWORD,		// 百科一句话
	CACHE_CAT_BAIKE_STAR,			// 百科一星星评分信息
	CACHE_CAT_WHITE_LIST,			// 白名单信息
	CACHE_CAT_START_OPTMZ,			// 开机加速信息

	CACHE_CAT_SOFTDATA,				// 软件列表软件数据
	CACHE_CAT_PAGELIST,				// 软件列表页面数据
	CACHE_CAT_CATALOGDATA,			// 软件列表类别数据
	CACHE_CAT_CATALOGLIST,			// 软件类别列表数据

	CACHE_CAT_DOWNLOADHISTORY,		// 下载历史记录

	CACHE_CAT_SOFTUPDATEIGNORELIST,	// 软件升级忽略列表

	CACHE_CAT_UNINST_SOFT_CATEGORY,	// 软件卸载分类

	CACHE_CAT_NET_IMAGE,			// 网络图片分类
	CACHE_CAT_TASK,					// 任务计划信息
};

// 缓存管理 该接口支持多线程安全
class SOM_NO_VTABLE ICache : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_CACHE };

	/*
	 * 往缓存中 写 读一段二进制流 DWORD为key
	 * dwCategory: 种类
	 * dwKey key
	 */
	STDMETHOD(WriteBufDWORD)(DWORD dwCategory, DWORD dwKey, void* pBuf, DWORD dwBytesLen) PURE;
	STDMETHOD(ReadBufDWORD)(DWORD dwCategory, DWORD dwKey, ISomBinaryBuffer* pBryBuf) PURE;

	/*
	* 往缓存中 写 读一段二进制流 字符串位key
	* dwCategory: 种类
	* pKey: key
	*/
	STDMETHOD(WriteBufSTR)(DWORD dwCategory, LPCWSTR pKey, void* pBuf, DWORD dwBytesLen) PURE;
	STDMETHOD(ReadBufSTR)(DWORD dwCategory, LPCWSTR pKey, ISomBinaryBuffer* pBryBuf) PURE;

	// 按种类清空缓存 不支持清空所有种类的缓存 
	// 清空后该种类缓存 可以换存贮类型 如以前是string可以改为dword
	STDMETHOD(ClearCache)(DWORD dwCategory) PURE;
};

class ICache2 : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_CACHE2 };

	/*
	* 往缓存中 写 读一段二进制流 DWORD为key
	* dwCategory: 种类
	* dwKey key
	* bCompress : 是否需要压缩
	* dwParam : 在缓存中多存一个参数
	*/
	STDMETHOD(WriteBufDWORD)(DWORD dwCategory, DWORD dwKey, void* pBuf, DWORD dwBytesLen, BOOL bCompress, DWORD dwParam) PURE;
	STDMETHOD(ReadBufDWORD)(DWORD dwCategory, DWORD dwKey, ISomBinaryBuffer* pBryBuf, DWORD* pdwParam) PURE;

	/*
	* 往缓存中 写 读一段二进制流 字符串位key
	* dwCategory: 种类
	* pKey: key
	*/
	STDMETHOD(WriteBufSTR)(DWORD dwCategory, LPCWSTR pKey, void* pBuf, DWORD dwBytesLen, BOOL bCompress, DWORD dwParam) PURE;
	STDMETHOD(ReadBufSTR)(DWORD dwCategory, LPCWSTR pKey, ISomBinaryBuffer* pBryBuf, DWORD* pdwParam) PURE;

	// 按种类清空缓存 不支持清空所有种类的缓存 
	// 清空后该种类缓存 可以换存贮类型 如以前是string可以改为dword
	STDMETHOD(ClearCache)(DWORD dwCategory) PURE;

	// 关闭缓存 
	STDMETHOD(CloseCache)() PURE;
};

const int IMG_MGR_ERROR_PATH_NOT_EXIST = 1;		// 路径不存在错误
class SOM_NO_VTABLE IImageMgr : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_IMAGE_MGR };

	// 异步函数 加载图片 会自动根据pszPath来区分是下载网络图片还是加载本地图片
	// 调用后立刻返回 图片下载完后 会设置pAsynRet 表示下载完成 如果pAsynRet为null 则不通知
	// 然后再调用该函数来取图片 这时设置pAsynRet位null
	STDMETHOD_(HBITMAP, LoadImage)(LPCWSTR pszPath, ISomAsyncResult* pAsynRet) PURE;

	// 同LoadImage一样 只不过是加载图标
	// 如果要设置取.exe文件中的第几个图标 则pszPath后加上","再加上所有  
	// bMaxIcon: 是否是大图标 TRUE : 32 * 32 FALSE: 16 * 16
	STDMETHOD_(HICON, LoadIcon)(LPCWSTR pszPath, BOOL bMaxIcon, ISomAsyncResult* pAsynRet) PURE;
};

class SOM_NO_VTABLE IImageMgr2 : public IImageMgr
{
public:
	enum { PLUGINID = SOMPLUGIN_IMAGE_MGR2};

	// 异步函数 加载图片 会自动根据pszPath来区分是下载网络图片还是加载本地图片
	// pdwErrCode表示错误的原因 0: 表示没有错误 1:表示路径无效
	// 调用后立刻返回 图片下载完后 会设置pAsynRet 表示下载完成 如果pAsynRet为null 则不通知
	// 然后再调用该函数来取图片 这时设置pAsynRet位null
	STDMETHOD_(HICON, LoadIcon2)(LPCWSTR pszPath, BOOL bMaxIcon, DWORD* pdwErrCode, ISomAsyncResult* pAsynRet) PURE;
};

class SOM_NO_VTABLE IFileShell : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_FILESHELL };

	STDMETHOD_(BOOL, LoadFile)(LPCWSTR lpwszPath, ISomString** lppStrXML) PURE;

};

// 升级设置改变通知回调函数 
// pParam:		为注册时传入的参数
// dwParam:		保留
typedef void (*SPEED_ASSIST_NOTIFY_FUNC)(LPVOID pParam, DWORD dwParam);

// 开机时间小助手对象
class SOM_NO_VTABLE ISpeedAssist : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_SPEED_ASSIST };

	// 更新开机时间
	STDMETHOD_(BOOL, UpdateSpeedTime)() PURE;

	// 获得本次的开机时间 0: 表示失败 非0:开机时间
	STDMETHOD_(DWORD, GetSpeedTime)() PURE;


	// 设置 获得是否显示开机小助手
	STDMETHOD_(BOOL, IsShowAssist)(BOOL* pbIsShow) PURE;
	STDMETHOD_(BOOL, SetShowAssist)(BOOL bShow) PURE;

	STDMETHOD_(BOOL, AdviseNotify)(SPEED_ASSIST_NOTIFY_FUNC pFunc, LPVOID pParam, DWORD* pDwCookie) PURE;
	/*
	* 注销一个key值改变通知
	* dwCookie:注册时返回的值
	*/
	STDMETHOD_(BOOL, UnadviseNotify)(DWORD dwCookie) PURE;
};

// 开机时间小助手对象
class SOM_NO_VTABLE ISpeedAssist2 : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_SPEED_ASSIST2 };

	// 更新开机时间
	STDMETHOD_(BOOL, UpdateSpeedTime)() PURE;

	// 获得本次的开机时间 0: 表示失败 非0:开机时间
	STDMETHOD_(DWORD, GetSpeedTime)() PURE;


	// 设置 获得是否显示开机小助手
	STDMETHOD_(BOOL, IsShowAssist)(BOOL* pbIsShow) PURE;
	STDMETHOD_(BOOL, SetShowAssist)(BOOL bShow) PURE;

	STDMETHOD_(BOOL, AdviseNotify)(SPEED_ASSIST_NOTIFY_FUNC pFunc, LPVOID pParam, DWORD* pDwCookie) PURE;
	/*
	* 注销一个key值改变通知
	* dwCookie:注册时返回的值
	*/
	STDMETHOD_(BOOL, UnadviseNotify)(DWORD dwCookie) PURE;

	STDMETHOD_(BOOL, WriteSpeedTimeStamp)() PURE;
};


