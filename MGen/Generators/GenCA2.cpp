#include "../stdafx.h"
#include "GenCA2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CGenCA2::CGenCA2()
{
	v_cnt = 4;
}

CGenCA2::~CGenCA2()
{
}

void CGenCA2::LoadConfigLine(CString * sN, CString * sV, int idata, float fdata) {
	//CheckVar(sN, sV, "cantus_high", &cantus_high);
	//CheckVar(sN, sV, "cantus_id", &cantus_id2);

	CGenCP1::LoadConfigLine(sN, sV, idata, fdata);
}

void CGenCA2::Generate() {
	CString st;
	int s_len2 = s_len;
	InitCP();
	LoadCP(midi_file);
	if (cpoint.size() < 1) return;
	// Saved t_generated
	int t_generated2 = 0;
	for (int i = 0; i < cpoint.size(); i++) {
		++cantus_id;
		// Check limit
		if (t_generated >= t_cnt) {
			WriteLog(3, "Reached t_cnt steps. Generation stopped");
			break;
		}
		st.Format("Analyzing: %d of %d", cantus_id, cpoint.size());
		SetStatusText(3, st);
		if (need_exit) break;
		if (step < 0) step = 0;
		milliseconds time_start = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
		// Add line
		linecolor[step] = Color(255, 0, 0, 0);
		// Get key
		GetCantusKey(cpoint[i][0]);
		if (tonic_cur == -1) continue;
		CalcCcIncrement();
		// Show imported melody
		cc_len = cantus_len[i];
		cc_tempo = cantus_tempo[i];
		real_len = accumulate(cantus_len[i].begin(), cantus_len[i].end(), 0);
		dpenalty_cur = 0;
		scpoint = cpoint[i];
		ScanCP(tEval, 0);
		key_eval = "";
		// Check if cantus was shown
		if (t_generated2 == t_generated) continue;
		t_generated2 = t_generated;
		// Fill pauses if no results generated
		FillPause(step - real_len - 1, real_len, 2);
		FillPause(step - real_len - 1, real_len, 3);
		// Count additional variables
		CountOff(step - real_len - 1, step - 1);
		CountTime(step - real_len - 1, step - 1);
		UpdateNoteMinMax(step - real_len - 1, step - 1);
		UpdateTempoMinMax(step - real_len - 1, step - 1);
		CreateScanMatrix(i);
		// If no corrections needed
		if (!corrections || !smatrixc) {
			// Go forward
			t_generated = step;
			Adapt(step - real_len - 1, step - 1);
			t_sent = t_generated;
			continue;
		}
		step -= real_len + 1;
		GetSourceRange();
		if (method == mSWA) {
			SWA(i, 1);
		}
		else {
			s_len = s_len2;
			clib.clear();
			rpenalty.clear();
			dpenalty.clear();
			rpenalty_min = 0;
			dpenalty_min = MAX_PENALTY;
			// Full scan marked notes
			ScanCantus(tCor, 0, &(cantus[i]));
			rpenalty_min = 0;
		}
		// Check if we have results
		if (clib.size()) {
			SendCorrections(i, time_start);
		}
		else {
			// Go forward
			step += real_len + 1;
			Adapt(step - real_len - 1, step - 1);
			t_generated = step;
			t_sent = t_generated;
		}
	}
	st.Format("Analyzed %d of %d", cantus_id, cantus.size());
	SetStatusText(3, st);
	ShowStuck();
}