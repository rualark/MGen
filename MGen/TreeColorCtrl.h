#pragma once


// CTreeColorCtrl

class CTreeColorCtrl : public CTreeCtrl
{
	DECLARE_DYNAMIC(CTreeColorCtrl)

public:
	CTreeColorCtrl();
	virtual ~CTreeColorCtrl();

	void SetItemFont(HTREEITEM hItem, LOGFONT & logfont);

	void SetItemBold(HTREEITEM hItem, BOOL bBold);

	void SetItemColor(HTREEITEM hItem, COLORREF color);

	BOOL GetItemFont(HTREEITEM hItem, LOGFONT * plogfont);

	BOOL GetItemBold(HTREEITEM hItem);

	COLORREF GetItemColor(HTREEITEM hItem);

protected:
	DECLARE_MESSAGE_MAP()

	struct Color_Font
	{
		COLORREF color;
		LOGFONT  logfont;
	};
	CMap< void*, void*, Color_Font, Color_Font& > m_mapColorFont; 

public:
	afx_msg void OnPaint();
	BOOL DeleteAllItems();
};


