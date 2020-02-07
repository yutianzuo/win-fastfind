/********************************************************************
	created:	2009/10/12
	created:	12:10:2009   18:10
	file base:	plugin
	file ext:	h
	author:		wanlixin
	
	purpose:	SOM �����Ļ���ͷ�ļ���ʵ���� COM ������ĸ��Ϊ�˼򵥻��͸�Ч��Ŀǰ
				��֧�����ü���������֧��QueryInterface,�� COM �ӿڵ� IID �� COM ���� CLSID ͳһ
				��һ������һ������ֻ��һ���ӿڣ�һ���ӿ�Ҳֻ��һ������ʵ�֣�����Ҳ���� GUID ����ʾ
				������һ������ ID����������ֱ���� ID ����Ϊ���󴴽�����±꣬�Ƚϸ�Ч
*********************************************************************/
#pragma once

//#ifdef _DEBUG
#define SOM_DUBUG_MODE			// ��ʾ����������Ϣ
//#endif

#define SOMPLUGIN_SIGNATURE								'somp'		// ��Ч�Ա��
#define SOMPLUGIN_INITIALIZE							0xffffffff	// ��ʼ���������
#define SOMPLUGIN_UNINITIALIZE							0xfffffffe	// ж�ز������
#define SOMPLUGIN_INCPLUGINCOUNT						0xfffffffd	// ��¼������һ�� plugin�������˳���ʱ�����֪����û��ûɾ���� plugin
#define SOMPLUGIN_DECPLUGINCOUNT						0xfffffffc	// ��¼ɾ����һ�� plugin�������˳���ʱ�����֪����û��ûɾ���� plugin
#define SOMPLUGIN_QUERYDEBUGGER							0xfffffffb	// ��ѯ ISomDebugger �ӿ�

#define SOMPLUGIN_FLAG_SINGLETON						0x1			// �����־
#define SOMPLUGIN_FLAG_EXIT_NOTIFY						0x2			// �˳�ʱ��Ҫ��֪ͨ
/////////////////////////////////////////////////////////////////////////////////////////////////////
// ��� ID ������ID һ��Ҫ��֤�������ã��������ɺ���ǰ�Ĳ����ݣ����⣬ID ��������Ϊ�����±꣬�Ա���ٶ�λ�����Բ�Ҫ̫�������������

#define SOMPLUGIN_MAX									0x400		// ������ ID

#define SOMPLUGIN_PLUGIN								1			// IPlugin

#define SOMPLUGIN_SOMSTRING								3			// �ַ���

#define SOMPLUGIN_SOMLIST								4			// ������

#define SOMPLUGIN_STRINGLIST							5			// �ַ��� ����

#define SOMPLUGIN_DWORDLIST								6			// DWORD ����

#define SOMPLUGIN_SOMBINARYBUFFER						7			// ���������ݿ�

#define SOMPLUGIN_SOMASYNCRESULT						8			// �첽������ͨѶ�ӿ�

#define SOMPLUGIN_WINVERSION							10			// windows �汾��Ϣ

#define SOMPLUGIN_SOMHASH								11			// Hash����

#define SOMPLUGIN_SOMKERNEL								20			// ��������

#define SOMPLUGIN_PATHPARSER							21			// ·�����������������н���

#define SOMPLUGIN_SOMREGISTRY							22			// ע������

#define SOMPLUGIN_SOMFILE								23			// �ļ�����

#define SOMPLUGIN_URLPARSER								24			// URL �ķ���

#define SOMPLUGIN_ZLIB									25			// zlib �ķ�װ

#define SOMPLUGIN_LIBPNG								26			// libpng �ķ�װ

#define SOMPLUGIN_JPEG									27			// jpeg �ķ�װ

#define SOMPLUGIN_URLMONIKER							28			// UrlMoniker �ķ�װ

#define SOMPLUGIN_SOMAPPINFO							40			// ����ܼ�ȫ����Ϣ����������Ͱ�ȫ��ʿ��װĿ¼������ mid

#define SOMPLUGIN_HTTPDOWNLOAD							41			// http ���� (���� get, post)

#define SOMPLUGIN_DOWNLOADMGR							42			// pdown ��װ

#define SOMPLUGIN_BAIKE									43			// ��ٿ�����

#define SOMPLUGIN_WHITELIST								44			// ������

#define SOMPLUGIN_FileIconMgr							45			// ͼ�����

#define SOMPLUGIN_SOMINI								47			// �����ļ�

#define SOMPLUGIN_SOMVARIABLE							48			// SomVaribale

#define SOMPLUGIN_SOFTUPDATE							49			// ���������Ϣ

#define SOMPLUGIN_SOFTINSTALLEDINFO						50			// ��������װ��Ϣ

#define SOMPLUGIN_STATISTICSMGR							51			// ͳ����Ϣ

#define SOMPLUGIN_LIVEUPDATE							52			// ����£�����֪ͨ��

#define SOMPLUGIN_WHITELIST_INFO						53			//  ��ȡ�İ�������Ϣ

#define SOMPLUGIN_RUN_SOFT_MGR							54			// ������������������

#define SOMPLUGIN_RUN_SOFT_INFO							55			// �������������Ϣ����

#define SOMPLUGIN_PROC_MODULE_INFO						56			// ���̰�����ģ����Ϣ

