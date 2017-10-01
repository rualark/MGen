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
	CheckClibSize();
	// Find minimum penalty
	int ccount = 0;
	// Cycle through all best matches
	st2.Empty();
	for (int p = 0; p < corrections; p++) {
		// Find minimum penalty
		cids.clear();
		int dpenalty_min = MAX_PENALTY;
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
			st.Format("%.0f/%d/%.0f/%zu/%zu/%d:%d ", rpenalty_min, dpenalty_min,
				rpenalty_source, cids.size(), clib.size(), swa_full, scan_full);
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
			++cor_sent;
			dp_sum += dpenalty_cur;
			rp_sum += rpenalty_min;
			swa_sum += swa_len;
			if (scan_full && (method != mSWA || swa_full)) ++cor_full;
			if (!rpenalty_min) ++cor_rp0;
			EmulateSASCP();
			//LogCantus("scor", cantus_id + 1, acc[cpv].size(), acc[cpv]);
			// Go back
			step = step0;
			if (step < 0) break;
			ShowLiningCP(acc[cpv]);
			// Go forward
			step = t_generated;
			Adapt(step0, step - 1);
			t_sent = t_generated;
		}
	}
	long long time_stop = CGLib::time();
	// Send log
	cor_log.Format("Sent corrections #%d in %d ms to %d:%d with rp/dp/srp/variants/lib/full: %s",
		cantus_id + 1, time_stop - time_start, step0 / 8 + 1, step0 % 8 + 1, st2);
	WriteLog(3, cor_log);
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
			if (!i) {
				cc_len.push_back(cantus_len[cantus_id][i] + fn * cantus_len[cantus_id][0]);
			}
			else {
				cc_len.push_back(cantus_len[cantus_id][i]);
			}
			cc_tempo.push_back(cantus_tempo[cantus_id][i]);
		}
		else {
			// Add to cc_len
			cc_len[cc_len.size()-1] += cantus_len[cantus_id][i];
		}
	}
	// Check cc_len
	for (int i = 1; i < cc_len.size(); ++i) {
		if (cc_len[i] != cc_len[0]) {
			// Send log
			CString est;
			est.Format("Warning: cantus notes usually are all of same length. Cantus #%d has non-uniform length at note %d", cantus_id+1, i);
			WriteLog(1, est);
		}
	}
}

void CGenCA2::ShrinkCantus2() {
	int l, min_len, max_len;
	// Detect minimum note length
	min_len = INT_MAX;
	max_len = 1;
	// Get lengths
	for (s = 0; s < cc_len.size(); ++s) {
		if (cc_len[s] < min_len) min_len = cc_len[s];
		if (cc_len[s] > max_len) max_len = cc_len[s];
	}
	// Check lengths
	// Shrink cantus
	for (s = 0; s < cc_len.size(); ++s) {
		l = cc_len[s] / min_len;
		//cc_tempo[s] = cc_tempo[s] * l / cc_len[s];
		cc_len[s] = l;
	}
}

void CGenCA2::ShrinkCP() {
	int l, min_len, max_len;
	for (int i = 0; i < cpoint.size(); ++i) {
		min_len = vmin(cantus_len[i]);
		// Shrink cp
		for (s = 0; s < cantus_len[i].size(); ++s) {
			l = cantus_len[i][s] / min_len;
			cantus_tempo[i][s] = cantus_tempo[i][s] * l / cantus_len[i][s];
			cantus_len[i][s] = l;
		}
	}
}

void CGenCA2::GetVlen() {
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
}

