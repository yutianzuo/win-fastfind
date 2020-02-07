/********************************************************************
	created:	2009/10/12
	created:	12:10:2009   18:10
	file base:	plugin
	file ext:	h
	author:		wanlixin
	
	purpose:	SOM 对象库的基础头文件，实现了 COM 最基本的概念，为了简单化和高效，目前
				仅支持引用计数，而不支持QueryInterface,把 COM 接口的 IID 和 COM 对象 CLSID 统一
				成一个，即一个对象只有一个接口，一个接口也只有一个对象实现，并且也不用 GUID 来表示
				而采用一个数字 ID，这样可以直接用 ID 来作为对象创建表的下标，比较高效
*********************************************************************/
#pragma once

//#ifdef _DEBUG
#define SOM_DUBUG_MODE			// 表示开启调试信息
//#endif

#define SOMPLUGIN_SIGNATURE								'somp'		// 有效性标记
#define SOMPLUGIN_INITIALIZE							0xffffffff	// 初始化插件引擎
#define SOMPLUGIN_UNINITIALIZE							0xfffffffe	// 卸载插件引擎
#define SOMPLUGIN_INCPLUGINCOUNT						0xfffffffd	// 记录创建了一个 plugin，这样退出的时候就能知道有没有没删除的 plugin
#define SOMPLUGIN_DECPLUGINCOUNT						0xfffffffc	// 记录删除了一个 plugin，这样退出的时候就能知道有没有没删除的 plugin
#define SOMPLUGIN_QUERYDEBUGGER							0xfffffffb	// 查询 ISomDebugger 接口

#define SOMPLUGIN_FLAG_SINGLETON						0x1			// 单体标志
#define SOMPLUGIN_FLAG_EXIT_NOTIFY						0x2			// 退出时需要给通知
/////////////////////////////////////////////////////////////////////////////////////////////////////
// 组件 ID 声明，ID 一定要保证不能重用，否则会造成和以前的不兼容，另外，ID 会用来作为数组下标，以便快速定位，所以不要太大，以免数组过大

#define SOMPLUGIN_MAX									0x400		// 最大组件 ID

#define SOMPLUGIN_PLUGIN								1			// IPlugin

#define SOMPLUGIN_SOMSTRING								3			// 字符串

#define SOMPLUGIN_SOMLIST								4			// 简单链表

#define SOMPLUGIN_STRINGLIST							5			// 字符串 链表

#define SOMPLUGIN_DWORDLIST								6			// DWORD 链表

#define SOMPLUGIN_SOMBINARYBUFFER						7			// 二进制数据块

#define SOMPLUGIN_SOMASYNCRESULT						8			// 异步操作的通讯接口

#define SOMPLUGIN_WINVERSION							10			// windows 版本信息

#define SOMPLUGIN_SOMHASH								11			// Hash计算

#define SOMPLUGIN_SOMKERNEL								20			// 基本函数

#define SOMPLUGIN_PATHPARSER							21			// 路径分析，包括命令行解析

#define SOMPLUGIN_SOMREGISTRY							22			// 注册表访问

#define SOMPLUGIN_SOMFILE								23			// 文件访问

#define SOMPLUGIN_URLPARSER								24			// URL 的分析

#define SOMPLUGIN_ZLIB									25			// zlib 的封装

#define SOMPLUGIN_LIBPNG								26			// libpng 的封装

#define SOMPLUGIN_JPEG									27			// jpeg 的封装

#define SOMPLUGIN_URLMONIKER							28			// UrlMoniker 的封装

#define SOMPLUGIN_SOMAPPINFO							40			// 软件管家全局信息，如主程序和安全卫士安装目录，机器 mid

#define SOMPLUGIN_HTTPDOWNLOAD							41			// http 下载 (包括 get, post)

#define SOMPLUGIN_DOWNLOADMGR							42			// pdown 封装

#define SOMPLUGIN_BAIKE									43			// 大百科数据

