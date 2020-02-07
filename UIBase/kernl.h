/********************************************************************
	created:	2009/10/12
	created:	12:10:2009   18:17
	file base:	kernl
	file ext:	h
	author:		wanlixin
	
	purpose:	SOM 对象库的基础对象的实现和一些实用的 C++ 类库 
*********************************************************************/
#pragma once

#ifdef _DEBUG
#define OutputLog SomOutputLog		//  OutputLog 输出的信息在发布版本不会显示
#else
#define OutputLog(LPCWSTR, ...)
#endif

#define OutputLog2 SomOutputLog2	//  OutputLog2 输出的信息在发布版本 默认不会显示 但启动时按特殊的键会显示
#define OutputLog3 SomOutputLog3	//  OutputLog3 输出的信息在发布版本一直会显示

#ifdef SOM_DUBUG_MODE
#define SomProfileFunction			CSomProfileFunction profile(__FUNCTIONW__);
#define SomProfileTick				profile.Tick
#else
#define SomProfileFunction
#define SomProfileTick(LPCWSTR, ...)
#endif



class SOM_NO_VTABLE ISomString : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_SOMSTRING };

	STDMETHOD (AssignString)(LPCWSTR pszSrc) PURE;
	STDMETHOD (AssignStringA)(LPCSTR pszSrc) PURE;

	STDMETHOD_(LPWSTR, GetBufferPtr)() PURE;						// 获得缓冲区指针
	STDMETHOD_(LPWSTR, GetBuffer)(int nMinBufLength) PURE;			// 获得指定大小的缓冲区，ISomString 应该保证这个大小后面多一个可写的位置
	STDMETHOD_(LPWSTR, GetBufferSetLength)(int nNewLength) PURE;	// 获得指定大小的缓冲区，ISomString 应该保证这个大小后面多一个可写的位置

	STDMETHOD_(void, ReleaseBuffer)() PURE;
	STDMETHOD_(void, ReleaseBufferSetLength)(int nNewLength) PURE;

	STDMETHOD_(LPCWSTR, GetString)() PURE;
	STDMETHOD_(DWORD, GetLength)() PURE;

	STDMETHOD_(void, MakeUpper)() PURE;
	STDMETHOD_(void, MakeLower)() PURE;
	STDMETHOD_(void, MakeReverse)() PURE;

	STDMETHOD_(int, Replace)(WCHAR chOld, WCHAR chNew) PURE;
	STDMETHOD_(int, ReplaceString)(LPCWSTR lpszOld, LPCWSTR lpszNew) PURE;

	STDMETHOD_(int, Compare)(LPCWSTR psz) PURE;
	STDMETHOD_(int, CompareNoCase)(LPCWSTR psz) PURE;

	STDMETHOD_(BOOL, IsEmpty)() PURE;
	STDMETHOD_(void, Empty)() PURE;

	STDMETHODV_(void, Format)(LPCWSTR pszFormat, ...) PURE;
	STDMETHODV_(void, FormatV)(LPCWSTR pszFormat, va_list args) PURE;

	STDMETHOD_(void, Trim)() PURE;
	STDMETHOD_(void, TrimLeft)() PURE;
	STDMETHOD_(void, TrimRight)() PURE;

	STDMETHOD_(void, Left)(int nCount, ISomString** ppOutStr) PURE;
	STDMETHOD_(void, Right)(int nCount, ISomString** ppOutStr) PURE;
	STDMETHOD_(void, Mid)(int nFirst, int nCount, ISomString** ppOutStr) PURE;
};

class SOM_NO_VTABLE ISomList : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_SOMLIST };

	STDMETHOD (Add)(ISomPlugin* pPlugin) PURE;
	STDMETHOD_(DWORD, Count)() PURE;
	STDMETHOD_(void ,Clear)() PURE;
	STDMETHOD (Item)(DWORD index, ISomPlugin** ppPlugin) PURE;
};

class SOM_NO_VTABLE ISomStringList : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_STRINGLIST };

	STDMETHOD (Add)(ISomString* pSomString) PURE;
	STDMETHOD (AddString)(LPCWSTR lpStr) PURE;
	STDMETHOD_(DWORD, Count)() PURE;
	STDMETHOD_(void ,Clear)() PURE;
	STDMETHOD (Item)(DWORD index, ISomString** ppSomString) PURE;
};

class SOM_NO_VTABLE ISomDWORDList : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_DWORDLIST };

	STDMETHOD (Add)(DWORD dwValue) PURE;
	STDMETHOD_(DWORD, Count)() PURE;
	STDMETHOD_(void ,Clear)() PURE;
	STDMETHOD (Item)(DWORD index, DWORD* pdwValue) PURE;
};

