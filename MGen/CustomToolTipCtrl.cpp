// CustomToolTipCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "MGen.h"
#include "CustomToolTipCtrl.h"
#include "MainFrm.h"
#include "MGenView.h"


// CCustomToolTipCtrl

IMPLEMENT_DYNAMIC(CCustomToolTipCtrl, CMFCToolTipCtrl)

CCustomToolTipCtrl::CCustomToolTipCtrl()
{
	m_nCurrID = 0;
}

CCustomToolTipCtrl::~CCustomToolTipCtrl()
{
}

CSize CCustomToolTipCtrl::OnDrawLabel(CDC * pDC, CRect rect, BOOL bCalcOnly)
{
	//	TRACE("ENTER CCustomToolTipCtrl::OnDrawLabel bCalcOnly=%d\n", bCalcOnly);
	ASSERT_VALID(pDC);
	CMainFrame* mf = (CMainFrame*)theApp.m_pMainWnd;
	CMGenView* pView = (CMGenView*)(mf->GetActiveView());

	// my special tooltip drawing system for context sensitive tooltips 
	// in view (track)
	POINT cursor;
	GetCursorPos(&cursor);
	mf->GetActiveView()->ScreenToClient(&cursor);
	CString labelText, descriptionText;
	pView->GetToolTipLabelText(cursor, labelText, descriptionText);

	CSize sizeText(0, 0);

	BOOL bDrawDescr = m_Params.m_bDrawDescription && !m_strDescription.IsEmpty();

	CFont* pOldFont = (CFont*)pDC->SelectObject(m_Params.m_bBoldLabel &&
		bDrawDescr ? &afxGlobalData.fontBold : &afxGlobalData.fontTooltip);

	if (labelText.Find(_T('\n')) >= 0) // Multi-line text
	{
		UINT nFormat = DT_NOPREFIX;
		if (bCalcOnly)
		{
			nFormat |= DT_CALCRECT;
		}

		if (m_pRibbonButton != NULL)
		{
			nFormat |= DT_NOPREFIX;
		}

		int nHeight = pDC->DrawText(labelText, rect, nFormat);
		sizeText = CSize(rect.Width(), nHeight);
	}
	else
	{
		if (bCalcOnly)
		{
			sizeText = pDC->GetTextExtent(labelText);
		}
		else
		{
			UINT nFormat = DT_LEFT | DT_NOCLIP | DT_SINGLELINE;

			if (!bDrawDescr)
			{
				nFormat |= DT_VCENTER;
			}

			if (m_pRibbonButton != NULL)
			{
				nFormat |= DT_NOPREFIX;
			}

			sizeText.cy = pDC->DrawText(labelText, rect, nFormat);
			sizeText.cx = rect.Width();
		}
	}

	pDC->SelectObject(pOldFont);

	SetDescription(descriptionText);

	return sizeText;
	//return CSize();
}


BEGIN_MESSAGE_MAP(CCustomToolTipCtrl, CMFCToolTipCtrl)
END_MESSAGE_MAP()



// CCustomToolTipCtrl message handlers


