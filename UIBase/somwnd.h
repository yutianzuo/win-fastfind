/********************************************************************
	created:	2009/10/12
	created:	12:10:2009   10:58
	file base:	somwnd
	file ext:	h
	author:		wanlixin
	
	purpose:	SOM 界面库的基础
*********************************************************************/
#pragma once

#define SOMWNDMANAGER_TIMERID	54321

enum SOMWND_STYLE
{
	SOMWND_VISIBLE		= 0x1u,
	SOMWND_AUTOSIZE		= 0x2u,
	SOMWND_DISABLED		= 0x4u,
	SOMWND_TRANSPARENT	= 0x8u,
};

class CSomWnd;

class CSomToolTipAdpater;
class CSomWndManager;
class CSomListView;
class CSomListBox;

class __declspec(novtable) ISomWndManager
{
public:
	virtual void AttachWindow(CSomWnd* pSomWnd) = 0;
	virtual void DetachWindow(CSomWnd* pSomWnd) = 0;
	virtual void SomSetCapture(CSomWnd* pSomWnd) = 0;
	virtual void SomReleaseCapture(CSomWnd* pSomWnd) = 0;
	virtual LRESULT SomSendMessage(UINT Msg, WPARAM wParam, LPARAM lParam) = 0;
	virtual LRESULT SomSendCommandMessage(CSomWnd* pSomWnd, WORD wNotifyCode, WORD wID) = 0;
	virtual BOOL SomPostMessage(UINT Msg, WPARAM wParam, LPARAM lParam) = 0;
	virtual void SomSetWindowPos(CSomWnd* pSomWnd, int x, int y, int cx, int cy) = 0;
	virtual void SomSizeWindow(CSomWnd* pSomWnd, int cx, int cy) = 0;
	virtual void SomMoveWindow(CSomWnd* pSomWnd, int x, int y) = 0;
	virtual void SomRedrawWindow(CSomWnd* pSomWnd, LPCRECT lpRectUpdate, BOOL bRedrawInvisible, LPVOID lpRedrawParam) = 0;
	virtual void SomAddTooltip(CSomWnd* pSomWnd, UINT_PTR uInnerTipId, LPRECT lpRect) = 0;
	virtual void SomDelTooltip(CSomWnd* pSomWnd) = 0;
	virtual void SomDelayUpdateWindow() = 0;
	virtual void SomCommitUpdateWindow() = 0;
private:
};

class __declspec(novtable) CSomWnd
{
public:
	friend CSomToolTipAdpater;
	friend CSomWndManager;
	friend CSomListView;
	friend CSomListBox;

