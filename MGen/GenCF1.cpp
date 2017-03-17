#include "stdafx.h"
#include "GenCF1.h"


CGenCF1::CGenCF1()
{
	t_cnt = 8000;
	t_allocated = 8000;
	Init();
}


CGenCF1::~CGenCF1()
{
}

void CGenCF1::Generate()
{
	for (int i = 0; i < t_cnt; i++) {
		if (i >= t_allocated) ResizeVectors(t_allocated*2);
		//TCHAR st[100];
		//_stprintf_s(st, _T("%d"), rand2());
		//TRACE(st);
		note[i][0] = 60 + 12 * rand2() / RAND_MAX;
		len[i][0] = 1;
		coff[i][0] = 0;
		if (i>0) poff[i][0] = 1;
		else poff[i][0] = 0;
		if (i < t_cnt-1) noff[i][0] = 1;
		else noff[i][0] = 0;
		att[i][0] = 100;
		tempo[i][0] = 100;
		pause[i][0] = 0;
		t_generated = i;
		if (ng_min > note[i][0]) ng_min = note[i][0];
		if (ng_max < note[i][0]) ng_max = note[i][0];
		Sleep(0);
	}
}
