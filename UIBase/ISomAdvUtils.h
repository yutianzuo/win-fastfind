#pragma once

#include "plugin.h"
#include "kernl.h"

// 是否显示开机时间小助手的设置路径		类型为DWORD  0:表示不显示 非0表示显示
#define	SPEED_ASSIST_SHOW_KEY	L"SpeedAssist\\IsShow"

#define ALL_ITEM_KEY				-1	// 代表所有的Item 
#define RUN_SOFT_ALL_BIT			0xFFFFFFFF		// 所有信息的位掩码
#define RUN_SOFT_DESCRIPTION_BIT	0x00000001		// 描述信息的位掩码
#define RUN_SOFT_SECURITY_BIT		0x00000002		// 安全信息的位掩码

class IStUpInfoEx ;

// 通知消息类型
enum ENotifyMsgType
{
	eNotifyUnknown = 0,
	eNotifyAdd,			// 添加了项
	eNotifyDel,			// 删除了项
	eNotifyChange,		// 项的内容改变了
	eNotifyBaseLoad,	// 基本信息加载完成
	eNotifyDelayLoad,	// 延迟信息加载完成
	eNotifyBeginDloadCfg,	// 开始下载配置信息
	eNotifyEndDloadCfg,		// 结束下载配置信息
};

// 通知消息参数 
struct NOTIFY_MSG_PARAM
{	
	DWORD	dwKey;			// 项的key	 ALL_ITEM_KEY表示所有的都改变了 不支持若干个变
	LONG	lDelayLoadBit;	// 延迟加载属性的位掩码 每个属性一位
};

// 说名: 所有可优化的项叫 启动项(StartUp) 其包块 服务(Service) 任务计划(Task) run项(Run)
// 

// 启动项目 启动的类型
enum EAutoRunStartType
{
	eStartUnknown = -1,
	eStartDisabled = 0,		// 禁止启动
	eStartDelay,			// 延迟启动
	eStartAuto,				// 自动启动
	eStartDelete,			// 删除启动项 只有启动项和任务计划支持
	eStartIgnore,			// 体检时忽略某项
};

// run项的类型
enum ERunItemType
{
	eRunUnknown = -1,
	eRunAllUserStartUp = 0,
	eRunCurUserStartUp,
	eRunLMRun,
	eRunCURun,
	eRunLMRunOnce,
	eRunCURunOnce,
	eRunLMRun64,
	eRunLMRunOnce64,
};

// 启动项类型
enum EStartUpType	
{
	eAtRunUnKnown = 0,
	eAtRunMin,
	eAtRunService,		// 服务
	eAtRunTask,			// 任务计划
	eAtRunRun,			// 开始项
	eAtRunDefault,		// 默认项
	eAtRunHistroy,		// 禁用历史
	eAtRunRelation,		// 间接启动项
	eAtRunSystem,		// 系统项
	eAtRunDeepScan,     // 木马云查杀启动项
	eAtRunMax,			// 最大类型
	eAtRunOnlyBanned,   // 只获取我们自己建议禁止的信息
};

// 建议类型
enum EAdviceType
{
	eAdcAllowDisable = 0,	//允许禁止		
	eAdcAdviseRun,		//建议启动
	eAdcMustRun,		//必须启动
	eAdcAdviseDelay,	//建议延迟加载
	eAdcAdviseKeep,		//建议保存现在的设置
	eAdcUnknown = 99,
};

// 软件类型
enum ESoftType
{
	etAVSoft     = 5, // 杀毒软件
	etChatSoft   = 17, // 聊天软件
	etCommonSoft = 0, // 普通软件
};

// 操作系统信息
enum ESystemVersion
{
	evUnknown       = 0x00000000, 
	evWindowsALL    = 0x00000001,
	evWindows64     = 0x00000002,
	evWindows2000   = 0x00000004,
	evWindowsXP     = 0x00000008,
	evWindows2003   = 0x00000010,
	evWindowsVista  = 0x00000020,
	evWindows7      = 0x00000040,
};

