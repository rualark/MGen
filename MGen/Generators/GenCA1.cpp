// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
	CheckVar(sN, sV, "corrections", &corrections, 0);
	CheckVar(sN, sV, "pre_bad", &pre_bad, 0);
	CheckVar(sN, sV, "post_bad", &post_bad, 0);
	CheckVar(sN, sV, "step_penalty", &step_penalty, 0);
	CheckVar(sN, sV, "pitch_penalty", &pitch_penalty, 0);

	CGenCF1::LoadConfigLine(sN, sV, idata, fdata);
}

void CGenCA1::GetCPKey()
{
	CString kst1, kst2;
	int t1, t2;
	// Major
	int c1 = GetCPKey2(t1, kst1, 0);
	// Minor
	int c2 = GetCPKey2(t2, kst2, 1);
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

void CGenCA1::GetCPKey3(vector <int> &key_miss, int &min_miss, int &min_key, int minor_cur2, int diatonic_repeat_check)
{
	key_miss.clear();
	key_miss.resize(12);
	// Cycle all keys and count miss
	for (int i = 0; i < 12; i++) {
		key_miss[i] = 0;
		// Cycle all notes
		for (int v = 0; v < av_cnt; v++) {
			for (int x = 0; x < c_len; x++) {
				if (minor_cur2) {
					// Check all possible pitches for minor
					if (!m_diatonic_full[(acc[v][x] - i) % 12]) key_miss[i]++;
				}
				else {
					// Check normal pitches for major
					if (!diatonic[(acc[v][x] - i) % 12]) key_miss[i]++;
				}
				// Check if diatonic repeats
				if (diatonic_repeat_check) {
					if (x && CC_C(acc[v][x], i, minor_cur2) == CC_C(acc[v][x - 1], i, minor_cur2) && abs(acc[v][x] - acc[v][x - 1]) == 1) key_miss[i]++;
				}
			}
		}
	}
	// Find minimum miss
	min_key = 0;
	min_miss = c_len;
	for (int i = 0; i < 12; i++) {
		if (key_miss[i] < min_miss) {
			min_miss = key_miss[i];
			min_key = i;
		}
	}
}

int CGenCA1::GetCPKey2(int &tonic_cur2, CString &ext_st, int minor_cur2)
{
	c_len = acc[0].size();
	vector<int> key_miss;
	int min_key = 0;
	int min_miss = c_len;
	CString cst, kst, st2;
	// Create melody string for log
	for (int x = 0; x < min(c_len, 30); x++) {
		st2.Format("%d", acc[0][x] / 12);
		cst += NoteName[acc[0][x] % 12] + st2 + " ";
	}
	GetCPKey3(key_miss, min_miss, min_key, minor_cur2, 1);
	// If no key selected run again without checking for repeating diatonic steps
	if (min_miss > 0) {
		GetCPKey3(key_miss, min_miss, min_key, minor_cur2, 0);
	}
	// If no key selected
	if (min_miss > 0) {
		ext_st.Format("Cannot detect key due to chromatic alterations");
		tonic_cur2 = -1;
		return 0;
	}
	// Count best keys
	vector <int> keys;
	int key_count = 0;
	for (int i = 0; i < 12; i++) {
		if (key_miss[i] == min_miss) {
			key_count++;
			keys.push_back(i);
			tonic_cur2 = i;
		}
	}
	// Create keys string for log
	for (int x = 0; x < keys.size(); x++) {
		if (!kst.IsEmpty()) kst += " ";
		kst += NoteName[keys[x]];
	}
	// Check if only one key
	if (key_count == 1) {
		ext_st.Format("Single key %s selected", NoteName[tonic_cur2]);
		if (acc[0][c_len - 1] % 12 == tonic_cur2) {
			return 500 - key_count;
		}
		else if (acc[0][0] % 12 == tonic_cur2) {
			return 400 - key_count;
		}
		else return 300 - key_count;
	}
	// If multiple keys and random_key set
	else if (random_key) {
		tonic_cur2 = keys[randbw(0, keys.size() - 1)];
		ext_st.Format("Ambiguous %zu keys (%s) resolved to %s (random)", keys.size(), kst, NoteName[tonic_cur2]);
		return 100 - keys.size();
	}
	// If multiple keys and random_key not set
	else {
		// Find accepted tonic same as last note
		for (int i = 0; i < keys.size(); i++) {
			if (acc[0][c_len - 1] % 12 == keys[i]) {
				tonic_cur2 = keys[i];
				ext_st.Format("Ambiguous %zu keys (%s) resolved to %s as last note", keys.size(), kst, NoteName[tonic_cur2]);
				return 400 - keys.size();
			}
		}
		// Find accepted tonic same as first note
		for (int i = 0; i < keys.size(); i++) {
			if (acc[0][0] % 12 == keys[i]) {
				tonic_cur2 = keys[i];
				ext_st.Format("Ambiguous %zu keys (%s) resolved to %s as first note", keys.size(), kst, NoteName[tonic_cur2]);
				return 300 - keys.size();
			}
		}
		// If nothing found, return random of accepted
		tonic_cur2 = keys[randbw(0, keys.size() - 1)];
		ext_st.Format("Ambiguous %zu keys (%s) resolved to %s (random)", keys.size(), kst, NoteName[tonic_cur2]);
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
	for (int x = fn; x < c_len; x++) {
		// Search each flag
		if (anflagsc[cpv][x] > 0) for (int f = 0; f < anflagsc[cpv][x]; f++) {
			// Find prohibited flag
			if (accept[anflags[cpv][x][f]] == 0) {
				// Create matrix window
				int pos1 = x - pre_bad;
				int pos2 = x + post_bad;
				// Increase post_bad if long leap
				//if (x > 0 && abs(cantus[i][x - 1] - cantus[i][x]) > 7) pos2 = x + 12;
				//if (x < c_len - 1 && abs(cantus[i][x + 1] - cantus[i][x]) > 7) pos2 = x + 12;
				// Do not rescan first and last step
				if (pos1 < fn) pos1 = fn;
				if (pos2 > c_len - 1) pos2 = c_len - 1;
				// Set smatrix values
				for (int z = pos1; z <= pos2; z++) {
					if (smatrix[z] == 0) smatrixc++;
					smatrix[z] = 1;
				}
			}
		}
	}
	st2.Empty();
	for (int x = 0; x < c_len; x++) {
		st.Format("%d ", smatrix[x]);
		st2 += st;
	}
	CString est;
	est.Format("Scan matrix for cantus %d created with %d steps of %d: %s", i + 1, smatrixc, c_len, st2);
	WriteLog(3, est);
}

void CGenCA1::SendCorrections(int i, PmTimestamp time_start) {
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
	st2.Empty();
	for (int m = 0; m < corrections; ++m) {
		// Find minimum penalty
		cids.clear();
		float dpenalty_min2 = MAX_PENALTY;
		for (int x = 0; x < cnum; x++) if (dpenalty[x] < dpenalty_min2) dpenalty_min2 = dpenalty[x];
		// Get all best corrections
		for (int x = 0; x < cnum; x++) if (dpenalty[x] == dpenalty_min2) {
			cids.push_back(x);
		}
		if (!cids.size() || dpenalty_min2 == MAX_PENALTY) break;
		// Shuffle cids
		long long seed = CGLib::time();
		::shuffle(cids.begin(), cids.end(), default_random_engine(seed));
		for (int x = 0; x < cids.size(); x++) {
			ccount++;
			if (ccount > corrections) break;
			// Write log
			st.Format("%.0f/%.0f/%zu/%zu ", rpenalty_min, dpenalty_min2, cids.size(), clib.size());
			st2 += st;
			// Show initial melody again if this is not first iteration
			if (ccount > 1) {
				dpenalty_cur = 0; 
				ScanCantus(tEval, 0, &(cantus[i]));
				step = step0;
			}
			// Get cantus
			m_cc = clib[cids[x]];
			dpenalty_cur = dpenalty[cids[x]]; //-V519
			// Clear penalty
			dpenalty[cids[x]] = MAX_PENALTY;
			// Show result
			ScanCantus(tEval, 1, &(m_cc));
			// Go back
			step = step0;
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
			step = t_generated;
			Adapt(step0, step - 1);
			t_generated = step;
			t_sent = t_generated;
		}
	}
	long long time_stop = CGLib::time();
	// Send log
	CString est;
	est.Format("Sent corrections in %d ms to step %d with rp/dp/variants/lib: %s", time_stop - time_start, step, st2);
	WriteLog(3, est);
}

void CGenCA1::ParseExpect() {
	int fl;
	enflags.clear();
	int max_i = cantus_incom[cantus_id].size();
	if (!max_i) return;
	// Continue if there are lyrics
	vector<CString> ast;
	enflags.resize(max_i);
	for (int i = 0; i < max_i; ++i) {
		if (!cantus_incom[cantus_id][i].IsEmpty()) {
			Tokenize(cantus_incom[cantus_id][i], ast, ",");
			for (int n = 0; n < ast.size(); ++n) {
				fl = atoi(ast[n]);
				if (fl) enflags[i].push_back(fl);
			}
		}
	}
}

void CGenCA1::ConfirmExpect() {
	int found, fl;
	int max_x = enflags.size();
	for (int x = 0; x < max_x; ++x) if (enflags[x].size()) {
		for (int e = 0; e < enflags[x].size(); ++e) {
			fl = enflags[x][e];
			// Do not confirm rule violation if rule checking is disabled
			//if (accept[fl] == -1) continue;
			found = 0;
			for (int f = 0; f < anflagsc[cpv][x]; ++f) {
				if (anflags[cpv][x][f] == fl) {
					found = 1;
					break;
				}
			}
			if (!found) {
				// Do not show errors if flag ignored and not testing
				if (m_testing || accept[fl] != -1) {
					CString est;
					est.Format("Expected flag not confirmed: [%d] %s (%s) at %d:%d %s",
						fl, RuleName[rule_set][fl], SubRuleName[rule_set][fl], cantus_id + 1, x + 1, midi_file);
					WriteLog(5, est);
					if (m_testing) AppendLineToFile("autotest\\expect.log", est + "\n");
				}
			}
			else if (debug_level > 0) {
				CString est;
				est.Format("Expected flag confirmed: [%d] %s (%s) at %d:%d %s",
					fl, RuleName[rule_set][fl], SubRuleName[rule_set][fl], cantus_id + 1, x + 1, midi_file);
				WriteLog(0, est);
				if (m_testing) AppendLineToFile("autotest\\expect.log", est + "\n");
			}
		}
	}
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
	cantus_id = -1;
	for (int i = 0; i < cantus.size(); i++) {
		++cantus_id;
		// Check limit
		if (t_generated >= t_cnt) {
			WriteLog(3, "Reached t_cnt steps. Generation stopped");
			break;
		}
		st.Format("Analyzing: %d of %zu", cantus_id+1, cantus.size());
		SetStatusText(3, st);
		if (need_exit) break;
		if (step < 0) step = 0;
		step0 = step;
		long long time_start = CGLib::time();
		// Add line
		linecolor[step] = Color(255, 0, 0, 0);
		// Get key
		acc.resize(1);
		acc[0] = cantus[i];
		GetCPKey();
		if (tonic_cur == -1) continue;
		CalcCcIncrement();
		// Show imported melody
		cc_len = cantus_len[i];
		cc_tempo = cantus_tempo[i];
		real_len = accumulate(cantus_len[i].begin(), cantus_len[i].end(), 0);
		dpenalty_cur = 0;
		c_len = cantus[i].size();
		GetSourceRange(cantus[i]);
		// Set enflags before evaluation
		ScanCantus(tEval, 0, &(cantus[i]));
		// Clear enflags after evaluation
		ParseExpect();
		ConfirmExpect();
		key_eval.Empty();
		// Check if cantus was shown
		if (t_generated2 == t_generated) continue;
		t_generated2 = t_generated;
		// Fill pauses if no results generated
		FillPause(step0, step-step0, 1);
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
		step1 = step;
		step = step0;
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
			step = t_generated;
			Adapt(step0, step - 1);
			t_sent = t_generated;
		}
	}
	st.Format("Analyzed %d of %zu", cantus_id+1, cantus.size());
	SetStatusText(3, st);
	ShowStuck();
}
