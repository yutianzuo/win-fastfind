// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
// #include <atlstr.h>
#include "./mft/EnumMFT.h"
#include "FindTread.h"
#include <map>

#include "NBTray.h"

#include "Detect/SystemInfo.h"

using namespace sysInfo;

#define WM_SYSTEM_IDL				WM_USER + 10


#include "RenewDataThread.h"


class CMainDlg : public CDialogImpl<CMainDlg>, public CUpdateUI<CMainDlg>,
		public CMessageFilter, public CIdleHandler, public CNBTray<CMainDlg>
{
public:

    enum
    {
        MENU_ID1 = 20001,
		MENU_ID2 = 20006,
		MENU_ID3 = 20007,
		MENU_ID4 = 20008,
		MENU_ID5 = 20009,

		TRAY_MENU_ID1 = 30001,
		TRAY_MENU_ID2 = 30002,
		TRAY_MENU_ID3 = 30003,
		TRAY_MENU_ID4 = 30004,
    };

	LRESULT CMainDlg::OnTrayMessage( UINT uMsg )
	{
		if (WM_RBUTTONUP == uMsg)
		{
			CMenu menu;
			menu.CreatePopupMenu();	
			menu.AppendMenu(MF_STRING, TRAY_MENU_ID1, L"打开(&O)");	
			menu.AppendMenu(MF_STRING, TRAY_MENU_ID2, L"隐藏(&H)");
			menu.AppendMenu(MF_HILITE);
			stringxw wstr_reg_value;
			get_register_value(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", L"360localsearch", wstr_reg_value);
			if (wstr_reg_value.empty())
			{
				menu.AppendMenu(MF_UNCHECKED, TRAY_MENU_ID4, L"开机启动(&L)");
			}
			else
			{
				menu.AppendMenu(MF_CHECKED, TRAY_MENU_ID4, L"开机启动(&L)");
			}			
			menu.AppendMenu(MF_HILITE);
			menu.AppendMenu(MF_STRING, TRAY_MENU_ID3, L"退出(&X)");
			POINT pt = {0};
			::GetCursorPos(&pt);
			SetForegroundWindow(this->m_hWnd);
			UINT i_cmd = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, pt.x, pt.y, m_hWnd);
			if (i_cmd)
			{
				switch (i_cmd)
				{
				case TRAY_MENU_ID1:
					{
						if (IsIconic())
						{
							ShowWindow(SW_RESTORE);
						}						
						ShowWindow(SW_SHOW);
						_SetTop();
					}
					break;
				case TRAY_MENU_ID2:
					{
						ShowWindow(SW_HIDE);
					}
					break;				
				case TRAY_MENU_ID3:
					{
						__super::DestroyTray();
						CloseDialog(0);
					}
					break;
				case TRAY_MENU_ID4:
					{
						stringxw wstr_reg_value;
						get_register_value(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", L"360localsearch", wstr_reg_value);
						if (wstr_reg_value.empty())
						{
							stringxw wstr_app_path;
							get_app_path(wstr_app_path);
							wstr_app_path = stringxw(L"\"") + wstr_app_path +  L"FastFind.exe" + L"\"" + L" autorun";
							write_register(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", L"360localsearch", wstr_app_path);
						}
						else
						{
							::SHDeleteValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", L"360localsearch");
						}
					}
					break;
				}
			}
		}
		else if (WM_LBUTTONUP == uMsg)
		{
			if (IsIconic())
			{
				ShowWindow(SW_RESTORE);
			}
			ShowWindow(SW_SHOW);
			_SetTop();
		}
		else if (WM_MOUSEMOVE == uMsg)
		{
			__super::ChangeTrayTip(L"360搜索");
		}
		return 1;
	}

public:
    static int __cdecl CompareFuncForFileName(void* context, const void *elem1, const void *elem2)
    {
		WCHAR** wsz_1 = (WCHAR**)elem1;
		WCHAR** wsz_2 = (WCHAR**)elem2;
		CMainDlg* p_dlg = (CMainDlg*)context;
		if (wsz_1 && wsz_2 && *wsz_1 && *wsz_2 && p_dlg)
		{
			CString cstr_name1 = PathFindFileName(*wsz_1);
			CString cstr_name2 = PathFindFileName(*wsz_2);
			int n_ret = wcscmp(cstr_name1, cstr_name2);
			if (p_dlg->m_b_sort_ctrl_name)
			{
				return n_ret;
			}
			else
			{
				if (0 != n_ret)
				{
					if (n_ret > 0)
					{
						n_ret = -1;
					}
					else
					{
						n_ret = 1;
					}
				}
				return n_ret;
			}			
		}
		return 0;
    }

	static int __cdecl CompareFuncForFilePath(void* context, const void *elem1, const void *elem2)
	{
		WCHAR** wsz_1 = (WCHAR**)elem1;
		WCHAR** wsz_2 = (WCHAR**)elem2;
		CMainDlg* p_dlg = (CMainDlg*)context;
		if (wsz_1 && wsz_2 && *wsz_1 && *wsz_2 && p_dlg)
		{
			int n_ret = wcscmp(*wsz_1, *wsz_2);
			if (p_dlg->m_b_sort_ctrl_path)
			{
				return n_ret;
			}
			else
			{
				if (0 != n_ret)
				{
					if (n_ret > 0)
					{
						n_ret = -1;
					}
					else
					{
						n_ret = 1;
					}
				}
				return n_ret;
			}	
		}
		return 0;

	}

	static int __cdecl CompareFuncForFileSize(void* context, const void *elem1, const void *elem2)
	{
		WCHAR** wsz_1 = (WCHAR**)elem1;
		WCHAR** wsz_2 = (WCHAR**)elem2;
		CMainDlg* p_dlg = (CMainDlg*)context;
		if (wsz_1 && wsz_2 && *wsz_1 && *wsz_2 && p_dlg)
		{
			__int64 n_size1 = 0; 
			get_file_sizex64(*wsz_1, n_size1);
			__int64 n_size2 = 0;
			get_file_sizex64(*wsz_2, n_size2);
			int n_ret;
			if (n_size2 == n_size1)
			{
				n_ret = 0;
			}
			else if (n_size1 < n_size2)
			{
				n_ret = -1;
			}
			else
			{
				n_ret = 1;
			}
			if (p_dlg->m_b_sort_ctrl_size)
			{
				return n_ret;
			}
			else
			{
				if (0 != n_ret)
				{
					if (n_ret > 0)
					{
						n_ret = -1;
					}
					else
					{
						n_ret = 1;
					}
				}
				return n_ret;
			}			
		}
		return 0;
	}


	static int __cdecl CompareFuncForFileTime(void* context, const void *elem1, const void *elem2)
	{
		WCHAR** wsz_1 = (WCHAR**)elem1;
		WCHAR** wsz_2 = (WCHAR**)elem2;
		CMainDlg* p_dlg = (CMainDlg*)context;
		if (wsz_1 && wsz_2 && *wsz_1 && *wsz_2 && p_dlg)
		{
			tag_file_info file_info1;
			get_file_info(*wsz_1, &file_info1);
			tag_file_info file_info2;
			get_file_info(*wsz_2, &file_info2);
			int n_ret = wcscmp(file_info1.wstr_create_time.c_str(), file_info2.wstr_create_time.c_str());
			if (p_dlg->m_b_sort_ctrl_time)
			{
				return n_ret;
			}
			else
			{
				if (0 != n_ret)
				{
					if (n_ret > 0)
					{
						n_ret = -1;
					}
					else
					{
						n_ret = 1;
					}
				}
				return n_ret;
			}			
		}
		return 0;
	}

    CMainDlg() : m_find_thread(m_vec_disks, m_vec_volums), m_b_can_use(false)
    {
		m_b_sort_ctrl_name = true;
		m_b_sort_ctrl_path = true;
		m_b_sort_ctrl_size = true;
		m_b_sort_ctrl_time = true;
		m_b_data_all_ok = false;
    }
	enum { IDD = IDD_MAINDLG };

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if (::TranslateAccelerator(m_hWnd, m_hAccel, pMsg))
			return TRUE; 
		if (pMsg && pMsg->message == WM_KEYDOWN)
		{
			if (pMsg->wParam == VK_UP || pMsg->wParam == VK_DOWN)
			{
				if (::GetFocus() != GetDlgItem(IDC_COMBO1) && ::GetFocus() != GetDlgItem(IDC_LIST1))
				{
					::SetFocus(GetDlgItem(IDC_COMBO1));
					::PostMessage(GetDlgItem(IDC_COMBO1), WM_KEYDOWN, pMsg->wParam, pMsg->lParam);
				}
			}
// 			else if ((pMsg->wParam >= 0x30 && pMsg->wParam <= 0x39) || (pMsg->wParam >= 0x41 && pMsg->wParam <= 0x5a))
// 			{
// 				if (::GetFocus() != GetDlgItem(IDC_EDIT1))
// 				{
// 					::SetFocus(GetDlgItem(IDC_EDIT1));
// 					::PostMessage(GetDlgItem(IDC_EDIT1), WM_KEYDOWN, pMsg->wParam, pMsg->lParam);
// 					return TRUE;
// 					//::PostMessage(GetDlgItem(IDC_EDIT1), WM_KEYUP, pMsg->wParam, pMsg->lParam);
// 				}
// 			}
			else if (pMsg->wParam == VK_ESCAPE)
			{
				return TRUE;
			}
		}
		return CWindow::IsDialogMessage(pMsg);
	}

	virtual BOOL OnIdle()
	{
		return FALSE;
	}

	virtual ~CMainDlg()
	{

	}

	BEGIN_UPDATE_UI_MAP(CMainDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainDlg)
		CHAIN_MSG_MAP(CNBTray<CMainDlg>)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
        MESSAGE_HANDLER(WM_MESSAGE_1, OnLoadAllFilesDone)
        MESSAGE_HANDLER(WM_MESSAGE_2, OnRefreshList)
        MESSAGE_HANDLER(WM_MESSAGE_3, OnSearchDone)
		MESSAGE_HANDLER(WM_MESSAGE_4, OnBeginSearch)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
        //NOTIFY_CODE_HANDLER(LVN_GETDISPINFO, NotifyHandler)
        NOTIFY_HANDLER(IDC_LIST1, NM_DBLCLK, OnItemDblClk)
        NOTIFY_HANDLER(IDC_LIST1, NM_RCLICK, OnItemRClick)
        NOTIFY_HANDLER(IDC_LIST1, LVN_COLUMNCLICK, OnColumnClick)
		NOTIFY_HANDLER(IDC_LIST1, LVN_GETDISPINFO, OnGetDispInfo)
        //COMMAND_HANDLER(MENU_ID1, BN_CLICKED, OnMenuPos)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		MESSAGE_HANDLER(WM_COPYDATA, OnCopyData)
		MESSAGE_HANDLER(WM_HOTKEY, OnHotKey)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_ENDSESSION, OnEndSession)
		MESSAGE_HANDLER(WM_SYSTEM_IDL, OnSysIdle)
		MESSAGE_HANDLER(CRnewDataThread::MSG_DATA_UPDATE_DONE, OnUpdateDataDone)
		COMMAND_HANDLER(IDC_BUTTON1, BN_CLICKED, OnBnClickedButton1)
		COMMAND_HANDLER(IDC_BUTTON2, BN_CLICKED, OnBnClickedButton2)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)


	LRESULT OnColumnClick(int idCtrl, LPNMHDR pnmh, BOOL& /*bHandled*/)
    {        
		CListViewCtrl list_view = (CListViewCtrl)GetDlgItem(IDC_LIST1);
		if (!m_b_data_all_ok && list_view.GetItemCount())
		{
			::MessageBox(this->m_hWnd, L"搜索尚未完成，请稍候", L"360搜索", MB_OK);
			return 1; 
		}

		if (list_view.GetItemCount() > 1000)
		{
			if (IDNO == ::MessageBox(this->m_hWnd, L"大数据量排序会导致界面长时间失去响应，是否继续？", L"360搜索", MB_YESNO))
			{
				return 1;
			}
		}

        LPNMLISTVIEW lpNMLISTVIEW	= (LPNMLISTVIEW)pnmh;

		if (lpNMLISTVIEW)
		{
			std::vector<WCHAR*>& vec_temp = m_find_thread.GetSearchAllData();
			switch (lpNMLISTVIEW->iSubItem)
			{
			case 0:
				{
					qsort_s(static_cast<void*>(&vec_temp[0]), vec_temp.size(), sizeof(WCHAR*), CompareFuncForFileName, (void*)this);
					m_b_sort_ctrl_name = !m_b_sort_ctrl_name;
				}
				break;
			case 1:
				{
					qsort_s(static_cast<void*>(&vec_temp[0]), vec_temp.size(), sizeof(WCHAR*), CompareFuncForFilePath, (void*)this);
					m_b_sort_ctrl_path = !m_b_sort_ctrl_path;
				}
				break;
			case 2:
				{
					qsort_s(static_cast<void*>(&vec_temp[0]), vec_temp.size(), sizeof(WCHAR*), CompareFuncForFileSize, (void*)this);
					m_b_sort_ctrl_size = !m_b_sort_ctrl_size;
				}
				break;
			case 3:
				{
					qsort_s(static_cast<void*>(&vec_temp[0]), vec_temp.size(), sizeof(WCHAR*), CompareFuncForFileTime, (void*)this);
					m_b_sort_ctrl_time = !m_b_sort_ctrl_time;
				}
				break;
			}
		}
        
        //list_view.SortItemsEx(&CMainDlg::CompareFunc, (LPARAM)this);
		list_view.Invalidate();
        return 1;
    }

	LRESULT OnGetDispInfo(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
    {        
        NMLVDISPINFO* lpDispInfo = (NMLVDISPINFO*)pnmh;
		if (lpDispInfo)
		{
			LVITEMW& lv_item = lpDispInfo->item;
			if (lv_item.mask & LVIF_TEXT) //字符串缓冲区有效
			{
				stringxw wstr_file_path = m_find_thread.GetSearchResult(lv_item.iItem);	
				switch(lv_item.iSubItem)
				{
				case 0:
					{
						CString cstr_filename;
						cstr_filename = PathFindFileName(wstr_file_path.c_str());
						lstrcpy(lv_item.pszText, cstr_filename);
					}
					break;
				case 1:
					{
						lstrcpy(lv_item.pszText, wstr_file_path.c_str());
					}					
					break;
				case 2:
					{
						if (_IsItemInSelectedItems(lv_item.iItem))
						{
							if (wstr_file_path == m_file_info.wstr_path)
							{
								lstrcpy(lv_item.pszText, m_file_info.file_info.wstr_file_size.c_str());
							}
							else
							{
								get_file_info(wstr_file_path.c_str(), &m_file_info.file_info);
								lstrcpy(lv_item.pszText, m_file_info.file_info.wstr_file_size.c_str());
								m_file_info.wstr_path = wstr_file_path;
							}
						}
						else
						{
							lstrcpy(lv_item.pszText, L"--");
						}
					}
					break;
				case 3:
					{
						if (_IsItemInSelectedItems(lv_item.iItem))
						{
							if (wstr_file_path == m_file_info.wstr_path)
							{
								lstrcpy(lv_item.pszText, m_file_info.file_info.wstr_create_time.c_str());
							}
							else
							{
								get_file_info(wstr_file_path.c_str(), &m_file_info.file_info);
								lstrcpy(lv_item.pszText, m_file_info.file_info.wstr_create_time.c_str());
								m_file_info.wstr_path = wstr_file_path;
							}
						}
						else
						{
							lstrcpy(lv_item.pszText, L"--");
						}
					}
					break;
				}
			}
		}
        return 0;
    }

    LRESULT OnItemRClick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
    {        
        LPNMLISTVIEW lpNMLISTVIEW = (LPNMLISTVIEW)pnmh;
        CListViewCtrl list_view = (CListViewCtrl)GetDlgItem(IDC_LIST1);
		//////////////////////////////////////////////////////////////////////////
		//UseShellMenu(cstr_text1); /*用shell的menu*/

		CString cstr_text;


		int n_sel = ::SendMessage(list_view.m_hWnd, LVM_GETNEXTITEM, -1, MAKELPARAM(LVIS_SELECTED, 0));
		if (-1 == n_sel)
		{
			return 1;
		}

		/*用自己的menu*/
		CMenu menu;
		menu.CreatePopupMenu();	
		menu.AppendMenu(MF_STRING, MENU_ID1, L"打开(&O)");
		menu.AppendMenu(MF_STRING, MENU_ID2, L"打开所在文件夹(&P)");
		menu.AppendMenu(MF_HILITE);
		menu.AppendMenu(MF_STRING, MENU_ID3, L"复制全路径名(&L)");
		menu.AppendMenu(MF_HILITE);
		menu.AppendMenu(MF_STRING, MENU_ID4, L"属性(&R)");
		POINT pt = {0};
		::GetCursorPos(&pt);
		UINT iCmd = menu.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON | TPM_RETURNCMD, pt.x, pt.y, m_hWnd);
		if (iCmd)
		{
			switch (iCmd)
			{
			case MENU_ID1:
				{
					int n_index = ::SendMessage(list_view.m_hWnd, LVM_GETNEXTITEM, -1, MAKELPARAM(LVIS_SELECTED, 0));
					while (-1 != n_index)
					{
						list_view.GetItemText(n_index, 1, cstr_text);
						HINSTANCE hins = ShellExecute(0, L"open", cstr_text, 0, 0, SW_SHOWNORMAL);
						if ((int)hins <= 32)
						{
							CString szParam;
							szParam.Format(_T("/select, %s"), cstr_text);
							::ShellExecute(NULL, L"open", L"explorer.exe", szParam, NULL, SW_SHOWNORMAL);
						}
						n_index = ::SendMessage(list_view.m_hWnd, LVM_GETNEXTITEM, n_index, MAKELPARAM(LVIS_SELECTED, 0));
					}
				}
				break;
			case MENU_ID2:
				{
					int n_index = ::SendMessage(list_view.m_hWnd, LVM_GETNEXTITEM, -1, MAKELPARAM(LVIS_SELECTED, 0));
					list_view.GetItemText(n_index, 1, cstr_text);
					if (cstr_text.GetLength())
					{
						CString szParam;
						szParam.Format(_T("/select, %s"), cstr_text);
						::ShellExecute(NULL, L"open", L"explorer.exe", szParam, NULL, SW_SHOWNORMAL);
					}
				}
				break;
			case MENU_ID3:
				{
					int n_index = ::SendMessage(list_view.m_hWnd, LVM_GETNEXTITEM, -1, MAKELPARAM(LVIS_SELECTED, 0));
					list_view.GetItemText(n_index, 1, cstr_text);
					if (cstr_text.GetLength())
					{
						HGLOBAL h_gb_text = ::GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE, 1024 * sizeof(WCHAR));
						LPSTR lpData = (LPSTR)::GlobalLock(h_gb_text);
						wmemcpy((WCHAR*)(lpData), cstr_text, 1024);
						::GlobalUnlock(h_gb_text);
						if (NO_ERROR != ::GetLastError())
						{
							ATLASSERT(TRUE);
						}
						if (::OpenClipboard(NULL))
						{
							::EmptyClipboard();
							if (!::SetClipboardData(CF_UNICODETEXT, h_gb_text))
							{
								::GlobalFree(h_gb_text);
								//失败才释放，否则不能free，msdn这么说的
							}
							::CloseClipboard();
						}
					}
				}
				break;
			case MENU_ID4:
				{
					int n_index = ::SendMessage(list_view.m_hWnd, LVM_GETNEXTITEM, -1, MAKELPARAM(LVIS_SELECTED, 0));
					while (-1 != n_index)
					{
						list_view.GetItemText(n_index, 1, cstr_text);
						SHELLEXECUTEINFO sei;
						sei.hwnd = m_hWnd;
						sei.lpVerb = L"properties";
						sei.lpFile = (LPCWSTR)cstr_text;
						sei.lpDirectory = NULL;
						sei.lpParameters = NULL;
						sei.nShow = SW_SHOWNORMAL;
						sei.fMask = SEE_MASK_INVOKEIDLIST;
						sei.lpIDList = NULL;
						sei.cbSize = sizeof(SHELLEXECUTEINFO);
						ShellExecuteEx(&sei);
						n_index = ::SendMessage(list_view.m_hWnd, LVM_GETNEXTITEM, n_index, MAKELPARAM(LVIS_SELECTED, 0));
					}
				}
				break;
			}
		}
		/*用自己的menu*/
        return 1;
    }

    LRESULT OnItemDblClk(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
    {        
        LPNMLISTVIEW lpNMLISTVIEW = (LPNMLISTVIEW)pnmh;
        CListViewCtrl list_view = (CListViewCtrl)GetDlgItem(IDC_LIST1);
        wchar_t cstr_text1[1024] = {0};
        list_view.GetItemText(lpNMLISTVIEW->iItem, 1, cstr_text1, 1024);

		CString cstr_0;
		list_view.GetItemText(lpNMLISTVIEW->iItem, 0, cstr_0);
        if (CString(cstr_0) == L"正在建立索引请稍候..." || CString(cstr_0) == L"索引建立完毕，尽情使用吧！")
        {
            return 1;
        }

		HINSTANCE hins = ShellExecute(0, L"open", cstr_text1, 0, 0, SW_SHOWNORMAL);
		if ((int)hins <= 32)
		{
			CString szParam;
			szParam.Format(_T("/select, %s"), cstr_text1);
			::ShellExecute(NULL, L"open", L"explorer.exe", szParam, NULL, SW_SHOWNORMAL);
		}
        return 1;
    }

    LRESULT OnEnterToSearch( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/ )
    {
        //PostMessage(WM_COMMAND, MAKEWPARAM(EDIT_SEARCH, BN_CLICKED), NULL);
        return 0;
    }

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// center the dialog on the screen
		CenterWindow();
		SetWindowText(L"360搜索");
		// set icons
		HICON hIcon = ::LoadIconW(_Module.m_hInst, MAKEINTRESOURCE(IDR_MAINFRAME));
		SetIcon(hIcon, TRUE);
		HICON hIconSmall = LoadIconW(_Module.m_hInst, MAKEINTRESOURCE(IDR_MAINFRAME));
		SetIcon(hIconSmall, FALSE);

		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);

		UIAddChildWindowContainer(m_hWnd);

		m_hAccel = ::LoadAcceleratorsW(_Module.m_hInst, MAKEINTRESOURCEW(IDR_MAINFRAME));


        CComboBox combo = GetDlgItem(IDC_COMBO1);

        m_vec_disks.clear();
        for (unsigned short i = 0x63; i < 0x7b; ++i)
        {
            wchar_t wsz_disk[3] = {0};
            wsz_disk[0] = i;
            wsz_disk[1] = L':';
            if (::PathFileExistsW(wsz_disk) && DRIVE_FIXED == GetDriveTypeW(wsz_disk))
            {
                int n_index = _IniMFT(CString(wsz_disk));
                if (m_vec_volums[n_index]->IsNTFS())
                    m_vec_disks.push_back(wsz_disk);
                else
                {
                    delete m_vec_volums[n_index];
                    m_vec_volums.erase(m_vec_volums.begin() + m_vec_volums.size() - 1);
                }
            }
        }

        combo.InsertString(0, L"我的电脑");

        for (size_t i = 0; i < m_vec_disks.size(); ++i)
        {
            CString cstr_file_name;
            WCHAR wsz_diskname[MAX_PATH] = {0};
            DWORD dw_serial_number;
            DWORD dw_max_size;
            DWORD dw_filesys_flag;
            WCHAR wsz_filesys_name[MAX_PATH] = {0};
            if (GetVolumeInformation(m_vec_disks[i], wsz_diskname, MAX_PATH, &dw_serial_number, &dw_max_size, &dw_filesys_flag, wsz_filesys_name, MAX_PATH))
            {
                combo.InsertString(i + 1, m_vec_disks[i]);
            }
            else
            {
                combo.InsertString(i + 1, m_vec_disks[i]);
            }
        }
        combo.SetCurSel(0);

        CListViewCtrl list_view = (CListViewCtrl)GetDlgItem(IDC_LIST1);