// 软件支持类型
enum EDialogSupportType
{
	stNotWantDialog    = 0, // 不需要任何对话框
	stSetPicDialog     = 1, // 带图片的设置对话框
	setUninstallDialog = 2, // 不带图片的卸载对话框
};

interface SOM_NO_VTABLE ISomRunInfo: public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_RUN_INFO };

	STDMETHOD(GetFileName)(ISomString** psFileName) PURE;
	STDMETHOD(GetDisplayName)(ISomString** psDispName) PURE;
	STDMETHOD(GetFilePath)(ISomString** psFilePath) PURE;
	STDMETHOD(GetDescription)(ISomString** psDescription) PURE;
	STDMETHOD(GetCmdLine)(ISomString** psCmdLine) PURE;
	STDMETHOD(GetParameter)(ISomString** psParam) PURE;
	STDMETHOD_(EAutoRunStartType, GetStartType)() PURE;
	STDMETHOD_(ERunItemType, GetRunType)() PURE;
};

class SOM_NO_VTABLE ISomRun: public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_RUN_MGR };

	// 枚举系统的启动项 返回服务项信息列表
	STDMETHOD(EnumRuns)(ISomList* plstSvr) PURE;

	// 设置启动项类型 如: 是自动启动 还是禁止
	STDMETHOD(SetRunStartType)(LPCWSTR psName, ERunItemType eRunTypem, EAutoRunStartType eNewType, EAutoRunStartType eOldType) PURE;
};

class SOM_NO_VTABLE ISomServiceInfo: public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_SERIVER_INFO };

	// 得到 服务名称
	STDMETHOD(GetName)(ISomString** psName) PURE;

	// 得到 服务显示名称
	STDMETHOD(GetDispName)(ISomString** psDispName) PURE;

	// 得到 描述信息
	STDMETHOD(GetDescription)(ISomString** psDesp) PURE;

	// 得到 命令行参数
	STDMETHOD(GetCmdLine)(ISomString** psCmdLine) PURE;

	// 得到 路径
	STDMETHOD(GetFilePath)(ISomString** psCmdLine) PURE;

	// 得到 服务启动类型
	STDMETHOD_(EAutoRunStartType, GetStartType)() PURE;

	// 得到 设置是否可以延迟加载
	STDMETHOD_(BOOL, CanDelayRun)() PURE;
};

class SOM_NO_VTABLE ISomService: public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_SERIVER_MGR };

	// 枚举系统的所有服务 返回服务项信息列表
	STDMETHOD(EnumServices)(ISomList* plstSvr) PURE;

	// 设置服务启动类型 如: 是自动启动 还是禁止
	// eType:	服务的启动类型
	// bForbid:	该服务是否被禁止
	STDMETHOD(SetServiceStartType)(LPCWSTR pszName, EAutoRunStartType eType, BOOL bForbid) PURE;

	// 恢复到系统默认设置
	STDMETHOD(RestoreSystemDef)() PURE;
};

class SOM_NO_VTABLE ISomTaskInfo: public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_TASK_INFO };

	// 得到 名称
	STDMETHOD(GetName)(ISomString** psName) PURE;

	// 得到 显示名称
	STDMETHOD(GetDispName)(ISomString** psDispName) PURE;

	// 得到 描述信息
	STDMETHOD(GetDescription)(ISomString** psDesp) PURE;

	// 得到 程序名称
	STDMETHOD(GetAppName)(ISomString** psDesp) PURE;

	// 得到 程序名称
	STDMETHOD(GetFilePath)(ISomString** psDesp) PURE;

	// 得到 程序名称
	STDMETHOD(GetParameters)(ISomString** psDesp) PURE;

	// 得到 命令行参数
	STDMETHOD(GetCmdLine)(ISomString** psCmdLine) PURE;

	// 得到 设置启动类型
	STDMETHOD_(EAutoRunStartType, GetStartType)() PURE;
};

