#pragma once
#include "afxcmn.h"
#include "GLibrary/GLib.h"
#include "afxwin.h"
#include "InfoEditCtrl.h"

// CInfoDlg dialog

class CInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CInfoDlg)

public:
	CInfoDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInfoDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_INFO };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CInfoEditCtrl m_info;
	virtual BOOL OnInitDialog();

	int melody = 0;
};