/*
        RECT rect;
        list_view.GetClientRect(&rect);
        list_view.ShowWindow(SW_HIDE);
        rect.top += 50;
        rect.bottom += 50;
        m_listview.Create(this->m_hWnd, &rect, L"ListView", WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE | WS_CHILDWINDOW | LVS_REPORT | LVS_OWNERDATA, 0, 6666);
        m_listview.InsertColumn(0, L"文件名");
        m_listview.SetColumnWidth(0, 700);
*/
        LONG   lStyle; 
        lStyle= ::GetWindowLong(list_view.m_hWnd,GWL_STYLE);//获取当前窗口类型 
        lStyle &= ~LVS_TYPEMASK;   //清除显示方式位 
        lStyle &= ~LVS_SHOWSELALWAYS;
        lStyle &= ~LVS_ALIGNLEFT; 
        lStyle &= ~LVS_ICON;
        lStyle |= LVS_REPORT;
		//lStyle |= LVS_SINGLESEL;
        //lStyle &= ~LVS_NOCOLUMNHEADER;
		lStyle |= LVS_OWNERDATA;
		//lStyle |= LVS_TYPEMASK;
        lStyle |= WS_CLIPCHILDREN;
        lStyle |= WS_CLIPSIBLINGS;
        ::SetWindowLong(list_view.m_hWnd,GWL_STYLE,lStyle);//设置窗口类型 


        DWORD   dwStyle; 
        dwStyle   =   list_view.GetExStyle(); 
        dwStyle &= ~LVS_EX_CHECKBOXES;
		dwStyle |= LVS_EX_FULLROWSELECT;
		//dwStyle |= LVS_EX_LABELTIP;
        list_view.SetExtendedListViewStyle(dwStyle);
        
        lStyle= ::GetWindowLong(list_view.m_hWnd, GWL_EXSTYLE);
        lStyle &= ~WS_EX_NOPARENTNOTIFY;
        lStyle |= WS_EX_CLIENTEDGE;
        ::SetWindowLong(list_view.m_hWnd, GWL_EXSTYLE, lStyle);





        list_view.InsertColumn(0, L"文件名称");
		list_view.InsertColumn(1, L"文件路径");
		list_view.InsertColumn(2, L"文件大小");
		list_view.InsertColumn(3, L"创建日期");
        //list_view.SetColumnWidth(0, 770);

        //list_view.InsertItem(0, L"正在建立索引请稍候...");


        m_find_thread.SetHWND(this->m_hWnd);
        m_find_thread.Create();

		((CButton)GetDlgItem(IDC_RADIO1)).SetCheck(TRUE);

		if (g_cstr_foldername.GetLength())
		{
			ConvertToLenPath(g_cstr_foldername);
			g_cstr_foldername.MakeLower();
			((CEdit)(GetDlgItem(IDC_EDIT2))).SetWindowText(g_cstr_foldername);
		}
		else
		{
			((CEdit)(GetDlgItem(IDC_EDIT2))).SetWindowText(L"");
		}

		__super::CreateTray(LoadIcon(_Module.m_hInst, MAKEINTRESOURCE(IDR_MAINFRAME)));
		::RegisterHotKey(m_hWnd, 0x888, 0, VK_F3);
		((CStatic)(GetDlgItem(IDC_STATIC))).SetWindowText(L"正在建立索引请稍候...");


