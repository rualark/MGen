// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../stdafx.h"
//#include "../GLibrary/GLib.h"
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
	CheckVar(sN, sV, "min_tempo", &min_tempo);
	CheckVar(sN, sV, "max_tempo", &max_tempo);
}

void CGenRL1::Generate()
{
	int v = 0;
	int tempo_value = GetVaueInt(min_tempo, max_tempo, 1);
	for (int i = 0; i < t_cnt; i++) {
		if (need_exit) return;
		if (i >= t_allocated) ResizeVectors(t_allocated * 2);
		Sleep(sleep_ms);
		//note[i][v] = 60 + (i % 12);
		//note[i][v] = 60 + (GetVaue() % 12);
		note[i][v] = 60 + GetVaueInt(0, 12, 7, 3);
		pause[i][v] = 0;
		len[i][v] = 1;
		dyn[i][v] = randbw(min_dyn, max_dyn);
		
		if ((i % 50) == 0)
		{
			tempo_value = GetVaueInt(min_tempo, max_tempo, 1);
		}
		tempo[i] = tempo_value;
		
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

unsigned int CGenRL1::GetVaue()
{
	/*
	CGLib *gl = new CGLib();
	unsigned int iRes = gl->rand2() / RAND_MAX;
	delete gl;
	return iRes;
	*/
	//// Return random value from 0 to RAND_MAX using randrsl
	return rand2();
}

int CGenRL1::GetVaueInt(int iMin, int iMax, int iType, int iDepth)
{
	/*
	1 uniform
	2 linear low priority
	3 linear high priority
	4 triangular
	5 linear low priority with depth
	6 linear high priority with depth
	7 triangular with depth
	*/
	if (iType == 1)
	{
		/*
		y = ax + b
		outMax = a inMax + b
		outMin = a inMin + b
		a = (outMax - outMin) / (inMax - inMin)
		b = outMax - a inMax
		*/

		/*
		float a = (float)(iMax - iMin) / (float)RAND_MAX;
		float b =  -1 * (float)iMin;
		// Return unsigned int random value from 0 to RAND_MAX using randrsl
		return a * rand2() + b;
		*/
		return randbw(iMin, iMax);
	}
	else if (iType == 2)
	{
		int iOne = randbw(iMin, iMax);
		int iTwo = randbw(iMin, iMax);
		return (iOne < iTwo) ? iOne : iTwo;
	}
	else if (iType == 3)
	{
		int iOne = randbw(iMin, iMax);
		int iTwo = randbw(iMin, iMax);
		return (iOne > iTwo) ? iOne : iTwo;
	}
	else if (iType == 4)
	{
		int iOne = randbw(iMin, iMax);
		int iTwo = randbw(iMin, iMax);
		return (float)(iOne + iTwo) / 2.0;
	}
	else if (iType == 5)
	{
		int iRnd = iMax;
		if (iDepth > 0)
		{
			int iUniRnd;
			for (int i = 0; i <= iDepth; i++)
			{
				iUniRnd = randbw(iMin, iMax);
				iRnd = (iUniRnd < iRnd) ? iUniRnd : iRnd;
			}
		}
		else
		{
			iRnd = GetVaueInt(iMin, iMax, 2);
		}
		
		return iRnd;
	}
	else if (iType == 6)
	{
		int iRnd = iMin;
		if (iDepth > 0)
		{
			int iUniRnd;
			for (int i = 0; i <= iDepth; i++)
			{
				iUniRnd = randbw(iMin, iMax);
				iRnd = (iUniRnd > iRnd) ? iUniRnd : iRnd;
			}
		}
		else
		{
			iRnd = GetVaueInt(iMin, iMax, 3);
		}
		return iRnd;
	}
	else if (iType == 7)
	{
		if (iDepth > 0)
		{
			int iAccum = 0;
			for (int i = 0; i <= iDepth; i++)
			{
				iAccum += randbw(iMin, iMax);
			}
			return (float)iAccum / (float)iDepth;
		}
		else
		{
			return GetVaueInt(iMin, iMax, 4);
		}
	}
	/*
	opcode trirnd_depth, i, iii
	iMin, iMax, iMaxCount xin
	 ;set a counter and accumulator
	iCount     =          0
	iAccum     =          0
	 ;perform loop and accumulate
	 until iCount == iMaxCount do
	iUniRnd    random     iMin, iMax
	iAccum     +=         iUniRnd
	iCount     +=         1
	 enduntil
	 ;get the mean and output
	iRnd       =          iAccum / iMaxCount
			   xout       iRnd
	endop
	*/
}
