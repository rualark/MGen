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
END_MESSAGE_MAP()

// CMGenView construction/destruction

CMGenView::CMGenView()
{
	// TODO: add construction code here

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

	//CClientDC aDC(this);//получить контекст устройства
	//OnPrepareDC(&aDC);//уточнить начальную точку в логических координатах
	//aDC.DPtoLP(&point);//перевод из клиентских координат в логические

	CRect ClipBox;
	pDC->GetClipBox(&ClipBox);
	CSize Size = GetTotalSize();

	CMainFrame *pMainWnd = (CMainFrame *)AfxGetMainWnd();
	CGenTemplate *pGen = pMainWnd->pGen;
	if (pGen != 0) {
		for (int i = 0; i < pGen->t_generated; i++) {
			CRect rc(i*3, pGen->note[i][0]*5, i*3+1, pGen->note[i][0] * 5+3);
			pDC->FillSolidRect(rc, RGB(255, 0, 0));
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

void CMGenView::OnMouseHover(UINT, CPoint)
{
	CMainFrame* mf = (CMainFrame*)theApp.m_pMainWnd;
	mf->m_wndStatusBar.GetElement(0)->SetText("Some text");
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
	CSize DocSize(6000, 0);
	//Установить режим отображения и размер документа
	SetScrollSizes(MM_TEXT, DocSize, CSize(500, 500), CSize(50, 50));
}


void CMGenView::OnMouseMove(UINT nFlags, CPoint point)
{
	CMainFrame* mf = (CMainFrame*)theApp.m_pMainWnd;
	//mf->m_wndStatusBar.GetElement(1)->SetText("Some text");
	mf->WriteDebug("Mouse move");

	CScrollView::OnMouseMove(nFlags, point);
}
