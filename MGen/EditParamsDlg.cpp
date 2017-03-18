// EditParamsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MGen.h"
#include "EditParamsDlg.h"
#include "afxdialogex.h"
//#include "brainchild.h"
#include "MainFrm.h"


// CEditParamsDlg dialog

IMPLEMENT_DYNAMIC(CEditParamsDlg, CDialog)

CEditParamsDlg::CEditParamsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_EDITPARAMS, pParent)
{

}

CEditParamsDlg::~CEditParamsDlg()
{
}

void CEditParamsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_ALGO, m_algo);
	DDX_Control(pDX, IDC_COMBO_CONFIG, m_config);
	DDX_Control(pDX, IDC_RICHEDIT_PARAMS, m_params);
}


BEGIN_MESSAGE_MAP(CEditParamsDlg, CDialog)
END_MESSAGE_MAP()


// CEditParamsDlg message handlers


BOOL CEditParamsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CMainFrame* mf = (CMainFrame*)theApp.m_pMainWnd;
	m_algo.SetWindowTextA(GAlgName[mf->GetAlgo()]);

	return TRUE;  // return TRUE unless you set the focus to a control
								// EXCEPTION: OCX Property Pages should return FALSE
}
