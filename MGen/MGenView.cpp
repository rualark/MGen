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
			// Get generator window
			int ng_min = pGen->ng_min;
			int ng_max = pGen->ng_max;
			int ncount = ng_max - ng_min + 1;
			int ng_min2 = ng_min;
			int ng_max2 = ng_max;
			int ncount2 = ng_max2 - ng_min2 + 1;
			// Load last window
			if (mf->ng_max != 0) {
				ng_min2 = mf->ng_min;
				ng_max2 = mf->ng_max;
				if ((ng_min < ng_min2) || (ng_max > ng_max2)) {
					ng_min2 = ng_min;
					ng_max2 = ng_max;
				}
				ncount2 = ng_max2 - ng_min2 + 1;
			}
			// Check if window is too small
			if (ncount2 < SINGLETRACK_MINNOTES) {
				ncount2 = SINGLETRACK_MINNOTES;
				ng_min2 = ng_min - (ncount2 - ncount) / 2;
				ng_max2 = ng_max + (ncount2 - ncount) / 2;
			}
			// Save window
			mf->ng_min = ng_min2;
			mf->ng_max = ng_max2;
			// Count height
			int nheight = (ClientRect.bottom - Y_HEADER - Y_FOOTER) / ncount2;
			// Select steps to show
			int step1 = max(0, (ClipBox.left - X_FIELD) / nwidth);
			int step2_2 = min((ClipBox.right - X_FIELD) / nwidth, 32000 / nwidth);
			int step2 = min(pGen->t_generated - 1, step2_2);
			// Show grid
			SolidBrush brush_gray(Color(127 /*A*/, 240 /*R*/, 240 /*G*/, 240 /*B*/));
			Pen pen_dgray(Color(127 /*A*/, 200 /*R*/, 200 /*G*/, 200 /*B*/), 1);
			Pen pen_ddgray(Color(127 /*A*/, 100 /*R*/, 100 /*G*/, 100 /*B*/), 1);
			for (int i = ng_min2; i <= ng_max2; i++) {
				if (diatonic[i % 12] == 0) {
					g.FillRectangle(&brush_gray, max(X_FIELD, ClipBox.left),
						ClientRect.bottom - Y_FOOTER - (i - ng_min2) * nheight,
						ClipBox.right-ClipBox.left, nheight);
				}
				if (i % 12 == 4) g.DrawLine(&pen_dgray, max(X_FIELD, ClipBox.left),
					ClientRect.bottom - Y_FOOTER - (i - ng_min2) * nheight, ClipBox.right,
					ClientRect.bottom - Y_FOOTER - (i - ng_min2) * nheight);
				if (i % 12 == 11) g.DrawLine(&pen_ddgray, max(X_FIELD, ClipBox.left),
					ClientRect.bottom - Y_FOOTER - (i - ng_min2) * nheight, ClipBox.right,
					ClientRect.bottom - Y_FOOTER - (i - ng_min2) * nheight);
			}
			// Draw vertical lines
			for (int i = step1; i < step2_2; i++) {
				if (i % 8 == 0) g.DrawLine(&pen_ddgray, X_FIELD + i * nwidth, ClientRect.bottom - Y_FOOTER,
					X_FIELD + i * nwidth, ClientRect.top + Y_HEADER);
			}
			// Show notes
			SolidBrush brush_v(Color(80 /*A*/, 0 /*R*/, 0 /*G*/, 255 /*B*/));
			for (int i = step1; i < step2; i++) {
				g.FillRectangle(&brush_v, X_FIELD + i * nwidth, 
					ClientRect.bottom - Y_FOOTER - (pGen->note[i][0] - ng_min2) * nheight, 
					nwidth-1, nheight-1);
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
	/*
	CRect rc(
	X_FIELD + i * nwidth,
	ClientRect.bottom - Y_FOOTER - (pGen->note[i][0]-ng_min2) * nheight,
	X_FIELD + (i+1) * nwidth - 1,
	ClientRect.bottom - Y_FOOTER - (pGen->note[i][0] - ng_min2 + 1) * nheight - 1);
	//dc.FillSolidRect(rc, RGB(255, 0, 0));
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
