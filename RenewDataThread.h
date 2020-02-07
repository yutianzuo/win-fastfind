#pragma once 

#include "mft/ThreadBase.h"

class CRnewDataThread : public CThreadBase
{
public:
	enum{MSG_DATA_UPDATE_DONE = 101,};
public:
	explicit CRnewDataThread(std::vector<CEnumMFT*>& vec_mfts, HWND h_wnd) : m_vec_volums(vec_mfts), m_hwnd(h_wnd)
	{

	}

	~CRnewDataThread(){}

protected:
	int Run()
	{
		for (std::size_t i = 0; i < m_vec_volums.size(); ++i)
		{
#ifdef _DEBUG
			if (i != 0) /*只用E盘测试*/
			{
				continue;
			}
#endif

			if (m_vec_volums[i])
			{
				m_vec_volums[i]->RenewAllData();
			}
		}
#ifdef _FAST_FIND_LOGMOD_
		::OutputDebugStringW(L"更新数据完毕\r\n");
#endif
		::PostMessage(m_hwnd, MSG_DATA_UPDATE_DONE, NULL, NULL);

		ClearStatus();
		delete this;
		return CThreadBase::ENUM_AUTO_DELETE_SELF;
	}

private:
	HWND m_hwnd;
	std::vector<CEnumMFT*>& m_vec_volums;
};