// 二进制数据块
class SOM_NO_VTABLE ISomBinaryBuffer : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_SOMBINARYBUFFER };

	// AllocBuffer实际分配空间大小为dwSize + 2，后两个字节为空，但是不影响GetSize结果，方便处理字符串需求。
	STDMETHOD_(LPBYTE, AllocBuffer)(DWORD dwSize) PURE;
	STDMETHOD_(LPBYTE, GetBuffer)() PURE;
	STDMETHOD_(DWORD, GetSize)() PURE;
};


// ISomAsyncResult::Complete 的 dwError 的可能值

#define SOMASYNCRESULT_PENDING	0		// 操作还在进行中，还没有完成
#define SOMASYNCRESULT_OK		1		// 成功完成
#define SOMASYNCRESULT_FAIL		2		// 失败

// 异步操作的通讯接口，必须多线程安全
class SOM_NO_VTABLE ISomAsyncResult : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_SOMASYNCRESULT };

	STDMETHOD_(LPVOID, GetCreateParameter)() PURE;
	STDMETHOD (Complete)(DWORD dwError, ISomPlugin* pResultObj) PURE;	// 异步操作完成，设置操作的完成情况和完成的结果对象
	STDMETHOD_(DWORD, GetCompleteResult)(ISomPlugin** pResultObj) PURE;
};

class ISomSimpleLog;
/////////////////////////////////////////////////////////////////////////////////////////
// C / C++ 辅助类
ATLINLINE ATLAPI_(ISomPlugin*) SomPtrAssign(ISomPlugin** pp, ISomPlugin* lp)
{
	if (pp == NULL)
		return NULL;

	if (lp != NULL)
		lp->AddRef();
	if (*pp)
		(*pp)->Release();
	*pp = lp;
	return lp;
}

template <class T>
class _NoAddRefReleaseOnCSomPtr : public T
{
private:
	STDMETHOD_(ULONG, AddRef)() PURE;
	STDMETHOD_(ULONG, Release)() PURE;
};
template<class T>
class CSomPtrBase
{
protected:
	CSomPtrBase() throw()
	{
		m_p	= NULL;
	}
	CSomPtrBase(int nNull) throw()
	{
		ATLASSERT(nNull == 0);
		(void)nNull;
		m_p = NULL;
	}
	CSomPtrBase(T* lp) throw()
	{
		m_p = lp;
		if (lp != NULL)
			lp->AddRef();
	}
	class proxy
	{ 
	public:
		proxy(T* p) : m_p(p)
		{
			m_tsc	= CPUTimeStampCounter();
			g_pSomDebugger->EnterSomPtr(m_p);
		}
		~proxy()
		{
			g_pSomDebugger->LeaveSomPtr(m_p, CPUTimeStampCounter() - m_tsc);
		}

		_NoAddRefReleaseOnCSomPtr<T>* operator->(){ return (_NoAddRefReleaseOnCSomPtr<T>*)m_p; }	

	private:
		T* m_p;
		unsigned __int64 m_tsc;
	};

public:
	~CSomPtrBase() throw()
	{
		if (m_p)
			m_p->Release();
	}
	operator T*() const throw()
	{ 
		return m_p; 
	}
	T& operator*() const
	{
		ATLENSURE(m_p != NULL);
		return *m_p;
	}
	T** operator&()
	{
		ATLASSERT(m_p == NULL);
		Release();
		m_p = NULL;
		return &m_p;
	}

#ifdef SOM_DUBUG_MODE
	proxy operator->() const throw()
	{ 
		ATLASSERT(m_p != NULL);
		return m_p;
	}
#else
	_NoAddRefReleaseOnCSomPtr<T>* operator->() const throw()
	{ 
		ATLASSERT(m_p != NULL);
		return (_NoAddRefReleaseOnCSomPtr<T>*)m_p;
	}
#endif

	bool operator!() const throw()
	{
		return (m_p == NULL);
	}
	bool operator!=(T* pT) const
	{
		return !operator==(pT);
	}
	bool operator==(T* pT) const throw()
	{
		return m_p == pT;
	}
	void Release()
	{
		T* pTemp = m_p;
		if (pTemp)
		{
			m_p = NULL;
			pTemp->Release();
		}
	}
	void Attach(T* p) throw()
	{
		Release();
		m_p = p;
	}

	T* Detach() throw()
	{
		T* const old = m_p;
		m_p = NULL;
		return old;
	}
	HRESULT CreateInstance()
	{
		ATLASSERT(m_p == NULL);
		Release();
		return SomPluginPtr(T::PLUGINID, (ISomPlugin**)&m_p);
	}
	T* operator=(T* p)
	{
		Attach(p);
		if (m_p != NULL)
			m_p->AddRef();

		return *this;
	}

	T* m_p;
};