// Detect npm and explode notes into uniform length notes
void CGenCA2::ExplodeCP() {
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
			cantus_tempo[cantus_id][i] *= 2;
		}
	}
	// Calculate npm
	npm = max(1, min_vlen[cfv] / min_vlen[cpv]);
	// Save old cantus
	vector<vector<int>> cc_old2 = cpoint[cantus_id];
	vector<float> tempo_old = cantus_tempo[cantus_id];
	vector<CString> incom_old = cantus_incom[cantus_id];
	cc_len = cantus_len[cantus_id];
	cantus_len[cantus_id].clear();
	cantus_tempo[cantus_id].clear();
	cantus_incom[cantus_id].clear();
	for (int v = 0; v < av_cnt; ++v) cpoint[cantus_id][v].clear();
	int steps;
	// Explode cpoint
	int ln;
	fn = 0;
	for (s = 0; s < cc_old2[0].size(); ++s) {
		steps = cc_len[s] / min_vlen[cpv];
		// Do not explode last measure
		ln = min_vlen[cpv];
		//if (s == cc_old2[0].size() - 1) {
		//	steps = 1;
		//	ln = cc_len[s];
		//}
		// Detect starting pause
		if (!cc_old2[cpv][s]) {
			fn += steps;
		}
		else for (int i = 0; i < steps; ++i) {
			for (int v = 0; v < av_cnt; ++v) {
				cpoint[cantus_id][v].push_back(cc_old2[v][s]);
			}
			cantus_tempo[cantus_id].push_back(tempo_old[s]);
			cantus_len[cantus_id].push_back(ln);
			if (incom_old.size() > s) {
				if (i) cantus_incom[cantus_id].push_back("");
				else cantus_incom[cantus_id].push_back(incom_old[s]);
			}
		}
	}
	// Calculate sus count
	for (int i = 1; i < cpoint[cantus_id][cpv].size(); ++i) {
		if (cpoint[cantus_id][cpv][i] == cpoint[cantus_id][cpv][i - 1] && 
			cpoint[cantus_id][cfv][i] != cpoint[cantus_id][cfv][i - 1]) ++sus_count;
	}
	// Check that counterpoint did not become shorter
	if (cpoint[cantus_id][0].size() < cc_old2[0].size() - fn) {
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
		species_detected, cantus_id + 1, cantus_high ? "high" : "low");
	WriteLog(0, est);
	// Check wrong text
	if (species) {
		if (species != species_detected) {
			CString est;
			est.Format("Counterpoint #%d (%s) looks like species %d, but species %d was specified in MIDI file",
				cantus_id + 1, cantus_high ? "high" : "low", species_detected, species);
			if (species_detected == 4 && species == 2) WriteLog(1, est);
			else if (species_detected == 2 && species == 4) WriteLog(1, est);
			else WriteLog(5, est);
		}
	}
	else species = species_detected;
	// Set midi out multiplier
	if (species_applied) return;
	species_applied = 1;
	if (species == 1) {
		midifile_out_mul *= 8;
	}
	if (species == 2) {
		midifile_out_mul *= 4;
	}
	if (species == 3) {
		midifile_out_mul *= 2;
	}
	if (species == 4) {
		midifile_out_mul *= 4;
	}
}

// Move pause from cf voice
void CGenCA2::FixUnisonPause() {
	for (s = 0; s < cpoint[cantus_id][0].size(); ++s) {
		if (!cpoint[cantus_id][0][s] && min_vlen[0] > min_vlen[1])
			swap(cpoint[cantus_id][0][s], cpoint[cantus_id][1][s]);
		if (!cpoint[cantus_id][1][s] && min_vlen[1] > min_vlen[0])
			swap(cpoint[cantus_id][0][s], cpoint[cantus_id][1][s]);
	}
}

int CGenCA2::CheckStartPause() {
	if (species >= 2 && species <= 5 && fn == 0) {
		if (!accept[273]) {
			CString est;
			est.Format("Counterpoint %s #%d does not have starting pause, which is currently prohibited for species %d. This counterpoint will not be corrected. Please fix midi file or allow no starting pause in rules if you want this counterpoint to be corrected",
				cantus_high ? "high" : "low", cantus_id + 1, species);
			WriteLog(1, est);
			return 1;
		}
	}
	else if ((species == 3 && fn > 1) || (species == 5 && fn != 0 && fn != 2 && fn != 4)) {
		CString est;
		est.Format("Counterpoint %s #%d has wrong starting pause (%d/%d) in species %d. This counterpoint will not be corrected. Please fix midi file if you want this counterpoint to be corrected",
			cantus_high?"high":"low", cantus_id + 1, fn, npm, species);
		WriteLog(1, est);
		return 1;
	}
	return 0;
}