	CSomWnd()
	{
		ResetSomWnd();
	}
	virtual ~CSomWnd() {}
	virtual void Release()
	{
		delete this;
	}
	void CreateWithoutEvent(ISomWndManager* pWndManager, void* pWndManagerParam = NULL, void* pWndManagerParam2 = NULL, int x = 0, int y = 0, int cx = 0, int cy = 0, UINT nID = 0, DWORD dwStyle = SOMWND_VISIBLE | SOMWND_AUTOSIZE)
	{
		ATLASSERT(m_pWndManager == NULL);

		m_pWndManager		= pWndManager;
		m_pWndManagerParam	= pWndManagerParam;
		m_pWndManagerParam2	= pWndManagerParam2;

		m_dwStyle			= dwStyle;
		m_nID				= nID;

		m_rcWindow.left		= x;
		m_rcWindow.top		= y;
		m_rcWindow.right	= x + cx;
		m_rcWindow.bottom	= y + cy;

		m_pWndManager->AttachWindow(this);
	}
	void CreateNotifyEvent()
	{
		OnCreate();
		OnMove(m_rcWindow.left, m_rcWindow.top);
		OnSize(m_rcWindow.right - m_rcWindow.left, m_rcWindow.bottom - m_rcWindow.top);
	}
	void Create(ISomWndManager* pWndManager, DWORD dwStyle = SOMWND_VISIBLE | SOMWND_AUTOSIZE)
	{
		Create(pWndManager, NULL, NULL, 0, 0, 0, 0, 0, dwStyle);
	}
	void Create(ISomWndManager* pWndManager, int x, int y, DWORD dwStyle = SOMWND_VISIBLE | SOMWND_AUTOSIZE)
	{
		Create(pWndManager, NULL, NULL, x, y, 0, 0, 0, dwStyle);
	}
	void Create(ISomWndManager* pWndManager, int x, int y, int cx, int cy, DWORD dwStyle = SOMWND_VISIBLE | SOMWND_AUTOSIZE)
	{
		Create(pWndManager, NULL, NULL, x, y, cx, cy, 0, dwStyle);
	}
	void CreateControl(ISomWndManager* pWndManager, UINT nID, DWORD dwStyle = SOMWND_VISIBLE | SOMWND_AUTOSIZE)
	{
		Create(pWndManager, NULL, NULL, 0, 0, 0, 0, nID, dwStyle);
	}
	void CreateControl(ISomWndManager* pWndManager, int x, int y, UINT nID, DWORD dwStyle = SOMWND_VISIBLE | SOMWND_AUTOSIZE)
	{
		Create(pWndManager, NULL, NULL, x, y, 0, 0, nID, dwStyle);
	}
	void CreateControl(ISomWndManager* pWndManager, int x, int y, int cx, int cy, UINT nID, DWORD dwStyle = SOMWND_VISIBLE | SOMWND_AUTOSIZE)
	{
		Create(pWndManager, NULL, NULL, x, y, cx, cy, nID, dwStyle);
	}
	void Create(ISomWndManager* pWndManager, void* pWndManagerParam, void* pWndManagerParam2, int x, int y, int cx, int cy, UINT nID, DWORD dwStyle)
	{
		ATLASSERT(m_pWndManager == NULL);

		m_pWndManager		= pWndManager;
		m_pWndManagerParam	= pWndManagerParam;
		m_pWndManagerParam2	= pWndManagerParam2;

		m_dwStyle			= dwStyle;
		m_nID				= nID;

		m_rcWindow.left		= x;
		m_rcWindow.top		= y;
		m_rcWindow.right	= x + cx;
		m_rcWindow.bottom	= y + cy;

		m_pWndManager->AttachWindow(this);

		OnCreate();
		OnMove(x, y);
		OnSize(cx, cy);
	}
	void Destroy()
	{
		OnDestroy();

		m_pWndManager->DetachWindow(this);

		ResetSomWnd();
	}
	BOOL IsWindowEnabled() const
	{
		ATLASSERT(m_pWndManager != NULL);
		return (m_dwStyle & SOMWND_DISABLED) == 0;
	}
	BOOL EnableWindow(BOOL bEnable = TRUE)
	{
		ATLASSERT(m_pWndManager != NULL);

		BOOL bTemp = IsWindowEnabled();
		if (bTemp != bEnable)
		{
			if (bEnable)
				m_dwStyle	&= ~SOMWND_DISABLED;
			else
				m_dwStyle	|= SOMWND_DISABLED;

			RedrawWindow();
		}

		return bTemp;
	}
	BOOL ShowWindow(BOOL bShow = TRUE)
	{
		ATLASSERT(m_pWndManager != NULL);

		BOOL bTemp = IsWindowVisible();
		if (bTemp != bShow)
		{
			if (bShow)
				m_dwStyle	|= SOMWND_VISIBLE;
			else
				m_dwStyle	&= ~SOMWND_VISIBLE;

			if (m_pWndManager)
				m_pWndManager->SomRedrawWindow(this, NULL, TRUE, NULL);
		}

		return bTemp;
	}
	BOOL IsWindowVisible()
	{
		ATLASSERT(m_pWndManager != NULL);
		return m_dwStyle & SOMWND_VISIBLE;
	}
	BOOL IsWindowTransparent()
	{
		ATLASSERT(m_pWndManager != NULL);
		return m_dwStyle & SOMWND_TRANSPARENT;
	}
	DWORD GetWndID()
	{
		return m_nID;
	}
	void GetClientRect(RECT& lpRect)
	{
		lpRect.left		= 0;
		lpRect.top		= 0;
		lpRect.right	= m_rcWindow.right - m_rcWindow.left;
		lpRect.bottom	= m_rcWindow.bottom - m_rcWindow.top;
	}
	DWORD GetClientWidth()
	{
		return m_rcWindow.right - m_rcWindow.left;
	}
	DWORD GetClientHeight()
	{
		return m_rcWindow.bottom - m_rcWindow.top;
	}
	void GetWindowRect(RECT& lpRect)
	{
		memcpy(&lpRect, &m_rcWindow, sizeof(m_rcWindow));
	}
	void ClientToManager(LPPOINT pos)
	{
		pos->x	+= m_rcWindow.left;
		pos->y	+= m_rcWindow.top;
	}
	void SetWindowPos(int x, int y, int cx, int cy)
	{
		m_pWndManager->SomSetWindowPos(this, x, y, cx, cy);
	}
	void SizeWindow(int cx, int cy)
	{
		m_pWndManager->SomSizeWindow(this, cx, cy);
	}
	void MoveWindow(int x, int y)
	{
		m_pWndManager->SomMoveWindow(this, x, y);
	}
	void MoveWindowHorizontal(int x)
	{
		m_pWndManager->SomMoveWindow(this, x, m_rcWindow.top);
	}

protected:
	DWORD				m_dwStyle;
	UINT				m_nID;
	RECT				m_rcWindow;

	ISomWndManager*		m_pWndManager;
	void*				m_pWndManagerParam;
	void*				m_pWndManagerParam2;

	void ResetSomWnd()
	{
		m_dwStyle		= 0;
		m_nID			= 0;
		memset(&m_rcWindow, 0, sizeof(m_rcWindow));

		m_pWndManager	= NULL;
		m_pWndManagerParam	= NULL;
		m_pWndManagerParam2	= NULL;
	}

	void DestroyWithoutDetach()
	{
		OnDestroy();

		ResetSomWnd();
	}
	void RedrawWindow(LPCRECT lpRectUpdate = NULL, LPVOID lpRedrawParam = NULL)
	{
		if (m_pWndManager)
			m_pWndManager->SomRedrawWindow(this, lpRectUpdate, FALSE, lpRedrawParam);
	}
	virtual void OnCreate() {}
	virtual void OnDestroy() {}

	virtual void OnTimer() {}

	virtual void OnMove(int cx, int cy) {}
	virtual void OnSize(int cx, int cy) {}

	virtual void OnPaint(HDC hDC, int max_paint_width, LPVOID lpRedrawParam) {}		// max_paint_width 表示能够绘制的最大宽度，-1表示不限定最大宽度，这个参数主要用在 listview 改变列宽的情况