template <class T, bool bAutoCreate = false>
class CSomPtr : public CSomPtrBase<T>
{
public:
	CSomPtr() throw()
	{
		if (bAutoCreate)
		{
			HRESULT hr = CreateInstance();
			ATLASSERT(SUCCEEDED(hr));
		}
	}
	CSomPtr(int nNull) throw()
		: CSomPtrBase<T>(nNull)
	{
	}
	CSomPtr(T* lp) throw()
		: CSomPtrBase<T>(lp)
	{
	}
	CSomPtr(const CSomPtr<T, true>& lp) throw() 
		: CSomPtrBase<T>(lp.m_p)
	{
	}
	CSomPtr(const CSomPtr<T, false>& lp) throw() 
		: CSomPtrBase<T>(lp.m_p)
	{
	}
	T* operator=(T* lp) throw()
	{
		if (*this != lp)
		{
			return static_cast<T*>(SomPtrAssign((ISomPlugin**)&m_p, lp));
		}
		return *this;
	}
	T* operator=(const CSomPtr<T, true>& lp) throw()
	{
		if (*this != lp)
		{
			return static_cast<T*>(SomPtrAssign((ISomPlugin**)&m_p, lp));
		}
		return *this;
	}
	T* operator=(const CSomPtr<T, false>& lp) throw()
	{
		if (*this != lp)
		{
			return static_cast<T*>(SomPtrAssign((ISomPlugin**)&m_p, lp));
		}
		return *this;
	}
};

class CPluginString : public ISomString
{
public:
	IMPLEMENT_PLUGIN(CPluginString, ISomString)

