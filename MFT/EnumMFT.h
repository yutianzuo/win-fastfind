#pragma once
#include "ntfs.h"
#include "miscs.h"
#include "MemoryList.h"
#include "atlrx.h"
#include <set>
#include <map>
#include "ThreadBase.h"
// #include <shlwapi.h>
// #pragma comment(lib, "shlwapi.lib")



class CEnumMFT : public CThreadBase
{
public:
	typedef struct
	{
		ULONG m_mft_parent_index;
		BOOL b_is_file;
		BYTE* m_wsz_name;
	}MFT_LIST_INFO;

    CEnumMFT()
    {
		m_version = 0; //标示目前硬盘缓存版本。
        hVolume = NULL;        
        BytesPerFileRecord = 0;
        memset(&bootb, 0, sizeof(bootb));
        MFT = NULL;
        memset(&m_cache_info, 0, sizeof(m_cache_info));
        m_b_is_ntfs = false;
		m_n_lastUSN = 0;
		m_n_USNid = 0;
		m_b_is_renewed = false;
    }

    ~CEnumMFT()
    {
        ClearAll();
    }

    void ClearAll()
    {
        if (hVolume)
        {
            ::CloseHandle(hVolume);
            hVolume = NULL;
        }
        if (MFT)
        {
            delete [] MFT;
            MFT = NULL;
        }
        m_vec_mtf.clear();
        m_mem_list.ReleaseAll();
        if (m_cache_info.g_pb)
        {
            delete [] m_cache_info.g_pb;
            m_cache_info.g_pb = NULL;
            m_cache_info.cache_lcn_begin = 0;
            m_cache_info.cache_lcn_count = 0;
            m_cache_info.cache_lcn_orl_begin = 0;
            m_cache_info.cache_lcn_total = 0;
        }
    }

    stringxw GetVolum()
    {
        return m_wstr_volum;
    }

    bool IsNTFS()
    {
        return m_b_is_ntfs;
    }

    void SetVolum(const wchar_t* wsz_volum)
    {
        if (wsz_volum && m_wstr_volum.empty())
        {
            m_wstr_volum = wsz_volum;
            m_wstr_volum.to_lower();
            m_wstr_volum.convert_code(m_str_volum_u8, CP_UTF8);
            stringxw wstr_volum = L"\\\\.\\";
            wstr_volum += m_wstr_volum;
            hVolume = CreateFile(
                wstr_volum.c_str(),
                GENERIC_READ,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                0,
                OPEN_EXISTING,
                0,
                0);

            if (hVolume != INVALID_HANDLE_VALUE)
            {
                DWORD dw_sizeof_bpb = sizeof(bootb); 
                DWORD dw_read;
                if (ReadFile(hVolume, &bootb, dw_sizeof_bpb, &dw_read, 0) == 0)
                {
                    return;
                }
                if (bootb.Format[0] == L'N' && bootb.Format[1] == L'T' && bootb.Format[2] == L'F' && bootb.Format[3] == L'S')
                {
                    m_b_is_ntfs = true;
                }
                else
                {
                    ::CloseHandle(hVolume);
                    hVolume = NULL;
                }
            }
        }
    }

    int LoadAllFiles()
    {
        int n_ret = 0;
        FUNCTION_BEGIN;
        if (m_vec_mtf.size() || !m_b_is_ntfs)
        {
            FUNCTION_LEAVE;
        }
        BytesPerFileRecord = bootb.ClustersPerFileRecord < 0x80
            ? bootb.ClustersPerFileRecord * bootb.SectorsPerCluster
            * bootb.BytesPerSector: 1 << (0x100 - bootb.ClustersPerFileRecord);
        MFT = PFILE_RECORD_HEADER(new UCHAR[BytesPerFileRecord]);
        ReadSector((bootb.MftStartLcn)*(bootb.SectorsPerCluster), (BytesPerFileRecord)/(bootb.BytesPerSector), MFT);
        FixupUpdateSequenceArray(MFT);
        FindFiles();

		m_lock.lock();
        if (m_cache_info.g_pb)
        {
            delete [] m_cache_info.g_pb;
            m_cache_info.g_pb = NULL;
            m_cache_info.cache_lcn_begin = 0;
            m_cache_info.cache_lcn_count = 0;
            m_cache_info.cache_lcn_orl_begin = 0;
            m_cache_info.cache_lcn_total = 0;
        }
		m_lock.unlock();

// 		if (hVolume)
// 		{
// 			::CloseHandle(hVolume);
// 			hVolume = NULL;
// 		}
		if (MFT)
		{
			delete [] MFT;
			MFT = NULL;
		}
		__super::Create(); //启动实时监控线程
        FUNCTION_END;
        return n_ret;
    }

	std::vector<MFT_LIST_INFO>* GetVolumPointer()
	{
		return &m_vec_mtf;
	}

	void WriteLocalCache()
	{
#ifdef _FAST_FIND_LOGMOD_
		DWORD dw_begin = ::GetTickCount();
#endif
		_WriteCache();
#ifdef _FAST_FIND_LOGMOD_
		stringxw wstr_output;
		wstr_output.format(L"写缓存用时：%d毫秒", ::GetTickCount() - dw_begin);
		::OutputDebugStringW(wstr_output);
#endif
	}

    template<class T>
    void SimpleFindFile(/*u8*/const char* wsz_find, T& func, /*bool b_only_file = false*/int n_state)
    {
		m_lock.lock();
        if (!wsz_find)
        {
			m_lock.unlock();
            return;
        }
        stringxa str_find(wsz_find);
        bool b_use_strstr = false;
        if (stringxa::npos == str_find.find('*') && stringxa::npos == str_find.find('?'))
        {
            b_use_strstr = true;
        }
        int n_wildcard = 0;
        stringxw wstr_name;
        stringxa str_u8_name;
        stringxw wstr_find;
        str_find.convert_code(wstr_find, CP_UTF8);

        if (!b_use_strstr && stringxa::npos == str_find.find('?'))
        {
            str_find.trim_left('*');
            if (stringxa::npos == str_find.find('*'))
            {
                n_wildcard = 1;
            }
            else
            {
                str_find.trim_right('*');
                if (stringxa::npos == str_find.find('*'))
                {
                    n_wildcard = 2;
                }
            }
        }

        for (size_t i = 0; i < m_vec_mtf.size(); ++i)
        {
            if (!m_vec_mtf[i].m_wsz_name)
            {
                continue;
            }
            if (0 != n_state)
            {
				if (1 == n_state)
				{
					if (!m_vec_mtf[i].b_is_file)
					{
						continue;
					}
				}
				else if (2 == n_state)
				{
					if (m_vec_mtf[i].b_is_file)
					{
						continue;
					}
				}
            }
            char* wsz_result = NULL;
            if (b_use_strstr)
            {
               wsz_result = strstr((char*)m_vec_mtf[i].m_wsz_name, wsz_find);
            }
            else
            {
                if (0 == n_wildcard)
                {
                    str_u8_name.assign((char*)m_vec_mtf[i].m_wsz_name);
                    str_u8_name.convert_code(wstr_name, CP_UTF8);
                    BOOL b_match = PathMatchSpec(wstr_name.c_str(), wstr_find.c_str());
                    if (b_match)
                    {
                        wsz_result = (char*)0x00000001;
                    }
                }
                else if (1 == n_wildcard)
                {
                    wsz_result = SimpleWildCard((char*)m_vec_mtf[i].m_wsz_name, str_find.c_str());
                }
                else if (2 == n_wildcard)
                {
                    wsz_result = strstr((char*)m_vec_mtf[i].m_wsz_name, str_find.c_str());
                }


//                  CAtlRegExp<CAtlRECharTraitsA> re_find;
//                  REParseError status = re_find.Parse(wsz_find);
//                  if (REPARSE_ERROR_OK != status)
//                  {
//                      continue;
//                  }
//                  CAtlREMatchContext<CAtlRECharTraitsA> mc_str;
//                  if (!re_find.Match((char*)m_vec_mtf[i].m_wsz_name, &mc_str))
//                  {
//                      continue;
//                  }
//                  wsz_result = (char*)0x00000001;
            }
            if (wsz_result)
            {
                std::string wstr_fullpath;
                wstr_fullpath = (char*)m_vec_mtf[i].m_wsz_name;
                ULONG ul_parent = m_vec_mtf[i].m_mft_parent_index;
				bool b_has_broken_chain = false;
                while (ul_parent != 5 && ul_parent < m_vec_mtf.size()) //5是根目录，即“.”
                {
                    if (!m_vec_mtf[ul_parent].m_wsz_name)
                    {
#ifdef _FAST_FIND_LOGMOD_
                        stringxw wstr_output;
                        wstr_output.format(L"出现父目录为空情况，索引号%d，此MFT项父索引为%d，自己记录索引为%d，是否为文件/文件夹 %d", ul_parent, m_vec_mtf[ul_parent].m_mft_parent_index, /*m_vec_mtf[ul_parent].m_mft_index*/ul_parent, m_vec_mtf[ul_parent].b_is_file);
                        ::OutputDebugStringW(wstr_output);
                        stringxa str_output;
                        str_output.format("fullpath打印为%s", wstr_fullpath.c_str());
                        ::OutputDebugStringA(str_output);
#endif
						b_has_broken_chain = true;
                        break;
                    }
                    wstr_fullpath = stringxa((char*)m_vec_mtf[ul_parent].m_wsz_name) + "\\" + wstr_fullpath;
                    ul_parent = m_vec_mtf[ul_parent].m_mft_parent_index;
                }
				if (b_has_broken_chain)
				{
#ifdef _FAST_FIND_LOGMOD_
					::OutputDebugStringW(L"****遇到断链情况****");
#endif
					continue;
				}
                wstr_fullpath = m_str_volum_u8 + "\\" + wstr_fullpath;
                stringxw wstr_fullpath_unicode(wstr_fullpath.c_str(), CP_UTF8);
                func(wstr_fullpath_unicode);
            }
        }
		m_lock.unlock();
    }

