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

void CGenCA2::SendCorrectionsCP(int i, milliseconds time_start) {
	CString st, st2;
	// Count penalty
	long cnum = clib.size();
	dpenalty.resize(cnum);
	for (int x = 0; x < cnum; x++) {
		dpenalty[x] = 0;
		for (int z = 0; z < c_len; z++) {
			int dif = abs(cpoint[i][1][z] - clib[x][z]);
			if (dif) dpenalty[x] += step_penalty + pitch_penalty * dif;
		}
	}
	// Find minimum penalty
	int ccount = 0;
	// Cycle through all best matches
	st2 = "";
	for (int p = 0; p < corrections; p++) {
		// Find minimum penalty
		cids.clear();
		float dpenalty_min = MAX_PENALTY;
		for (int x = 0; x < cnum; x++) if (dpenalty[x] < dpenalty_min) dpenalty_min = dpenalty[x];
		// Get all best corrections
		for (int x = 0; x < cnum; x++) if (dpenalty[x] == dpenalty_min) {
			cids.push_back(x);
		}
		if (!cids.size() || dpenalty_min == MAX_PENALTY) break;
		// Shuffle cids
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		::shuffle(cids.begin(), cids.end(), default_random_engine(seed));
		for (int x = 0; x < cids.size(); x++) {
			ccount++;
			if (ccount > corrections) break;
			// Write log
			st.Format("%.0f/%.0f/%d/%d ", rpenalty_min, dpenalty_min, cids.size(), clib.size());
			st2 += st;
			// Show initial melody again if this is not first iteration
			if (ccount > 1) {
				dpenalty_cur = 0;
				scpoint = cpoint[i];
				ScanCP(tEval, 0);
				step = step0;
			}
			// Get cantus
			scpoint[cpv] = clib[cids[x]];
			dpenalty_cur = dpenalty[cids[x]];
			// Clear penalty
			dpenalty[cids[x]] = MAX_PENALTY;
			// Show result
			ScanCP(tEval, 2);
			// Go back
			step = step0;
			if (step < 0) break;
			// Add lining
			int pos = step;
			for (int z = 0; z < c_len; z++) {
				if (cpoint[i][cpv][z] != clib[cids[x]][z]) {
					for (int g = 0; g < cc_len[z]; g++) {
						lining[pos + g][1] = 1;
					}
				}
				pos += cc_len[z];
			}
			// Go forward
			step = t_generated;
			Adapt(step0, step - 1);
			t_sent = t_generated;
		}
	}
	milliseconds time_stop = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	// Send log
	CString est;
	est.Format("Sent corrections in %d ms to step %d with rp/dp/variants/lib: %s", time_stop - time_start, step, st2);
	WriteLog(3, est);
}

// Create cc, cc_len and cc_tempo
void CGenCA2::MergeCantus() {
	int prev_note = -1;
	int lpos = 0;
	fli_size = 0;
	cc.clear();
	cc_len.clear();
	cc_tempo.clear();
	for (int i = 0; i < cpoint[cantus_id][cfv].size(); ++i) {
		if (prev_note != cpoint[cantus_id][cfv][i]) {
			prev_note = cpoint[cantus_id][cfv][i];
			cc.push_back(prev_note);
			cc_len.push_back(cantus_len[cantus_id][i]);
			cc_tempo.push_back(cantus_tempo[cantus_id][i]);
		}
		else {
			// Add to cc_len
			cc_len[cc_len.size()-1] += cantus_len[cantus_id][i];
		}
	}
}