	CPluginString()
	{

	}
	CPluginString(LPCSTR lpStr)
		: m_str(lpStr)
	{
	}
	CPluginString(LPCWSTR lpStr)
		: m_str(lpStr)
	{
	}
	CPluginString(const CString& lpStr)
		: m_str(lpStr)
	{
	}
	STDMETHODIMP AssignString(LPCWSTR pszSrc)
	{
		if (pszSrc == NULL)
			return E_INVALIDARG;

		m_str	= pszSrc;

		return S_OK;
	}
	STDMETHODIMP AssignStringA(LPCSTR pszSrc)
	{
		if (pszSrc == NULL)
			return E_INVALIDARG;

		m_str	= pszSrc;

		return S_OK;
	}
	STDMETHODIMP_(LPWSTR) GetBufferPtr()
	{
		return m_str.GetBuffer();
	}
	STDMETHODIMP_(LPWSTR) GetBuffer(int nMinBufLength)
	{
		return m_str.GetBuffer(nMinBufLength);
	}
	STDMETHODIMP_(LPWSTR) GetBufferSetLength(int nNewLength)
	{
		return m_str.GetBufferSetLength(nNewLength);
	}
	STDMETHODIMP_(void) ReleaseBuffer()
	{
		m_str.ReleaseBuffer();
	}
	STDMETHODIMP_(void) ReleaseBufferSetLength(int nNewLength)
	{
		m_str.ReleaseBufferSetLength(nNewLength);
	}
	STDMETHODIMP_(LPCWSTR) GetString()
	{
		return m_str;
	}
	STDMETHODIMP_(DWORD) GetLength()
	{
		return m_str.GetLength();
	}
	STDMETHODIMP_(void) MakeUpper()
	{
		m_str.MakeUpper();
	}
	STDMETHODIMP_(void) MakeLower()
	{
		m_str.MakeLower();
	}
	STDMETHODIMP_(void) MakeReverse()
	{
		m_str.MakeReverse();
	}
	STDMETHODIMP_(int) Replace(WCHAR chOld, WCHAR chNew)
	{
		return m_str.Replace(chOld, chNew);
	}
	STDMETHODIMP_(int) ReplaceString(LPCWSTR lpszOld, LPCWSTR lpszNew)
	{
		return m_str.Replace(lpszOld, lpszNew);
	}
	STDMETHODIMP_(int) Compare(LPCWSTR psz)
	{
		return m_str.Compare(psz);
	}
	STDMETHODIMP_(int) CompareNoCase(LPCWSTR psz)
	{
		return m_str.CompareNoCase(psz);
	}
	STDMETHODIMP_(BOOL) IsEmpty()
	{
		return m_str.IsEmpty();
	}
	STDMETHODIMP_(void) Empty()
	{
		m_str.Empty();
	}
	STDMETHODIMPV_(void) Format(LPCWSTR pszFormat, ...)
	{
		va_list args;
		va_start(args, pszFormat);

		m_str.FormatV(pszFormat, args);
	}
	STDMETHODIMPV_(void) FormatV(LPCWSTR pszFormat, va_list args)
	{
		m_str.FormatV(pszFormat, args);
	}
	STDMETHODIMP_(void) Trim()
	{
		m_str.Trim();
	}
	STDMETHODIMP_(void) TrimLeft()
	{
		m_str.TrimLeft();
	}
	STDMETHODIMP_(void) TrimRight()
	{
		m_str.TrimRight();
	}
	STDMETHODIMP_(void) Left(int nCount, ISomString** ppOutStr)
	{
		*ppOutStr	= new CPluginString(m_str.Left(nCount));
	}
	STDMETHODIMP_(void) Right(int nCount, ISomString** ppOutStr)
	{
		*ppOutStr	= new CPluginString(m_str.Right(nCount));
	}
	STDMETHODIMP_(void) Mid(int nFirst, int nCount, ISomString** ppOutStr)
	{
		*ppOutStr	= new CPluginString(m_str.Mid(nFirst, nCount));
	}
protected:
	CString m_str;
};
//specialization for ISomString
template <bool bAutoCreate>
class CSomPtr<ISomString, bAutoCreate> : public CSomPtrBase<ISomString>
{
public:
	CSomPtr(ISomString* lp) throw()
		: CSomPtrBase<ISomString>(lp)
	{
	}
	CSomPtr(const CSomPtr<ISomString, true>& lp) throw()
		: CSomPtrBase<ISomString>(lp.m_p)
	{
	}
	CSomPtr(const CSomPtr<ISomString, false>& lp) throw()
		: CSomPtrBase<ISomString>(lp.m_p)
	{
	}
	ISomString* operator=(ISomString* lp) throw()
	{
		if (*this != lp)
		{
			return static_cast<ISomString*>(SomPtrAssign((ISomPlugin**)&m_p, lp));
		}
		return *this;
	}
	ISomString* operator=(const CSomPtr<ISomString, true>& lp) throw()
	{
		if (*this != lp)
		{
			return static_cast<ISomString*>(SomPtrAssign((ISomPlugin**)&m_p, lp.m_p));
		}
		return *this;
	}
	ISomString* operator=(const CSomPtr<ISomString, false>& lp) throw()
	{
		if (*this != lp)
		{
			return static_cast<ISomString*>(SomPtrAssign((ISomPlugin**)&m_p, lp.m_p));
		}
		return *this;
	}
	CSomPtr() throw()
	{
		if (bAutoCreate)
			m_p	= new CPluginString();
	}
	CSomPtr(const CString& lpStr) throw()
	{
		m_p	= new CPluginString(lpStr);
	}
	CSomPtr(LPCSTR lpStr) throw()
	{
		m_p	= new CPluginString(lpStr);
	}
	CSomPtr(LPCWSTR lpStr) throw()
	{
		m_p	= new CPluginString(lpStr);
	}
	operator LPCWSTR() const throw()
	{
		ATLASSERT(m_p != NULL);
		if (m_p)
			return m_p->GetString();
		else
			return L"";
	}
	CSomPtr<ISomString, bAutoCreate>& operator=(LPCWSTR pszSrc)
	{
		ATLASSERT(m_p != NULL);
		if (m_p)
			m_p->AssignString(pszSrc);

		return (*this);
	}
	CSomPtr<ISomString, bAutoCreate>& operator=(LPCSTR pszSrc)
	{
		ATLASSERT(m_p != NULL);
		if (m_p)
			m_p->AssignStringA(pszSrc);

		return (*this);
	}
	LPWSTR GetBuffer()
	{
		ATLASSERT(m_p != NULL);
		if (m_p)
			return m_p->GetBufferPtr();
		else
			return NULL;
	}
	LPWSTR GetBuffer(int nMinBufferLength)
	{
		ATLASSERT(m_p != NULL);
		if (m_p)
			return m_p->GetBuffer(nMinBufferLength);
		else
			return NULL;
	}
	LPWSTR GetBufferSetLength(int nLength)
	{
		ATLASSERT(m_p != NULL);
		if (m_p)
			return m_p->GetBufferSetLength(nLength);
		else
			return NULL;
	}
	void ReleaseBuffer(int nNewLength = -1)
	{
		ATLASSERT(m_p != NULL);
		if (m_p)
			m_p->ReleaseBuffer();
	}
	void ReleaseBufferSetLength(int nNewLength)
	{
		ATLASSERT(m_p != NULL);
		if (m_p)
			m_p->ReleaseBufferSetLength(nNewLength);
	}
	int GetLength() const throw()
	{
		ATLASSERT(m_p != NULL);
		if (m_p)
			return m_p->GetLength();
		else
			return 0;
	}
	CSomPtr<ISomString, bAutoCreate>& MakeLower()
	{
		ATLASSERT(m_p != NULL);
		if (m_p)
			m_p->MakeLower();

		return (*this);
	}
	CSomPtr<ISomString, bAutoCreate>& MakeUpper()
	{
		ATLASSERT(m_p != NULL);
		if (m_p)
			m_p->MakeUpper();

		return (*this);
	}
	void MakeReverse()
	{
		ATLASSERT(m_p != NULL);
		if (m_p)
			m_p->MakeReverse();
	}
	int Replace(WCHAR chOld, WCHAR chNew)
	{
		ATLASSERT(m_p != NULL);
		if (m_p)
			m_p->Replace(chOld, chNew);

		return 0;
	}
	int Replace(LPCWSTR lpszOld, LPCWSTR lpszNew)
	{
		ATLASSERT(m_p != NULL);
		if (m_p)
			return m_p->ReplaceString(lpszOld, lpszNew);

		return 0;
	}
	int Compare(LPCWSTR psz) const
	{
		ATLASSERT(m_p != NULL);
		if (m_p)
			return m_p->Compare(psz);

		return 0;
	}
	int CompareNoCase(LPCWSTR psz) const
	{
		ATLASSERT(m_p != NULL);
		if (m_p)
			return m_p->CompareNoCase(psz);

		return 0;
	}
	BOOL IsEmpty()
	{
		ATLASSERT(m_p != NULL);
		if (m_p)
			return m_p->IsEmpty();

		return TRUE;
	}
	void Empty()
	{
		ATLASSERT(m_p != NULL);
		if (m_p)
			m_p->Empty();
	}
	void Format(LPCWSTR pszFormat, ...)
	{
		ATLASSERT(m_p != NULL);
		if (m_p)
		{
			va_list args;
			va_start(args, pszFormat);
			m_p->FormatV(pszFormat, args);
		}
	}
	void FormatV(LPCWSTR pszFormat, va_list args)
	{
		ATLASSERT(m_p != NULL);
		if (m_p)
			m_p->FormatV(pszFormat, args);
	}
	CSomPtr<ISomString, bAutoCreate>& Trim()
	{
		ATLASSERT(m_p != NULL);
		if (m_p)
			m_p->Trim();

		return (*this);
	}
	CSomPtr<ISomString, bAutoCreate>& TrimLeft()
	{
		ATLASSERT(m_p != NULL);
		if (m_p)
			m_p->TrimLeft();

		return (*this);
	}
	CSomPtr<ISomString, bAutoCreate>& TrimRight()
	{
		ATLASSERT(m_p != NULL);
		if (m_p)
			m_p->TrimRight();

		return (*this);
	}
	CSomPtr<ISomString, bAutoCreate> Left(int nCount)
	{
		CSomStringPtr spOutStr;

		ATLASSERT(m_p != NULL);
		if (m_p)
		{
			m_p->Left(nCount, &spOutStr);
		}

		return spOutStr;
	}
	CSomPtr<ISomString, bAutoCreate> Right(int nCount)
	{
		CSomStringPtr spOutStr;

		ATLASSERT(m_p != NULL);
		if (m_p)
		{
			m_p->Right(nCount, &spOutStr);
		}

		return spOutStr;
	}
	CSomPtr<ISomString, bAutoCreate> Mid(int nFirst, int nCount)
	{
		CSomStringPtr spOutStr;

		ATLASSERT(m_p != NULL);
		if (m_p)
		{
			m_p->Mid(nFirst, nCount, &spOutStr);
		}

		return spOutStr;
	}
};