    template<class T>
    void ListAllFiles(T& func) //TODO:正式发行要屏蔽
    {
        for (size_t i = 0; i < m_vec_mtf.size(); ++i)
        {
            if (!m_vec_mtf[i].m_wsz_name || !m_vec_mtf[i].b_is_file)
            {
                continue;
            }       
            std::string wstr_fullpath;
            wstr_fullpath = (char*)m_vec_mtf[i].m_wsz_name;
            ULONG ul_parent = m_vec_mtf[i].m_mft_parent_index;
            while (ul_parent != 5 && ul_parent < m_vec_mtf.size()) //5是根目录，即“.”
            {
                if (!m_vec_mtf[ul_parent].m_wsz_name)
                {
#ifdef _DEBUG
                    stringxw wstr_output;
                    wstr_output.format(L"出现父目录为空情况，索引号%d，此MFT项父索引为%d，自己记录索引为%d，是否为文件/文件夹 %d", ul_parent, m_vec_mtf[ul_parent].m_mft_parent_index, /*m_vec_mtf[ul_parent].m_mft_index*/ul_parent, m_vec_mtf[ul_parent].b_is_file);
                    ::OutputDebugStringW(wstr_output);
                    stringxa str_output;
                    str_output.format("fullpath打印为%s", wstr_fullpath.c_str());
                    ::OutputDebugStringA(str_output);
#endif
                    break;
                }
                wstr_fullpath = stringxa((char*)m_vec_mtf[ul_parent].m_wsz_name) + "\\" + wstr_fullpath;
                ul_parent = m_vec_mtf[ul_parent].m_mft_parent_index;
            }
            wstr_fullpath = m_str_volum_u8 + "\\" + wstr_fullpath;
            stringxw wstr_fullpath_unicode(wstr_fullpath.c_str(), CP_UTF8);
            func(wstr_fullpath_unicode);
        }
    }

	void RenewAllData()
	{
		m_lock.lock();
		if (m_cache_info.g_pb)
		{
			delete [] m_cache_info.g_pb;
			m_cache_info.g_pb = NULL;
			m_cache_info.cache_lcn_begin = 0;
			m_cache_info.cache_lcn_count = 0;
			m_cache_info.cache_lcn_orl_begin = 0;
			m_cache_info.cache_lcn_total = 0;
		}
		{
			std::vector<MFT_LIST_INFO> vec_tmp;
			m_vec_mtf.swap(vec_tmp);
		}
		m_mem_list.ReleaseAll();

		m_n_lastUSN = 0;
		LONGLONG usnid = 0x11;
		RenewUSNAndUSNID(usnid); /*重置lastUSN和USNid*/
		m_n_USNid = usnid;


        MFT = PFILE_RECORD_HEADER(new UCHAR[BytesPerFileRecord]);
        ReadSector((bootb.MftStartLcn)*(bootb.SectorsPerCluster), (BytesPerFileRecord)/(bootb.BytesPerSector), MFT);
        FixupUpdateSequenceArray(MFT);
		PATTRIBUTE attr = FindAttribute(MFT, AttributeBitmap, 0);
		PUCHAR bitmap = new UCHAR[AttributeLengthAllocated(attr)];
		ReadAttribute(attr, bitmap);//$MFT元文件中的$Bitmap属性，此属性中标识了$MFT元文件中MFT项的使用状况
		ULONG n = AttributeLength(FindAttribute(MFT, AttributeData, 0))/BytesPerFileRecord;
		//MTF的总项数
		m_vec_mtf.reserve(n + n/2);
		PFILE_RECORD_HEADER file = PFILE_RECORD_HEADER(new UCHAR[BytesPerFileRecord]);
		stringxw wstr_namebuff;
		stringxa str_namebuff_u8;
		for(ULONG i = 0; i < n; i++)
		{
			MFT_LIST_INFO mft_list_info_tmp = {0};
			m_vec_mtf.push_back(mft_list_info_tmp);
			if (!bitset(bitmap, i)) //跳过被删除的文件
				continue;

			ReadFileRecord(i, file);

			if (file->Ntfs.Type == 'ELIF')
			{
				attr = FindAttribute(file, AttributeFileName, 0);
				if (attr == 0)
					continue;

				PFILENAME_ATTRIBUTE name = PFILENAME_ATTRIBUTE(Padd(attr,PRESIDENT_ATTRIBUTE(attr)->ValueOffset));

				int n_ctrl = 1;
				PFILENAME_ATTRIBUTE name_lenname = NULL;
				PATTRIBUTE attr_lenname = NULL;
				BYTE file_type = name->NameType;
				while (file_type == 2) //只取长文件名
				{
					attr_lenname = FindAttribute(file, AttributeFileName, 0, n_ctrl);
					n_ctrl++;
					if (attr_lenname == 0)
						break;
					name_lenname = PFILENAME_ATTRIBUTE(Padd(attr_lenname,PRESIDENT_ATTRIBUTE(attr_lenname)->ValueOffset));
					file_type = name_lenname->NameType;
				}
				if (attr_lenname != 0) //如果没有长文件名，就取短文件名
				{
					attr = attr_lenname;
					name = name_lenname;
				}
				std::size_t n_index = m_vec_mtf.size() - 1;
				m_vec_mtf[n_index].m_mft_parent_index = (ULONG)name->DirectoryFileReferenceNumber;
				wstr_namebuff.assign(name->Name, name->NameLength);
				wstr_namebuff.to_lower();
				wstr_namebuff.convert_code(str_namebuff_u8, CP_UTF8);
				m_vec_mtf[n_index].m_wsz_name = m_mem_list.GetPointer(str_namebuff_u8.size() + 1);
				if (m_vec_mtf[n_index].m_wsz_name)
				{
					memcpy(m_vec_mtf[n_index].m_wsz_name, str_namebuff_u8.c_str(), str_namebuff_u8.size());
				}
				else
				{
#ifdef _DEBUG
					DebugBreak();
#endif
				}
				m_vec_mtf[n_index].b_is_file = ((file->Flags & 1) && (file->Flags & 2) == 0);				
			}
		}

		if (bitmap)
		{
			delete [] bitmap;
		}

		if (file)
		{
			delete [] file;
		}

		if (MFT)
		{
			delete [] MFT;
			MFT = NULL;
		}
		if (m_cache_info.g_pb)
		{
			delete [] m_cache_info.g_pb;
			m_cache_info.g_pb = NULL;
			m_cache_info.cache_lcn_begin = 0;
			m_cache_info.cache_lcn_count = 0;
			m_cache_info.cache_lcn_orl_begin = 0;
			m_cache_info.cache_lcn_total = 0;
		}
		m_b_is_renewed = true;
		m_lock.unlock();
	}



protected:
    char* SimpleWildCard(char* sz_u8, const char* sz_find) //处理了*.rmvb这样典型的查找，目的是为了最大限度提升速度,这里的第二参数应该由外面过滤“*”
    {
        if (!sz_u8 || !sz_find)
        {
            return NULL;
        }
        int n_src = strlen(sz_u8);
        int n_find = strlen(sz_find);
        if (n_find > n_src)
        {
            return NULL;
        }
        int n_index = n_find - 1;
        int n_index2 = n_src - 1;
        while (n_index >= 0)
        {
            if (sz_find[n_index] != sz_u8[n_index2])
            {
                return NULL;
            }
            --n_index;
            --n_index2;
        }
        return &sz_u8[++n_index2];
    }

protected:
    VOID ReadSector(ULONGLONG sector, ULONG count, PVOID buffer)
    {
        ULARGE_INTEGER offset;
        OVERLAPPED overlap = {0};
        ULONG n;
        offset.QuadPart = sector * bootb.BytesPerSector;
        overlap.Offset = offset.LowPart;
        overlap.OffsetHigh = offset.HighPart;
        ReadFile(hVolume, buffer, count * bootb.BytesPerSector, &n, &overlap);
    }

