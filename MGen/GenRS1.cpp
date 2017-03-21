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

void CGenRS1::LoadConfigLine(CString* sN, CString* sV, int idata, double fdata)
{
	CGenTemplate::CheckVar(sN, sV, "min_tempo", &min_tempo);
	CGenTemplate::CheckVar(sN, sV, "max_tempo", &max_tempo);
	CGenTemplate::CheckVar(sN, sV, "min_len", &min_len);
	CGenTemplate::CheckVar(sN, sV, "max_len", &max_len);
	CGenTemplate::CheckVar(sN, sV, "min_note", &min_note);
	CGenTemplate::CheckVar(sN, sV, "max_note", &max_note);
	CGenTemplate::CheckVar(sN, sV, "note_step", &note_step);
	CGenTemplate::CheckVar(sN, sV, "sleep_ms", &sleep_ms);
	//CGenTemplate::LoadVar(st2, st3, "some_var", &some_var);
}

void CGenRS1::Generate()
{
	for (int i = 0; i < t_cnt; i++) {
		if (i >= t_allocated) ResizeVectors(t_allocated * 2);
		if ((i > 0) && (len[i - 1][0] > 1) && (coff[i - 1][0] < len[i - 1][0] - 1)) {
			// Repeat last note
			note[i][0] = note[i - 1][0];
			att[i][0] = att[i - 1][0];
			len[i][0] = len[i - 1][0];
			coff[i][0] = coff[i - 1][0] + 1;
		}
		else {
			// Create new note
			if (i == 0) {
				note[i][0] = 60 + (max_note - min_note) * rand2() / RAND_MAX;
			}
			else {
				note[i][0] = note[i - 1][0] + randbw(-note_step, note_step);
				// Choose again if same note
				if (note[i][0] == note[i-1][0]) note[i][0] = note[i - 1][0] + randbw(-note_step, note_step);
				// Check limits
				if (note[i][0] > max_note) note[i][0] = 2 * max_note - note[i][0];
				if (note[i][0] < min_note) note[i][0] = 2 * min_note - note[i][0];
			}
			len[i][0] = min_len + (max_len-min_len) * rand2() / RAND_MAX;
			if (len[i][0] < min_len) len[i][0] = min_len;
			if (i + len[i][0] > t_cnt - 1) len[i][0] = t_cnt - i;
			coff[i][0] = 0;
			att[i][0] = 50 + 60 * rand2() / RAND_MAX;
		}
		if (i == 0) {
			tempo[i] = min_tempo + (double)(max_tempo - min_tempo) * (double)rand2() / (double)RAND_MAX;
		}
		else {
			tempo[i] = tempo[i - 1] + randbw(-7, 7);
			if (tempo[i] > max_tempo) tempo[i] = 2 * max_tempo - tempo[i];
			if (tempo[i] < min_tempo) tempo[i] = 2 * min_tempo - tempo[i];
		}
		// Count additional variables
		CountOff(i, i);
		CountTime(i, i);
		pause[i][0] = 0;
		t_generated = i + 1;
		UpdateNoteMinMax(i, i);
		UpdateTempoMinMax(i, i);
		// Check if we can send new chunk
		if (((i > 0) && ((i-3) % t_send == 0)) || (i == t_cnt - 1)) {
			// Moving average 7 <<<|>>>
			double ma;
			double ma_size;
			double* tempo2 = new double[t_generated - t_sent];
			int delta = 3;
			if (i == t_cnt - 1) delta = 0;
			for (int x = t_sent; x < t_generated-delta; x++) {
				ma = 0;
				ma_size = 0;
				for (int z = x - 3; z < x + 4; z++) if ((z >= 0) && (z < t_generated)) {
					ma += tempo[z];
					ma_size += 1;
				}
				tempo2[x - t_sent] = ma / ma_size;
			}
			for (int x = t_sent; x < t_generated-delta; x++) {
				tempo[x] = tempo2[x - t_sent];
				if (tg_min > tempo[x]) tg_min = tempo[x];
				if (tg_max < tempo[x]) tg_max = tempo[x];
			}
			delete tempo2;
			// Send
			if (i == t_cnt - 1) t_sent = t_generated;
			else t_sent = t_generated-3;
			// Interface
			::PostMessage(m_hWnd, WM_GEN_FINISH, 1, 0);
			CString* est = new CString;
			est->Format("Chunk sent up to %d", t_sent);
			WriteLog(0, est);
		}
		if (len[i][0] == 0) WriteLog(1, new CString("Critical error: Len = 0"));
		Sleep(sleep_ms);
		if (need_exit) return;
	}
	t_sent = t_generated;
}
