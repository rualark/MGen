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
	LoadMidi("load", "test");
	t_sent = t_generated;
}

void CGenMP1::LoadConfigLine(CString* sN, CString* sV, int idata, double fdata)
{
}
