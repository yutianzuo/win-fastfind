#include "stdafx.h"
#include "Thread.h"
namespace util{
	CThread::CThread()
	{
		m_bRuning     = FALSE;
		m_hWorkEvent  = NULL ;
		m_hThread     = NULL ;
		m_dwThreadID  = 0    ;
		m_dwTimeout   = INFINITE;
		m_lpVoid      = NULL ;
		m_lpOutHandle = NULL ;
	}
	CThread::CThread( LP_FUN_HANDLEEVENT fun , LPVOID pVoid  )
	{
		m_bRuning     = FALSE;
		m_hWorkEvent  = NULL ;
		m_hThread     = NULL ;
		m_dwThreadID  = 0    ;
		m_dwTimeout   = INFINITE;
		m_lpVoid      = pVoid ;
		m_lpOutHandle = fun ;
	}
	CThread::~CThread()
	{
		Stop ();
	}
	BOOL CThread::Start (  LPVOID pVoid ,BOOL bManual )
	{
		if  ( m_hWorkEvent )
		{
			Stop ();
		}
		m_lpVoid     = pVoid;
		m_bRuning    = TRUE;
		m_hWorkEvent = ::CreateEvent( NULL,bManual,FALSE,NULL);
		m_hThread    = ::CreateThread(NULL,0,ThreadProc,this,0,&m_dwThreadID);
		return ( m_hThread != NULL );
	}
	VOID  CThread::SetTimeOut ( DWORD  dwTimeOut )
	{
		m_dwTimeout = dwTimeOut;		
	}
	VOID  CThread::SetPriority ( INT  nLevel  )
	{
		if ( m_hThread )
		{
			if  ( nLevel == -1  )
			   ::SetThreadPriority( m_hThread ,THREAD_PRIORITY_BELOW_NORMAL);
			else if  ( nLevel == 0  )
			   ::SetThreadPriority( m_hThread ,THREAD_PRIORITY_NORMAL);
			else if  ( nLevel == 1  )
				::SetThreadPriority( m_hThread ,THREAD_PRIORITY_ABOVE_NORMAL);

			else if  ( nLevel == -2  )
				::SetThreadPriority( m_hThread ,THREAD_PRIORITY_LOWEST);							
			else if  ( nLevel == 2  )
				::SetThreadPriority( m_hThread ,THREAD_PRIORITY_HIGHEST);

		}
	}
	BOOL CThread::Stop  ( DWORD  dwWaitTime  )
	{
		m_bRuning = FALSE;

		if  ( 0 !=  m_dwThreadID )
		{
			//是否在线程内部调用
			if  ( ::GetCurrentThread() == m_hThread)
			{
				::ExitThread( 0 );
			}
			else
			{
				::SetEvent( m_hWorkEvent );
				if (::WaitForSingleObject( m_hThread, dwWaitTime ) != WAIT_OBJECT_0)
				{	
					::TerminateThread(m_hThread,0) ;
				}     
			}
		}
		if  ( m_hWorkEvent )
		   ::CloseHandle( m_hWorkEvent );
		m_hWorkEvent = NULL ;
	    
		m_hThread    = NULL ;
		m_dwThreadID = 0;	

		return TRUE;
	}
	BOOL  CThread::Do()
	{
		if  ( !m_hWorkEvent )  return FALSE;

		::SetEvent( m_hWorkEvent );
		return TRUE;
	}

	BOOL CThread::OnError( LPVOID pVoid )
	{
		return TRUE;
	}
	BOOL CThread::OnTimeOut( LPVOID pVoid )
	{
		return TRUE;
	}
	BOOL CThread::OnHandleEvent ( LPVOID pVoid )
	{
		if( m_lpOutHandle )
		{
			return m_lpOutHandle(pVoid);
		}

		::Sleep( 1000 );
		return TRUE;
	}	
	/*  初始启动线程 ，正常结束线程前进行的调用  ，返回FALSE=退出**/
	BOOL CThread::OnInitThread ( LPVOID pVoid )
	{
		return TRUE;
	}
	VOID CThread::OnExitThread ( LPVOID pVoid )
	{}
	DWORD WINAPI CThread::ThreadProc(LPVOID pVoid)
	{
		CThread* pThis = (CThread*)pVoid;

		if ( !pThis->OnInitThread( pThis->m_lpVoid ) )
		{
			pThis->Stop( 0 );
			pThis->OnExitThread( pThis->m_lpVoid);
			return 0;
		}

		BOOL  bRuning = TRUE;
		while ( pThis->m_bRuning && bRuning )
		{			
			DWORD  dwRet = ::WaitForSingleObject( pThis->m_hWorkEvent,pThis->m_dwTimeout );
			if  (  WAIT_TIMEOUT == dwRet )
			{
				bRuning = pThis->OnTimeOut( pThis->m_lpVoid );
			}
			else if  (  WAIT_OBJECT_0 == dwRet ) 
			{
				if ( !pThis->m_bRuning  ) break;
				bRuning = pThis->OnHandleEvent( pThis->m_lpVoid );
			}
			else
			{
				bRuning = pThis->OnError( pThis->m_lpVoid );
			}
		}

		pThis->OnExitThread( pThis->m_lpVoid);

		pThis->m_dwThreadID = 0 ;

		return 0;
	}

}