void CGenCA2::Generate() {
	CString st;
	int s_len2 = s_len;
	InitCP();
	SetStatusText(8, "MIDI file: " + fname_from_path(midi_file));
	LoadCP(midi_file);
	if (cpoint.size() < 1) return;
	// Saved t_generated
	int t_generated2 = 0;
	cantus_id = -1;
	for (int i = 0; i < cpoint.size(); i++) {
		++cantus_id;
		step0 = step;
		// Check limit
		if (t_generated >= t_cnt) {
			WriteLog(3, "Reached t_cnt steps. Generation stopped");
			break;
		}
		st.Format("Analyzing: %d of %d", cantus_id+1, cpoint.size());
		SetStatusText(3, st);
		if (need_exit) break;
		if (step < 0) step = 0;
		milliseconds time_start = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
		// Add line
		linecolor[step] = Color(255, 0, 0, 0);
		// Choose level
		if (cp_incom[i].size()) {
			if (cp_incom[i][0] == "cf=low") cantus_high = 0;
			if (cp_incom[i][0] == "cf=high") cantus_high = 1;
		}
		if (cantus_high) {
			cfv = 1;
		}
		else {
			cfv = 0;
		}
		// Get key
		GetCantusKey(cpoint[i][cfv]);
		if (tonic_cur == -1) continue;
		CalcCcIncrement();
		// Show imported melody
		MergeCantus();
		real_len = accumulate(cantus_len[i].begin(), cantus_len[i].end(), 0);
		dpenalty_cur = 0;
		// Create pause
		FillPause(step0, floor(real_len / 8 + 1) * 8, 1);
		FillPause(step0, floor(real_len / 8 + 1) * 8, 2);
		FillPause(step0, floor(real_len / 8 + 1) * 8, 3);
		cpv = cfv;
		SelectRuleSet(cf_rule_set);
		ScanCantus(tEval, 0, &(cc));
		// Show cantus id
		st.Format("Counterpoint %d. ", cantus_id + 1);
		comment[step0][0] = st + comment[step0][0];
		// Go forward
		t_generated = step;
		Adapt(step0, step - 1);
		t_sent = t_generated;
		t_generated2 = t_generated;
		// Load first voice
		cc_len = cantus_len[i];
		cc_tempo = cantus_tempo[i];
		ac[cfv] = c;
		acc[cfv] = cc;
		apc[cfv] = pc;
		apcc[cfv] = pcc;
		dpenalty_cur = 0;
		scpoint = cpoint[i];
		// Choose level
		if (cantus_high) {
			cpv = 0;
		}
		else {
			cpv = 1;
		}
		SelectRuleSet(cp_rule_set);
		// Get cantus interval
		GetMelodyInterval(cpoint[i][cfv], 0, cpoint[i][cfv].size(), cf_nmin, cf_nmax);
		step0 = step;
		ScanCP(tEval, 0);
		key_eval = "";
		// Check if cantus was shown
		if (t_generated2 == t_generated) continue;
		t_generated2 = t_generated;
		// Fill pauses if no results generated
		FillPause(step0, step-step0, 2);
		FillPause(step0, step - step0, 3);
		// Count additional variables
		CountOff(step0, step - 1);
		CountTime(step0, step - 1);
		UpdateNoteMinMax(step0, step - 1);
		UpdateTempoMinMax(step0, step - 1);
		CreateScanMatrix(i);
		// If no corrections needed
		if (!corrections || !smatrixc) {
			// Go forward
			t_generated = step;
			Adapt(step0, step - 1);
			t_sent = t_generated;
			continue;
		}
		step = step0;
		//GetSourceRange();
		if (method == mSWA) {
			SWACP(i, 1);
		}
		else {
			s_len = s_len2;
			clib.clear();
			rpenalty.clear();
			dpenalty.clear();
			rpenalty_min = 0;
			dpenalty_min = MAX_PENALTY;
			scpoint = cpoint[i];
			// Full scan marked notes
			ScanCP(tCor, 0);
			rpenalty_min = 0;
		}
		// Check if we have results
		if (clib.size()) {
			SendCorrectionsCP(i, time_start);
		}
		else {
			// Go forward
			step = t_generated;
			Adapt(step0, step - 1);
			t_sent = t_generated;
		}
	}
	st.Format("Analyzed %d of %d", cantus_id+1, cpoint.size());
	SetStatusText(3, st);
	ShowStuck();
}