// 		if (g_b_starthide)
// 		{
// 			//PostMessage(WM_SHOWWINDOW, FALSE, 0);
// 			LONG   lStyle; 
// 			lStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
// 			lStyle &= ~WS_VISIBLE;
// 			::SetWindowLong(m_hWnd, GWL_STYLE, lStyle);
// 			PostMessage(WM_SHOWWINDOW, FALSE, 0);
// 		}

		SetTimer(1111, 0);

		int n_input_idl = 2 * 60 * 1000;
// #ifdef _DEBUG
// 		n_input_idl = 10 * 1000;
// #endif

// #ifdef _FAST_FIND_LOGMOD_
// 		n_input_idl = 10 * 1000;
// #endif
		CRegeditIdleMsg::GetInstance()->SetCpuLimit(20);
		CRegeditIdleMsg::GetInstance()->SetLastInputLimit(n_input_idl);
		CRegeditIdleMsg::GetInstance()->SetMemoryLimit(80);
		CRegeditIdleMsg::GetInstance()->Regedit(m_hWnd, WM_SYSTEM_IDL);

		return TRUE;
	}

	LRESULT OnEndSession(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
#ifdef _FAST_FIND_LOGMOD_
		::OutputDebugStringW(L"关机或重启进入正常结束步骤");
#endif
		CloseDialog(0);
		return TRUE;
	}

	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		static int n_ctrl;
		if (wParam == SIZE_MAXIMIZED)
		{
			CRect rect_edit1 = m_rect_edit1;
			CRect rect_edit2 = m_rect_edit2;
			CRect rect_botton1;
			CRect rect_botton2;
			CRect rect_combo;
			CRect rect_radio1 = m_rect_radio1;
			CRect rect_radio2 = m_rect_radio2;
			CRect rect_radio3 = m_rect_radio3;
			CRect rect_list = m_rect_list;
			CRect rect_static = m_rect_static;

			CRect rect_client;
			GetClientRect(&rect_client);
			rect_edit1.right = rect_client.Width() + m_rect_edit1.left;
			((CEdit)(GetDlgItem(IDC_EDIT1))).MoveWindow(&rect_edit1);

			rect_radio1.left = rect_client.Width() - 237;
			rect_radio1.right = rect_radio1.left + m_rect_radio1.Width();
			((CButton)(GetDlgItem(IDC_RADIO1))).MoveWindow(&rect_radio1);

			rect_radio2.left = rect_client.Width() - 138;
			rect_radio2.right = rect_radio2.left + m_rect_radio2.Width();
			((CButton)(GetDlgItem(IDC_RADIO2))).MoveWindow(&rect_radio2);

			rect_radio3.left = rect_client.Width() - 82;
			rect_radio3.right = rect_radio3.left + m_rect_radio3.Width();
			((CButton)(GetDlgItem(IDC_RADIO3))).MoveWindow(&rect_radio3);


			rect_list.right = rect_list.left + rect_client.Width();
			rect_list.bottom = rect_list.top + rect_client.Height() - m_rect_edit1.Height() - m_rect_combo.Height() - 2 - m_rect_static.Height() - 10;
			((CListViewCtrl)(GetDlgItem(IDC_LIST1))).MoveWindow(&rect_list);
			((CListViewCtrl)(GetDlgItem(IDC_LIST1))).SetColumnWidth(0, 168);
			((CListViewCtrl)(GetDlgItem(IDC_LIST1))).SetColumnWidth(1, rect_client.Width() - 25 - 168 - 170);
			((CListViewCtrl)(GetDlgItem(IDC_LIST1))).SetColumnWidth(2, 100);
			((CListViewCtrl)(GetDlgItem(IDC_LIST1))).SetColumnWidth(3, 70);

			rect_static.right = rect_static.left + rect_client.Width();
			rect_static.top = rect_list.bottom + 3;
			rect_static.bottom = rect_static.top + m_rect_static.Height();
			((CStatic)(GetDlgItem(IDC_STATIC))).MoveWindow(&rect_static);
			return 0;
		}
		else if (wParam == SIZE_RESTORED)
		{
			if (n_ctrl == 0)
			{
				((CEdit)(GetDlgItem(IDC_EDIT2))).GetWindowRect(&m_rect_edit2);
				ScreenToClient(m_rect_edit2);
				((CEdit)(GetDlgItem(IDC_EDIT1))).GetWindowRect(&m_rect_edit1);
				ScreenToClient(m_rect_edit1);
				((CButton)(GetDlgItem(IDC_BUTTON1))).GetWindowRect(&m_rect_botton1);
				ScreenToClient(m_rect_botton1);
				((CButton)(GetDlgItem(IDC_BUTTON2))).GetWindowRect(&m_rect_botton2);
				ScreenToClient(m_rect_botton2);
				((CButton)(GetDlgItem(IDC_RADIO1))).GetWindowRect(&m_rect_radio1);
				ScreenToClient(m_rect_radio1);
				((CButton)(GetDlgItem(IDC_RADIO2))).GetWindowRect(&m_rect_radio2);
				ScreenToClient(m_rect_radio2);
				((CButton)(GetDlgItem(IDC_RADIO3))).GetWindowRect(&m_rect_radio3);
				ScreenToClient(m_rect_radio3);
				((CComboBox)(GetDlgItem(IDC_COMBO1))).GetWindowRect(&m_rect_combo);
				ScreenToClient(m_rect_combo);
				((CListViewCtrl)(GetDlgItem(IDC_LIST1))).GetWindowRect(&m_rect_list);
				ScreenToClient(m_rect_list);
				((CStatic)(GetDlgItem(IDC_STATIC))).GetWindowRect(&m_rect_static);
				ScreenToClient(m_rect_static);
				++n_ctrl;
			}
			((CEdit)(GetDlgItem(IDC_EDIT2))).MoveWindow(&m_rect_edit2);
			((CEdit)(GetDlgItem(IDC_EDIT1))).MoveWindow(&m_rect_edit1);
			((CButton)(GetDlgItem(IDC_BUTTON1))).MoveWindow(&m_rect_botton1);
			((CButton)(GetDlgItem(IDC_BUTTON2))).MoveWindow(&m_rect_botton2);
			((CButton)(GetDlgItem(IDC_RADIO1))).MoveWindow(&m_rect_radio1);
			((CButton)(GetDlgItem(IDC_RADIO2))).MoveWindow(&m_rect_radio2);
			((CButton)(GetDlgItem(IDC_RADIO3))).MoveWindow(&m_rect_radio3);
			((CComboBox)(GetDlgItem(IDC_COMBO1))).MoveWindow(&m_rect_combo);
			((CListViewCtrl)(GetDlgItem(IDC_LIST1))).MoveWindow(&m_rect_list);
			((CListViewCtrl)(GetDlgItem(IDC_LIST1))).SetColumnWidth(0, 168);
			((CListViewCtrl)(GetDlgItem(IDC_LIST1))).SetColumnWidth(1, 450);
			((CListViewCtrl)(GetDlgItem(IDC_LIST1))).SetColumnWidth(2, 85);
			((CListViewCtrl)(GetDlgItem(IDC_LIST1))).SetColumnWidth(3, 70);
			((CStatic)(GetDlgItem(IDC_STATIC))).MoveWindow(&m_rect_static);
		}
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnHotKey(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (wParam == 0x888)
		{
			if (IsWindowVisible() && m_hWnd == ::GetForegroundWindow())
			{
				ShowWindow(SW_HIDE);
				m_find_thread.CancelSearch();
				::Sleep(500);
				m_find_thread.ClearCache();
				CListViewCtrl list_box = GetDlgItem(IDC_LIST1);				
				list_box.DeleteAllItems();				
				::SetFocus(GetDlgItem(IDC_EDIT1));
				((CEdit)(GetDlgItem(IDC_EDIT1))).SetSel(0, -1);
				((CEdit)(GetDlgItem(IDC_EDIT2))).SetWindowText(L"");
				g_cstr_foldername = L"";
				return 0;
			}
			if (IsIconic())
			{
				ShowWindow(SW_RESTORE);
			}
			ShowWindow(SW_SHOW);
			_SetTop();
			::SetFocus(GetDlgItem(IDC_EDIT1));
			((CEdit)(GetDlgItem(IDC_EDIT1))).SetSel(0, -1);
		}
		return 0;
	}

	LRESULT OnCopyData(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		COPYDATASTRUCT* p_copydata = (COPYDATASTRUCT*)lParam;
		if (p_copydata && p_copydata->lpData)
		{
			CString cstr_text;
			cstr_text = (LPCWSTR)p_copydata->lpData;
			g_cstr_foldername = cstr_text;
			ConvertToLenPath(g_cstr_foldername);
			g_cstr_foldername.MakeLower();
			((CEdit)(GetDlgItem(IDC_EDIT2))).SetWindowText(g_cstr_foldername);
			CComboBox combo = GetDlgItem(IDC_COMBO1);
			combo.SetCurSel(0);
			::SetFocus(GetDlgItem(IDC_EDIT1));
			((CEdit)(GetDlgItem(IDC_EDIT1))).SetSel(0, -1);
		}
		else
		{
			CComboBox combo = GetDlgItem(IDC_COMBO1);
			combo.SetCurSel(0);
			::SetFocus(GetDlgItem(IDC_EDIT1));
			g_cstr_foldername = L"";
			((CEdit)(GetDlgItem(IDC_EDIT2))).SetWindowText(L"");
		}
		return 0;
	}


	LRESULT OnCommand(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if (LOWORD(wParam) == ID_EDIT_COPY)
		{
			if (::GetFocus() == GetDlgItem(IDC_EDIT1))
			{
				WCHAR wsz_tmp[MAX_PATH] = {0};
				::GetWindowText(GetDlgItem(IDC_EDIT1), wsz_tmp, MAX_PATH);

				HGLOBAL h_gb_text = ::GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE, MAX_PATH * sizeof(WCHAR));
				LPSTR lpData = (LPSTR)::GlobalLock(h_gb_text);
				wmemcpy((WCHAR*)(lpData), wsz_tmp, MAX_PATH);
				::GlobalUnlock(h_gb_text);
				if (NO_ERROR != ::GetLastError())
				{
					ATLASSERT(TRUE);
				}

				if (::OpenClipboard(NULL))
				{
					::EmptyClipboard();
					if (!::SetClipboardData(CF_UNICODETEXT, h_gb_text))
					{
						::GlobalFree(h_gb_text);
						//失败才释放，否则不能free，msdn这么说的
					}
					::CloseClipboard();
				}
			}
			else if (::GetFocus() == GetDlgItem(IDC_LIST1))
			{
				CListViewCtrl list_view = (CListViewCtrl)GetDlgItem(IDC_LIST1);
				int n_index = ::SendMessage(list_view.m_hWnd, LVM_GETNEXTITEM, -1, MAKELPARAM(LVIS_SELECTED, 0));
				std::vector<CString> vec_paths;
				while (-1 != n_index)
				{
					CString cstr_text;
					list_view.GetItemText(n_index, 1, cstr_text);
					vec_paths.push_back(cstr_text);
					n_index = ::SendMessage(list_view.m_hWnd, LVM_GETNEXTITEM, n_index, MAKELPARAM(LVIS_SELECTED, 0));
				}
				CopyFileToClipboard(vec_paths, TRUE);
			}
			return 0;
		}
		else if (ID_ALT_D == LOWORD(wParam))
		{
			::SetFocus(GetDlgItem(IDC_EDIT1));
			((CEdit)(GetDlgItem(IDC_EDIT1))).SetSel(0, -1);
			return 0;
		}
		else if (ID_EDIT_PASTE == LOWORD(wParam))
		{
			if (::GetFocus() == GetDlgItem(IDC_EDIT1))
			{
				((CEdit)(GetDlgItem(IDC_EDIT1))).Paste();
			}
		}
		else if (ID_ALT_ENTER == LOWORD(wParam))
		{
			if (::GetFocus() == GetDlgItem(IDC_LIST1))
			{
				CListViewCtrl list_view = (CListViewCtrl)GetDlgItem(IDC_LIST1);
				int n_index = ::SendMessage(list_view.m_hWnd, LVM_GETNEXTITEM, -1, MAKELPARAM(LVIS_SELECTED, 0));
				while (-1 != n_index)
				{
					CString cstr_text;
					list_view.GetItemText(n_index, 1, cstr_text);
					SHELLEXECUTEINFO sei;
					sei.hwnd = m_hWnd;
					sei.lpVerb = L"properties";
					sei.lpFile = (LPCWSTR)cstr_text;
					sei.lpDirectory = NULL;
					sei.lpParameters = NULL;
					sei.nShow = SW_SHOWNORMAL;
					sei.fMask = SEE_MASK_INVOKEIDLIST;
					sei.lpIDList = NULL;
					sei.cbSize = sizeof(SHELLEXECUTEINFO);
					ShellExecuteEx(&sei);
					n_index = ::SendMessage(list_view.m_hWnd, LVM_GETNEXTITEM, n_index, MAKELPARAM(LVIS_SELECTED, 0));
				}
			}
			return 0;
		}
		else if (ID_SHIFT_DEL == LOWORD(wParam))
		{
			if (::GetFocus() == GetDlgItem(IDC_LIST1))
			{
				CListViewCtrl list_view = (CListViewCtrl)GetDlgItem(IDC_LIST1);
				int n_index = ::SendMessage(list_view.m_hWnd, LVM_GETNEXTITEM, -1, MAKELPARAM(LVIS_SELECTED, 0));
				while (-1 != n_index)
				{
					CString cstr_text;
					list_view.GetItemText(n_index, 1, cstr_text);
					::DeleteFile(cstr_text);
					n_index = ::SendMessage(list_view.m_hWnd, LVM_GETNEXTITEM, n_index, MAKELPARAM(LVIS_SELECTED, 0));
				}
				n_index = ::SendMessage(list_view.m_hWnd, LVM_GETNEXTITEM, -1, MAKELPARAM(LVIS_SELECTED, 0));
				while (-1 != n_index)
				{
					list_view.DeleteItem(n_index);
					n_index = ::SendMessage(list_view.m_hWnd, LVM_GETNEXTITEM, -1, MAKELPARAM(LVIS_SELECTED, 0));
				}
			}
			return 0;
		}	
		else if (ID_DEL == LOWORD(wParam))
		{
			if (::GetFocus() == GetDlgItem(IDC_LIST1))
			{
				CListViewCtrl list_view = (CListViewCtrl)GetDlgItem(IDC_LIST1);
				int n_index = ::SendMessage(list_view.m_hWnd, LVM_GETNEXTITEM, -1, MAKELPARAM(LVIS_SELECTED, 0));
				while (-1 != n_index)
				{
					CString cstr_text;
					list_view.GetItemText(n_index, 1, cstr_text);
					SHFILEOPSTRUCT op = {0};
					WCHAR* wsz_tmp = new WCHAR[cstr_text.GetLength() + 2];
					wmemset(wsz_tmp, 0, cstr_text.GetLength() + 2);
					wmemcpy(wsz_tmp, (LPCWSTR)cstr_text, cstr_text.GetLength());
					op.hwnd = m_hWnd;
					op.wFunc = FO_DELETE;
					op.pFrom = wsz_tmp;
					op.pTo = NULL;
					op.fFlags = FOF_ALLOWUNDO | FOF_NO_UI;
					::SHFileOperation(&op);
					n_index = ::SendMessage(list_view.m_hWnd, LVM_GETNEXTITEM, n_index, MAKELPARAM(LVIS_SELECTED, 0));
				}
				n_index = ::SendMessage(list_view.m_hWnd, LVM_GETNEXTITEM, -1, MAKELPARAM(LVIS_SELECTED, 0));
				while (-1 != n_index)
				{
					list_view.DeleteItem(n_index);
					n_index = ::SendMessage(list_view.m_hWnd, LVM_GETNEXTITEM, -1, MAKELPARAM(LVIS_SELECTED, 0));
				}
				return 0;
			}
			if (::GetFocus() == GetDlgItem(IDC_EDIT1))
			{
				::SendMessage(GetDlgItem(IDC_EDIT1), WM_KEYDOWN, VK_DELETE, NULL);
			}
		}
		else if (ID_F4 == LOWORD(wParam))
		{
			if (m_b_data_all_ok)
			{
				int n_total = 0;
				for (std::size_t i = 0; i < m_vec_volums.size(); ++i)
				{
					if (m_vec_volums[i])
					{
						n_total += m_vec_volums[i]->GetVolumPointer()->size();
					}
				}
				CString cstr_result;
				cstr_result.Format(L"F4命令清屏（全盘共%d个实体）", n_total);
				((CStatic)GetDlgItem(IDC_STATIC)).SetWindowTextW(cstr_result);
				CListViewCtrl list_box = GetDlgItem(IDC_LIST1);
				list_box.SetRedraw(FALSE);
				//list_box.DeleteAllItems();
				list_box.SetItemCount(0);
				m_find_thread.ClearCache();
				list_box.SetRedraw(TRUE);
				::SetFocus(GetDlgItem(IDC_EDIT1));
				((CEdit)(GetDlgItem(IDC_EDIT1))).SetSel(0, -1);
				((CEdit)(GetDlgItem(IDC_EDIT2))).SetWindowText(L"");
				g_cstr_foldername = L"";
			}
		}
		else if (ID_CTRL_ENTER == LOWORD(wParam))
		{
			WCHAR wsz_tmp[MAX_PATH] = {0};
			::GetWindowText(GetDlgItem(IDC_EDIT1), wsz_tmp, MAX_PATH);
			stringxw cstr_text = wsz_tmp;

			cstr_text.trim();

			if (!cstr_text.empty())
			{
				stringxw wstr_url = L"http://www.so.com/s?&q=";
				wstr_url += cstr_text;
				WipeShellExecuteEx(wstr_url, NULL, SW_SHOW);
			}

		}
		else if (ID_RADIO1 == LOWORD(wParam))
		{
			((CButton)GetDlgItem(IDC_RADIO1)).SetCheck(TRUE);
			((CButton)GetDlgItem(IDC_RADIO2)).SetCheck(FALSE);
			((CButton)GetDlgItem(IDC_RADIO3)).SetCheck(FALSE);
			::SetFocus(GetDlgItem(IDC_EDIT1));
			((CEdit)(GetDlgItem(IDC_EDIT1))).SetSel(0, -1);
		}
		else if (ID_RADIO2 == LOWORD(wParam))
		{
			((CButton)GetDlgItem(IDC_RADIO2)).SetCheck(TRUE);
			((CButton)GetDlgItem(IDC_RADIO1)).SetCheck(FALSE);
			((CButton)GetDlgItem(IDC_RADIO3)).SetCheck(FALSE);
			::SetFocus(GetDlgItem(IDC_EDIT1));
			((CEdit)(GetDlgItem(IDC_EDIT1))).SetSel(0, -1);
		}
		else if (ID_RADIO3 == LOWORD(wParam))
		{
			((CButton)GetDlgItem(IDC_RADIO3)).SetCheck(TRUE);
			((CButton)GetDlgItem(IDC_RADIO2)).SetCheck(FALSE);
			((CButton)GetDlgItem(IDC_RADIO1)).SetCheck(FALSE);
			::SetFocus(GetDlgItem(IDC_EDIT1));
			((CEdit)(GetDlgItem(IDC_EDIT1))).SetSel(0, -1);
		}
		bHandled = FALSE;
		return 0;
	}


	LRESULT OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		if (1111 == wParam)
		{
			if (g_b_starthide)
			{
				ShowWindow(SW_HIDE);
			}
			else
			{
				ShowWindow(SW_SHOW);
			}
			KillTimer(1111);
		}
		return 0;
	}

	LRESULT OnUpdateDataDone(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		((CStatic)(GetDlgItem(IDC_STATIC))).SetWindowText(L"已经更新了所有硬盘索引数据");
		return 1;
	}

	LRESULT OnSysIdle(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		static int n_ctrl = 0;
		if (n_ctrl > 0)
		{
			return 1;
		}

#ifdef _FAST_FIND_LOGMOD_
		::OutputDebugStringW(L"开始更新数据\r\n");
#endif


		CRnewDataThread* thread_tmp = new (std::nothrow) CRnewDataThread(m_vec_volums, this->m_hWnd);
		if (thread_tmp)
		{
			thread_tmp->Create();
		}

		++n_ctrl;	
        return 1;
    }

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// unregister message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->RemoveMessageFilter(this);
		pLoop->RemoveIdleHandler(this);

		return 0;
	}
	LRESULT OnLoadAllFilesDone(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
        CListViewCtrl list_view = (CListViewCtrl)GetDlgItem(IDC_LIST1);
        list_view.DeleteAllItems();
		int n_total = 0;
		for (std::size_t i = 0; i < m_vec_volums.size(); ++i)
		{
			if (m_vec_volums[i])
			{
				n_total += m_vec_volums[i]->GetVolumPointer()->size();
			}
		}
		CString cstr_output;
		cstr_output.Format(L"索引建立完毕(共%d个实体)", n_total);
		((CStatic)(GetDlgItem(IDC_STATIC))).SetWindowText(cstr_output);
        m_b_can_use = true;
        return 1;
    }

	LRESULT OnRefreshList(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		CListViewCtrl list_view = (CListViewCtrl)GetDlgItem(IDC_LIST1);
		list_view.SetItemCountEx((int)lParam, LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL);
		list_view.Invalidate();
		//((CStatic)GetDlgItem(IDC_STATIC)).SetWindowTextW(L"正在搜索...");
        return 1;
    }

	LRESULT OnBeginSearch(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		CString cstr_result;
		cstr_result.Format(L"正在搜索...");
		((CStatic)GetDlgItem(IDC_STATIC)).SetWindowTextW(cstr_result);
		return 1;
	}

	LRESULT OnSearchDone(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		CListViewCtrl list_view = (CListViewCtrl)GetDlgItem(IDC_LIST1);	
		BOOL bRet = list_view.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		int n_total = 0;
		for (std::size_t i = 0; i < m_vec_volums.size(); ++i)
		{
			if (m_vec_volums[i])
			{
				n_total += m_vec_volums[i]->GetVolumPointer()->size();
			}
		}

		CString cstr_result;
		cstr_result.Format(L"共找到%d个（全盘共%d个实体）", (int)lParam, n_total);
		((CStatic)GetDlgItem(IDC_STATIC)).SetWindowTextW(cstr_result);
		m_b_data_all_ok = true;
        return 1;
    }

	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CAboutDlg dlg;
		dlg.DoModal();
		return 0;
	}

	LRESULT OnMenuPos(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
        CString szParam;
        szParam.Format(_T("/select, %s"), m_map_pos_id_to_string[wID].c_str());
        ::ShellExecute(NULL, L"open", L"explorer.exe", szParam, NULL, SW_SHOWNORMAL);
        return 1;
    }

	

	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
        if (!m_b_can_use)
        {
            return 1;
        }

		HWND h_focus = ::GetFocus();
		if (h_focus != GetDlgItem(IDC_EDIT1))
		{
			if (h_focus == GetDlgItem(IDC_LIST1))
			{
				CListViewCtrl list_view = (CListViewCtrl)GetDlgItem(IDC_LIST1);

				int n_index = ::SendMessage(list_view.m_hWnd, LVM_GETNEXTITEM, -1, MAKELPARAM(LVIS_SELECTED, 0));
				//int n_index = list_view.GetSelectedIndex();
				while (-1 != n_index)
				{
					CString cstr_text;	
					list_view.GetItemText(n_index, 1, cstr_text);
					HINSTANCE hins = ShellExecute(0, L"open", cstr_text, 0, 0, SW_SHOWNORMAL);
					if ((int)hins <= 32)
					{
						CString szParam;
						szParam.Format(_T("/select, %s"), cstr_text);
						::ShellExecute(NULL, L"open", L"explorer.exe", szParam, NULL, SW_SHOWNORMAL);
					}
					n_index = ::SendMessage(list_view.m_hWnd, LVM_GETNEXTITEM, n_index, MAKELPARAM(LVIS_SELECTED, 0));
				}
				return 1;
			}
		}

		WCHAR wsz_tmp[MAX_PATH] = {0};
		::GetWindowText(GetDlgItem(IDC_EDIT1), wsz_tmp, MAX_PATH);
		stringxw cstr_text = wsz_tmp;

        cstr_text.trim();

        if (cstr_text.empty())
        {
            return 1;
        }
        cstr_text.to_lower();
	
        stringxw wstr_search(cstr_text);
        stringxa str_search_u8(wstr_search, CP_UTF8);

        

        CComboBox combo = GetDlgItem(IDC_COMBO1);
		wmemset(wsz_tmp, 0, MAX_PATH);
		::GetWindowText(GetDlgItem(IDC_COMBO1), wsz_tmp, MAX_PATH);

        CListViewCtrl list_box = GetDlgItem(IDC_LIST1);
        //list_box.SetRedraw(FALSE);
        //list_box.DeleteAllItems();
		list_box.SetItemCount(0);

		if (cstr_text == L".clear")
		{
			int n_total = 0;
			for (std::size_t i = 0; i < m_vec_volums.size(); ++i)
			{
				if (m_vec_volums[i])
				{
					n_total += m_vec_volums[i]->GetVolumPointer()->size();
				}
			}
			CString cstr_result;
			cstr_result.Format(L".clear命令清屏（全盘共%d个实体）",n_total);
			((CStatic)GetDlgItem(IDC_STATIC)).SetWindowTextW(cstr_result);
			list_box.SetRedraw(TRUE);
			return 0;
		}


		if (cstr_text == L".export")
		{
			int n_total = 0;
			for (std::size_t i = 0; i < m_vec_volums.size(); ++i)
			{
				if (m_vec_volums[i])
				{
					n_total += m_vec_volums[i]->GetVolumPointer()->size();
				}
			}
			CString cstr_result;
			cstr_result.Format(L".export命令输出当前所有结果到硬盘（全盘共%d个实体）", n_total);
			((CStatic)GetDlgItem(IDC_STATIC)).SetWindowTextW(cstr_result);

			std::vector<WCHAR*>& vec_tmp = m_find_thread.GetSearchAllData();

			stringxw wstr_path;
			get_app_path(wstr_path);
			wstr_path += L"fastfind_debug.txt";
			FILE* file = _wfopen(wstr_path, L"wb");

			for (std::size_t i = 0; i < vec_tmp.size(); ++i)
			{
				if (vec_tmp[i])
				{	
					stringxa str_path(vec_tmp[i]);
					if (file)
					{
						fwrite((const void*)str_path.c_str(), 1, str_path.size(), file);
						short n_end = 0x0a0d;
						fwrite((const void*)&n_end, 2, 1, file);
					}
					
				}
			}

			if (file)
			{
				fclose(file);
			}
			return 0;
		}

		if (cstr_text == L".exit")
		{
			__super::DestroyTray();
			CloseDialog(0);
		}


		m_b_data_all_ok = false;
		CString cstr_combo(wsz_tmp);
		int n_state = 0;
		if (((CButton)GetDlgItem(IDC_RADIO1)).GetCheck())
		{
			n_state = 0; //all
		}
		else if (((CButton)GetDlgItem(IDC_RADIO2)).GetCheck())
		{
			n_state = 1; //file
		}
		else if (((CButton)GetDlgItem(IDC_RADIO3)).GetCheck())
		{
			n_state = 2; //dir
		}

        if (cstr_combo == L"我的电脑")
        {
			int n_count = combo.GetCount();
			int n_index = -1;
			for (int i = 0; i < n_count; ++i)
			{
				if (i == 0)
				{
					continue;
				}
				CString cstr_combo_text;
				combo.GetLBText(i, cstr_combo_text);
				if (g_cstr_foldername.Find(cstr_combo_text) != -1)
				{
					n_index = i;
					break;
				}
			}
			if (n_index != -1)
			{
				n_index -= 1;
			}
            m_find_thread.Search(str_search_u8, n_index, n_state);
        }
        else
        {
            m_find_thread.Search(str_search_u8, combo.GetCurSel() - 1, n_state);
        }
		::SetFocus(GetDlgItem(IDC_LIST1));

		((CStatic)(GetDlgItem(IDC_STATIC))).SetWindowText(L"");
		return 0;
	}

	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		ShowWindow(SW_HIDE);
		m_find_thread.CancelSearch();
		::Sleep(500);
		m_find_thread.ClearCache();
		CListViewCtrl list_box = GetDlgItem(IDC_LIST1);
		list_box.DeleteAllItems();		
		::SetFocus(GetDlgItem(IDC_EDIT1));
		((CEdit)(GetDlgItem(IDC_EDIT1))).SetSel(0, -1);
		((CEdit)(GetDlgItem(IDC_EDIT2))).SetWindowText(L"");
		g_cstr_foldername = L"";
		return 0;
	}

	void CloseDialog(int nVal)
	{
		ShowWindow(SW_HIDE);
		::UnregisterHotKey(m_hWnd, 0x888);
		for (std::size_t i = 0; i < m_vec_volums.size(); ++i)
		{
			if (m_vec_volums[i])
			{
				m_vec_volums[i]->WriteLocalCache();
			}
		}
		::TerminateProcess(::GetCurrentProcess(), 0);
	}

