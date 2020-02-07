#ifndef  _SYSTEM_INFO_H_
#define _SYSTEM_INFO_H_
namespace sysInfo
{
	//��ȡcpuʹ����
	int GetCpuUsgae();
	//�ڴ�ʹ����
	int GetMemoryUsage();
	//��꣬����û��������ʱ��
	int GetLastInputTime();	

	//ע�����������������Ϣ
	class CRegeditIdleMsg
	{
		CRegeditIdleMsg();
		~CRegeditIdleMsg();
	public:
		static CRegeditIdleMsg* GetInstance(); 				
		//��ȡ������ļ���ֵ
		int GetLastInputLimit();
		int GetCpuLimit();
		int GetMemoryLimit();
		void SetLastInputLimit(int nVal);
		void SetCpuLimit(int nVal);
		void SetMemoryLimit(int nVal);

		//ע����Ϣ,��ע����Ϣ
		BOOL Regedit(HWND hWnd,DWORD dwID );
		VOID UnRegedit();
		//֪ͨ�ⲿ
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