#define SOMPLUGIN_WHITELIST								44			// 白名单

#define SOMPLUGIN_FileIconMgr							45			// 图标管理

#define SOMPLUGIN_SOMINI								47			// 配置文件

#define SOMPLUGIN_SOMVARIABLE							48			// SomVaribale

#define SOMPLUGIN_SOFTUPDATE							49			// 软件升级信息

#define SOMPLUGIN_SOFTINSTALLEDINFO						50			// 检测软件安装信息

#define SOMPLUGIN_STATISTICSMGR							51			// 统计信息

#define SOMPLUGIN_LIVEUPDATE							52			// 库更新（包括通知）

#define SOMPLUGIN_WHITELIST_INFO						53			//  获取的白名单信息

#define SOMPLUGIN_RUN_SOFT_MGR							54			// 正在运行软件管理对象

#define SOMPLUGIN_RUN_SOFT_INFO							55			// 正在运行软件信息对象

#define SOMPLUGIN_PROC_MODULE_INFO						56			// 进程包括的模块信息

#define SOMPLUGIN_START_OPTIMIZE						57			// 开机加速对象

#define SOMPLUGIN_AUTO_RUN_ITEM							58			// 开机运行项目对象

#define SOMPLUGIN_DOWNLOADDATA							59			// 下载任务状态信息

#define SOMPLUGIN_BAIKE_STAR							60			// 大百科数据,星星信息

#define SOMPLUGIN_BAIKE_ONEWORD							61			// 大百科数据,一句话简介

#define SOMPLUGIN_UNINST_ITEM_INFO						62			// 卸载元素本地结构信息

#define SOMPLUGIN_UNINST_ITEM_BAIKE_INFO				63			// 卸载元素网上百科结构信息

#define SOMPLUGIN_UNINST_SOFT_MGR						64			// 卸载软件管理对象

#define SOMPLUGIN_RUN_SOFT_INFO_EX						65			// 正在运行软件延迟加载的信息对象

#define SOMPLUGIN_SOFTUPDATEDATA						66			// 升级软件信息

#define SOMPLUGIN_UNINST_SOFT_REGINFO					67			// 搜索注册表信息

#define SOMPLUGIN_SOFTLISTDATA							68			// 软件列表信息

#define SOMPLUGIN_SOFTCATALOGDATA						69			// 软件列表分类信息

#define SOMPLUGIN_SOFTLISTMGR							70			// 软件列表管理对象

#define SOMPLUGIN_RUN_INFO								71			// 启动项信息对象

#define SOMPLUGIN_SERIVER_MGR							72			// 服务项管理对象

#define SOMPLUGIN_AUTO_RUN_ITEM_EX						73			// 开机运行项目延迟加载的信息对象

#define SOMPLUGIN_CACHE									74			// 缓存对象

#define SOMPLUGIN_SERIVER_INFO							75			// 服务项信息

#define SOMPLUGIN_TASK_MGR								76			// 计划任务管理对象

#define SOMPLUGIN_TASK_INFO								77			// 计划任务信息对象

#define SOMPLUGIN_RUN_MGR								78			// 启动项管理对象

#define SOMPLUGIN_UNINST_SOFT_DELTIEM_INFO              79

#define SOMPLUGIN_SQLITE								80			// Sqlite 数据库对象

#define SOMPLUGIN_WOW64									81			// 32位程序运行在64位系统上工具函数

#define SOMPLUGIN_SIMPLE_LOG							82			// 简单log文件

#define SOMPLUGIN_SOM_UNINSTSOFT_ITEM					83			// 软件卸载

#define SOMPLUGIN_IMAGE_MGR								85			// 图片管理对象

#define SOMPLUGIN_FILESHELL								86			// XML库文件壳

#define SOMPLUGIN_ASSOCIATION							87			// 文件关联，默认输入法模块

#define SOMPLUGIN_SETUPUTIL								88			// 软件管家安装卸载时的处理模块