class SOM_NO_VTABLE ISomTask: public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_TASK_MGR };

	// 枚举系统的所有任务计划 返回任务计划信息列表
	STDMETHOD(EnumTask)(ISomList* plstTask) PURE;

	// 设置任务计划启动类型 目前支持: 是自动启动 还是禁止
	/*
	*	psTaskName: 任务计划名称
	*	eNewType:	准备设置的启动类型 
	*	eOldType:   现在设置的启动类型
	*/
	STDMETHOD(SetTaskStartType)(LPCWSTR pszName, EAutoRunStartType eNewType, EAutoRunStartType eOldType) PURE;
};

class SOM_NO_VTABLE IStUpInfo: public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_AUTO_RUN_ITEM };

	// 得到 名称
	STDMETHOD(GetName)(ISomString** psName) PURE;

	// 得到 显示名
	STDMETHOD(GetDispName)(ISomString** psName) PURE;

	// 得到 程序名称
	STDMETHOD(GetAppName)(ISomString** psAppName) PURE;

	// 得到 文件路径
	STDMETHOD(GetFilePath)(ISomString** psFilePath) PURE;

	// 得到 警告
	STDMETHOD(GetWarning)(ISomString** psWarning) PURE;

	// 得到 命令行
	STDMETHOD(GetCmdLine)(ISomString** psCmdLine) PURE;

	// 获得 启动项的类型 表示是计划任务，服务还是启动项
	STDMETHOD_(EStartUpType, GetItemType)() PURE;

	// 获得 Run项的类型 表示是在HKLM 还是 HKCU 或者在开始项菜单里
	STDMETHOD_(ERunItemType, GetRunItemType)() PURE;

	//  启动类型: 是启动 禁止还是延迟 
	STDMETHOD_(EAutoRunStartType, GetStartType)() PURE;

	// 得到 该项目被禁用的时间
	STDMETHOD(GetDisabledTime)(time_t* ptmDisabledTime) PURE;

	/// 获得需要禁止的项
	STDMETHOD(GetStartUpParent)(IStUpInfo** pInfo) PURE;

	/// 获得父的扩展Item
	STDMETHOD(GetStartUpParentEx)(IStUpInfoEx** pInfo) PURE;

	/// 获得需要禁止的子项
	STDMETHOD(GetStartUpChild)(ISomList ** spList) PURE;

	/// 判断是否为残留
	STDMETHOD_(BOOL, IsItemRemanet)() PURE;
};

class SOM_NO_VTABLE IStUpInfoEx: public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_AUTO_RUN_ITEM_EX };

	STDMETHOD(GetDescription)(ISomString** psDesp) PURE;

	// 获得 建议类型 例如: 是建议启动还是建议禁止
	STDMETHOD_(EAdviceType, GetAdviceType)() PURE;

	// 获得 软件类型 例如: 是建议启动还是建议禁止
	STDMETHOD_(ESoftType, GetSoftType)() PURE;

	// 获得对话框的类型
	STDMETHOD_(EDialogSupportType, GetDlgSupportType());

	// 获得该项的Rank值
	STDMETHOD_(LONG, GetItemRank());

	// 获取A-B相关的对话框文本信息
	STDMETHOD(GetRelationText)(ISomString** psInfo) PURE;

	// 获取A-B相关联的Key值数组
	STDMETHOD(GetRelationKeys)(ISomStringList** valueVec) PURE;

	// 获取设置其他软件的URL
	STDMETHOD(GetSettingPicUrl)(ISomString** psInfo) PURE;

	// 获取禁用某个启动项的提示信息
	STDMETHOD(GetForbidenTipInfo)(ISomString** psInfo) PURE;

	// 根据参数调用程序去设置
	STDMETHOD(InvokeItemProgram)() PURE;

	// 卸载该项
	STDMETHOD(UninstallItem)() PURE;

	// 获取详细信息的对话框
	STDMETHOD(GetItemPicUrl)(ISomString** psInfo) PURE;

	// 获取组名称
	STDMETHOD(GetGroupName)(ISomString** psInfo) PURE;

	// 获取组信息
	STDMETHOD(GetGroupInfo)(ISomString** psInfo) PURE;

	// 获取内存信息
	STDMETHOD(GetMemoryInfo)(ISomString** psInfo) PURE;

	// 是否是系统的启动项
	STDMETHOD_(BOOL, IsSystemItem)() PURE;

	// 是否需要锁定注册表
	STDMETHOD_(BOOL, IsLockReg)() PURE;

	// 获取组ID
	STDMETHOD_(LONG, GetGroupID)() PURE;

	// 获取规则ID
	STDMETHOD_(LONG, GetRuleID)() PURE;

	// 能否被一键优化
	STDMETHOD_(BOOL, CanAutoOptimize)() PURE;
	// 判断该项是否为一键优化的项
	STDMETHOD_(BOOL, IsAutoOptimize)() PURE;

	/* 该属性只针对服务项
	* disble时是否将其设置为禁止
	* 服务项禁止时是将其设置为手动启动
	* 但有些服务禁止时 是将其设置为禁止启动  
	*/
	STDMETHOD_(BOOL, IsForbidService)() PURE;

	// 得到 能否被延迟加载
	STDMETHOD_(BOOL, CanDelayRun)() PURE;

	// 得到警告信息
	STDMETHOD(GetAlertInfo)(ISomString** psAlterInfo) PURE;

	// 体检时忽略此项
	STDMETHOD(Ignore)() PURE;
	// 该项是否已经被用户忽略
	STDMETHOD_(BOOL, IsIgnored)() PURE;

};

