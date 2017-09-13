// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../stdafx.h"
#include "GenCP1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CGenCP1::CGenCP1() {
	av_cnt = 2;
	v_cnt = 2;
	track_id[0] = 2;
	track_id[1] = 1;
}

CGenCP1::~CGenCP1() {
}

void CGenCP1::LoadConfigLine(CString * sN, CString * sV, int idata, float fdata) {
	CheckVar(sN, sV, "cantus_id", &cantus_id2, 0);
	CheckVar(sN, sV, "accept_cantus_rechoose", &accept_cantus_rechoose, 0, 1);
	CheckVar(sN, sV, "notes_per_measure", &npm, 1, 8);
	CheckVar(sN, sV, "start_pause", &fn, 0, 7);
	CheckVar(sN, sV, "species", &species, 1, 5);

	if (*sN == "species") {
		if (species == 1) {
			npm = 1;
			fn = 0;
		}
		if (species == 2) {
			npm = 2;
			fn = 1;
		}
		if (species == 3) {
			npm = 4;
			fn = 1;
		}
		if (species == 4) {
			npm = 2;
			fn = 1;
		}
		if (species == 5) {
			npm = 8;
			fn = 2;
		}
	}

	CGenCA1::LoadConfigLine(sN, sV, idata, fdata);
}

int CGenCP1::InitCP() {
	InitGen();
	ac.resize(av_cnt);
	acc.resize(av_cnt);
	acc_old.resize(av_cnt);
	apc.resize(av_cnt);
	apcc.resize(av_cnt);
	asmooth.resize(av_cnt);
	aleap.resize(av_cnt);
	aslur.resize(av_cnt);
	anflags.resize(av_cnt);
	anflagsc.resize(av_cnt);
	return error;
}

void CGenCP1::MakeNewCP() {
	// Set pitch limits
	if (cantus_high) {
		for (int i = 0; i < c_len; ++i) {
			max_cc[i] = acc[cfv][i] - min_between;
			min_cc[i] = max(acc[cfv][i] - burst_between, cf_nmax - sum_interval);
		}
	}
	else {
		for (int i = 0; i < c_len; ++i) {
			min_cc[i] = acc[cfv][i] + min_between;
			max_cc[i] = min(acc[cfv][i] + burst_between, cf_nmin + sum_interval);
		}
	}
	// Recalibrate 
	for (int i = 0; i < c_len; ++i) {
		if (minor_cur) {
			if (!m_diatonic_full[(min_cc[i] + 12 - tonic_cur) % 12]) ++min_cc[i];
		}
		else {
			if (!diatonic[(min_cc[i] + 12 - tonic_cur) % 12]) ++min_cc[i];
		}
	}
	// Convert limits to diatonic
	for (int i = 0; i < c_len; ++i) {
		min_c[i] = CC_C(min_cc[i], tonic_cur, minor_cur);
		max_c[i] = CC_C(max_cc[i], tonic_cur, minor_cur);
	}
	CalculateCcOrder(acc_old[cpv], 0, c_len);
	FillCantus(cc_id, 0, c_len, 0);
	FillCantus(acc[cpv], 0, c_len, cc_order);
}

void CGenCP1::SingleCPInit() {
	// Copy cantus
	acc = scpoint;
	// Get diatonic steps from chromatic
	for (int v = 0; v < acc.size(); ++v) {
		for (int i = fn; i < c_len; ++i) {
			ac[v][i] = CC_C(acc[v][i], tonic_cur, minor_cur);
		}
	}
	if (!is_animating) {
		// Save value for future use;
		acc_old = acc;
		// Calculate limits
		if (cantus_high) {
			for (int i = fn; i < c_len; ++i) {
				min_cc[i] = max(acc[cfv][i] - burst_between,
					max(src_nmin, max(cf_nmax - sum_interval, acc[cpv][i] - correct_range)));
				max_cc[i] = min(acc[cfv][i] - min_between,
					min(max(src_nmax, min_cc[i] + (src_nmax - src_nmin)),
						max(acc[cpv][i] + correct_range, min_cc[i] + 2 * correct_range)));
			}
		}
		else {
			for (int i = fn; i < c_len; ++i) {
				max_cc[i] = min(acc[cfv][i] + burst_between,
					min(src_nmax, min(cf_nmin + sum_interval, acc[cpv][i] + correct_range)));
				min_cc[i] = max(acc[cfv][i] + min_between,
					max(min(src_nmin, max_cc[i] - (src_nmax - src_nmin)),
						min(acc[cpv][i] - correct_range, max_cc[i] - 2 * correct_range)));
			}
		}
		// Recalibrate 
		for (int i = 0; i < c_len; ++i) {
			if (minor_cur) {
				if (!m_diatonic_full[(min_cc[i] + 12 - tonic_cur) % 12]) ++min_cc[i];
			}
			else {
				if (!diatonic[(min_cc[i] + 12 - tonic_cur) % 12]) ++min_cc[i];
			}
		}
		// Convert limits to diatonic
		for (int i = fn; i < c_len; ++i) {
			min_c[i] = CC_C(min_cc[i], tonic_cur, minor_cur);
			max_c[i] = CC_C(max_cc[i], tonic_cur, minor_cur);
		}
		sp1 = fn;
		sp2 = c_len;
		ep1 = max(0, sp1 - 1);
		ep2 = c_len;
	}
	// Clear flags
	++accepted3;
	fill(flags.begin(), flags.end(), 0);
	flags[0] = 1;
	for (int i = 0; i < ep2; ++i) {
		anflagsc[cpv][i] = 0;
	}
	// Matrix scan
	if (task != tEval) {
		// Exit if no violations
		if (!smatrixc) return;
		// Create map
		smap.resize(smatrixc);
		int map_id = 0;
		for (int i = fn; i < c_len; ++i) if (smatrix[i]) {
			smap[map_id] = i;
			++map_id;
		}
		// Shuffled smap
		//unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		//::shuffle(smap.begin(), smap.end(), default_random_engine(seed));
		sp1 = 0;
		sp2 = sp1 + s_len; // End of search window
		if (sp2 > smatrixc) sp2 = smatrixc;
		// Record window
		wid = 0;
		wpos1[wid] = sp1;
		wpos2[wid] = sp2;
		CalculateCcOrder(acc_old[cpv], 0, c_len);
		// Add last note if this is last window
		// End of evaluation window
		if (method == mScan) {
			ep2 = GetMaxSmap() + 1;
			if (sp2 == smatrixc) ep2 = c_len;
			// Clear scan steps
			FillCantusMap(cc_id, smap, 0, smatrixc, 0);
			FillCantusMap(acc[cpv], smap, 0, smatrixc, cc_order);
			// Can skip flags - full scan must remove all flags
			dpenalty_step.clear();
			dpenalty_step.resize(c_len, 0);
		}
		// For sliding windows algorithm evaluate whole melody
		if (method == mSWA) {
			ep2 = c_len;
			// Cannot skip flags - need them for penalty if cannot remove all flags
			skip_flags = 0;
			// Clear scan steps of current window
			FillCantusMap(cc_id, smap, sp1, sp2, 0);
			FillCantusMap(acc[cpv], smap, sp1, sp2, cc_order);
			// Prepare dpenalty
			dpenalty_outside_swa = CalcDpenaltyCP(cpoint[cantus_id][cpv], acc[cpv], fn, sp1-1) + 
				CalcDpenaltyCP(cpoint[cantus_id][cpv], acc[cpv], sp2, c_len - 1);
		}
		// Minimum element
		ep1 = max(0, GetMinSmap() - 1);
		// Minimal position in array to cycle
		pp = sp2 - 1;
		p = smap[pp];
	}
	else {
		// For single cantus scan - cannot skip flags - must show all
		skip_flags = 0;
		// SAS emulator
		if (fixed_ep2) {
			ep2 = fixed_ep2;
		}
	}
}

void CGenCP1::MultiCPInit() {
	MakeNewCP();
	// First pause
	for (int i = 0; i < fn; ++i) acc[cpv][i] = acc[cpv][fn];
	sp1 = fn; // Start of search window
	sp2 = sp1 + s_len; // End of search window
	if (sp2 > c_len - 1) sp2 = c_len - 1;
	// Record window
	wid = 0;
	wpos1[wid] = sp1;
	wpos2[wid] = sp2;
	ep1 = max(0, sp1 - 1);
	ep2 = sp2; // End of evaluation window
	p = sp2 - 1; // Minimal position in array to cycle
}

void CGenCP1::ScanCPInit() {
	// Get cantus size
	if (task != tGen) c_len = scpoint[0].size();
	ScanInit();
	// Resize global vectors
	for (int i = 0; i < av_cnt; ++i) {
		ac[i].resize(c_len); // cantus (diatonic)
		acc[i].resize(c_len); // cantus (chromatic)
		acc_old[i].resize(c_len); // Cantus diatonic saved for SWA
		apc[i].resize(c_len);
		apcc[i].resize(c_len);
		aleap[i].resize(c_len);
		asmooth[i].resize(c_len);
		aslur[i].resize(c_len);
	}
	rpos.resize(c_len);
	ivl.resize(c_len);
	ivlc.resize(c_len);
	civl.resize(c_len);
	civlc.resize(c_len);
	tivl.resize(c_len);
	motion.resize(c_len);
	beat.resize(c_len);
	sus.resize(c_len);
}

