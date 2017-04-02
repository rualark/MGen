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

void CGenCP1::LoadConfigLine(CString * sN, CString * sV, int idata, double fdata)
{
	CheckVar(sN, sV, "min_dyn", &min_dyn);
	CheckVar(sN, sV, "max_dyn", &max_dyn);
}

void CGenCP1::Generate()
{
	int v = 0;
	for (int i = 0; i < t_cnt; i++) {
		if (i >= t_allocated) ResizeVectors(t_allocated * 2);
		note[i][v] = 60 + (i % 12);
		pause[i][v] = 0;
		len[i][v] = 1;
		dyn[i][v] = randbw(min_dyn, max_dyn);
		tempo[i] = randbw(95, 100);
		coff[i][v] = 0;
		CString st;
		st.Format("Hello, my note is %d!", note[i][v]);
		if (i % 3 == 0) comment[i][v] = st;
		color[i][v] = Color(0, (note[i][v] - 60) * 20, 100, 100);
		// Set additional variables
		CountOff(i, i);
		CountTime(i, i);
		UpdateNoteMinMax(i, i);
		UpdateTempoMinMax(i, i);
		t_generated = i + 1;
		Sleep(sleep_ms);
		if (need_exit) return;
		if (i % 14 == 13) {
			Adapt(t_sent, t_generated - 1);
			t_sent = t_generated;
		}
	}
	t_sent = t_generated;
}