void CGenCA2::Generate() {
	//CString test_st = "62 62 62 62 69 69 66 66 67 67 67 67 66 66 64 64 66 66 66 66 66 66 67 67 66 66 66 66 69 69 69 69 71 71 69 69 67 67 76 76 73 73 73 73 71 71 69 69 74 74 73 73 71 71 69 69 67 67 71 71 73 73 73 73 74";
	//test_cc.resize(65);
	//StringToVector(&test_st, " ", test_cc);

	CString st;
	int fn0 = fn;
	int s_len2 = s_len;
	if (error) return;
	InitCP();
	SetStatusText(8, "MIDI file: " + fname_from_path(midi_file));
	LoadCP(midi_file);
	//LinkCpPauses();
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
		//FixUnisonPause();
		LoadCantusHigh();
		LoadSpecies();
		// Check level
		if ((cantus_high && cpoint[i][1][0] == 0) || (!cantus_high && cpoint[i][0][0] == 0)) {
			if (specified_high) {
				st.Format("Cantus starts with a pause (%s cantus #%d). As it was specified in midi, moved pause to other voice",
					cantus_high ? "high" : "low", cantus_id + 1);
				WriteLog(0, st);
				swap(cpoint[cantus_id][0][0], cpoint[cantus_id][1][0]);
			}
			else {
				st.Format("Warning: Cantus starts with a pause (%s cantus #%d). Changed to %s",
					cantus_high ? "high" : "low", cantus_id + 1, (!cantus_high) ? "high" : "low");
				WriteLog(5, st);
				cantus_high = !cantus_high;
			}
		}
		if (cantus_high) {
			cfv = 1;
			cpv = 0;
		}
		else {
			cfv = 0;
			cpv = 1;
		}
		// Check if starting pause is greater or equal to cantus note length
		if (!cpoint[cantus_id][cpv][0] && cpoint[cantus_id][cfv][1] != cpoint[cantus_id][cfv][0]) {
			st.Format("Warning: Starting pause in counterpoint is greater or equal to cantus note length (%s cpoint #%d). Skipping counterpoint",
				cantus_high ? "high" : "low", cantus_id + 1);
			WriteLog(1, st);
			continue;
		}
		GetVlen();
		ExplodeCP();
		ShrinkCP();
		DetectSpecies();
		fn0 = fn;
		// Get key
		acc = cpoint[i];
		GetCPKey();
		if (tonic_cur == -1) continue;
		CalcCcIncrement();
		// Show imported melody
		MergeCantus();
		ShrinkCantus2();
		//LogCantus("ca2", cantus_id, cpoint[cantus_id][cpv].size(), cpoint[cantus_id][cpv]);
		real_len = accumulate(cantus_len[i].begin(), cantus_len[i].end(), 0) + fn * cantus_len[i][0];
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
		step00 = step0;
		fn = fn0;
		ScanCP(tEval, 0);
		ParseExpect();
		ConfirmExpect();
		EmulateSASCP();
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
		if (!corrections || !smatrixc || 
			(m_testing && time() - gen_start_time > (m_test_sec - ANALYZE_RESERVE) * 1000) ||
			CheckStartPause()) {
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
		// Save source rpenalty
		rpenalty_source = rpenalty_cur;
		InitCorAck();
		if (method == mSWA) {
			SWACP(i, 1);
			// Check if we have results
			if (clib.size()) {
				SendCorrectionsCP(i, time_start);
				SaveCorAck();
			}
			else {
				// Go forward
				step = t_generated;
				Adapt(step0, step - 1);
				t_sent = t_generated;
			}
		}
		if (cor_ack) {
			method = mScan;
			FillPause(step, step - step0, 0);
			FillPause(step, step - step0, 1);
			step0 = step;
		}
		if (method == mScan) {
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
			// Check if we have results
			if (clib.size()) {
				SendCorrectionsCP(i, time_start);
				SaveCorAck();
			}
			else {
				// Go forward
				step = t_generated;
				Adapt(step0, step - 1);
				t_sent = t_generated;
			}
		}
		CorAck();
		if (need_exit) break;
	}
	st.Format("Analyzed %d of %d", cantus_id+1, cpoint.size());
	SetStatusText(3, st);
	ShowStuck();
	LogPerf();
}