typedef CSomPtr<ISomString, false>	CSomStringPtr;
typedef CSomPtr<ISomString, true>	CSomString;

//specialization for ISomStringList
template <bool bAutoCreate>
class CSomPtr<ISomStringList, bAutoCreate> : public CSomPtrBase<ISomStringList>
{
public:
	CSomPtr(ISomStringList* lp) throw()
		: CSomPtrBase<ISomStringList>(lp)
	{
	}
	CSomPtr(const CSomPtr<ISomStringList, true>& lp) throw()
		: CSomPtrBase<ISomStringList>(lp.m_p)
	{
	}
	CSomPtr(const CSomPtr<ISomStringList, false>& lp) throw()
		: CSomPtrBase<ISomStringList>(lp.m_p)
	{
	}
	ISomStringList* operator=(ISomStringList* lp) throw()
	{
		if (*this != lp)
		{
			return static_cast<ISomStringList*>(SomPtrAssign((ISomPlugin**)&m_p, lp));
		}
		return *this;
	}
	ISomStringList* operator=(const CSomPtr<ISomStringList, true>& lp) throw()
	{
		if (*this != lp)
		{
			return static_cast<ISomStringList*>(SomPtrAssign((ISomPlugin**)&m_p, lp.m_p));
		}
		return *this;
	}
	ISomStringList* operator=(const CSomPtr<ISomStringList, false>& lp) throw()
	{
		if (*this != lp)
		{
			return static_cast<ISomStringList*>(SomPtrAssign((ISomPlugin**)&m_p, lp.m_p));
		}
		return *this;
	}
	CSomPtr() throw()
	{
		if (bAutoCreate)
		{
			HRESULT hr = CreateInstance();
			ATLASSERT(SUCCEEDED(hr));
		}
	}
	void Add(ISomString* pSomString)
	{
		ATLASSERT(m_p != NULL);

		if (m_p)
			m_p->Add(pSomString);
	}
	void Add(LPCWSTR lpStr)
	{
		ATLASSERT(m_p != NULL);

		if (m_p)
			m_p->AddString(lpStr);
	}
	DWORD Count()
	{
		ATLASSERT(m_p != NULL);

		if (m_p)
			return m_p->Count();

		return 0;
	}
	void Clear()
	{
		ATLASSERT(m_p != NULL);

		if (m_p)
			m_p->Clear();
	}
	CSomStringPtr operator[] (int index) const throw()
	{
		ATLASSERT(m_p != NULL);

		CSomStringPtr strValue;
		if (m_p && SUCCEEDED(m_p->Item(index, &strValue)))
			return strValue;

		ATLASSERT(FALSE);
		return strValue;
	}
};

