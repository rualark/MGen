#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "TreeColorCtrl.h"

// CAlgoDlg dialog

class CAlgoDlg : public CDialog
{
	DECLARE_DYNAMIC(CAlgoDlg)

public:
	CAlgoDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAlgoDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ALGO };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CTreeColorCtrl m_tree;
	CEdit m_comment;
	afx_msg void OnTvnSelchangedTreeAlgo(NMHDR *pNMHDR, LRESULT *pResult);
	void LoadTree();
	CStatic m_algo;
	CStatic m_config;
	CRichEditCtrl m_text;
	afx_msg void OnBnClickedButtonSaveas();
	afx_msg void OnBnClickedButtonEdit();
	void UpdateControls();
};
