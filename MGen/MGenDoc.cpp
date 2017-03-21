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

// MGenDoc.cpp : implementation of the CMGenDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "MGen.h"
#endif

#include "MGenDoc.h"
#include "MainFrm.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMGenDoc

IMPLEMENT_DYNCREATE(CMGenDoc, CDocument)

BEGIN_MESSAGE_MAP(CMGenDoc, CDocument)
END_MESSAGE_MAP()


// CMGenDoc construction/destruction

CMGenDoc::CMGenDoc()
{
	// TODO: add one-time construction code here

}

CMGenDoc::~CMGenDoc()
{
}

// CMGenDoc serialization

void CMGenDoc::Serialize(CArchive& ar)
{
	CMainFrame* mf = (CMainFrame*)theApp.m_pMainWnd;
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		//ar.Close();
		//CString fname = ar.m_strFileName;
		//mf->WriteLog(1, fname);
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CMGenDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CMGenDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CMGenDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CMGenDoc diagnostics

#ifdef _DEBUG
void CMGenDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMGenDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMGenDoc commands

BOOL CMGenDoc::DoSave(LPCTSTR lpszPathName, BOOL bReplace)
{
	CMainFrame* mf = (CMainFrame*)theApp.m_pMainWnd;
	TCHAR buffer[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, buffer);
	string::size_type pos = string(buffer).find_last_of("\\/");
	CString path_old = string(buffer).c_str();
	path_old += "\\saves";

	// szFilters is a text string that includes two file name filters:
	// "*.my" for "MyType Files" and "*.*' for "All Files."
	TCHAR szFilters[] = _T("MGen result files (*.mgr)|*.mgr|");

	// Create an Open dialog; the default file name extension is ".my".
	CFileDialog fileDlg(FALSE, _T("pl"), GetPathName(),
		OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT, szFilters, mf, 0, false);
	fileDlg.m_ofn.lpstrInitialDir = path_old;

	// Display the file dialog. When user clicks OK, fileDlg.DoModal() 
	// returns IDOK.
	if (fileDlg.DoModal() == IDOK)
	{
		// Get name
		CString path = fileDlg.GetPathName();
		OnSaveDocument(path);
		SetPathName(path);
	}

	return TRUE;
}

BOOL CMGenDoc::OnNewDocument()
{
	//CMainFrame* mf = (CMainFrame*)theApp.m_pMainWnd;
	//if (mf != nullptr) {
	//	if (mf->NewDocument() == false) return FALSE;
	//}

	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}

BOOL CMGenDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	CMainFrame* mf = (CMainFrame*)theApp.m_pMainWnd;
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	// TODO:  Add your specialized creation code here
	CString fname = lpszPathName;
	//mf->WriteLog(1, fname);
	mf->LoadResults(fname);

	return TRUE;
}


BOOL CMGenDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO: Add your specialized code here and/or call the base class
	//return false;

	return CDocument::OnSaveDocument(lpszPathName);
}
