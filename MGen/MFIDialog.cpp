// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// MFIDialog.cpp : implementation file
//

#include "stdafx.h"
#include "MGen.h"
#include "MFIDialog.h"
#include "afxdialogex.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMFIDialog dialog

IMPLEMENT_DYNAMIC(CMFIDialog, CDialog)

CMFIDialog::CMFIDialog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_MFI, pParent)
{

}

CMFIDialog::~CMFIDialog()
{
}

void CMFIDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_ALGO, m_tree);
	DDX_Control(pDX, IDC_EDIT_COMMENT, m_comment);
}


BEGIN_MESSAGE_MAP(CMFIDialog, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_ALGO, &CMFIDialog::OnTvnSelchangedTreeAlgo)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_ALGO, &CMFIDialog::OnNMDblclkTreeAlgo)
	ON_BN_CLICKED(IDOK, &CMFIDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// CMFIDialog message handlers


void CMFIDialog::OnTvnSelchangedTreeAlgo(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	UpdateControls();

	*pResult = 0;
}


void CMFIDialog::OnNMDblclkTreeAlgo(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (GetDlgItem(IDOK)->IsWindowEnabled()) OnBnClickedOk();

	*pResult = 0;
}


void CMFIDialog::OnBnClickedOk()
{
	CMainFrame* mf = (CMainFrame *)AfxGetMainWnd();
	HTREEITEM hti = m_tree.GetSelectedItem();
	mf->m_algo = m_tree.GetItemData(hti);
	mf->m_algo_id = mf->AlgID[mf->m_algo];

	CDialog::OnOK();
}


BOOL CMFIDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	LoadTree();
	UpdateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
								// EXCEPTION: OCX Property Pages should return FALSE
}

void CMFIDialog::LoadTree()
{
	CMainFrame* mf = (CMainFrame *)AfxGetMainWnd();
	CString gname;
	m_tree.DeleteAllItems();
	if (ext == "xml" || ext == "mxl") {
		for (int i = 0; i < mf->AlgXFIGCount; i++) {
			gname = mf->AlgXFIGroups[i];
			HTREEITEM gti = m_tree.InsertItem(gname, TVI_ROOT, TVI_LAST);
			m_tree.SetItemState(gti, TVIS_BOLD, TVIS_BOLD);
			// Font
			LOGFONT logfont;
			CFont *pFont = GetFont();
			pFont->GetLogFont(&logfont);
			logfont.lfItalic = TRUE;
			//logfont.lfUnderline = TRUE;
			for (int x = 0; x < mf->AlgCount; x++) 
				if ((mf->AlgXFI[x]) && (CGLib::fileExists("configs\\" + mf->AlgFolder[x] + ".pl"))) {
				if (mf->AlgGroup[x] == gname) {
					HTREEITEM gti2 = m_tree.InsertItem(mf->AlgName[x], gti, TVI_LAST);
					m_tree.SetItemData(gti2, x);
					//if (mf->ParamCount[x] > 0) 
					if (mf->m_algo == x) m_tree.SelectItem(gti2);
					m_tree.Expand(gti2, TVM_EXPAND);
				}
			}
			m_tree.Expand(gti, TVM_EXPAND);
		}
	}
	else {
		for (int i = 0; i < mf->AlgMFIGCount; i++) {
			gname = mf->AlgMFIGroups[i];
			HTREEITEM gti = m_tree.InsertItem(gname, TVI_ROOT, TVI_LAST);
			m_tree.SetItemState(gti, TVIS_BOLD, TVIS_BOLD);
			// Font
			LOGFONT logfont;
			CFont *pFont = GetFont();
			pFont->GetLogFont(&logfont);
			logfont.lfItalic = TRUE;
			//logfont.lfUnderline = TRUE;
			for (int x = 0; x < mf->AlgCount; x++) if ((mf->AlgMFI[x]) && (CGLib::fileExists("configs\\" + mf->AlgFolder[x] + ".pl"))) {
				if (mf->AlgGroup[x] == gname) {
					HTREEITEM gti2 = m_tree.InsertItem(mf->AlgName[x], gti, TVI_LAST);
					m_tree.SetItemData(gti2, x);
					//if (mf->ParamCount[x] > 0) 
					if (mf->m_algo == x) m_tree.SelectItem(gti2);
					m_tree.Expand(gti2, TVM_EXPAND);
				}
			}
			m_tree.Expand(gti, TVM_EXPAND);
		}
	}
}

void CMFIDialog::UpdateControls()
{
	CMainFrame* mf = (CMainFrame *)(AfxGetApp()->GetMainWnd());
	HTREEITEM hti = m_tree.GetSelectedItem();
	if (hti == NULL) {
		// Clear all
		m_comment.SetWindowTextA("");
		m_comment.EnableWindow(false);
		GetDlgItem(IDOK)->EnableWindow(false);
		return;
	}
	int i = m_tree.GetItemData(hti);
	// Group
	if (m_tree.GetParentItem(hti) == NULL) {
		// Clear all
		m_comment.SetWindowTextA("");
		m_comment.EnableWindow(false);
		GetDlgItem(IDOK)->EnableWindow(false);
	}
	// Algorithm
	else if (m_tree.GetParentItem(m_tree.GetParentItem(hti)) == NULL) {
		m_comment.SetWindowTextA(mf->AlgComment[i]);
		m_comment.EnableWindow(true);
		GetDlgItem(IDOK)->EnableWindow(true);
	}
}