int CGenCP1::SendCP() {
	int step00 = step;
	int pause_len = 0;
	CString st, rpst;
	int pos = 0, plen;
	int v, x1;
	Sleep(sleep_ms);
	long long time_start = CGLib::time();
	long long time_stop;
	len_export.resize(c_len);
	coff_export.resize(c_len);
	if (!mutex_animate.try_lock_for(chrono::milliseconds(5000))) {
		WriteLog(5, "Critical error: ResizeVectors mutex timed out");
	}
	for (int av = 0; av < av_cnt; ++av) {
		CreateLinks(ac[av]);
		MakeMacc(acc[av]);
		pos = step;
		// Sent voice is the same as acc voice
		v = svoice + av;
		if (av == cpv) {
			plen = cc_len[0] * fn;
			FillPause(pos, plen, v);
			pos += plen;
			x1 = fn;
		}
		else {
			x1 = 0;
		}
		MakeLenExport(acc[av], x1, av);
		// Copy cantus to output
		if (step + real_len >= t_allocated) ResizeVectors(t_allocated * 2);
		for (int x = x1; x < ep2; ++x) {
			SendLyrics(pos, v, av, x);
			for (int i = 0; i < cc_len[x]; ++i) {
				if (av == cpv) {
					// Set color
					color[pos + i][v] = color_noflag;
				}
				SendNotes(pos, i, v, av, x, acc[av]);
				SendNgraph(pos, i, v, x);
				SendComment(pos, v, av, x, i);
			}
			pos += cc_len[x];
		}

		pause_len = SendPause(pos, v);
		InterpolateNgraph(v, step, pos);
		// Merge notes
		MergeNotes(step, pos - 1, v);
		MakeBellDyn(v, step, pos - 1, 40, 100, 20);
	}
	step = pos + pause_len;
	FixLen(step00, step - 1);
	// Count additional variables
	CountOff(step00, step - 1);
	CountTime(step00, step - 1);
	UpdateNoteMinMax(step00, step - 1);
	UpdateTempoMinMax(step00, step - 1);
	mutex_animate.unlock();
	// Increment cantus_sent only if is not animating
	if (!is_animating) ++cantus_sent;
	// Create rule penalty string
	if (!skip_flags) {
		for (int x = 0; x < max_flags; ++x) {
			if (!accept[x] && fpenalty[x]) {
				st.Format("%d=%.0f", x, fpenalty[x]);
				if (!rpst.IsEmpty()) rpst += ", ";
				rpst += st;
			}
		}
		st.Format("%.0f", rpenalty_cur);
		if (!rpst.IsEmpty()) rpst = st + " (" + rpst + ")";
		else rpst = st;
		if (rpenalty_cur == MAX_PENALTY) rpst = "0";
	}
	if (task == tGen) {
		if (!shuffle) {
			Adapt(step00, step - 1);
		}
		// If  window-scan
		st.Format("#%d\nCantus: %s\nSpecies: %d\nRule penalty: %s", cantus_sent, cantus_high?"high":"low", species, rpst);
		AddMelody(step00, step - 1, svoice + cpv, st);
	}
	else if (task == tEval) {
		if (m_algo_id == 101) {
			// If RSWA
			st.Format("#%d\nCantus: %s\nSpecies: %d\nRule penalty: %s", cantus_sent, cantus_high ? "high" : "low", species, rpst);
		}
		else {
			if (key_eval.IsEmpty()) {
				// If SWA
				st.Format("#%d (from MIDI file %s)\nCantus: %s\nSpecies: %d\nRule penalty: %s\nDistance penalty: %d", cantus_id+1, midi_file, cantus_high ? "high" : "low", species, rpst, dpenalty_cur);
			}
			else {
				// If evaluating
				st.Format("#%d (from MIDI file %s)\nCantus: %s\nSpecies: %d\nRule penalty: %s\nKey selection: %s", cantus_id+1, midi_file, cantus_high ? "high" : "low", species, rpst, key_eval);
			}
		}
		AddMelody(step00, step - 1, svoice + cpv, st);
	}
	if (debug_level > 2) {
		time_stop = CGLib::time();
		st.Format("SendCP run time %d ms", time_stop - time_start);
		//WriteLog(1, st);
	}
	// Send
	t_generated = step;
	if (task == tGen) {
		if (!shuffle) {
			// Add line
			linecolor[t_sent] = MakeColor(255, 0, 0, 0);
			t_sent = t_generated;
		}
	}
	st.Format("Sent: %ld (ignored %ld)", cantus_sent, cantus_ignored);
	SetStatusText(0, st);
	// Check limit
	if (t_generated >= t_cnt) {
		WriteLog(3, "Reached t_cnt steps. Generation stopped");
		return 1;
	}
	return 0;
}

void CGenCP1::ReseedCP()
{
	CString st;
	MultiCPInit();
	// Allow two seed cycles for each accept
	seed_cycle = 0;
	++reseed_count;
	st.Format("Reseed: %d", reseed_count);
	SetStatusText(4, st);
}

int CGenCP1::FailAlteredInt2(int i, int c1, int c2, int flag) {
	if ((apcc[0][i] == c1 && apcc[1][i] == c2) || (apcc[0][i] == c2 && apcc[1][i] == c1)) FLAG2(flag, i);
	return 0;
}

// Fail vertical altered intervals
int CGenCP1::FailAlteredInt() {
	for (int i = fn; i < ep2; ++i) {
		if (FailAlteredInt2(i, 9, 8, 170)) return 1;
		if (FailAlteredInt2(i, 11, 10, 171)) return 1;
		if (FailAlteredInt2(i, 11, 8, 172)) return 1;
		if (FailAlteredInt2(i, 9, 3, 173)) return 1;
		if (FailAlteredInt2(i, 11, 3, 174)) return 1;
	}
	return 0;
}

int CGenCP1::FailCrossInt2(int i, int i_1, int c1, int c2, int flag) {
	if ((apcc[cfv][i_1] == c1 && apcc[cpv][i] == c2) || (apcc[cfv][i_1] == c2 && apcc[cpv][i] == c1)) FLAG2(flag, i)
	else if ((apcc[cpv][i_1] == c1 && apcc[cfv][i] == c2) || (apcc[cpv][i_1] == c2 && apcc[cfv][i] == c1)) FLAG2(flag, i_1);
	return 0;
}

// Fail cross relation altered intervals
int CGenCP1::FailCrossInt() {
	for (int s = fn+1; s < ep2; ++s) {
		s_1 = s - 1;
		if (FailCrossInt2(s, s_1, 9, 8, 164)) return 1;
		if (FailCrossInt2(s, s_1, 11, 10, 165)) return 1;
		if (FailCrossInt2(s, s_1, 11, 8, 166)) return 1;
		if (FailCrossInt2(s, s_1, 9, 3, 167)) return 1;
		if (FailCrossInt2(s, s_1, 11, 3, 168)) return 1;
		if (FailCrossInt2(s, s_1, 11, 5, 29)) return 1;
		if (FailCrossInt2(s, s_1, 2, 8, 29)) return 1;
	}
	return 0;
}

void CGenCP1::GetVIntervals() {
	// Calculate intervals
	for (int i = fn; i < ep2; ++i) {
		ivl[i] = ac[1][i] - ac[0][i];
		ivlc[i] = ivl[i] % 7;
		civl[i] = acc[1][i] - acc[0][i];
		civlc[i] = civl[i] % 12;
		//if (civlc[i] == 1 || civlc[i] == 2 || civlc[i] == 5 || civlc[i] == 6 || civlc[i] == 10 || civlc[i] == 11) tivl[i] = iDis;
		if (civlc[i] == 3 || civlc[i] == 4 || civlc[i] == 8 || civlc[i] == 9) tivl[i] = iIco;
		else if (civlc[i] == 7 || civlc[i] == 0) tivl[i] = iPco;
		else tivl[i] = iDis;
	}
}

int CGenCP1::FailVMotion() {
	int mtemp;
	int scontra = 0;
	int sdirect = 0;
	for (int i = fn; i < ep2; ++i) {
		if (i < ep2 - 1) {
			motion[i] = mStay;
			if (acc[cfv][i + 1] != acc[cfv][i] || acc[cpv][i + 1] != acc[cpv][i]) {
				mtemp = (acc[cfv][i + 1] - acc[cfv][i])*(acc[cpv][i + 1] - acc[cpv][i]);
				if (mtemp > 0) {
					motion[i] = mDirect;
					++sdirect;
				}
				else if (mtemp < 0) {
					motion[i] = mContrary;
					++scontra;
				}
				else motion[i] = mOblique;
			}
		}
	}
	// Check how many contrary if full melody analyzed
	if (ep2 == c_len) {
		if (scontra + sdirect) { //-V793
			int pcontra = (scontra * 100) / (scontra + sdirect);
			if (pcontra < contrary_min2) FLAG2(46, fn)
			else if (pcontra < contrary_min) FLAG2(35, fn);
		}
	}
	return 0;
}

