#include "stdafx.h"
#include "GenCP1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CGenCP1::CGenCP1()
{
}


CGenCP1::~CGenCP1()
{
}

void CGenCP1::LoadConfigLine(CString * sN, CString * sV, int idata, float fdata)
{
	CheckVar(sN, sV, "min_dyn", &min_dyn);
	CheckVar(sN, sV, "max_dyn", &max_dyn);
}

void CGenCP1::Generate()
{
	int v = 0;
	for (int i = 0; i < t_cnt; i++) {
	}
	t_sent = t_generated;
}
