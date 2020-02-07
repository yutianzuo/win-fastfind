#include "stdafx.h"
#include "SystemInfo.h"
#include "Thread.h"

namespace sysInfo
{
	DWORD CaclulateCPUPercent ()
	{
		static UINT SystemBasicInformation = 0 ; 
		static UINT SystemPerformanceInformation = 2 ; 
		static UINT SystemTimeInformation = 3 ;

        #define Li2Double(x) ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart))

		typedef struct 
		{ 
			DWORD dwUnknown1; 
			ULONG uKeMaximumIncrement; 
			ULONG uPageSize; 
			ULONG uMmNumberOfPhysicalPages; 
			ULONG uMmLowestPhysicalPage; 
			ULONG uMmHighestPhysicalPage; 
			ULONG uAllocationGranularity; 
			PVOID pLowestUserAddress; 
			PVOID pMmHighestUserAddress; 
			ULONG uKeActiveProcessors; 
			BYTE bKeNumberProcessors; 
			BYTE bUnknown2; 
			WORD wUnknown3; 
		} SYSTEM_BASIC_INFORMATION;

		typedef struct 
		{ 
			LARGE_INTEGER liIdleTime; 
			DWORD dwSpare[152]; 
		} SYSTEM_PERFORMANCE_INFORMATION;

		typedef struct 
		{ 
			LARGE_INTEGER liKeBootTime; 
			LARGE_INTEGER liKeSystemTime; 
			LARGE_INTEGER liExpTimeZoneBias; 
			ULONG uCurrentTimeZoneId; 
			DWORD dwReserved; 
		} SYSTEM_TIME_INFORMATION;


		// ntdll!NtQuerySystemInformation (NT specific!) 
		// 
		// The function copies the system information of the 
		// specified type into a buffer 
		// 
		// NTSYSAPI 
		// NTSTATUS 
		// NTAPI 
		// NtQuerySystemInformation( 
		// IN UINT SystemInformationClass, // information type 
		// OUT PVOID SystemInformation, // pointer to buffer 
		// IN ULONG SystemInformationLength, // buffer size in bytes 
		// OUT PULONG ReturnLength OPTIONAL // pointer to a 32-bit 
		// // variable that receives 
		// // the number of bytes 
		// // written to the buffer 
		// ); 
		typedef LONG (WINAPI *PROCNTQSI)(UINT,PVOID,ULONG,PULONG);

		PROCNTQSI NtQuerySystemInformation;

		SYSTEM_PERFORMANCE_INFORMATION SysPerfInfo; 
		SYSTEM_TIME_INFORMATION SysTimeInfo; 
		SYSTEM_BASIC_INFORMATION SysBaseInfo; 
		double dbIdleTime; 
		double dbSystemTime; 
		LONG status; 
		LARGE_INTEGER liOldIdleTime = {0,0}; 
		LARGE_INTEGER liOldSystemTime = {0,0};

		NtQuerySystemInformation = (PROCNTQSI)GetProcAddress(GetModuleHandleW(L"ntdll"),"NtQuerySystemInformation");

		if (!NtQuerySystemInformation) 
		{
			//PrintDbgInfo (("开始计算，如果失败的话就不会显示时间！111111111111111111")) ;
			return -1 ;
		}

		// get number of processors in the system 
		status = NtQuerySystemInformation(SystemBasicInformation,&SysBaseInfo,sizeof(SysBaseInfo),NULL); 
		if (status != NO_ERROR) 
		{
			//PrintDbgInfo (("开始计算，如果失败的话就不会显示时间！2222222222222222"));
			return -1 ;
		}

		for (int i = 0; i < 2; i++)
		{ 
			// get new system time 
			status = NtQuerySystemInformation(SystemTimeInformation,&SysTimeInfo,sizeof(SysTimeInfo),0); 
			if (status!=NO_ERROR) 
			{
				//PrintDbgInfo (("开始计算，如果失败的话就不会显示时间！33333333333333333333"));
				return -1 ;
			}

			// get new CPU's idle time 
			status =NtQuerySystemInformation(SystemPerformanceInformation,&SysPerfInfo,sizeof(SysPerfInfo),NULL); 
			if (status != NO_ERROR) 
			{
				//PrintDbgInfo (("开始计算，如果失败的话就不会显示时间！4444444444444444444444"));
				return -1 ;
			}

			// if it's a first call - skip it 
			if (liOldIdleTime.QuadPart != 0) 
			{ 
				// CurrentValue = NewValue - OldValue 
				dbIdleTime = Li2Double(SysPerfInfo.liIdleTime) - Li2Double(liOldIdleTime); 
				dbSystemTime = Li2Double(SysTimeInfo.liKeSystemTime) -

					Li2Double(liOldSystemTime);

				// CurrentCpuIdle = IdleTime / SystemTime 
				dbIdleTime = dbIdleTime / dbSystemTime;

				// CurrentCpuUsage% = 100 - (CurrentCpuIdle * 100) / NumberOfProcessors 
				dbIdleTime = 100.0 - dbIdleTime * 100.0 /

					(double)SysBaseInfo.bKeNumberProcessors + 0.5;

				CString  s ;
				s.Format (L"CPU占用：%d", (UINT)dbIdleTime) ;
				//PrintDbgInfo ((s));
				return (UINT)dbIdleTime ;
			}

			// store new CPU's idle and system time 
			liOldIdleTime = SysPerfInfo.liIdleTime; 
			liOldSystemTime = SysTimeInfo.liKeSystemTime;

			// wait one second 
			Sleep(1000); 			
		} 

		return -1 ;
	}
	//获取cpu使用率
	int GetCpuUsgae()
	{
		DWORD dwValue=CaclulateCPUPercent();
		return dwValue;
	}
	//内存使用率
	int GetMemoryUsage()
	{
		MEMORYSTATUS memoryStatus;
		::GlobalMemoryStatus( &memoryStatus );

		int   iLen = sizeof(memoryStatus.dwLength);
		DWORD dwMemory    = memoryStatus.dwMemoryLoad; 
		DWORD dwTotalPhys = memoryStatus.dwTotalPhys;  
		DWORD dwAvailPhys = memoryStatus.dwAvailPhys; 
		return (int)dwMemory;
	}
	//鼠标，键盘没操作过的时间
	int GetLastInputTime()
	{
		LASTINPUTINFO info={0};
		info.cbSize=sizeof(info);

		if(!::GetLastInputInfo(&info))
		{
			return 0;
		}

		DWORD dwCurTick=GetTickCount();
		if(dwCurTick<=info.dwTime)
			return 0;

		DWORD dwIdleTime=dwCurTick-info.dwTime;
		return  (int)dwIdleTime;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	class CMonitor :public util::CThread
	{
		CRegeditIdleMsg* m_pMsg;
	public:
		CMonitor(CRegeditIdleMsg* pMsg):m_pMsg(pMsg)
		{
			util::CThread::SetTimeOut(1000);	
		}
		~CMonitor(){}
		BOOL OnTimeOut ( LPVOID pVoid )
		{
			//鼠标，键盘没操作过的时间
			int nVal=GetLastInputTime();
            //TRACE(_T("idle=%d\r\n"),nVal);
			if(nVal<m_pMsg->GetLastInputLimit())
				return TRUE;
			if(GetMemoryUsage() > m_pMsg->GetMemoryLimit())
				return TRUE;
			if(GetCpuUsgae()>m_pMsg->GetCpuLimit())
				return TRUE;

			//连续发送3次
			const int MAX_COUNT=3;
			int nCount=MAX_COUNT;
			while( nCount>0&& (!m_pMsg->Notify()))
			{
				--nCount;
				::Sleep((MAX_COUNT-nCount)*100);
			}
			return FALSE;
		}
	};
	////////////////////////////////////////////////////////////////////////////////////////////////
	CRegeditIdleMsg::CRegeditIdleMsg():m_hWnd(NULL),m_dwID(NULL),m_dwData(NULL)
	{
		m_nLastInputLimit=10*1000;
		m_nCpuLimit=50;
		m_nMemoryLimit=30;
	}
	CRegeditIdleMsg::~CRegeditIdleMsg()
	{
		UnRegedit();
	}
	CRegeditIdleMsg* CRegeditIdleMsg::GetInstance()
	{
		static CRegeditIdleMsg reg;
		return &reg;
	}
	//获取无输入的极限值
	int CRegeditIdleMsg::GetLastInputLimit()
	{
		return m_nLastInputLimit;
	}
	int CRegeditIdleMsg::GetCpuLimit()
	{
		return m_nCpuLimit;
	}
	int CRegeditIdleMsg::GetMemoryLimit()
	{
	    return m_nMemoryLimit;
	}
	void CRegeditIdleMsg::SetLastInputLimit(int nVal)
	{
        m_nLastInputLimit=nVal;
	}
	void CRegeditIdleMsg::SetCpuLimit(int nVal)
	{
		m_nCpuLimit=nVal;
	}
	void CRegeditIdleMsg::SetMemoryLimit(int nVal)
	{
        m_nMemoryLimit=nVal;
	}
	//注册消息,反注册消息
	BOOL CRegeditIdleMsg::Regedit(HWND hWnd,DWORD dwID )
	{
		if(NULL==hWnd||
		   (!::IsWindow(hWnd)))
		   return FALSE;
	    
		UnRegedit();
		m_hWnd=hWnd;
		m_dwID=dwID;

		//启动线程
		CMonitor* pMon=new CMonitor(this);
		pMon->Start(this);
		m_dwData=pMon;
		return TRUE;
	}
	VOID CRegeditIdleMsg::UnRegedit()
	{
	    //结束线程
		if(NULL==m_hWnd)
			return ;
		if(m_dwData)
		{//结束线程
          CMonitor* pMon=(CMonitor*)m_dwData;
		  pMon->Stop();
		  delete pMon;
		}
		m_dwData=NULL;
	}
	//通知外部
	BOOL CRegeditIdleMsg::Notify()
	{
		 if(::IsWindow(m_hWnd))
		 {
			 return ::SendMessage(m_hWnd,m_dwID,0,0);
		 }
		 return FALSE;
	}

}