int CGenCP1::FailSus() {
	int pre_end;
	for (ls = 0; ls < fli_size; ++ls) if (sus[ls] && (ls < fli_size-1 || ep2 == c_len)) {
		s = fli[ls];
		s2 = fli2[ls];
		// Flag suspension
		FLAG2(225, s); 
		// Check if sus starts from discord
		if (tivl[s] == iDis) FLAG2(224, s);
		// Check if sus ends before cantus
		pre_end = 1;
		if (s2 == ep2 - 1) pre_end = 0;
		else if (acc[cfv][sus[ls]] != acc[cfv][s2 + 1]) pre_end = 0;
		if (pre_end) {
			// Check if suspension second part is discord
			if (tivl[s2] != iDis) {}
			// Resolution to discord
			else if (tivl[s2+1] == iDis) FLAG2(220, s2)
			// Resolution by leap
			else if (aleap[cpv][s2]) FLAG2(221, s2)
			else {
				// Resolution up
				if (acc[cpv][s2 + 1] > acc[cpv][s2]) {
					// Allowed only for resolution of leading tone
					if (apcc[cpv][s2] == 11) FLAG2(222, s2)
					else FLAG2(219, s2);
				}
				// 9th to 8va
				if (ivlc[s2] == 1 && ivlc[s2 + 1] == 0) {
					if (ivl[s2] > 7) FLAG2(216, s2)
					// 2nd to unison
					else FLAG2(218, s2);
				}
				// 7th to 8va
				else if (ivlc[s2] == 6 && ivlc[s2 + 1] == 0) FLAG2(217, s2)
			}
		}
		else {
			last_note = -1;
			for (s = sus[ls]; s <= s2; ++s) {
				if (last_note != acc[cpv][s]) {
					last_note = acc[cpv][s];
					if (FailUnison()) return 1;
					if (FailDisSus()) return 1;
					if (FailPcoSus()) return 1;
				}
			}
		}
	}
	return 0;
}

int CGenCP1::FailDisSus() {
	// Discord
	if (tivl[s] == iDis) {
		FLAG2(83, s);
	}
	return 0;
}

int CGenCP1::FailUnison() {
	// Unison
	if (!civl[s]) {
		// Inside
		if (ls > 1 && ls < fli_size - 1) FLAG2(91, fli2[ls - 1]);
	}
	return 0;
}

int CGenCP1::FailDis() {
	// Discord
	if (tivl[s] == iDis) {
		// Downbeat
		if (rpos[ls] == pDownbeat) FLAG2(83, s)
		// Upbeat 
		else if (rpos[ls] == pOffbeat) FLAG2(83, s)
		else if (rpos[ls] == pLeap) FLAG2(187, s)
		else {
			FLAG2(169, s)
			// Check if discord is longer than neighboring consonance
			if (ls > 0 && llen[ls] > llen[ls - 1] && tivl[fli2[ls - 1]] != iDis) FLAG2(223, s)
			else if (ls < fli_size - 2 && llen[ls] > llen[ls + 1] && tivl[fli[ls + 1]] != iDis) FLAG2(223, s);
		}
	}
	return 0;
}

int CGenCP1::FailPcoSus() {
	// Perfect consonance
	if (tivl[s] == iPco) {
		// Prohibit combinatory
		if (civlc[s] == civlc[s-1]) FLAG2(85, s)
			// Prohibit different
		else if (tivl[s-1] == iPco) 
			FLAG2(86, s)
	}
	return 0;
}

int CGenCP1::FailPco() {
	// Perfect consonance
	if (tivl[s] == iPco) {
		// Prohibit parallel first - first (this is for sus notes, which starts are parallel)
		if (civl[s] == civl[fli[ls - 1]]) FLAG2(84, s)
			// Prohibit parallel last - first
		else if (civl[s] == civl[fli2[ls - 1]]) FLAG2(84, s)
		else {
			// Prohibit combinatory
			if (civlc[s] == civlc[fli2[ls - 1]]) FLAG2(85, s)
			// Prohibit different
			else if (tivl[fli2[ls - 1]] == iPco) FLAG2(86, s)
			// All other cases if previous interval is not pco
			// Direct movement to pco
			if (motion[fli2[ls - 1]] == mDirect) {
				// Stepwize
				if (abs(acc[1][s] - acc[1][s - 1]) < 3) {
					if (s2 == c_len - 1) {
						if (civlc[s] == 0) FLAG2(209, s)
						else FLAG2(208, s);
					}
					else {
						if (civlc[s] == 0) FLAG2(211, s)
						else FLAG2(210, s);
					}
				}
				// Non-stepwize
				else {
					if (s2 == c_len - 1) {
						if (civlc[s] == 0) FLAG2(213, s)
						else FLAG2(212, s);
					}
					else {
						if (civlc[s] == 0) FLAG2(215, s)
						else FLAG2(214, s);
					}
				}
			}
		}
		// Prohibit downbeats and culminations only if not last step
		if (ls < fli_size - 1) {
			if (beat[ls]) {
				// Prohibit culmination
				if (acc[cpv][s] == nmax || acc[cfv][s] == nmax) FLAG2(81, s);
			}
			else {
				// Prohibit downbeat culmination
				if (acc[cpv][s] == nmax || acc[cfv][s] == nmax) FLAG2(82, s)
					// Prohibit downbeat
				else FLAG2(80, s);
			}
		}
	}
	return 0;
}

void CGenCP1::GetRpos() {
	int sm1, sm2;
	// Main calculation
	rpos[0] = pDownbeat;
	for (ls = 1; ls < fli_size; ++ls) {
		s = fli[ls];
		s2 = fli2[ls];
		if (s % npm == 0) rpos[ls] = pDownbeat;
		else if (s > 0 && aleap[cpv][s - 1]) rpos[ls] = pLeap;
		else if (s2 < ep2-1 && aleap[cpv][s2]) rpos[ls] = pLeap;
		else {
			if (s > 0 && s2 < ep2 - 1 && ac[cpv][s - 1] == ac[cpv][s2 + 1]) rpos[ls] = pAux;
			else rpos[ls] = pPass;
		}
	}
	// Calculate cambiata
	int tlen;
	if (npm >= 4) {
		for (int ms = 0; ms < mli.size(); ++ms) {
			s = mli[ms];
			if (s >= ep2) break;
			ls = bli[s];
			// If last ls
			if (ls >= fli_size - 4) break;
			// If longer than measure (removed because it duplicates next test)
			//if (fli2[ls + 3] - fli[ls] >= npm) continue;
			tlen = llen[ls] + llen[ls + 1] + llen[ls + 2] + llen[ls + 3];
			// If total length does not equal measure
			if (tlen != npm) continue;
			// Double neighbor tone
			if (acc[cpv][fli[ls]] == acc[cpv][fli[ls + 3]] &&
				asmooth[cpv][fli[ls]] * asmooth[cpv][fli[ls + 2]] == 1) {
				rpos[ls + 1] = pAux;
				rpos[ls + 2] = pAux;
				rpos[ls + 3] = pOffbeat;
			}
			// Cambiata
			if (ac[cpv][fli[ls]] - 2 == ac[cpv][fli[ls + 3]] &&
				asmooth[cpv][fli[ls]] == -1 && asmooth[cpv][fli[ls + 2]] == 1) {
				rpos[ls + 1] = pAux;
			}
			// Inverted cambiata
			if (ac[cpv][fli[ls]] + 2 == ac[cpv][fli[ls + 3]] &&
				asmooth[cpv][fli[ls]] == 1 && asmooth[cpv][fli[ls + 2]] == -1) {
				rpos[ls + 1] = pAux;
			}
		}
	}
}

// Fail rhythm for species 3
int CGenCP1::FailRhythm3() {
	if (species != 3) return 0;
	for (ls = 0; ls < fli_size; ++ls) {
		s = fli[ls];
		// 1/4 syncope
		if (beat[ls] == 2 && llen[ls] > 1) FLAG4(235, s);
		// 1/2 after 1/4
		if (ls > 0 && beat[ls] && llen[ls] > 1 && llen[ls - 1] == 1) {
			if (s / npm >= c_len / npm - 1) FLAG4(238, s)
			else if (sus[ls]) FLAG4(239, s)
			else FLAG4(240, s);
		}
		// Non-uniform starting rhythm
		if (s / npm == 0 && llen[ls] > 1) FLAG4(254, s);
	}
	return 0;
}