typedef CSomPtr<ISomStringList, false>	CSomStringListPtr;
typedef CSomPtr<ISomStringList, true>	CSomStringList;

//specialization for ISomDWORDList
template <bool bAutoCreate>
class CSomPtr<ISomDWORDList, bAutoCreate> : public CSomPtrBase<ISomDWORDList>
{
public:
	CSomPtr(ISomDWORDList* lp) throw()
		: CSomPtrBase<ISomDWORDList>(lp)
	{
	}
	CSomPtr(const CSomPtr<ISomDWORDList, true>& lp) throw()
		: CSomPtrBase<ISomDWORDList>(lp.m_p)
	{
	}
	CSomPtr(const CSomPtr<ISomDWORDList, false>& lp) throw()
		: CSomPtrBase<ISomDWORDList>(lp.m_p)
	{
	}
	ISomDWORDList* operator=(ISomDWORDList* lp) throw()
	{
		if (*this != lp)
		{
			return static_cast<ISomDWORDList*>(SomPtrAssign((ISomPlugin**)&m_p, lp));
		}
		return *this;
	}
	ISomDWORDList* operator=(const CSomPtr<ISomDWORDList, true>& lp) throw()
	{
		if (*this != lp)
		{
			return static_cast<ISomDWORDList*>(SomPtrAssign((ISomPlugin**)&m_p, lp.m_p));
		}
		return *this;
	}
	ISomDWORDList* operator=(const CSomPtr<ISomDWORDList, false>& lp) throw()
	{
		if (*this != lp)
		{
			return static_cast<ISomDWORDList*>(SomPtrAssign((ISomPlugin**)&m_p, lp.m_p));
		}
		return *this;
	}
	CSomPtr() throw()
	{
		if (bAutoCreate)
		{
			HRESULT hr = CreateInstance();
			ATLASSERT(SUCCEEDED(hr));
		}
	}
	void Add(DWORD dwValue)
	{
		ATLASSERT(m_p != NULL);

		if (m_p)
			m_p->Add(dwValue);
	}
	DWORD Count()
	{
		ATLASSERT(m_p != NULL);

		if (m_p)
			return m_p->Count();

		return 0;
	}
	void Clear()
	{
		ATLASSERT(m_p != NULL);

		if (m_p)
			m_p->Clear();
	}
	DWORD operator[] (int index) const throw()
	{
		ATLASSERT(m_p != NULL);

		DWORD dwValue;
		if (m_p && SUCCEEDED(m_p->Item(index, &dwValue)))
			return dwValue;

		ATLASSERT(FALSE);
		return 0;
	}
};


typedef CSomPtr<ISomDWORDList, false>	CSomDWORDListPtr;
typedef CSomPtr<ISomDWORDList, true>	CSomDWORDList;

class CWindowMessageAsyncResult : public ISomAsyncResult
{
public:
	IMPLEMENT_PLUGIN(CWindowMessageAsyncResult, ISomAsyncResult)

	CWindowMessageAsyncResult()
	{
		m_dwError			= SOMASYNCRESULT_PENDING;
		m_hWndNotify		= NULL;
		m_messageNotify		= WM_NULL;
		m_wParamNotify		= 0;
		m_lParamNofity		= 0;
		m_lpParameter		= NULL;
	}
	CWindowMessageAsyncResult(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPVOID lpParameter)
	{
		m_dwError			= SOMASYNCRESULT_PENDING;
		m_hWndNotify		= hWnd;
		m_messageNotify		= message;
		m_wParamNotify		= wParam;
		m_lParamNofity		= lParam;
		m_lpParameter		= lpParameter;
	}
protected:
	STDMETHODIMP_(LPVOID) GetCreateParameter()
	{
		return m_lpParameter;
	}
	STDMETHODIMP Complete(DWORD dwError, ISomPlugin* pResultObj)
	{
		m_dwError	= dwError;

		if (pResultObj)
			m_ResultObj	= pResultObj;

		SendMessage(m_hWndNotify, m_messageNotify, m_wParamNotify, m_lParamNofity);

		return S_OK;
	}
	STDMETHODIMP_(DWORD) GetCompleteResult(ISomPlugin** pResultObj)
	{   
		if (pResultObj)
		{
			if (m_ResultObj)
			{
				*pResultObj	= m_ResultObj;
				(*pResultObj)->AddRef();
			}
			else
			{
				*pResultObj	= NULL;
			}
		}

		return m_dwError;
	}

