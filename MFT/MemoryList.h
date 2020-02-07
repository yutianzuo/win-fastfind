#pragma once

#define MEMORY_LIST_DEAULT_ALLOC_SIZE           1024 * 1024 * 1
class CMemoryList
{
public:
    CMemoryList(ULONG ul_default_allocate_len = MEMORY_LIST_DEAULT_ALLOC_SIZE)
    {
        m_n_default_alloc_len = ul_default_allocate_len;
        m_p_mem = NULL;
    }

    ~CMemoryList()
    {
        ReleaseAll();
    }

    BYTE* GetPointer(ULONG len)
    {
        for (size_t i = 0; i < m_vec_mem_list.size(); ++i)
        {
            if (m_vec_mem_list[i].m_n_offset + len <= m_n_default_alloc_len)
            {
                BYTE* p_tmp = m_vec_mem_list[i].m_p_pointer + m_vec_mem_list[i].m_n_offset;
                m_vec_mem_list[i].m_n_offset += len;
                return p_tmp;
            }
        }        
        MEMORY_POINT_INFO info_tmp = {0};
        info_tmp.m_p_pointer = new (std::nothrow) BYTE[m_n_default_alloc_len];
        if (info_tmp.m_p_pointer)
        {
            memset(info_tmp.m_p_pointer, 0, m_n_default_alloc_len);
            if (len > m_n_default_alloc_len)
            {
#ifdef  _DEBUG
                DebugBreak();
#endif
            }
            BYTE* p_tmp = info_tmp.m_p_pointer + info_tmp.m_n_offset;
            info_tmp.m_n_offset += len;
            m_vec_mem_list.push_back(info_tmp);
            return p_tmp;
        }

        return NULL;
    }

    void ReleaseAll()
    {
        for (size_t i = 0; i < m_vec_mem_list.size(); ++i)
        {
            delete [] m_vec_mem_list[i].m_p_pointer;
        }
		{
			std::vector<MEMORY_POINT_INFO> vec_tmp;
			m_vec_mem_list.swap(vec_tmp);
		}
        m_vec_mem_list.clear();
    }
protected:
private:
    ULONG m_n_default_alloc_len;
    BYTE* m_p_mem;

    typedef struct
    {
        BYTE* m_p_pointer;
        ULONG m_n_offset;
    }MEMORY_POINT_INFO;
    std::vector<MEMORY_POINT_INFO> m_vec_mem_list;
};