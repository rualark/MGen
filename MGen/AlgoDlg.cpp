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
	ON_BN_CLICKED(IDOK, &CAlgoDlg::OnBnClickedOk)
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
				if (mf->m_algo == x) m_tree.SelectItem(gti2);
				for (int z = 0; z < mf->ParamCount[x]; z++) {
					HTREEITEM gti3 = m_tree.InsertItem(mf->ParamName[x][z], gti2, TVI_LAST);
					m_tree.SetItemColor(gti3, RGB(0, 200, 0));
					//m_tree.SetItemFont(gti3, logfont);
					if (mf->m_config == mf->ParamName[x][z]) {
						m_tree.SelectItem(gti3);
					}
					m_tree.SetItemData(gti3, z);
				}
				if (mf->m_algo == x) m_tree.Expand(gti2, TVM_EXPAND);
			}
		}
		m_tree.Expand(gti, TVM_EXPAND);
	}
}

void CAlgoDlg::UpdateControls()
{
	CMainFrame* mf = (CMainFrame*)theApp.m_pMainWnd;
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
		GetDlgItem(IDOK)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_SAVEAS)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_EDIT)->EnableWindow(false);
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
		GetDlgItem(IDOK)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_SAVEAS)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_EDIT)->EnableWindow(false);
	}
	// Algorithm
	else if (m_tree.GetParentItem(m_tree.GetParentItem(hti)) == NULL) {
		m_comment.SetWindowTextA(mf->AlgComment[i]);
		m_algo.SetWindowTextA("Algorithm: " + mf->AlgName[i]);
		m_algo.EnableWindow(true);
		m_comment.EnableWindow(true);
		// Clear config
		m_config.SetWindowTextA("Config:");
		m_config.EnableWindow(false);
		m_text.SetWindowTextA("");
		m_text.EnableWindow(false);
		GetDlgItem(IDOK)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_SAVEAS)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_EDIT)->EnableWindow(false);
	}
	// Config
	else {
		m_config.EnableWindow(true);
		m_text.EnableWindow(true);
		m_algo.EnableWindow(true);
		m_comment.EnableWindow(true);
		int alg_id = m_tree.GetItemData(m_tree.GetParentItem(hti));
		CString path = "configs\\" + mf->AlgFolder[alg_id] + "\\" + m_tree.GetItemText(hti) + ".pl";
		LoadFile(path);
		m_algo.SetWindowTextA("Algorithm: " + mf->AlgName[alg_id]);
		m_comment.SetWindowTextA(mf->AlgComment[alg_id]);
		m_config.SetWindowTextA("Config: " + m_tree.GetItemText(hti));
		GetDlgItem(IDOK)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_SAVEAS)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_EDIT)->EnableWindow(true);
	}
}

void CAlgoDlg::LoadFile(CString path) {
	ifstream fs;
	m_text.SetWindowTextA("");
	fs.open(path);
	CString st, st2, st3, st4, st5;
	char pch[2550];
	int pos = 0;
	// Load header
	fs.getline(pch, 2550);
	m_text.AddText(path + "\n", RGB(0, 0, 0), CFE_BOLD);
	while (fs.good()) {
		fs.getline(pch, 2550);
		st = pch;
		pos = st.Find("#");
		if (pos != -1) {
			m_text.AddText(st.Left(pos), RGB(0, 0, 0), 0);
			m_text.AddText(st.Mid(pos) + "\n", RGB(0, 150, 0), 0);
		}
		else {
			m_text.AddText(st + "\n", RGB(0, 0, 0), 0);
		}
	}
}

void CAlgoDlg::OnBnClickedButtonSaveas()
{
	CMainFrame* mf = (CMainFrame*)theApp.m_pMainWnd;
	HTREEITEM hti = m_tree.GetSelectedItem();
	// szFilters is a text string that includes two file name filters:
	// "*.my" for "MyType Files" and "*.*' for "All Files."
	TCHAR szFilters[] = _T("MGen configuration files (*.pl)|*.pl|All Files (*.*)|*.*||");

	// Create an Open dialog; the default file name extension is ".my".
	CFileDialog fileDlg(FALSE, _T("pl"), _T("*.pl"),
		OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, szFilters);

	// Display the file dialog. When user clicks OK, fileDlg.DoModal() 
	// returns IDOK.
	if (fileDlg.DoModal() == IDOK)
	{
		// Get name
		CString path = fileDlg.GetPathName();
		CString cname = path.Mid(path.ReverseFind('\\') + 1);
		cname = cname.Mid(0, cname.GetLength()-3)
	  // Check name
		int alg_id = m_tree.GetItemData(m_tree.GetParentItem(hti));
		CString path_old = "configs\\" + mf->AlgFolder[alg_id] + "\\";
		if ((path.Find(path_old) == -1) || (path.Find(".pl") == -1)) {
			MessageBox("File must be created in the same folder " + path_old + " and must have extension *.pl", "Error", 0);
			return;
		}
		// Save name
		mf->LoadAlgo();
		// Change to new config
		mf->m_algo = m_tree.GetItemData(m_tree.GetParentItem(hti));
		mf->m_algo_id = mf->AlgID[mf->m_algo];
		mf->m_config = m_tree.GetItemText(hti);
		mf->SaveSettings();
		LoadTree();
	}	
}


void CAlgoDlg::OnBnClickedButtonEdit()
{
	CMainFrame* mf = (CMainFrame*)theApp.m_pMainWnd;
	HTREEITEM hti = m_tree.GetSelectedItem();
	int alg_id = m_tree.GetItemData(m_tree.GetParentItem(hti));
	CString path = "configs\\" + mf->AlgFolder[alg_id] + "\\" + m_tree.GetItemText(hti) + ".pl";
	::ShellExecute(GetDesktopWindow()->m_hWnd, "open", path, NULL, NULL, SW_SHOWNORMAL);
}


void CAlgoDlg::OnBnClickedOk()
{
	CMainFrame* mf = (CMainFrame*)theApp.m_pMainWnd;
	HTREEITEM hti = m_tree.GetSelectedItem();
	mf->m_algo = m_tree.GetItemData(m_tree.GetParentItem(hti));
	mf->m_algo_id = mf->AlgID[mf->m_algo];
	mf->m_config = m_tree.GetItemText(hti);
	mf->SaveSettings();

	CDialog::OnOK();
}