	virtual void OnCaptureChanged() {}
	virtual BOOL OnSetCursor(LPPOINT pos) { return FALSE; }

	virtual BOOL OnLButtonDown(LPPOINT pos) { return FALSE; }
	virtual BOOL OnLButtonUp(LPPOINT pos) { return FALSE; }
	virtual BOOL OnLButtonDblClk(LPPOINT pos) { return FALSE; }
	virtual BOOL OnMouseMove(LPPOINT pos) { return FALSE; }

	virtual void OnMouseEnter(LPPOINT pos) {}
	virtual void OnMouseLeave() {}
	virtual void OnGetInfoTip(UINT_PTR uTipId, LPNMTTDISPINFO lpNMTTDISPINFO) {}
};

class CSomMemoryDC : public CDC
{
public:
	CSomMemoryDC(int cx, int cy)
	{
		HDC hDC = ::GetDC(NULL);

		CreateCompatibleDC(hDC);
		ATLASSERT(m_hDC != NULL);

		m_bmp.CreateCompatibleBitmap(hDC, cx, cy);
		ATLASSERT(m_bmp.m_hBitmap != NULL);

		m_hBmpOld = SelectBitmap(m_bmp);
		ReleaseDC(NULL, hDC);

		m_size.cx	= cx;
		m_size.cy	= cy;
	}
	SIZE& GetSize()
	{
		return m_size;
	}
	~CSomMemoryDC()
	{
		SelectBitmap(m_hBmpOld);
	}
protected:
	SIZE m_size;
	CBitmap m_bmp;
	HBITMAP m_hBmpOld;
};

class CSomTemporaryDC : public CDC
{
public:
	CSomTemporaryDC()
	{
		HDC hDC = ::GetDC(NULL);

		CreateCompatibleDC(hDC);

		::ReleaseDC(NULL, hDC);
	}
};

__declspec(selectany) CSomMemoryDC SomMemoryDC(1024, 128);	// 用来做绘制时的避免闪烁的临时 DC, 因为是全局共享的，所以不要长时间占用
__declspec(selectany) CSomTemporaryDC SomTemporaryDC;		// 全局临时 DC, 用在所有临时需要一个 DC 的地方，节省创建一个临时 DC 的开销