#define START_OPTMZ_BASE_INFO_COMPLETE -2		// 基本信息添加完成
class SOM_NO_VTABLE IStartOptimize: public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_START_OPTIMIZE };

	// 注册一个查询正在运行软件的窗口 
	// hWnd 为NULL  不会收到通知消息
	// dwMsgID: 发往注册窗口的消息ID 
	STDMETHOD(AdviseNotifyWnd)(HWND hWnd, UINT dwMsgID) PURE;
	// 注销一个正在运行软件的 为注册时的窗口句柄
	STDMETHOD(UnadviseNotifyWnd)(HWND hWnd) PURE;

	// 如果注册成功 可立刻调用以下两个方法来得到一些可立即获得的信息
	// 正在运行软件数目 该数目会比真正的正在运行软件数目大
	STDMETHOD_(DWORD, GetCount)();
	// 获得具体一项 
	STDMETHOD(GetItem)(DWORD dwIndex, IStUpInfo** ppItem) PURE;

	// 通过Key获得具体一项 
	STDMETHOD(GetItemByKey)(LPCTSTR lpszKey, IStUpInfo** ppItem) PURE;

	// 通过Key获得具体一项的延迟信息
	STDMETHOD(GetItemExByKey)(LPCTSTR lpszKey, IStUpInfoEx** ppItem) PURE;

	// 该函数在收到延迟加载成功消息后才可以调用
	STDMETHOD(GetItemEx)(DWORD dwIndex, IStUpInfoEx** ppItemEx) PURE;

	// 设置服务项的启动类型
	STDMETHOD(SetItemStartType)(DWORD dwIndex, EAutoRunStartType eStartType) PURE;

	// 刷新 会重新加载所有的启动项
	STDMETHOD(Refresh)() PURE;

	// 对当前的所有Item的状态 保存一个快照
	STDMETHOD(SnapShot)() PURE;
	// 恢复所有的item到上次保存的快照的状态
	STDMETHOD(Restore)() PURE;

	// 对所有的启动项保存一个最初的快照 也就是没有用我们的软件进行任何优化前的设置
	STDMETHOD(OriginalSnapShot)() PURE;
	// 恢复到没有进行任何优化前的状态
	STDMETHOD(OriginalRestore)() PURE;

	// 恢复的系统默认设置
	STDMETHOD(RestoreSystemDef)() PURE;
};


