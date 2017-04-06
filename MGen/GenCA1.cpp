#include "stdafx.h"
#include "GenCA1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CGenCA1::CGenCA1()
{
}

CGenCA1::~CGenCA1()
{
}

void CGenCA1::LoadConfigLine(CString* sN, CString* sV, int idata, double fdata)
{
	LoadVar(sN, sV, "midi_file", &midi_file);

	CGenCF1::LoadConfigLine(sN, sV, idata, fdata);
}

void CGenCA1::Generate()
{
	InitCantus();
	LoadCantus(midi_file);
	if (cantus.size() < 1) return;
	// This flag is needed to prevent flag skipping
	calculate_stat = 1;
	for (int i = 1; i <= cantus.size(); i++) ScanCantus(i);
	Adapt(0, t_generated - 1);
	t_sent = t_generated;
}
