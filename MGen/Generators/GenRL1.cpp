#include "../stdafx.h"
#include "GenRL1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CGenRL1::CGenRL1()
{
}


CGenRL1::~CGenRL1()
{
}


void CGenRL1::LoadConfigLine(CString * sN, CString * sV, int idata, float fdata)
{
	CheckVar(sN, sV, "min_dyn", &min_dyn);
	CheckVar(sN, sV, "max_dyn", &max_dyn);
}

void CGenRL1::Generate()
{
	int v = 0;
	for (int i = 0; i < t_cnt; i++) {
		if (need_exit) return;
		if (i >= t_allocated) ResizeVectors(t_allocated * 2);
		Sleep(sleep_ms);
		note[i][v] = 60 + (i % 12);
		pause[i][v] = 0;
		len[i][v] = 1;
		dyn[i][v] = 100;
		tempo[i] = randbw(min_dyn, max_dyn);
		coff[i][v] = 0;
		CountOff(i, i);
		CountTime(i, i);
		t_generated = i + 1;
		UpdateNoteMinMax(i, i);
		UpdateTempoMinMax(i, i);
	}
	Adapt(0, t_generated - 1);
	t_sent = t_generated;
}