protected:

    int _IniMFT(const CString& cstr_volum)
    {
        bool b_ini = false;
        for (size_t i = 0; i < m_vec_volums.size(); ++i)
        {
            if (m_vec_volums[i]->GetVolum() == (LPCWSTR)cstr_volum)
            {
                b_ini = true;
                return i;
            }
        }
        if (!b_ini)
        {
            CEnumMFT* mft_tmp = new (std::nothrow) CEnumMFT;
            mft_tmp->SetVolum((LPCWSTR)cstr_volum);
            m_vec_volums.push_back(mft_tmp);
        }
        return m_vec_volums.size() - 1;
    }

	/*shell menu*/

	int CopyFileToClipboard(const std::vector<CString>& vec_paths, BOOL b_copy_or_cut)
	{
		UINT uDropEffect;
		HGLOBAL hGblEffect;
		LPDWORD lpdDropEffect;
		DROPFILES stDrop;
		HGLOBAL hGblFiles;
		LPSTR lpData;

		//与剪贴板交互的数据格式并非标准剪贴板格式，因此需要注册
		uDropEffect = ::RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT);
		//为剪贴板数据分配空间
		hGblEffect = ::GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE, sizeof(DWORD));
		lpdDropEffect = (LPDWORD)::GlobalLock(hGblEffect);
		//设置操作的模式
		*lpdDropEffect = b_copy_or_cut ? DROPEFFECT_COPY : DROPEFFECT_MOVE;
		::GlobalUnlock(hGblEffect);

		//剪贴板数据需要一个这个结构
		stDrop.pFiles = sizeof(DROPFILES);
		stDrop.pt.x = 0;
		stDrop.pt.y = 0;
		stDrop.fNC = FALSE;
		stDrop.fWide = TRUE; //Unicode

		//分配数据空间，并且预留文件名的空间
		//文件列表为两个'\0'结尾，因此需要多分配2个字节
		int n_len = 0;
		for (std::size_t i = 0; i < vec_paths.size(); ++i)
		{
			n_len += vec_paths[i].GetLength() * sizeof(WCHAR);
			n_len += sizeof(WCHAR);
		}
		hGblFiles = ::GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE, sizeof(DROPFILES) + n_len + sizeof(WCHAR));
		lpData = (LPSTR)::GlobalLock(hGblFiles);
		memcpy(lpData, &stDrop, sizeof(DROPFILES));
		memset((BYTE*)(lpData + sizeof(DROPFILES)), 0, n_len + sizeof(WCHAR));
		WCHAR* wsz_tmp = (WCHAR*)(lpData + sizeof(DROPFILES));
		for (std::size_t i = 0; i < vec_paths.size(); ++i)
		{
			wmemcpy(wsz_tmp, (LPCWSTR)(vec_paths[i]), vec_paths[i].GetLength());
			wsz_tmp += vec_paths[i].GetLength();
			wsz_tmp += 1;
		}
		::GlobalUnlock(hGblFiles);

		//普通的剪贴板操作
		if (::OpenClipboard(NULL))
		{
			::EmptyClipboard();
			//设置剪贴板文件信息
			if (!::SetClipboardData(CF_HDROP, hGblFiles))
			{
				::GlobalFree(hGblFiles);
			}
			//设置剪贴板操作信息
			if (!::SetClipboardData(uDropEffect, hGblEffect))
			{
				::GlobalFree(hGblEffect);
			}
			::CloseClipboard();
		}		
		return 1;
	}


	int CopyFileToClipboard(const WCHAR* wsz_filename, BOOL b_copy_or_cut)
	{
		UINT uDropEffect;
		HGLOBAL hGblEffect;
		LPDWORD lpdDropEffect;
		DROPFILES stDrop;
		HGLOBAL hGblFiles;
		LPSTR lpData;

		//与剪贴板交互的数据格式并非标准剪贴板格式，因此需要注册
		uDropEffect = ::RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT);
		//为剪贴板数据分配空间
		hGblEffect = ::GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE, sizeof(DWORD));
		lpdDropEffect = (LPDWORD)::GlobalLock(hGblEffect);
		//设置操作的模式
		*lpdDropEffect = b_copy_or_cut ? DROPEFFECT_COPY : DROPEFFECT_MOVE;
		::GlobalUnlock(hGblEffect);

		//剪贴板数据需要一个这个结构
		stDrop.pFiles = sizeof(DROPFILES);
		stDrop.pt.x = 0;
		stDrop.pt.y = 0;
		stDrop.fNC = FALSE;
		stDrop.fWide = TRUE; //Unicode

		//分配数据空间，并且预留文件名的空间
		//文件列表为两个'\0'结尾，因此需要多分配2个字节
		hGblFiles = ::GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE, sizeof(DROPFILES) + (wcslen(wsz_filename) + 2) * sizeof(WCHAR));
		lpData = (LPSTR)::GlobalLock(hGblFiles);
		memcpy(lpData, &stDrop, sizeof(DROPFILES));
		wmemset((WCHAR*)(lpData + sizeof(DROPFILES)), 0, wcslen(wsz_filename) + 2);
		wmemcpy((WCHAR*)(lpData + sizeof(DROPFILES)), wsz_filename, wcslen(wsz_filename));
		::GlobalUnlock(hGblFiles);

		//普通的剪贴板操作
		if (::OpenClipboard(NULL))
		{
			::EmptyClipboard();
			//设置剪贴板文件信息
			if (!::SetClipboardData(CF_HDROP, hGblFiles))
			{
				::GlobalFree(hGblFiles);
			}
			//设置剪贴板操作信息
			if (!::SetClipboardData(uDropEffect, hGblEffect))
			{
				::GlobalFree(hGblEffect);
			}
			::CloseClipboard();
		}		
		return 1;
	}

	void UseShellMenu(CString pFullPathFileName)   
	{   
		pFullPathFileName.MakeLower();
		WCHAR* pDest = NULL;   
		WCHAR* pFile = NULL;
		pFile = pFullPathFileName.GetBuffer(pFullPathFileName.GetLength());   
		pDest = wcsrchr(pFile, L'\\');
		pDest++;
		CComPtr<IShellFolder> pMyFolder;
		RetreiveIShellFolder(pFile, pMyFolder);
		if (!pMyFolder)
		{
			return;
		}
		CString cstr_file(pDest);
		ITEMIDLIST* rgelt = RetreiveItemIDList(pMyFolder, (LPCWSTR)cstr_file, 1 == dir_file_exist((LPCWSTR)pFullPathFileName));
		pFullPathFileName.ReleaseBuffer();
		if (!pMyFolder)
		{
			return;
		}
		CComPtr<IContextMenu> lpCm;
		pMyFolder->GetUIObjectOf(NULL, 1, (const struct _ITEMIDLIST **)&rgelt, IID_IContextMenu, 0, (LPVOID*)&lpCm);
		if (!lpCm)
		{
			return;
		}
		CMenu menu;
		menu.CreatePopupMenu();
		lpCm->QueryContextMenu(menu.m_hMenu, 0, 1, 0x7fff, /*CMF_NORMAL | */CMF_EXPLORE);
		POINT pt;
		GetCursorPos(&pt);

		MENUITEMINFO MenuInfo = {0};  
		MenuInfo.cbSize = sizeof(MENUITEMINFO);
		MenuInfo.fMask = MIIM_ID | MIIM_STRING;  
		MenuInfo.fType = MFT_STRING;
		MenuInfo.wID = 0x111;
		MenuInfo.dwTypeData = L"定位";
		MenuInfo.cch = 2;
		menu.InsertMenuItem(1, TRUE, &MenuInfo);
		UINT idCmd = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, m_hWnd);
		if (idCmd)
		{
			if (idCmd == 0x111)
			{
				CString szParam;
				szParam.Format(_T("/select, %s"), (LPCWSTR)pFullPathFileName);
				::ShellExecute(NULL, L"open", L"explorer.exe", szParam, NULL, SW_SHOWNORMAL);
			}
			else
			{
				CMINVOKECOMMANDINFO cmi = {0};
				cmi.cbSize = sizeof(CMINVOKECOMMANDINFO);
				cmi.fMask = 0;
				cmi.hwnd = m_hWnd;
				cmi.lpVerb = MAKEINTRESOURCEA(idCmd - 1);
				cmi.lpParameters = NULL;
				cmi.lpDirectory = NULL;
				cmi.nShow = SW_SHOWNORMAL;
				cmi.dwHotKey = 0;
				cmi.hIcon = NULL;
				HRESULT hr = E_FAIL;
				if (25 == idCmd)
				{
					CopyFileToClipboard(pFullPathFileName, FALSE);
				}
				else if (26 == idCmd)
				{
					CopyFileToClipboard(pFullPathFileName, TRUE);
				}
				else
				{
					hr = lpCm->InvokeCommand(&cmi);
				}
				
			}
		}
	}


	void RetreiveIShellFolder(const CComPtr<IShellFolder>& pParentFolder, DWORD grfFlags, const WCHAR* ItemName, CComPtr<IShellFolder>& sp_out)   
	{   
		CComPtr<IEnumIDList> lpEnumIDList;   
		HRESULT hr = pParentFolder->EnumObjects(NULL, grfFlags, &lpEnumIDList);
		if(SUCCEEDED(hr) && lpEnumIDList)
		{
			LPITEMIDLIST rgelt;   
			ULONG pceltFetched = 0;   
			STRRET Name;
			while(lpEnumIDList->Next(1, &rgelt, &pceltFetched) == NOERROR)
			{
				pParentFolder->GetDisplayNameOf(rgelt, SHGDN_FORPARSING | SHGDN_FOREDITING | SHGDN_FORADDRESSBAR , &Name);
				if(Name.uType == STRRET_WSTR && Name.pOleStr)
				{
					stringxw wstr_name(Name.pOleStr);
					wstr_name.to_lower();
					if (wstr_name.size() > 3)
					{
						wstr_name.trim_right(L'\\');
						wstr_name = wstr_name.substr(wstr_name.find_last_of(L'\\') + 1);
					}
					if(wstr_name == stringxw(ItemName))
					{
						pParentFolder->BindToObject(rgelt, NULL, IID_IShellFolder, (LPVOID*)&sp_out);
						::CoTaskMemFree(Name.pOleStr);
						break;
					}
					::CoTaskMemFree(Name.pOleStr);
				}
			}
		}
	}

	ITEMIDLIST* RetreiveItemIDList(const CComPtr<IShellFolder>& pFolder, const WCHAR* ItemName, bool b_file_dir)   
	{   
		CComPtr<IEnumIDList> lpEnumIDList; 
		DWORD sh_folder_file = b_file_dir ? (SHCONTF_NONFOLDERS | SHCONTF_INCLUDEHIDDEN) : (SHCONTF_FOLDERS | SHCONTF_INCLUDEHIDDEN); 
		pFolder->EnumObjects(NULL, sh_folder_file, &lpEnumIDList); 
		if (!lpEnumIDList)
		{
			return NULL;
		}
		LPITEMIDLIST rgelt = NULL;   
		ULONG pceltFetched = 0;   
		STRRET Name;
		while(lpEnumIDList->Next(1, &rgelt, &pceltFetched) == NOERROR)
		{
			pFolder->GetDisplayNameOf(rgelt, SHGDN_FORPARSING | SHGDN_FOREDITING | SHGDN_FORADDRESSBAR, &Name);
			if(Name.uType == STRRET_WSTR && Name.pOleStr)
			{
				stringxw wstr_name(Name.pOleStr);
				wstr_name.to_lower();
				if (wstr_name.size() > 3)
				{
					wstr_name.trim_right(L'\\');
					wstr_name = wstr_name.substr(wstr_name.find_last_of(L'\\') + 1);
				}
				if(wstr_name == stringxw(ItemName))
				{   		
					::CoTaskMemFree(Name.pOleStr);			
					break;   
				}
				::CoTaskMemFree(Name.pOleStr);	
			}
		}
		return rgelt;
	}   

	void RetreiveIShellFolder(const CString& filePath, CComPtr<IShellFolder>& sp_out)   
	{   
		CString szPath;
		CString szDriverRoot;   
		CString szDriver;
		szDriverRoot = filePath.Left(3);
		szDriver = filePath.Left(2);
		szPath = filePath.Right(filePath.GetLength() - 3);
		CComPtr<IShellFolder> pDeskFolder;   
		CComPtr<IShellFolder> pDrivers;
		LPITEMIDLIST pItemIDList = NULL;   
		SHGetDesktopFolder(&pDeskFolder);
		SHGetSpecialFolderLocation(NULL, CSIDL_DRIVES, &pItemIDList);
		if (pDeskFolder)
		{
			pDeskFolder->BindToObject(pItemIDList, NULL, IID_IShellFolder, (LPVOID*)&pDrivers);
			if (pDrivers)
			{
				CComPtr<IShellFolder> pDriver;
				RetreiveIShellFolder(pDrivers, SHCONTF_FOLDERS, ((LPCWSTR)szDriverRoot), pDriver);
				if (!pDriver)
				{
					return;
				}
				if (wcslen(szPath) == 0)
				{
					sp_out = pDriver;
				}
				else
				{
					stringxw wstr_path(szPath);
					std::vector<stringxw> vec_path;
					wstr_path.split_string(L"\\", vec_path);
					if (vec_path.empty())
					{
						vec_path.push_back(wstr_path);
					}
					vec_path.pop_back();
					for (std::size_t i = 0; i < vec_path.size() && pDriver; ++i)
					{
						CComPtr<IShellFolder> sp_tmp;
						RetreiveIShellFolder(pDriver, SHCONTF_FOLDERS | SHCONTF_INCLUDEHIDDEN | SHCONTF_FLATLIST | SHCONTF_ENABLE_ASYNC | SHCONTF_STORAGE | SHCONTF_SHAREABLE, (WCHAR*)vec_path[i].c_str(), sp_tmp);
						pDriver = sp_tmp;
					}
					sp_out = pDriver;
				}
			}
		}
		else
		{
			return;
		}
	}
	/*shell menu*/


	void _SetTop()
	{
		::AttachThreadInput(GetWindowThreadProcessId(::GetForegroundWindow(),NULL), GetCurrentThreadId(),TRUE); //置我们的为焦点窗口
		::SetForegroundWindow(m_hWnd); 
		::SetFocus(m_hWnd); //释放thread 
		::AttachThreadInput(GetWindowThreadProcessId( ::GetForegroundWindow(),NULL), GetCurrentThreadId(),FALSE);
	}

	int _ConvertFileSize(INT64 n_size, stringxw& wstr_size)
	{
		int n_ret = 0;
		FUNCTION_BEGIN;
		INT64 n64_size = n_size;
		if (((n64_size / (1024 * 1024 * 1024)) / 1024)) //TB
		{
			wstr_size.format(L"%.1f TB", (((double)n64_size / (1024 * 1024 * 1024)) / 1024));
		}
		else if (n64_size / (1024 * 1024 * 1024)) //GB
		{
			if (n64_size / (1024 * 1024 * 1024) >= 1000)
			{
				wstr_size = L"1.0 TB";
			}
			else
				wstr_size.format(L"%.1f GB", (double)n64_size / (1024 * 1024 * 1024));
		}
		else if (n64_size / (1024 * 1024)) //MB
		{
			if (n64_size / (1024 * 1024) >= 1000)
			{
				wstr_size = L"1.0 GB";
			}
			else
			{
				long double db_tmp = n64_size;
				db_tmp = db_tmp / (1024 * 1024);
				wstr_size.format(L"%.1f MB", db_tmp);
			}

		}
		else if (n64_size / 1024) //KB
		{
			if (n64_size / 1024 >= 1000)
			{
				wstr_size = L"1.0 MB";
			}
			else
			{
				long double db_tmp = n64_size;
				db_tmp = db_tmp / (1024);
				wstr_size.format(L"%3.1f KB", db_tmp);

			}
		}
		else //Bytes
		{
			if (n64_size >= 1000)
			{
				wstr_size = L"1.0 KB";
			}
			else
			{
				wstr_size.format(L"%d", (int)n_size);
				wstr_size += L"Bytes";
			}
		}
		FUNCTION_END;
		return n_ret;
	}

	bool _IsItemInSelectedItems(int index)
	{
		CListViewCtrl list_view = (CListViewCtrl)GetDlgItem(IDC_LIST1);
		int n_index = ::SendMessage(list_view.m_hWnd, LVM_GETNEXTITEM, -1, MAKELPARAM(LVIS_SELECTED, 0));		
		while (-1 != n_index)
		{
			if (index == n_index)
			{
				return true;
			}			
			n_index = ::SendMessage(list_view.m_hWnd, LVM_GETNEXTITEM, n_index, MAKELPARAM(LVIS_SELECTED, 0));
		}
		return false;
	}

