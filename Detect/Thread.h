#ifndef _THREAD_H__
#define _THREAD_H__

namespace util{
	/*  �¼������� */
	typedef BOOL  (*LP_FUN_HANDLEEVENT)(LPVOID lpVoid );
	/* �߳� */
	class CThread
	{
	public:
		CThread();
		CThread( LP_FUN_HANDLEEVENT fun , LPVOID pVoid = NULL );
		~CThread();
		/*
		  pVoid   : OnHandleEvent����
		  bManual ���Ƿ���Ҫ�ֶ�����OnHandleEventִ��
		*/
		BOOL Start( LPVOID pVoid,BOOL bManual = FALSE );
		BOOL Stop( DWORD  dwWaitTime = 2000 );
		BOOL Do();
		VOID SetTimeOut( DWORD  dwTimeOut );
		/*  -1��0��1 �ͣ���������*/
		VOID SetPriority( INT  nLevel = 0 );
	public:
		/* ����3�������� ����ֵ��ʾ�Ƿ��˳��߳� ��FALSE=�˳�*/
		virtual BOOL OnError   ( LPVOID pVoid );
		virtual BOOL OnTimeOut ( LPVOID pVoid );
		virtual BOOL OnHandleEvent ( LPVOID pVoid );		
		/*  ��ʼ�����߳� �����������߳�ǰ���еĵ���  ������FALSE=�˳�**/
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