// Fail rhythm for species 5
int CGenCP1::FailRhythm5() {
	if (species != 5) return 0;
	// Rhythm id
	vector<int> rid;
	int rid_cur = 0;
	int count8;
	// Note lengths inside measure
	vector<int> l_len;
	l_len.resize(8);
	int s3;
	// Measure size in notes
	int m_size = 0;
	// Slurs at start and finish of measure (show length of slurred notes)
	int slur1 = 0;
	int slur2 = 0;
	// Full measure collected
	int full_measure = 0;
	// Position inside measure
	int pos = 0;
	// Length sum
	int suml = 0;
	int ls2 = 0;
	// Check pause length
	if (fn * 2 > npm) FLAG2(197, fn);
	for (int ms = 0; ms < mli.size(); ++ms) {
		s = mli[ms];
		if (s >= ep2) break;
		ls = bli[s];
		l_len.clear();
		slur1 = 0;
		slur2 = 0;
		// First pause
		if (s < fn) {
			l_len.push_back(fn);
		}
		// Build note lengths
		full_measure = 0;
		pos = 0;
		for (ls2 = ls; ls2 < fli_size; ++ls2) {
			// Do not process last note if not full melody generated
			if (ep2 != c_len && ls2 == fli_size - 1) {
				// Whole inside
				if (llen[ls2] >= 8 && !pos && !sus[ls2]) FLAG4(236, s)
				// 1/8 syncope
				else if (llen[ls2] > 1 && pos % 2) FLAG4(232, fli[ls2])
				// 1/4 syncope
				else if (llen[ls2] > 2 && pos % 4 == 2) FLAG4(235, fli[ls2])
				full_measure = 0;
				break;
			}
			s2 = fli[ls2];
			l_len.push_back(llen[ls2]);
			// Stop if out of measure
			if (fli2[ls2] >= s + npm - 1) {
				full_measure = 1;
				break;
			}
			pos += l_len.back();
		}
		// Check if there is nothing to analyze
		if (!l_len.size()) continue;
		// First note in measure with slur
		if (fli[ls] < s) {
			l_len[0] = min(8, (fli2[ls] - s + 1));
			slur1 = s - fli[ls];
		}
		// Last note in measure with slur
		if (full_measure && sus[ls2]) {
			l_len[l_len.size()-1] = min(8, llen[ls2] - (sus[ls2] - s2));
			slur2 = fli2[ls2] - sus[ls2] + 1;
		}
		// Full evaluation?
		if (ep2 == c_len) {
			// Last measure
			if (ms == mli.size() - 1) {
				// Check last whole note (really 1/8)
				if (l_len.size() == 1) break;
			}
		}
		// Set first rhythm id bit
		rid_cur = slur1?0:1;
		// Iterative rhythm checks
		count8 = 0;
		pos = 0;
		for (int lp = 0; lp < l_len.size(); ++lp) {
			s2 = s + pos;
			ls2 = bli[s2];
			// Last note
			if (ep2 == c_len && ls2 == fli_size - 1) {
				// Correct note length
				l_len[lp] = npm - pos;
				// Check 
				if (l_len[lp] == 1) FLAG4(253, s2)
				else if (l_len[lp] == 2) FLAG4(252, s2)
			}
			// Calculate rhythm id
			rid_cur += 1 << (pos + l_len[lp]);
			// Check 1/8 only if it is not last 1/8
			if (l_len[lp] == 1) {
				// Too many 1/8
				++count8;
				if (count8 > 2) FLAG4(255, s2);
				// 1/8 on leap
				if ((ls2 < fli_size - 1 && aleap[cpv][s2]) || (ls2 > 0 && aleap[cpv][s2 - 1])) FLAG4(88, s2);
				// 1/8 in first measure
				if (ms == 0) FLAG4(230, s2)
				// If second 1/8
				if (pos % 2) {
					// Isolated 1/8
					if (l_len[lp - 1] != 1) FLAG4(231, s2)
				}
				// If first 8th
				else {
					// 1/8 beats
					if (pos == 0) FLAG4(226, s2)
					else if (pos == 2) FLAG4(227, s2)
					else if (pos == 4) FLAG4(228, s2)
					else if (pos == 6) FLAG4(229, s2)
				}
			}
			else {
				// 1/8 syncope
				if (pos % 2) FLAG4(232, s2)
				// 1/4 syncope
				else if (l_len[lp] > 2 && pos == 2) FLAG4(235, s2)
				else if (l_len[lp] == 2 && pos == 6 && slur2) FLAG4(235, s2);
			}
			// Uneven starting rhythm
			if (!ms && lp>0 && l_len[lp] != l_len[lp-1]) FLAG4(254, s2);
			pos += l_len[lp];
		}
		// Check rhythm repeat
		if (full_measure) {
			if (rid.size()) {
				if (rid.back() == rid_cur) FLAG4(247, s);
			}
			rid.push_back(rid_cur);
		}
		// Check rhythm rules
		// First measure
		if (!ms) {
			// Uneven pause
			if (l_len.size() > 1 && l_len[0] == fn && l_len[0] != l_len[1]) FLAG4(237, s);
		}
		// Whole inside
		if (l_len[0] >= 8 && ms < mli.size() - 1) FLAG4(236, s)
		// 1/2.
		else if (l_len[0] == 6) FLAG4(233, s)
		else if (l_len.size() > 1 && l_len[1] == 6) FLAG4(234, fli[ls + 1])
		else if (l_len.size() > 2 && l_len[2] == 6) FLAG4(234, fli[ls + 2])
		// 1/2 after 1/4 or 1/8 in measure
		else if (full_measure && l_len[l_len.size() - 1] == 4 && l_len[0] != 4) {
			s3 = fli[ls + l_len.size() - 1];
			if (ms >= mli.size() - 2) FLAG4(238, s3)
			else if (slur2 != 0) FLAG4(239, s3)
			else FLAG4(240, s3);
		}
		// Many notes in measure
		if (l_len.size() == 5) FLAG4(245, s)
		else if (l_len.size() > 5) FLAG4(246, s);
		// Suspensions
		if (slur1 == 4 && l_len[0] == 2) FLAG4(241, s)
		else if (slur1 == 4 && l_len[0] == 4) FLAG4(242, s)
		else if (slur1 == 2) FLAG4(251, s)
		if (slur1 && l_len[0] == 6) FLAG4(243, s)
		if (slur1 == 6) FLAG4(244, s);
	}
	// Check last measure
	return 0;
}

int CGenCP1::FailPcoApart() {
	// Step index of last perfect consonance
	int pco5_last = -1000;
	int pco8_last = -1000;
	int skip_len;
	for (ls = 0; ls < fli_size; ++ls) {
		s = fli[ls];
		s2 = fli2[ls];
		// 5th apart
		if (civlc[s] == 7) {
			if (pco5_last > -1) {
				skip_len = s - pco5_last - llen[bli[pco5_last]];
				if (skip_len > 0 && skip_len < (pco_apart * npm) / 4) {
					if (!beat[ls]) FLAG2(250, s)
					else if ((acc[0][s] - acc[0][pco5_last])*
						(acc[1][s] - acc[1][pco5_last]) < 0) FLAG2(248, s)
					else if (rpos[ls] < 0 || rpos[bli[pco5_last]] < 0) FLAG2(249, s)
					else FLAG2(250, s);
				}
			}
			pco5_last = s;
		}
		// 8va apart
		if (civlc[s] == 0) {
			if (pco8_last > -1) {
				skip_len = s - pco8_last - llen[bli[pco8_last]];
				if (skip_len > 0 && skip_len < (pco_apart * npm) / 4) {
					if (!beat[ls]) FLAG2(250, s)
					else if ((acc[0][s] - acc[0][pco8_last])*
						(acc[1][s] - acc[1][pco8_last]) < 0) FLAG2(248, s)
						//else if (rpos[ls] < 0 || rpos[bli[pco8_last]] < 0) FLAG2(249, s)
					else FLAG2(250, s);
				}
			}
			pco8_last = s;
		}
	}
	return 0;
}

int CGenCP1::FailVIntervals() {
	// Number of sequential parallel imperfect consonances
	int pico_count = 0;
	// Check first step
	if (tivl[fn] == iDis) FLAG2(83, fn);
	for (ls = 1; ls < fli_size; ++ls) {
		s = fli[ls];
		s2 = fli2[ls];
		if (FailUnison()) return 1;
		if (FailDis()) return 1;
		if (FailPco()) return 1;
		// Long parallel ico
		if (tivl[s] == iIco && ivl[s] == ivl[fli2[ls - 1]]) {
			++pico_count;
			// Two same ico transitions means three intervals already
			if (pico_count == ico_chain-1) {
				FLAG2(89, s)
			}
			else if (pico_count >= ico_chain2) {
				FLAG2(96, s)
			}
		}
		else pico_count = 0;
	}
	return 0;
}

