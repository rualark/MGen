#include "stdafx.h"
#include "GenMP1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CGenMP1::CGenMP1()
{
}

CGenMP1::~CGenMP1()
{
}

void CGenMP1::Generate()
{
	LoadMidi("load", "hamlet");
	Adapt(0, t_generated - 1);
	t_sent = t_generated;
}

void CGenMP1::LoadConfigLine(CString* sN, CString* sV, int idata, double fdata)
{
}