    VOID FixupUpdateSequenceArray(PFILE_RECORD_HEADER file)
    {
        ULONG i = 0;
        PUSHORT usa = PUSHORT(Padd(file, file->Ntfs.UsaOffset));
        PUSHORT sector = PUSHORT(file);
        for (i = 1; i < file->Ntfs.UsaCount; i++)
        {
            sector[255] = usa[i];
            sector += 256;
        }
    }

    template <class T1, class T2> inline T1* Padd(T1* p, T2 n)
    {
        return (T1*)((char *)p + n);
    }

    ULONG RunLength(PUCHAR run)
    {
        return (*run & 0xf) + ((*run >> 4) & 0xf) + 1;
    }

    LONGLONG RunLCN(PUCHAR run)
    {
        LONG i = 0;
        UCHAR n1 = 0 , n2 = 0;
        LONGLONG lcn = 0;

        n1 = *run & 0xf;
        n2 = (*run >> 4) & 0xf;

        lcn = n2 == 0 ? 0 : CHAR(run[n1 + n2]);

        for (i = n1 + n2 - 1; i > n1; i--)
            lcn = (lcn << 8) + run[i];
        return lcn;
    }

    ULONGLONG RunCount(PUCHAR run)
    {
        UCHAR n = *run & 0xf;
        ULONGLONG count = 0;
        ULONG i;


        for (i = n; i > 0; i--)
            count = (count << 8) + run[i];

        return count;
    }

    BOOL FindRun(PNONRESIDENT_ATTRIBUTE attr, ULONGLONG vcn, PULONGLONG lcn, PULONGLONG count)
    {
        PUCHAR run = NULL;
        *lcn = 0;
        ULONGLONG base = attr->LowVcn;

        if (vcn < attr->LowVcn || vcn > attr->HighVcn)
            return FALSE;

        for(run = PUCHAR(Padd(attr, attr->RunArrayOffset));   *run != 0;  run += RunLength(run))
        {
            *lcn += RunLCN(run);
            *count = RunCount(run);

            if (base <= vcn && vcn < base + *count)
            {
                *lcn = RunLCN(run) == 0 ? 0 : *lcn + vcn - base;
                *count -= ULONG(vcn - base);
                return TRUE;
            }
            else
                base += *count;
        }
        return FALSE;
    }

    PATTRIBUTE FindAttribute(PFILE_RECORD_HEADER file,ATTRIBUTE_TYPE type, PWSTR name, int n_attr_count = 0)
    {
        PATTRIBUTE attr = NULL;

        int n_ctrl = 0;

        for (attr = PATTRIBUTE(Padd(file, file->AttributesOffset));
            attr->AttributeType != -1;attr = Padd(attr, attr->Length))
        {
            if (attr->AttributeType == type)
            {
                if (n_attr_count != n_ctrl)
                {
                    n_ctrl++;
                    continue;
                }
                if (name == 0 && attr->NameLength == 0)
                    return attr;
                if (name != 0 && wcslen(name) == attr->NameLength && _wcsicmp(name,
                    PWSTR(Padd(attr, attr->NameOffset))) == 0)
                    return attr;
            }
        }
        return 0;
    }

    VOID ReadLCN(ULONGLONG lcn, ULONG count, PVOID buffer)
    {
        ReadSector(lcn * bootb.SectorsPerCluster,count * bootb.SectorsPerCluster, buffer);
    }

    ULONG AttributeLength(PATTRIBUTE attr)
    {
        return attr->Nonresident == FALSE ? PRESIDENT_ATTRIBUTE(attr)->ValueLength : ULONG(PNONRESIDENT_ATTRIBUTE(attr)->DataSize);
    }

    ULONG AttributeLengthAllocated(PATTRIBUTE attr)
    {
        return attr->Nonresident == FALSE ? PRESIDENT_ATTRIBUTE(attr)->ValueLength : ULONG(PNONRESIDENT_ATTRIBUTE(attr)->AllocatedSize);
    }

    VOID ReadAttribute(PATTRIBUTE attr, PVOID buffer)
    {
        PRESIDENT_ATTRIBUTE rattr = NULL;
        PNONRESIDENT_ATTRIBUTE nattr = NULL;

        if (attr->Nonresident == FALSE)
        {
            rattr = PRESIDENT_ATTRIBUTE(attr);
            memcpy(buffer, Padd(rattr, rattr->ValueOffset), rattr->ValueLength);
        }
        else
        {
            nattr = PNONRESIDENT_ATTRIBUTE(attr);
            ReadExternalAttribute(nattr, 0, ULONG(nattr->HighVcn) + 1, buffer);
        }
    }

    VOID ReadVCN(PFILE_RECORD_HEADER file, ATTRIBUTE_TYPE type,ULONGLONG vcn, ULONG count, PVOID buffer)
    {
        PATTRIBUTE attrlist = NULL;
        PNONRESIDENT_ATTRIBUTE attr = PNONRESIDENT_ATTRIBUTE(FindAttribute(file, type, 0));

        if (attr == 0 || (vcn < attr->LowVcn || vcn > attr->HighVcn))
        {
            attrlist = FindAttribute(file, AttributeAttributeList, 0);
            //DebugBreak();
            //TODO:基本遇不到这样的情况
        }
        ReadExternalAttribute(attr, vcn, count, buffer, true);
    }


    VOID ReadFileRecord(ULONG index, PFILE_RECORD_HEADER file)
    {
        ULONG clusters = bootb.ClustersPerFileRecord;

        if (clusters > 0x80)
            clusters = 1;

        PUCHAR p = new UCHAR[bootb.BytesPerSector* bootb.SectorsPerCluster * clusters];
        ULONGLONG vcn = ULONGLONG(index) * BytesPerFileRecord/bootb.BytesPerSector/bootb.SectorsPerCluster;
        ReadVCN(MFT, AttributeData, vcn, clusters, p);
        LONG m = (bootb.SectorsPerCluster * bootb.BytesPerSector/BytesPerFileRecord) - 1;
        ULONG n = m > 0 ? (index & m) : 0;
        memcpy(file, p + n * BytesPerFileRecord, BytesPerFileRecord);
        delete [] p;
        FixupUpdateSequenceArray(file);
    }

	VOID ReadVCN2(PFILE_RECORD_HEADER file, ATTRIBUTE_TYPE type,ULONGLONG vcn, ULONG count, PVOID buffer)
	{
		PATTRIBUTE attrlist = NULL;
		PNONRESIDENT_ATTRIBUTE attr = PNONRESIDENT_ATTRIBUTE(FindAttribute(file, type, 0));

		if (attr == 0 || (vcn < attr->LowVcn || vcn > attr->HighVcn))
		{
			attrlist = FindAttribute(file, AttributeAttributeList, 0);
			//DebugBreak();
			//TODO:基本遇不到这样的情况
		}
		ReadExternalAttribute(attr, vcn, count, buffer, false);
	}

