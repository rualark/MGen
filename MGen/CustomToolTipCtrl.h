#pragma once


// CCustomToolTipCtrl

class CCustomToolTipCtrl : public CMFCToolTipCtrl
{
	DECLARE_DYNAMIC(CCustomToolTipCtrl)

public:
	CCustomToolTipCtrl();
	virtual ~CCustomToolTipCtrl();
	int	m_nCurrID;

protected:
	DECLARE_MESSAGE_MAP()
	CSize OnDrawLabel(CDC* pDC, CRect rect, BOOL bCalcOnly);
};


