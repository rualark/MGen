// AlgoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MGen.h"
#include "AlgoDlg.h"
#include "afxdialogex.h"
#include "MainFrm.h"

// CAlgoDlg dialog

IMPLEMENT_DYNAMIC(CAlgoDlg, CDialog)

CAlgoDlg::CAlgoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_ALGO, pParent)
{

}

CAlgoDlg::~CAlgoDlg()
{
}

void CAlgoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_ALGO, m_tree);
	DDX_Control(pDX, IDC_EDIT_COMMENT, m_comment);
	DDX_Control(pDX, IDC_STATIC_ALGO, m_algo);
	DDX_Control(pDX, IDC_STATIC_CONFIG, m_config);
	DDX_Control(pDX, IDC_RICHEDIT_CONFIG, m_text);
}


BEGIN_MESSAGE_MAP(CAlgoDlg, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_ALGO, &CAlgoDlg::OnTvnSelchangedTreeAlgo)
	ON_BN_CLICKED(IDC_BUTTON_SAVEAS, &CAlgoDlg::OnBnClickedButtonSaveas)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, &CAlgoDlg::OnBnClickedButtonEdit)
END_MESSAGE_MAP()


// CAlgoDlg message handlers


BOOL CAlgoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	LoadTree();
	UpdateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
								// EXCEPTION: OCX Property Pages should return FALSE
}


void CAlgoDlg::OnTvnSelchangedTreeAlgo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	UpdateControls();

	*pResult = 0;
}

void CAlgoDlg::LoadTree()
{
	CMainFrame* mf = (CMainFrame*)theApp.m_pMainWnd;
	CString gname;
	m_tree.DeleteAllItems();
	for (int i = 0; i < mf->AlgGCount; i++) {
		gname = mf->AlgGroups[i];
		HTREEITEM gti = m_tree.InsertItem(gname, TVI_ROOT, TVI_LAST);
		m_tree.SetItemState(gti, TVIS_BOLD, TVIS_BOLD);
		// Font
		LOGFONT logfont;
		CFont *pFont = GetFont();
		pFont->GetLogFont(&logfont);
		logfont.lfItalic = TRUE;
		//logfont.lfUnderline = TRUE;
		for (int x = 0; x < mf->AlgCount; x++) {
			if (mf->AlgGroup[x] == gname) {
				HTREEITEM gti2 = m_tree.InsertItem(mf->AlgName[x], gti, TVI_LAST);
				m_tree.SetItemData(gti2, x);
				//if (mf->ParamCount[x] > 0) 
				for (int z = 0; z < mf->ParamCount[x]; z++) {
					HTREEITEM gti3 = m_tree.InsertItem(mf->ParamName[x][z], gti2, TVI_LAST);
					m_tree.SetItemColor(gti3, RGB(0, 200, 0));
					//m_tree.SetItemFont(gti3, logfont);
					if (mf->m_config == mf->ParamName[x][z]) {
						m_tree.SelectItem(gti3);
					}
				}
				if (mf->m_algo == x) {
					m_tree.SelectItem(gti2);
					m_tree.Expand(gti2, TVM_EXPAND);
				}
			}
		}
		m_tree.Expand(gti, TVM_EXPAND);
	}
}

void CAlgoDlg::UpdateControls()
{
	HTREEITEM hti = m_tree.GetSelectedItem();
	if (hti == NULL) {
		// Clear all
		m_algo.SetWindowTextA("Algorithm:");
		m_algo.EnableWindow(false);
		m_comment.SetWindowTextA("");
		m_comment.EnableWindow(false);
		m_config.SetWindowTextA("Config:");
		m_config.EnableWindow(false);
		m_text.SetWindowTextA("");
		m_text.EnableWindow(false);
		GetDlgItem(IDC_BUTTON_SAVEAS)->EnableWindow(false);
		return;
	}
	int i = m_tree.GetItemData(hti);
	// Group
	if (m_tree.GetParentItem(hti) == NULL) {
		// Clear all
		m_algo.SetWindowTextA("Algorithm:");
		m_algo.EnableWindow(false);
		m_comment.SetWindowTextA("");
		m_comment.EnableWindow(false);
		m_config.SetWindowTextA("Config:");
		m_config.EnableWindow(false);
		m_text.SetWindowTextA("");
		m_text.EnableWindow(false);
	}
	// Algorithm
	else if (m_tree.GetParentItem(m_tree.GetParentItem(hti)) == NULL) {
		CMainFrame* mf = (CMainFrame*)theApp.m_pMainWnd;
		m_comment.SetWindowTextA(mf->AlgComment[i]);
		m_algo.SetWindowTextA(mf->AlgName[i]);
		m_algo.EnableWindow(true);
		m_comment.EnableWindow(true);
		// Clear config
		m_config.SetWindowTextA("Config:");
		m_config.EnableWindow(false);
		m_text.SetWindowTextA("");
		m_text.EnableWindow(false);
	}
	// Config
	else {
		m_config.EnableWindow(true);
		m_text.EnableWindow(true);
	}
}

void CAlgoDlg::OnBnClickedButtonSaveas()
{
	// TODO: Add your control notification handler code here
}


void CAlgoDlg::OnBnClickedButtonEdit()
{
	LoadTree();
}
