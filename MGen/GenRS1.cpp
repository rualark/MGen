#include "stdafx.h"
#include "GenRS1.h"

CGenRS1::CGenRS1()
{
	// Set default variables
	t_cnt = 60;
	t_allocated = 10;
	t_send = 30;
}

CGenRS1::~CGenRS1()
{
}

void CGenRS1::LoadConfigLine(CString* sN, CString* sV)
{
	int idata = atoi(*sV);
	double fdata = atof(*sV);
	CGenTemplate::CheckVar(sN, sV, "t_cnt", &t_cnt);
	CGenTemplate::CheckVar(sN, sV, "min_tempo", &min_tempo);
	CGenTemplate::CheckVar(sN, sV, "max_tempo", &max_tempo);
	CGenTemplate::CheckVar(sN, sV, "min_len", &min_len);
	CGenTemplate::CheckVar(sN, sV, "max_len", &max_len);
	CGenTemplate::CheckVar(sN, sV, "min_note", &min_note);
	CGenTemplate::CheckVar(sN, sV, "max_note", &max_note);
	CGenTemplate::CheckVar(sN, sV, "note_step", &note_step);
	CGenTemplate::CheckVar(sN, sV, "sleep_ms", &sleep_ms);
	//CGenTemplate::LoadVar(st2, st3, "config", &m_config);
}

void CGenRS1::Generate()
{
	for (int i = 0; i < t_cnt; i++) {
		if (i >= t_allocated) ResizeVectors(t_allocated * 2);
		//TCHAR st[100];
		//_stprintf_s(st, _T("%d"), rand2());
		//TRACE(st);
		if ((i > 0) && (len[i - 1][0] > 1) && (coff[i - 1][0] < len[i - 1][0] - 1)) {
			note[i][0] = note[i - 1][0];
			att[i][0] = att[i - 1][0];
			len[i][0] = len[i - 1][0];
			coff[i][0] = coff[i - 1][0] + 1;
			poff[i][0] = poff[i - 1][0] + 1;
		}
		else {
			if (i == 0) {
				note[i][0] = 60 + (max_note - min_note) * rand2() / RAND_MAX;
			}
			else {
				note[i][0] = note[i - 1][0] + randbw(-note_step, note_step);
				if (note[i][0] > max_note) note[i][0] = 2 * max_note - note[i][0];
				if (note[i][0] < min_note) note[i][0] = 2 * min_note - note[i][0];
			}
			len[i][0] = 8 * rand2() / RAND_MAX;
			if (len[i][0] == 0) len[i][0] = 1;
			coff[i][0] = 0;
			att[i][0] = 50 + 60 * rand2() / RAND_MAX;
			if (i > 0) poff[i][0] = coff[i - 1][0] + 1;
			else poff[i][0] = 0;
			if (i > 0) {
				for (int x = i - 1; x >= i - len[i - 1][0]; x--) {
					noff[x][0] = i - x;
				}
			}
		}
		//if (i < t_cnt-1) noff[i][0] = 1;
		//else noff[i][0] = 0;
		if (i == 0) {
			tempo[i] = min_tempo + (double)(max_tempo - min_tempo) * (double)rand2() / (double)RAND_MAX;
		}
		else {
			tempo[i] = tempo[i - 1] + randbw(-3, 3);
			if (tempo[i] > max_tempo) tempo[i] = 2 * max_tempo - tempo[i];
			if (tempo[i] < min_tempo) tempo[i] = 2 * min_tempo - tempo[i];
		}
		if (i > 0) stime[i] = stime[i - 1] + 30000.0 / (double)tempo[i - 1];
		else stime[i] = 0;
		ntime[i] = stime[i] + 30000.0 / (double)tempo[i];
		pause[i][0] = 0;
		t_generated = i + 1;
		if (ng_min > note[i][0]) ng_min = note[i][0];
		if (ng_max < note[i][0]) ng_max = note[i][0];
		if (tg_min > tempo[i]) tg_min = tempo[i];
		if (tg_max < tempo[i]) tg_max = tempo[i];
		//CString* st = new CString;
		//st->Format("Note generated %d", note[i][0]);
		//::PostMessage(m_hWnd, WM_DEBUG_MSG, 0, (LPARAM)st);
		if ((i > 0) && (i % t_send == 0)) {
			t_sent = t_generated;
			::PostMessage(m_hWnd, WM_GEN_FINISH, 1, 0);
		}
		if (len[i][0] == 0) ::PostMessage(m_hWnd, WM_DEBUG_MSG, 1, (LPARAM)new CString("Critical error: Len = 0"));
		Sleep(sleep_ms);
		if (need_exit) return;
	}
	t_sent = t_generated;
}
