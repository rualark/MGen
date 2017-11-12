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
	CheckVar(sN, sV, "sleep_ms", &sleep_ms);

	CheckVar(sN, sV, "min_pitch", &min_pitch, 1, 127);
	CheckVar(sN, sV, "max_pitch", &max_pitch, 1, 127);
	CheckVar(sN, sV, "distr_type_pitch", &distr_type_pitch, 1, 7);
	CheckVar(sN, sV, "distr_depth_pitch", &distr_depth_pitch, 1, 10);
	
	CheckVar(sN, sV, "min_dyn", &min_dyn, 1, 127);
	CheckVar(sN, sV, "max_dyn", &max_dyn, 1, 127);
	CheckVar(sN, sV, "distr_type_dyn", &distr_type_dyn, 1, 7);
	CheckVar(sN, sV, "distr_depth_dyn", &distr_depth_dyn, 1, 10);

	CheckVar(sN, sV, "min_len", &min_len, 1, 10);
	CheckVar(sN, sV, "max_len", &max_len, 1, 10);
	CheckVar(sN, sV, "distr_type_len", &distr_type_len, 1, 7);
	CheckVar(sN, sV, "distr_depth_len", &distr_depth_len, 1, 10);

	CheckVar(sN, sV, "note_step", &note_step);
	CheckVar(sN, sV, "pause_freq", &pause_freq);
	
	CheckVar(sN, sV, "min_tempo", &min_tempo, 1, 250);
	CheckVar(sN, sV, "max_tempo", &max_tempo, 1, 250);
	CheckVar(sN, sV, "distr_type_tempo", &distr_type_tempo, 1, 7);
	CheckVar(sN, sV, "distr_depth_tempo", &distr_depth_tempo, 1, 10);
}

