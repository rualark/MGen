// InfoEditCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "MGen.h"
#include "InfoEditCtrl.h"


// CInfoEditCtrl

IMPLEMENT_DYNAMIC(CInfoEditCtrl, CRichEditCtrl)

CInfoEditCtrl::CInfoEditCtrl()
{

}

CInfoEditCtrl::~CInfoEditCtrl()
{
}


BEGIN_MESSAGE_MAP(CInfoEditCtrl, CRichEditCtrl)
	ON_WM_GETDLGCODE()
END_MESSAGE_MAP()



// CInfoEditCtrl message handlers




UINT CInfoEditCtrl::OnGetDlgCode()
{
	return CRichEditCtrl::OnGetDlgCode() & (~DLGC_HASSETSEL); // clear the bit 
	//return CRichEditCtrl::OnGetDlgCode();
}
