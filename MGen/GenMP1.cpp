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

void CGenMP1::LoadConfigLine(CString* sN, CString* sV, int idata, float fdata)
{
	LoadVar(sN, sV, "midi_file", &midi_file);
}

void CGenMP1::Generate()
{
	LoadMidi(midi_file);
	if (!t_generated) return;
	Adapt(0, t_generated - 1);
	t_sent = t_generated;
}