	CSomPtr<ISomPlugin> m_ResultObj;
	DWORD m_dwError;

	HWND m_hWndNotify;
	UINT m_messageNotify;
	WPARAM m_wParamNotify;
	LPARAM m_lParamNofity;
	LPVOID m_lpParameter;
};

class CThreadMessageAsyncResult : public ISomAsyncResult
{
public:
	IMPLEMENT_PLUGIN(CThreadMessageAsyncResult, ISomAsyncResult)

	CThreadMessageAsyncResult()
	{
	}
	CThreadMessageAsyncResult(DWORD idThread, UINT message, WPARAM wParam, LPARAM lParam, LPVOID lpParameter)
	{
		m_dwError			= SOMASYNCRESULT_PENDING;
		m_idThreadNotify	= idThread;
		m_messageNotify		= message;
		m_wParamNotify		= wParam;
		m_lParamNofity		= lParam;
		m_lpParameter		= lpParameter;
	}

protected:
	STDMETHODIMP_(LPVOID) GetCreateParameter()
	{
		return m_lpParameter;
	}
	STDMETHODIMP Complete(DWORD dwError, ISomPlugin* pResultObj)
	{
		m_dwError	= dwError;

		if (pResultObj)
			m_ResultObj	= pResultObj;

		PostThreadMessage(m_idThreadNotify, m_messageNotify, m_wParamNotify, m_lParamNofity);

		return S_OK;
	}
	STDMETHODIMP_(DWORD) GetCompleteResult(ISomPlugin** pResultObj)
	{
		if (pResultObj)
		{
			if (m_ResultObj)
			{
				*pResultObj	= m_ResultObj;
				(*pResultObj)->AddRef();
			}
			else
			{
				*pResultObj	= NULL;
			}
		}

		return m_dwError;
	}

	CSomPtr<ISomPlugin> m_ResultObj;
	DWORD m_dwError;

	DWORD m_idThreadNotify;
	UINT m_messageNotify;
	WPARAM m_wParamNotify;
	LPARAM m_lParamNofity;
	LPVOID m_lpParameter;
};

class CEventAsyncResult : public ISomAsyncResult
{
public:
	IMPLEMENT_PLUGIN(CEventAsyncResult, ISomAsyncResult)

	CEventAsyncResult()
	{
		m_dwError			= SOMASYNCRESULT_PENDING;
		m_hEventNotify		= NULL;
		m_lpParameter		= NULL;
	}
	CEventAsyncResult(HANDLE hEvent, LPVOID lpParameter)
	{
		m_dwError			= SOMASYNCRESULT_PENDING;
		m_hEventNotify		= hEvent;
		m_lpParameter		= lpParameter;
	}

protected:
	STDMETHODIMP_(LPVOID) GetCreateParameter()
	{
		return m_lpParameter;
	}
	STDMETHODIMP Complete(DWORD dwError, ISomPlugin* pResultObj)
	{
		m_dwError	= dwError;

		if (pResultObj)
			m_ResultObj	= pResultObj;

		SetEvent(m_hEventNotify);

		return S_OK;
	}
	STDMETHODIMP_(DWORD) GetCompleteResult(ISomPlugin** pResultObj)
	{
		if (pResultObj)
		{
			if (m_ResultObj)
			{
				*pResultObj	= m_ResultObj;
				(*pResultObj)->AddRef();
			}
			else
			{
				*pResultObj	= NULL;
			}
		}

		return m_dwError;
	}

	CSomPtr<ISomPlugin> m_ResultObj;
	DWORD m_dwError;

	HANDLE m_hEventNotify;
	LPVOID m_lpParameter;
};