#define SOMPLUGIN_DAEMON								89			// 客户端运行状态统计模块，会在客户端开始运行，退出，切换到某个 tab 页等运行事件时被调用

#define SOMPLUGIN_PINTOSTARMENU                         90          //开始菜单

#define SOMPLUGIN_SETUPJOB                              91          //安装卸载杂事

#define SOMPLUGIN_SPEED_ASSIST                          92          //开机时间小助手

#define SOMPLUGIN_IMAGE_MGR2							93			// 图片管理对象2

#define SOMPLUGIN_CACHE2								94			// 缓存对象2

#define SOMPLUGIN_SPEED_ASSIST2							95			// 图片管理对象2	

#define SOMPLUGIN_DOWNLOADMGR2							96			// pdown 封装

#define SOMPLUGIN_START_OPTIMIZE2						97			// 开机加速对象2

#define SOMPLUGIN_UNINST_SOFT_MGR2						98			// 卸载软件管理对象

// 2010-03-26 定义新的下载对象接口
// 管炜评审通过
#define SOMPLUGIN_DOWNLOADMGR3							99			// pdown封装对象3

//2010-5-14 邢超评审通过
#define SOMPLUGIN_SIGN_LIB								100			// 验证签名
#define SOMPLUGIN_SOM_BAPI_REGISTRY							105	
///////////////////////////////////////////////////////////////////////////////////////////////////////

// 2010-05-7 添加签名校验的功能
// 2010-05-7 邢超评审通过
#ifdef SOMPLUGIN_NEED_LOADLIBRARY
#include "signlib.h"
#ifdef DEBUG
#pragma comment(lib, "signlib.lib")
#else
#pragma comment(lib, "signlib.lib")
#endif
#endif

#define SOM_NO_VTABLE __declspec(novtable)


class SOM_NO_VTABLE ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_PLUGIN };

	STDMETHOD_(ULONG, PluginID)() PURE;
	STDMETHOD_(ULONG, AddRef)() PURE;
	STDMETHOD_(ULONG, Release)() PURE;
};

HRESULT __stdcall SomPlugin(DWORD dwPluginID, ISomPlugin** ppSomPlugin);

#ifdef SOMPLUGIN_NEED_LOADLIBRARY

__declspec(selectany) HRESULT (__stdcall* SomPluginPtr)(DWORD dwPluginID, ISomPlugin** ppSomPlugin) = NULL;
__declspec(selectany) HMODULE g_hSomkernl = NULL;

#else	// SOMPLUGIN_NEED_LOADLIBRARY

__declspec(selectany) HRESULT (__stdcall* SomPluginPtr)(DWORD dwPluginID, ISomPlugin** ppSomPlugin) = SomPlugin;

#endif	// SOMPLUGIN_NEED_LOADLIBRARY

enum
{
	SOMDEBUGGER_FLAG_SHOW		= 0x1	// 通过启动时按特殊键才会显示的信息
};

class ISomDebugger
{
public:
	virtual void SetShowFlag(DWORD dwFlag) = 0;
	virtual DWORD GetShowFlag() = 0;

	virtual void EnterSomPtr(ISomPlugin* p) = 0;
	virtual void LeaveSomPtr(ISomPlugin* p, unsigned __int64 tick) = 0;

	virtual void EnterFunction(LPCWSTR lpFunctionName) = 0;
	virtual void LeaveFunction(LPCWSTR lpFunctionName, unsigned __int64 tick) = 0;

	virtual int StartProcessProfilePhase(LPCWSTR lpText) = 0;
	virtual int StartProfilePhase(LPCWSTR lpText) = 0;
	virtual void StopProfilePhase(int index, LPCWSTR lpText) = 0;

