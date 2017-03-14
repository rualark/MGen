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

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMGenView

IMPLEMENT_DYNCREATE(CMGenView, CView)

BEGIN_MESSAGE_MAP(CMGenView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMGenView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
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

	return CView::PreCreateWindow(cs);
}

// CMGenView drawing

void CMGenView::OnDraw(CDC* pDC)
{
	CMGenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	Graphics graphics(pDC->m_hDC);

	Pen blue(Color(255, 0, 0, 255));
	graphics.DrawLine(&blue, 10, 10, 50, 50);

	/*
	hdc = BeginPaint(hWnd, &ps);
	Graphics gr(hdc);
	Font font(&FontFamily(L"Arial"), 12);
	LinearGradientBrush brush(Rect(0, 0, 100, 100), Color::Red, Color::Yellow, LinearGradientModeHorizontal);
	Status st = gr.DrawString(L"Look at this text!", -1, &font, PointF(0, 0), &brush);
	assert(st == Ok);
	EndPaint(hWnd, &ps);
	*/
	CPaintDC dc(this); // Creates a device context for the client area, which
					   // also erases the area to be drawn.

	CPen MyPen, *pOldPen;
	CBrush MyBrush, *pOldBrush; 
	// A red pen, one pixel wide
	MyPen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));

	// Selecting an object returns the old one
	// we need to catch and return it to avoid memory leaks
	pOldPen = dc.SelectObject(&MyPen);

	// A Blue brush                                         
	MyBrush.CreateSolidBrush(RGB(0, 0, 255));
	pOldBrush = dc.SelectObject(&MyBrush);

	// Finally, we have our device context set up with our pen and brush and can
	// use them to draw. <BR>
	//Draws a rectangle that is red on the outside, filled in blue 
	dc.Rectangle(0, 0, 200, 200);
	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);
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
	CView::AssertValid();
}

void CMGenView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMGenDoc* CMGenView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMGenDoc)));
	return (CMGenDoc*)m_pDocument;
}
#endif //_DEBUG


// CMGenView message handlers