class SOM_NO_VTABLE IStartOptimize2: public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_START_OPTIMIZE2 };

	// 注册一个查询正在运行软件的窗口 
	// hWnd 为NULL  不会收到通知消息
	// dwMsgID: 发往注册窗口的消息ID 
	STDMETHOD(AdviseNotifyWnd)(HWND hWnd, UINT dwMsgID) PURE;

	// 注销一个正在运行软件的 为注册时的窗口句柄
	STDMETHOD(UnadviseNotifyWnd)(HWND hWnd) PURE;

	// 执行在主线程中获取启动项信息，通过读取本地库，进行匹配
	STDMETHOD(GetCurrentComputerForbbidenCount)(UINT* puTotalStartUp, UINT* puForbiddenCount) PURE;

	// 同步加载信息不起线程
	STDMETHOD(SyncLoadInfo)() PURE;

	// 如果注册成功 可立刻调用以下两个方法来得到一些可立即获得的信息
	// 正在运行软件数目 该数目会比真正的正在运行软件数目大
	STDMETHOD_(DWORD, GetCount)();
	// 获得具体一项 
	STDMETHOD(GetItem)(DWORD dwIndex, IStUpInfo** ppItem) PURE;

	// 通过Key获得具体一项 
	STDMETHOD(GetItemByKey)(LPCTSTR lpszKey, IStUpInfo** ppItem) PURE;

	// 通过Key获得具体一项的延迟信息
	STDMETHOD(GetItemExByKey)(LPCTSTR lpszKey, IStUpInfoEx** ppItem) PURE;

	// 该函数在收到延迟加载成功消息后才可以调用
	STDMETHOD(GetItemEx)(DWORD dwIndex, IStUpInfoEx** ppItemEx) PURE;

	// 设置服务项的启动类型
	STDMETHOD(SetItemStartType)(DWORD dwIndex, EAutoRunStartType eStartType) PURE;

	// 刷新 会重新加载所有的启动项
	STDMETHOD(Refresh)() PURE;

	// 对当前的所有Item的状态 保存一个快照
	STDMETHOD(SnapShot)() PURE;
	// 恢复所有的item到上次保存的快照的状态
	STDMETHOD(Restore)() PURE;

	// 对所有的启动项保存一个最初的快照 也就是没有用我们的软件进行任何优化前的设置
	STDMETHOD(OriginalSnapShot)() PURE;
	// 恢复到没有进行任何优化前的状态
	STDMETHOD(OriginalRestore)() PURE;

	// 恢复的系统默认设置
	STDMETHOD(RestoreSystemDef)() PURE;

	// 恢复的系统默认设置
	STDMETHOD(SetFirstEnumItemType)(EStartUpType eType) PURE;

	// 停止正在进行的扫描或网络查询
	STDMETHOD(Stop)() PURE;
};

// 可以立刻得到的属性
class SOM_NO_VTABLE IRunSoftInfo: public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_RUN_SOFT_INFO };

	// 得到 设置进程名称
	STDMETHOD(GetName)(ISomString** psName) PURE;

	// 得到 设置可执行文件的绝对路径
	STDMETHOD(GetFilePath)(ISomString** psFilePath) PURE;

	// 得到 设置进程ID
	STDMETHOD_(DWORD, GetPID)() PURE;

	// 获得 设置可执行文件的版本
	STDMETHOD(GetFileVersion)(ISomString** psVers) PURE;

	// 获得 设置所属公司
	STDMETHOD(GetCompanyName)(ISomString** psCmpy) PURE;

	// 得到 设置进程占用的内存大小 单位字节
	STDMETHOD_(DWORD, GetMemorySize)() PURE;

	// 是否是系统的进程
	STDMETHOD_(BOOL, IsSystemProc)() PURE;
};

// 延迟返回的属性
class SOM_NO_VTABLE IRunSoftInfoEx: public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_RUN_SOFT_INFO_EX };

	// 得到 设置描述信息
	STDMETHOD(GetDescription)(ISomString** psDesp) PURE;


	// 得到 设置安全级别
	STDMETHOD(GetSecurityLevel)(ISomString** psLevel) PURE;
};

class SOM_NO_VTABLE IModuleInfo: public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_PROC_MODULE_INFO };

	// 得到 设置模块名称
	STDMETHOD(GetName)(ISomString** psName) PURE;

	// 得到 设置模块文件的绝对路径
	STDMETHOD(GetFilePath)(ISomString** psFilePath) PURE;

	// 得到 设置描述信息
	STDMETHOD(GetDescription)(ISomString** psDesp) PURE;

	// 获得 设置可执行文件的版本
	STDMETHOD(GetFileVersion)(ISomString** psVers) PURE;

	// 获得 设置所属公司名称
	STDMETHOD(GetCompanyName)(ISomString** psVers) PURE;
};

