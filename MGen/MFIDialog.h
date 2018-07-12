#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "TreeColorCtrl.h"


// CMFIDialog dialog

class CMFIDialog : public CDialog
{
	DECLARE_DYNAMIC(CMFIDialog)

public:
	CMFIDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMFIDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MFI };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CTreeColorCtrl m_tree;
	CEdit m_comment;
	CString ext; // file extension
	afx_msg void OnTvnSelchangedTreeAlgo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkTreeAlgo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	void LoadTree();
	void UpdateControls();
};