	VOID ReadFileRecord2(ULONG index, PFILE_RECORD_HEADER file)
	{
		ULONG clusters = bootb.ClustersPerFileRecord;

		if (clusters > 0x80)
			clusters = 1;

		PUCHAR p = new UCHAR[bootb.BytesPerSector* bootb.SectorsPerCluster * clusters];
		ULONGLONG vcn = ULONGLONG(index) * BytesPerFileRecord/bootb.BytesPerSector/bootb.SectorsPerCluster;
		ReadVCN2(MFT, AttributeData, vcn, clusters, p);
		LONG m = (bootb.SectorsPerCluster * bootb.BytesPerSector/BytesPerFileRecord) - 1;
		ULONG n = m > 0 ? (index & m) : 0;
		memcpy(file, p + n * BytesPerFileRecord, BytesPerFileRecord);
		delete [] p;
		FixupUpdateSequenceArray(file);
	}

    // Non resident attributes
    VOID ReadExternalAttribute(PNONRESIDENT_ATTRIBUTE attr,ULONGLONG vcn, ULONG count, PVOID buffer, bool b_allow_cache = false)
    {
        ULONGLONG lcn, runcount;
        ULONG readcount, left;
        PUCHAR bytes = PUCHAR(buffer);

        for(left = count; left > 0; left -= readcount)
        {
            FindRun(attr, vcn, &lcn, &runcount);
            if (NeedCache(lcn) && b_allow_cache)
            {
                NewCache(lcn, runcount);
            }
            readcount = ULONG(min(runcount, left));
            ULONG n = readcount * bootb.BytesPerSector * bootb.SectorsPerCluster;

            if(lcn == 0)
                memset(bytes, 0, n);
            else
            {
                int n_ret = CanReadFromCache(lcn);
                if (n_ret >= 0)
                {
                    memcpy(bytes, m_cache_info.g_pb + ((lcn - m_cache_info.cache_lcn_begin) * bootb.SectorsPerCluster * bootb.BytesPerSector), readcount * bootb.SectorsPerCluster * bootb.BytesPerSector);
                }
                else
                {
                    ReadLCN(lcn, readcount, bytes);
                }
            }          
            vcn += readcount;
            bytes += n;
        }
    }

    bool NeedCache(ULONGLONG lcn)
    {
        if (!m_cache_info.g_pb)
        {
            return true;
        }
        if (lcn >= m_cache_info.cache_lcn_begin && lcn < m_cache_info.cache_lcn_begin + m_cache_info.cache_lcn_count)
        {
            return false;
        }
        return true;
    }

    void NewCache(ULONGLONG lcn, ULONGLONG lcn_count)
    {
        if (lcn < m_cache_info.cache_lcn_begin)
        {
            return;
        }
        if (m_cache_info.g_pb)
        {
            delete [] m_cache_info.g_pb;
            m_cache_info.g_pb = NULL;
        }
        ULONGLONG new_len;
        if (!m_cache_info.cache_lcn_orl_begin)
        {
            new_len = MAX_CACHE_SECTORS >= lcn_count ? lcn_count : MAX_CACHE_SECTORS;
            m_cache_info.cache_lcn_orl_begin = lcn;
            m_cache_info.cache_lcn_total = lcn_count;
        }
        else if (m_cache_info.cache_lcn_orl_begin && lcn > m_cache_info.cache_lcn_orl_begin + m_cache_info.cache_lcn_total)
        {
            new_len = MAX_CACHE_SECTORS >= lcn_count ? lcn_count : MAX_CACHE_SECTORS;
            m_cache_info.cache_lcn_orl_begin = lcn;
            m_cache_info.cache_lcn_total = lcn_count;
        }
        else
        {
            new_len = MAX_CACHE_SECTORS >= m_cache_info.cache_lcn_orl_begin + m_cache_info.cache_lcn_total - lcn ? m_cache_info.cache_lcn_orl_begin + m_cache_info.cache_lcn_total - lcn : MAX_CACHE_SECTORS;
        }
        BYTE* p_tmp = new (std::nothrow) BYTE[new_len * bootb.SectorsPerCluster * bootb.BytesPerSector];
        if (p_tmp)
        {
            m_cache_info.g_pb = p_tmp;
            m_cache_info.cache_lcn_begin = lcn;
            m_cache_info.cache_lcn_count = new_len;
            ReadLCN(lcn, new_len, m_cache_info.g_pb);
        }
    }

    int CanReadFromCache(ULONGLONG lcn)
    {
        if (m_cache_info.g_pb && lcn >= m_cache_info.cache_lcn_begin && lcn < m_cache_info.cache_lcn_begin + m_cache_info.cache_lcn_count)
        {
            return 1;
        }
        return -1;
    }
//////////////////////////////////////////////////////////////////////////

	USN RenewUSNAndUSNID(LONGLONG& usnid)
	{
		static const int g_len = 1024 * 1024;
		DWORD br;
		USN_JOURNAL_DATA qujd = {0};
		CREATE_USN_JOURNAL_DATA cujd = {1024 * 1024 * 100, 1024 * 1024 * 20};
		USN ret = 0;
		if (DeviceIoControl(hVolume, FSCTL_QUERY_USN_JOURNAL, NULL, 0, &qujd, sizeof(qujd), &br, NULL))
		{
			BYTE* buffer = new (std::nothrow) BYTE[g_len];
			if (buffer)
			{
				DWORD BytesReturned = 0;
				READ_USN_JOURNAL_DATA rujd = {m_n_lastUSN == 0 ? qujd.FirstUsn : m_n_lastUSN , 0xffffffff/*to get all reason*/, 0, 0, 0, qujd.UsnJournalID};
				memset(buffer, 0, g_len);
				DWORD cb = 0;
				while (TRUE) 
				{
					BOOL fOk = DeviceIoControl(hVolume, FSCTL_READ_USN_JOURNAL, &rujd, sizeof(rujd), buffer, g_len, &cb, NULL);
					if (!fOk || (cb <= sizeof(USN))) 
						break;

					rujd.StartUsn = * (USN*) buffer;

					// The first returned record is just after the first sizeof(USN) bytes
					USN_RECORD *pUsnRecord = (PUSN_RECORD) &buffer[sizeof(USN)];

					// Walk the output buffer
					while ((PBYTE) pUsnRecord < (buffer + cb))
					{
						m_n_lastUSN = pUsnRecord->Usn;
						ret = pUsnRecord->Usn;
						pUsnRecord = (PUSN_RECORD) 
							((PBYTE) pUsnRecord + pUsnRecord->RecordLength);
					}
				}
				if (::GetLastError() == ERROR_JOURNAL_ENTRY_DELETED)
				{
#ifdef _FAST_FIND_LOGMOD_
					::OutputDebugStringW(L"遇到读入USN项目被删除...");
#endif
					ret = 0;
				}
			}
			if (buffer)
			{
				delete [] buffer;
			}
			usnid = qujd.UsnJournalID;
		}
		else
		{
			DeviceIoControl(hVolume, FSCTL_CREATE_USN_JOURNAL, &cujd, sizeof(cujd), NULL, 0, &br, NULL);
			DeviceIoControl(hVolume, FSCTL_QUERY_USN_JOURNAL, NULL, 0, &qujd, sizeof(qujd), &br, NULL);
			usnid = qujd.UsnJournalID;
		}
		return ret;
	}




