#pragma once
#include "afxcmn.h"

#include "InfoEditCtrl.h"

// CMsgDlg dialog

class CMsgDlg : public CDialog
{
	DECLARE_DYNAMIC(CMsgDlg)

public:
	CMsgDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMsgDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MSG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_text;
	CInfoEditCtrl m_edit;
	virtual BOOL OnInitDialog();
};