void CGenRL1::Generate()
{
	//int v = 0;
	//int tempo_value = GetVaueInt(min_tempo, max_tempo, 1);

	/*
	for (int i = 0; i < t_cnt; i++) {
		if (need_exit) return;
		if (i >= t_allocated) ResizeVectors(t_allocated * 2);
		Sleep(sleep_ms);

		//note[i][v] = 60 + (i % 12);
		//note[i][v] = 60 + GetVaueInt(0, 12, 7, 3);
		note[i][v] = GetVaueInt(min_pitch, max_pitch, distr_type_pitch, distr_depth);

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
	*/
	//WriteLog(5, "Test error 1");
	//WriteLog(5, "Test error 2");
	for (int i = 0; i < t_cnt; i++) {
		if (i >= t_allocated) ResizeVectors(t_allocated * 2);
		if ((i > 0) && (len[i - 1][0] > 1) && (coff[i - 1][0] < len[i - 1][0] - 1)) {
			// Repeat last note
			pause[i][0] = pause[i - 1][0];
			note[i][0] = note[i - 1][0];
			len[i][0] = len[i - 1][0];
			coff[i][0] = coff[i - 1][0] + 1;
		}
		else {
			
			//==============	pitch		========================

			// Create new note
			if (randbw(0, 100) < pause_freq) {
				pause[i][0] = 1;
				note[i][0] = 0;
			}
			else {
				pause[i][0] = 0;
				if (i == 0) {
					//note[i][0] = min_note + (max_note - min_note) * rand2() / RAND_MAX;
					note[i][0] = GetVaueInt(min_pitch, max_pitch, distr_type_pitch, distr_depth_pitch);
				}
				else {
					//if (pause[i - 1][0]) note[i][0] = min_note + (max_note - min_note) * rand2() / RAND_MAX;
					if (pause[i - 1][0])
					{
						note[i][0] = GetVaueInt(min_pitch, max_pitch, distr_type_pitch, distr_depth_pitch);
					}
					else 
					{
						//note[i][0] = note[i - 1][0] + randbw(-note_step, note_step);
						note[i][0] = GetVaueInt(min_pitch, max_pitch, distr_type_pitch, distr_depth_pitch);
						/*
						// Choose again if same note
						if (note[i][0] == note[i - 1][0]) note[i][0] = note[i - 1][0] + randbw(-note_step, note_step);
						// Check limits
						if (note[i][0] > max_note) note[i][0] = 2 * max_note - note[i][0];
						if (note[i][0] < min_note) note[i][0] = 2 * min_note - note[i][0];
						*/
					}
				}
			}
			
			/*
			// Shift note to diatonic
			int found = 0;
			int ni = (note[i][0] - min_note) % 12;
			for (int x = 0; x < 7; x++) {
				// If found note in diatonic
				if (dia_to_chrom[x] == ni) {
					found = 1;
					break;
				}
			}
			if (!found) note[i][0] ++;
			*/

			//==============	length		========================

			//len[i][0] = min_len + (max_len - min_len) * rand2() / RAND_MAX;
			len[i][0] = GetVaueInt(min_len, max_len, distr_type_len, distr_depth_len);
			/*
			if (len[i][0] < min_len) len[i][0] = min_len;
			if (i + len[i][0] > t_cnt - 1) len[i][0] = t_cnt - i;
			*/
			coff[i][0] = 0;
		}

		//==============	tempo		========================

		if (i == 0) 
		{
			//tempo[i] = min_tempo + (float)(max_tempo - min_tempo) * (float)rand2() / (float)RAND_MAX;
			tempo[i] = GetVaueInt(min_tempo, max_tempo, distr_type_tempo, distr_depth_tempo);
		}
		else 
		{
			//tempo[i] = tempo[i - 1] + randbw(-7, 7);
			//if (tempo[i] > max_tempo) tempo[i] = 2 * max_tempo - tempo[i];
			//if (tempo[i] < min_tempo) tempo[i] = 2 * min_tempo - tempo[i];
			tempo[i] = GetVaueInt(min_tempo, max_tempo, distr_type_tempo, distr_depth_tempo);
		}

		//==============	dynamic ========================

		if (i == 0)
		{
			//dyn[i][0] = min_dyn + (max_dyn - min_dyn) * rand2() / RAND_MAX;
			dyn[i][0] = GetVaueInt(min_dyn, max_dyn, distr_type_dyn, distr_depth_dyn);
		}
		else 
		{
			//dyn[i][0] = dyn[i - 1][0] + randbw(-1.5*dyn_step, dyn_step);
			dyn[i][0] = GetVaueInt(min_dyn, max_dyn, distr_type_dyn, distr_depth_dyn);
			/*
			// Check limits
			if (dyn[i][0] > max_dyn) dyn[i][0] = max_dyn;
			if (dyn[i][0] < min_dyn) dyn[i][0] = min_dyn;
			*/
		}


		// Count additional variables
		CountOff(i, i);
		CountTime(i, i);
		UpdateNoteMinMax(i, i);
		UpdateTempoMinMax(i, i);
		// Do not send if did not finish generating current note
		if (coff[i][0] < len[i][0] - 1) continue;
		t_generated = i + 1;
		// Check if we can send new chunk
		if (((i > t_send) && ((i - 3) % t_send == 0)) || (i == t_cnt - 1)) {
			// Moving average 7 <<<|>>>
			int t_sent_old = t_sent;
			float ma;
			float ma_size;
			vector<float> tempo2;
			tempo2.resize(t_generated - t_sent);
			int delta = 3;
			if (i == t_cnt - 1) delta = 0;
			for (int x = t_sent; x < t_generated - delta; x++) {
				ma = 0;
				ma_size = 0;
				for (int z = x - 3; z < x + 4; z++) if ((z >= 0) && (z < t_generated)) {
					ma += tempo[z];
					ma_size += 1;
				}
				tempo2[x - t_sent] = ma / ma_size;
			}
			for (int x = t_sent; x < t_generated - delta; x++) {
				tempo[x] = tempo2[x - t_sent];
				if (tg_min > tempo[x]) tg_min = tempo[x];
				if (tg_max < tempo[x]) tg_max = tempo[x];
			}
			// Copy voice to other voices
			if (v_cnt > 1) {
				for (int x = 1; x < v_cnt; x++) CopyVoice(0, x, t_sent_old, t_generated - 1, -12);
				UpdateNoteMinMax(t_sent_old, t_generated - 1);
			}
			// Send
			if (i == t_cnt - 1) t_sent = t_generated;
			else t_sent = t_generated - 3;
			Adapt(t_sent_old, t_sent - 1);
			// Interface
			::PostMessage(m_hWnd, WM_GEN_FINISH, 1, 0);
		}
		Sleep(sleep_ms);
		if (need_exit) return;
	}
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
	return 0;
}