	USN EnumUsnRecord(LONGLONG& usnid)
	{
#define BUFF_LEN3	1024 * 1024
		DWORD br;
		USN_JOURNAL_DATA qujd = {0};
		CREATE_USN_JOURNAL_DATA cujd = {1024 * 1024 * 100, 1024 * 1024 * 20};
		USN ret = 0;
		if (DeviceIoControl(hVolume, FSCTL_QUERY_USN_JOURNAL, NULL, 0, &qujd, sizeof(qujd), &br, NULL))
		{
			if ((/*m_n_lastUSN == 0 || */(m_n_lastUSN >= qujd.FirstUsn && m_n_lastUSN <= qujd.NextUsn)))
			{
				BYTE* buffer = new (std::nothrow) BYTE[BUFF_LEN3];
				if (buffer)
				{
					DWORD BytesReturned = 0;
					READ_USN_JOURNAL_DATA rujd = {m_n_lastUSN == 0 ? qujd.FirstUsn : m_n_lastUSN , 0xffffffff/*to get all reason*/, 0, 0, 0, qujd.UsnJournalID};
					memset(buffer, 0, BUFF_LEN3);
					DWORD cb = 0;
					while (TRUE) 
					{
						BOOL fOk = DeviceIoControl(hVolume, FSCTL_READ_USN_JOURNAL, &rujd, sizeof(rujd), buffer, BUFF_LEN3, &cb, NULL);
						if (!fOk || (cb <= sizeof(USN))) 
							break;

						rujd.StartUsn = * (USN*) buffer;

						// The first returned record is just after the first sizeof(USN) bytes
						USN_RECORD *pUsnRecord = (PUSN_RECORD) &buffer[sizeof(USN)];

						// Walk the output buffer
						while ((PBYTE) pUsnRecord < (buffer + cb))
						{
							DWORDLONG mtf_index = pUsnRecord->FileReferenceNumber;
							if (pUsnRecord->Reason & USN_REASON_FILE_DELETE)
							{
								ULONG n_index = mtf_index & 0x0000FFFFFFFFFFFF;
								while (m_vec_mtf.size() <= n_index)
								{
									MFT_LIST_INFO mft_list_info_tmp = {0};
									m_vec_mtf.push_back(mft_list_info_tmp);
								}
								m_vec_mtf[n_index].m_mft_parent_index = 0;
								m_vec_mtf[n_index].m_wsz_name = NULL;
							}
							else if (pUsnRecord->Reason & USN_REASON_FILE_CREATE || pUsnRecord->Reason & USN_REASON_RENAME_NEW_NAME)
							{
								ULONG n_index = mtf_index & 0x0000FFFFFFFFFFFF;
								while (m_vec_mtf.size() <= n_index)
								{
									MFT_LIST_INFO mft_list_info_tmp = {0};
									m_vec_mtf.push_back(mft_list_info_tmp);
								}
								m_vec_mtf[n_index].m_mft_parent_index = pUsnRecord->ParentFileReferenceNumber & 0x0000FFFFFFFFFFFF;
								stringxw wstr_namebuff;
								stringxa str_namebuff_u8;
								wstr_namebuff.assign(pUsnRecord->FileName, pUsnRecord->FileNameLength / sizeof(WCHAR));
								wstr_namebuff.to_lower();
								wstr_namebuff.convert_code(str_namebuff_u8, CP_UTF8);
								m_vec_mtf[n_index].m_wsz_name = m_mem_list.GetPointer(str_namebuff_u8.size() + 1);
								if (m_vec_mtf[n_index].m_wsz_name)
								{
									memcpy(m_vec_mtf[n_index].m_wsz_name, str_namebuff_u8.c_str(), str_namebuff_u8.size());
								}
								else
								{
#ifdef _DEBUG
									DebugBreak();
#endif
								}
								m_vec_mtf[n_index].b_is_file = !(pUsnRecord->FileAttributes & FILE_ATTRIBUTE_DIRECTORY);
							}
							m_n_lastUSN = pUsnRecord->Usn;
							ret = pUsnRecord->Usn;
							pUsnRecord = (PUSN_RECORD) 
								((PBYTE) pUsnRecord + pUsnRecord->RecordLength);
						}
					}
					if (::GetLastError() == ERROR_JOURNAL_ENTRY_DELETED)
					{
#ifdef _FAST_FIND_LOGMOD_
						::OutputDebugStringW(L"遇到读入USN项目被删除...");
#endif
						ret = 0;
					}
				}
				if (buffer)
				{
					delete [] buffer;
				}
			}
			else
			{
				m_n_lastUSN = qujd.NextUsn;
			}
			usnid = qujd.UsnJournalID;
		}
		else
		{
			DeviceIoControl(hVolume, FSCTL_CREATE_USN_JOURNAL, &cujd, sizeof(cujd), NULL, 0, &br, NULL);
			DeviceIoControl(hVolume, FSCTL_QUERY_USN_JOURNAL, NULL, 0, &qujd, sizeof(qujd), &br, NULL);
			usnid = qujd.UsnJournalID;
		}
		return ret;
	}

	USN EnumUsnRecord2(std::vector<ULONG>& con, LONGLONG& usnid, USN start_usn)
	{
#define BUFF_LEN1	1024 * 32
		DWORD br;
		USN_JOURNAL_DATA qujd = {0};
		USN ret = 0;
		if (DeviceIoControl(hVolume, FSCTL_QUERY_USN_JOURNAL, NULL, 0, &qujd, sizeof(qujd), &br, NULL))
		{
			if (start_usn >= qujd.FirstUsn && start_usn <= qujd.NextUsn)
			{
				BYTE buffer[BUFF_LEN1] = {0};
				DWORD BytesReturned = 0;
				READ_USN_JOURNAL_DATA rujd = {start_usn, 0xffffffff/*to get all reason*/, 0, 0, 0, qujd.UsnJournalID};

				DWORD cb = 0;
				while (TRUE) 
				{
					BOOL fOk = DeviceIoControl(hVolume, FSCTL_READ_USN_JOURNAL, &rujd, sizeof(rujd), 
						buffer, BUFF_LEN1, &cb, NULL);
					if (!fOk || (cb <= sizeof(USN))) 
						break;

					rujd.StartUsn = *(USN*)buffer;

					// The first returned record is just after the first sizeof(USN) bytes
					USN_RECORD *pUsnRecord = (PUSN_RECORD) &buffer[sizeof(USN)];

					// Walk the output buffer
					while ((PBYTE) pUsnRecord < (buffer + cb))
					{
						ULONG mtf_index = pUsnRecord->FileReferenceNumber & 0x0000FFFFFFFFFFFF;
						if (pUsnRecord->Reason & USN_REASON_FILE_CREATE || pUsnRecord->Reason & USN_REASON_RENAME_NEW_NAME || pUsnRecord->Reason & USN_REASON_FILE_DELETE)
						{
							con.push_back(mtf_index);
						}
						ret = pUsnRecord->Usn;
						pUsnRecord = (PUSN_RECORD) 
							((PBYTE) pUsnRecord + pUsnRecord->RecordLength);
					}

					memset(buffer, 0, BUFF_LEN1);
				}
				if (::GetLastError() == ERROR_JOURNAL_ENTRY_DELETED)
				{
#ifdef _FAST_FIND_LOGMOD_
					::OutputDebugStringW(L"遇到读入USN项目被删除...");
#endif
					ret = 0;
				}
			}
			usnid = qujd.UsnJournalID;
		}
		return ret;
	}



