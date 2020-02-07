#pragma once

// ������ע�����Ϣ
#define	 WM_TRAY WM_USER+7864

template<class T>
class CNBTray
{
public:
	CNBTray(void)
	{
		m_hIcon = NULL;
		ZeroMemory( &m_nid,sizeof(NOTIFYICONDATA) );
		WM_TASKBARCREATED = RegisterWindowMessage(_T("360FastFind"));
	}
	virtual ~CNBTray(void)
	{
		if( m_hIcon!=NULL )
			DestroyIcon(m_hIcon);
	}

	BEGIN_MSG_MAP(CNBTray)
		MESSAGE_HANDLER(WM_TASKBARCREATED, OnTaskBarCreated)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_TRAY, OnTray)
	END_MSG_MAP()

	HRESULT OnDestroy( UINT, WPARAM, LPARAM, BOOL& bHandled)
	{
		bHandled = FALSE;
		DestroyTray();
		return 0;
	}
	// �������ؽ���Ϣ��Ӧ����
	HRESULT OnTaskBarCreated( UINT, WPARAM, LPARAM, BOOL& )
	{
		CreateTray();
		return 0;

	}
	// ��Ӧ�����¼�
	HRESULT OnTray( UINT, WPARAM, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		T* pT = static_cast<T*>(this);
		return pT->OnTrayMessage(lParam);
	}
	// ��������ͼ��
	BOOL CreateTray( HICON hIcon=NULL )
	{
		if( hIcon!=NULL && m_hIcon!=hIcon )
			m_hIcon = hIcon;
		ATLASSERT( m_hIcon!=NULL );
		T* pT = static_cast<T*>(this);
		m_nid.cbSize = sizeof(NOTIFYICONDATA);
		m_nid.uID = (UINT)this;
		m_nid.hIcon = m_hIcon;
		m_nid.hWnd = pT->m_hWnd;
		m_nid.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP;
		m_nid.uCallbackMessage = WM_TRAY;
		lstrcpy(m_nid.szTip,m_strTip);
		return Shell_NotifyIcon(NIM_ADD,&m_nid);

	}
	// ɾ������ͼ��
	VOID DestroyTray()
	{
		ShowTray(FALSE);
	}
	// �޸�����ͼ��
	HICON ChangeTrayIcon( HICON hIcon )
	{
		if( m_hIcon==hIcon )
			return NULL;
		HICON h = m_hIcon;
		m_hIcon = hIcon;
		m_nid.uFlags = NIF_ICON;
		m_nid.hIcon = m_hIcon;
		Shell_NotifyIcon(NIM_MODIFY,&m_nid);
		return h;
	}
	// �޸�����ͼ����ʾ�ı�
	VOID ChangeTrayTip( LPCTSTR lpszTip )
	{
		m_strTip = lpszTip;
		m_nid.uFlags = NIF_TIP;
		lstrcpyn(m_nid.szTip,lpszTip,63);
		Shell_NotifyIcon(NIM_MODIFY,&m_nid);
	}
	// ��������ͼ����ʾ
	VOID ShowTray( BOOL bShow )
	{

		m_nid.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP;
		Shell_NotifyIcon(bShow?NIM_ADD:NIM_DELETE,&m_nid);
	}
	//����
	VOID ShowBalloon(UINT n_timeout, LPCWSTR wsz_title, LPCWSTR wsz_info)
	{
		m_nid.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP|NIF_INFO;
		m_nid.dwInfoFlags = NIIF_INFO;
		m_nid.uTimeout = n_timeout;
		if (wsz_title && wcslen(wsz_title) < 64)
		{
			wcsncpy_s(m_nid.szInfoTitle, wsz_title, wcslen(wsz_title) + 1);
		}
		if (wsz_info && wcslen(wsz_info) < 256)
		{
			wcsncpy_s(m_nid.szInfo, wsz_info, wcslen(wsz_info) + 1);
		}
		Shell_NotifyIcon(NIM_MODIFY, &m_nid);
	}

protected:
	// ��Ӧ������Ϣ����T����ʵ��
	LRESULT OnTrayMessage( UINT uMsg )
	{
		return 0;
	}

protected:
	// ��Ӧ�������ؽ���ϢID
	UINT WM_TASKBARCREATED;
	// ����֪ͨ���ݽṹ
	NOTIFYICONDATA m_nid;
	// ��ǰ����ͼ��
	HICON m_hIcon;
	// ������ʾ
	CString m_strTip;
};