void CGenCP1::CalcStepDpenaltyCP(int i) {
	int dif = abs(cpoint[cantus_id][cpv][i] - acc[cpv][i]);
	int dpe = 0;
	if (dif) dpe = step_penalty + pitch_penalty * dif;
	if (i > fn) dpenalty_step[i] = dpenalty_step[i-1] + dpe;
	else dpenalty_step[i] = dpe;
}

void CGenCP1::SaveCP() {
	if (method == mScan) dpenalty_cur = dpenalty_step[c_len - 1];
	if (!dpenalty_cur) dpenalty_cur = CalcDpenaltyCP(cpoint[cantus_id][cpv], acc[cpv], fn, c_len - 1);
	if (rpenalty_cur == rpenalty_min) {
		// Do not save cantus if it has higher dpenalty
		if (dpenalty_cur > dpenalty_min) return;
		// Do not save cantus if it is same as source
		if (!dpenalty_cur) return;
		dpenalty_min = dpenalty_cur;
	}
	// If rpenalty lowered, set new dpenalty_min
	else {
		dpenalty_min = dpenalty_cur;
	}
	dpenalty.push_back(dpenalty_cur);
	clib.push_back(acc[cpv]);
	rpenalty.push_back(rpenalty_cur);
	rpenalty_min = rpenalty_cur;
	if (method == mScan) {
		// Animation
		long long time = CGLib::time();
		int acy = 0;
		if (animate) acy = (time - correct_start_time) / animate;
		if (!animate || acy > acycle) {
			ShowScanStatus();
			acycle = acy;
			scpoint = acc;
			is_animating = 1;
			// Start showing from initial step to 2 voice (for GenCA2)
			if (m_algo_id == 112) svoice = 2;
			ScanCP(tEval, 2);
			// Reinitialize
			task = tCor;
			if (m_algo_id == 112) ShowLiningCP(acc[cpv]);
			skip_flags = !calculate_blocking && !calculate_correlation && !calculate_stat;
			is_animating = 0;
			step = step0;
			ValidateVectors(step0, t_generated - 1);
			Sleep(animate_delay);
		}
	}
}

// Show best rejected variant
void CGenCP1::ShowBestRejectedCP() {
	if (best_rejected) {
		long long time = CGLib::time();
		int rc = (time - accept_time) / best_rejected;
		if (debug_level > 2) {
			CString st;
			st.Format("Back window with rc %d", rc);
			WriteLog(3, st);
		}
		if (rc > rcycle) {
			rcycle = rc;
			if (br_cc.size() > 0) {
				// Save old cantus
				vector<int> cc_saved = acc[cpv];
				// Load best rejected cantus
				acc[cpv] = br_cc;
				scpoint = acc;
				is_animating = 1;
				// Start showing from initial step to 2 voice (for GenCA2)
				if (m_algo_id == 112) svoice = 2;
				ScanCP(tEval, 2);
				// Reinitialize
				task = tCor;
				if (m_algo_id == 112) ShowLiningCP(acc[cpv]);
				skip_flags = !calculate_blocking && !calculate_correlation && !calculate_stat;
				is_animating = 0;
				step = step0;
				ValidateVectors(step0, t_generated - 1);
				acc[cpv] = cc_saved;
				// Log
				if (debug_level > 0) {
					CString st;
					st.Format("Showing best rejected results with rpenalty %.0f", rpenalty_min);
					WriteLog(3, st);
				}
				// Clear
				br_cc.clear();
				rpenalty_min = MAX_PENALTY;
			}
			else {
				if (debug_level > 1)
					WriteLog(3, "No best rejected results to show");
			}
		}
	}
}

void CGenCP1::SaveCPIfRp() {
	// Is penalty not greater than minimum of all previous?
	if (rpenalty_cur <= rpenalty_min) {
		// If rpenalty 0, we can skip_flags (if allowed)
		if (!skip_flags && rpenalty_cur == 0)
			skip_flags = !calculate_blocking && !calculate_correlation && !calculate_stat;
		// Insert only if cc is unique
		if (clib_vs.Insert(acc[cpv])) {
			SaveCP();
			// Save flags for SWA stuck flags
			if (rpenalty_cur) best_flags = flags;
			TestRpenalty();
		}
	}
}

// Detect many slurs
int CGenCP1::FailSlurs() {
	// For species 5 there are separate rules
	if (species == 5) return 0;
	// Number of sequential slurs 
	int scount = 0;
	// Number of slurs in window
	int scount2 = 0;
	int max_count = 0;
	int max_i = 0;
	// Check pause length
	if (fn*2 > npm) FLAG2(197, fn);
	for (int i = fn; i < ep2-1; ++i) {
		if (acc[cpv][i] == acc[cpv][i + 1]) {
			// Check simultaneous slurs
			//if (acc[cfv][i] == acc[cfv][i + 1]) {
			//FLAG2(98, i);
			//}
			// Check slurs sequence
			++scount;
			if (scount > 1) FLAG2(97, i);
			// Check slurs in window
			++scount2;
			// Subtract old slur
			if ((i - fn >= slurs_window) && (acc[cpv][i - slurs_window] == acc[cpv][i - slurs_window + 1])) --scount2;
			if (scount2 > max_count) {
				max_count = scount2;
				max_i = i;
			}
		}
		else scount = 0;
	}
	if (species != 4) {
		if (max_count == 1) FLAG2(93, max_i)
		else if (max_count == 2) FLAG2(94, max_i)
		else if (max_count > 2) {
			FLAG2(95, max_i);
			if (!accept[95]) fpenalty[95] += (max_count - 2) * 50;
		}
	}
	return 0;
}

// Detect missing slurs
int CGenCP1::FailMissSlurs() {
	// Check only for species 4
	if (species != 4) return 0;
	// Current window size
	int wcount = 0;
	// Number of slurs in window
	int scount = 0;
	int miss, max_miss=0;
	int max_i=0;
	for (int i = fn; i < ep2-1; ++i) if (i % 2) { 
		if (i - fn < miss_slurs_window * npm) ++wcount;
		// Subtract old slur
		if ((i - fn >= miss_slurs_window * npm) && (acc[cpv][i - miss_slurs_window * npm] == acc[cpv][i - miss_slurs_window * npm + 1])) --scount;
		if (acc[cpv][i] == acc[cpv][i + 1]) {
			// Check slurs in window
			++scount;
		}
		else {
			miss = wcount - scount;
			if (miss > max_miss) {
				max_miss = miss;
				max_i = i;
			}
		}
	}
	if (max_miss == 1) FLAG2(188, max_i)
	else if (max_miss == 2) FLAG2(189, max_i)
	else if (max_miss > 2) {
		FLAG2(190, max_i);
		if (!accept[190]) fpenalty[190] += (max_miss - 2) * 50;
	}
	return 0;
}

// Count limits
int CGenCP1::FailCPInterval() {
	int bsteps = 0;
	int step = 0;
	for (int i = 0; i < fli_size; ++i) {
		step = fli2[i];
		// Check between
		if (acc[1][step] - acc[0][step] > max_between) {
			++bsteps;
			// Flag very far burst
			if (acc[1][step] - acc[0][step] > burst_between) FLAG2(11, step);
			if (bsteps > burst_steps) {
				// Flag long burst only on first overrun
				if (bsteps == burst_steps + 1) FLAG2(11, step)
					// Next overruns are sent to fpenalty
				else fpenalty[37] += bsteps - burst_steps;
			}
		}
		else bsteps = 0;
	}
	return 0;
}

// Find situations when one voice goes over previous note of another voice
int CGenCP1::FailOverlap() {
	// Do not check overlap for 1 note
	if (fli_size < 3) return 0;
	if (cantus_high) {
		for (int i = fli[1]; i < ep2; ++i) {
			if (i > fn && acc[cpv][i] >= acc[cfv][i - 1]) FLAG2(24, i)
			else if (i < c_len - 1 && acc[cpv][i] >= acc[cfv][i + 1]) FLAG2(24, i);
		}
	}
	else {
		for (int i = fli[1]; i < ep2; ++i) {
			if (i > fn && acc[cpv][i] <= acc[cfv][i - 1]) FLAG2(24, i)
			else if (i < c_len - 1 && acc[cpv][i] <= acc[cfv][i + 1]) FLAG2(24, i);
		}
	}
	return 0;
}

