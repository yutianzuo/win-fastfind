#ifndef  _SYSTEM_INFO_H_
#define _SYSTEM_INFO_H_
namespace sysInfo
{
	//获取cpu使用率
	int GetCpuUsgae();
	//内存使用率
	int GetMemoryUsage();
	//鼠标，键盘没操作过的时间
	int GetLastInputTime();	

	//注册满足空闲条件的消息
	class CRegeditIdleMsg
	{
		CRegeditIdleMsg();
		~CRegeditIdleMsg();
	public:
		static CRegeditIdleMsg* GetInstance(); 				
		//获取无输入的极限值
		int GetLastInputLimit();
		int GetCpuLimit();
		int GetMemoryLimit();
		void SetLastInputLimit(int nVal);
		void SetCpuLimit(int nVal);
		void SetMemoryLimit(int nVal);

		//注册消息,反注册消息
		BOOL Regedit(HWND hWnd,DWORD dwID );
		VOID UnRegedit();
		//通知外部
		BOOL Notify();	
	protected:
		HWND m_hWnd;
		DWORD m_dwID;
		LPVOID m_dwData;
		int m_nLastInputLimit;
		int m_nCpuLimit;
		int m_nMemoryLimit;
	};
}
#endif