	virtual void OutputTickLog(LPCWSTR lpText, unsigned __int64 tick, ...) = 0;
	virtual void OutputTickLogV(LPCWSTR lpText, unsigned __int64 tick, va_list args) = 0;
	virtual void OutputDebugLog(LPCWSTR pszFormat, ...) = 0;
	virtual void OutputDebugLogV(LPCWSTR pszFormat, va_list args) = 0;
	virtual void OutputDebugLog2(LPCWSTR pszFormat, ...) = 0;			// 只有在程序启动时按特殊的键才会输出
	virtual void OutputDebugLogV2(LPCWSTR pszFormat, va_list args) = 0;	// 只有在程序启动时按特殊的键才会输出
};

class CSomDebuggerDummy : public ISomDebugger
{
public:
	virtual void SetShowFlag(DWORD dwFlag) {}
	virtual DWORD GetShowFlag() { return 0; }

	virtual void EnterSomPtr(ISomPlugin* p) {}
	virtual void LeaveSomPtr(ISomPlugin* p, unsigned __int64 tick) {}

	virtual void EnterFunction(LPCWSTR lpFunctionName) {}
	virtual void LeaveFunction(LPCWSTR lpFunctionName, unsigned __int64 tick) {}

	virtual int StartProcessProfilePhase(LPCWSTR lpText) { return 0; }
	virtual int StartProfilePhase(LPCWSTR lpText) { return 0; }
	virtual void StopProfilePhase(int index, LPCWSTR lpText) {}

	virtual void OutputTickLog(LPCWSTR lpText, unsigned __int64 tick, ...) {}
	virtual void OutputTickLogV(LPCWSTR lpText, unsigned __int64 tick, va_list args) {}
	virtual void OutputDebugLog(LPCWSTR pszFormat, ...) {}
	virtual void OutputDebugLogV(LPCWSTR pszFormat, va_list args) {}
	virtual void OutputDebugLog2(LPCWSTR pszFormat, ...) {}
	virtual void OutputDebugLogV2(LPCWSTR pszFormat, va_list args) {}
};

__declspec(selectany) CSomDebuggerDummy g_SomDebuggerDummy;

static __inline ISomDebugger* QuerySomDebugger()
{
	// 2010-03-26 对指针是否为null 做个判断
	// 管炜评审通过
	ISomDebugger* pSomDebugger = SomPluginPtr ? (ISomDebugger*)SomPluginPtr(SOMPLUGIN_QUERYDEBUGGER, NULL) : NULL;
	return pSomDebugger ? pSomDebugger : &g_SomDebuggerDummy;
}

__declspec(selectany) ISomDebugger* g_pSomDebugger = QuerySomDebugger();

static __inline unsigned __int64 CPUTimeStampCounter()
{
	__asm
	{
		_emit 0x0F;
		_emit 0x31;
	}
}
class CSomProfileFunction
{
public:
	CSomProfileFunction(LPCWSTR lpText)
		: m_lpText(lpText)
	{
		m_tsc	= CPUTimeStampCounter();
		g_pSomDebugger->EnterFunction(m_lpText);
	}
	~CSomProfileFunction()
	{
		g_pSomDebugger->LeaveFunction(m_lpText, CPUTimeStampCounter() - m_tsc);
	}
	void Tick(LPCWSTR pszText, ...)
	{
		va_list args;
		va_start(args, pszText);

		g_pSomDebugger->OutputTickLogV(pszText, CPUTimeStampCounter() - m_tsc, args);
	}
protected:
	unsigned __int64 m_tsc;
	LPCWSTR m_lpText;
};

