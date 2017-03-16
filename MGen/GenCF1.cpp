#include "stdafx.h"
#include "GenCF1.h"


CGenCF1::CGenCF1()
{
	t_cnt = 800;
	Init();
}


CGenCF1::~CGenCF1()
{
}

void CGenCF1::Generate()
{
	for (int i = 0; i < t_cnt; i++) {
		note[i][0] = 60 + 12 * rand() / RAND_MAX;
		len[i][0] = 1;
		coff[i][0] = 0;
		if (i>0) poff[i][0] = 1;
		else poff[i][0] = 0;
		if (i<15) noff[i][0] = 1;
		else noff[i][0] = 0;
		att[i][0] = 100;
		tempo[i][0] = 100;
		pause[i][0] = 0;
		t_generated = i;
		Sleep(100);
	}
}
