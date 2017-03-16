// InfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MGen.h"
#include "InfoDlg.h"
#include "afxdialogex.h"


// CInfoDlg dialog

IMPLEMENT_DYNAMIC(CInfoDlg, CDialog)

CInfoDlg::CInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_INFO, pParent)
{

}

CInfoDlg::~CInfoDlg()
{
}

void CInfoDlg::AddText(CString str, COLORREF color, DWORD dwEffects)
{
	// CFE_BOLD | CFE_ITALIC | CFE_STRIKEOUT | CFE_UNDERLINE 
	// CFE_DISABLED CFE_AUTOCOLOR
	int nOldLines = 0, nNewLines = 0, nScroll = 0;
	long nInsertionPoint = 0;
	CHARFORMAT cf;

	// Save number of lines before insertion of new text
	nOldLines = m_info.GetLineCount();

	// Initialize character format structure
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_COLOR | CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE | CFM_STRIKEOUT;
	cf.dwEffects = dwEffects;	// To disable CFE_AUTOCOLOR
	cf.crTextColor = color;

	// Set insertion point to end of text
	nInsertionPoint = m_info.GetWindowTextLength();
	/*if (nInsertionPoint > 800)
	{
	// Delete first half of text to avoid running into the 64k limit
	m_info.SetSel(0, nInsertionPoint / 2);
	m_info.ReplaceSel("");
	UpdateData(FALSE);
	}*/
	nInsertionPoint = -1;
	m_info.SetSel(nInsertionPoint, -1);

	//  Set the character format
	m_info.SetSelectionCharFormat(cf);

	// Replace selection. Because we have nothing selected, this will simply insert
	// the string at the current caret position.
	m_info.ReplaceSel(str);

	// Get new line count
	//nNewLines = m_info.GetLineCount();

	// Scroll by the number of lines just inserted
	//nScroll = nNewLines - nOldLines;
	//m_info.LineScroll(nScroll);
}

void CInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICHEDIT21, m_info);
}


BEGIN_MESSAGE_MAP(CInfoDlg, CDialog)
END_MESSAGE_MAP()


// CInfoDlg message handlers


