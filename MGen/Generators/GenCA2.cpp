// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../stdafx.h"
#include "GenCA2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CGenCA2::CGenCA2()
{
	v_cnt = 4;
	track_id[0] = 2;
	track_id[1] = 1;
	track_id[2] = 4;
	track_id[3] = 3;
}

CGenCA2::~CGenCA2()
{
}

void CGenCA2::LoadConfigLine(CString * sN, CString * sV, int idata, float fdata) {
	//CheckVar(sN, sV, "cantus_high", &cantus_high);
	//CheckVar(sN, sV, "cantus_id", &cantus_id2);

	CGenCP1::LoadConfigLine(sN, sV, idata, fdata);
}

void CGenCA2::SendCorrectionsCP(int i, long long time_start) {
	CString st, st2;
	// Count penalty
	long cnum = clib.size();
	dpenalty.resize(cnum);
	for (int x = 0; x < cnum; x++) {
		dpenalty[x] = 0;
		for (int z = 0; z < c_len; z++) {
			int dif = abs(cpoint[i][cpv][z] - clib[x][z]);
			if (dif) dpenalty[x] += step_penalty + pitch_penalty * dif;
		}
	}
	// Find minimum penalty
	int ccount = 0;
	// Cycle through all best matches
	st2.Empty();
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
		long long seed = CGLib::time();
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
			dpenalty_cur = dpenalty[cids[x]]; //-V519
			// Clear penalty
			dpenalty[cids[x]] = MAX_PENALTY;
			// Show result
			ScanCP(tEval, 2);
			// Go back
			step = step0;
			if (step < 0) break;
			ShowLiningCP(clib[cids[x]]);
			// Go forward
			step = t_generated;
			Adapt(step0, step - 1);
			t_sent = t_generated;
		}
	}
	long long time_stop = CGLib::time();
	// Send log
	CString est;
	est.Format("Sent corrections in %d ms to step %d with rp/dp/variants/lib: %s", time_stop - time_start, step, st2);
	WriteLog(3, est);
}

// Create cc, cc_len and cc_tempo
void CGenCA2::MergeCantus() {
	int prev_note = -1;
	fli_size = 0;
	m_cc.clear();
	cc_len.clear();
	cc_tempo.clear();
	for (int i = 0; i < cpoint[cantus_id][cfv].size(); ++i) {
		if (prev_note != cpoint[cantus_id][cfv][i]) {
			prev_note = cpoint[cantus_id][cfv][i];
			m_cc.push_back(prev_note);
			cc_len.push_back(cantus_len[cantus_id][i]);
			cc_tempo.push_back(cantus_tempo[cantus_id][i]);
		}
		else {
			// Add to cc_len
			cc_len[cc_len.size()-1] += cantus_len[cantus_id][i];
		}
	}
	// Check cc_len
	int last_len = cc_len[0];
	for (int i = 1; i < cc_len.size(); ++i) {
		if (cc_len[i] != last_len) {
			// Send log
			CString est;
			est.Format("Warning: cantus notes usually are all of same length. Cantus #%d has non-uniform length at note %d", cantus_id+1, i);
			WriteLog(1, est);
		}
	}
}