// 正在运行软件管理接口
class SOM_NO_VTABLE IRunSoftMgr: public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_RUN_SOFT_MGR };

	// 注册一个查询正在运行软件的窗口 
	// hWnd 为NULL  不会收到通知消息
	// dwMsgID: 发往注册窗口的消息ID 
	STDMETHOD(AdviseNotifyWnd)(HWND hWnd, UINT dwMsgID) PURE;
	// 注销一个正在运行软件的 为注册时的窗口句柄
	STDMETHOD(UnadviseNotifyWnd)(HWND hWnd) PURE;

	// 如果注册成功 可立刻调用以下两个方法来得到一些可立即获得的信息
	// 正在运行软件数目 该数目会比真正的正在运行软件数目大
	STDMETHOD_(DWORD, GetCount)();
	// 获得具体一项 
	STDMETHOD(GetItem)(DWORD dwIndex, IRunSoftInfo** ppItem) PURE;

	// 该函数在收到延迟加载成功消息后才可以调用
	STDMETHOD(GetItemEx)(DWORD dwIndex, IRunSoftInfoEx** ppItemEx) PURE;

	// 结束一个正在运行的程序 
	// dwIndex:	进程项的索引
	// pbRet:	是否成功结束
	STDMETHOD(TerminateTask)(DWORD dwIndex, BOOL* pbRet) PURE;

	// 刷新 会重新加载正在运行的软件项目
	STDMETHOD(Refresh)() PURE;

	// 查询加载到一个进程中的所有DLL信息
	// dwIndex:	进程项的索引
	// pSList为 ISomModuleInfo列表
	STDMETHOD(QueryProcModules)(DWORD dwIndex, ISomList* pSList) PURE;
};

// 关联类型
enum assoc_catalog
{
	assoc_catalog_browser	= 0,
	assoc_catalog_mail		= 1,
	assoc_catalog_video		= 2,
	assoc_catalog_music		= 3,
	assoc_catalog_picture	= 4,
	assoc_catalog_ime		= 5
};

class SOM_NO_VTABLE IAssociation : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_ASSOCIATION };

	STDMETHOD_(int, BeginRefreshSoftwareList)(LPCWSTR lpXmlPath) PURE;
	STDMETHOD_(int, GetSoftwareIdAll)(int index) PURE;
	STDMETHOD_(bool, SetSoftwareInstalled)(int index, int version1, int version2, LPCWSTR lpIcon = NULL) PURE;
	STDMETHOD_(bool, EndRefreshSoftwareList)() PURE;

	STDMETHOD_(int, GetSoftwareCount)(int catalog) PURE;
	STDMETHOD_(LPCWSTR, GetSoftwareName)(int catalog, int index) PURE;
	STDMETHOD_(int, GetSoftwareId)(int catalog, int index) PURE;
	STDMETHOD_(LPCWSTR, GetSoftwareIcon)(int catalog, int index) PURE;
	STDMETHOD_(LPCWSTR, GetSoftwareUninstallKey)(int catalog, int index) PURE;
	STDMETHOD_(LPCWSTR, GetSoftwareUninstallName)(int catalog, int index) PURE;

	STDMETHOD_(bool, ExecuteSoftware)(int catalog, int index) PURE;	
	STDMETHOD_(int, GetSoftwareExtCount)(int catalog, int index) PURE;
	STDMETHOD_(LPCWSTR, GetSoftwareExt)(int catalog, int index, int extindex) PURE;
	STDMETHOD_(bool, GetSoftwareExtAssoc)(int catalog, int index, int extindex) PURE;

	STDMETHOD_(int, GetDefaultSoftware)(int catalog) PURE;
	STDMETHOD_(bool, SetDefaultSoftware)(int catalog, int index) PURE;

	STDMETHOD (GetSoftwareDebugInfo)(int catalog, int index, ISomString** ppOutInfo) PURE;
};