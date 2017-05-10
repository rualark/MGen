#include "../stdafx.h"
#include "GenCA1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CGenCA1::CGenCA1()
{
	v_cnt = 2;
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
	int t1, t2;
	// Major
	int c1 = GetCantusKey2(cc, t1, 0);
	// Minor
	int c2 = GetCantusKey2(cc, t2, 1);
	CString* est = new CString;
	est->Format("Key selection: major confidence %d, minor confidence %d", c1, c2);
	WriteLog(3, est);
	if (c1 == 0 && c2 == 0) {
		tonic_cur = -1;
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

int CGenCA1::GetCantusKey2(vector <int> &cc, int &tonic_cur, int minor_cur)
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
			if (x && CC_C(cc[x], i, minor_cur) == CC_C(cc[x - 1], i, minor_cur)) key_miss[i]++;
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
		CString* est = new CString;
		est->Format("Error: due to chromatic alterations, cannot detect key (m%d) for melody %s", minor_cur, cst);
		WriteLog(3, est);
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
		CString* est = new CString;
		est->Format("Single key %s selected for melody %s (m%d)", NoteName[tonic_cur], cst, minor_cur);
		WriteLog(3, est);
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
		CString* est = new CString;
		est->Format("Ambiguous %d keys m%d (%s) resolved to %s (random) in melody %s", keys.size(), minor_cur, kst, NoteName[tonic_cur], cst);
		WriteLog(3, est);
		return 100 - keys.size();
	}
	// If multiple keys and random_key not set
	else {
		// Find accepted tonic same as last note
		for (int i = 0; i < keys.size(); i++) {
			if (cc[c_len - 1] % 12 == keys[i]) {
				tonic_cur = keys[i];
				CString* est = new CString;
				est->Format("Ambiguous %d keys m%d (%s) resolved to %s as last note in melody %s", keys.size(), minor_cur, kst, NoteName[tonic_cur], cst);
				WriteLog(3, est);
				return 400 - keys.size();
			}
		}
		// Find accepted tonic same as first note
		for (int i = 0; i < keys.size(); i++) {
			if (cc[0] % 12 == keys[i]) {
				tonic_cur = keys[i];
				CString* est = new CString;
				est->Format("Ambiguous %d keys m%d (%s) resolved to %s as first note in melody %s", keys.size(), minor_cur, kst, NoteName[tonic_cur], cst);
				WriteLog(3, est);
				return 300 - keys.size();
			}
		}
		// If nothing found, return random of accepted
		tonic_cur = keys[randbw(0, keys.size() - 1)];
		CString* est = new CString;
		est->Format("Ambiguous %d keys m%d (%s) resolved to %s (random) in melody %s", keys.size(), minor_cur, kst, NoteName[tonic_cur], cst);
		WriteLog(3, est);
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
		if (nflagsc[x] > 0) for (int f = 0; f < nflagsc[x]; f++) {
			// Find prohibited flag
			if (accept[nflags[x][f]] == 0) {
				// Create matrix window
				int pos1 = x - pre_bad;
				int pos2 = x + post_bad;
				// Increase post_bad if long leap
				if (x > 0 && abs(cantus[i][x - 1] - cantus[i][x]) > 7) pos2 = x + 12;
				if (x < c_len - 1 && abs(cantus[i][x + 1] - cantus[i][x]) > 7) pos2 = x + 12;
				// Do not rescan first and last step
				if (pos1 < 1) pos1 = 1;
				if (pos2 > c_len - 2) pos2 = c_len - 2;
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
	CString* est = new CString;
	est->Format("Scan matrix for cantus %d created with %d steps of %d: %s", i + 1, smatrixc, c_len, st2);
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
			st.Format("%.0f/%.0f/%d ", rpenalty_min, dpenalty_min, cids.size());
			st2 += st;
			// Clear penalty
			dpenalty[cids[x]] = MAX_PENALTY;
			// Show initial melody again if this is not first iteration
			if (ccount > 1) {
				ScanCantus(&(cantus[i]), 0, 0);
				step -= real_len + 1;
			}
			// Get cantus
			cc = clib[cids[x]];
			// Show result
			ScanCantus(&(cc), 0, 1);
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
	CString* est = new CString;
	est->Format("Sent corrections in %d ms to step %d with rp/dp/variants: %s", time_stop - time_start, step, st2);
	WriteLog(3, est);
}

void CGenCA1::Generate()
{
	CString st, st2;
	int s_len2 = s_len;
	InitCantus();
	LoadCantus(midi_file);
	if (cantus.size() < 1) return;
	// Transpose corrected voice up for display
	int t_generated2 = 0; // Saved t_generated
	for (int i = 0; i < cantus.size(); i++) {
		if (need_exit) break;
		if (step < 0) step = 0;
		milliseconds time_start = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
		// Add line
		linecolor[step] = Color(255, 0, 0, 0);
		// Get key
		GetCantusKey(cantus[i]);
		if (tonic_cur == -1) continue;
		// Show imported melody
		cc_len = cantus_len[i];
		cc_tempo = cantus_tempo[i];
		real_len = accumulate(cantus_len[i].begin(), cantus_len[i].end(), 0);
		ScanCantus(&(cantus[i]), 0, 0);
		// Check if cantus was shown
		if (t_generated2 == t_generated) continue;
		t_generated2 = t_generated;
		// If no corrections needed
		if (!corrections) {
			v_cnt = 1;
			// Go forward
			t_generated = step;
			Adapt(step - real_len - 1, step - 1);
			t_sent = t_generated;
			continue;
		}
		step -= real_len + 1;
		// Fill pauses if no results generated
		FillPause(step, real_len, 1);
		CreateScanMatrix(i);
		// Count additional variables
		CountOff(step, step + real_len);
		CountTime(step, step + real_len);
		UpdateNoteMinMax(step, step + real_len);
		UpdateTempoMinMax(step, step + real_len);
		if (smatrixc > fullscan_max) {
			SWA(i, 1);
		}
		else {
			s_len = s_len2;
			clib.clear();
			rpenalty.clear();
			rpenalty_min = 0;
			// Full scan marked notes
			ScanCantus(&(cantus[i]), 1, 0);
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
}