// Detect npm and explode notes into uniform length notes
void CGenCA2::ExplodeCP() {
	// Detect minimum note length for each voice
	min_vlen.clear();
	max_vlen.clear();
	sus_count = 0;
	int cur_len = 0;
	int n;
	min_vlen.resize(av_cnt, INT_MAX);
	max_vlen.resize(av_cnt, 0);
	for (int v = 0; v < av_cnt; ++v) {
		int prev_note = cpoint[cantus_id][v][0];
		cur_len = cantus_len[cantus_id][0];
		for (s = 1; s < cpoint[cantus_id][v].size(); ++s) {
			n = cpoint[cantus_id][v][s];
			if (n != prev_note) {
				if (cur_len < min_vlen[v]) min_vlen[v] = cur_len;
				if (cur_len > max_vlen[v]) max_vlen[v] = cur_len;
				cur_len = 0;
				prev_note = n;
			}
			cur_len += cantus_len[cantus_id][s];
		}
		if (cur_len < min_vlen[v]) min_vlen[v] = cur_len;
	}
	// Check that cantus has longer notes than other voice
	if (min_vlen[cfv] < min_vlen[cpv]) {
		CString est;
		est.Format("Warning: minimum counterpoint note length %d is longer than minimum cantus note %d of %s cantus #%d. Changed cantus to %s", 
			min_vlen[cpv], min_vlen[cfv], cantus_high?"higher":"lower", cantus_id+1, (!cantus_high) ? "higher" : "lower");
		WriteLog(5, est);
		// Change cantus type
		cantus_high = !cantus_high;
		cpv = !cpv;
		cfv = !cfv;
	}
	if (min_vlen[cpv] * 4 == min_vlen[cfv] && species == 5) {
		// Do not increase cpv length - it will decrease two times this way
		// Increase minimum cantus length
		min_vlen[cfv] *= 2;
		max_vlen[cfv] *= 2;
		// Grow counterpoint two times
		for (int i = 0; i < cantus_len[cantus_id].size(); ++i) {
			cantus_len[cantus_id][i] *= 2;
		}
	}
	// Calculate npm
	npm = max(1, min_vlen[cfv] / min_vlen[cpv]);
	// Save old cantus
	vector<vector<int>> cc_old2 = cpoint[cantus_id];
	vector<float> tempo_old = cantus_tempo[cantus_id];
	cc_len = cantus_len[cantus_id];
	cantus_len[cantus_id].clear();
	cantus_tempo[cantus_id].clear();
	for (int v = 0; v < av_cnt; ++v) cpoint[cantus_id][v].clear();
	int steps;
	// Explode cpoint
	int ln;
	for (s = 0; s < cc_old2[0].size(); ++s) {
		steps = cc_len[s] / min_vlen[cpv];
		// Do not explode last measure
		ln = min_vlen[cpv];
		if (s == cc_old2[0].size() - 1) {
			steps = 1;
			ln = cc_len[s];
		}
		for (int i = 0; i < steps; ++i) {
			for (int v = 0; v < av_cnt; ++v) {
				cpoint[cantus_id][v].push_back(cc_old2[v][s]);
			}
			cantus_tempo[cantus_id].push_back(tempo_old[s]);
			cantus_len[cantus_id].push_back(ln);
		}
	}
	// Calculate sus count
	for (int i = 1; i < cpoint[cantus_id][cpv].size(); ++i) {
		if (cpoint[cantus_id][cpv][i] == cpoint[cantus_id][cpv][i - 1] && 
			cpoint[cantus_id][cfv][i] != cpoint[cantus_id][cfv][i - 1]) ++sus_count;
	}
	// Detect start pause
	fn = 0;
	for (int i = 0; i < cpoint[cantus_id][cpv].size(); ++i) {
		if (!cpoint[cantus_id][cpv][i]) {
			fn = i + 1;
		}
		else break;
	}
	// Copy notes over start pause
	for (int i = 0; i < fn; ++i) {
		cpoint[cantus_id][cpv][i] = cpoint[cantus_id][cpv][fn];
	}
	// Check that counterpoint did not become shorter
	if (cpoint[cantus_id][0].size() < cc_old2[0].size()) {
		CString est;
		est.Format("Warning: ExplodeCP returned shorter voice than initial for %s cantus #%d (initial=%d, new=%d)",
			cantus_high ? "higher" : "lower", cantus_id+1, cc_old2[0].size(), cpoint[cantus_id][0].size());
		WriteLog(5, est);
	}
}

void CGenCA2::LinkCpPauses() {
	int v2;
	for (int c = 0; c < cpoint.size(); ++c) {
		for (int v = 0; v < cpoint[c].size(); ++v) {
			if (v) v2 = 0;
			else v2 = 1;
			for (int i = 1; i < cpoint[c][v].size(); ++i) {
				// Detect wrong voice slur
				if (!cpoint[c][v][i - 1] && cpoint[c][v][i] == cpoint[c][v2][i - 1]) {
					// Move slur to correct voice
					cpoint[c][v][i - 1] = cpoint[c][v][i];
					cpoint[c][v2][i - 1] = 0;
				}
			}
		}
	}
}