BOOL CInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText("Dialog: some text");
	AddText("Some text text text\r\n", RGB(250, 100, 0), CFE_BOLD | CFE_ITALIC | CFE_STRIKEOUT | CFE_UNDERLINE);
	AddText("Other text text\r\n", RGB(10, 170, 0), CFE_ITALIC);
	AddText("Some text text text\r\n", RGB(250, 100, 0), CFE_BOLD | CFE_ITALIC | CFE_STRIKEOUT | CFE_UNDERLINE);
	AddText("Other text text\r\n", RGB(10, 170, 0), CFE_ITALIC);
	AddText("Some text text text\r\n", RGB(250, 100, 0), CFE_BOLD | CFE_ITALIC | CFE_STRIKEOUT | CFE_UNDERLINE);
	AddText("Other text text\r\n", RGB(10, 170, 0), CFE_ITALIC);
	AddText("Some text text text\r\n", RGB(250, 100, 0), CFE_BOLD | CFE_ITALIC | CFE_STRIKEOUT | CFE_UNDERLINE);
	AddText("Other text text\r\n", RGB(10, 170, 0), CFE_ITALIC);
	AddText("Some text text text\r\n", RGB(250, 100, 0), CFE_BOLD | CFE_ITALIC | CFE_STRIKEOUT | CFE_UNDERLINE);
	AddText("Other text text\r\n", RGB(10, 170, 0), CFE_ITALIC);
	AddText("Some text text text\r\n", RGB(250, 100, 0), CFE_BOLD | CFE_ITALIC | CFE_STRIKEOUT | CFE_UNDERLINE);
	AddText("Other text text\r\n", RGB(10, 170, 0), CFE_ITALIC);
	AddText("Some text text text\r\n", RGB(250, 100, 0), CFE_BOLD | CFE_ITALIC | CFE_STRIKEOUT | CFE_UNDERLINE);
	AddText("Other text text\r\n", RGB(10, 170, 0), CFE_ITALIC);
	AddText("Some text text text\r\n", RGB(250, 100, 0), CFE_BOLD | CFE_ITALIC | CFE_STRIKEOUT | CFE_UNDERLINE);
	AddText("Other text text\r\n", RGB(10, 170, 0), CFE_ITALIC);
	AddText("Some text text text\r\n", RGB(250, 100, 0), CFE_BOLD | CFE_ITALIC | CFE_STRIKEOUT | CFE_UNDERLINE);
	AddText("Other text text\r\n", RGB(10, 170, 0), CFE_ITALIC);
	AddText("Some text text text\r\n", RGB(250, 100, 0), CFE_BOLD | CFE_ITALIC | CFE_STRIKEOUT | CFE_UNDERLINE);
	AddText("Other text text\r\n", RGB(10, 170, 0), CFE_ITALIC);
	AddText("Some text text text\r\n", RGB(250, 100, 0), CFE_BOLD | CFE_ITALIC | CFE_STRIKEOUT | CFE_UNDERLINE);
	AddText("Other text text\r\n", RGB(10, 170, 0), CFE_ITALIC);
	AddText("Some text text text\r\n", RGB(250, 100, 0), CFE_BOLD | CFE_ITALIC | CFE_STRIKEOUT | CFE_UNDERLINE);
	AddText("Other text text\r\n", RGB(10, 170, 0), CFE_ITALIC);
	AddText("Some text text text\r\n", RGB(250, 100, 0), CFE_BOLD | CFE_ITALIC | CFE_STRIKEOUT | CFE_UNDERLINE);
	AddText("Other text text\r\n", RGB(10, 170, 0), CFE_ITALIC);
	AddText("Some text text text\r\n", RGB(250, 100, 0), CFE_BOLD | CFE_ITALIC | CFE_STRIKEOUT | CFE_UNDERLINE);
	AddText("Other text text\r\n", RGB(10, 170, 0), CFE_ITALIC);
	AddText("Some text text text\r\n", RGB(250, 100, 0), CFE_BOLD | CFE_ITALIC | CFE_STRIKEOUT | CFE_UNDERLINE);
	AddText("Other text text\r\n", RGB(10, 170, 0), CFE_ITALIC);
	AddText("Some text text text\r\n", RGB(250, 100, 0), CFE_BOLD | CFE_ITALIC | CFE_STRIKEOUT | CFE_UNDERLINE);
	AddText("Other text text\r\n", RGB(10, 170, 0), CFE_ITALIC);
	AddText("Some text text text\r\n", RGB(250, 100, 0), CFE_BOLD | CFE_ITALIC | CFE_STRIKEOUT | CFE_UNDERLINE);
	AddText("Other text text\r\n", RGB(10, 170, 0), CFE_ITALIC);
	AddText("Some text text text\r\n", RGB(250, 100, 0), CFE_BOLD | CFE_ITALIC | CFE_STRIKEOUT | CFE_UNDERLINE);
	AddText("Other text text\r\n", RGB(10, 170, 0), CFE_ITALIC);
	AddText("Some text text text\r\n", RGB(250, 100, 0), CFE_BOLD | CFE_ITALIC | CFE_STRIKEOUT | CFE_UNDERLINE);
	AddText("Other text text\r\n", RGB(10, 170, 0), CFE_ITALIC);
	AddText("Some text text text\r\n", RGB(250, 100, 0), CFE_BOLD | CFE_ITALIC | CFE_STRIKEOUT | CFE_UNDERLINE);
	AddText("Other text text\r\n", RGB(10, 170, 0), CFE_ITALIC);
	AddText("Some text text text\r\n", RGB(250, 100, 0), CFE_BOLD | CFE_ITALIC | CFE_STRIKEOUT | CFE_UNDERLINE);
	AddText("Other text text\r\n", RGB(10, 170, 0), CFE_ITALIC);
	AddText("Some text text text\r\n", RGB(250, 100, 0), CFE_BOLD | CFE_ITALIC | CFE_STRIKEOUT | CFE_UNDERLINE);
	AddText("Other text text\r\n", RGB(10, 170, 0), CFE_ITALIC);
	AddText("Some text text text\r\n", RGB(250, 100, 0), CFE_BOLD | CFE_ITALIC | CFE_STRIKEOUT | CFE_UNDERLINE);
	AddText("Other text text\r\n", RGB(10, 170, 0), CFE_ITALIC);
	AddText("Some text text text\r\n", RGB(250, 100, 0), CFE_BOLD | CFE_ITALIC | CFE_STRIKEOUT | CFE_UNDERLINE);
	AddText("Other text text\r\n", RGB(10, 170, 0), CFE_ITALIC);
	AddText("Some text text text\r\n", RGB(250, 100, 0), CFE_BOLD | CFE_ITALIC | CFE_STRIKEOUT | CFE_UNDERLINE);
	AddText("Other text text\r\n", RGB(10, 170, 0), CFE_ITALIC);
	m_info.SetSel(0, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
								// EXCEPTION: OCX Property Pages should return FALSE
}
