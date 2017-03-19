#pragma once


// CInfoEditCtrl

class CInfoEditCtrl : public CRichEditCtrl
{
	DECLARE_DYNAMIC(CInfoEditCtrl)

public:
	CInfoEditCtrl();
	virtual ~CInfoEditCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg UINT OnGetDlgCode();
	void AddText(CString str, COLORREF color, DWORD dwEffects);
};