	USN EnumUsnRecord3(USN start_usn)
	{
#define BUFF_LEN2	1024 * 32
		DWORD br;
		USN_JOURNAL_DATA qujd = {0};
		qujd.UsnJournalID = m_n_USNid;
		USN ret = 0;
		BYTE buffer[BUFF_LEN2] = {0};
		DWORD BytesReturned = 0;
		READ_USN_JOURNAL_DATA rujd = {start_usn, 0xffffffff/*to get all reason*/, 0, 0, 0, qujd.UsnJournalID};
		DWORD cb = 0;
		while (TRUE) 
		{
			BOOL fOk = DeviceIoControl(hVolume, FSCTL_READ_USN_JOURNAL, &rujd, sizeof(rujd), buffer, BUFF_LEN2, &cb, NULL);
			if (!fOk || (cb <= sizeof(USN))) 
				break;

			rujd.StartUsn = * (USN*) buffer;

			// The first returned record is just after the first sizeof(USN) bytes
			USN_RECORD *pUsnRecord = (PUSN_RECORD) &buffer[sizeof(USN)];

			// Walk the output buffer
			while ((PBYTE) pUsnRecord < (buffer + cb))
			{
				DWORDLONG mtf_index = pUsnRecord->FileReferenceNumber;
				if (pUsnRecord->Reason & USN_REASON_FILE_DELETE)
				{
					ULONG n_index = mtf_index & 0x0000FFFFFFFFFFFF;
					while (m_vec_mtf.size() <= n_index)
					{
						MFT_LIST_INFO mft_list_info_tmp = {0};
						m_vec_mtf.push_back(mft_list_info_tmp);
					}
					m_vec_mtf[n_index].m_mft_parent_index = 0;
					m_vec_mtf[n_index].m_wsz_name = NULL;
				}
				else if (pUsnRecord->Reason & USN_REASON_FILE_CREATE || pUsnRecord->Reason & USN_REASON_RENAME_NEW_NAME)
				{
					ULONG n_index = mtf_index & 0x0000FFFFFFFFFFFF;
					while (m_vec_mtf.size() <= n_index)
					{
						MFT_LIST_INFO mft_list_info_tmp = {0};
						m_vec_mtf.push_back(mft_list_info_tmp);
					}
					m_vec_mtf[n_index].m_mft_parent_index = pUsnRecord->ParentFileReferenceNumber & 0x0000FFFFFFFFFFFF;
					stringxw wstr_namebuff;
					stringxa str_namebuff_u8;
					wstr_namebuff.assign(pUsnRecord->FileName, pUsnRecord->FileNameLength / sizeof(WCHAR));
					wstr_namebuff.to_lower();
					wstr_namebuff.convert_code(str_namebuff_u8, CP_UTF8);
					m_vec_mtf[n_index].m_wsz_name = m_mem_list.GetPointer(str_namebuff_u8.size() + 1);
					if (m_vec_mtf[n_index].m_wsz_name)
					{
						memcpy(m_vec_mtf[n_index].m_wsz_name, str_namebuff_u8.c_str(), str_namebuff_u8.size());
					}
					else
					{
#ifdef _DEBUG
						DebugBreak();
#endif
					}
					m_vec_mtf[n_index].b_is_file = !(pUsnRecord->FileAttributes & FILE_ATTRIBUTE_DIRECTORY);
				}
				ret = pUsnRecord->Usn;
				pUsnRecord = (PUSN_RECORD) 
					((PBYTE) pUsnRecord + pUsnRecord->RecordLength);
			}
			memset(buffer, 0, BUFF_LEN2);
		}
		if (::GetLastError() == ERROR_JOURNAL_ENTRY_DELETED)
		{
#ifdef _FAST_FIND_LOGMOD_
			::OutputDebugStringW(L"遇到读入USN项目被删除...");
#endif
			ret = 0;
		}
		return ret;
	}
//////////////////////////////////////////////////////////////////////////
    VOID FindFiles()
    {
		m_lock.lock();
        PATTRIBUTE attr = FindAttribute(MFT, AttributeBitmap, 0);
        PUCHAR bitmap = new UCHAR[AttributeLengthAllocated(attr)];
        ReadAttribute(attr, bitmap);//$MFT元文件中的$Bitmap属性，此属性中标识了$MFT元文件中MFT项的使用状况
        ULONG n = AttributeLength(FindAttribute(MFT, AttributeData, 0))/BytesPerFileRecord;
        //MTF的总项数

		/*从本地load缓存*/
		m_vec_mtf.reserve(n + 1000);

#ifdef _FAST_FIND_LOGMOD_
		stringxw wstr_output_point;
		wstr_output_point.format(L"%s盘log开始...", m_wstr_volum.c_str());
		::OutputDebugStringW(wstr_output_point);
		DWORD dw_begin = ::GetTickCount();
#endif
		_ReadCache();

#ifdef _FAST_FIND_LOGMOD_
		DWORD dw_point = ::GetTickCount() - dw_begin;
		wstr_output_point.format(L"读入本地缓存时间：%d毫秒", dw_point);
		::OutputDebugStringW(wstr_output_point);
#endif
		
		LONGLONG usnid = 0x11;

#ifdef _FAST_FIND_LOGMOD_
		dw_begin = ::GetTickCount();
#endif
		bool b_newdata = false;
		USN usn_ret = 0;
		if (0 == m_n_lastUSN)
		{
			b_newdata = true;
			RenewUSNAndUSNID(usnid);
		}
		else
		{
			usn_ret = EnumUsnRecord(usnid);
		}

		

#ifdef _FAST_FIND_LOGMOD_
		dw_point = ::GetTickCount() - dw_begin;
		wstr_output_point.format(L"读USN用时：%d毫秒", dw_point);
		::OutputDebugStringW(wstr_output_point);
#endif

		if (0 == usn_ret || usnid != m_n_USNid || b_newdata)
		{
			{
				std::vector<MFT_LIST_INFO> vec_tmp;
				m_vec_mtf.swap(vec_tmp);
			}
			m_vec_mtf.clear();
			m_mem_list.ReleaseAll();
			m_n_USNid = usnid;
		}
		/*从本地load缓存*/

		PFILE_RECORD_HEADER file = PFILE_RECORD_HEADER(new UCHAR[BytesPerFileRecord]);
		stringxw wstr_namebuff;
		stringxa str_namebuff_u8;

#ifdef _FAST_FIND_LOGMOD_
		dw_begin = ::GetTickCount();
#endif

		if (!m_vec_mtf.size())
		{
			for(ULONG i = 0; i < n; i++)
			{
				MFT_LIST_INFO mft_list_info_tmp = {0};
				m_vec_mtf.push_back(mft_list_info_tmp);
				if (!bitset(bitmap, i)) //跳过被删除的文件
					continue;

				ReadFileRecord(i, file);

				if (file->Ntfs.Type == 'ELIF')
				{
					attr = FindAttribute(file, AttributeFileName, 0);
					if (attr == 0)
						continue;

					PFILENAME_ATTRIBUTE name = PFILENAME_ATTRIBUTE(Padd(attr,PRESIDENT_ATTRIBUTE(attr)->ValueOffset));

					int n_ctrl = 1;
					PFILENAME_ATTRIBUTE name_lenname = NULL;
					PATTRIBUTE attr_lenname = NULL;
					BYTE file_type = name->NameType;
					while (file_type == 2) //只取长文件名
					{
						attr_lenname = FindAttribute(file, AttributeFileName, 0, n_ctrl);
						n_ctrl++;
						if (attr_lenname == 0)
							break;
						name_lenname = PFILENAME_ATTRIBUTE(Padd(attr_lenname,PRESIDENT_ATTRIBUTE(attr_lenname)->ValueOffset));
						file_type = name_lenname->NameType;
					}
					if (attr_lenname != 0) //如果没有长文件名，就取短文件名
					{
						attr = attr_lenname;
						name = name_lenname;
					}
					std::size_t n_index = m_vec_mtf.size() - 1;
					m_vec_mtf[n_index].m_mft_parent_index = (ULONG)name->DirectoryFileReferenceNumber;
					wstr_namebuff.assign(name->Name, name->NameLength);
					wstr_namebuff.to_lower();
					wstr_namebuff.convert_code(str_namebuff_u8, CP_UTF8);
					m_vec_mtf[n_index].m_wsz_name = m_mem_list.GetPointer(str_namebuff_u8.size() + 1);
					if (m_vec_mtf[n_index].m_wsz_name)
					{
						memcpy(m_vec_mtf[n_index].m_wsz_name, str_namebuff_u8.c_str(), str_namebuff_u8.size());
					}
					else
					{
#ifdef _DEBUG
						DebugBreak();
#endif
					}
					m_vec_mtf[n_index].b_is_file = ((file->Flags & 1) && (file->Flags & 2) == 0);
				}
			}

#ifdef _FAST_FIND_LOGMOD_
			dw_point = ::GetTickCount() - dw_begin;
			wstr_output_point.format(L"重新读MFT用时：%d毫秒", dw_point);
			::OutputDebugStringW(wstr_output_point);
#endif
		}

		if (bitmap)
		{
			delete [] bitmap;
		}

		if (file)
		{
			delete [] file;
		}

		m_lock.unlock();
    }

    BOOL bitset(PUCHAR bitmap, ULONG i)
    {
        return (bitmap[i >> 3] & (1 << (i & 7))) != 0;
    }

