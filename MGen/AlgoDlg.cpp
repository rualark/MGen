// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// AlgoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MGen.h"
#include "AlgoDlg.h"
#include "afxdialogex.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAlgoDlg dialog

IMPLEMENT_DYNAMIC(CAlgoDlg, CDialog)

CAlgoDlg::CAlgoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_ALGO, pParent)
{
	m_ft = { 0 };
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
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_ALGO, &CAlgoDlg::OnNMDblclkTreeAlgo)
	ON_WM_TIMER()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CAlgoDlg message handlers


BOOL CAlgoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	LoadTree();
	UpdateControls();
	SetTimer(TIMER6, 1000, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
								// EXCEPTION: OCX Property Pages should return FALSE
}


void CAlgoDlg::OnTvnSelchangedTreeAlgo(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	UpdateControls();

	*pResult = 0;
}

void CAlgoDlg::LoadTree()
{
	CMainFrame* mf = (CMainFrame *)AfxGetMainWnd();
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
					if ((mf->m_algo == x) && (mf->m_config == mf->ParamName[x][z])) {
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
	CMainFrame* mf = (CMainFrame *)AfxGetMainWnd();
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
	CString st;
	char pch[2550];
	int pos = 0;
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
	CMainFrame* mf = (CMainFrame *)AfxGetMainWnd();
	HTREEITEM hti = m_tree.GetSelectedItem();
	int alg_id = m_tree.GetItemData(m_tree.GetParentItem(hti));
	// Get current folder
	//char buffer[MAX_PATH];
	//GetModuleFileName(NULL, buffer, MAX_PATH);
	TCHAR buffer[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, buffer); 
	//string::size_type pos = string(buffer).find_last_of("\\/");
	//CString path_old = string(buffer).substr(0, pos).c_str();
	CString path_old = string(buffer).c_str();
	path_old += "\\configs\\" + mf->AlgFolder[alg_id] + "\\";
	CString cname_old = m_tree.GetItemText(hti);
	//mf->WriteLog(1, path_old);
	//mf->WriteLog(1, cname_old);

	// szFilters is a text string that includes two file name filters:
	// "*.my" for "MyType Files" and "*.*' for "All Files."
	TCHAR szFilters[] = _T("MGen configuration files (*.pl)|*.pl|");

	// Create an Open dialog; the default file name extension is ".my".
	CFileDialog fileDlg(FALSE, _T("pl"), path_old + cname_old + ".pl",
		OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT, szFilters, this, 0, false);
	fileDlg.m_ofn.lpstrInitialDir = path_old;

	// Display the file dialog. When user clicks OK, fileDlg.DoModal() 
	// returns IDOK.
	if (fileDlg.DoModal() == IDOK)
	{
		// Get name
		CString path = fileDlg.GetPathName();
		CString cname = path.Mid(path.ReverseFind('\\') + 1);
		cname = cname.Mid(0, cname.GetLength() - 3);
	  // Check name
		//mf->WriteLog(1, cname);
		if ((path.Find(path_old) == -1) || (path.Find(".pl") == -1)) {
			MessageBox("New file must be created in the same folder " + path_old, "Error");
			return;
		}
		//mf->WriteLog(1, cname);
		// Check exists
		if (CGLib::fileExists(path)) {
			MessageBox("This file already exists: " + path, "Error");
			return;
		}
		if (CGLib::dirExists(path)) {
			MessageBox("There is already a folder with this name: " + path, "Error");
			return;
		}
		// Try to copy
		CGLib::copy_file(path_old + cname_old + ".pl", path_old + cname + ".pl");
		// Check file exists now
		if (!CGLib::fileExists(path)) {
			MessageBox("Could not copy file " + path_old + cname_old + ".pl to " + path_old + cname + ".pl", "Error");
			return;
		}
		// Save name
		mf->LoadAlgo();
		// Change to new config
		mf->m_algo = m_tree.GetItemData(m_tree.GetParentItem(hti));
		mf->m_algo_id = mf->AlgID[mf->m_algo];
		mf->m_config = cname;
		mf->SaveSettings();
		LoadTree();
		UpdateControls();
	}	
}


void CAlgoDlg::OnBnClickedButtonEdit()
{
	CMainFrame* mf = (CMainFrame *)AfxGetMainWnd();
	HTREEITEM hti = m_tree.GetSelectedItem();
	int alg_id = m_tree.GetItemData(m_tree.GetParentItem(hti));
	CString path = "configs\\" + mf->AlgFolder[alg_id] + "\\" + m_tree.GetItemText(hti) + ".pl";
	::ShellExecute(GetDesktopWindow()->m_hWnd, "open", path, NULL, NULL, SW_SHOWNORMAL);
}


void CAlgoDlg::OnBnClickedOk()
{
	CMainFrame* mf = (CMainFrame *)AfxGetMainWnd();
	HTREEITEM hti = m_tree.GetSelectedItem();
	if (hti) {
		mf->m_algo = m_tree.GetItemData(m_tree.GetParentItem(hti));
		mf->m_algo_id = mf->AlgID[mf->m_algo];
		mf->m_config = m_tree.GetItemText(hti);
		mf->SaveSettings();
	}

	CDialog::OnOK();
}


void CAlgoDlg::OnNMDblclkTreeAlgo(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (GetDlgItem(IDOK)->IsWindowEnabled()) OnBnClickedOk();

	*pResult = 0;
}


void CAlgoDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER6) {
		CMainFrame* mf = (CMainFrame *)AfxGetMainWnd();
		HTREEITEM hti = m_tree.GetSelectedItem();
		if (hti) {
			//CString config = m_tree.GetItemText(hti);
			CString fname = "configs\\" + mf->AlgFolder[mf->m_algo] + "\\" + m_tree.GetItemText(hti) + ".pl";
			if (CGLib::fileExists(fname)) {
				FILETIME ft = CGLib::fileTime(fname);
				if (CompareFileTime(&ft, &m_ft)) {
					m_ft = ft;
					UpdateControls();
				}
			}
		}
	}

	CDialog::OnTimer(nIDEvent);
}


void CAlgoDlg::OnClose()
{
	KillTimer(TIMER6);

	CDialog::OnClose();
}
