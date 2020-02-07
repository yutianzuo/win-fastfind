/********************************************************************
	created:	2009/10/12
	created:	12:10:2009   10:58
	file base:	somlist
	file ext:	h
	author:		wanlixin
	
	purpose:	SOM 界面库实现自绘的 ListBox, ListView 控件
*********************************************************************/
#pragma once

#define SOM_LBN_SCROLL		0x1000
#define SOM_BN_CLICKED		0x1001

class CSomListBox 
	: public CWindowImpl<CSomListBox, CListBox>
	, public COwnerDraw<CSomListBox>
	, public ISomWndManager
	, public CSomToolTipAdpater
{
public:
	static DWORD GetWndStyle(DWORD dwStyle)
	{
		return dwStyle == 0 ? WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | LBS_OWNERDRAWVARIABLE | LBS_NOTIFY | WS_VSCROLL : dwStyle;
	}

	typedef struct _LBCONTROL
	{
		int column;
		CSomWnd* pSomWnd;	
		int x;
		int y;
		int cx;
		int cy;
		UINT nID;
		DWORD dwStyle;

	} LBCONTROL, *PLBCONTROL;

	BEGIN_MSG_MAP(CSomListBox)
		NOTIFY_CODE_HANDLER(TTN_GETDISPINFO, OnGetInfoTip)
		MESSAGE_RANGE_HANDLER(WM_MOUSEFIRST, WM_MBUTTONDBLCLK, OnMouseMessage)
		MESSAGE_HANDLER(WM_SETCURSOR, OnMouseMessage)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		CHAIN_MSG_MAP_ALT(COwnerDraw<CSomListBox>, 1)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_MOUSEWHEEL, OnMouseWheel)
		MESSAGE_HANDLER(WM_VSCROLL, OnVScroll)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_CAPTURECHANGED, OnCaptureChanged)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SETREDRAW, OnSetRedraw)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()

	CSomListBox(void)
	{
		ResetSomWnd();

#ifdef _DEBUG
		m_iItemDataCount	= 0;
#endif
	}
	~CSomListBox(void)
	{
#ifdef _DEBUG
		ATLASSERT(m_iItemDataCount == 0);
#endif
		ATLASSERT(m_ItemHandleMap.size() == 0);
	}
	void SetToolTips(HWND hWndToolTip)
	{
		ATLASSERT(::IsWindow(m_hWnd) && ::IsWindow(hWndToolTip));
		CSomToolTipAdpater::SetToolTips(m_hWnd, hWndToolTip);
	}
	void ResetContent()
	{
		SomDelayResetContent();
		CListBox::ResetContent();
	}
	void SomDelayResetContent(DWORD dwDelayTick = 0)
	{
		if (dwDelayTick == 0)
			m_dwDelayResetContentTick	= 0;
		else
			m_dwDelayResetContentTick	= GetTickCount() + dwDelayTick;
	}
	virtual void SomDelayUpdateWindow()
	{
		m_bUpdateToggle	= FALSE;
	}
	virtual void SomCommitUpdateWindow()
	{
		m_bUpdateToggle	= TRUE;

		if (m_bRedrawToggle)
			UpdateWindow();
	}
	void SetHoverItem(int nItem)
	{
		ATLASSERT(nItem == -1 || (nItem >= 0 && nItem < GetCount()));

		if (nItem == m_iHoverItem)
			return;

		if (nItem != -1 && (nItem < 0 || nItem >= GetCount()))
			return;

		int iOld		= m_iHoverItem;  
		m_iHoverItem	= nItem;

		if (iOld != -1)
		{
			RECT rect;
			GetItemRect(iOld, &rect);
			RedrawWindow(&rect, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
		}
		if (m_iHoverItem != -1)
		{
			RECT rect;
			GetItemRect(m_iHoverItem, &rect);
			RedrawWindow(&rect, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
		}
	}
	void SetHoverItem(DWORD_PTR pItemKey)
	{
		map<DWORD_PTR, PLBITEMDATA>::iterator iter = m_ItemHandleMap.find(pItemKey);
		if (iter == m_ItemHandleMap.end())
			return;

		PLBITEMDATA pdata	= iter->second;
		ATLASSERT(pdata != NULL);

		SetHoverItem(GetLbItemIndex(pdata));
	}
	void EnableItemHover(BOOL bHover = TRUE)
	{
		m_bItemHover	= bHover;
	}
	BOOL SubclassWindow(HWND hWnd)
	{
		ResetSomWnd();

		BOOL bRet = CWindowImpl<CSomListBox, CListBox>::SubclassWindow(hWnd);
		SetTimer(SOMWNDMANAGER_TIMERID, 100, NULL);
		return bRet;
	}
	BOOL SetItemData(int nIndex, DWORD_PTR dwItemData)
	{
		PLBITEMDATA pdata = GetLbItemData(nIndex);
		if (pdata)
		{
			pdata->pdwItemData	= dwItemData;
			return TRUE;
		}
		else
			return FALSE;
	}
	DWORD_PTR GetItemData(int nIndex) const
	{
		PLBITEMDATA pdata = GetLbItemData(nIndex);
		if (pdata)
			return pdata->pdwItemData;
		else
			return NULL;
	}
	int SetItemDataPtr(int nIndex, void* pData)
	{
		return SetItemData(nIndex, (DWORD_PTR)pData);
	}
	void* GetItemDataPtr(int nIndex) const
	{
		return (void*)GetItemData(nIndex);
	}
	void SetColumnInfoTypeCount(int count = 1)
	{
		m_ColumnInfo.resize(count);
	}
	void AddColumn(int min_width, float max_width_percent = 0, int columninfo_type = 0)
	{
		if (columninfo_type < 0 || columninfo_type >= (int)m_ColumnInfo.size())
		{
			ATLASSERT(FALSE);
			return;
		}

		vector<LBCOLUMNINFO>& column_info = m_ColumnInfo[columninfo_type];
		column_info.push_back(LBCOLUMNINFO(min_width, max_width_percent));
	}
	void SetColumnWidth(int nCol, int min_width, float max_width_percent = 0, int columninfo_type = 0)
	{
		if (columninfo_type < 0 || columninfo_type >= (int)m_ColumnInfo.size())
		{
			ATLASSERT(FALSE);
			return;
		}

		vector<LBCOLUMNINFO>& column_info = m_ColumnInfo[columninfo_type];

		if (nCol < 0 || nCol >= (int)column_info.size())
		{
			ATLASSERT(FALSE);
			return;
		}
		LBCOLUMNINFO& LbColumnInfo		= column_info[nCol];
		LbColumnInfo.min_width			= min_width;
		LbColumnInfo.max_width_percent	= max_width_percent;
	}
	// pItemKey 用来作为使用者对于这个 Item 的标识，之后使用者对这个 Item 的操作以及 ListBox 对使用者的通知都用这个 pItemKey 来定位这个 Item
	// 所以 pItemKey 不能重复，这样使用者可以选择用从 0 开始的下标做key，或者是 this 指针做key，为了兼容那些没有 key 的 Item，目前保留高字为 0xFFFF 的
	// key，所以这些 key 可以重复，缺点是使用者不能用这些 key 来定位 Item 了
	int AddString(PLBCONTROL pLbControl, int LbControlCount, DWORD_PTR pItemKey = (DWORD_PTR)-1, int columninfo_type = 0)
	{
		return InsertString(-1, pLbControl, LbControlCount, pItemKey, columninfo_type);
	}
	int InsertString(int nIndex, PLBCONTROL pLbControl, int LbControlCount, DWORD_PTR pItemKey = (DWORD_PTR)-1, int columninfo_type = 0)
	{
		int iRet;
		if (HIWORD(pItemKey) == 0xFFFF)
		{
			iRet	= CListBox::InsertString(nIndex, L"");
			if (iRet < 0)
				return iRet;

			PLBITEMDATA h = new LBITEMDATA(pItemKey, this, pLbControl, LbControlCount, columninfo_type);
			CListBox::SetItemData(iRet, (DWORD_PTR)h);
			for (int i = 0; i < h->ChildWnds.GetSize(); i++)
				h->ChildWnds[i]->CreateNotifyEvent();
#ifdef _DEBUG
			m_iItemDataCount++;
#endif
			return iRet;
		}

		if (m_ItemHandleMap.find(pItemKey) != m_ItemHandleMap.end())
		{
			ATLASSERT(FALSE);
			return LB_ERR;
		}
		else
		{
			iRet	= CListBox::InsertString(nIndex, L"");
			if (iRet < 0)
				return iRet;

			PLBITEMDATA h = new LBITEMDATA(pItemKey, this, pLbControl, LbControlCount,columninfo_type);
			m_ItemHandleMap[pItemKey]	= h;

			CListBox::SetItemData(iRet, (DWORD_PTR)h);
			for (int i = 0; i < h->ChildWnds.GetSize(); i++)
				h->ChildWnds[i]->CreateNotifyEvent();
#ifdef _DEBUG
			m_iItemDataCount++;
#endif
			return iRet;
		}
	}
	void DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct)
	{
		if (m_iHoverItem == (int)lpDeleteItemStruct->itemID)
			m_iHoverItem	= -1;

		DeleteItemAllSomControls((int)lpDeleteItemStruct->itemID);
	}
	void DeleteItemAllSomControls(DWORD_PTR pItemKey)
	{
		map<DWORD_PTR, PLBITEMDATA>::iterator iter = m_ItemHandleMap.find(pItemKey);
		if (iter == m_ItemHandleMap.end())
			return;

		PLBITEMDATA pdata	= iter->second;
		ATLASSERT(pdata != NULL);

		int nIndex = GetLbItemIndex(pdata);
		if (nIndex != -1)
			DeleteItemAllSomControls(nIndex);
	}
	void DeleteItemAllSomControls(int nIndex)
	{
		PLBITEMDATA pdata = GetLbItemData(nIndex);
		CListBox::SetItemData(nIndex, NULL);
		if (pdata == NULL)
			return;

		if (m_pCaptureWnd && (PLBITEMDATA)m_pCaptureWnd->m_pWndManagerParam == pdata)
		{
			SomReleaseCapture(m_pCaptureWnd);
		}
		if (m_pHoverWnd && (PLBITEMDATA)m_pHoverWnd->m_pWndManagerParam == pdata)
		{
			m_pHoverWnd->OnMouseLeave();

			m_pHoverWnd		= NULL;
		}

		if (HIWORD(pdata->pdwItemKey) != 0xFFFF)
		{
			map<DWORD_PTR, PLBITEMDATA>::iterator iter = m_ItemHandleMap.find(pdata->pdwItemKey);
// 2010-03-26 出现了m_ItemHandleMap.erase(iter); 崩溃 由于iter == m_ItemHandleMap.end()
// 具体原因不清楚 先做下处理 
// 管炜：逻辑导致iter==m_ItemHandleMap.end()，是否有潜在的逻辑错误隐患？这样的判断是否还是不保险
			/*ATLASSERT(iter->second == pdata);
			ATLASSERT(iter != m_ItemHandleMap.end());*/
			ATLASSERT(iter->second == pdata);
		// 2010-03-26 出现了m_ItemHandleMap.erase(iter); 崩溃 由于iter == m_ItemHandleMap.end()
		// 具体原因不清楚 先做下处理 
		// 管炜：逻辑导致iter==m_ItemHandleMap.end()，是否有潜在的逻辑错误隐患？这样的判断是否还是不保险
			if (iter != m_ItemHandleMap.end())
				m_ItemHandleMap.erase(iter);
			else
				ATLASSERT(0);
		}

		delete pdata;

#ifdef _DEBUG
		m_iItemDataCount--;
#endif
	}
	DWORD_PTR GetLbItemKey(int nIndex)
	{
		PLBITEMDATA pdata = GetLbItemData(nIndex);

		if (pdata != NULL)
			return pdata->pdwItemKey;

		return NULL;
	}
	CSomWnd* GetSomControl(int nIndex, int nID)
	{
		PLBITEMDATA pdata = GetLbItemData(nIndex);

		if (pdata != NULL)
		{
			CSimpleArray<CSomWnd*>& pChildWnds = pdata->ChildWnds;

			for (int i = 0; i < pChildWnds.GetSize(); i++)
			{
				CSomWnd* pSomWnd = pChildWnds[i];
				if (pSomWnd->GetWndID() == nID)
				{
					return pSomWnd;
				}
			}
		}

		return NULL;
	}
	CSomWnd* GetSomControlByIndex(int nIndex, int nCtrlIndex)
	{
		PLBITEMDATA pdata = GetLbItemData(nIndex);

		if (pdata != NULL)
		{
			CSimpleArray<CSomWnd*>& pChildWnds = pdata->ChildWnds;

			if (nCtrlIndex < 0 || nCtrlIndex >= pChildWnds.GetSize())
				return NULL;
			else
				return pChildWnds[nCtrlIndex];
		}
		else
			return NULL;
	}
	CSomWnd* GetSomControl(DWORD_PTR pItemKey, int nID)
	{
		map<DWORD_PTR, PLBITEMDATA>::iterator iter = m_ItemHandleMap.find(pItemKey);

		if (iter == m_ItemHandleMap.end())
			return NULL;

		PLBITEMDATA pdata	= iter->second;
		ATLASSERT(pdata != NULL);

		CSimpleArray<CSomWnd*>& pChildWnds = pdata->ChildWnds;

		for (int i = 0; i < pChildWnds.GetSize(); i++)
		{
			CSomWnd* pSomWnd = pChildWnds[i];
			if (pSomWnd->GetWndID() == nID)
			{
				return pSomWnd;
			}
		}
		return NULL;
	}
	CSomWnd* GetSomControlByIndex(DWORD_PTR pItemKey, int nCtrlIndex)
	{
		map<DWORD_PTR, PLBITEMDATA>::iterator iter = m_ItemHandleMap.find(pItemKey);

		if (iter == m_ItemHandleMap.end())
			return NULL;

		PLBITEMDATA pdata	= iter->second;
		ATLASSERT(pdata != NULL);

		CSimpleArray<CSomWnd*>& pChildWnds = pdata->ChildWnds;

		if (nCtrlIndex < 0 || nCtrlIndex >= pChildWnds.GetSize())
			return NULL;
		else
			return pChildWnds[nCtrlIndex];
	}
	BOOL IsItemExists(DWORD_PTR pItemKey)
	{
		return m_ItemHandleMap.find(pItemKey) != m_ItemHandleMap.end();
	}
	int GetItemIndex(DWORD_PTR pItemKey)
	{
		map<DWORD_PTR, PLBITEMDATA>::iterator iter = m_ItemHandleMap.find(pItemKey);
		if (iter == m_ItemHandleMap.end())
			return -1;

		return GetLbItemIndex(iter->second);
	}
	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		int count = GetCount();
		if (count == 0)
			return TRUE;

		RECT rcClient;
		GetClientRect(&rcClient);

		RECT rcTail;
		if (count && GetItemRect(count - 1, &rcTail) != LB_ERR)
		{
			if (rcTail.bottom >= rcClient.bottom)
				return TRUE;
			else
				rcClient.top	= rcTail.bottom;
		}

		OnDrawItemBackgound((HDC)wParam, FALSE, FALSE, &rcClient, -1, -1, NULL);

		return TRUE;
	}
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		if (GetCount() == 0)
		{
			CPaintDC dc(*this);

			RECT rcClient;
			GetClientRect(&rcClient);

			OnDrawItemBackgound(dc, FALSE, FALSE, &rcClient, -1, -1, NULL);

			OnDrawWhenNoItem(dc);

			return 0;
		}
		else
			return DefWindowProc(uMsg, wParam, lParam);
	}
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
	{
		int nItem = (int)lpDrawItemStruct->itemID;
		RECT& rect	= lpDrawItemStruct->rcItem;
		PLBITEMDATA pdata = (PLBITEMDATA)lpDrawItemStruct->itemData;

		if (pdata == NULL)
			return;

		HDC hDC = lpDrawItemStruct->hDC;

		CSimpleArray<CSomWnd*>& pChildWnds = pdata->ChildWnds;
		RECT rcUpdate;
		GetClipBox(hDC, &rcUpdate);

		POINT pt;
		OffsetViewportOrgEx(hDC, rect.left, rect.top, &pt);
		for (int i = 0; i < pChildWnds.GetSize(); i++)
		{
			CSomWnd* pSomWnd = pChildWnds[i];

			if (pSomWnd && pSomWnd->IsWindowVisible())
				SomDrawWnd(hDC, pSomWnd, TRUE, pSomWnd->IsWindowTransparent(), m_iHoverItem == nItem, lpDrawItemStruct->itemState & ODS_SELECTED, nItem, rect.right - rect.left, rect.bottom - rect.top, pdata->pdwItemKey, TRUE, NULL);
		}
		SetViewportOrgEx(hDC, pt.x, pt.y, NULL);

		OnDrawItemBackgound(hDC, m_iHoverItem == nItem, lpDrawItemStruct->itemState & ODS_SELECTED, &rect, nItem, pdata->pdwItemKey);
	}
	void SomDrawWnd(HDC hDC, CSomWnd* pSomWnd, BOOL bPaint, BOOL bEraseBackground, BOOL bHover, BOOL bCheck, int nItem, int nItemWidth, int nItemHeight, DWORD_PTR pItemKey, BOOL bExcludeClipRect, LPVOID lpRedrawParam)
	{
		RECT& rcWindow = pSomWnd->m_rcWindow;

		if (rcWindow.left > nItemWidth)
			return;

		if (m_ColumnInfo.size() == 0)
			return;

		PLBITEMDATA pLbData	= (PLBITEMDATA)pSomWnd->m_pWndManagerParam;
		int columninfo_type = pLbData->columninfo_type;
		if (columninfo_type < 0 || columninfo_type >= (int)m_ColumnInfo.size())
		{
			ATLASSERT(FALSE);
			return;
		}
		vector<LBCOLUMNINFO>& column_info = m_ColumnInfo[columninfo_type];

		int column = (int)pSomWnd->m_pWndManagerParam2;
		ATLASSERT(column >= 0 && column < (int)column_info.size());
		if (column < 0 || column >= (int)column_info.size())
		{
			ATLASSERT(FALSE);
			return;
		}

		LBCOLUMNINFO& LbColumnInfo = column_info[column];

		// 2010-03-18 修改界面显示
		// 2010.03.18 万立新评审通过
		if ((LbColumnInfo.x + LbColumnInfo.width) < nItemWidth)
			nItemWidth	= LbColumnInfo.width;
		else
			nItemWidth	-= LbColumnInfo.x;

		int paint_width		= rcWindow.right - rcWindow.left;
		int max_paint_width	= -1;
		if (nItemWidth < rcWindow.right)
		{
			paint_width		= nItemWidth - rcWindow.left;
			max_paint_width	= paint_width;
		}

		HDC hTargetDC = hDC;
		if (bEraseBackground)
		{
			if (bPaint && SomMemoryDC && SomMemoryDC.GetSize().cx >= paint_width && SomMemoryDC.GetSize().cy >= (rcWindow.bottom - rcWindow.top))
			{
				hTargetDC	= SomMemoryDC;
				SetViewportOrgEx(hTargetDC, -(rcWindow.left + LbColumnInfo.x), -rcWindow.top, NULL);
			}

			// 2010-03-18 修改界面显示
			// 2010.03.18 万立新评审通过
			RECT rect = { LbColumnInfo.x, 0, LbColumnInfo.x + nItemWidth, nItemHeight };
			OnDrawItemBackgound(hTargetDC, bHover, bCheck, &rect, nItem, pItemKey, &rcWindow);
		}
		if (bPaint)
		{
			if (hTargetDC == hDC)
			{
				POINT pt;
				OffsetViewportOrgEx(hTargetDC, rcWindow.left + LbColumnInfo.x, rcWindow.top, &pt);
				pSomWnd->OnPaint(hTargetDC, max_paint_width, lpRedrawParam);
				SetViewportOrgEx(hTargetDC, pt.x, pt.y, NULL);
			}
			else
			{
				SetViewportOrgEx(hTargetDC, 0, 0, NULL);
				pSomWnd->OnPaint(hTargetDC, max_paint_width, lpRedrawParam);
				BitBlt(hDC, rcWindow.left + LbColumnInfo.x, rcWindow.top, paint_width, rcWindow.bottom - rcWindow.top, hTargetDC, 0, 0, SRCCOPY);
			}
			if (bExcludeClipRect)
				ExcludeClipRect(hDC, rcWindow.left + LbColumnInfo.x, rcWindow.top, rcWindow.left + LbColumnInfo.x + paint_width, rcWindow.bottom);
		}
	}
	void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
	{
		lpMeasureItemStruct->itemHeight	= m_iLastItemHeight;
	}
	void SetItemHeight(int height)
	{
		m_iLastItemHeight	= height;
	}
	virtual void OnDrawItemBackgound(HDC hDC, BOOL bHover, BOOL bCheck, LPCRECT rcItem, int nItem, DWORD_PTR pItemKey, LPCRECT lpRectDraw = NULL)
	{
		HBRUSH hBackBrush = (HBRUSH)GetClassLong(m_hWnd, GCLP_HBRBACKGROUND);
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
	virtual void OnDrawWhenNoItem(HDC hDC)
	{

	}
protected:
	typedef struct _LBITEMDATA
	{
		CSimpleArray<CSomWnd*> ChildWnds;
		int iItemIndex;

		int columninfo_type;
		DWORD_PTR pdwItemKey;
		DWORD_PTR pdwItemData;

		_LBITEMDATA(DWORD_PTR ItemKey, ISomWndManager* pWndManager, PLBCONTROL pLbControl, int LbControlCount, int _columninfo_type)
		{
			columninfo_type	= _columninfo_type;
			pdwItemKey		= ItemKey;
			pdwItemData		= NULL;
			for (int i = 0; i < LbControlCount; i++)
			{
				LBCONTROL& lbctrl = pLbControl[i];

				if (lbctrl.pSomWnd == NULL)
					continue;

				lbctrl.pSomWnd->CreateWithoutEvent(pWndManager, (void*)this, (void*)lbctrl.column, lbctrl.x, lbctrl.y, lbctrl.cx, lbctrl.cy, lbctrl.nID, lbctrl.dwStyle);
			}
		}
		~_LBITEMDATA()
		{
			for (int i = 0; i < ChildWnds.GetSize(); i++)
			{
				CSomWnd* pSomWnd = ChildWnds[i];
				pSomWnd->DestroyWithoutDetach();
				pSomWnd->Release();
			}
		}

	} LBITEMDATA, *PLBITEMDATA;

	typedef struct _LBCOLUMNINFO
	{
		int min_width;				// 最小的宽度
		float max_width_percent;	// 最大的宽度的百分比
		int x;
		int width;

		_LBCOLUMNINFO(int _min_width, float _max_width_percent)
			: min_width(_min_width)
			, max_width_percent(_max_width_percent)
		{
			x		= 0;
			width	= min_width;
		}

	} LBCOLUMNINFO, *PLBCOLUMNINFO;

	__inline int GetSubItemOffsetX(CSomWnd* pSomWnd)
	{
		PLBITEMDATA pLbData	= (PLBITEMDATA)pSomWnd->m_pWndManagerParam;
		int columninfo_type = pLbData->columninfo_type;
		if (columninfo_type < 0 || columninfo_type >= (int)m_ColumnInfo.size())
		{
			ATLASSERT(FALSE);
			return 0;
		}
		vector<LBCOLUMNINFO>& column_info = m_ColumnInfo[columninfo_type];

		int column = (int)pSomWnd->m_pWndManagerParam2;
		if (column < 0 || column >= (int)column_info.size())
		{
			ATLASSERT(FALSE);
			return 0;
		}
		return column_info[column].x;
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
	void UpdateColumnWidth(int cx = 0, int cy = 0)
	{
		if (cx == 0 && cy == 0)
		{
			RECT rc;
			GetClientRect(&rc);
			cx = rc.right;
			cy = rc.bottom;
		}
		for (vector<vector<LBCOLUMNINFO> >::iterator iter2 = m_ColumnInfo.begin(); iter2 != m_ColumnInfo.end(); iter2++)
		{
			vector<LBCOLUMNINFO>& column_info = *iter2;

			int w = 0;
			for (vector<LBCOLUMNINFO>::iterator iter = column_info.begin(); iter != column_info.end(); iter++)
				w	+= iter->min_width;

			if (w < cx)
				w	= cx - w;
			else
				w	= 0;


			int x = 0;
			float percent = 1;
			for (vector<LBCOLUMNINFO>::iterator iter = column_info.begin(); iter != column_info.end(); iter++)
			{
				iter->x		= x;
				iter->width	= iter->min_width + (int)(w * (min(iter->max_width_percent, percent)));
				x			+= iter->width;
				percent		-= iter->max_width_percent;
				if (percent < 0)
					percent	= 0;
			}
		}
	}
	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if(wParam != SIZE_MINIMIZED)
		{
			int cx = LOWORD(lParam);
			int cy = HIWORD(lParam);

			if (GetCount() == 0 || m_ColumnInfo.size())
			{
				UpdateColumnWidth(cx, cy);
				RedrawWindow();
			}
		}

		bHandled = FALSE;
		return 1;
	}
	LRESULT OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (uMsg != WM_SETCURSOR && (uMsg < WM_MOUSEMOVE || uMsg > WM_RBUTTONDBLCLK))
			return 0;

		POINT pt = {LOWORD(lParam), HIWORD(lParam)};

		if (uMsg == WM_SETCURSOR)
		{
			if ((HWND)wParam != *this || LOWORD(lParam) != HTCLIENT)
				return 0;

			GetCursorPos(&pt);
			ScreenToClient(&pt);
		}

		CSomWnd* pTargetWnd	= NULL;
		CSomWnd* pHoverWnd	= NULL;
		POINT pTargetPt;

		do 
		{
			if (m_pCaptureWnd)
			{
				ATLASSERT(m_pHoverWnd == NULL || m_pHoverWnd == m_pCaptureWnd);

				PLBITEMDATA pItemData	= (PLBITEMDATA)m_pCaptureWnd->m_pWndManagerParam;
				int nItem = GetLbItemIndex(pItemData);
				int nItem_x = GetSubItemOffsetX(m_pCaptureWnd);

				RECT rect;
				GetItemRect(nItem, &rect);

				POINT pos = { pt.x - rect.left - nItem_x, pt.y - rect.top };

				pTargetWnd	= m_pCaptureWnd;
				pTargetPt.x	= pos.x - pTargetWnd->m_rcWindow.left;
				pTargetPt.y	= pos.y - pTargetWnd->m_rcWindow.top;

				if (uMsg == WM_MOUSEMOVE)
				{
					if (PtInRect(&m_pCaptureWnd->m_rcWindow, pos))
						pHoverWnd	= m_pCaptureWnd;
				}
				break;
			}

			BOOL bOutside;
			int nItem = ItemFromPoint(pt, bOutside);

			RECT rect;
			if (nItem == 0xFFFF)
				nItem	= -1;
			else
			{
				GetItemRect(nItem, &rect);
				if (pt.y >= rect.bottom)
					nItem	= -1;
			}

			if (uMsg == WM_MOUSEMOVE && m_bItemHover && nItem != m_iHoverItem)
			{
				int iOld		= m_iHoverItem;  
				m_iHoverItem	= nItem;

				if (iOld != -1)
				{
					RECT rect2;
					GetItemRect(iOld, &rect2);
					InvalidateRect(&rect2, FALSE);
				}
				if (m_iHoverItem != -1)
				{
					InvalidateRect(&rect, FALSE);
				}
			}

			if (nItem == -1)
				break;

			int nSubItem = -1;
			PLBITEMDATA pdata = GetLbItemData(nItem);

			if (pdata == NULL)
				break;

			int columninfo_type = pdata->columninfo_type;
			if (columninfo_type < 0 || columninfo_type >= (int)m_ColumnInfo.size())
			{
				ATLASSERT(FALSE);
				break;
			}
			vector<LBCOLUMNINFO>& column_info = m_ColumnInfo[columninfo_type];

			for (vector<LBCOLUMNINFO>::iterator iter = column_info.begin(); iter != column_info.end(); iter++)
			{
				if (pt.x >= iter->x && pt.x < (iter->x + iter->width))
				{
					nSubItem	= iter - column_info.begin();
					break;
				}
			}
			if (nSubItem == -1)
				break;

			CSimpleArray<CSomWnd*>& ChildWnds = pdata->ChildWnds;

			int count = ChildWnds.GetSize();
			for (int i = 0; i < count; i++)
			{
				CSomWnd* pSomWnd = ChildWnds[i];
				if (nSubItem != (int)pSomWnd->m_pWndManagerParam2)
					continue;

				int nItem_x = GetSubItemOffsetX(pSomWnd);
				POINT pos = { pt.x - rect.left - nItem_x, pt.y - rect.top };
				if (pSomWnd->IsWindowVisible() && pSomWnd->IsWindowEnabled() && PtInRect(&pSomWnd->m_rcWindow, pos))
				{
					pHoverWnd	= pSomWnd;
					pTargetWnd	= pSomWnd;
					pTargetPt.x	= pos.x - pTargetWnd->m_rcWindow.left;
					pTargetPt.y	= pos.y - pTargetWnd->m_rcWindow.top;

					break;
				}
			}
		} while (false);

		if (uMsg == WM_MOUSEMOVE && pHoverWnd != m_pHoverWnd)
		{
			if (m_pHoverWnd)
				m_pHoverWnd->OnMouseLeave();

			m_pHoverWnd	= pHoverWnd;

			if (m_pHoverWnd != NULL)
				m_pHoverWnd->OnMouseEnter(&pTargetPt);
		}

		if (uMsg == WM_SETCURSOR)
		{
			bHandled	= pTargetWnd && pTargetWnd->OnSetCursor(&pTargetPt);
			return TRUE;
		}

		if (pTargetWnd)
		{
			if (uMsg == WM_MOUSEMOVE)
				bHandled	= pTargetWnd->OnMouseMove(&pTargetPt);
			else if (uMsg == WM_LBUTTONDOWN)
				bHandled	= pTargetWnd->OnLButtonDown(&pTargetPt);
			else if (uMsg == WM_LBUTTONUP)
				bHandled	= pTargetWnd->OnLButtonUp(&pTargetPt);
			else if (uMsg == WM_LBUTTONDBLCLK)
				bHandled	= pTargetWnd->OnLButtonDblClk(&pTargetPt);
		}
		else
			bHandled	= FALSE;

		return 0;
	}
	LRESULT OnVScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lRet	= DefWindowProc(uMsg, wParam, lParam);

		SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), SOM_LBN_SCROLL), (LPARAM)m_hWnd);

		return lRet;
	}
	LRESULT OnMouseWheel(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// 为了解决 ListBox 在没有竖直滚动条时收到鼠标滚轮滚动消息时会忽略而不滚屏的现象
		//if (GetStyle() & WS_VSCROLL)
		//	bHandled	= FALSE;
		//else
		{
			int zDelta = (int)GET_WHEEL_DELTA_WPARAM(wParam);

			m_zDelta += zDelta;

			int iWheel = abs(m_zDelta) / WHEEL_DELTA;

			for (int i = 0; i < iWheel; i++)
				SendMessage(WM_VSCROLL, m_zDelta > 0 ? SB_LINEUP : SB_LINEDOWN);

			m_zDelta %= WHEEL_DELTA;
		}
		return 0;
	}
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if (wParam == SOMWNDMANAGER_TIMERID)
		{
			if ((m_iHoverItem != -1 || m_pHoverWnd != NULL) && m_pCaptureWnd == NULL)
			{
				POINT pt;
				RECT rc;
				::GetCursorPos(&pt);
				::GetClientRect(m_hWnd, &rc);
				::MapWindowPoints(m_hWnd, NULL, (LPPOINT)&rc, 2);
				if (!::PtInRect(&rc, pt) ||(WindowFromPoint(pt) != m_hWnd))
				{
					if (m_pHoverWnd)
					{
						m_pHoverWnd->OnMouseLeave();
						m_pHoverWnd	= NULL;
					}
					if (m_bItemHover && m_iHoverItem != -1)
					{
						int iOld		= m_iHoverItem;  
						m_iHoverItem	= -1;

						RECT rect;
						GetItemRect(iOld, &rect);
						InvalidateRect(&rect);
					}
				}
			}
			for (map<DWORD_PTR, PLBITEMDATA>::iterator iter = m_ItemHandleMap.begin(); iter != m_ItemHandleMap.end(); iter++)
			{
				CSimpleArray<CSomWnd*>& ChildWnds	= iter->second->ChildWnds;
				for (int i = 0; i < ChildWnds.GetSize(); i++)
				{
					CSomWnd* pSomWnd = ChildWnds[i];
					pSomWnd->OnTimer();
				}
			}

			if (m_dwDelayResetContentTick != 0 && (GetTickCount() >= m_dwDelayResetContentTick))
			{
				m_dwDelayResetContentTick	= 0;
				ResetContent();
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
			CSomWnd* pCaptureWnd = m_pCaptureWnd;
			m_pCaptureWnd	= NULL;
			pCaptureWnd->OnCaptureChanged();
		}
		else
			bHandled = FALSE;

		return 0;
	}
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;

		ResetSomWnd();

		SetTimer(SOMWNDMANAGER_TIMERID, 100, NULL);

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

		KillTimer(SOMWNDMANAGER_TIMERID);

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

		ResetSomWnd();

		return 0;
	}
	virtual void AttachWindow(CSomWnd* pSomWnd)
	{
		PLBITEMDATA pLbData	= (PLBITEMDATA)pSomWnd->m_pWndManagerParam;
		pLbData->ChildWnds.Add(pSomWnd);
	}
	virtual void DetachWindow(CSomWnd* pSomWnd)
	{
		if (m_pCaptureWnd == pSomWnd)
		{
			SomReleaseCapture(m_pCaptureWnd);
		}
		if (m_pHoverWnd == pSomWnd)
		{
			m_pHoverWnd->OnMouseLeave();

			m_pHoverWnd		= NULL;
		}

		PLBITEMDATA pdata	= (PLBITEMDATA)pSomWnd->m_pWndManagerParam;
		pdata->ChildWnds.Remove(pSomWnd);
	}
	virtual void SomSetCapture(CSomWnd* pSomWnd)
	{
		if (m_pCaptureWnd)
			m_pCaptureWnd->OnCaptureChanged();

		m_pCaptureWnd	= pSomWnd;

		if (GetCapture() != m_hWnd)
		{
			::SetCapture(m_hWnd);
		}
	}
	virtual void SomReleaseCapture(CSomWnd* pSomWnd)
	{
		if (pSomWnd == m_pCaptureWnd)
		{
			::ReleaseCapture();
			m_pCaptureWnd	= NULL;
		}
	}
	virtual LRESULT SomSendMessage(UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		return ::SendMessage(GetParent(), Msg, wParam, lParam);
	}
	virtual LRESULT SomSendCommandMessage(CSomWnd* pSomWnd, WORD wNotifyCode, WORD wID)
	{
		PLBITEMDATA pLbData	= (PLBITEMDATA)pSomWnd->m_pWndManagerParam;
		return ::SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(wID, wNotifyCode), (LPARAM)pLbData->pdwItemKey);
	}
	virtual BOOL SomPostMessage(UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		return ::PostMessage(GetParent(), Msg, wParam, lParam);
	}
	virtual void SomSetWindowPos(CSomWnd* pSomWnd, int x, int y, int cx, int cy)
	{
		BOOL bRepaint	= m_bRedrawToggle && pSomWnd->IsWindowVisible();
		RECT& rcWindow = pSomWnd->m_rcWindow;
		RECT rcTemp		= rcWindow;

		int x2	= rcWindow.left;
		int y2	= rcWindow.top;
		int cx2 = rcWindow.right - rcWindow.left;
		int cy2 = rcWindow.bottom - rcWindow.top;

		if (x != x2 || y != y2 || cx != cx2 || cy != cy2)
		{
			int offset_x		= x - x2;
			int offset_y		= y - y2;

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
			{
				SomListBoxToClient(pSomWnd, &rcTemp);

				InvalidateRect(&rcTemp, FALSE);

				rcTemp.left		+= offset_x;
				rcTemp.top		+= offset_y;
				rcTemp.right	= rcTemp.left + cx;
				rcTemp.bottom	= rcTemp.top + cy;

				InvalidateRect(&rcTemp, FALSE);

				if (m_bUpdateToggle)
					UpdateWindow();
			}
		}
	}
	virtual void SomSizeWindow(CSomWnd* pSomWnd, int cx, int cy)
	{
		BOOL bRepaint	= m_bRedrawToggle && pSomWnd->IsWindowVisible();
		RECT& rcWindow	= pSomWnd->m_rcWindow;
		RECT rcTemp		= rcWindow;

		int cx2 = rcWindow.right - rcWindow.left;
		int cy2 = rcWindow.bottom - rcWindow.top;

		if (cx != cx2 || cy != cy2)
		{
			rcWindow.right	= rcWindow.left + cx;
			rcWindow.bottom	= rcWindow.top + cy;

			pSomWnd->OnSize(cx, cy);

			if (bRepaint)
			{
				SomListBoxToClient(pSomWnd, &rcTemp);

				InvalidateRect(&rcTemp, FALSE);

				rcTemp.right	= rcTemp.left + cx;
				rcTemp.bottom	= rcTemp.top + cy;

				InvalidateRect(&rcTemp, FALSE);

				if (m_bUpdateToggle)
					UpdateWindow();
			}
		}
	}
	virtual void SomMoveWindow(CSomWnd* pSomWnd, int x, int y)
	{
		BOOL bRepaint	= m_bRedrawToggle && pSomWnd->IsWindowVisible();
		RECT& rcWindow	= pSomWnd->m_rcWindow;
		RECT rcTemp		= rcWindow;

		if (x != rcWindow.left || y != rcWindow.top)
		{
			int offset_x = x - rcWindow.left;
			int offset_y = y - rcWindow.top;

			OffsetRect(&rcWindow, offset_x, offset_y);

			pSomWnd->OnMove(x, y);

			if (bRepaint)
			{
				SomListBoxToClient(pSomWnd, &rcTemp);

				InvalidateRect(&rcTemp, FALSE);

				OffsetRect(&rcTemp, offset_x, offset_y);

				InvalidateRect(&rcTemp, FALSE);

				if (m_bUpdateToggle)
					UpdateWindow();
			}
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

		PLBITEMDATA pdata	= (PLBITEMDATA)pSomWnd->m_pWndManagerParam;
		int nItem = GetLbItemIndex(pdata);

		if (nItem == -1)
			return;

		RECT rect;
		GetItemRect(nItem, &rect);
		int nItem_x = GetSubItemOffsetX(pSomWnd);
		OffsetRect(&rcPaint, rect.left + nItem_x, rect.top);

		if (m_bUpdateToggle)
		{
			HDC hDC = GetDC();

			IntersectClipRect(hDC, rcPaint.left, rcPaint.top, rcPaint.right, rcPaint.bottom);

			POINT pt;
			OffsetViewportOrgEx(hDC, rect.left, rect.top, &pt);

			SomDrawWnd(hDC, pSomWnd, bPaint, bEraseBackground, m_iHoverItem == nItem, GetCurSel() == nItem, nItem, rect.right - rect.left, rect.bottom - rect.top, pdata->pdwItemKey, FALSE, lpRedrawParam);

			SetViewportOrgEx(hDC, pt.x, pt.y, NULL);

			ReleaseDC(hDC);
		}
		else
			InvalidateRect(&rcPaint, FALSE);
	}
	virtual void SomAddTooltip(CSomWnd* pSomWnd, UINT_PTR uInnerTipId, LPRECT lpRect)
	{
		PLBITEMDATA pdata	= (PLBITEMDATA)pSomWnd->m_pWndManagerParam;
		int nItem = GetLbItemIndex(pdata);

		if (nItem == -1)
			return;

		RECT rect;
		GetItemRect(nItem, &rect);

		UINT_PTR uToolTipId = AddToolTipId(pSomWnd, uInnerTipId, lpRect, rect.left + GetSubItemOffsetX(pSomWnd), rect.top);
		ATLASSERT(uToolTipId != NULL);
	}
	virtual void SomDelTooltip(CSomWnd* pSomWnd)
	{
		DelToolTipId(pSomWnd);
	}
	int SomListBoxToClient(CSomWnd* pSomWnd, LPRECT lpRect)
	{
		PLBITEMDATA pdata	= (PLBITEMDATA)pSomWnd->m_pWndManagerParam;
		int nItem = GetLbItemIndex(pdata);

		if (nItem == -1)
			return -1;
		
		RECT rect;
		GetItemRect(nItem, &rect);
		OffsetRect(lpRect, rect.left + GetSubItemOffsetX(pSomWnd), rect.top);

		return nItem;
	}
	int GetLbItemIndex(PLBITEMDATA pLbItemData)
	{
		if ((PLBITEMDATA)CListBox::GetItemData(pLbItemData->iItemIndex) != pLbItemData)
		{
			int iCount = GetCount();
			for (int i = 0; i < iCount; i++)
			{
				PLBITEMDATA pData = (PLBITEMDATA)CListBox::GetItemData(i);
				if (pData)
					pData->iItemIndex	= i;
			}

			ATLASSERT((PLBITEMDATA)CListBox::GetItemData(pLbItemData->iItemIndex) == pLbItemData);

			if ((PLBITEMDATA)CListBox::GetItemData(pLbItemData->iItemIndex) == pLbItemData)
				return pLbItemData->iItemIndex;
			else
				return -1;
		}
		else
			return pLbItemData->iItemIndex;
	}
	__inline PLBITEMDATA GetLbItemData(int nIndex) const
	{
		ATLASSERT(nIndex >= 0 && nIndex < GetCount());

		PLBITEMDATA pdata = (PLBITEMDATA)CListBox::GetItemData(nIndex);
		ATLASSERT((pdata != (PLBITEMDATA)LB_ERR) && (pdata != NULL));

		if (pdata == (PLBITEMDATA)LB_ERR)
			return NULL;
		else
			return pdata;
	}
	void ResetSomWnd()
	{
		m_bRedrawToggle	= TRUE;
		m_bUpdateToggle	= TRUE;
		m_pCaptureWnd	= NULL;
		m_pHoverWnd		= NULL;
		m_iHoverItem	= -1;
		m_zDelta		= 0;
		m_bItemHover	= FALSE;
		m_iLastItemHeight	= ::GetSystemMetrics(SM_CYMENU);
		m_dwDelayResetContentTick	= 0;
		m_ColumnInfo.clear();
		m_ColumnInfo.resize(1);
	}

	BOOL m_bRedrawToggle;
	BOOL m_bUpdateToggle;
	BOOL m_bItemHover;
	map<DWORD_PTR, PLBITEMDATA> m_ItemHandleMap;
	vector<vector<LBCOLUMNINFO> > m_ColumnInfo;
	int m_zDelta;
	int m_iHoverItem;
	CSomWnd* m_pCaptureWnd;
	CSomWnd* m_pHoverWnd;
	int m_iLastItemHeight;
	DWORD m_dwDelayResetContentTick;

#ifdef _DEBUG
	int m_iItemDataCount;
#endif
};

