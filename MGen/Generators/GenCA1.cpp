#include "../stdafx.h"
#include "GenCA1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CGenCA1::CGenCA1()
{
	v_cnt = 2;
	track_id[0] = 2;
	track_id[1] = 1;
}

CGenCA1::~CGenCA1()
{
}

void CGenCA1::LoadConfigLine(CString* sN, CString* sV, int idata, float fdata)
{
	LoadVar(sN, sV, "midi_file", &midi_file);
	CheckVar(sN, sV, "corrections", &corrections);
	CheckVar(sN, sV, "pre_bad", &pre_bad);
	CheckVar(sN, sV, "post_bad", &post_bad);
	CheckVar(sN, sV, "step_penalty", &step_penalty);
	CheckVar(sN, sV, "pitch_penalty", &pitch_penalty);

	CGenCF1::LoadConfigLine(sN, sV, idata, fdata);
}

void CGenCA1::GetCantusKey(vector <int> &cc)
{
	CString kst1, kst2;
	int t1, t2;
	// Major
	int c1 = GetCantusKey2(cc, t1, kst1, 0);
	// Minor
	int c2 = GetCantusKey2(cc, t2, kst2, 1);
	key_eval.Format("major confidence %d [%s], minor confidence %d [%s]", c1, kst1, c2, kst2);
	if (c1 == 0 && c2 == 0) {
		tonic_cur = -1;
		WriteLog(3, "Key evaluation: " + key_eval);
		return;
	}
	// Cope with same confidence
	while (c1 == c2) {
		c1 = randbw(0, 100);
		c2 = randbw(0, 100);
	}
	// Select best confidence
	if (c1 > c2) {
		tonic_cur = t1;
		minor_cur = 0;
	}
	else {
		tonic_cur = t2;
		minor_cur = 1;
	}
}

int CGenCA1::GetCantusKey2(vector <int> &cc, int &tonic_cur, CString &ext_st, int minor_cur)
{
	int c_len = cc.size();
	int key_miss[12];
	CString cst, kst, st2;
	// Create melody string for log
	for (int x = 0; x < min(c_len, 30); x++) {
		st2.Format("%d", cc[x] / 12);
		cst += NoteName[cc[x] % 12] + st2 + " ";
	}
	// Cycle all keys and count miss
	for (int i = 0; i < 12; i++) {
		key_miss[i] = 0;
		// Cycle all notes
		for (int x = 0; x < c_len; x++) {
			if (minor_cur) {
				// Check all possible pitches for minor
				if (!m_diatonic_full[(cc[x] - i) % 12]) key_miss[i]++;
			}
			else {
				// Check normal pitches for major
				if (!diatonic[(cc[x] - i) % 12]) key_miss[i]++;
			}
			// Check if diatonic repeats
			if (x && CC_C(cc[x], i, minor_cur) == CC_C(cc[x - 1], i, minor_cur) && abs(cc[x] - cc[x-1]) == 1) key_miss[i]++;
		}
	}
	// Find minimum miss
	int min_key = 0;
	int min_miss = c_len;
	for (int i = 0; i < 12; i++) {
		if (key_miss[i] < min_miss) {
			min_miss = key_miss[i];
			min_key = i;
		}
	}
	// If no key selected
	if (min_miss > 0) {
		ext_st.Format("Cannot detect key due to chromatic alterations");
		tonic_cur = -1;
		return 0;
	}
	// Count best keys
	vector <int> keys;
	int key_count = 0;
	for (int i = 0; i < 12; i++) {
		if (key_miss[i] == min_miss) {
			key_count++;
			keys.push_back(i);
			tonic_cur = i;
		}
	}
	// Create keys string for log
	for (int x = 0; x < keys.size(); x++) {
		if (kst != "") kst += " ";
		kst += NoteName[keys[x]];
	}
	// Check if only one key
	if (key_count == 1) {
		ext_st.Format("Single key %s selected", NoteName[tonic_cur]);
		if (cc[c_len - 1] % 12 == tonic_cur) {
			return 500 - key_count;
		}
		else if (cc[0] % 12 == tonic_cur) {
			return 400 - key_count;
		}
		else return 300 - key_count;
	}
	// If multiple keys and random_key set
	else if (random_key) {
		tonic_cur = keys[randbw(0, keys.size() - 1)];
		ext_st.Format("Ambiguous %d keys (%s) resolved to %s (random)", keys.size(), kst, NoteName[tonic_cur]);
		return 100 - keys.size();
	}
	// If multiple keys and random_key not set
	else {
		// Find accepted tonic same as last note
		for (int i = 0; i < keys.size(); i++) {
			if (cc[c_len - 1] % 12 == keys[i]) {
				tonic_cur = keys[i];
				ext_st.Format("Ambiguous %d keys (%s) resolved to %s as last note", keys.size(), kst, NoteName[tonic_cur]);
				return 400 - keys.size();
			}
		}
		// Find accepted tonic same as first note
		for (int i = 0; i < keys.size(); i++) {
			if (cc[0] % 12 == keys[i]) {
				tonic_cur = keys[i];
				ext_st.Format("Ambiguous %d keys (%s) resolved to %s as first note", keys.size(), kst, NoteName[tonic_cur]);
				return 300 - keys.size();
			}
		}
		// If nothing found, return random of accepted
		tonic_cur = keys[randbw(0, keys.size() - 1)];
		ext_st.Format("Ambiguous %d keys (%s) resolved to %s (random)", keys.size(), kst, NoteName[tonic_cur]);
		return 100 - keys.size();
	}
}