// Create random cantus and optimize it using SWA
void CGenCP1::RandomSWACP()
{
	// Init animation
	correct_start_time = time();
	acycle = 0;
	CString st;
	//test_cc.resize(24);
	//test_cc[0] = 55;
	int is_sent;
	VSet<int> vs; // Unique checker
								// Disable debug flags
	calculate_blocking = 0;
	calculate_correlation = 0;
	calculate_stat = 0;
	// Create single cantus
	cpoint.resize(1);
	// Forget real cantus id and make algorithms think that my counterpoint is 0
	cantus_id = 0;
	cpoint[0].resize(2);
	cpoint[0][cpv] = acc[cfv];
	cpoint[0][cfv] = acc[cfv];
	scpoint = cpoint[0];
	ScanCPInit();
	for (int i = 0; i < INT_MAX; ++i) {
		if (need_exit) break;
		// Set random_seed to initiate random cantus
		int random_seed0 = random_seed;
		random_seed = 1;
		// Set random_range to limit scanning to one of possible fast-scan ranges
		int random_range0 = random_range;
		random_range = 1;
		// Prohibit limits recalculation during SWA
		int swa_inrange0 = swa_inrange;
		swa_inrange = 1;
		// Create random cantus
		MakeNewCP();
		// Load initial parameters
		random_range = random_range0;
		random_seed = random_seed0;
		swa_inrange = swa_inrange0;
		min_cc0 = min_cc;
		max_cc0 = max_cc;
		// Debug source random cp
		if (0) {
			scpoint = acc;
			ScanCP(tEval, 0);
			Adapt(step, t_generated - 1);
			t_sent = t_generated;
		}
		scpoint[cpv] = acc[cpv];
		cpoint[0][cpv] = acc[cpv];
		// Set scan matrix to scan all
		smatrixc = c_len - fn;
		smatrix.clear();
		smatrix.resize(c_len, 0);
		// Do not scan first pause
		for (int x = fn; x < c_len; ++x) {
			smatrix[x] = 1;
		}
		// Optimize cpoint
		rpenalty_cur = MAX_PENALTY;
		step0 = step;
		SWACP(0, 0);
		is_sent = 0;
		// Show cantus if it is perfect
		if (rpenalty_min <= rpenalty_accepted) {
			if (vs.Insert(acc[cpv])) {
				//if (acc[cpv] == test_cc) {
					//WriteLog(1, "Wow");
				//}
				int step = t_generated;
				// Add line
				linecolor[t_generated] = MakeColor(255, 0, 0, 0);
				scpoint = acc;
				ScanCP(tEval, 0);
				if (rpenalty_cur > rpenalty_accepted) {
					st.Format("Error calculating rpenalty %f min %f at step %d", rpenalty_cur, rpenalty_min, t_generated);
					WriteLog(5, st);
				}
				Adapt(step, t_generated - 1);
				t_sent = t_generated;
				is_sent = 1;
			}
			else {
				++cantus_ignored;
			}
		}
		st.Format("Random SWACP: %d", i);
		SetStatusText(3, st);
		st.Format("Sent: %ld (ignored %ld)", cantus_sent, cantus_ignored);
		SetStatusText(0, st);
		//SendCantus(0, 0);
		// Check limit
		if (t_generated >= t_cnt) {
			WriteLog(3, "Reached t_cnt steps. Generation stopped");
			return;
		}
		// Check rechoose
		if (is_sent && accept_cantus_rechoose && !cantus_id2) return;
	}
	ShowStuck();
}

void CGenCP1::ShowLiningCP(vector<int> &cc) {
	// Add lining
	int pos = step0;
	int lni;
	for (int z = 0; z < c_len; z++) {
		if (cpoint[cantus_id][cpv][z] != cc[z]) lni = 1;
		else lni = 0;
		for (int g = 0; g < cc_len[z]; g++) {
			lining[pos + g][cpv] = lni;
		}
		pos += cc_len[z];
	}
}

// Do not calculate dpenalty (dp = 0). Calculate dpenalty (dp = 1).
void CGenCP1::SWACP(int i, int dp) {
	CString st;
	long long time_start = CGLib::time();
	s_len = 1;
	// Save source rpenalty
	float rpenalty_source = rpenalty_cur;
	long cnum = 0;
	vector<int> animate_cc; // This is cc that was sent in previous animation step
	// Save cantus only if its penalty is less or equal to source rpenalty
	rpenalty_min = rpenalty_cur;
	best_flags = flags;
	dpenalty_min = 0;
	acc = cpoint[i];
	int a;
	for (a = 0; a < approximations; a++) {
		if (need_exit) break;
		// Save previous minimum penalty
		int rpenalty_min_old = rpenalty_min;
		int dpenalty_min_old = dpenalty_min;
		// Clear before scan
		clib.clear();
		clib_vs.clear();
		rpenalty.clear();
		dpenalty.clear();
		clib.push_back(acc[cpv]);
		clib_vs.Insert(acc[cpv]);
		rpenalty.push_back(rpenalty_min_old);
		dpenalty.push_back(dpenalty_min_old);
		// Sliding Windows Approximation
		scpoint = acc;
		ScanCP(tCor, -1);
		cnum = clib.size();
		if (cnum) {
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
			//CString est;
			//est.Format("SWA%d #%d: rp %.0f from %.0f, dp %d, cnum %ld", s_len, a, rpenalty_min, rpenalty_source, dpenalty_min, cnum);
			//WriteLog(3, est);
			// Send intermediate results for debugging
			if (0) {
				scpoint = acc;
				ScanCP(tEval, 0);
				Adapt(step, t_generated - 1);
				t_sent = t_generated;
			}
		}
		if (acc[cfv].size() > 60 || s_len > 0) {
			st.Format("SWA%d #%d RP %.0f DP %d", s_len, a+1, rpenalty_min, dpenalty_min);
			SetStatusText(4, st);
		}
		// Animation
		long long time = CGLib::time();
		int acy = 0;
		if (animate) acy = (time - correct_start_time) / animate;
		if (!animate || acy > acycle) {
			if (debug_level > 2) {
				CString est;
				est.Format("Animation at SWA%d #%d: rp %.0f from %.0f, dp %d, cnum %ld",
					s_len, a + 1, rpenalty_min, rpenalty_source, dpenalty_min, cnum);
				WriteLog(3, est);
			}
			acycle = acy;
			if (animate_cc != acc[cpv]) {
				scpoint = acc;
				is_animating = 1;
				// Start showing from initial step to 0 or 2 voice (for GenCA2)
				if (m_algo_id == 112) {
					ScanCP(tEval, 2);
					ShowLiningCP(acc[cpv]);
				}
				else {
					ScanCP(tEval, 0);
				}
				LogCantus("Animate SWA", 0, acc[cpv]);
				is_animating = 0;
				step = step0;
				ValidateVectors(step0, t_generated - 1);
				//t_sent = t_generated;
				// Delay only if cc changed
				Sleep(animate_delay);
				animate_cc = acc[cpv];
			}
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
					if (best_flags.size()) for (int x = 0; x < max_flags; ++x) {
						if (best_flags[x]) ++ssf[x];
					}
					break;
				}
				else ++s_len;
			}
		}
	}
	// Log
	long long time_stop = CGLib::time();
	CString est;
	// For successful rpenalty_cur == 0, show last flag that was fixed. For unsuccessful, show best variant
	CString sst = GetStuck();
	est.Format("Finished SWA%d #%d: rp %.0f from %.0f, dp %d, cnum %ld (in %d ms): %s",
		s_len, a+1, rpenalty_min, rpenalty_source, dpenalty_min, cnum, time_stop - time_start, sst);
	WriteLog(3, est);
	TestBestRpenalty();
}

int CGenCP1::FailLastIntervals() {
	// Do not check if melody is short yet
	if (fli_size < 3) return 0;
	s = fli[fli_size - 1];
	s_1 = fli[fli_size - 2];
	s_2 = fli[fli_size - 3];
	// Prohibit last note not tonic
	if (ep2 >= c_len) {
		if (apc[cpv][c_len - 1] != 0) {
			// Detect upper last G if lower note is C
			if (apcc[1][c_len - 1] == 7 && apc[0][c_len - 1] == 0) 
				FLAG2(33, c_len-1)
			// Detect upper last E if lower note is C
			else if ((apcc[1][c_len - 1] == 3 || apcc[1][c_len - 1] == 4) && apc[0][c_len - 1] == 0)
				FLAG2(87, c_len - 1)
			else FLAG2(50, c_len - 1);
		}
		// Prohibit major second up before I (applicable to major and minor)
		if (apcc[cpv][s] == 0 && apcc[cpv][s_1] == 10) FLAG2(74, s_1);
		if (apcc[cpv][s] == 0 && apcc[cpv][s_2] == 10) FLAG2(74, s_2);
		// Scan 2nd to last measure
		if (mli.size() > 1) {
			int start = mli[mli.size() - 2];
			if (start + npm < ep2) {
				int b_found = 0;
				int g_found = 0;
				int d_found = 0;
				for (int x = start; x < start + npm; ++x) {
					for (int v = 0; v < av_cnt; ++v) {
						if (apc[v][x] == 6) b_found = 1;
						if (apc[v][x] == 4) g_found = 1;
						if (apc[v][x] == 1) d_found = 1;
					}
				}
				if (!b_found) {
					// Set B needed flag if last bass notes are not G-C
					if (apc[0][c_len - 1] != 0 || apc[0][fli2[fli_size - 2]] != 4) FLAG2(47, start);
				}
				if (!g_found && !d_found) FLAG2(75, start);
			}
		}
	}
	return 0;
}