private:
    std::vector<CString> m_vec_disks;
    std::vector<CEnumMFT*> m_vec_volums;
    CFindThread m_find_thread;
    bool m_b_can_use;
    CListViewCtrl m_listview;
    std::map<int, stringxw> m_map_pos_id_to_string;
	CRect m_rect_edit1;
	CRect m_rect_edit2;
	CRect m_rect_botton1;
	CRect m_rect_botton2;
	CRect m_rect_combo;
	CRect m_rect_radio1;
	CRect m_rect_radio2;
	CRect m_rect_radio3;
	CRect m_rect_list;
	CRect m_rect_static;
	HACCEL m_hAccel;
	bool m_b_data_all_ok;
	typedef struct  
	{
		stringxw wstr_path;
		tag_file_info file_info;
	}INNER_FILE_INFO;

	INNER_FILE_INFO m_file_info;
	
public:
	bool m_b_sort_ctrl_name;
	bool m_b_sort_ctrl_path;
	bool m_b_sort_ctrl_size;
	bool m_b_sort_ctrl_time;
public:
	LRESULT OnBnClickedButton1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CString strFilter;
		CString str;
		BROWSEINFO bi;
		WCHAR name[MAX_PATH];
		ZeroMemory(&bi, sizeof(BROWSEINFO));
		bi.hwndOwner = m_hWnd;
		bi.pszDisplayName = name;
		bi.lpszTitle = L"请选择文件夹";
		bi.ulFlags = BIF_USENEWUI;
		LPITEMIDLIST idl = SHBrowseForFolder(&bi);
		if(idl == NULL)
		{
			return 1;
		}
		SHGetPathFromIDList(idl, str.GetBuffer(MAX_PATH));
		str.ReleaseBuffer();
		strFilter = str;
		if(str.GetLength() > 0 && str.GetAt(str.GetLength()-1) != L'\\')
			strFilter += L"\\";
		((CEdit)(GetDlgItem(IDC_EDIT2))).SetWindowText(strFilter);
		g_cstr_foldername = strFilter;
		ConvertToLenPath(g_cstr_foldername);
		g_cstr_foldername.MakeLower();
		LPMALLOC pMalloc;   
		if(SUCCEEDED(SHGetMalloc(&pMalloc)))   
		{   
			pMalloc->Free(idl);   
			pMalloc->Release();   
		}
		return 0;
	}
	LRESULT OnBnClickedButton2(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		((CEdit)(GetDlgItem(IDC_EDIT2))).SetWindowText(L"");
		g_cstr_foldername = L"";
		return 0;
	}
};