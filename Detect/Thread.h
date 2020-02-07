#ifndef _THREAD_H__
#define _THREAD_H__

namespace util{
	/*  事件处理函数 */
	typedef BOOL  (*LP_FUN_HANDLEEVENT)(LPVOID lpVoid );
	/* 线程 */
	class CThread
	{
	public:
		CThread();
		CThread( LP_FUN_HANDLEEVENT fun , LPVOID pVoid = NULL );
		~CThread();
		/*
		  pVoid   : OnHandleEvent参数
		  bManual ：是否需要手动激发OnHandleEvent执行
		*/
		BOOL Start( LPVOID pVoid,BOOL bManual = FALSE );
		BOOL Stop( DWORD  dwWaitTime = 2000 );
		BOOL Do();
		VOID SetTimeOut( DWORD  dwTimeOut );
		/*  -1，0，1 低，正常，高*/
		VOID SetPriority( INT  nLevel = 0 );
	public:
		/* 下面3函数函数 返回值表示是否退出线程 ，FALSE=退出*/
		virtual BOOL OnError   ( LPVOID pVoid );
		virtual BOOL OnTimeOut ( LPVOID pVoid );
		virtual BOOL OnHandleEvent ( LPVOID pVoid );		
		/*  初始启动线程 ，正常结束线程前进行的调用  ，返回FALSE=退出**/
		virtual BOOL OnInitThread ( LPVOID pVoid );
		virtual VOID OnExitThread ( LPVOID pVoid );
	private:
		static  DWORD WINAPI ThreadProc(LPVOID pVoid);
	protected:
		BOOL    m_bRuning ;
		HANDLE	m_hWorkEvent;	
		HANDLE	m_hThread;
		DWORD	m_dwThreadID;
		DWORD	m_dwTimeout;	

		LPVOID  m_lpVoid ;
		LP_FUN_HANDLEEVENT   m_lpOutHandle ;
	};

}
#endif