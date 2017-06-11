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
				step -= real_len + 1;
			}
			// Get cantus
			scpoint[cpv] = clib[cids[x]];
			dpenalty_cur = dpenalty[cids[x]];
			// Clear penalty
			dpenalty[cids[x]] = MAX_PENALTY;
			// Show result
			ScanCP(tEval, 2);
			// Go back
			step -= real_len + 1;
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

// Do not calculate dpenalty (dp = 0). Calculate dpenalty (dp = 1).
void CGenCA2::SWACP(int i, int dp) {
	CString st;
	milliseconds time_start = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	s_len = 1;
	// Save source rpenalty
	float rpenalty_source = rpenalty_cur;
	long cnum;
	// Save cantus only if its penalty is less or equal to source rpenalty
	rpenalty_min = rpenalty_cur;
	dpenalty_min = MAX_PENALTY;
	acc = cpoint[i];
	int a;
	for (a = 0; a < approximations; a++) {
		// Save previous minimum penalty
		int rpenalty_min_old = rpenalty_min;
		int dpenalty_min_old = dpenalty_min;
		// Clear before scan
		clib.clear();
		clib_vs.clear();
		rpenalty.clear();
		dpenalty.clear();
		dpenalty_min = MAX_PENALTY;
		clib.push_back(acc[cpv]);
		clib_vs.Insert(acc[cpv]);
		rpenalty.push_back(rpenalty_min_old);
		dpenalty.push_back(dpenalty_min_old);
		// Sliding Windows Approximation
		scpoint = acc;
		ScanCP(tCor, -1);
		dpenalty_min = MAX_PENALTY;
		cnum = clib.size();
		if (cnum) {
			if (dp) {
				// Count dpenalty for results, where rpenalty is minimal
				dpenalty.resize(cnum);
				for (int x = 0; x < cnum; x++) if (rpenalty[x] <= rpenalty_min) {
					dpenalty[x] = 0;
					for (int z = 0; z < c_len; z++) {
						int dif = abs(cpoint[i][1][z] - clib[x][z]);
						if (dif) dpenalty[x] += step_penalty + pitch_penalty * dif;
					}
					if (dpenalty[x] && dpenalty[x] < dpenalty_min) dpenalty_min = dpenalty[x];
					//st.Format("rp %.0f, dp %0.f: ", rpenalty[x], dpenalty[x]);
					//AppendLineToFile("temp.log", st);
					//LogCantus(clib[x]);
				}
			}
			// Get all best corrections
			cids.clear();
			for (int x = 0; x < cnum; x++) if (rpenalty[x] <= rpenalty_min && (!dp || dpenalty[x] <= dpenalty_min)) {
				cids.push_back(x);
			}
			if (cids.size()) {
				// Get random cid
				int cid = randbw(0, cids.size() - 1);
				// Get random cantus to continue
				acc[cpv] = clib[cids[cid]];
			}
		}
		// Send log
		if (debug_level > 0) {
			CString est;
			est.Format("SWA%d #%d: rp %.0f from %.0f, dp %.0f, cnum %ld", s_len, a, rpenalty_min, rpenalty_source, dpenalty_min, cnum);
			WriteLog(3, est);
		}
		if (acc[cfv].size() > 60) {
			st.Format("SWA%d attempt: %d", s_len, a);
			SetStatusText(4, st);
		}
		if (dp) {
			// Abort SWA if dpenalty and rpenalty not decreasing
			if (rpenalty_min >= rpenalty_min_old && dpenalty_min >= dpenalty_min_old) {
				if (s_len >= swa_steps)	break;
				++s_len;
			}
		}
		else {
			// Abort SWA if rpenalty zero or not decreasing
			if (!rpenalty_min) break;
			if (rpenalty_min >= rpenalty_min_old) {
				if (s_len >= swa_steps) {
					// Record SWA stuck flags
					for (int x = 0; x < max_flags; ++x) {
						if (best_flags[x]) ++ssf[x];
					}
					break;
				}
				else ++s_len;
			}
		}
	}
	// Log
	milliseconds time_stop = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	CString est;
	est.Format("Finished SWA%d #%d: rp %.0f from %.0f, dp %.0f, cnum %ld (in %d ms): " + GetStuck(),
		s_len, a, rpenalty_min, rpenalty_source, dpenalty_min, cnum, time_stop - time_start);
	WriteLog(3, est);
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
		cc_len = cantus_len[i];
		cc_tempo = cantus_tempo[i];
		real_len = accumulate(cantus_len[i].begin(), cantus_len[i].end(), 0);
		dpenalty_cur = 0;
		// Create pause
		FillPause(step, real_len, 1);
		FillPause(step, real_len, 2);
		FillPause(step, real_len, 3);
		cpv = cfv;
		SelectRuleSet(cf_rule_set);
		ScanCantus(tEval, 0, &(cpoint[i][cfv]));
		// Show cantus id
		st.Format("Counterpoint %d. ", cantus_id + 1);
		comment[step - real_len - 1][0] = st + comment[step - real_len - 1][0];
		// Go forward
		t_generated = step;
		Adapt(step - real_len - 1, step - 1);
		t_sent = t_generated;
		t_generated2 = t_generated;
		// Load first voice
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
		//GetSourceRange();
		// Get cantus interval
		GetMelodyInterval(cpoint[i][cfv], 0, cpoint[i][cfv].size(), cf_nmin, cf_nmax);
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
			step += real_len + 1;
			Adapt(step - real_len - 1, step - 1);
			t_generated = step;
			t_sent = t_generated;
		}
	}
	st.Format("Analyzed %d of %d", cantus_id+1, cpoint.size());
	SetStatusText(3, st);
	ShowStuck();
}