//specialization for ISomAsyncResult
template <bool bAutoCreate>
class CSomPtr<ISomAsyncResult, bAutoCreate> : public CSomPtrBase<ISomAsyncResult>
{
public:
	CSomPtr(ISomAsyncResult* lp) throw()
		: CSomPtrBase<ISomAsyncResult>(lp)
	{
	}
	CSomPtr(const CSomPtr<ISomAsyncResult, true>& lp) throw()
		: CSomPtrBase<ISomAsyncResult>(lp.m_p)
	{
	}
	CSomPtr(const CSomPtr<ISomAsyncResult, false>& lp) throw()
		: CSomPtrBase<ISomAsyncResult>(lp.m_p)
	{
	}
	ISomAsyncResult* operator=(ISomAsyncResult* lp) throw()
	{
		if (*this != lp)
		{
			return static_cast<ISomAsyncResult*>(SomPtrAssign((ISomPlugin**)&m_p, lp));
		}
		return *this;
	}
	ISomAsyncResult* operator=(const CSomPtr<ISomAsyncResult, true>& lp) throw()
	{
		if (*this != lp)
		{
			return static_cast<ISomAsyncResult*>(SomPtrAssign((ISomPlugin**)&m_p, lp.m_p));
		}
		return *this;
	}
	ISomAsyncResult* operator=(const CSomPtr<ISomAsyncResult, false>& lp) throw()
	{
		if (*this != lp)
		{
			return static_cast<ISomAsyncResult*>(SomPtrAssign((ISomPlugin**)&m_p, lp.m_p));
		}
		return *this;
	}
	CSomPtr() throw()
	{
		if (bAutoCreate)
		{
			HRESULT hr = CreateInstance();
			ATLASSERT(SUCCEEDED(hr));
		}
	}
	CSomPtr(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPVOID lpParameter)
	{
		m_p	= new CWindowMessageAsyncResult(hWnd, message, wParam, lParam, lpParameter);
	}
	CSomPtr(DWORD idThread, UINT message, WPARAM wParam, LPARAM lParam, LPVOID lpParameter)
	{
		m_p	= new CThreadMessageAsyncResult(idThread, message, wParam, lParam, lpParameter);
	}
	CSomPtr(HANDLE hEvent, LPVOID lpParameter)
	{
		m_p	= new CEventAsyncResult(hEvent, lpParameter);
	}
};


typedef CSomPtr<ISomAsyncResult, false>	CSomAsyncResultPtr;
typedef CSomPtr<ISomAsyncResult, true>	CSomAsyncResult;

class CAutoCriticalSection
{
public:
	CAutoCriticalSection()
	{
		InitializeCriticalSection(&m_cs);
	}
	~CAutoCriticalSection()
	{
		DeleteCriticalSection(&m_cs);
	}
	void Lock()
	{
		EnterCriticalSection(&m_cs);
	}
	void Unlock()
	{
		LeaveCriticalSection(&m_cs);
	}
private:
	CRITICAL_SECTION m_cs;
};

class CAutoLock 
{
    CAutoLock(const CAutoLock &refAutoLock);
    CAutoLock &operator=(const CAutoLock &refAutoLock);

protected:
    CAutoCriticalSection* m_pLock;

public:
    CAutoLock(CAutoCriticalSection* plock)
    {
        m_pLock = plock;
        m_pLock->Lock();
    };

    ~CAutoLock() 
	{
        m_pLock->Unlock();
    };
};

// Simplelog
class SOM_NO_VTABLE ISomSimpleLog : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_SIMPLE_LOG };

	STDMETHOD_ (BOOL ,SomSimpleLog)(LPCWSTR pszText,...)PURE;
	STDMETHOD_ (BOOL ,SomSimpleLog)(LPSTR pszText,...)PURE;

	// 设置是否显示日志的标志
	STDMETHOD_ (void ,SetShowFlag)(DWORD dwFlag) PURE;
	STDMETHOD_ (DWORD ,GetShowFlag)() PURE;
};

class CSimpleLog: public ISomSimpleLog
{
public:
	IMPLEMENT_SINGLETON(CSimpleLog, ISomSimpleLog)

	STDMETHODIMP_(BOOL) SomSimpleLog(LPCWSTR pszText,...)
	{
		va_list args;
		va_start(args, pszText);

		g_pSomDebugger->OutputDebugLogV(pszText, args);

		return TRUE;
	}
	STDMETHODIMP_(BOOL) SomSimpleLog(LPSTR pszText,...)
	{
		USES_CONVERSION;
		LPCWSTR psaBuf = A2W(pszText);

		va_list args;
		va_start(args, pszText);

		g_pSomDebugger->OutputDebugLogV(psaBuf, args);

		return TRUE;
	}
	STDMETHODIMP_(void) SetShowFlag(DWORD dwFlag)
	{
		g_pSomDebugger->SetShowFlag(dwFlag);
	}
	STDMETHODIMP_(DWORD) GetShowFlag()
	{
		return g_pSomDebugger->GetShowFlag();
	}
};

inline void SomOutputLog(LPCWSTR lpFormat, ...)
{
	va_list args;
	va_start(args, lpFormat);

	g_pSomDebugger->OutputDebugLogV(lpFormat, args);
}

// 该函数输出的信息 在发布后的版本 也可以看到
inline void SomOutputLog2(LPCWSTR lpFormat, ...)
{
	va_list args;
	va_start(args, lpFormat);

	g_pSomDebugger->OutputDebugLogV2(lpFormat, args);
}

inline void SomOutputLog3(LPCWSTR lpFormat, ...)
{
	va_list args;
	va_start(args, lpFormat);

	g_pSomDebugger->OutputDebugLogV(lpFormat, args);
}



#include "common.h"