#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CEditParamsDlg dialog

class CEditParamsDlg : public CDialog
{
	DECLARE_DYNAMIC(CEditParamsDlg)

public:
	CEditParamsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditParamsDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_EDITPARAMS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CEdit m_algo;
	CComboBox m_config;
	CRichEditCtrl m_params;
};
