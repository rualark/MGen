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

// MGenView.cpp : implementation of the CMGenView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "MGen.h"
#endif

#include "MGenDoc.h"
#include "MGenView.h"
#include "MainFrm.h"
#include "InfoDlg.h"
#include "MemDC2.cpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMGenView

IMPLEMENT_DYNCREATE(CMGenView, CScrollView)

BEGIN_MESSAGE_MAP(CMGenView, CScrollView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMGenView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CMGenView construction/destruction

CMGenView::CMGenView()
{
	// TODO: add construction code here
	//m_ToolTip.Create(this);
	//m_ToolTip.Activate(TRUE);
}

CMGenView::~CMGenView()
{
}

BOOL CMGenView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CScrollView::PreCreateWindow(cs);
}

// CMGenView drawing

void CMGenView::OnDraw(CDC* pDC)
{
	CMGenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// This is invalidated rect
	CRect ClipBox;
	pDC->GetClipBox(&ClipBox);
	// Total client rect
	CRect ClientRect;
	GetClientRect(&ClientRect);
	// Total scrollable size
	CSize Size = GetTotalSize();

	CMemDC2 dc(pDC);
	dc->FillRect(ClipBox, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));

	Graphics g(dc->m_hDC);
	//CClientDC aDC(this);//получить контекст устройства
	//OnPrepareDC(&aDC);//уточнить начальную точку в логических координатах
	//aDC.DPtoLP(&point);//перевод из клиентских координат в логические

	CMainFrame *mf = (CMainFrame *)AfxGetMainWnd();
	CGenTemplate *pGen = mf->pGen;
	if (pGen != 0) {
		int nwidth = 4 * mf->zoom_x / 100;
		if (mf->view_single_track) {
			int ng_min = pGen->ng_min;
			int ng_max = pGen->ng_max;
			int ncount = ng_max - ng_min;
			int ncount2 = ncount;
			int ng_min2 = ng_min;
			int ng_max2 = ng_max;
			if (ncount2 < SINGLETRACK_MINNOTES) {
				ncount2 = SINGLETRACK_MINNOTES;
				ng_min2 = ng_min - (ncount2 - ncount) / 2;
				ng_max2 = ng_max + (ncount2 - ncount) / 2;
			}
			int nheight = (ClientRect.bottom - Y_HEADER - Y_FOOTER) / ncount2;
			// Select steps to show
			int step1 = max(0, (ClipBox.left - X_FIELD) / nwidth);
			int step2 = min(pGen->t_generated - 1, min((ClipBox.right - X_FIELD) / nwidth, 32000/nwidth));
			for (int i = step1; i < step2; i++) {
				/*
				CRect rc(
					X_FIELD + i * nwidth, 
					ClientRect.bottom - Y_FOOTER - (pGen->note[i][0]-ng_min2) * nheight, 
					X_FIELD + (i+1) * nwidth - 1,
					ClientRect.bottom - Y_FOOTER - (pGen->note[i][0] - ng_min2 + 1) * nheight - 1);
				//dc.FillSolidRect(rc, RGB(255, 0, 0));
				*/
				SolidBrush brush(Color(80 /*A*/, 0 /*R*/, 0 /*G*/, 255 /*B*/));
				g.FillRectangle(&brush, 
					X_FIELD + i * nwidth, 
					ClientRect.bottom - Y_FOOTER - (pGen->note[i][0] - ng_min2) * nheight, 
					nwidth-1, 
					nheight-1);
			}
		}
		if (min(32000, nwidth*pGen->t_generated) > ClientRect.right) {
			CSize DocSize(min(32000, nwidth*pGen->t_generated) + 10, 0);
			SetScrollSizes(MM_TEXT, DocSize, CSize(500, 500), CSize(50, 50));
		}
	}

	//CRect rc;
	//GetClientRect(&rc);
	//rc.SetRect(0, 0, 200000, 1000);
	//pDC->FillSolidRect(ClipBox, RGB(255, 0, 0));
	//ClipBox.InflateRect(-1, -1);
	//pDC->FillSolidRect(ClipBox, RGB(0, 0, 255));

	/*
	Graphics graphics(pDC->m_hDC);
	Pen blue(Color(255, 0, 0, 255));
	graphics.DrawLine(&blue, 10, 10, 50, 50);
	*/
}


// CMGenView printing


void CMGenView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CMGenView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMGenView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CMGenView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CMGenView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMGenView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CMGenView diagnostics

#ifdef _DEBUG
void CMGenView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CMGenView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CMGenDoc* CMGenView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMGenDoc)));
	return (CMGenDoc*)m_pDocument;
}
#endif //_DEBUG


// CMGenView message handlers


void CMGenView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	//Определить размер документа
	CSize DocSize(0, 0);
	//Установить режим отображения и размер документа
	SetScrollSizes(MM_TEXT, DocSize, CSize(500, 500), CSize(50, 50));
	
	// Tooltip
	//CMFCToolTipInfo params;
	//params.m_bVislManagerTheme = TRUE;
	//m_ToolTip.SetParams(&params);
	//m_ToolTip.AddTool(this, _T("."));
	//m_ToolTip.SetDelayTime(100);					// 0.5 secondS before tooltips pop up in view
	//m_ToolTip.SetDelayTime(TTDT_RESHOW, 500);		// 0.5 seconds before tooltip after moving
	//m_ToolTip.SetDelayTime(TTDT_AUTOPOP, INT_MAX);	// leave tooltip visible
}


void CMGenView::OnMouseMove(UINT nFlags, CPoint point)
{
	//CMainFrame* mf = (CMainFrame*)theApp.m_pMainWnd;
	//mf->m_wndStatusBar.GetElement(0)->SetText("Some text");
	//mf->m_wndStatusBar.Invalidate(1);
	//mf->WriteDebug("Mouse move");

	CScrollView::OnMouseMove(nFlags, point);
}

void CMGenView::OnMouseHover(UINT nFlags, CPoint point)
{
	//CMainFrame* mf = (CMainFrame*)theApp.m_pMainWnd;
	//mf->m_wndStatusBar.GetElement(0)->SetText("Some text");
	CScrollView::OnMouseHover(nFlags, point);
}

void CMGenView::GetToolTipLabelText(POINT cursor, CString & labelText, CString & descriptionText) const
{
	/*
	ScreenToClient(&cursor);
	CPoint pos(cursor);
	pos += GetScrollPosition();

	labelText = "Label";
	descriptionText = "Desc";
	*/
}


BOOL CMGenView::PreTranslateMessage(MSG* pMsg)
{
	/*
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_MOUSEMOVE:
		//m_ToolTip.Activate(TRUE); 
		m_ToolTip.RelayEvent(pMsg);
		break;
	}
	*/

	return CScrollView::PreTranslateMessage(pMsg);
}

void CMGenView::OnLButtonUp(UINT nFlags, CPoint point)
{
	CMainFrame* mf = (CMainFrame*)theApp.m_pMainWnd;
	CInfoDlg dlg;
	dlg.pGen = mf->pGen;
	dlg.DoModal();

	CScrollView::OnLButtonUp(nFlags, point);
}


BOOL CMGenView::OnEraseBkgnd(CDC* pDC)
{
	return false;

	//return CScrollView::OnEraseBkgnd(pDC);
}
