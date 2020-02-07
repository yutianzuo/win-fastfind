/*
 * Copyright(C) 2009,yutianzuo (All rights reserved.)
 * 
 * FileDes  : CThreadBase.h
 * Abstract : 
 * Version  : 1.0 
 * Author   : yutianzuo
 * Date     : 2009-2-18
 * 
 */
#ifndef __CBTHREADBASE_H__
#define __CBTHREADBASE_H__
//------------------------------------------------------------------------

#include <assert.h>
#include <process.h>
#include <windows.h>
#include <vector>


class CThreadBase
{
protected:
	enum
	{
		ENUM_AUTO_DELETE_SELF = 99,
	};
public:	
	typedef unsigned (__stdcall * ThreadFunc)(LPVOID param);

private:
	volatile HANDLE	m_h_thread;		
	volatile DWORD	m_u_thread_id;	


private:
	VOID ResetHandle()
	{
		if (m_h_thread)
		{
			CloseHandle(m_h_thread);
			m_h_thread = NULL;
		}

		m_u_thread_id = 0;
	}

public:
	CThreadBase()
	{
		m_h_thread	 = NULL;
		m_u_thread_id	 = 0;
	}

	virtual ~CThreadBase()
	{
		//Destroy(250, FALSE, 200);
	}

public:
	virtual int Create(DWORD uCreateFlags = 0, DWORD nStackSize = 0,
		LPVOID lpSecurityAttrs = NULL)
	{
		if (m_h_thread)		
			return -1;


		m_h_thread = (HANDLE)_beginthreadex(lpSecurityAttrs, nStackSize, 
			(ThreadFunc)ThreadHandle, this, uCreateFlags, 
			(unsigned int*)&m_u_thread_id);

		if (!m_h_thread)	
			return -3;

		return 0;
	}

	DWORD Destroy(DWORD uExitCode, BOOL bAutoDelete = TRUE,
		DWORD dwWaitTime = INFINITE)
	{
		if (!m_h_thread) return uExitCode;

		if (::WaitForSingleObject(m_h_thread, dwWaitTime) != WAIT_OBJECT_0)
		{
			TerminateThread((HANDLE)m_h_thread, uExitCode);
		}

		ResetHandle();

		if (bAutoDelete)
		{
			delete this;
		}

		return uExitCode;
	}

public:		
	inline int		GetPriority()
	{ return ::GetThreadPriority((HANDLE)m_h_thread); }

	inline BOOL		SetPriority(int nPriority)
	{ return (::SetThreadPriority((HANDLE)m_h_thread, nPriority) == TRUE); }

	inline DWORD	Suspend() { return ::SuspendThread(m_h_thread); }
	inline DWORD	Resume() { return ::ResumeThread(m_h_thread); }

	inline HANDLE	GetHandle() const { return m_h_thread; }
	inline DWORD	GetId() const { return m_u_thread_id; }

protected:
	virtual int	Run() {  return 0; }

private:
	static unsigned __stdcall ThreadHandle(LPVOID pParam)
	{
		CThreadBase* pThis = (CThreadBase*)pParam;
		assert (pThis != NULL);

		if (!pThis) return 1;
		int uRet = pThis->Run();
		if (uRet == ENUM_AUTO_DELETE_SELF) //标识子类要自我删除
		{
			return 0;
		}
		pThis->ResetHandle();
		return (unsigned)uRet ;
	}

public:
	inline BOOL  IsAlive()
	{
		return (m_h_thread != NULL);
	}

	inline BOOL  IsCurrentThread()
	{
		return (m_u_thread_id == GetCurrentThreadId());
	}

	inline BOOL  IsValid () 
	{
		return (m_h_thread != NULL);
	}

	inline void ClearStatus()
	{
		ResetHandle();
	}

	static LPVOID	GetCurrentThread() { return ::GetCurrentThread(); } 
	static DWORD	GetCurrentThreadId() { return ::GetCurrentThreadId(); }
};

//------------------------------------------------------------------------
#endif