#define SOM_LVN_FIRST              (0U-10000U)       // somlistview
#define SOM_LVN_LAST               (0U-10099U)

#define SOM_LVN_SELCHANGE          (SOM_LVN_FIRST-0)
#define SOM_LVN_DBLCLK             (SOM_LVN_FIRST-1)
#define SOM_LVN_RCLICK             (SOM_LVN_FIRST-2)

typedef struct tagSOMNMHDR
{
	NMHDR		hdr;
	int			iItem;

} SOMNMHDR, *LPSOMNMHDR;

class CSomListView 
	: public CWindowImpl<CSomListView
	, CListViewCtrl>
	, public CCustomDraw<CSomListView>
	, public ISomWndManager
	, public CSomToolTipAdpater
{
public:
	static DWORD GetWndStyle(DWORD dwStyle)
	{
		return dwStyle == 0 ? WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | LVS_REPORT | LVS_SINGLESEL : dwStyle;
	}

	typedef struct _LVCONTROL
	{
		int column;
		CSomWnd* pSomWnd;	
		int x;
		int y;
		int cx;
		int cy;
		UINT nID;
		DWORD dwStyle;

	}LVCONTROL, *PLVCONTROL;


	BEGIN_MSG_MAP(CSomListView)
		NOTIFY_CODE_HANDLER(TTN_GETDISPINFO, OnGetInfoTip)
		MESSAGE_RANGE_HANDLER(WM_MOUSEFIRST, WM_MBUTTONDBLCLK, OnMouseMessage)
		MESSAGE_HANDLER(WM_SETCURSOR, OnMouseMessage)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		CHAIN_MSG_MAP_ALT(CCustomDraw<CSomListView>, 1)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
//		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_CAPTURECHANGED, OnCaptureChanged)
		REFLECTED_NOTIFY_CODE_HANDLER(LVN_DELETEITEM, OnDeleteItem)
		NOTIFY_CODE_HANDLER(HDN_DIVIDERDBLCLICK, OnDividerDblClick)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SETREDRAW, OnSetRedraw)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()

	CSomListView(void)
	{
		ResetSomWnd();

#ifdef _DEBUG
		m_iItemDataCount	= 0;
#endif
	}
	~CSomListView(void)
	{
#ifdef _DEBUG
		ATLASSERT(m_iItemDataCount == 0);
#endif
		ATLASSERT(m_ItemHandleMap.size() == 0);
	}

	struct _LVITEMDATA;

	typedef struct _LVSUBITEMDATA
	{
		_LVITEMDATA* pLvItemData;
		int iSubItemIndex;
		CSimpleArray<CSomWnd*> ChildWnds;

		_LVSUBITEMDATA(_LVITEMDATA* pItemData, int iSubItem)
		{
			pLvItemData		= pItemData;
			iSubItemIndex	= iSubItem;
		}
	} LVSUBITEMDATA, *PLVSUBITEMDATA;

	typedef struct _LVITEMDATA
	{
		CSimpleArray<PLVSUBITEMDATA> SubItemDatas; 

		int iItemIndex;

		DWORD_PTR pdwItemKey;
		DWORD_PTR pdwItemData;

		_LVITEMDATA(DWORD_PTR ItemKey, ISomWndManager* pWndManager, PLVCONTROL pLvControl, int LvControlCount)
		{
			pdwItemKey	= ItemKey;
			pdwItemData	= NULL;

			int iMaxColumn = 0;
			for (int i = 0; i < LvControlCount; i++)
			{
				LVCONTROL& lvctrl = pLvControl[i];
				if (lvctrl.column >= iMaxColumn)
					iMaxColumn	= lvctrl.column + 1;
			}
			for (int i = 0; i < iMaxColumn; i++)
			{
				SubItemDatas.Add(NULL);
			}
			for (int i = 0; i < LvControlCount; i++)
			{
				LVCONTROL& lvctrl = pLvControl[i];

				if (lvctrl.pSomWnd == NULL)
					continue;

				PLVSUBITEMDATA pLvSubItemData = SubItemDatas[lvctrl.column];
				if (pLvSubItemData == NULL)
				{
					pLvSubItemData	= new _LVSUBITEMDATA(this, lvctrl.column);
					SubItemDatas[lvctrl.column]	= pLvSubItemData;
				}

				lvctrl.pSomWnd->Create(pWndManager, (void*)pLvSubItemData, NULL, lvctrl.x, lvctrl.y, lvctrl.cx, lvctrl.cy, lvctrl.nID, lvctrl.dwStyle);
			}
		}
		~_LVITEMDATA()
		{
			for (int i = 0; i < SubItemDatas.GetSize(); i++)
			{
				PLVSUBITEMDATA pSubItemData = SubItemDatas[i];
				if (pSubItemData)
				{
					CSimpleArray<CSomWnd*>& pChildWnds = pSubItemData->ChildWnds;

					for (int i = 0; i < pChildWnds.GetSize(); i++)
					{
						CSomWnd* pSomWnd = pChildWnds[i];
						pSomWnd->DestroyWithoutDetach();
						pSomWnd->Release();
					}

					delete pSubItemData;
				}
			}
			SubItemDatas.RemoveAll();
		}

	} LVITEMDATA, *PLVITEMDATA;

	void SetToolTips(HWND hWndToolTip)
	{
		ATLASSERT(::IsWindow(m_hWnd) && ::IsWindow(hWndToolTip));
		CSomToolTipAdpater::SetToolTips(m_hWnd, hWndToolTip);
	}
	void AddColumn(LPCTSTR strItem, int min_width, float max_width_percent = 0)
	{
		CListViewCtrl::AddColumn(strItem, m_ColumnInfo.size());
		m_ColumnInfo.push_back(LVCOLUMNINFO(min_width, max_width_percent));
		UpdateColumnWidth();
	}
	void SetColumnWidth(int nCol, int min_width, float max_width_percent = 0)
	{
		if (nCol < 0 || nCol >= (int)m_ColumnInfo.size())
		{
			ATLASSERT(FALSE);
			return;
		}
		LVCOLUMNINFO& LvColumnInfo		= m_ColumnInfo[nCol];
		LvColumnInfo.min_width			= min_width;
		LvColumnInfo.max_width_percent	= max_width_percent;
	}
	void DeleteColumn(int nCol)
	{
		if ((int)m_ColumnInfo.size() > nCol)
		{
			CListViewCtrl::DeleteColumn(nCol);
			m_ColumnInfo.erase(m_ColumnInfo.begin() + nCol);
		}
	}
	void DeleteItemAllSomControls(DWORD_PTR pItemKey)
	{
		map<DWORD_PTR, PLVITEMDATA>::iterator iter = m_ItemHandleMap.find(pItemKey);
		if (iter == m_ItemHandleMap.end())
			return;

		PLVITEMDATA pdata	= iter->second;
		ATLASSERT(pdata != NULL);

		int nIndex = GetLvItemIndex(pdata);
		if (nIndex != -1)
			DeleteItemAllSomControls(nIndex);
	}
	void DeleteItemAllSomControls(int nIndex)
	{
		PLVITEMDATA pdata = GetLvItemData(nIndex);
		CListViewCtrl::SetItemData(nIndex, NULL);
		if (pdata == NULL)
			return;

		if (m_pCaptureWnd && ((PLVSUBITEMDATA)m_pCaptureWnd->m_pWndManagerParam)->pLvItemData == pdata)
		{
			SomReleaseCapture(m_pCaptureWnd);
		}
		if (m_pHoverWnd && ((PLVSUBITEMDATA)m_pHoverWnd->m_pWndManagerParam)->pLvItemData == pdata)
		{
			m_pHoverWnd->OnMouseLeave();
			m_pHoverWnd		= NULL;
		}

		if (HIWORD(pdata->pdwItemKey) != 0xFFFF)
		{
			map<DWORD_PTR, PLVITEMDATA>::iterator iter = m_ItemHandleMap.find(pdata->pdwItemKey);

			// 2010-03-26 出现了m_ItemHandleMap.erase(iter); 崩溃 由于iter == m_ItemHandleMap.end()
			// 具体原因不清楚 先做下处理 
			ATLASSERT(iter->second == pdata);

			if (iter != m_ItemHandleMap.end())
				m_ItemHandleMap.erase(iter);
			else
				ATLASSERT(0);
		}

		delete pdata;

#ifdef _DEBUG
		m_iItemDataCount--;
#endif
	}
	DWORD_PTR GetLvItemKey(int nIndex)
	{
		ATLASSERT(nIndex >= 0 && nIndex < GetItemCount());

		if (nIndex >= 0 && nIndex < GetItemCount())
		{
			PLVITEMDATA pdata = GetLvItemData(nIndex);
			if (pdata)
				return pdata->pdwItemKey;
		}

		return NULL;
	}
	UINT GetSelectedCount() const
	{
		return m_iSelectItem == -1 ? 0 : 1;
	}
	int GetSelectedIndex() const
	{
		return m_iSelectItem;
	}
	CSomWnd* GetSomControl(DWORD_PTR pItemKey, int nSubItem, int nCtrlIndex)
	{
		map<DWORD_PTR, PLVITEMDATA>::iterator iter = m_ItemHandleMap.find(pItemKey);

		if (iter == m_ItemHandleMap.end())
			return NULL;

		PLVITEMDATA pdata	= iter->second;
		ATLASSERT(pdata != NULL);

		if (nSubItem < 0 || nSubItem >= pdata->SubItemDatas.GetSize())
			return NULL;

		PLVSUBITEMDATA pSubItemData = pdata->SubItemDatas[nSubItem];

		CSimpleArray<CSomWnd*>& pChildWnds = pSubItemData->ChildWnds;

		if (nCtrlIndex < 0 || nCtrlIndex >= pChildWnds.GetSize())
			return NULL;

		return pChildWnds[nCtrlIndex];
	}
	CSomWnd* GetSomControl(int nItem, int nSubItem, int nCtrlIndex)
	{
		PLVITEMDATA pdata = GetLvItemData(nItem);

		if (pdata == NULL)
			return NULL;

		if (nSubItem < 0 || nSubItem >= pdata->SubItemDatas.GetSize())
			return NULL;

		PLVSUBITEMDATA pSubItemData = pdata->SubItemDatas[nSubItem];

		CSimpleArray<CSomWnd*>& pChildWnds = pSubItemData->ChildWnds;

		if (nCtrlIndex < 0 || nCtrlIndex >= pChildWnds.GetSize())
			return NULL;

		return pChildWnds[nCtrlIndex];
	}
	// pItemKey 用来作为使用者对于这个 Item 的标识，之后使用者对这个 Item 的操作以及 ListView 对使用者的通知都用这个 pItemKey 来定位这个 Item
	// 所以 pItemKey 不能重复，这样使用者可以选择用从 0 开始的下标做key，或者是 this 指针做key，为了兼容那些没有 key 的 Item，目前保留高字为 0xFFFF 的
	// key，所以这些 key 可以重复，缺点是使用者不能用这些 key 来定位 Item 了
	int InsertItem(int nItem, PLVCONTROL pLvControl, int LvControlCount, DWORD_PTR pItemKey = (DWORD_PTR)-1)
	{
		int iRet;
		if (HIWORD(pItemKey) == 0xFFFF)
		{
			iRet	= CListViewCtrl::InsertItem(nItem, NULL);
			if (iRet < 0)
				return iRet;

			CListViewCtrl::SetItemData(iRet, (DWORD_PTR)new LVITEMDATA(pItemKey, this, pLvControl, LvControlCount));
#ifdef _DEBUG
			m_iItemDataCount++;
#endif
			return iRet;
		}

		if (m_ItemHandleMap.find(pItemKey) != m_ItemHandleMap.end())
		{
			ATLASSERT(FALSE);
			return -1;
		}
		else
		{
			iRet	= CListViewCtrl::InsertItem(nItem, NULL);
			if (iRet < 0)
				return iRet;

			PLVITEMDATA h = new LVITEMDATA(pItemKey, this, pLvControl, LvControlCount);
			m_ItemHandleMap[pItemKey]	= h;

			CListViewCtrl::SetItemData(iRet, (DWORD_PTR)h);
#ifdef _DEBUG
			m_iItemDataCount++;
#endif
			return iRet;
		}
	}
	void EnableItemHover(BOOL bHover = TRUE)
	{
		m_bItemHover	= bHover;
	}
	BOOL SubclassWindow(HWND hWnd)
	{
		BOOL bRet = CWindowImpl<CSomListView, CListViewCtrl>::SubclassWindow(hWnd);
		SetTimer(SOMWNDMANAGER_TIMERID, 100, NULL);
		return bRet;
	}
	BOOL SetItemData(int nItem, DWORD_PTR pdwItemData)
	{
		PLVITEMDATA pdata = GetLvItemData(nItem);

		if (pdata == NULL)
			return FALSE;

		pdata->pdwItemData	= pdwItemData;
		return TRUE;
	}
	DWORD_PTR GetItemData(int nItem) const
	{
		PLVITEMDATA pdata = GetLvItemData(nItem);

		if (pdata == NULL)
			return NULL;

		return pdata->pdwItemData;
	}
	void SetItemHeight(int height)
	{
		CImageList imagelist;
		imagelist.Create(1, height - 1, ILC_COLOR, 1, 1);
		SetImageList(imagelist.m_hImageList, LVSIL_SMALL);
		imagelist.Destroy();
	}
	DWORD OnPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw)
	{
		return CDRF_NOTIFYITEMDRAW;
	}
	DWORD OnItemPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw)
	{
		return CDRF_NOTIFYSUBITEMDRAW|CDRF_NOTIFYPOSTPAINT;
	}
	DWORD OnItemPostPaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw)
	{
		NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(lpNMCustomDraw);
		int nItem = (int)pLVCD->nmcd.dwItemSpec;

		RECT rect;
		GetItemRect(nItem, &rect, LVIR_BOUNDS);

		PLVITEMDATA pdata = GetLvItemData(nItem);

		if (pdata)
			OnDrawItemBackgound(pLVCD->nmcd.hdc, m_iHoverItem == nItem, m_iSelectItem == nItem, &rect, nItem, pdata->pdwItemKey);

		return CDRF_SKIPDEFAULT;
	}
	DWORD OnSubItemPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw)
	{
		NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(lpNMCustomDraw);

		int nItem = (int)pLVCD->nmcd.dwItemSpec;
		int nSubItem = pLVCD->iSubItem;

		RECT rect;
		GetSubItemRect(nItem, nSubItem, LVIR_LABEL, &rect);

		PLVITEMDATA pdata = GetLvItemData(nItem);

		if (pdata && (nSubItem < pdata->SubItemDatas.GetSize()))
		{
			PLVSUBITEMDATA pLvSubItemData = pdata->SubItemDatas[nSubItem];
			if (pLvSubItemData == NULL)
				return CDRF_SKIPDEFAULT;

			HDC hDC = pLVCD->nmcd.hdc;
			CSimpleArray<CSomWnd*>& ChildWnds = pLvSubItemData->ChildWnds;
			RECT rcUpdate;
			GetClipBox(hDC, &rcUpdate);

			POINT pt;
			OffsetViewportOrgEx(hDC, rect.left, rect.top, &pt);
			for (int i = 0; i < ChildWnds.GetSize(); i++)
			{
				CSomWnd* pSomWnd = ChildWnds[i];

				if (pSomWnd->IsWindowVisible())
					SomDrawWnd(hDC, pSomWnd, TRUE, pSomWnd->IsWindowTransparent(), nItem,  pdata->pdwItemKey, rect.right - rect.left, rect.bottom - rect.top, TRUE, NULL);
			}
			SetViewportOrgEx(hDC, pt.x, pt.y, NULL);

			return CDRF_SKIPDEFAULT;
		}
		else
			return CDRF_DODEFAULT;
	}
	void SomDrawWnd(HDC hDC, CSomWnd* pSomWnd, BOOL bPaint, BOOL bEraseBackground, int nItem, DWORD_PTR pItemKey, int nItemWidth, int nItemHeight, BOOL bExcludeClipRect, LPVOID lpRedrawParam)
	{
		RECT rcWindow = pSomWnd->m_rcWindow;

		if (rcWindow.left > nItemWidth)
			return;

		int paint_width		= rcWindow.right - rcWindow.left;
		int max_paint_width	= -1;
		if (nItemWidth < rcWindow.right)
		{
			paint_width		= nItemWidth - rcWindow.left;
			max_paint_width	= paint_width;
		}

		HDC hTargetDC = hDC;
		if (bEraseBackground)
		{
			if (bPaint && SomMemoryDC && SomMemoryDC.GetSize().cx >= paint_width && SomMemoryDC.GetSize().cy >= (rcWindow.bottom - rcWindow.top))
			{
				hTargetDC	= SomMemoryDC;
				SetViewportOrgEx(hTargetDC, -rcWindow.left, -rcWindow.top, NULL);
			}
			RECT rect = { 0, 0, nItemWidth, nItemHeight };
			OnDrawItemBackgound(hTargetDC, m_iHoverItem == nItem, m_iSelectItem == nItem, &rect, nItem, pItemKey, &rcWindow);
		}
		if (bPaint)
		{
			if (hTargetDC == hDC)
			{
				POINT pt;
				OffsetViewportOrgEx(hTargetDC, rcWindow.left, rcWindow.top, &pt);
				pSomWnd->OnPaint(hTargetDC, max_paint_width, lpRedrawParam);
				SetViewportOrgEx(hTargetDC, pt.x, pt.y, NULL);
			}
			else
			{
				SetViewportOrgEx(hTargetDC, 0, 0, NULL);
				pSomWnd->OnPaint(hTargetDC, max_paint_width, lpRedrawParam);
				BitBlt(hDC, rcWindow.left, rcWindow.top, paint_width, rcWindow.bottom - rcWindow.top, hTargetDC, 0, 0, SRCCOPY);
			}
			if (bExcludeClipRect)
				ExcludeClipRect(hDC, rcWindow.left, rcWindow.top, rcWindow.left + paint_width, rcWindow.bottom);
		}
	}
	BOOL IsItemExists(DWORD_PTR pItemKey)
	{
		return m_ItemHandleMap.find(pItemKey) != m_ItemHandleMap.end();
	}
	int GetItemIndex(DWORD_PTR pItemKey)
	{
		map<DWORD_PTR, PLVITEMDATA>::iterator iter = m_ItemHandleMap.find(pItemKey);
		if (iter == m_ItemHandleMap.end())
			return -1;

		return GetLvItemIndex(iter->second);
	}
	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		int count = GetItemCount();
		if (count == 0)
			return TRUE;

		RECT rcClient;
		GetClientRect(&rcClient);

		RECT rcTail;
		if (GetItemRect(GetItemCount() - 1, &rcTail, LVIR_BOUNDS))
		{
			RECT rc;

			if (rcTail.bottom < rcClient.bottom)
			{
				rc = rcClient;
				rc.top	= rcTail.bottom;

				OnDrawItemBackgound((HDC)wParam, FALSE, FALSE, &rc, -1, -1, NULL);
			}

			if (rcTail.left > 0)
			{
				rc = rcClient;
				if (rcTail.bottom < rcClient.bottom)
					rc.bottom	=  rcTail.bottom;
				rc.right	= rcTail.left;

				OnDrawItemBackgound((HDC)wParam, FALSE, FALSE, &rc, -1, -1, NULL);
			}

			if (rcTail.right < rcClient.right)
			{
				rc = rcClient;
				if (rcTail.bottom < rcClient.bottom)
					rc.bottom	=  rcTail.bottom;
				rc.left	= rcTail.right;

				OnDrawItemBackgound((HDC)wParam, FALSE, FALSE, &rc, -1, -1, NULL);
			}

			return TRUE;
		}

		OnDrawItemBackgound((HDC)wParam, FALSE, FALSE, &rcClient, -1, -1, NULL);

		return TRUE;
	}
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		if (GetItemCount() == 0)
		{
			CPaintDC dc(*this);

			RECT rcClient;
			GetClientRect(&rcClient);

			OnDrawItemBackgound(dc, FALSE, FALSE, &rcClient, -1, -1, NULL);

			OnDrawWhenNoItem(dc);

			return 0;
		}
		else
			return DefWindowProc(uMsg, wParam, lParam);
	}
	virtual void OnDrawItemBackgound(HDC hDC, BOOL bHover, BOOL bCheck, LPCRECT rcItem, int nItem, DWORD_PTR pItemKey, LPCRECT lpRectDraw = NULL)
	{
		HBRUSH hBackBrush = (HBRUSH)GetClassLong(m_hWnd, GCLP_HBRBACKGROUND);
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
	virtual void OnDrawWhenNoItem(HDC hDC)
	{

	}
	virtual void SomDelayUpdateWindow()
	{
		m_bUpdateToggle	= FALSE;
	}
	virtual void SomCommitUpdateWindow()
	{
		m_bUpdateToggle	= TRUE;

		if (m_bRedrawToggle)
			UpdateWindow();
	}
	void UpdateColumnWidth(int cx = 0, int cy = 0)
	{
		if (cx == 0 && cy == 0)
		{
			RECT rc;
			GetClientRect(&rc);
			cx = rc.right;
			cy = rc.bottom;
		}
		cx -= 20;
		int w = 0;
		for (vector<LVCOLUMNINFO>::iterator iter = m_ColumnInfo.begin(); iter != m_ColumnInfo.end(); iter++)
			w	+= iter->min_width;

		if (w < cx)
			w	= cx - w;
		else
			w	= 0;


		int x = 0;
		float percent = 1;
		for (vector<LVCOLUMNINFO>::iterator iter = m_ColumnInfo.begin(); iter != m_ColumnInfo.end(); iter++)
		{
			iter->x		= x;
			iter->width	= iter->min_width + (int)(w * (min(iter->max_width_percent, percent)));
			x			+= iter->width;
			percent		-= iter->max_width_percent;
			if (percent < 0)
				percent	= 0;

			CListViewCtrl::SetColumnWidth(iter - m_ColumnInfo.begin(), iter->width);
		}
	}
protected:
	LRESULT OnDeleteItem(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
	{
		LPNMLISTVIEW lpNMLISTVIEW	= (LPNMLISTVIEW)pnmh;

		if (m_iHoverItem == lpNMLISTVIEW->iItem)
			m_iHoverItem	= -1;

		if (m_iSelectItem == lpNMLISTVIEW->iItem)
			m_iSelectItem	= -1;

		DeleteItemAllSomControls(lpNMLISTVIEW->iItem);

		return 0;
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
	//LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	//{
	//	if(wParam != SIZE_MINIMIZED)
	//	{
	//		int cx = LOWORD(lParam);
	//		int cy = HIWORD(lParam);

	//		if (m_ColumnInfo.size())
	//		{
	//			LRESULT lRet = DefWindowProc(uMsg, wParam, lParam);
	//			UpdateColumnWidth(cx, cy);
	//			return lRet;
	//		}
	//	}

	//	bHandled = FALSE;
	//	return 1;
	//}
	LRESULT OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (uMsg != WM_SETCURSOR && (uMsg < WM_MOUSEMOVE || uMsg > WM_RBUTTONDBLCLK))
			return 0;

		POINT pt = {LOWORD(lParam), HIWORD(lParam)};

		if (uMsg == WM_SETCURSOR)
		{
			if ((HWND)wParam != *this || LOWORD(lParam) != HTCLIENT)
				return 0;

			GetCursorPos(&pt);
			ScreenToClient(&pt);
		}

		CSomWnd* pTargetWnd	= NULL;
		CSomWnd* pHoverWnd	= NULL;
		POINT pTargetPt;

		LVHITTESTINFO lvhit;
		do 
		{
			if (m_pCaptureWnd)
			{
				PLVSUBITEMDATA pSubItemData	= (PLVSUBITEMDATA)m_pCaptureWnd->m_pWndManagerParam;
				int nItem = GetLvItemIndex(pSubItemData->pLvItemData);
				int nSubItem = pSubItemData->iSubItemIndex;

				RECT rect;
				GetSubItemRect(nItem, nSubItem, LVIR_LABEL, &rect);

				POINT pos = { pt.x - rect.left, pt.y - rect.top };

				pTargetWnd	= m_pCaptureWnd;
				pTargetPt.x	= pos.x - pTargetWnd->m_rcWindow.left;
				pTargetPt.y	= pos.y - pTargetWnd->m_rcWindow.top;

				if (uMsg == WM_MOUSEMOVE)
				{
					if (PtInRect(&m_pCaptureWnd->m_rcWindow, pos))
						pHoverWnd	= m_pCaptureWnd;
				}
				break;
			}

			lvhit.pt	= pt;
			SubItemHitTest(&lvhit);

			if (uMsg == WM_MOUSEMOVE && m_bItemHover && lvhit.iItem != m_iHoverItem)
			{
				int iOld		= m_iHoverItem;  
				m_iHoverItem	= lvhit.iItem;

				if (iOld != -1)
				{
					RECT rect;
					GetItemRect(iOld, &rect, LVIR_BOUNDS);
					rect.bottom -= 1;	// 避免 listview 的 bug 导致多刷新下面一个 item
					InvalidateRect(&rect, FALSE);
				}
				if (m_iHoverItem != -1)
				{
					RECT rect;
					GetItemRect(m_iHoverItem, &rect, LVIR_BOUNDS);
					rect.bottom -= 1;	// 避免 listview 的 bug 导致多刷新下面一个 item
					InvalidateRect(&rect, FALSE);
				}
			}

			if (lvhit.iItem == -1 || lvhit.iSubItem == -1)
				break;

			PLVITEMDATA pdata = GetLvItemData(lvhit.iItem);

			if (pdata && (lvhit.iSubItem < pdata->SubItemDatas.GetSize()))
			{ 
				PLVSUBITEMDATA pLvSubItemData = pdata->SubItemDatas[lvhit.iSubItem];
				if (pLvSubItemData)
				{
					RECT rect;
					GetSubItemRect(lvhit.iItem, lvhit.iSubItem, LVIR_LABEL, &rect);

					POINT pos = { pt.x - rect.left, pt.y - rect.top };

					CSimpleArray<CSomWnd*>& ChildWnds = pdata->SubItemDatas[lvhit.iSubItem]->ChildWnds;
					int count = ChildWnds.GetSize();
					for (int i = 0; i < count; i++)
					{
						CSomWnd* pSomWnd = ChildWnds[i];
						if (pSomWnd->IsWindowVisible() && pSomWnd->IsWindowEnabled() && PtInRect(&pSomWnd->m_rcWindow, pos))
						{
							pHoverWnd	= pSomWnd;
							pTargetWnd	= pSomWnd;
							pTargetPt.x	= pos.x - pTargetWnd->m_rcWindow.left;
							pTargetPt.y	= pos.y - pTargetWnd->m_rcWindow.top;

							break;
						}
					}
				}
			}

		} while (false);

		if (uMsg == WM_MOUSEMOVE && pHoverWnd != m_pHoverWnd)
		{
			if (m_pHoverWnd)
				m_pHoverWnd->OnMouseLeave();

			m_pHoverWnd	= pHoverWnd;

			if (m_pHoverWnd != NULL)
				m_pHoverWnd->OnMouseEnter(&pTargetPt);
		}

		if (uMsg == WM_SETCURSOR)
		{
			bHandled	= pTargetWnd && pTargetWnd->OnSetCursor(&pTargetPt);
			return TRUE;
		}

		bHandled	= FALSE;

		if (pTargetWnd)
		{
			if (uMsg == WM_MOUSEMOVE)
				bHandled	= pTargetWnd->OnMouseMove(&pTargetPt);
			else if (uMsg == WM_LBUTTONDOWN)
				bHandled	= pTargetWnd->OnLButtonDown(&pTargetPt);
			else if (uMsg == WM_LBUTTONUP)
				bHandled	= pTargetWnd->OnLButtonUp(&pTargetPt);
			else if (uMsg == WM_LBUTTONDBLCLK)
				bHandled	= pTargetWnd->OnLButtonDblClk(&pTargetPt);
		}

		if (!bHandled)
		{
			if ((uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN) && lvhit.iItem != m_iSelectItem)
			{
				int iOld		= m_iSelectItem;  
				m_iSelectItem	= lvhit.iItem;

				if (iOld != -1)
				{
					RECT rect;
					GetItemRect(iOld, &rect, LVIR_BOUNDS);
					rect.bottom -= 1;	// 避免 listview 的 bug 导致多刷新下面一个 item
					InvalidateRect(&rect, FALSE);
				}
				if (m_iSelectItem != -1)
				{
					RECT rect;
					GetItemRect(m_iSelectItem, &rect, LVIR_BOUNDS);
					rect.bottom -= 1;	// 避免 listview 的 bug 导致多刷新下面一个 item
					InvalidateRect(&rect, FALSE);
				}
				SetFocus();

				SOMNMHDR nmhdr = { *this, GetDlgCtrlID(), SOM_LVN_SELCHANGE, lvhit.iItem };
				SendMessage(GetParent(), WM_NOTIFY, 0, (LPARAM)&nmhdr);
			}
			else if (uMsg == WM_LBUTTONDBLCLK && lvhit.iItem != -1)
			{
				SOMNMHDR nmhdr = { *this, GetDlgCtrlID(), SOM_LVN_DBLCLK, lvhit.iItem };
				SendMessage(GetParent(), WM_NOTIFY, 0, (LPARAM)&nmhdr);
			}
			else if (uMsg == WM_RBUTTONUP && lvhit.iItem != -1)
			{
				SOMNMHDR nmhdr = { *this, GetDlgCtrlID(), SOM_LVN_RCLICK, lvhit.iItem };
				SendMessage(GetParent(), WM_NOTIFY, 0, (LPARAM)&nmhdr);
			}
			bHandled = TRUE;
		}

		return 0;
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
				::GetClientRect(m_hWnd, &rc);
				::MapWindowPoints(m_hWnd, NULL, (LPPOINT)&rc, 2);
				if (!::PtInRect(&rc, pt) ||(WindowFromPoint(pt) != m_hWnd))
				{
					m_pHoverWnd->OnMouseLeave();
					m_pHoverWnd	= NULL;
				}
			}
			for (map<DWORD_PTR, PLVITEMDATA>::iterator iter = m_ItemHandleMap.begin(); iter != m_ItemHandleMap.end(); iter++)
			{
				CSimpleArray<PLVSUBITEMDATA>& SubItemDatas = iter->second->SubItemDatas;

				for (int i = 0; i < SubItemDatas.GetSize(); i++)
				{
					CSimpleArray<CSomWnd*>& ChildWnds	= SubItemDatas[i]->ChildWnds;
					for (int i = 0; i < ChildWnds.GetSize(); i++)
					{
						CSomWnd* pSomWnd = ChildWnds[i];
						pSomWnd->OnTimer();
					}
				}
			}

			return TRUE;
		}
		else
			bHandled = FALSE;

		return 0;
	}
	LRESULT OnDividerDblClick(int idCtrl, LPNMHDR lpNMHDR, BOOL& /*bHandled*/)
	{
		return 0;
	}
	LRESULT OnCaptureChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		if (m_pCaptureWnd)
		{
			CSomWnd* pCaptureWnd = m_pCaptureWnd;
			m_pCaptureWnd	= NULL;
			pCaptureWnd->OnCaptureChanged();
		}
		else
			bHandled = FALSE;

		return 0;
	}
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;

		ResetSomWnd();

		SetTimer(SOMWNDMANAGER_TIMERID, 100, NULL);

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

		KillTimer(SOMWNDMANAGER_TIMERID);

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

		ResetSomWnd();

		return 0;
	}

	virtual void AttachWindow(CSomWnd* pSomWnd)
	{
		PLVSUBITEMDATA pSubItemData	= (PLVSUBITEMDATA)pSomWnd->m_pWndManagerParam;
		CSimpleArray<CSomWnd*>& ChildWnds = pSubItemData->ChildWnds;

		ChildWnds.Add(pSomWnd);
	}
	virtual void DetachWindow(CSomWnd* pSomWnd)
	{
		if (m_pCaptureWnd == pSomWnd)
		{
			SomReleaseCapture(pSomWnd);
		}
		if (m_pHoverWnd == pSomWnd)
		{
			m_pHoverWnd->OnMouseLeave();
			m_pHoverWnd		= NULL;
		}

		PLVSUBITEMDATA pSubItemData	= (PLVSUBITEMDATA)pSomWnd->m_pWndManagerParam;
		CSimpleArray<CSomWnd*>& ChildWnds = pSubItemData->ChildWnds;

		ChildWnds.Remove(pSomWnd);
	}
	virtual void SomSetCapture(CSomWnd* pSomWnd)
	{
		if (m_pCaptureWnd)
			m_pCaptureWnd->OnCaptureChanged();

		m_pCaptureWnd	= pSomWnd;

		if (GetCapture() != m_hWnd)
		{
			::SetCapture(m_hWnd);
		}
	}
	virtual void SomReleaseCapture(CSomWnd* pSomWnd)
	{
		if (pSomWnd == m_pCaptureWnd)
		{
			::ReleaseCapture();
			m_pCaptureWnd	= NULL;
		}
	}
	virtual LRESULT SomSendMessage(UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		return ::SendMessage(GetParent(), Msg, wParam, lParam);
	}
	virtual LRESULT SomSendCommandMessage(CSomWnd* pSomWnd, WORD wNotifyCode, WORD wID)
	{
		PLVSUBITEMDATA pLvSubItemData	= (PLVSUBITEMDATA)pSomWnd->m_pWndManagerParam;
		return ::SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(wID, wNotifyCode), (LPARAM)pLvSubItemData->pLvItemData->pdwItemKey);
	}
	virtual BOOL SomPostMessage(UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		return ::PostMessage(GetParent(), Msg, wParam, lParam);
	}
	virtual void SomSetWindowPos(CSomWnd* pSomWnd, int x, int y, int cx, int cy)
	{
		BOOL bRepaint	= m_bRedrawToggle && pSomWnd->IsWindowVisible();
		RECT& rcWindow = pSomWnd->m_rcWindow;
		RECT rcTemp		= rcWindow;

		int x2	= rcWindow.left;
		int y2	= rcWindow.top;
		int cx2 = rcWindow.right - rcWindow.left;
		int cy2 = rcWindow.bottom - rcWindow.top;

		if (x != rcWindow.left || y != rcWindow.top || cx != cx2 || cy != cy2)
		{
			int offset_x		= x - x2;
			int offset_y		= y - y2;

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
			{
				SomListViewToClient(pSomWnd, &rcTemp);

				InvalidateRect(&rcTemp, FALSE);

				rcTemp.left		+= offset_x;
				rcTemp.top		+= offset_y;
				rcTemp.right	= rcTemp.left + cx;
				rcTemp.bottom	= rcTemp.top + cy;

				InvalidateRect(&rcTemp, FALSE);

				if (m_bUpdateToggle)
					UpdateWindow();
			}
		}
	}
	virtual void SomSizeWindow(CSomWnd* pSomWnd, int cx, int cy)
	{
		BOOL bRepaint	= m_bRedrawToggle && pSomWnd->IsWindowVisible();
		RECT& rcWindow	= pSomWnd->m_rcWindow;
		RECT rcTemp		= rcWindow;

		int cx2 = rcWindow.right - rcWindow.left;
		int cy2 = rcWindow.bottom - rcWindow.top;

		if (cx != cx2 || cy != cy2)
		{
			rcWindow.right	= rcWindow.left + cx;
			rcWindow.bottom	= rcWindow.top + cy;

			pSomWnd->OnSize(cx, cy);

			if (bRepaint)
			{
				SomListViewToClient(pSomWnd, &rcTemp);

				InvalidateRect(&rcTemp, FALSE);

				rcTemp.right	= rcTemp.left + cx;
				rcTemp.bottom	= rcTemp.top + cy;

				InvalidateRect(&rcTemp, FALSE);

				if (m_bUpdateToggle)
					UpdateWindow();
			}
		}
	}
	virtual void SomMoveWindow(CSomWnd* pSomWnd, int x, int y)
	{
		BOOL bRepaint	= m_bRedrawToggle && pSomWnd->IsWindowVisible();
		RECT& rcWindow	= pSomWnd->m_rcWindow;
		RECT rcTemp		= rcWindow;

		if (x != rcWindow.left || y != rcWindow.top)
		{
			int offset_x = x - rcWindow.left;
			int offset_y = y - rcWindow.top;

			OffsetRect(&rcWindow, offset_x, offset_y);

			pSomWnd->OnMove(x, y);

			if (bRepaint)
			{
				SomListViewToClient(pSomWnd, &rcTemp);

				InvalidateRect(&rcTemp, FALSE);

				OffsetRect(&rcTemp, offset_x, offset_y);

				InvalidateRect(&rcTemp, FALSE);

				if (m_bUpdateToggle)
					UpdateWindow();
			}
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

		PLVSUBITEMDATA pSubItemData	= (PLVSUBITEMDATA)pSomWnd->m_pWndManagerParam;
		int nItem = GetLvItemIndex(pSubItemData->pLvItemData);
		int nSubItem = pSubItemData->iSubItemIndex;

		if (nItem == -1)
			return;

		RECT rect;
		GetSubItemRect(nItem, nSubItem, LVIR_LABEL, &rect);
		OffsetRect(&rcPaint, rect.left, rect.top);

		if (m_bUpdateToggle)
		{
			HDC hDC = GetDC();

			IntersectClipRect(hDC, rcPaint.left, rcPaint.top, rcPaint.right, rcPaint.bottom);

			POINT pt;
			OffsetViewportOrgEx(hDC, rect.left, rect.top, &pt);

			SomDrawWnd(hDC, pSomWnd, bPaint, bEraseBackground, nItem, pSubItemData->pLvItemData->pdwItemKey, rect.right - rect.left, rect.bottom - rect.top, FALSE, lpRedrawParam);

			SetViewportOrgEx(hDC, pt.x, pt.y, NULL);

			ReleaseDC(hDC);
		}
		else
			InvalidateRect(&rcPaint, FALSE);
	}
	virtual void SomAddTooltip(CSomWnd* pSomWnd, UINT_PTR uInnerTipId, LPRECT lpRect)
	{
		PLVSUBITEMDATA pSubItemData	= (PLVSUBITEMDATA)pSomWnd->m_pWndManagerParam;
		int nItem = GetLvItemIndex(pSubItemData->pLvItemData);
		int nSubItem = pSubItemData->iSubItemIndex;

		if (nItem == -1)
			return;

		RECT rect;
		GetSubItemRect(nItem, nSubItem, LVIR_LABEL, &rect);

		UINT_PTR uToolTipId = AddToolTipId(pSomWnd, uInnerTipId, lpRect, rect.left, rect.top);
		ATLASSERT(uToolTipId != NULL);
	}
	virtual void SomDelTooltip(CSomWnd* pSomWnd)
	{
		DelToolTipId(pSomWnd);
	}
	int SomListViewToClient(CSomWnd* pSomWnd, LPRECT lpRect)
	{
		PLVSUBITEMDATA pSubItemData	= (PLVSUBITEMDATA)pSomWnd->m_pWndManagerParam;
		int nItem = GetLvItemIndex(pSubItemData->pLvItemData);
		int nSubItem = pSubItemData->iSubItemIndex;

		if (nItem == -1)
			return -1;

		RECT rect;
		GetSubItemRect(nItem, nSubItem, LVIR_LABEL, &rect);
		OffsetRect(lpRect, rect.left, rect.top);

		return nItem;
	}
	int GetLvItemIndex(PLVITEMDATA pLvItemData)
	{
		if ((PLVITEMDATA)CListViewCtrl::GetItemData(pLvItemData->iItemIndex) != pLvItemData)
		{
			int iCount = GetItemCount();
			for (int i = 0; i < iCount; i++)
			{
				// 出现一次 卸载完一款软件后崩溃在这里 修改下
				PLVITEMDATA pCurItemData = (PLVITEMDATA)CListViewCtrl::GetItemData(i);
				if (pCurItemData)
					pCurItemData->iItemIndex	= i;
			}

			ATLASSERT((PLVITEMDATA)CListViewCtrl::GetItemData(pLvItemData->iItemIndex) == pLvItemData);

			if ((PLVITEMDATA)CListViewCtrl::GetItemData(pLvItemData->iItemIndex) == pLvItemData)
				return pLvItemData->iItemIndex;
			else
				return -1;
		}
		else
			return pLvItemData->iItemIndex;
	}
	__inline PLVITEMDATA GetLvItemData(int nIndex) const
	{
		ATLASSERT(nIndex >= 0 && nIndex < GetItemCount());

		PLVITEMDATA pdata = (PLVITEMDATA)CListViewCtrl::GetItemData(nIndex);
		ATLASSERT(pdata != NULL);

		return pdata;
	}
	void ResetSomWnd()
	{
		m_bRedrawToggle	= TRUE;
		m_bUpdateToggle	= TRUE;
		m_bItemHover	= FALSE;
		m_pCaptureWnd	= NULL;
		m_pHoverWnd		= NULL;
		m_iHoverItem	= -1;
		m_iSelectItem	= -1;
	}

	typedef struct _LVCOLUMNINFO
	{
		int min_width;				// 最小的宽度
		float max_width_percent;	// 最大的宽度的百分比
		int x;
		int width;

		_LVCOLUMNINFO(int _min_width, float _max_width_percent)
			: min_width(_min_width)
			, max_width_percent(_max_width_percent)
		{
			x		= 0;
			width	= min_width;
		}

	} LVCOLUMNINFO, *PLVCOLUMNINFO;

	BOOL m_bRedrawToggle;
	BOOL m_bUpdateToggle;
	BOOL m_bItemHover;
	int m_iHoverItem;
	int m_iSelectItem;
	map<DWORD_PTR, PLVITEMDATA> m_ItemHandleMap;
	vector<LVCOLUMNINFO> m_ColumnInfo;
	CSomWnd* m_pCaptureWnd;
	CSomWnd* m_pHoverWnd;

#ifdef _DEBUG
	int m_iItemDataCount;
#endif
};
