// MsgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MGen.h"
#include "MsgDlg.h"
#include "afxdialogex.h"


// CMsgDlg dialog

IMPLEMENT_DYNAMIC(CMsgDlg, CDialog)

CMsgDlg::CMsgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_MSG, pParent)
	
{

}

CMsgDlg::~CMsgDlg()
{
}

void CMsgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICHEDIT_MSG, m_edit);
}

BEGIN_MESSAGE_MAP(CMsgDlg, CDialog)
END_MESSAGE_MAP()

// CMsgDlg message handlers


BOOL CMsgDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_edit.AddText(m_text, RGB(0, 0, 100), 0);
	m_edit.SetSel(0, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
								// EXCEPTION: OCX Property Pages should return FALSE
}