class CSomToolTipAdpater
{
public:
	void SetToolTips(HWND hOwnerWnd, HWND hWndToolTip)
	{
		m_hOwnerWnd.Attach(hOwnerWnd);
		m_wndToolTip.Attach(hWndToolTip);
	}
	UINT_PTR AddToolTipId(CSomWnd* pSomWnd, UINT_PTR uTipId, LPRECT lpRect = NULL, int x = 0, int y = 0)
	{
		ATLASSERT(pSomWnd);

		UINT_PTR uToolTipId	= InnerToOuterTipId(pSomWnd, uTipId);
		ATLASSERT(uToolTipId == NULL);
		if (uToolTipId)
			return NULL;

		for (int i = 1; ; i++)
		{
			uToolTipId = (UINT_PTR)i;
			if (m_OuterTipMap.find(uToolTipId) == m_OuterTipMap.end())
			{
				RECT rect;
				TIPINFO& TipInfo	= m_OuterTipMap[uToolTipId];
				TipInfo.pSomWnd		= pSomWnd;
				TipInfo.uInnerTipId	= uTipId;
				if (lpRect == NULL)
				{
					TipInfo.rect.left	= 0x80000000;
					rect				= pSomWnd->m_rcWindow;
					::OffsetRect(&rect, x, y);
				}
				else
				{
					TipInfo.rect	= *lpRect;
					rect			= *lpRect;
					::OffsetRect(&rect, x + pSomWnd->m_rcWindow.left, y + pSomWnd->m_rcWindow.top);
				}

				map<UINT_PTR, UINT_PTR>& TipMap = m_InnerTipMap[pSomWnd];
				TipMap[uTipId]	= uToolTipId;

				if (m_wndToolTip)
					m_wndToolTip.AddTool(m_hOwnerWnd, LPSTR_TEXTCALLBACK, &rect, uToolTipId);

				return uToolTipId;
			}
		}
		return NULL;
	}
	UINT_PTR DelToolTipId(CSomWnd* pSomWnd, UINT_PTR uTipId)
	{
		ATLASSERT(pSomWnd);

		UINT_PTR uToolTipId	= InnerToOuterTipId(pSomWnd, uTipId);
		ATLASSERT(uToolTipId != NULL);
		if (uToolTipId == NULL)
			return NULL;

		m_OuterTipMap.erase(uToolTipId);
		map<UINT_PTR, UINT_PTR>& TipMap = m_InnerTipMap[pSomWnd];
		TipMap.erase(uTipId);
		if (TipMap.size() == 0)
			m_InnerTipMap.erase(pSomWnd);

		if (m_wndToolTip)
			m_wndToolTip.DelTool(m_hOwnerWnd, uToolTipId);

		return uToolTipId;
	}
	void DelToolTipId(CSomWnd* pSomWnd)
	{
		ATLASSERT(pSomWnd);

		map<CSomWnd*, map<UINT_PTR, UINT_PTR> >::iterator iter = m_InnerTipMap.find(pSomWnd);
		if (iter == m_InnerTipMap.end())
			return;

		map<UINT_PTR, UINT_PTR>& TipMap = iter->second;

		for (map<UINT_PTR, UINT_PTR>::iterator iter2 = TipMap.begin(); iter2 != TipMap.end(); iter2++)
		{
			UINT_PTR uToolTipId = iter2->second;

			m_OuterTipMap.erase(uToolTipId);

			if (m_wndToolTip)
				m_wndToolTip.DelTool(m_hOwnerWnd, uToolTipId);
		}

		m_InnerTipMap.erase(pSomWnd);
	}
	void SomEnableToolTip(CSomWnd* pSomWnd, BOOL bEnable = TRUE, int x = 0, int y = 0)
	{
		ATLASSERT(pSomWnd && ::IsWindow(m_wndToolTip));
		if (m_wndToolTip == NULL)
			return;

		map<CSomWnd*, map<UINT_PTR, UINT_PTR> >::iterator iter = m_InnerTipMap.find(pSomWnd);
		if (iter == m_InnerTipMap.end())
			return;

		map<UINT_PTR, UINT_PTR>& TipMap = iter->second;

		for (map<UINT_PTR, UINT_PTR>::iterator iter2 = TipMap.begin(); iter2 != TipMap.end(); iter2++)
		{
			UINT_PTR uToolTipId = iter2->second;
			if (bEnable)
			{
				RECT rect;
				TIPINFO& TipInfo	= m_OuterTipMap[uToolTipId];
				if (TipInfo.rect.left == 0x80000000)
				{
					rect		= pSomWnd->m_rcWindow;
					::OffsetRect(&rect, x, y);
				}
				else
				{
					rect		= TipInfo.rect;
					::OffsetRect(&rect, x + pSomWnd->m_rcWindow.left, y + pSomWnd->m_rcWindow.top);
				}
				m_wndToolTip.AddTool(m_hOwnerWnd, LPSTR_TEXTCALLBACK, &rect, uToolTipId);
			}
			else
				m_wndToolTip.DelTool(m_hOwnerWnd, uToolTipId);
		}
	}
	UINT_PTR SetToolTipRect(CSomWnd* pSomWnd, UINT_PTR uTipId, LPRECT lpRect = NULL, int x = 0, int y = 0)
	{
		ATLASSERT(pSomWnd);

		UINT_PTR uToolTipId	= InnerToOuterTipId(pSomWnd, uTipId);
		ATLASSERT(uToolTipId != NULL);
		if (uToolTipId == NULL)
			return NULL;

		RECT rect;
		TIPINFO& TipInfo	= m_OuterTipMap[uToolTipId];

		if (lpRect == NULL)
		{
			TipInfo.rect.left	= 0x80000000;
			rect				= pSomWnd->m_rcWindow;
			::OffsetRect(&rect, x, y);
		}
		else
		{
			TipInfo.rect	= *lpRect;
			rect			= *lpRect;
			::OffsetRect(&rect, x + pSomWnd->m_rcWindow.left, y + pSomWnd->m_rcWindow.top);
		}
		if (m_wndToolTip)
			m_wndToolTip.SetToolRect(m_hOwnerWnd, uToolTipId, &rect);

		return uToolTipId;
	}
	void UpdateToolTipRect(CSomWnd* pSomWnd, UINT_PTR uTipId = NULL, int x = 0, int y = 0)
	{
		ATLASSERT(pSomWnd);

		map<CSomWnd*, map<UINT_PTR, UINT_PTR> >::iterator iter = m_InnerTipMap.find(pSomWnd);
		if (iter == m_InnerTipMap.end())
			return;
		
		map<UINT_PTR, UINT_PTR>& TipMap = iter->second;

		if (uTipId)
		{
			map<UINT_PTR, UINT_PTR>::iterator iter2 = TipMap.find(uTipId);
			ATLASSERT(iter2 != TipMap.end());
			if (iter2 != TipMap.end())
			{
				UINT_PTR uToolTipId = iter2->second;
				UpdateToolTipRectImpl(pSomWnd, uToolTipId, x, y);
			}

		}
		else
		{
			for (map<UINT_PTR, UINT_PTR>::iterator iter2 = TipMap.begin(); iter2 != TipMap.end(); iter2++)
			{
				UINT_PTR uToolTipId = iter2->second;
				UpdateToolTipRectImpl(pSomWnd, uToolTipId, x, y);
			}
		}
	}
	UINT_PTR InnerToOuterTipId(CSomWnd* pSomWnd, UINT_PTR uTipId)
	{
		ATLASSERT(pSomWnd);

		map<CSomWnd*, map<UINT_PTR, UINT_PTR> >::iterator iter = m_InnerTipMap.find(pSomWnd);
		if (iter == m_InnerTipMap.end())
			return NULL;

		map<UINT_PTR, UINT_PTR>& TipMap = iter->second;
		map<UINT_PTR, UINT_PTR>::iterator iter2 = TipMap.find(uTipId);
		if (iter2 == TipMap.end())
			return NULL;

		return iter2->second;
	}
	UINT_PTR OuterToInnerTipId(UINT_PTR uTipId, CSomWnd** ppSomWnd, LPRECT lpRect = NULL)
	{
//		ATLASSERT(uTipId);

		map<UINT_PTR, TIPINFO>::iterator iter = m_OuterTipMap.find(uTipId);
//		ATLASSERT(iter != m_OuterTipMap.end());
		if (iter == m_OuterTipMap.end())
			return NULL;

		TIPINFO& TipInfo	= iter->second;
		if (ppSomWnd)
			*ppSomWnd		= TipInfo.pSomWnd;
		if (lpRect)
			*lpRect			= TipInfo.rect;

		return TipInfo.uInnerTipId;
	}
protected:
	void UpdateToolTipRectImpl(CSomWnd* pSomWnd, UINT_PTR uOuterTipId, int x = 0, int y = 0)
	{
		map<UINT_PTR, TIPINFO>::iterator iter3 = m_OuterTipMap.find(uOuterTipId);
		ATLASSERT(iter3 != m_OuterTipMap.end());
		if (iter3 == m_OuterTipMap.end())
			return;

		TIPINFO& TipInfo	= iter3->second;
		RECT rect;
		if (TipInfo.rect.left == 0x80000000)
		{
			rect		= pSomWnd->m_rcWindow;
			::OffsetRect(&rect, x, y);
		}
		else
		{
			rect		= TipInfo.rect;
			::OffsetRect(&rect, x + pSomWnd->m_rcWindow.left, y + pSomWnd->m_rcWindow.top);
		}

		if (m_wndToolTip)
			m_wndToolTip.SetToolRect(m_hOwnerWnd, uOuterTipId, &rect);
	}
	typedef struct _TIPINFO
	{
		CSomWnd* pSomWnd;
		UINT_PTR uInnerTipId;
		RECT rect;

	} TIPINFO;