class CSomProfile
{
public:
	void Begin(LPCWSTR pszText, ...)
	{
		m_tsc	= CPUTimeStampCounter();

		va_list args;
		va_start(args, pszText);

		g_pSomDebugger->OutputDebugLogV2(pszText, args);
	}
	void End(LPCWSTR pszText, ...)
	{
		va_list args;
		va_start(args, pszText);

		g_pSomDebugger->OutputTickLogV(pszText, CPUTimeStampCounter() - m_tsc, args);
	}
protected:
	unsigned __int64 m_tsc;
};
class CSomProfilePhase
{
public:
	CSomProfilePhase()
		: m_ProfilePhaseIndex(-1)
	{
	}
	void Start(LPCWSTR pszText)
	{
		m_ProfilePhaseIndex	= g_pSomDebugger->StartProfilePhase(pszText);
	}
	void Stop(LPCWSTR pszText)
	{
		if (m_ProfilePhaseIndex != -1)
		{
			g_pSomDebugger->StopProfilePhase(m_ProfilePhaseIndex, pszText);
			m_ProfilePhaseIndex	= -1;
		}
	}
protected:
	int m_ProfilePhaseIndex;
};
class CSomProfileProcessPhase : public CSomProfilePhase
{
public:
	void Start(LPCWSTR pszText)
	{
		m_ProfilePhaseIndex	= g_pSomDebugger->StartProcessProfilePhase(pszText);
	}
};