void CGenCA2::DetectSpecies() {
	species_detected = 0;
	// Do not detect if cantus has uneven note length
	if (max_vlen[cfv] != min_vlen[cfv]) return;
	if (sus_count > cpoint[cantus_id][cpv].size() / 4 && (min_vlen[cpv] == min_vlen[cfv] || min_vlen[cpv]*2 == min_vlen[cfv])) species_detected = 4;
	else if (min_vlen[cpv] == min_vlen[cfv]) species_detected = 1;
	else if (min_vlen[cpv] * 2 == min_vlen[cfv]) species_detected = 2;
	else if (min_vlen[cpv] * 4 == min_vlen[cfv]) species_detected = 3;
	else if (min_vlen[cpv] * 8 == min_vlen[cfv]) species_detected = 5;
	// Write log
	CString est;
	est.Format("Detected species %d for counterpoint #%d (%s)",
		species_detected, cantus_id, cantus_high ? "high" : "low");
	WriteLog(0, est);
	// Check wrong text
	if (species) {
		if (species != species_detected) {
			CString est;
			est.Format("Counterpoint #%d (%s) looks like species %d, but species %d was specified in MIDI file",
				cantus_id, cantus_high ? "high" : "low", species_detected, species);
			WriteLog(5, est);
		}
	}
	else species = species_detected;
}

void CGenCA2::Generate() {
	CString st;
	int fn0 = fn;
	int s_len2 = s_len;
	InitCP();
	SetStatusText(8, "MIDI file: " + fname_from_path(midi_file));
	LoadCP(midi_file);
	LinkCpPauses();
	if (cpoint.size() < 1) return;
	// Saved t_generated
	int t_generated2 = 0;
	cantus_id = -1;
	for (int i = 0; i < cpoint.size(); i++) {
		++cantus_id;
		if (cpoint[cantus_id].size() != av_cnt) {
			st.Format("Error: need %d voices in counterpoint. Loaded only %d instead in counterpoint %d. Skipping this counterpoint.",
				av_cnt, cpoint[cantus_id].size(), cantus_id+1);
			WriteLog(1, st);
			continue;
		}
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
		long long time_start = CGLib::time();
		// Add line
		linecolor[step] = MakeColor(255, 0, 0, 0);
		LoadCantusHigh();
		LoadSpecies();
		// Check level
		if ((cantus_high && cpoint[i][1][0] == 0) || (!cantus_high && cpoint[i][0][0] == 0)) {
			st.Format("Warning: Cantus starts with a pause (%s cantus #%d). Changed to %s",
				cantus_high ? "high" : "low", cantus_id + 1, (!cantus_high) ? "high" : "low");
			WriteLog(5, st);
			cantus_high = !cantus_high;
		}
		if (cantus_high) {
			cfv = 1;
			cpv = 0;
		}
		else {
			cfv = 0;
			cpv = 1;
		}
		ExplodeCP();
		DetectSpecies();
		fn0 = fn;
		// Get key
		acc = cpoint[i];
		GetCPKey();
		if (tonic_cur == -1) continue;
		CalcCcIncrement();
		// Show imported melody
		MergeCantus();
		real_len = accumulate(cantus_len[i].begin(), cantus_len[i].end(), 0);
		dpenalty_cur = 0;
		// Create pause
		FillPause(step0, floor((real_len + 1) / 8 + 1) * 8, 0);
		FillPause(step0, floor((real_len + 1) / 8 + 1) * 8, 1);
		FillPause(step0, floor((real_len + 1) / 8 + 1) * 8, 2);
		FillPause(step0, floor((real_len + 1) / 8 + 1) * 8, 3);
		cpv = cfv;
		fn = 0;
		show_note_scan_range = 0;
		SelectRuleSet(cf_rule_set);
		ScanCantus(tEval, cfv, &(m_cc));
		// Go forward
		show_note_scan_range = 1;
		t_generated = step;
		Adapt(step0, step - 1);
		t_sent = t_generated;
		t_generated2 = t_generated;
		// Load cantus voice
		cc_len = cantus_len[i];
		cc_tempo = cantus_tempo[i];
		acc[cfv] = cpoint[cantus_id][cfv];
		c_len = acc[cfv].size();
		ac[cfv].resize(c_len);
		apc[cfv].resize(c_len);
		apcc[cfv].resize(c_len);
		FailDiatonic(ac[cfv], acc[cfv], 0, c_len, minor_cur);
		GetPitchClass(ac[cfv], acc[cfv], apc[cfv], apcc[cfv], 0, c_len);
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
		GetSourceRange(cpoint[i][cpv]);
		step0 = step;
		fn = fn0;
		ScanCP(tEval, 0);
		key_eval.Empty();
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
		// Init animation
		acycle = 0;
		correct_start_time = CGLib::time();
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
