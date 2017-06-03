#pragma once

// CMyVisualManagerOffice2007 command target

class CMyVisualManagerOffice2007 : public CMFCVisualManagerOffice2007
{
	DECLARE_DYNCREATE(CMyVisualManagerOffice2007);
public:
	CMyVisualManagerOffice2007();
	virtual ~CMyVisualManagerOffice2007();
	BOOL DrawTextOnGlass(
		CDC* pDC,
		CString strText,
		CRect rect,
		DWORD dwFlags,
		int nGlowSize = 0,
		COLORREF clrText = (COLORREF)-1) override;
};