void CGenCP1::GetNoteTypes() {
	int s = fn;
	int l;
	for (ls = 0; ls < fli_size; ++ls) {
		if (ls > 0) s = fli2[ls-1]+1;
		l = llen[ls];
		// Get beat
		if (s % npm) {
			if (npm>2 && s % (npm / 2)) {
				if (npm>4 && s % (npm / 4)) {
					beat[ls] = 3;
				}
				else beat[ls] = 2;
			}
			else beat[ls] = 1;
		}
		else beat[ls] = 0;
		// Get suspension if cantus note changes during counterpoint note
		sus[ls] = 0;
		if (l > 1) {
			for (int i = 1; i < l; ++i) {
				if (acc[cfv][s + i] != acc[cfv][s + i - 1]) {
					sus[ls] = s + i;
					break;
				}
			}
		}
	}
}

// Create links to unique note columns
void CGenCF1::CreateULinks() {
	vector<int> prev_note;
	prev_note.resize(av_cnt, -1);
	int changed;
	uli.clear();
	// Set first step in case it is pause
	bli[0] = 0;
	for (int i = fn; i < ep2; ++i) {
		changed = 0;
		// Did any of voices change?
		for (int v = 0; v < av_cnt; ++v) {
			if (prev_note[v] != acc[v][i]) {
				changed = 1;
			}
			prev_note[v] = acc[v][i];
		}
		if (!changed) continue;
		uli.push_back(i);
	}
}

void CGenCP1::GetMeasures() {
	mli.clear();
	for (int i = 0; i < c_len; ++i) {
		// Find measures
		if (i % npm == 0) {
			mli.push_back(i);
		}
	}
}

int CGenCP1::FailGisTrail2() {
	int gis_trail = 0;
	for (ls = 0; ls < fli_size; ++ls) {
		s = fli[ls];
		if (apcc[0][s] == 11 || apcc[1][s] == 11) {
			// Set to maximum on new G# note
			gis_trail = gis_trail_max;
		}
		else {
			if (apcc[0][s] == 10 || apcc[1][s] == 10) {
				// Prohibit G note close to G#
				if (gis_trail) FLAG2(200, s);
			}
		}
		// Decrease if not zero
		if (gis_trail) --gis_trail;
	}
	return 0;
}

int CGenCP1::FailHarm() {
	hli_size = 0;
	hli.clear();
	// Detect harmony changes
	for (ls = 0; ls < fli_size; ++ls) {
		s = fli[ls];
	}
	// Build chm vector
	for (ls = 0; ls < hli_size; ++ls) {
	}
	return 0;
}

void CGenCP1::ScanCP(int t, int v) {
	int finished = 0;
	int scycle = 0;
	// Load master parameters
	task = t;
	svoice = v;

	ScanCPInit();
	if (task == tGen) MultiCPInit();
	else SingleCPInit();
	if (FailWindowsLimit()) return;
	// Need to clear flags, because if skip_flags, they can contain previous prohibited flags
	fill(flags.begin(), flags.end(), 0);
	// We can call get measures once, because it calculates measures within c_len
	GetMeasures();
	// Analyze combination
check:
	while (true) {
		// First pause
		for (int i = 0; i < fn; ++i) acc[cpv][i] = acc[cpv][fn];
		//LogCantus("ep2", ep2, cc_id);
		//if (MatchVectors(acc[cpv], test_cc, 0, 20))
		//	WriteLog(1, "Found");
		// Check if dpenalty is already too high
		if (task == tCor && !rpenalty_min) {
			if (method == mScan) {
				CalcStepDpenaltyCP(ep2 - 1);
				if (dpenalty_step[ep2 - 1] > dpenalty_min) goto skip;
			}
			else {
				dpenalty_cur = dpenalty_outside_swa + CalcDpenaltyCP(cpoint[cantus_id][cpv], acc[cpv], sp1, sp2-1);
				if (dpenalty_cur > dpenalty_min) goto skip;
			}
		}
		else dpenalty_cur = 0;
		GetMelodyInterval(acc[cpv], 0, ep2, nmin, nmax);
		// Limit melody interval
		if (task == tGen) {
			if (nmax - nmin > max_interval) goto skip;
			if (c_len == ep2 && nmax - nmin < min_interval) goto skip;
			if (cantus_high) {
				if (cf_nmax - nmin > sum_interval) goto skip;
			}
			else {
				if (nmax - cf_nmin > sum_interval) goto skip;
			}
			ClearFlags(0, ep2);
		}
		else {
			ClearFlags(0, ep2);
			if (nmax - nmin > max_interval) FLAG(37, fn);
			if (cantus_high) {
				if (cf_nmax - nmin > sum_interval) FLAG(7, fn);
			}
			else {
				if (nmax - cf_nmin > sum_interval) FLAG(7, fn);
			}
			if (c_len == ep2 && nmax - nmin < min_interval) FLAG(38, fn);
		}
		if (FailMissSlurs()) goto skip;
		if (FailSlurs()) goto skip;
		++accepted3;
		if (need_exit) break;
		// Show status
		long long time = CGLib::time();
		scycle = (time - gen_start_time) / STATUS_PERIOD;
		if (scycle > status_cycle) {
			ShowScanStatus();
			status_cycle = scycle;
		}
		// Limit SAS correction time
		if (task == tCor && method == mScan && max_correct_ms && time - scan_start_time > max_correct_ms) break;
		if (FailDiatonic(ac[cpv], acc[cpv], 0, ep2, minor_cur)) goto skip;
		GetPitchClass(ac[cpv], acc[cpv], apc[cpv], apcc[cpv], 0, ep2);
		CreateLinks(acc[cpv]);
		//CreateULinks();
		if (minor_cur) {
			if (FailMinor(apcc[cpv], acc[cpv])) goto skip;
			if (FailGisTrail2()) goto skip;
		}
		//if (acc[cpv][1] == 105 && acc[cpv][2] == 98)
			//WriteLog(1, "Found");
		if (FailCPInterval()) goto skip;
		GetNoteTypes();
		GetLeapSmooth(ac[cpv], acc[cpv], aleap[cpv], asmooth[cpv], aslur[cpv]);
		if (FailRhythm3()) goto skip;
		if (FailRhythm5()) goto skip;
		GetRpos();
		GetVIntervals();
		if (FailVMotion()) goto skip;
		if (FailVIntervals()) goto skip;
		if (FailTonic(acc[cpv], apc[cpv])) goto skip;
		if (FailLastIntervals()) goto skip;
		if (FailNoteSeq(apc[cpv])) goto skip;
		if (FailIntervals(ac[cpv], acc[cpv], apc[cpv], apcc[cpv])) goto skip;
		if (FailManyLeaps(ac[cpv], acc[cpv], aleap[cpv], asmooth[cpv], aslur[cpv], max_leaps, max_leaped, max_leap_steps, 3, 25)) goto skip;
		if (FailManyLeaps(ac[cpv], acc[cpv], aleap[cpv], asmooth[cpv], aslur[cpv], max_leaps2, max_leaped2, max_leap_steps2, 202, 203)) goto skip;
		if (FailLeapSmooth(ac[cpv], acc[cpv], aleap[cpv], asmooth[cpv], aslur[cpv])) goto skip;
		if (FailOutstandingRepeat(ac[cpv], acc[cpv], aleap[cpv], repeat_steps2, 2, 76)) goto skip;
		if (FailOutstandingRepeat(ac[cpv], acc[cpv], aleap[cpv], repeat_steps3, 3, 36)) goto skip;
		if (FailLongRepeat(acc[cpv], aleap[cpv], repeat_steps5, 5, 72)) goto skip;
		if (FailLongRepeat(acc[cpv], aleap[cpv], repeat_steps7, 7, 73)) goto skip;
		// Calculate diatonic limits
		nmind = CC_C(nmin, tonic_cur, minor_cur);
		nmaxd = CC_C(nmax, tonic_cur, minor_cur);
		if (FailGlobalFill(ac[cpv], nstat2)) goto skip;
		if (FailLocalRange(acc[cpv], notes_lrange, min_lrange, 98)) goto skip;
		if (FailLocalRange(acc[cpv], notes_lrange2, min_lrange2, 198)) goto skip;
		if (FailAlteredInt()) goto skip;
		if (FailCrossInt()) goto skip;
		if (FailPcoApart()) goto skip;
		if (FailSus()) goto skip;
		if (FailOverlap()) goto skip;
		if (FailStagnation(acc[cpv], nstat, stag_note_steps, stag_notes, 10)) goto skip;
		if (FailStagnation(acc[cpv], nstat, stag_note_steps2, stag_notes2, 39)) goto skip;
		if (FailMultiCulm(acc[cpv], aslur[cpv])) goto skip;
		if (FailFirstNotes(apc[cpv])) goto skip;
		if (FailLeap(ac[cpv], aleap[cpv], asmooth[cpv], nstat2, nstat3)) goto skip;
		//if (FailMelodyHarm(apc[cpv], 0, ep2)) goto skip;
		MakeMacc(acc[cpv]);
		if (FailLocalMacc(notes_arange, min_arange, 15)) goto skip;
		if (FailLocalMacc(notes_arange2, min_arange2, 16)) goto skip;
		if (FailHarm()) goto skip;

		//LogCantus("Rpenalty", rpenalty_cur, flags);
		SaveBestRejected(acc[cpv]);
		// If we are window-scanning
		if ((task == tGen || task == tCor) && method == mScan) {
			++accepted2;
			CalcFlagStat();
			if (FailFlagBlock()) goto skip;
			if (FailAccept()) goto skip;
			++accepted4[wid];
			//LogCantus(acc[cpv]);
			// If this is not last window, go to next window
			if (ep2 < c_len) {
				NextWindow();
				goto check;
			}
			// Check random_choose
			if (random_choose < 100) if (rand2() >= (float)RAND_MAX*random_choose / 100.0) goto skip;
		}
		// Calculate rules penalty if we evaluate or correct cantus without full scan
		else {
			CalcRpenalty(acc[cpv]);
		}
		// Accept cantus
		++accepted;
		TimeBestRejected();
		if (method == mScan && task == tCor) {
			SaveCP();
		}
		else if (method == mSWA && task == tCor) {
			SaveCPIfRp();
		}
		else {
			// Put rule debug lines here:
			//if (FailVIntervals()) goto skip;
			if (task == tGen && accept_reseed) {
				if (clib_vs.Insert(acc[cpv])) {
					if (SendCP()) break;
					ReseedCP();
					// Start evaluating without scan
					goto check;
				}
				else {
					++cantus_ignored;
					ReseedCantus();
					// Start evaluating without scan
					goto check;
				}
			}
			else {
				// Calculate dpenalty if this is evaluation
				if (task == tEval) dpenalty_cur = CalcDpenaltyCP(cpoint[cantus_id][cpv], acc[cpv], fn, c_len - 1);
				if (SendCP()) break;
			}
			// Exit if this is evaluation
			if (task == tEval) return;
		}
	skip:
		ScanLeft(acc[cpv], finished);
		if (finished) {
			// Clear flag to prevent coming here again
			finished = 0;
			// Sliding Windows Approximation
			if (method == mSWA) {
				if (NextSWA(acc[cpv], acc_old[cpv])) break;
				goto check;
			}
			// Finish if this is last variant in first window and not SWA
			else if ((p == 0) || (wid == 0)) {
				if (random_seed && random_range && accept_reseed) {
					// Infinitely cycle through ranges
					ReseedCP();
					// Start evaluating without scan
					goto check;
				}
				WriteLog(3, "Last variant in first window reached");
				break;
			}
			ShowBestRejectedCP();
			BackWindow(acc[cpv]);
			// Goto next variant calculation
			goto skip;
		} // if (finished)
		ScanRight(acc[cpv]);
	}
	if (accepted3 > 100000) ShowScanStatus();
	WriteFlagCor();
	ShowFlagStat();
	ShowFlagBlock();
	WritePerfLog();
}

