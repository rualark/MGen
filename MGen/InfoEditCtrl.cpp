// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// InfoEditCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "MGen.h"
#include "InfoEditCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CInfoEditCtrl

IMPLEMENT_DYNAMIC(CInfoEditCtrl, CRichEditCtrl)

CInfoEditCtrl::CInfoEditCtrl()
{

}

CInfoEditCtrl::~CInfoEditCtrl()
{
}


BEGIN_MESSAGE_MAP(CInfoEditCtrl, CRichEditCtrl)
	ON_WM_GETDLGCODE()
END_MESSAGE_MAP()



// CInfoEditCtrl message handlers




UINT CInfoEditCtrl::OnGetDlgCode()
{
	return CRichEditCtrl::OnGetDlgCode() & (~DLGC_HASSETSEL); // clear the bit 
	//return CRichEditCtrl::OnGetDlgCode();
}

void CInfoEditCtrl::AddText(CString str, COLORREF color, DWORD dwEffects)
{
	// CFE_BOLD | CFE_ITALIC | CFE_STRIKEOUT | CFE_UNDERLINE 
	// CFE_DISABLED CFE_AUTOCOLOR
	//int nOldLines = 0;
	long nInsertionPoint;
	CHARFORMAT cf;

	// Save number of lines before insertion of new text
	//nOldLines = GetLineCount();

	// Initialize character format structure
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_COLOR | CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE | CFM_STRIKEOUT;
	cf.dwEffects = dwEffects;	// To disable CFE_AUTOCOLOR
	cf.crTextColor = color;

	// Set insertion point to end of text
	//nInsertionPoint = GetWindowTextLength();
	/*if (nInsertionPoint > 800)
	{
	// Delete first half of text to avoid running into the 64k limit
	SetSel(0, nInsertionPoint / 2);
	ReplaceSel("");
	UpdateData(FALSE);
	}*/
	nInsertionPoint = -1;
	SetSel(nInsertionPoint, -1);

	//  Set the character format
	SetSelectionCharFormat(cf);

	// Replace selection. Because we have nothing selected, this will simply insert
	// the string at the current caret position.
	ReplaceSel(str);

	// Get new line count
	//nNewLines = GetLineCount();

	// Scroll by the number of lines just inserted
	//nScroll = nNewLines - nOldLines;
	//LineScroll(nScroll);
}

