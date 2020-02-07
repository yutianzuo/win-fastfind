#pragma once
#include "mft/ThreadBase.h"

#define  WM_MESSAGE_1               WM_USER + 40
#define  WM_MESSAGE_2               WM_USER + 41
#define  WM_MESSAGE_3               WM_USER + 42
#define  WM_MESSAGE_4				WM_USER + 43

#include "./MFT/MemoryList.h"

class CFindThread : public CThreadBase
{
public:
    CFindThread(std::vector<CString>& vec_disks, std::vector<CEnumMFT*>& vec_mfts) : m_vec_disks(vec_disks), m_vec_volums(vec_mfts)
    {
        m_h_event = ::CreateEvent(NULL, TRUE, FALSE, NULL);
        m_b_insearch = false;
		m_p_mem_pool = NULL;
		m_b_cancel_search = false;
    }
    void SetHWND(HWND wnd)
    {
        m_hWnd = wnd;
    }

    void Search(const stringxa& str_search, int index /*-1 means all disks*/, int n_state)
    {
        if (m_b_insearch)
        {
            return;
        }
        n_index = index;
        m_str_search = str_search;
        m_b_insearch = true;
        m_n_state = n_state;
        ::SetEvent(m_h_event);
    }

    void operator()(const stringxw& wstr_file)
    {
		if (m_b_cancel_search)
		{
			return;
		}
		if (g_cstr_foldername.GetLength() && wstr_file.find((LPCWSTR)g_cstr_foldername) != stringxw::npos)
		{
			 if (m_p_mem_pool)
			 {
				 WCHAR* wsz_tmp = (WCHAR*)m_p_mem_pool->GetPointer(wstr_file.size() * sizeof(WCHAR) + sizeof(WCHAR));
				 if (wsz_tmp)
				 {
					 wmemcpy_s(wsz_tmp, wstr_file.size() + 1, wstr_file.c_str(), wstr_file.size());
					 m_vec_buff.push_back(wsz_tmp);
				 }
			 }			 
		}
		else if (!g_cstr_foldername.GetLength())
		{
			if (m_p_mem_pool)
			{
				WCHAR* wsz_tmp = (WCHAR*)m_p_mem_pool->GetPointer(wstr_file.size() * sizeof(WCHAR) + sizeof(WCHAR));
				if (wsz_tmp)
				{
					wmemcpy_s(wsz_tmp, wstr_file.size() + 1, wstr_file.c_str(), wstr_file.size());
					m_vec_buff.push_back(wsz_tmp);
				}
			}
		}
		if (m_vec_buff.size() >= 5000 * m_n_search_count)
		{
			++m_n_search_count;
			if (m_n_search_count <= 2)
			{
				::SendMessage(m_hWnd, WM_MESSAGE_2, 0, (LPARAM)(m_vec_buff.size()));
			}
		}
    }

	stringxw GetSearchResult(int n_index)
	{
		stringxw wstr_ret;
		if (n_index < m_vec_buff.size() && m_vec_buff[n_index])
		{
			wstr_ret = m_vec_buff[n_index];
		}
		return wstr_ret;
	}

	void ClearCache()
	{
		std::vector<WCHAR*> vec_dummy;
		m_vec_buff.swap(vec_dummy);
		if (m_p_mem_pool)
		{
			m_p_mem_pool->ReleaseAll();
			delete m_p_mem_pool;
			m_p_mem_pool = NULL;
		}
	}

	std::vector<WCHAR*>& GetSearchAllData()
	{
		return m_vec_buff;
	}

	void CancelSearch()
	{
		m_b_cancel_search = true;
	}
protected:
    int Run()
    {
        int n_being = 0;
        int n_volums = m_vec_volums.size();
  #ifdef _DEBUG
//           n_being = 0;
//           n_volums = 1;
  #endif
        for (int i = n_being; i < n_volums; ++i)
        {
            m_vec_volums[i]->LoadAllFiles();
        }
        ::PostMessage(m_hWnd, WM_MESSAGE_1, 0, 0);		
        while (true)
        {
            ::WaitForSingleObject(m_h_event, INFINITE);
			::PostMessage(m_hWnd, WM_MESSAGE_4, 0, 0);
			m_b_cancel_search = false;
			ClearCache();
			m_vec_buff.reserve(1000000);
			m_n_search_count = 1;
			if (!m_p_mem_pool)
			{
				m_p_mem_pool = new (std::nothrow) CMemoryList(MEMORY_LIST_DEAULT_ALLOC_SIZE * 5);
			}
            if (-1 == n_index)
            {
                for (size_t i = n_being; i < n_volums; ++i)
                {
                    if (m_str_search == ".listallfile")
                    {
                        m_vec_volums[i]->ListAllFiles(*this);
                    }
                    else
                    {
                        m_vec_volums[i]->SimpleFindFile(m_str_search.c_str(), *this, m_n_state);
                    }
                }
            }
            else
            {
                if (m_str_search == ".listallfile")
                {
                    m_vec_volums[n_index]->ListAllFiles(*this);
                }
                else
                {
                    m_vec_volums[n_index]->SimpleFindFile(m_str_search.c_str(), *this, m_n_state);
                }
            }
 			if (m_vec_buff.size() && !m_b_cancel_search)
 			{
 				::SendMessage(m_hWnd, WM_MESSAGE_2, 0, (LPARAM)(m_vec_buff.size()));
 			}
            ::PostMessage(m_hWnd, WM_MESSAGE_3, 0, (LPARAM)(m_vec_buff.size()));
            ::ResetEvent(m_h_event);
            m_b_insearch = false;
        }
		ClearCache();
        return 0;
    }
private:
    std::vector<CString>& m_vec_disks;
    std::vector<CEnumMFT*>& m_vec_volums;
    HWND m_hWnd;
    HANDLE m_h_event;
    stringxa m_str_search;
    int n_index;
    bool m_b_insearch;
    std::vector<WCHAR*> m_vec_buff;
    int m_n_state;
	int m_n_search_count;

	bool m_b_cancel_search;

	CMemoryList* m_p_mem_pool;
};