void CGenCP1::LoadCantusHigh() {
	CString st;
	if (cp_incom[cantus_id].size()) {
		if (cp_incom[cantus_id][0].Find("cf=low") != -1) cantus_high = 0;
		else if (cp_incom[cantus_id][0].Find("cf=high") != -1) cantus_high = 1;
		else {
			// Use previous cantus_high (of previous cantus or from configuration if first cantus)
			st.Format("Warning: no CF=high or CF=low lyrics for counterpoint #%d. Assuming %s",
				cantus_id + 1, cantus_high ? "high" : "low");
			WriteLog(5, st);
		}
	}
	else {
		st.Format("Warning: no CF=high or CF=low lyrics for counterpoint #%d. Assuming %s",
			cantus_id + 1, cantus_high ? "high" : "low");
		WriteLog(5, st);
	}
}

void CGenCP1::LoadSpecies() {
	species = 0;
	if (cp_incom[cantus_id].size()) {
		if (cp_incom[cantus_id][0].Find("sp1") != -1) species = 1;
		else if (cp_incom[cantus_id][0].Find("sp2") != -1) species = 2;
		else if (cp_incom[cantus_id][0].Find("sp3") != -1) species = 3;
		else if (cp_incom[cantus_id][0].Find("sp4") != -1) species = 4;
		else if (cp_incom[cantus_id][0].Find("sp5") != -1) species = 5;
	}
}

void CGenCP1::Generate() {
	//CString st = "60 52 59 57 55 60 62 57 59 55 57 59 57 50 52 53 55 52 59 59 60";
	//test_cc.resize(24);
	//test_cc[0] = 55;
	//StringToVector(&st, " ", test_cc);

	int fn0 = fn;
	if (InitCP()) return;
	SetStatusText(8, "MIDI file: " + fname_from_path(midi_file));
	LoadCantus(midi_file);
	if (cantus.size() < 1) return;
	for (int a = 0; a < INT_MAX; ++a) {
		// Reset note scan range to ignore it for showing cantus
		min_cc0.clear();
		max_cc0.clear();
		// Choose cantus to use
		cantus_id = randbw(0, cantus.size() - 1);
		if (cantus_id2) {
			if (cantus_id2 <= cantus.size()) {
				cantus_id = cantus_id2 - 1;
			}
			else {
				CString est;
				est.Format("Warning: cantus_id in configuration file (%d) is greater than number of canti loaded (%d). Selecting highest cantus.",
					cantus_id2, cantus.size());
				WriteLog(1, est);
				cantus_id = cantus.size() - 1;
			}
		}
		c_len = cantus[cantus_id].size();
		// Get key
		av_cnt = 1;
		acc[0] = cantus[cantus_id];
		GetCPKey();
		av_cnt = 2;
		// Get cantus interval
		GetMelodyInterval(cantus[cantus_id], 0, cantus[cantus_id].size(), cf_nmin, cf_nmax);
		if (tonic_cur == -1) return;
		CalcCcIncrement();
		// Show imported melody
		cc_len = cantus_len[cantus_id];
		cc_tempo = cantus_tempo[cantus_id];
		real_len = accumulate(cantus_len[cantus_id].begin(), cantus_len[cantus_id].end(), 0);
		dpenalty_cur = 0;
		fn = 0;
		// Create pause
		FillPause(step, step + floor(real_len / 8 + 1) * 8, 0);
		FillPause(step, step + floor(real_len / 8 + 1) * 8, 1);
		// Select rule set
		SelectRuleSet(cf_rule_set);
		// Choose level
		if (cantus_high) {
			cpv = 0;
			cfv = 1;
		}
		else {
			cpv = 1;
			cfv = 0;
		}
		show_note_scan_range = 0; 
		ScanCantus(tEval, cfv, &(cantus[cantus_id]));
		// Go forward
		show_note_scan_range = 1;
		Adapt(0, step - 1);
		t_generated = step;
		t_sent = t_generated;
		// Load first voice
		vector<int> cc_len_old = cc_len;
		vector<float> cc_tempo_old = cc_tempo;
		vector<int> anflagsc_old = anflagsc[cfv];
		vector<vector<int>> anflags_old = anflags[cfv];
		c_len = m_c.size() * npm - (npm - 1);
		ac[cfv].clear();
		acc[cfv].clear();
		apc[cfv].clear();
		apcc[cfv].clear();
		cc_len.clear();
		cc_tempo.clear();
		anflags[cfv].clear();
		anflagsc[cfv].clear();
		// Create empty arrays
		anflags[cfv].resize(m_c.size()*npm);
		anflagsc[cfv].resize(m_c.size()*npm);
		int npm2 = npm;
		for (int i = 0; i < m_c.size(); ++i) {
			// Last measure should have one note
			if (i == m_c.size() - 1) npm2 = 1;
			for (int x = 0; x < npm2; ++x) {
				ac[cfv].push_back(m_c[i]);
				acc[cfv].push_back(m_cc[i]);
				apc[cfv].push_back(m_pc[i]);
				apcc[cfv].push_back(m_pcc[i]);
				cc_len.push_back(cc_len_old[i] * npm / npm2);
				cc_tempo.push_back(cc_tempo_old[i]);
				if (!x) {
					int y = i*npm + x;
					anflagsc[cfv][y] = anflagsc_old[i];
					anflags[cfv][y] = anflags_old[i];
				}
			}
		}
		fn = fn0;
		// Generate second voice
		real_len = accumulate(cc_len.begin(), cc_len.end(), 0);
		rpenalty_cur = MAX_PENALTY;
		if (SelectRuleSet(cp_rule_set)) return;
		if (method == mSWA) {
			RandomSWACP();
		}
		else {
			ScanCP(tGen, 0);
		}
		// Stop generation if finished or not random or not rechoosing
		if (need_exit || t_generated >= t_cnt || !accept_cantus_rechoose || cantus_id2) break;
	}
}
