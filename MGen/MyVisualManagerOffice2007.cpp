// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// MyVisualManagerOffice2007.cpp : implementation file
//

#include "stdafx.h"
#include "MGen.h"
#include "MyVisualManagerOffice2007.h"

IMPLEMENT_DYNCREATE(CMyVisualManagerOffice2007, CMFCVisualManagerOffice2007);

// CMyVisualManagerOffice2007

CMyVisualManagerOffice2007::CMyVisualManagerOffice2007()
{
}

CMyVisualManagerOffice2007::~CMyVisualManagerOffice2007()
{
}

BOOL CMyVisualManagerOffice2007::DrawTextOnGlass(CDC * pDC, CString strText, CRect rect, DWORD dwFlags, int nGlowSize, COLORREF clrText) //-V813
{
	return CMFCVisualManagerOffice2007::DrawTextOnGlass(pDC, strText, rect, dwFlags, nGlowSize, RGB(0, 0, 0));
}

// CMyVisualManagerOffice2007 member functions