	void _ReadCache()
	{
		stringxw wstr_path;
		get_user_path(wstr_path);
		wstr_path += L"fastfind\\";
		//mkdir_recursively(wstr_path);
		wstr_path += m_wstr_volum[0];
		wstr_path += L".ntdb";
		FILE* file = _wfopen(wstr_path, L"rb");
		if (file)
		{
			ULONG ul_size;
			get_file_size(wstr_path, ul_size);
			if (ul_size < (sizeof(USN) + sizeof(LONGLONG) + sizeof(BYTE)))
			{
				fclose(file);
				return;
			}
			BYTE* buff = new (std::nothrow) BYTE[ul_size];
			if (buff)
			{
				fread(buff, sizeof(BYTE), ul_size, file);
				BYTE* p_tmp = buff;
				p_tmp += sizeof(BYTE); //version
				m_n_USNid = *((LONGLONG*)p_tmp); //usn journal id
				p_tmp += sizeof(LONGLONG);
				m_n_lastUSN = *((USN*)p_tmp); //usn number
				p_tmp += sizeof(USN);
				while (p_tmp < (buff + ul_size))
				{
					MFT_LIST_INFO info_tmp = {0};
					info_tmp.m_mft_parent_index = *((ULONG*)p_tmp);
					if (info_tmp.m_mft_parent_index)
					{
						p_tmp += sizeof(ULONG);
						info_tmp.b_is_file = *((ULONG*)p_tmp);
						p_tmp += sizeof(ULONG);
						ULONG ul_len = strlen((const char*)p_tmp);
						BYTE* wsz_name = m_mem_list.GetPointer(ul_len + 1);
						info_tmp.m_wsz_name = wsz_name;
						memcpy(wsz_name, p_tmp, ul_len + 1);
						p_tmp += (ul_len + 1);
					}
					else
					{
						p_tmp += sizeof(ULONG);
					}
					m_vec_mtf.push_back(info_tmp);
				}
				delete [] buff;
			}
			fclose(file);
		}
	}

	void _WriteCache()
	{
		m_lock.lock();
		stringxw wstr_path;
		get_user_path(wstr_path);
		wstr_path += L"fastfind\\";
		mkdir_recursively(wstr_path);
		wstr_path += m_wstr_volum[0];
		wstr_path += L".ntdb";
		FILE* file = _wfopen(wstr_path, L"wb");
		if (file)
		{
			fwrite(&m_version, sizeof(BYTE), 1, file); //version
			fwrite(&m_n_USNid, sizeof(m_n_USNid), 1, file); //USN journal id
			fwrite(&m_n_lastUSN, sizeof(m_n_lastUSN), 1, file); //Last USN number
			for (std::size_t i = 0; i < m_vec_mtf.size(); ++i)
			{
// #ifdef _FAST_FIND_LOGMOD_
// 				if (i == 175366)
// 				{
// 					CString cstr_output;
// 					if (!m_vec_mtf[i].m_wsz_name)
// 					{
// 						cstr_output.Format(L"name:null, parent num:%d\r\n", m_vec_mtf[i].m_mft_parent_index);
// 					}
// 					cstr_output.Format(L"name:%s, parent num:%d\r\n", m_vec_mtf[i].m_wsz_name, m_vec_mtf[i].m_mft_parent_index);
// 					::OutputDebugStringW(cstr_output);
// 				}
// #endif

				if (!m_vec_mtf[i].m_wsz_name)
				{
					fwrite(&m_vec_mtf[i].m_mft_parent_index, sizeof(m_vec_mtf[i].m_mft_parent_index), 1, file);
				}
				else
				{
					fwrite(&m_vec_mtf[i].m_mft_parent_index, sizeof(m_vec_mtf[i].m_mft_parent_index), 1, file);
					fwrite(&m_vec_mtf[i].b_is_file, sizeof(m_vec_mtf[i].b_is_file), 1, file);
					fwrite(m_vec_mtf[i].m_wsz_name, sizeof(BYTE), strlen((const char*)m_vec_mtf[i].m_wsz_name) + 1, file);
				}
			}
			fclose(file);
		}
		m_lock.unlock();
	}

	/*
	int Run() //这种方式比较笨，cpu占用会高些，不过没有问题。
	{
		stringxw wstr_volum_path;
		wstr_volum_path = m_wstr_volum + L"\\";
		HANDLE h_monitor = FindFirstChangeNotification(wstr_volum_path.c_str(), TRUE, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME);
		if (INVALID_HANDLE_VALUE != h_monitor)
		{
			USN start_usn = m_n_lastUSN;
			while (true)
			{
				::WaitForSingleObject(h_monitor, INFINITE);
				::Sleep(1000);
				stringxw wstr_volum = L"\\\\.\\";
				wstr_volum += m_wstr_volum;
				hVolume = CreateFile(
					wstr_volum.c_str(),
					GENERIC_READ,
					FILE_SHARE_READ | FILE_SHARE_WRITE,
					0,
					OPEN_EXISTING,
					0,
					0);
				USN usn_ret = 0;
				if (hVolume != INVALID_HANDLE_VALUE)
				{
					std::vector<ULONG> vec_tmp;
					LONGLONG usnid = 0;
					usn_ret = EnumUsnRecord2(vec_tmp, usnid, start_usn);
					if (0 == usn_ret || usnid != m_n_USNid)
					{
						break;
					}

					BytesPerFileRecord = bootb.ClustersPerFileRecord < 0x80
						? bootb.ClustersPerFileRecord* bootb.SectorsPerCluster
						* bootb.BytesPerSector: 1 << (0x100 - bootb.ClustersPerFileRecord);				
					MFT = PFILE_RECORD_HEADER(new UCHAR[BytesPerFileRecord]);
					ReadSector((bootb.MftStartLcn)*(bootb.SectorsPerCluster), (BytesPerFileRecord)/(bootb.BytesPerSector), MFT);
					FixupUpdateSequenceArray(MFT);
					PATTRIBUTE attr = FindAttribute(MFT, AttributeBitmap, 0);				
					PUCHAR bitmap = new UCHAR[AttributeLengthAllocated(attr)];
					ReadAttribute(attr, bitmap);//$MFT元文件中的$Bitmap属性，此属性中标识了$MFT元文件中MFT项的使用状况
					ULONG n = AttributeLength(FindAttribute(MFT, AttributeData, 0))/BytesPerFileRecord;


					m_lock.lock();
					PFILE_RECORD_HEADER file = PFILE_RECORD_HEADER(new UCHAR[BytesPerFileRecord]);
					stringxw wstr_namebuff;
					stringxa str_namebuff_u8;
					for (std::size_t i = 0; i < vec_tmp.size(); ++i)
					{
						if (m_vec_mtf.size() > vec_tmp[i])
						{
							if (!bitset(bitmap, vec_tmp[i]))
							{
								m_vec_mtf[vec_tmp[i]].m_mft_parent_index = 0;
								m_vec_mtf[vec_tmp[i]].m_wsz_name = 0;
								continue;
							}
						}
						else
						{
							while (m_vec_mtf.size() <= vec_tmp[i])
							{
								MFT_LIST_INFO mft_list_info_tmp = {0};
								m_vec_mtf.push_back(mft_list_info_tmp);
							}
							if (!bitset(bitmap, vec_tmp[i]))
							{
								m_vec_mtf[vec_tmp[i]].m_mft_parent_index = 0;
								m_vec_mtf[vec_tmp[i]].m_wsz_name = 0;
								continue;
							}
						}
						ReadFileRecord2(vec_tmp[i], file);
						if (file->Ntfs.Type == 'ELIF')
						{
							attr = FindAttribute(file, AttributeFileName, 0);
							if (attr == 0)
								continue;

							PFILENAME_ATTRIBUTE name = PFILENAME_ATTRIBUTE(Padd(attr,PRESIDENT_ATTRIBUTE(attr)->ValueOffset));

							int n_ctrl = 1;
							PFILENAME_ATTRIBUTE name_lenname = NULL;
							PATTRIBUTE attr_lenname = NULL;
							BYTE file_type = name->NameType;
							while (file_type == 2) //只取长文件名
							{
								attr_lenname = FindAttribute(file, AttributeFileName, 0, n_ctrl);
								n_ctrl++;
								if (attr_lenname == 0)
									break;
								name_lenname = PFILENAME_ATTRIBUTE(Padd(attr_lenname,PRESIDENT_ATTRIBUTE(attr_lenname)->ValueOffset));
								file_type = name_lenname->NameType;
							}
							if (attr_lenname != 0) //如果没有长文件名，就取短文件名
							{
								attr = attr_lenname;
								name = name_lenname;
							}
							std::size_t n_index = vec_tmp[i];
							m_vec_mtf[n_index].m_mft_parent_index = (ULONG)name->DirectoryFileReferenceNumber;
							wstr_namebuff.assign(name->Name, name->NameLength);
							wstr_namebuff.to_lower();
							wstr_namebuff.convert_code(str_namebuff_u8, CP_UTF8);
							m_vec_mtf[n_index].m_wsz_name = m_mem_list.GetPointer(str_namebuff_u8.size() + 1);
							if (m_vec_mtf[n_index].m_wsz_name)
							{
								memcpy(m_vec_mtf[n_index].m_wsz_name, str_namebuff_u8.c_str(), str_namebuff_u8.size());
							}
							else
							{
#ifdef _DEBUG
								DebugBreak();
#endif
							}
							m_vec_mtf[n_index].b_is_file = ((file->Flags & 1) && (file->Flags & 2) == 0);
						}
					}

					if (MFT)
					{
						delete [] MFT;
						MFT = NULL;
					}
					if (bitmap)
					{
						delete [] bitmap;
						bitmap = NULL;
					}
					::CloseHandle(hVolume);
					hVolume = NULL;
				}
				start_usn = usn_ret;
				//m_n_lastUSN = start_usn;
				m_lock.unlock();
				::FindNextChangeNotification(h_monitor);
			}
			::FindCloseChangeNotification(h_monitor);
		}		
		return 0;
	}
	*/