#define SOMPLUGIN_START_OPTIMIZE						57			// �������ٶ���

#define SOMPLUGIN_AUTO_RUN_ITEM							58			// ����������Ŀ����

#define SOMPLUGIN_DOWNLOADDATA							59			// ��������״̬��Ϣ

#define SOMPLUGIN_BAIKE_STAR							60			// ��ٿ�����,������Ϣ

#define SOMPLUGIN_BAIKE_ONEWORD							61			// ��ٿ�����,һ�仰���

#define SOMPLUGIN_UNINST_ITEM_INFO						62			// ж��Ԫ�ر��ؽṹ��Ϣ

#define SOMPLUGIN_UNINST_ITEM_BAIKE_INFO				63			// ж��Ԫ�����ϰٿƽṹ��Ϣ

#define SOMPLUGIN_UNINST_SOFT_MGR						64			// ж������������

#define SOMPLUGIN_RUN_SOFT_INFO_EX						65			// ������������ӳټ��ص���Ϣ����

#define SOMPLUGIN_SOFTUPDATEDATA						66			// ���������Ϣ

#define SOMPLUGIN_UNINST_SOFT_REGINFO					67			// ����ע�����Ϣ

#define SOMPLUGIN_SOFTLISTDATA							68			// ����б���Ϣ

#define SOMPLUGIN_SOFTCATALOGDATA						69			// ����б������Ϣ

#define SOMPLUGIN_SOFTLISTMGR							70			// ����б�������

#define SOMPLUGIN_RUN_INFO								71			// ��������Ϣ����

#define SOMPLUGIN_SERIVER_MGR							72			// ������������

#define SOMPLUGIN_AUTO_RUN_ITEM_EX						73			// ����������Ŀ�ӳټ��ص���Ϣ����

#define SOMPLUGIN_CACHE									74			// �������

#define SOMPLUGIN_SERIVER_INFO							75			// ��������Ϣ

#define SOMPLUGIN_TASK_MGR								76			// �ƻ�����������

#define SOMPLUGIN_TASK_INFO								77			// �ƻ�������Ϣ����

#define SOMPLUGIN_RUN_MGR								78			// ������������

#define SOMPLUGIN_UNINST_SOFT_DELTIEM_INFO              79

#define SOMPLUGIN_SQLITE								80			// Sqlite ���ݿ����

#define SOMPLUGIN_WOW64									81			// 32λ����������64λϵͳ�Ϲ��ߺ���

#define SOMPLUGIN_SIMPLE_LOG							82			// ��log�ļ�

#define SOMPLUGIN_SOM_UNINSTSOFT_ITEM					83			// ���ж��

#define SOMPLUGIN_IMAGE_MGR								85			// ͼƬ�������

#define SOMPLUGIN_FILESHELL								86			// XML���ļ���

#define SOMPLUGIN_ASSOCIATION							87			// �ļ�������Ĭ�����뷨ģ��

#define SOMPLUGIN_SETUPUTIL								88			// ����ܼҰ�װж��ʱ�Ĵ���ģ��

#define SOMPLUGIN_DAEMON								89			// �ͻ�������״̬ͳ��ģ�飬���ڿͻ��˿�ʼ���У��˳����л���ĳ�� tab ҳ�������¼�ʱ������

#define SOMPLUGIN_PINTOSTARMENU                         90          //��ʼ�˵�

#define SOMPLUGIN_SETUPJOB                              91          //��װж������

#define SOMPLUGIN_SPEED_ASSIST                          92          //����ʱ��С����

#define SOMPLUGIN_IMAGE_MGR2							93			// ͼƬ�������2

#define SOMPLUGIN_CACHE2								94			// �������2

#define SOMPLUGIN_SPEED_ASSIST2							95			// ͼƬ�������2	

#define SOMPLUGIN_DOWNLOADMGR2							96			// pdown ��װ

#define SOMPLUGIN_START_OPTIMIZE2						97			// �������ٶ���2

#define SOMPLUGIN_UNINST_SOFT_MGR2						98			// ж������������

// 2010-03-26 �����µ����ض���ӿ�
// �������ͨ��
#define SOMPLUGIN_DOWNLOADMGR3							99			// pdown��װ����3

//2010-5-14 �ϳ�����ͨ��
#define SOMPLUGIN_SIGN_LIB								100			// ��֤ǩ��
#define SOMPLUGIN_SOM_BAPI_REGISTRY							105	
///////////////////////////////////////////////////////////////////////////////////////////////////////

// 2010-05-7 ���ǩ��У��Ĺ���
// 2010-05-7 �ϳ�����ͨ��
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
	SOMDEBUGGER_FLAG_SHOW		= 0x1	// ͨ������ʱ��������Ż���ʾ����Ϣ
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
	virtual void OutputDebugLog2(LPCWSTR pszFormat, ...) = 0;			// ֻ���ڳ�������ʱ������ļ��Ż����
	virtual void OutputDebugLogV2(LPCWSTR pszFormat, va_list args) = 0;	// ֻ���ڳ�������ʱ������ļ��Ż����
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
	// 2010-03-26 ��ָ���Ƿ�Ϊnull �����ж�
	// �������ͨ��
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

			// ����ǰ��360ǩ����֤
			//2010-5-14 �ϳ�����ͨ��
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
