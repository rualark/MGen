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

// MGenView.h : interface of the CMGenView class
//

#pragma once

//#include "CustomToolTipCtrl.h"

#define Y_HEADER 50
#define Y_TIMELINE 20
#define Y_FOOTER 2
#define X_FIELD 40
#define MULTITRACK_MINNOTES 12
#define SINGLETRACK_MINNOTES 36

const char diatonic[] = { 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1 };

class CMGenDoc;

class CMGenView : public CScrollView
{
protected: // create from serialization only
	CMGenView();
	DECLARE_DYNCREATE(CMGenView)

	//CCustomToolTipCtrl m_ToolTip;

// Attributes
public:
	CMGenDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CMGenView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	int mouse_voice; // Current voice
	int mouse_voice_old; // Previously drawn voice
	int mouse_step; // Current step under mouse
	int mouse_step_old; // Previously drawn mouse_step
	int nwidth; // Step width
	int nheight; // Note height
	int y_start; // Position of lowest note

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnMouseHover(UINT, CPoint);
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual void GetToolTipLabelText(POINT cursor, CString& labelText, CString& descriptionText) const;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

#ifndef _DEBUG  // debug version in MGenView.cpp
inline CMGenDoc* CMGenView::GetDocument() const
   { return reinterpret_cast<CMGenDoc*>(m_pDocument); }
#endif

