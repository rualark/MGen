#include "../stdafx.h"
#include "GenCA2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CGenCA2::CGenCA2()
{
}

CGenCA2::~CGenCA2()
{
}

void CGenCA2::LoadConfigLine(CString * sN, CString * sV, int idata, float fdata) {
	//CheckVar(sN, sV, "cantus_high", &cantus_high);
	//CheckVar(sN, sV, "cantus_id", &cantus_id2);

	CGenCP1::LoadConfigLine(sN, sV, idata, fdata);
}

void CGenCA2::Generate() {
	CString st;
	InitCantus();
	LoadCP(midi_file);
}