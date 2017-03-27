#include "stdafx.h"
#include "GenCF2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CGenCF2::CGenCF2()
{
}


CGenCF2::~CGenCF2()
{
	TRACE("CGenCF2 destructor called\n");
}

void CGenCF2::Generate()
{
	TRACE("CGenCF2.Generate called\n");
}

void CGenCF2::LoadConfigLine(CString* sN, CString* sV, int idata, double fdata)
{
}