static HRESULT SomInitialize()
{
#ifdef SOMPLUGIN_NEED_LOADLIBRARY

	ATLASSERT(SomPluginPtr == NULL && g_hSomkernl == NULL);

	DWORD dwType;
	WCHAR szPath[MAX_PATH];
	WCHAR szBuffer[0x1000];
	DWORD dwSize = MAX_PATH;
	if (ERROR_SUCCESS == SHGetValue(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\360safe.exe", NULL, &dwType, szPath, &dwSize))
	{
		LPWSTR p = wcsrchr(szPath, '\\');
		*++p = 0;

		if (ExpandEnvironmentStrings(szPath, szBuffer, 0x1000))
		{
			StringCbCat(szBuffer, sizeof(szBuffer), L"SoftMgr\\somkernl.dll");

			// 加载前做360签名验证
			//2010-5-14 邢超评审通过
#ifndef _DEBUG 
			if (! Check360Sign(szBuffer))
				return E_FAIL;
#endif
			g_hSomkernl	= LoadLibrary(szBuffer);
			if (g_hSomkernl)
				*((FARPROC*)&SomPluginPtr)	= GetProcAddress(g_hSomkernl, "SomPlugin");
		}
	}
	if (SomPluginPtr == NULL)
		return E_FAIL;

#endif	// SOMPLUGIN_NEED_LOADLIBRARY

	return SomPluginPtr(SOMPLUGIN_INITIALIZE, NULL);
}

static HRESULT SomUninitialize()
{
	HRESULT hr = SomPluginPtr(SOMPLUGIN_UNINITIALIZE, NULL);

#ifdef SOMPLUGIN_NEED_LOADLIBRARY
	ATLASSERT(SomPluginPtr && g_hSomkernl);
	if (g_hSomkernl)
		FreeLibrary(g_hSomkernl);

#endif	// SOMPLUGIN_NEED_LOADLIBRARY

	return hr;
}

typedef HRESULT (__stdcall* fnCreateInstance)(ISomPlugin** ppSomPlugin);

typedef struct _PLUGINENTRY
{
	DWORD dwID;
	fnCreateInstance CreateInstance;
	LPCWSTR DisplayName;
	WORD Version;
	WORD Flag;

} PLUGINENTRY, *PPLUGINENTRY;

#define IMPLEMENT_PLUGIN_BASE(cplugin, iplugin)										\
	volatile LONG m_cRef;															\
	STDMETHODIMP_(ULONG) AddRef()													\
	{																				\
		return InterlockedIncrement(&m_cRef);										\
	};																				\
	STDMETHODIMP_(ULONG) PluginID()													\
	{																				\
		return iplugin::PLUGINID;													\
	};																				\
	static HRESULT __stdcall CreateInstance(ISomPlugin** ppSomPlugin)				\
	{																				\
		if (ppSomPlugin == NULL)													\
			return E_INVALIDARG;													\
		cplugin* p = new cplugin();													\
		if (p == NULL)																\
			return E_OUTOFMEMORY;													\
		*ppSomPlugin	= (iplugin*)p;												\
		return S_OK;																\
	}																				\
	void* operator new(size_t in_size)												\
	{																				\
		cplugin* p = (cplugin*)::operator new(in_size);								\
		p->m_cRef	= 1;															\
		SomPluginPtr(SOMPLUGIN_INCPLUGINCOUNT, (ISomPlugin**)iplugin::PLUGINID);		\
		return p;																	\
	}
	

#define IMPLEMENT_PLUGIN_RELEASE(cplugin, iplugin)									\
	STDMETHODIMP_(ULONG) Release()													\
	{																				\
		LONG lRef = InterlockedDecrement(&m_cRef);									\
		if (lRef == 0) 																\
		{																			\
			SomPluginPtr(SOMPLUGIN_DECPLUGINCOUNT, (ISomPlugin**)iplugin::PLUGINID);	\
			delete this;															\
			return ULONG(0);														\
		}																			\
		else																		\
			return lRef;															\
	};	
\
#define IMPLEMENT_SINGLETON_EX_RELEASE(cplugin, iplugin)							\
	STDMETHODIMP_(ULONG) Release()													\
	{																				\
		ATLASSERT(PLUGINFLAG & SOMPLUGIN_FLAG_EXIT_NOTIFY);							\
		LONG lRef = InterlockedDecrement(&m_cRef);									\
		if (lRef == 0) 																\
		{																			\
			FinalRelease();															\
			return ULONG(0);														\
		}																			\
		else if (lRef == -1)														\
		{																			\
			SomPluginPtr(SOMPLUGIN_DECPLUGINCOUNT, (ISomPlugin**)iplugin::PLUGINID);	\
			delete this;															\
			return ULONG(0);														\
		}																			\
		else																		\
			return lRef;															\
	};																				\


#define IMPLEMENT_PLUGIN(cplugin, iplugin)											\
	enum { PLUGINFLAG = 0 };														\
	IMPLEMENT_PLUGIN_BASE(cplugin, iplugin)											\
	IMPLEMENT_PLUGIN_RELEASE(cplugin, iplugin)

#define IMPLEMENT_SINGLETON(cplugin, iplugin)										\
	enum { PLUGINFLAG = SOMPLUGIN_FLAG_SINGLETON };									\
	IMPLEMENT_PLUGIN_BASE(cplugin, iplugin)											\
	IMPLEMENT_PLUGIN_RELEASE(cplugin, iplugin)

#define IMPLEMENT_SINGLETON_EX(cplugin, iplugin)									\
	enum { PLUGINFLAG = SOMPLUGIN_FLAG_SINGLETON | SOMPLUGIN_FLAG_EXIT_NOTIFY };	\
	IMPLEMENT_PLUGIN_BASE(cplugin, iplugin)											\
	IMPLEMENT_SINGLETON_EX_RELEASE(cplugin, iplugin)

#define BEGIN_SOMPLUGINS(plugins)													\
	PLUGINENTRY plugins[] =															\
	{																				\
		{ SOMPLUGIN_SIGNATURE, NULL },												

#ifdef SOM_DUBUG_MODE

#define SOMPLUGIN_ENTRY(cplugin)													\
		{ cplugin::PLUGINID, cplugin::CreateInstance, L#cplugin, 0, cplugin::PLUGINFLAG },

#define SOMPLUGIN_ENTRY2(cplugin, version)											\
		{ cplugin::PLUGINID, cplugin::CreateInstance, L#cplugin, version, cplugin::PLUGINFLAG },

#else

#define SOMPLUGIN_ENTRY(cplugin)													\
		{ cplugin::PLUGINID, cplugin::CreateInstance, NULL, 0, cplugin::PLUGINFLAG },

#define SOMPLUGIN_ENTRY2(cplugin, version)											\
		{ cplugin::PLUGINID, cplugin::CreateInstance, NULL, version, cplugin::PLUGINFLAG },
#endif

#define END_SOMPLUGINS()															\
		{ NULL, NULL }																\
	};