	int Run()
	{
		stringxw wstr_volum_path;
		wstr_volum_path = m_wstr_volum + L"\\";
		HANDLE h_monitor = FindFirstChangeNotification(wstr_volum_path.c_str(), TRUE, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME);
		if (INVALID_HANDLE_VALUE != h_monitor)
		{
			USN start_usn = m_n_lastUSN;
			USN usn_ret = 0;
			stringxw wstr_volum = L"\\\\.\\";
			wstr_volum += m_wstr_volum;
// 			hVolume = CreateFile(
// 				wstr_volum.c_str(),
// 				GENERIC_READ,
// 				FILE_SHARE_READ | FILE_SHARE_WRITE,
// 				0,
// 				OPEN_EXISTING,
// 				0,
// 				0);
			while (true)
			{
				::WaitForSingleObject(h_monitor, INFINITE);
				::Sleep(1000);
				m_lock.lock();
				usn_ret = EnumUsnRecord3(start_usn);
				start_usn = usn_ret;	

				m_n_lastUSN = start_usn;
			
// 				if (!m_b_is_renewed)
// 				{
// 					m_n_lastUSN = start_usn; /*未知会不会引起什么负面作用*/
// 				}
				m_lock.unlock();
			}
		}
		return 0;
	}

	//暂时无用
	void GetFullPathByFileReferenceNumber( HANDLE hVol, DWORDLONG FileReferenceNumber, stringxw& wstr_out)
	{
		typedef ULONG (__stdcall *PNtCreateFile)(
			PHANDLE FileHandle,
			ULONG DesiredAccess,
			PVOID ObjectAttributes,
			PVOID IoStatusBlock,
			PLARGE_INTEGER AllocationSize,
			ULONG FileAttributes,
			ULONG ShareAccess,
			ULONG CreateDisposition,
			ULONG CreateOptions,
			PVOID EaBuffer,
			ULONG EaLength );
		PNtCreateFile NtCreatefile = (PNtCreateFile)GetProcAddress( GetModuleHandle(L"ntdll.dll"), "NtCreateFile" );

		typedef struct _UNICODE_STRING 
		{
			USHORT Length;
			USHORT MaximumLength;
			PWCH Buffer;
		} UNICODE_STRING, *PUNICODE_STRING;
		UNICODE_STRING fidstr = { 8, 8, (PWSTR)&FileReferenceNumber };

		typedef struct _OBJECT_ATTRIBUTES 
		{
			ULONG Length;
			HANDLE RootDirectory;
			PUNICODE_STRING ObjectName;
			ULONG Attributes;
			PVOID SecurityDescriptor;
			PVOID SecurityQualityOfService;
		} OBJECT_ATTRIBUTES;
		const ULONG OBJ_CASE_INSENSITIVE = 0x00000040UL;
		OBJECT_ATTRIBUTES oa = { sizeof(OBJECT_ATTRIBUTES), hVol, &fidstr, OBJ_CASE_INSENSITIVE, 0, 0 };

		HANDLE hFile;
		ULONG iosb[2];
		const ULONG FILE_OPEN_BY_FILE_ID = 0x00002000UL;
		const ULONG FILE_OPEN            = 0x00000001UL;
		ULONG status = NtCreatefile( &hFile, GENERIC_ALL, &oa, iosb, NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ|FILE_SHARE_WRITE, FILE_OPEN, FILE_OPEN_BY_FILE_ID, NULL, 0 );
		if( status == 0 )
		{
			typedef struct _IO_STATUS_BLOCK
			{
				union {
					NTSTATUS Status;
					PVOID Pointer;
				};
				ULONG_PTR Information;
			} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;
			typedef enum _FILE_INFORMATION_CLASS {
				// ……
				FileNameInformation = 9
				// ……
			} FILE_INFORMATION_CLASS, *PFILE_INFORMATION_CLASS;
			typedef NTSTATUS (__stdcall *PNtQueryInformationFile)(
				HANDLE FileHandle,
				PIO_STATUS_BLOCK IoStatusBlock,
				PVOID FileInformation,
				DWORD Length,
				FILE_INFORMATION_CLASS FileInformationClass );
			PNtQueryInformationFile NtQueryInformationFile = (PNtQueryInformationFile)GetProcAddress( GetModuleHandle(L"ntdll.dll"), "NtQueryInformationFile" );

			typedef struct _OBJECT_NAME_INFORMATION {
				UNICODE_STRING Name;
			} OBJECT_NAME_INFORMATION, *POBJECT_NAME_INFORMATION;
			IO_STATUS_BLOCK IoStatus;
			size_t allocSize = sizeof(OBJECT_NAME_INFORMATION) + 32770 * sizeof(WCHAR);
			POBJECT_NAME_INFORMATION pfni = (POBJECT_NAME_INFORMATION)operator new(allocSize);
			status = NtQueryInformationFile(hFile, &IoStatus, pfni, allocSize, FileNameInformation);
			if( status == 0 )
			{
				int len = *((USHORT*)pfni) / sizeof(WCHAR);
				BYTE* p_tmp = (BYTE*)pfni;
				p_tmp += sizeof(USHORT) * 2;
				wstr_out.assign((WCHAR*)p_tmp, len);
				wstr_out = m_wstr_volum + wstr_out;
				wstr_out.to_lower();
			}
			operator delete(pfni);
			CloseHandle(hFile);
		}
	}

private:
    CEnumMFT(const CEnumMFT& mft_obj)
    {
        
    }

    CEnumMFT& operator = (const CEnumMFT& mft_obj)
    {
        return *this;
    }


    enum
    {
        MAX_CACHE_SECTORS = 4000,
    };
    BOOT_BLOCK bootb;  //ntfs boot sector
    ULONG BytesPerFileRecord; //mft len
    PFILE_RECORD_HEADER MFT; //mft meta file


    HANDLE hVolume;    
    stringxw m_wstr_volum; 
    stringxa m_str_volum_u8;
    typedef struct
    {
        ULONGLONG cache_lcn_begin;
        ULONGLONG cache_lcn_count;
        BYTE* g_pb;
        ULONGLONG cache_lcn_orl_begin;
        ULONGLONG cache_lcn_total;
    }CACHE_INFO;
    CACHE_INFO m_cache_info;
    std::vector<MFT_LIST_INFO> m_vec_mtf;
    CMemoryList m_mem_list;
    bool m_b_is_ntfs;
	USN m_n_lastUSN;
	LONGLONG m_n_USNid;
	BYTE m_version;
	CCriticalSectionWrapper m_lock;
	bool m_b_is_renewed;
};