	map<UINT_PTR, TIPINFO> m_OuterTipMap;
	map<CSomWnd*, map<UINT_PTR, UINT_PTR> > m_InnerTipMap;

	CToolTipCtrl m_wndToolTip;
	CWindow m_hOwnerWnd;
};

class CSomWndManager : public ISomWndManager, public CSomToolTipAdpater
{
public:
	CSomWndManager()
	{
		ResetSomWnd();
	}
	BEGIN_MSG_MAP(CSomWndManager)
		if (m_hWndUI == NULL)
		{
			m_hWndUI	= hWnd;
			SetTimer(hWnd, SOMWNDMANAGER_TIMERID, 100, NULL);
		}
		else
			ATLASSERT(m_hWndUI == hWnd);

		NOTIFY_CODE_HANDLER(TTN_GETDISPINFO, OnGetInfoTip)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDblClk)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_PRINTCLIENT, OnPaint)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_CAPTURECHANGED, OnCaptureChanged)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_SETREDRAW, OnSetRedraw)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)

	END_MSG_MAP()

protected:
	void ResetSomWnd()
	{
		m_bRedrawToggle	= TRUE;
		m_bUpdateToggle	= TRUE;
		m_hWndUI		= NULL;
		m_pCaptureWnd	= NULL;
		m_pHoverWnd		= NULL;
		m_ChildWnds.RemoveAll();
	}
	void SetToolTips(HWND hWndToolTip)
	{
		ATLASSERT(::IsWindow(m_hWndUI) && ::IsWindow(hWndToolTip));
		CSomToolTipAdpater::SetToolTips(m_hWndUI, hWndToolTip);
	}
	LRESULT OnGetInfoTip(int idCtrl, LPNMHDR lpNMHDR, BOOL& bHandled)
	{
		LPNMTTDISPINFO lpNMTTDISPINFO	= (LPNMTTDISPINFO)lpNMHDR;
		ATLASSERT((lpNMTTDISPINFO->uFlags & TTF_IDISHWND) == 0);

		if (lpNMTTDISPINFO->uFlags & TTF_IDISHWND)
		{
			bHandled	= FALSE;
			return 0;
		}

		UINT_PTR uToolTipId = lpNMHDR->idFrom;
		CSomWnd* pSomWnd = NULL;
		UINT_PTR uInnerId = OuterToInnerTipId(uToolTipId, &pSomWnd);
		if (pSomWnd)
			pSomWnd->OnGetInfoTip(uInnerId, lpNMTTDISPINFO);

		return 0;
	}
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 1;
	}
	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		return TRUE;
	}
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if(wParam != NULL)
		{
			RECT rc;
			GetClipBox((HDC)wParam, &rc);
			DoPaint((HDC)wParam, &rc);
			OnDrawBackground((HDC)wParam, &rc);
		}
		else
		{
			CPaintDC dc(m_hWndUI);
			DoPaint(dc.m_hDC, &dc.m_ps.rcPaint);
			OnDrawBackground(dc.m_hDC, &dc.m_ps.rcPaint);
		}
		return 0;
	}
	LRESULT OnSetCursor(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		POINT pos = {0};
		GetCursorPos(&pos);
		ScreenToClient(m_hWndUI, &pos);

		if (m_pCaptureWnd)
		{
			ManagerToClient(m_pCaptureWnd, &pos);
			bHandled = m_pCaptureWnd->OnSetCursor(&pos);
			return bHandled;
		}

		for (int i = 0; i < m_ChildWnds.GetSize(); i++)
		{
			CSomWnd* pSomWnd = m_ChildWnds[i];
			if (pSomWnd->IsWindowVisible() && pSomWnd->IsWindowEnabled() && PtInRect(&pSomWnd->m_rcWindow, pos))
			{
				ManagerToClient(pSomWnd, &pos);
				bHandled = pSomWnd->OnSetCursor(&pos);
				return bHandled;
			}
		}

		bHandled = FALSE;
		return bHandled;
	}
	virtual void OnDrawBackground(HDC hDC, LPCRECT lpRectDraw = NULL)
	{
		 HBRUSH hBackBrush = (HBRUSH)GetClassLong(m_hWndUI, GCLP_HBRBACKGROUND);
		 if (hBackBrush)
		 {
			 RECT rc;
			 if (lpRectDraw == NULL)
			 {
				 GetClipBox(hDC, &rc);
				 lpRectDraw	= &rc;
			 }
			 FillRect(hDC, lpRectDraw, hBackBrush);
		 }
	}
	virtual void DoPaint(HDC hDC, LPCRECT lpRectUpdate)
	{
		for (int i = 0; i < m_ChildWnds.GetSize(); i++)
		{
			CSomWnd* pSomWnd = m_ChildWnds[i];

			RECT rcTemp;
			if (pSomWnd->IsWindowVisible() && IntersectRect(&rcTemp, &pSomWnd->m_rcWindow, lpRectUpdate))
				SomDrawWnd(hDC, pSomWnd, TRUE, pSomWnd->IsWindowTransparent(), TRUE, NULL);
		}
	}
	void SomDrawWnd(HDC hDC, CSomWnd* pSomWnd, BOOL bPaint, BOOL bEraseBackground, BOOL bExcludeClipRect, LPVOID lpRedrawParam)
	{
		RECT rcWindow = pSomWnd->m_rcWindow;

		HDC hTargetDC = hDC;
		if (bEraseBackground)
		{
			if (bPaint && SomMemoryDC && SomMemoryDC.GetSize().cx >= (rcWindow.right - rcWindow.left) && SomMemoryDC.GetSize().cy >= (rcWindow.bottom - rcWindow.top))
			{
				hTargetDC	= SomMemoryDC;
				SetViewportOrgEx(hTargetDC, -rcWindow.left, -rcWindow.top, NULL);
			}
			OnDrawBackground(hTargetDC, &rcWindow);
		}
		if (bPaint)
		{
			if (hTargetDC == hDC)
			{
				POINT pt;
				OffsetViewportOrgEx(hTargetDC, rcWindow.left, rcWindow.top, &pt);
				pSomWnd->OnPaint(hTargetDC, -1, lpRedrawParam);
				SetViewportOrgEx(hTargetDC, pt.x, pt.y, NULL);
			}
			else
			{
				SetViewportOrgEx(hTargetDC, 0, 0, NULL);
				pSomWnd->OnPaint(hTargetDC, -1, lpRedrawParam);
				BitBlt(hDC, rcWindow.left, rcWindow.top, rcWindow.right - rcWindow.left, rcWindow.bottom - rcWindow.top, hTargetDC, 0, 0, SRCCOPY);
			}
			if (bExcludeClipRect)
				ExcludeClipRect(hDC, rcWindow.left, rcWindow.top, rcWindow.right, rcWindow.bottom);
		}
	}
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if (wParam == SOMWNDMANAGER_TIMERID)
		{
			if (m_pHoverWnd != NULL && m_pCaptureWnd == NULL)
			{
				POINT pt;
				RECT rc;
				::GetCursorPos(&pt);
				::GetClientRect(m_hWndUI, &rc);
				::MapWindowPoints(m_hWndUI, NULL, (LPPOINT)&rc, 2);
				if (!::PtInRect(&rc, pt) ||(WindowFromPoint(pt) != m_hWndUI))
				{
					m_pHoverWnd->OnMouseLeave();
					m_pHoverWnd	= NULL;
				}
			}

			for (int i = 0; i < m_ChildWnds.GetSize(); i++)
			{
				CSomWnd* pSomWnd = m_ChildWnds[i];
				pSomWnd->OnTimer();
			}

			return TRUE;
		}
		else
			bHandled = FALSE;

		return 0;
	}
	LRESULT OnCaptureChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		if (m_pCaptureWnd)
		{
			m_pCaptureWnd->OnCaptureChanged();
			m_pCaptureWnd	= NULL;
		}
		else
			bHandled = FALSE;

		return 0;
	}
	LRESULT OnLButtonDblClk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		POINT pos = {LOWORD(lParam), HIWORD(lParam)};

		if (m_pCaptureWnd)
		{
			ManagerToClient(m_pCaptureWnd, &pos);
			return m_pCaptureWnd->OnLButtonDblClk(&pos);
		}

		for (int i = 0; i < m_ChildWnds.GetSize(); i++)
		{
			CSomWnd* pSomWnd = m_ChildWnds[i];
			if (pSomWnd->IsWindowVisible() && pSomWnd->IsWindowEnabled() && PtInRect(&pSomWnd->m_rcWindow, pos))
			{
				ManagerToClient(pSomWnd, &pos);
				bHandled = pSomWnd->OnLButtonDblClk(&pos);
				return 0;
			}
		}

		bHandled = FALSE;

		return 0;
	}
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		POINT pos = {LOWORD(lParam), HIWORD(lParam)};

		if (m_pCaptureWnd)
		{
			ManagerToClient(m_pCaptureWnd, &pos);
			return m_pCaptureWnd->OnLButtonDown(&pos);
		}

		for (int i = 0; i < m_ChildWnds.GetSize(); i++)
		{
			CSomWnd* pSomWnd = m_ChildWnds[i];

			if (pSomWnd->IsWindowVisible() && pSomWnd->IsWindowEnabled() && PtInRect(&pSomWnd->m_rcWindow, pos))
			{
				ManagerToClient(pSomWnd, &pos);
				bHandled = pSomWnd->OnLButtonDown(&pos);
				return 0;
			}
		}

		bHandled = FALSE;

		return 0;
	}
	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		POINT pos = {LOWORD(lParam), HIWORD(lParam)};

		if (m_pCaptureWnd)
		{
			ManagerToClient(m_pCaptureWnd, &pos);
			return m_pCaptureWnd->OnLButtonUp(&pos);
		}

		for (int i = 0; i < m_ChildWnds.GetSize(); i++)
		{
			CSomWnd* pSomWnd = m_ChildWnds[i];

			if (pSomWnd->IsWindowVisible() && pSomWnd->IsWindowEnabled() && PtInRect(&pSomWnd->m_rcWindow, pos))
			{
				ManagerToClient(pSomWnd, &pos);
				bHandled = pSomWnd->OnLButtonUp(&pos);
				return 0;
			}
		}

		bHandled = FALSE;

		return 0;
	}
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		POINT pos = {LOWORD(lParam), HIWORD(lParam)};

		if (m_pCaptureWnd)
		{
			ATLASSERT(m_pHoverWnd == NULL || m_pHoverWnd == m_pCaptureWnd);

			POINT pt = pos;
			ManagerToClient(m_pCaptureWnd, &pt);

			if (PtInRect(&m_pCaptureWnd->m_rcWindow, pos))
			{
				if (m_pHoverWnd == NULL)
				{
					m_pHoverWnd	= m_pCaptureWnd;
					m_pCaptureWnd->OnMouseEnter(&pt);
				}
			}
			else
			{
				if (m_pHoverWnd == m_pCaptureWnd)
				{
					m_pHoverWnd	= NULL;
					m_pCaptureWnd->OnMouseLeave();
				}
			}
			return m_pCaptureWnd->OnMouseMove(&pt);
		}

		CSomWnd* pSomWnd	= NULL;

		for (int i = 0; i < m_ChildWnds.GetSize(); i++)
		{
			CSomWnd* pSomWnd2 = m_ChildWnds[i];

			if (pSomWnd2->IsWindowVisible() && pSomWnd2->IsWindowEnabled() && PtInRect(&pSomWnd2->m_rcWindow, pos))
			{
				pSomWnd	= pSomWnd2;
				ManagerToClient(pSomWnd, &pos);
				break;
			}
		}

		if (pSomWnd != m_pHoverWnd)
		{
			if (m_pHoverWnd != NULL)
			{
				m_pHoverWnd->OnMouseLeave();
			}

			m_pHoverWnd	= pSomWnd;

			if (pSomWnd != NULL)
			{
				pSomWnd->OnMouseEnter(&pos);
			}
		}
		if (pSomWnd)
		{
			pSomWnd->OnMouseMove(&pos);
			bHandled = FALSE;
		}
		return 0;
	}
	LRESULT OnSetRedraw(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;

		m_bRedrawToggle	= (BOOL)wParam;

		return 0;
	}
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;

		KillTimer(m_hWndUI, SOMWNDMANAGER_TIMERID);

		if (m_pHoverWnd)
		{
			m_pHoverWnd->OnMouseLeave();
			m_pHoverWnd	= NULL;
		}
		if (m_pCaptureWnd)
		{
			m_pCaptureWnd->OnCaptureChanged();
			m_pCaptureWnd	= NULL;
		}

		for (int i = 0; i < m_ChildWnds.GetSize(); i++)
		{
			m_ChildWnds[i]->DestroyWithoutDetach();
		}
		ResetSomWnd();

		return 0;
	}
	virtual void AttachWindow(CSomWnd* pSomWnd)
	{
		m_ChildWnds.Add(pSomWnd);
	}
	virtual void DetachWindow(CSomWnd* pSomWnd)
	{
		if (m_pCaptureWnd == pSomWnd)
		{
			SomReleaseCapture(m_pCaptureWnd);
			if (m_pCaptureWnd)
			{
				m_pCaptureWnd->OnCaptureChanged();
				m_pCaptureWnd	= NULL;
			}
		}
		if (m_pHoverWnd == pSomWnd)
		{
			m_pHoverWnd->OnMouseLeave();
			m_pHoverWnd		= NULL;
		}

		m_ChildWnds.Remove(pSomWnd);
	}
	virtual void SomSetCapture(CSomWnd* pSomWnd)
	{
		if (m_pCaptureWnd)
			m_pCaptureWnd->OnCaptureChanged();

		m_pCaptureWnd	= pSomWnd;

		if (GetCapture() != m_hWndUI)
		{
			::SetCapture(m_hWndUI);
		}
	}
	virtual void SomReleaseCapture(CSomWnd* pSomWnd) 
	{
		if (pSomWnd == m_pCaptureWnd)
			::ReleaseCapture();
	}
	virtual LRESULT SomSendMessage(UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		return ::SendMessage(m_hWndUI, Msg, wParam, lParam);
	}
	virtual LRESULT SomSendCommandMessage(CSomWnd* pSomWnd, WORD wNotifyCode, WORD wID)
	{
		return ::SendMessage(m_hWndUI, WM_COMMAND, MAKEWPARAM(wID, wNotifyCode), (LPARAM)pSomWnd);
	}
	virtual BOOL SomPostMessage(UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		return ::PostMessage(m_hWndUI, Msg, wParam, lParam);
	}
	virtual void SomSetWindowPos(CSomWnd* pSomWnd, int x, int y, int cx, int cy)
	{
		BOOL bRepaint	= pSomWnd->IsWindowVisible();

		RECT& rcWindow = pSomWnd->m_rcWindow;

		int x2	= rcWindow.left;
		int y2	= rcWindow.top;
		int cx2 = rcWindow.right - rcWindow.left;
		int cy2 = rcWindow.bottom - rcWindow.top;

		if (x != x2 || y != y2 || cx != cx2 || cy != cy2)
		{
			if (bRepaint)
				InvalidateRect(m_hWndUI, &rcWindow, FALSE);

			rcWindow.left		= x;
			rcWindow.top		= y;
			rcWindow.right		= x + cx;
			rcWindow.bottom		= y + cy;

			if (x != x2 || y != y2)
			{
				pSomWnd->OnMove(x, y);
			}
			if (cx != cx2 || cy != cy2)
			{
				pSomWnd->OnSize(cx, cy);
			}

			if (bRepaint)
				InvalidateRect(m_hWndUI, &rcWindow, FALSE);

			if (bRepaint && m_bUpdateToggle)
				UpdateWindow(m_hWndUI);
		}
	}
	virtual void SomSizeWindow(CSomWnd* pSomWnd, int cx, int cy)
	{
		BOOL bRepaint	= pSomWnd->IsWindowVisible();

		RECT& rcWindow = pSomWnd->m_rcWindow;

		int cx2 = rcWindow.right - rcWindow.left;
		int cy2 = rcWindow.bottom - rcWindow.top;

		if (cx != cx2 || cy != cy2)
		{
			if (bRepaint)
				InvalidateRect(m_hWndUI, &rcWindow, FALSE);

			rcWindow.right	= rcWindow.left + cx;
			rcWindow.bottom	= rcWindow.top + cy;

			pSomWnd->OnSize(cx, cy);

			if (bRepaint)
				InvalidateRect(m_hWndUI, &rcWindow, FALSE);

			if (bRepaint && m_bUpdateToggle)
				UpdateWindow(m_hWndUI);
		}
	}
	virtual void SomMoveWindow(CSomWnd* pSomWnd, int x, int y)
	{
		BOOL bRepaint	= pSomWnd->IsWindowVisible();

		RECT& rcWindow = pSomWnd->m_rcWindow;
		if (x != rcWindow.left || y != rcWindow.top)
		{
			if (bRepaint)
				InvalidateRect(m_hWndUI, &rcWindow, FALSE);

			OffsetRect(&rcWindow, x - rcWindow.left, y - rcWindow.top);

			pSomWnd->OnMove(x, y);

			if (bRepaint)
				InvalidateRect(m_hWndUI, &rcWindow, FALSE);

			if (bRepaint && m_bUpdateToggle)
				UpdateWindow(m_hWndUI);
		}
	}
	virtual void SomRedrawWindow(CSomWnd* pSomWnd, LPCRECT lpRectUpdate, BOOL bRedrawInvisible, LPVOID lpRedrawParam)
	{
		if (!m_bRedrawToggle)
			return;

		BOOL bPaint				= pSomWnd->IsWindowVisible();
		BOOL bEraseBackground	= bPaint ? pSomWnd->IsWindowTransparent() : bRedrawInvisible;

		if (!(bPaint || bEraseBackground))
			return;

		RECT rcPaint;
		RECT& rcWindow	= pSomWnd->m_rcWindow;
		if (lpRectUpdate)
		{
			RECT rcTemp = *lpRectUpdate;
			OffsetRect(&rcTemp, rcWindow.left, rcWindow.top);
			if (!IntersectRect(&rcPaint, &rcWindow, &rcTemp))
				return;
		}
		else
			rcPaint	= rcWindow;

		if (m_bUpdateToggle)
		{
			HDC hDC = GetDC(m_hWndUI);
			ATLASSERT(hDC);

			IntersectClipRect(hDC, rcPaint.left, rcPaint.top, rcPaint.right, rcPaint.bottom);

			SomDrawWnd(hDC, pSomWnd, bPaint, bEraseBackground, FALSE, lpRedrawParam);

			ReleaseDC(m_hWndUI, hDC);
		}
		else
			InvalidateRect(m_hWndUI, &rcPaint, FALSE);
	}
	virtual void SomAddTooltip(CSomWnd* pSomWnd, UINT_PTR uInnerTipId, LPRECT lpRect)
	{
		UINT_PTR uToolTipId = AddToolTipId(pSomWnd, uInnerTipId, lpRect);
		ATLASSERT(uToolTipId != NULL);
	}
	virtual void SomDelTooltip(CSomWnd* pSomWnd)
	{
		DelToolTipId(pSomWnd);
	}
	virtual void SomDelayUpdateWindow()
	{
		m_bUpdateToggle	= FALSE;
	}
	virtual void SomCommitUpdateWindow()
	{
		m_bUpdateToggle	= TRUE;

		if (m_bRedrawToggle)
			UpdateWindow(m_hWndUI);
	}
	void ManagerToClient(CSomWnd* pSomWnd, LPPOINT lpPoint)
	{
		lpPoint->x	-= pSomWnd->m_rcWindow.left;
		lpPoint->y	-= pSomWnd->m_rcWindow.top;
	}
protected:
	BOOL	m_bRedrawToggle;
	BOOL	m_bUpdateToggle;
	HWND	m_hWndUI;
	CSimpleArray<CSomWnd*>	m_ChildWnds;
	CSomWnd* m_pCaptureWnd;
	CSomWnd* m_pHoverWnd;
};
