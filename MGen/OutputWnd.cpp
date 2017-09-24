// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

#include "stdafx.h"
 
#include "OutputWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "MGenView.h"
#include "MsgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputBar

COutputWnd::COutputWnd()
{
}

COutputWnd::~COutputWnd()
{
}

BEGIN_MESSAGE_MAP(COutputWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create tabs window:
	if (!m_wndTabs.Create(CMFCTabCtrl::STYLE_FLAT, rectDummy, this, 1))
	{
		TRACE0("Failed to create output tab window\n");
		return -1;      // fail to create
	}

	// Create output panes:
	const DWORD dwStyle = LBS_NOINTEGRALHEIGHT | LBS_NOTIFY | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;

	if (!m_wndOutputWarn.Create(dwStyle, rectDummy, &m_wndTabs, 2) ||
		!m_wndOutputDebug.Create(dwStyle, rectDummy, &m_wndTabs, 3) ||
		!m_wndOutputAlgo.Create(dwStyle, rectDummy, &m_wndTabs, 4) ||
		!m_wndOutputMidi.Create(dwStyle, rectDummy, &m_wndTabs, 5) ||
		!m_wndOutputConfirm.Create(dwStyle, rectDummy, &m_wndTabs, 6) ||
		!m_wndOutputEmu.Create(dwStyle, rectDummy, &m_wndTabs, 7) ||
		!m_wndOutputCorack.Create(dwStyle, rectDummy, &m_wndTabs, 8) ||
		!m_wndOutputPerf.Create(dwStyle, rectDummy, &m_wndTabs, 9))
	{
		TRACE0("Failed to create output windows\n");
		return -1;      // fail to create
	}

	UpdateFonts();

	m_wndTabs.AddTab(&m_wndOutputWarn,  " Warnings     ", (UINT)0);
	m_wndTabs.AddTab(&m_wndOutputDebug, " Debug     ", (UINT)1);
	m_wndTabs.AddTab(&m_wndOutputPerf, " Performance     ", (UINT)2);
	m_wndTabs.AddTab(&m_wndOutputAlgo, " Algorithm     ", (UINT)3);
	m_wndTabs.AddTab(&m_wndOutputMidi, " MIDI out     ", (UINT)4);
	m_wndTabs.AddTab(&m_wndOutputConfirm, " Rule confirmations     ", (UINT)5);
	m_wndTabs.AddTab(&m_wndOutputEmu, " SAS emulator     ", (UINT)6);
	m_wndTabs.AddTab(&m_wndOutputCorack, " Correction acknowledge     ", (UINT)7);

	return 0;
}

void COutputWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// Tab control should cover the whole client area:
	m_wndTabs.SetWindowPos (NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void COutputWnd::AdjustHorzScroll(CListBox& wndListBox)
{
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(&afxGlobalData.fontRegular);

	int cxExtentMax = 0;

	for (int i = 0; i < wndListBox.GetCount(); i ++)
	{
		CString strItem;
		wndListBox.GetText(i, strItem);

		cxExtentMax = max(cxExtentMax, (int)dc.GetTextExtent(strItem).cx);
	}

	wndListBox.SetHorizontalExtent(cxExtentMax);
	dc.SelectObject(pOldFont);
}

void COutputWnd::UpdateFonts()
{
	m_wndOutputWarn.SetFont(&afxGlobalData.fontRegular);
	m_wndOutputDebug.SetFont(&afxGlobalData.fontRegular);
	m_wndOutputPerf.SetFont(&afxGlobalData.fontRegular);
	m_wndOutputAlgo.SetFont(&afxGlobalData.fontRegular);
	m_wndOutputMidi.SetFont(&afxGlobalData.fontRegular);
	m_wndOutputConfirm.SetFont(&afxGlobalData.fontRegular);
	m_wndOutputEmu.SetFont(&afxGlobalData.fontRegular);
	m_wndOutputCorack.SetFont(&afxGlobalData.fontRegular);
}

/////////////////////////////////////////////////////////////////////////////
// COutputList1

COutputList::COutputList()
{
}

COutputList::~COutputList()
{
}

BEGIN_MESSAGE_MAP(COutputList, CListBox)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_COMMAND(ID_VIEW_OUTPUTWND, OnViewOutput)
	ON_WM_WINDOWPOSCHANGING()
	ON_COMMAND(ID_POPUP_SHOW, &COutputList::OnPopupShow)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SAVELINE, &COutputList::OnUpdateEditSaveline)
	ON_COMMAND(ID_EDIT_SAVELINE, &COutputList::OnEditSaveline)
	ON_CONTROL_REFLECT(LBN_DBLCLK, &COutputList::OnLbnDblclk)
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_CLEARALL, &COutputList::OnClearall)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// COutputList message handlers

void COutputList::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CMenu menu;
	menu.LoadMenu(IDR_OUTPUT_POPUP);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}

	SetFocus();
}

void COutputList::OnEditCopy()
{
}

void COutputList::OnEditClear()
{
	MessageBox(_T("Clear output"));
}

void COutputList::OnViewOutput()
{
	CDockablePane* pParentBar = DYNAMIC_DOWNCAST(CDockablePane, GetOwner());
	CFrameWndEx* pMainFrame = DYNAMIC_DOWNCAST(CFrameWndEx, GetTopLevelFrame());

	if (pMainFrame != NULL && pParentBar != NULL)
	{
		pMainFrame->SetFocus();
		pMainFrame->ShowPane(pParentBar, FALSE, FALSE, FALSE);
		pMainFrame->RecalcLayout();

	}
}

void COutputList::OnPopupShow()
{
	CString st;
	GetText(GetCurSel(), st);
	CMsgDlg dlg;
	dlg.m_text = st;
	dlg.DoModal();
}


void COutputList::OnUpdateEditSaveline(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}


void COutputList::OnEditSaveline()
{
	CString st, st2;
	for (int i = 0; i < GetCount(); ++i) {
		GetText(i, st);
		st2 += st + "\n";
	}
	CMsgDlg dlg;
	dlg.m_text = st2;
	dlg.DoModal();
}


void COutputList::OnLbnDblclk()
{
	OnPopupShow();
}


void COutputWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CFrameWndEx* pMainFrame = DYNAMIC_DOWNCAST(CFrameWndEx, GetTopLevelFrame());
	(static_cast<CMGenView*>(pMainFrame->GetActiveView()))->OnKeyDown(nChar, nRepCnt, nFlags);

	CDockablePane::OnKeyDown(nChar, nRepCnt, nFlags);
}


void COutputList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar != VK_NEXT && nChar != VK_PRIOR && nChar != VK_RIGHT && nChar != VK_LEFT && nChar != VK_END && nChar != VK_HOME) {
		CFrameWndEx* pMainFrame = DYNAMIC_DOWNCAST(CFrameWndEx, GetTopLevelFrame());
		(static_cast<CMGenView*>(pMainFrame->GetActiveView()))->OnKeyDown(nChar, nRepCnt, nFlags);
	}

	CListBox::OnKeyDown(nChar, nRepCnt, nFlags);
}


void COutputList::OnClearall()
{
	ResetContent();
}