void CGenCA1::CreateScanMatrix(int i) {
	CString st, st2;
	// Clear scan matrix
	smatrixc = 0;
	smatrix.resize(c_len);
	fill(smatrix.begin(), smatrix.end(), 0);
	// Search each note
	for (int x = 0; x < c_len; x++) {
		// Search each flag
		if (anflagsc[av][x] > 0) for (int f = 0; f < anflagsc[av][x]; f++) {
			// Find prohibited flag
			if (accept[anflags[av][x][f]] == 0) {
				// Create matrix window
				int pos1 = x - pre_bad;
				int pos2 = x + post_bad;
				// Increase post_bad if long leap
				//if (x > 0 && abs(cantus[i][x - 1] - cantus[i][x]) > 7) pos2 = x + 12;
				//if (x < c_len - 1 && abs(cantus[i][x + 1] - cantus[i][x]) > 7) pos2 = x + 12;
				// Do not rescan first and last step
				if (pos1 < 0) pos1 = 0;
				if (pos2 > c_len - 1) pos2 = c_len - 1;
				// Set smatrix values
				for (int z = pos1; z <= pos2; z++) {
					if (smatrix[z] == 0) smatrixc++;
					smatrix[z] = 1;
				}
			}
		}
	}
	st2 = "";
	for (int x = 0; x < c_len; x++) {
		st.Format("%d ", smatrix[x]);
		st2 += st;
	}
	CString est;
	est.Format("Scan matrix for cantus %d created with %d steps of %d: %s", i + 1, smatrixc, c_len, st2);
	WriteLog(3, est);
}

void CGenCA1::SendCorrections(int i, milliseconds time_start) {
	CString st, st2;
	// Count penalty
	long cnum = clib.size();
	dpenalty.resize(cnum);
	for (int x = 0; x < cnum; x++) {
		dpenalty[x] = 0;
		for (int z = 0; z < c_len; z++) {
			int dif = abs(cantus[i][z] - clib[x][z]);
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
				ScanCantus(tEval, 0, &(cantus[i]));
				step -= real_len + 1;
			}
			// Get cantus
			cc = clib[cids[x]];
			dpenalty_cur = dpenalty[cids[x]];
			// Clear penalty
			dpenalty[cids[x]] = MAX_PENALTY;
			// Show result
			ScanCantus(tEval, 1, &(cc));
			// Go back
			step -= real_len + 1;
			if (step < 0) break;
			// Add lining
			int pos = step;
			for (int z = 0; z < c_len; z++) {
				if (cantus[i][z] != clib[cids[x]][z]) {
					for (int g = 0; g < cc_len[z]; g++) {
						lining[pos + g][0] = 1;
					}
				}
				pos += cc_len[z];
			}
			// Go forward
			step += real_len + 1;
			Adapt(step - real_len - 1, step - 1);
			t_generated = step;
			t_sent = t_generated;
		}
	}
	milliseconds time_stop = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	// Send log
	CString est;
	est.Format("Sent corrections in %d ms to step %d with rp/dp/variants/lib: %s", time_stop - time_start, step, st2);
	WriteLog(3, est);
}

void CGenCA1::Generate()
{
	CString st;
	int s_len2 = s_len;
	InitCantus();
	SetStatusText(8, "MIDI file: " + fname_from_path(midi_file));
	LoadCantus(midi_file);
	if (cantus.size() < 1) return;
	// Saved t_generated
	int t_generated2 = 0; 
	for (int i = 0; i < cantus.size(); i++) {
		++cantus_id;
		// Check limit
		if (t_generated >= t_cnt) {
			WriteLog(3, "Reached t_cnt steps. Generation stopped");
			break;
		}
		st.Format("Analyzing: %d of %d", cantus_id, cantus.size());
		SetStatusText(3, st);
		if (need_exit) break;
		if (step < 0) step = 0;
		milliseconds time_start = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
		// Add line
		linecolor[step] = Color(255, 0, 0, 0);
		// Get key
		GetCantusKey(cantus[i]);
		if (tonic_cur == -1) continue;
		CalcCcIncrement();
		// Show imported melody
		cc_len = cantus_len[i];
		cc_tempo = cantus_tempo[i];
		real_len = accumulate(cantus_len[i].begin(), cantus_len[i].end(), 0);
		dpenalty_cur = 0;
		ScanCantus(tEval, 0, &(cantus[i]));
		key_eval = "";
		// Check if cantus was shown
		if (t_generated2 == t_generated) continue;
		t_generated2 = t_generated;
		// Fill pauses if no results generated
		FillPause(step - real_len - 1, real_len, 1);
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
