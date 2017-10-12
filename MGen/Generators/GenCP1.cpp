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
	bmli.resize(c_len);
	// Copy cantus
	acc = scpoint;
	// Get diatonic steps from chromatic
	for (int v = 0; v < acc.size(); ++v) {
		for (int i = 0; i < c_len; ++i) {
			ac[v][i] = CC_C(acc[v][i], tonic_cur, minor_cur);
		}
	}
	if (!is_animating) {
		// Save value for future use;
		acc_old = acc;
		// Calculate limits
		if (cantus_high) {
			for (int i = 0; i < c_len; ++i) {
				min_cc[i] = max(acc[cfv][i] - burst_between,
					max(src_nmin, max(cf_nmax - sum_interval, acc[cpv][i] - correct_range)));
				max_cc[i] = min(acc[cfv][i] - min_between,
					min(max(src_nmax, min_cc[i] + (src_nmax - src_nmin)),
						max(acc[cpv][i] + correct_range, min_cc[i] + 2 * correct_range)));
			}
		}
		else {
			for (int i = 0; i < c_len; ++i) {
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
		for (int i = 0; i < c_len; ++i) {
			min_c[i] = CC_C(min_cc[i], tonic_cur, minor_cur);
			max_c[i] = CC_C(max_cc[i], tonic_cur, minor_cur);
		}
		sp1 = 0;
		sp2 = c_len;
		ep1 = max(0, sp1 - 1);
		ep2 = c_len;
	}
	// Clear flags
	++accepted3;
	fill(flags.begin(), flags.end(), 0);
	flags[0] = 1;
	for (int i = 0; i < ep2; ++i) {
		anflags[cpv][i].clear();
	}
	// Matrix scan
	if (task != tEval) {
		// Exit if no violations
		if (!smatrixc) return;
		// Create map
		smap.resize(smatrixc);
		int map_id = 0;
		for (int i = 0; i < c_len; ++i) if (smatrix[i]) {
			smap[map_id] = i;
			++map_id;
		}
		sp1 = 0;
		sp2 = sp1 + s_len; // End of search window
		if (sp2 > smatrixc) sp2 = smatrixc;
		// Record window
		wid = 0;
		wpos1[wid] = sp1;
		wpos2[wid] = sp2;
		CalculateCcOrder(acc_old[cpv], 0, c_len);
		// Clear scan steps
		FillCantusMap(cc_id, smap, 0, smatrixc, 0);
		FillCantusMap(acc[cpv], smap, 0, smatrixc, cc_order);
		ep2 = GetMaxSmap() + 1;
		if (sp2 == smatrixc) ep2 = c_len;
		dpenalty_step.clear();
		dpenalty_step.resize(c_len, 0);
		if (method == mScan) {
			ResizeToWindow();
			// Can skip flags - full scan must remove all flags
		}
		// For sliding windows algorithm evaluate whole melody
		if (method == mSWA) {
			sp2 = sp1 + min(swa_len, s_len);
			swa1 = 0;
			swa2 = swa1 + swa_len;
			// Record window
			swcount = 1;
			swid = 0;
			swpos1[swid] = swa1;
			swpos2[swid] = swa2;
			// Cannot skip flags - need them for penalty if cannot remove all flags
			skip_flags = 0;
			dpenalty_outside_swa = 0;
			if (swa1 > 0) dpenalty_outside_swa += CalcDpenalty(cpoint[cantus_id][cpv], acc[cpv], 0, smap[swa1 - 1]);
			if (swa2 < smap.size()) dpenalty_outside_swa += CalcDpenalty(cpoint[cantus_id][cpv], acc[cpv], smap[swa2], c_len - 1);
			fill(src_rpenalty_step.begin(), src_rpenalty_step.end(), 0);
			if (sp2 == swa2) ep2 = c_len;
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
	// Absolute maximum of scan range for culminations
	max_cc2 = vmax(max_cc);
}

void CGenCP1::MultiCPInit() {
	bmli.resize(c_len);
	MakeNewCP();
	sp1 = 0; // Start of search window
	sp2 = sp1 + s_len; // End of search window
	if (sp2 > c_len - 1) sp2 = c_len - 1;
	// Record window
	wid = 0;
	wpos1[wid] = sp1;
	wpos2[wid] = sp2;
	ep1 = max(0, sp1 - 1);
	ep2 = sp2; // End of evaluation window
	p = sp2 - 1; // Minimal position in array to cycle
	ResizeToWindow();
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
	retrigger.resize(c_len);
	rposb.resize(c_len);
	rposf.resize(c_len);
	rpos.resize(c_len);
	pat.resize(c_len);
	pat_state.resize(c_len);
	ivl.resize(c_len);
	ivlc.resize(c_len);
	civl.resize(c_len);
	civlc.resize(c_len);
	tivl.resize(c_len);
	motion.resize(c_len);
	beat.resize(c_len);
	sus.resize(c_len);
	ep2 = c_len;
}

void CGenCP1::SendRpos(int pos, int i, int v, int av, int x) {
	//if (rpos[bli[x]] < 0) lining[pos + i][v] = HatchStyleLargeConfetti;
	//else lining[pos + i][v] = 0;
	if (tivl[x] == iDis) lining[pos + i][v] = HatchStyleNarrowHorizontal;
	else lining[pos + i][v] = 0;
}

int CGenCP1::SendCP() {
	int step00 = step;
	int pause_len = 0;
	CString st, rpst;
	int pos = 0, plen;
	int v, x1;
	if (svoice < 0) return 0;
	Sleep(sleep_ms);
	long long time_start = CGLib::time();
	long long time_stop;
	len_export.resize(c_len);
	coff_export.resize(c_len);
	if (!mutex_animate.try_lock_for(chrono::milliseconds(5000))) {
		WriteLog(5, "Critical error: ResizeVectors mutex timed out");
	}
	for (int av = 0; av < av_cnt; ++av) {
		CreateLinks(ac[av], av == cpv);
		MakeMacc(acc[av]);
		pos = step;
		// Sent voice is the same as acc voice
		v = svoice + av;
		MakeLenExport(acc[av], av, av == cpv);
		// Reset cc_len back after extending first cf note
		cc_len[0] = cc_len[1];
		plen = cc_len[0] * fn;
		if (av == cpv) {
			FillPause(pos, plen, v);
			pos += plen;
		}
		else {
			//AddNote(pos, v, acc[cfv][0], len_export[0] + plen, 100);
			// Extend first cf note
			cc_len[0] = cc_len[1] * (fn + 1);
		}
		// Copy cantus to output
		if (step + real_len >= t_allocated) ResizeVectors(t_allocated * 2);
		for (int x = 0; x < ep2; ++x) {
			mark_color[pos][v] = MakeColor(255, 120, 120, 120);
			mark[pos][v] = "";
			if (av == cpv) {
				cpos[x] = pos;
				if (species != 1) {
					if (x == fli[bli[x]]) {
						if (pat[bli[x]] == pCam) mark[pos][v] = "C\nA";
						else if (pat[bli[x]] == pPDD) mark[pos][v] = "P\nD";
						else if (pat[bli[x]] == pDNT) mark[pos][v] = "D\nN";
						if (pat_state[bli[x]] == 1) mark_color[pos][v] = MakeColor(255, 255, 120, 120);
					}
				}
			}
			SendLyrics(pos, v, av, x);
			for (int i = 0; i < cc_len[x]; ++i) {
				if (av == cpv) {
					// Set color
					color[pos + i][v] = color_noflag;
					SendRpos(pos, i, v, av, x);
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
	if ((apcc[0][i] == c1 && apcc[1][i] == c2) || (apcc[0][i] == c2 && apcc[1][i] == c1)) 
		FLAG2(flag, i);
	return 0;
}

// Fail vertical altered intervals
int CGenCP1::FailAlteredInt() {
	CHECK_READY(DR_pc);
	for (int i = 0; i < ep2; ++i) {
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
	CHECK_READY(DR_pc);
	for (int s = 1; s < ep2; ++s) {
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
	CHECK_READY(DR_c);
	SET_READY(DR_ivl);
	// Calculate intervals
	for (int i = 0; i < ep2; ++i) {
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
	SET_READY(DR_motion);
	int mtemp;
	int scontra = 0;
	int sdirect = 0;
	for (int i = 0; i < ep2; ++i) {
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
			if (pcontra < contrary_min2) FLAG2(46, 0)
			else if (pcontra < contrary_min) FLAG2(35, 0);
		}
	}
	return 0;
}

int CGenCP1::FailSusResolution(int s3) {
	// Check if suspension second part is discord
	if (tivl[s2] != iDis) {}
	// Resolution to discord
	else if (tivl[s3] == iDis) FLAG2(220, s3)
		// Resolution by leap
	else if (abs(ac[cpv][s3] - ac[cpv][s2]) > 1) FLAG2(221, s3)
	else {
		// Resolution up
		if (acc[cpv][s3] > acc[cpv][s2]) {
			// Allowed only for resolution of leading tone
			if (apcc[cpv][s2] == 11) FLAG2(222, s3)
			else FLAG2(219, s3);
		}
		// 9th to 8va
		if (ivlc[s2] == 1 && ivlc[s3] == 0) {
			if (ivl[s2] > 7) FLAG2(216, s3)
				// 2nd to unison
			else FLAG2(218, s3);
		}
		// 7th to 8va
		else if (ivlc[s2] == 6 && ivlc[s3] == 0) FLAG2(217, s3)
	}
	return 0;
}

int CGenCP1::FailSus1() {
	CHECK_READY(DR_fli, DR_ivl, DR_sus);
	CHECK_READY(DR_leap, DR_beat);
	int last_cf;
	for (ls = 0; ls < fli_size; ++ls) if (sus[ls]) {
		s = fli[ls];
		s2 = fli2[ls];
		// Flag suspension
		FLAG2(225, s);
		// Check if sus starts from discord
		if (tivl[s] == iDis) FLAG2(224, s);
		last_cf = -1;
		// Check all cantus note changes
		for (s = sus[ls]; s <= s2; ++s) {
			if (last_cf != acc[cfv][s]) {
				last_cf = acc[cfv][s];
				if (FailUnison()) return 1;
				if (FailDisSus()) return 1;
				if (FailPcoSus()) return 1;
			}
		}
	}
	return 0;
}

int CGenCP1::FailSus2() {
	CHECK_READY(DR_fli, DR_ivl, DR_sus);
	CHECK_READY(DR_leap);
	int ls3, s3, antici;
	for (ls = 0; ls < fli_size; ++ls) if (sus[ls]) {
		// Run sus checks
		s = fli[ls];
		s2 = fli2[ls];
		antici = 0;
		// If sus starts with dissonance, it is anticipation
		if (tivl[s] == iDis) antici = 1;
		// If both consonances
		else if (tivl[s2] != iDis) {
			// If sus starts with note shorter than 1/2, it is anticipation
			if (sus[ls] - s < npm / 2) antici = 1;
			// If sus is longer than whole note
			else if (llen[ls] > npm) antici = 1;
		}
		// Check if sus starts from discord
		if (antici) {
			// Check if start and end of slur is a discord - then it is interbar discord
			if (tivl[s2] == iDis) {
				FLAG2(224, s);
				continue;
			}
			// Flag anticipation
			FLAG2(287, s);
			++retrigger[sus[ls]];
			// Anticipation approached by leap
			if (s > 0 && aleap[cpv][s - 1]) FLAG2(289, s);
			// If anticipation is not last generated note
			if (ls < fli_size - 1) {
				// Anticipation is not last note
				FLAG2(290, s);
				// Anticipation should be shorter then next note
				if (llen[ls] == (sus[ls] - s) * 2) FLAG2(293, s)
				else if (llen[ls] < (sus[ls] - s) * 2) FLAG2(288, s);
				// Anticipation should be shorter then previous note
				if (ls > 0 && sus[ls] - s > llen[ls - 1]) FLAG2(294, s);
			}
		}
		else {
			// Flag suspension
			FLAG2(225, s);
			if (species < 4) {
				if (bmli[s] < mli.size() - sus_last_measures) FLAG2(139, s);
			}
			// 1/4 syncope (for last 1/4)
			if (beat[ls] == 5 && llen[ls] > 1) FLAG2(235, s);
			// If sus is not last note
			if (ls < fli_size - 1) {
				// If mid-bar already generated
				s3 = sus[ls] + npm / 2;
				if (s3 < ep2) {
					// If this step does not start new note
					if (acc[cpv][s3] == acc[cpv][s3 - 1]) FLAG2(286, s2 + 1);
					// If resolution note is too short
					ls3 = bli[s3];
					if (llen[ls3] < npm / 4 && ls3 < fli_size - 1) FLAG2(291, s3);
					if (FailSusResolution(s3)) return 1;
					// If there is one intermediate step
					if (ls3 - ls == 2) {
						// If leap is too long
						if (abs(acc[cpv][fli[ls + 1]] - acc[cpv][s2]) > sus_insert_max_leap) FLAG2(295, fli[ls + 1])
						// If second movement is leap
						if (aleap[cpv][fli2[ls + 1]] > 0) FLAG2(136, fli[ls + 1])
						else if (aleap[cpv][fli2[ls + 1]] < 0) FLAG2(296, fli[ls + 1])
						else if (asmooth[cpv][fli2[ls + 1]] > 0) FLAG2(137, fli[ls + 1])
						else if (asmooth[cpv][fli2[ls + 1]] < 0) FLAG2(138, fli[ls + 1])
					}
					// If there are two intermediate steps
					//else if (ls3 - ls == 3) {
					//}
					// If there are more than two intermediate steps
					else if (ls3 - ls > 3) {
						FLAG2(292, s3);
					}
				}
			}
			else if (ep2 == c_len) {
				// If sus is absolute last note, flag unresolved
				FLAG2(220, s);
			}
		}
	}
	return 0;
}

int CGenCP1::FailSus() {
	fill(retrigger.begin(), retrigger.end(), 0);
	if (species == 1) {
		if (FailSus1()) return 1;
	}
	else {
		if (FailSus2()) return 1;
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
		// 2nd -> unison
		if (ls > 0 && ivl[fli[ls - 1]] == 1) FLAG2(275, s);
		// Unison near m2
		if (ls < fli_size - 1 && civl[fli[ls + 1]] == 1) FLAG2(277, s)
		else if (ls > 0 && civl[fli[ls - 1]] == 1) FLAG2(275, s);
		// Inside downbeat
		if (!beat[ls] && ls > 0 && ls < fli_size - 1) FLAG2(91, s);
	}
	return 0;
}

int CGenCP1::FailDis() {
	// Discord
	if (tivl[s] == iDis) {
		// Do not flag discord if suspension, because anticipation will flag it
		// Do not flag discord if last note, because it can become suspension
		if (sus[ls] || ls == fli_size - 1) return 0;
		// Leap
		if (rpos[ls] == pLeap) {
			FLAG2(187, s)
		}
		// Other harmonic
		else if (rpos[ls] > 0) FLAG2(83, s)
		else {
			// Stepwize
			if (civl[s] == 1) FLAG2(276, s)
			else {
				if (rpos[ls] < -30) FLAG2(256, s)
				else if (rpos[ls] < -20) FLAG2(258, s)
				else if (rpos[ls] < -10) FLAG2(282, s)
				else FLAG2(169, s)
			}
		}
		// Check if discord is longer than neighboring consonance
		if (ls > 0 && llen[ls] > llen[ls - 1] && tivl[fli2[ls - 1]] != iDis) FLAG2(223, s)
		else if (ls < fli_size - 2 && llen[ls] > llen[ls + 1] && tivl[fli[ls + 1]] != iDis)
			FLAG2(223, s);
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
					if (s2 == c_len - 1 && cfli[cfli.size()-1] == s) {
						if (civlc[s] == 0) FLAG2(209, s)
						else FLAG2(208, s);
					}
					else if (ls < fli_size - 1 || ep2 == c_len) {
						if (civlc[s] == 0) FLAG2(211, s)
						else FLAG2(210, s);
					}
				}
				// Non-stepwize
				else {
					if (s2 == c_len - 1 && cfli[cfli.size() - 1] == s) {
						if (civlc[s] == 0) FLAG2(213, s)
						else FLAG2(212, s);
					}
					else if (ls < fli_size - 1 || ep2 == c_len) {
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
				if (culm_ls == ls || (cfv == 1 && cf_culm_s == s)) FLAG2(81, s);
			}
			else {
				// Prohibit downbeat culmination
				if (culm_ls == ls || (cfv == 1 && cf_culm_s == s)) 
					FLAG2(82, s)
				// Prohibit downbeat
				else FLAG2(80, s);
			}
		}
	}
	return 0;
}

// Do not scan sus
int CGenCP1::SkipSus(int notes) {
	int found = -1;
	// Search for sus in first window
	if (ls == 0) {
		for (int ls2 = 0; ls2 < notes; ++ls2) {
			if (sus[ls2]) found = ls2;
		}
	}
	// Search for sus in last note
	if (sus[ls + notes]) found = ls + notes;
	if (found > -1) {
		// Skip to note next to sus
		ls += found + 1;
		// Find next downbeat or half-downbeat note
		while (ls < fli_size && beat[ls] > 1 && ls - found < 3) ++ls;
		return 1;
	}
	return 0;
}

// Detect passing downbeat dissonance
void CGenCP1::DetectPDD() {
	if (!accept[282]) return;
	int max_ls = fli_size - 1;
	if (ep2 == c_len) max_ls = fli_size - 2;
	for (ls = 0; ls < max_ls; ++ls) {
		if (ls < fli_size - 2 && SkipSus(1)) continue;
		s = fli[ls];
		s2 = fli2[ls];
		// Second note is downbeat (only beat 1 allowed)
		if (beat[ls + 1]) continue;
		// Stepwize downward movement
		if (ac[cpv][fli[ls + 1]] - ac[cpv][s] != -1) continue;
		// Note 2 is not long
		if (llen[ls + 1] > npm / 2) continue;
		// Parallel motion
		if (ac[cfv][fli[ls + 1]] - ac[cfv][s] == -1) {
			if (!accept[298]) continue;
		}
		// Direct motion
		else if (ac[cfv][fli[ls + 1]] - ac[cfv][s] < 0) {
			if (!accept[297]) continue;
		}
		if (ls < fli_size - 2) {
			// Stepwize downward movement
			if (ac[cpv][fli[ls + 2]] - ac[cpv][fli[ls + 1]] != -1) continue;
			// Note 2 is not longer than 3
			if (llen[ls + 1] > llen[ls + 2] && (ep2 == c_len || ls < fli_size - 3)) continue;
			// Third note must be consonance
			if (tivl[fli[ls + 2]] == iDis) continue;
		}
		SavePattern(pPDD);
	}
}

// Detect downbeat neighbour tone
void CGenCP1::DetectDNT() {
	if (!accept[258]) return;
	int max_ls = fli_size - 1;
	if (ep2 == c_len) max_ls = fli_size - 3;
	for (ls = 0; ls < max_ls; ++ls) {
		if (ls < fli_size - 3 && SkipSus(3)) continue;
		s = fli[ls];
		s2 = fli2[ls];
		// Note 2 is long
		if (llen[ls + 1] > npm / 2) continue;
		// Note 1 is not dissonance
		if (tivl[s] == iDis) continue;
		// Note 2 is downbeat
		if (!beat[ls + 1]) continue;
		// Movements are stepwize
		if (!asmooth[cpv][s2]) continue;
		if (ls < fli_size - 2) {
			// Note 2 is longer than 1
			if (llen[ls + 1] > llen[ls]) continue;
			// Note 3 is long
			if (llen[ls + 2] > npm / 2) continue;
			// No leap
			if (!aleap[cpv][fli2[ls + 1]]) continue;
			// Leap has same direction
			if (aleap[cpv][fli2[ls + 1]] == asmooth[cpv][s2]) continue;
			// Maximum leap
			int lp = abs(acc[cpv][fli[ls + 2]] - acc[cpv][fli[ls + 1]]);
			if (lp < 3) continue;
			if (lp > 4) {
				if (!accept[260]) continue;
				if (lp > dnt_max_leap) continue;
			}
			if (ls < fli_size - 3) {
				// Note 3 is longer than 4
				if (llen[ls + 2] > llen[ls + 3] && (ep2 == c_len || ls < fli_size - 4)) continue;
				// Note 4 is not dissonance
				if (tivl[fli[ls + 3]] == iDis) continue;
				// Movements are stepwize
				if (!asmooth[cpv][fli2[ls + 2]]) continue;
				// Both movements have same direction
				if (asmooth[cpv][s2] != asmooth[cpv][fli2[ls + 2]]) continue;
				// Mixed rhythm
				if (llen[ls] != npm / 4 || llen[ls] != llen[ls + 1] ||
					llen[ls] != llen[ls + 2] ||
					(llen[ls] != llen[ls + 3] && (ep2 == c_len || ls < fli_size - 4))) {
					if (!accept[280]) continue;
				}
				// Cross-bar
				if (bmli[s] != bmli[fli2[ls + 3]]) {
					if (!accept[281]) continue;
				}
			}
		}
		SavePattern(pDNT);
	}
}

void CGenCP1::DetectCambiata() {
	if (!accept[256]) return;
	int max_ls = fli_size - 1;
	if (ep2 == c_len) max_ls = fli_size - 3;
	for (ls = 0; ls < max_ls; ++ls) {
		if (ls < fli_size - 3 && SkipSus(3)) continue;
		s = fli[ls];
		s2 = fli2[ls];
		// Second note is upbeat (beat 1 not allowed)
		if (beat[ls+1] < 1) continue;
		// Note 1 is not a discord
		if (tivl[s] == iDis) continue;
		// Second note is created by stepwize movement from first
		if (aleap[cpv][s2]) continue;
		// Third note is created by leaping motion in same direction as second note moves
		if (!aleap[cpv][fli2[ls + 1]]) continue;
		if (asmooth[cpv][s2] * aleap[cpv][fli2[ls + 1]] < 0) continue;
		// Inverted
		if (asmooth[cpv][s2] == 1 && !accept[279]) continue;
		if (ls < fli_size - 2) {
			// Leap from second note is longer than 4th
			if (abs(ac[cpv][fli2[ls + 2]] - ac[cpv][fli2[ls + 1]]) > 3) continue;
			// Non-harmonic note is not longer than harmonic
			if (llen[ls + 1] > llen[ls]) continue;
			// Leap 4th
			if (abs(ac[cpv][fli2[ls + 2]] - ac[cpv][fli2[ls + 1]]) == 3) {
				if (!accept[263]) continue;
			}
			if (ls < fli_size - 3) {
				// Fourth note is created by direction opposite to third note movement
				if ((acc[cpv][fli[ls + 3]] - acc[cpv][fli[ls + 2]]) * aleap[cpv][fli2[ls + 1]] > 0) continue;
				// Non-harmonic note is not longer than harmonic
				if (llen[ls + 1] > llen[ls + 2]) continue;
				// Both leaps from notes 3 and 4
				if (aleap[cpv][fli2[ls + 2]] && aleap[cpv][fli2[ls + 3]]) continue;
				// Notes 2 and 3 should not be longer than halfnote
				if (llen[ls + 1] > npm / 2 || llen[ls + 2] > npm / 2) continue;
				// Mixed rhythm
				if (llen[ls] != npm / 4 || llen[ls] != llen[ls + 1] ||
					llen[ls] != llen[ls + 2] ||
					(llen[ls] != llen[ls + 3] && (ep2 == c_len || ls < fli_size - 4))) {
					if (!accept[257]) continue;
				}
				// Cross-bar
				if (bmli[s] != bmli[fli2[ls + 3]]) {
					if (!accept[259]) continue;
				}
				// Third diss
				if (tivl[fli[ls + 2]] == iDis) {
					if (!accept[261]) continue;
					// If third note is dissonance, it should not be downbeat
					if (!beat[ls + 2]) continue;
					// If third note is dissonance, it should resolve back stepwize
					if (aleap[cpv][fli2[ls + 1]] * asmooth[cpv][fli2[ls + 2]] >= 0) continue;
					// Non-harmonic note is not longer than harmonic
					if (llen[ls + 2] > llen[ls]) continue;
					if (llen[ls + 2] > llen[ls + 3] && (ep2 == c_len || ls < fli_size - 4)) continue;
					// Fourth note should not be dissonance
					if (tivl[fli[ls + 3]] == iDis) continue;
				}
				// Leap from note 3
				if (aleap[cpv][fli2[ls + 2]]) {
					if (!accept[264]) continue;
					// Leap too long
					if (abs(acc[cpv][fli2[ls + 3]] - acc[cpv][fli2[ls + 2]]) > cambiata_max_leap3) continue;
				}
				if (ls < fli_size - 4) {
					// Leap from note 4
					if (aleap[cpv][fli2[ls + 3]]) {
						if (!accept[265]) continue;
						// Leap too long
						if (abs(acc[cpv][fli2[ls + 4]] - acc[cpv][fli2[ls + 3]]) > cambiata_max_leap4) continue;
					}
					// Fourth note moves back
					if (ls < fli_size - 4 && (acc[cpv][fli[ls + 4]] - acc[cpv][fli[ls + 3]]) *
						(acc[cpv][fli[ls + 3]] - acc[cpv][fli[ls + 2]]) < 0) {
						if (!accept[266]) continue;
					}
				}
			}
		}
		SavePattern(pCam);
		//if (task == tEval && ep2 == c_len) {
		//	CString est;
		//	est.Format("Detected cambiata at %d:%d", cantus_id + 1, s + 1);
		//	WriteLog(1, est);
		//}
	}
}

void CGenCP1::SavePattern(int pattern) {
	if (pat[ls]) {
		CString est;
		est.Format("Pattern vector %d (%d -> %d) overwrite detected at %d:%d (ep2=%d)",
			ls, pat[ls], pattern, cantus_id + 1, s + 1, ep2);
		WriteLog(1, est);
	}
	pat[ls] = pattern;
}

void CGenCP1::DetectPatterns() {
	CHECK_READY_PERSIST(DR_mli);
	CHECK_READY(DR_beat, DR_ivl);
	CHECK_READY(DR_fli, DR_leap, DR_c);
	SET_READY(DR_pat);
	// Detect no patterns for species 1
	if (species == 1) return;
	fill(pat.begin(), pat.end(), 0);
	fill(pat_state.begin(), pat_state.end(), 0);
	DetectPDD();
	// Detect next patterns only for species 3 and 5
	if (species != 3 && species != 5) return;
	DetectCambiata();
	DetectDNT();
}

void CGenCP1::GetBasicRpos() {
	CHECK_READY_PERSIST(DR_c);
	CHECK_READY(DR_fli, DR_leap);
	SET_READY(DR_rposb);
	// Main calculation
	rposb[0] = pDownbeat;
	for (ls = 1; ls < fli_size; ++ls) {
		s = fli[ls];
		s2 = fli2[ls];
		if ((s + fn) % npm == 0) rposb[ls] = pDownbeat;
		else if (s > 0 && aleap[cpv][s - 1]) rposb[ls] = pLeap;
		else if (s2 < ep2 - 1 && aleap[cpv][s2]) rposb[ls] = pLeap;
		else {
			if (s > 0 && s2 < ep2 - 1 && ac[cpv][s - 1] == ac[cpv][s2 + 1]) rposb[ls] = pAux;
			else rposb[ls] = pPass;
		}
	}
}

void CGenCP1::SetRpos(int ls, vector<int> &l_rpos, int val) {
	// Check if in range
	if (ls >= fli_size) return;
	// Detect changing sign for dissonance
	if (l_rpos[ls] * val < 0 && !tivl[fli[ls]]) {
		CString est;
		est.Format("Detected rpos overwrite at note %d:%d (%s) with value %d (old value %d): %s", 
			cantus_id + 1, ls + 1, tivl[fli[ls]]?"non-Dis":"Dis", 
			val, l_rpos[ls], vint2st(ep2, acc[cpv]));
		WriteLog(5, est);
	}
	l_rpos[ls] = val;
}

void CGenCP1::ApplyPDD(int ls, vector<int> &l_rpos, int state) {
	SetRpos(ls + 1, l_rpos, -11);
	SetRpos(ls + 2, l_rpos, 12);
	pat_state[ls] = state;
}

void CGenCP1::ApplyDNT(int ls, vector<int> &l_rpos, int state) {
	SetRpos(ls,     l_rpos, 21);
	SetRpos(ls + 1, l_rpos, -22);
	SetRpos(ls + 2, l_rpos, -23);
	SetRpos(ls + 3, l_rpos, 24);
	pat_state[ls] = state;
}

void CGenCP1::ApplyCam(int ls, vector<int> &l_rpos, int state) {
	SetRpos(ls, l_rpos, 31);
	SetRpos(ls + 1, l_rpos, -32);
	pat_state[ls] = state;
}

// Cambiata with third note non-harmonic allowed
void CGenCP1::ApplyCam2(int ls, vector<int> &l_rpos, int state) {
	SetRpos(ls, l_rpos, 41);
	SetRpos(ls + 1, l_rpos, -42);
	SetRpos(ls + 2, l_rpos, -43);
	SetRpos(ls + 3, l_rpos, 44);
	pat_state[ls] = state;
	// Do not need to set note 1, because it can be dissonant
	// Do not need to set note 4, because it can be dissonant if note 3 is consonant
	// Do not need to set note 4, because if it is stepwize after dissonant, it can be only consonant. If it is not stepwize, it is not resolution
}

void CGenCP1::ApplyFixedPat() {
	CHECK_READY(DR_rposb, DR_ivl);
	CHECK_READY(DR_fli);
	SET_READY(DR_rpos, DR_rposf);
	if (species == 1) {
		for (int ls = 0; ls < fli_size; ++ls) rpos[ls] = rposb[ls];
		return;
	}
	// Clear rposf
	fill(rposf.begin(), rposf.end(), 0);
	// Walk through patterns
	for (int ls = 0; ls < fli_size; ++ls) {
		if (pat[ls] == 0) continue;
		if (pat[ls] == pPDD) {
			if (tivl[fli[ls + 1]] == iDis) ApplyPDD(ls, rposf, 1);
		}
		else if (pat[ls] == pDNT) {
			if (tivl[fli[ls + 1]] == iDis || tivl[fli[ls + 2]] == iDis) {
				ApplyDNT(ls, rposf, 1);
			}
		}
		else if (pat[ls] == pCam) {
			if (tivl[fli[ls + 2]] == iDis) ApplyCam2(ls, rposf, 1);
			else if (tivl[fli[ls + 1]] == iDis) ApplyCam(ls, rposf, 1);
		}
	}
	// Set rposf for empty
	for (int ls = 0; ls < fli_size; ++ls) if (!rposf[ls]) rposf[ls] = rposb[ls];
	// Set rpos
	for (int ls = 0; ls < fli_size; ++ls) rpos[ls] = rposf[ls];
}

// Fail rhythm for species 3
int CGenCP1::FailRhythm3() {
	CHECK_READY(DR_fli, DR_beat, DR_sus);
	if (species != 3) return 0;
	// Last measure not whole
	if (c_len - fli[fli_size - 1] < npm) {
		FLAG2(267, c_len - npm)
		if (c_len - fli[fli_size - 1] == 1) FLAG2(252, fli[fli_size - 1])
	}
	for (ls = 0; ls < fli_size; ++ls) {
		s = fli[ls];
		// Note longer than whole
		if (llen[ls] > 4 && ls < fli_size - 1) FLAG2(274, s);
		// 1/4 syncope (not for last 1/4 because it is applied with anticipation or sus)
		if (beat[ls] == 3 && llen[ls] > 1) FLAG2(235, s);
		// 1/2 after 1/4
		if (ls > 0 && beat[ls] == 1 && llen[ls] > 1 && llen[ls - 1] == 1) {
			if (s / npm >= c_len / npm - 1) FLAG2(238, s)
				// Flag slurred if sus or note is cut by scan window
			else if (sus[ls] || (ls == fli_size - 1 && c_len > ep2)) FLAG2(239, s)
			else FLAG2(240, s);
		}
		// Non-uniform starting rhythm
		if (s / npm == 0 && llen[ls] > 1) FLAG2(254, s);
	}
	return 0;
}

// Fail rhythm for species 5
int CGenCP1::FailRhythm5() {
	CHECK_READY_PERSIST(DR_mli);
	CHECK_READY(DR_fli, DR_leap);
	CHECK_READY(DR_sus);
	if (species != 5) return 0;
	// Rhythm id
	vector<int> rid;
	int rid_cur = 0;
	int count8;
	// Note lengths inside measure
	vector<int> l_len;
	vector<int> l_ls;
	l_len.resize(8);
	l_ls.resize(8);
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
	// Starting step of measure
	int mstart = 0;
	// Length sum
	int suml = 0;
	int ls2 = 0;
	// Check pause length
	if (fn * 2 > npm) FLAG2(197, 0);
	for (int ms = 0; ms < mli.size(); ++ms) {
		s = mli[ms];
		if (s >= ep2) break;
		ls = bli[s];
		l_len.clear();
		l_ls.clear();
		slur1 = 0;
		slur2 = 0;
		pos = 0;
		mstart = 0;
		// First pause
		if (!ms && fn) {
			l_len.push_back(fn);
			l_ls.push_back(0);
			pos = fn;
			mstart = fn;
		}
		// Build note lengths
		full_measure = 0;
		for (ls2 = ls; ls2 < fli_size; ++ls2) {
			// Note longer than whole
			if (llen[ls2] > 8 && ls2 == ls) FLAG2(274, fli[ls2]);
			if (!ms && fn) pos = fn + max(0, fli[ls2] - s);
			else pos = max(0, fli[ls2] - s);
			// Do not process last note if not full melody generated
			if (ep2 != c_len && ls2 == fli_size - 1) {
				// Last measure without whole note
				if (ms == mli.size() - 1 && l_len.size()) FLAG2(267, s);
				// Whole inside if it starts not from first measure, from first step and is not a suspension
				if (llen[ls2] >= 8 && ms && !pos && !sus[ls2]) FLAG2(236, s) 
				// 1/8 syncope
				else if (llen[ls2] > 1 && pos % 2) FLAG2(232, fli[ls2])
				// 1/4 syncope
				else if (llen[ls2] > 2 && pos % 4 == 2) FLAG2(235, fli[ls2])
				full_measure = 0;
				break;
			}
			s2 = fli[ls2];
			l_len.push_back(llen[ls2]);
			l_ls.push_back(ls2);
			// Stop if out of measure
			if (mstart + fli2[ls2] - s + 1 >= npm) {
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
			l_len[l_len.size()-1] = min(8, sus[ls2] - s2);
			slur2 = fli2[ls2] - sus[ls2] + 1;
		}
		// Full evaluation?
		if (ep2 == c_len) {
			// Last measure
			if (ms == mli.size() - 1) {
				// Check last whole note (really 1/8)
				if (l_len[0] != 8) FLAG2(267, s);
			}
		}
		// Set first rhythm id bit
		rid_cur = slur1?0:1;
		// Iterative rhythm checks
		count8 = 0;
		pos = 0;
		for (int lp = 0; lp < l_len.size(); ++lp) {
			if (!ms && pos >= fn) s2 = s + pos - fn; 
			else s2 = s + pos;
			ls2 = bli[s2];
			// Last note
			if (ep2 == c_len && ls2 == fli_size - 1) {
				// Check length
				if (l_len[lp] == 1) FLAG2(253, s2)
				else if (l_len[lp] == 2) FLAG2(252, s2)
			}
			// Calculate rhythm id
			if (lp < l_len.size() - 1 || !slur2)
				rid_cur += 1 << (pos + l_len[lp]);
			// Check 1/8
			if (l_len[lp] == 1) {
				// Last 1/8 syncope
				if (pos == 7 && slur2) FLAG2(232, s2)
				// Other types of 1/8
				else {
					// If second 1/8
					if (pos % 2) {
						// Isolated 1/8
						if (l_len[lp - 1] != 1) FLAG2(231, s2)
					}
					// Too many 1/8
					++count8;
					if (count8 == 3) FLAG2(255, s2)
					else if (count8 > 3) ++fpenalty[255];
					// 1/8 in first measure
					if (ms == 0) FLAG2(230, s2)
						// If first 8th
					else {
						// 1/8 beats
						if (pos == 0) FLAG2(226, s2)
						else if (pos == 2) FLAG2(227, s2)
						else if (pos == 4) FLAG2(228, s2)
						else if (pos == 6) FLAG2(229, s2)
					}
				}
				// 1/8 on leap
				if ((ls2 < fli_size - 1 && aleap[cpv][s2]) || (ls2 > 0 && aleap[cpv][s2 - 1])) FLAG2(88, s2);
			}
			else {
				// 1/8 syncope
				if (pos % 2) FLAG2(232, s2)
				// 1/4 syncope
				else if (l_len[lp] > 2 && pos == 2) FLAG2(235, s2)
				else if (l_len[lp] == 2 && pos == 6 && slur2) FLAG2(235, s2);
			}
			// Uneven starting rhythm
			if (!ms && lp>0 && l_len[lp] != l_len[lp-1]) FLAG2(254, s2);
			pos += l_len[lp];
		}
		// Check rhythm repeat
		if (full_measure) {
			if (rid.size()) {
				if (rid.back() == rid_cur) FLAG2(247, s);
			}
			rid.push_back(rid_cur);
		}
		// Check rhythm rules
		// First measure
		if (!ms) {
			// Uneven pause
			if (l_len.size() > 1 && l_len[0] == fn && l_len[0] != l_len[1]) FLAG2(237, s);
		}
		// Whole inside
		if (l_len[0] >= 8 && ms < mli.size() - 1 && ms) FLAG2(236, s)
		// 1/2.
		else if (l_len[0] == 6 && !slur1) FLAG2(233, s)
		else if (l_len.size() > 1 && l_len[1] == 6) FLAG2(234, fli[ls + 1])
		else if (l_len.size() > 2 && l_len[2] == 6) FLAG2(234, fli[ls + 2])
		// 1/2 after 1/4 or 1/8 in measure
		else if (full_measure && l_len[l_len.size() - 1] == 4 && l_len[0] != 4) {
			s3 = fli[l_ls[l_ls.size() - 1]];
			if (ms >= mli.size() - 2) FLAG2(238, s3)
			else if (slur2 != 0) FLAG2(239, s3)
			else if (slur1 != 0) FLAG2(278, s3)
			else FLAG2(240, s3);
		}
		// Many notes in measure
		if (l_len.size() == 5) FLAG2(245, s)
		else if (l_len.size() > 5) FLAG2(246, s);
		// Suspensions
		if (slur1 == 4 && l_len[0] == 2) FLAG2(241, s)
		else if (slur1 == 4 && l_len[0] == 4) FLAG2(242, s)
		else if (slur1 == 2) FLAG2(251, s)
		if (slur1 && l_len[0] == 6) FLAG2(243, s)
		if (slur1 == 6) FLAG2(244, s);
	}
	// Check last measure
	return 0;
}

int CGenCP1::FailPcoApart() {
	CHECK_READY(DR_fli, DR_ivl, DR_beat);
	CHECK_READY(DR_rpos);
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
					else if (rpos[ls] < 0 || rpos[bli[pco8_last]] < 0) FLAG2(249, s)
					else FLAG2(250, s);
				}
			}
			pco8_last = s;
		}
	}
	return 0;
}

int CGenCP1::FailVIntervals() {
	CHECK_READY(DR_fli, DR_ivl, DR_rpos);
	CHECK_READY(DR_motion, DR_culm_ls, DR_sus);
	// Number of sequential parallel imperfect consonances
	int pico_count = 0;
	// Check first step
	if (tivl[0] == iDis) FLAG2(83, 0);
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

void CGenCP1::SaveCP() {
	if (method == mScan) dpenalty_cur = dpenalty_step[c_len - 1];
	if (!dpenalty_cur) dpenalty_cur = CalcDpenalty(cpoint[cantus_id][cpv], acc[cpv], 0, c_len - 1);
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
			//LogCantus("Animate", clib.size(), acc[cpv]);
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

void CGenCP1::SaveCPIfRp() {
	CHECK_READY(DR_rpenalty_cur);
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
	// For species 5 there are separate rules (FailRhythm5)
	if (species == 5) return 0;
	// Number of sequential slurs 
	int scount = 0;
	// Number of slurs in window
	int scount2 = 0;
	int max_count = 0;
	int max_i = 0;
	// Check pause length
	if (fn*2 > npm) FLAG2(197, 0);
	// Do not process last measure, because it can be whole note
	int max_step = min(ep2 - 1, mli[mli.size()-1]);
	for (int i = 0; i < max_step; ++i) {
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
			if ((i >= slurs_window) && (acc[cpv][i - slurs_window] == acc[cpv][i - slurs_window + 1])) 
				--scount2;
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
	for (int i = 0; i < ep2-1; ++i) if ((i + fn) % 2) { 
		if (i < miss_slurs_window * npm) ++wcount;
		// Subtract old slur
		if ((i >= miss_slurs_window * npm) && 
			(acc[cpv][i - miss_slurs_window * npm] == acc[cpv][i - miss_slurs_window * npm + 1])) 
			--scount;
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
	CHECK_READY(DR_fli);
	int bsteps = 0;
	for (ls = 0; ls < fli_size; ++ls) {
		s = fli[ls];
		// Check between
		if (acc[1][s] - acc[0][s] > max_between) {
			++bsteps;
			// Flag very far burst
			if (acc[1][s] - acc[0][s] > burst_between) FLAG2(11, s);
			if (bsteps > burst_steps) {
				// Flag long burst only on first overrun
				if (bsteps == burst_steps + 1) FLAG2(11, s)
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
	CHECK_READY(DR_fli);
	// Do not check overlap for 1 note
	if (fli_size < 3) return 0;
	if (cantus_high) {
		for (int i = fli[1]; i < ep2; ++i) {
			if (acc[cpv][i] == acc[cpv][i - 1] || acc[cfv][i] == acc[cfv][i - 1]) continue;
			if (acc[cpv][i] >= acc[cfv][i - 1]) FLAG2(24, i)
			else if (acc[cfv][i] <= acc[cpv][i - 1]) FLAG2(24, i);
		}
	}
	else {
		for (int i = fli[1]; i < ep2; ++i) {
			if (acc[cpv][i] == acc[cpv][i - 1] || acc[cfv][i] == acc[cfv][i - 1]) continue;
			if (acc[cpv][i] <= acc[cfv][i - 1]) FLAG2(24, i)
			else if (acc[cfv][i] >= acc[cpv][i - 1]) FLAG2(24, i);
		}
	}
	return 0;
}

// Create random cantus and optimize it using SWA
void CGenCP1::RandomSWACP() {
	// Init animation
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
		task = tGen;
		MakeNewCP();
		task = tCor;
		EmulateSASCP();
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
		smatrixc = c_len;
		smatrix.clear();
		smatrix.resize(c_len, 0);
		// Do not scan first pause
		for (int x = 0; x < c_len; ++x) {
			smatrix[x] = 1;
		}
		// Optimize cpoint
		rpenalty_cur = MAX_PENALTY;
		step0 = step;
		correct_start_time = time();
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
				EmulateSASCP();
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
	if (!show_correct_hatch) return;
	// Add lining
	int pos = step0 + fn;
	int lni;
	for (int z = 0; z < c_len; z++) {
		if (cpoint[cantus_id][cpv][z] != cc[z]) lni = HatchStyleLightUpwardDiagonal;
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
	swa_len = 1;
	// Save source rpenalty
	rpenalty_source = rpenalty_cur;
	long cnum = 0;
	vector<int> animate_cc; // This is cc that was sent in previous animation step
	// Save cantus only if its penalty is less or equal to source rpenalty
	rpenalty_min = rpenalty_cur;
	best_flags = flags;
	dpenalty_min = 0;
	acc = cpoint[i];
	swa_full = 0;
	best_flags.clear();
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
		CheckClibSize();
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
		if (debug_level > 1) {
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
		if (acc[cfv].size() > 60 || swa_len > 0) {
			st.Format("SWA%d #%d RP %.0f DP %d", swa_len, a+1, rpenalty_min, dpenalty_min);
			SetStatusText(4, st);
		}
		// Animation
		long long time = CGLib::time();
		int acy = 0;
		if (animate) acy = (time - gen_start_time) / animate;
		if (!animate || acy > acycle) {
			if (debug_level > 2) {
				CString est;
				est.Format("Animation at SWA%d #%d: rp %.0f from %.0f, dp %d, cnum %ld",
					swa_len, a + 1, rpenalty_min, rpenalty_source, dpenalty_min, cnum);
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
				//LogCantus("Animate SWA", cantus_id, acc[cpv]);
				is_animating = 0;
				step = step0;
				ValidateVectors(step0, t_generated - 1);
				//t_sent = t_generated;
				// Delay only if cc changed
				Sleep(animate_delay);
				animate_cc = acc[cpv];
			}
		}
		if (max_correct_ms && time - correct_start_time > max_correct_ms) break;
		if (dp) {
			// Abort SWA if dpenalty and rpenalty not decreasing
			if (rpenalty_min >= rpenalty_min_old && dpenalty_min >= dpenalty_min_old) {
				if (swa_len >= swa_steps || swa_len >= smap.size()) {
					swa_full = 1;
					if (swa_len >= smap.size()) swa_full = 2;
					break;
				}
				++swa_len;
			}
		}
		else {
			// Abort SWA if rpenalty zero or not decreasing
			if (!rpenalty_min) break;
			if (rpenalty_min >= rpenalty_min_old) {
				if (swa_len >= swa_steps) {
					// Record SWA stuck flags
					if (best_flags.size()) for (int x = 0; x < max_flags; ++x) {
						if (best_flags[x]) ++ssf[x];
					}
					swa_full = 1;
					break;
				}
				else ++swa_len;
			}
		}
	}
	// Log
	long long time_stop = CGLib::time();
	CString est;
	// For successful rpenalty_cur == 0, show last flag that was fixed. For unsuccessful, show best variant
	CString sst = GetStuck();
	est.Format("Finished SWA%d #%d: rp %.0f from %.0f, dp %d, cnum %ld (in %d ms): %s",
		swa_len, a+1, rpenalty_min, rpenalty_source, dpenalty_min, cnum, time_stop - time_start, sst);
	WriteLog(0, est);
	TestBestRpenalty();
}

int CGenCP1::FailFirstIntervals() {
	CHECK_READY(DR_fli, DR_pc);
	if (apc[0][0] == 0) {
		if (apc[1][0] == 0) FLAG2(268, 0)
		else if (apc[1][0] == 4) FLAG2(269, 0)
		else if (apc[1][0] == 2) FLAG2(270, 0)
		else FLAG2(272, 0)
	}
	else if (apc[0][0] == 2) {
		if (apc[1][0] == 0) FLAG2(271, 0)
		else FLAG2(272, 0)
	}
	else FLAG2(272, 0);
	return 0;
}

int CGenCP1::FailLastIntervals() {
	CHECK_READY_PERSIST(DR_mli);
	CHECK_READY(DR_fli, DR_pc);
	// Do not check if melody is short yet
	if (fli_size < 3) return 0;
	// Prohibit last note not tonic
	if (ep2 >= c_len) {
		s = fli[fli_size - 1];
		s_1 = fli[fli_size - 2];
		s_2 = fli[fli_size - 3];
		if (apc[cpv][c_len - 1] != 0) {
			// Detect upper last G if lower note is C
			if (apcc[1][c_len - 1] == 7 && apc[0][c_len - 1] == 0) 
				FLAG2(33, s)
			// Detect upper last E if lower note is C
			else if ((apcc[1][c_len - 1] == 3 || apcc[1][c_len - 1] == 4) && apc[0][c_len - 1] == 0)
				FLAG2(87, s)
			else FLAG2(50, s);
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
					if (apc[0][c_len - 1] != 0 || apc[0][fli[fli_size - 2]] != 4) FLAG2(47, start);
				}
				if (!g_found && !d_found) FLAG2(75, start);
			}
		}
	}
	return 0;
}

void CGenCP1::GetNoteTypes() {
	CHECK_READY(DR_fli);
	SET_READY(DR_beat, DR_sus);
	int s = 0, sf, sl;
	int l;
	for (ls = 0; ls < fli_size; ++ls) {
		s = fli[ls];
		sf = s + fn;
		l = llen[ls];
		sl = sf - (sf / npm) * npm;
		// Get beat
		if (sf % npm) {
			if (npm>2 && sf % (npm / 2)) {
				if (npm>4 && sf % (npm / 4)) {
					beat[ls] = 10 + sl / (npm / 4);
				}
				else beat[ls] = 2 + sl / (npm / 4);
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
	SET_READY(DR_uli);
	vector<int> prev_note;
	prev_note.resize(av_cnt, -1);
	int changed;
	uli.clear();
	for (int i = 0; i < ep2; ++i) {
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
	SET_READY_PERSIST(DR_mli);
	mli.clear();
	if (fn) mli.push_back(0);
	for (int i = 0; i < c_len; ++i) {
		// Find measures
		if ((i + fn) % npm == 0) {
			mli.push_back(i);
		}
		bmli[i] = mli.size() - 1;
	}
}

int CGenCP1::FailStartPause() {
	if (species == 1 && fn > 0) {
		// Show warning and exit if not evaluating
		if (task != tEval&& !warn_wrong_fn) {
			CString est;
			est.Format("Excessive pause prevents from generating any results in counterpoint species %d, because starting pause is set to %d/%d",
				species, fn, npm);
			WriteLog(5, est);
			++warn_wrong_fn;
			return 1;
		}
	}
	else if (species > 1 && fn == 0) {
		// Show warning and exit if not evaluating
		if (task != tEval && !accept[273] && !warn_wrong_fn) {
			CString est;
			est.Format("Rule '%s - %s' prevents from generating any results in counterpoint species %d, because there is no starting pause",
				RuleName[273], SubRuleName[273], species);
			WriteLog(5, est);
			++warn_wrong_fn;
			return 1;
		}
		FLAG2(273, 0)
	}
	return 0;
}

void CGenCP1::EmulateSASCP() {
	if (v_cnt == 2) {
		// Evaluate for CP1
		scpoint = acc;
		ScanCP(tEval, -1);
	}
	if (need_exit) return;
	// Save full analysis flags
	nflags_full = anflags[cpv];
	flags_full = flags;
	nflags_prev.clear();
	for (fixed_ep2 = 1; fixed_ep2 <= acc[cpv].size(); ++fixed_ep2) {
		// Show emulator status
		CString est;
		est.Format("SAS emulator: %d of %d", fixed_ep2, acc[cpv].size());
		SetStatusText(7, est);
		// Visible emulation
		if (emulate_sas) {
			step0 = step;
			FillPause(step0, floor((real_len + 1) / 8 + 1) * 8, 0);
			FillPause(step0, floor((real_len + 1) / 8 + 1) * 8, 1);
			if (v_cnt == 4) {
				FillPause(step0, floor((real_len + 1) / 8 + 1) * 8, 2);
				FillPause(step0, floor((real_len + 1) / 8 + 1) * 8, 3);
			}
			ScanCP(tEval, 0);
		}
		// Hidden emulation
		else {
			ScanCP(tEval, -1);
		}
		if (need_exit) break;
		CheckSASEmulatorFlags();
		nflags_prev = anflags[cpv];
	}
	if (!need_exit) OutputFlagDelays();
	fixed_ep2 = 0;
	SetStatusText(7, "SAS emulator: finished");
}

// Get links to cantus notes
void CGenCP1::GetCfli() {
	SET_READY_PERSIST(DR_cfli);
	cfli.clear();
	int last_note = -1;
	for (s = 0; s < c_len; ++s) {
		if (acc[cfv][s] != last_note) {
			last_note = acc[cfv][s];
			cfli.push_back(s);
		}
	}
}

int CGenCP1::FailGisTrail2() {
	CHECK_READY(DR_fli, DR_pc);
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
	CHECK_READY(DR_fli);
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
	// We can call get measures and cfli once, because they are calculated within c_len
	GetMeasures();
	GetCfli();
	// Convert cf culmination to steps
	cf_culm_s = -1;
	if (cf_culm_cfs > -1) cf_culm_s = cfli[cf_culm_cfs];
	// Check for wrong fn
	if (FailStartPause()) return;
	// Analyze combination
check:
	while (true) {
		CLEAR_READY();
		//LogCantus("sp2-swa2-ep2", ep2 + swa2 * 1000 + sp2 * 1000000, acc[cpv]);
		//if (ep2 > 56 && MatchVectors(acc[cpv], test_cc, 2, ep2 - 1)) {
			//CString est;
			//est.Format("Found ep2 %d", ep2);
			//WriteLog(1, est);
		//}
		// Check if dpenalty is already too high
		if (task == tCor && !rpenalty_min) {
			if (method == mScan) {
				CalcStepDpenalty(cpoint[cantus_id][cpv], acc[cpv], ep2 - 1);
				if (dpenalty_step[ep2 - 1] > dpenalty_min) goto skip;
			}
			else {
				dpenalty_cur = dpenalty_outside_swa + CalcDpenalty(cpoint[cantus_id][cpv], acc[cpv], smap[swa1], smap[sp2 - 1]);
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
			// Still skipping even when evaluating to get error if wrong flag set
			if (task == tEval)
				if (FailStartPause()) goto skip;
			if (nmax - nmin > max_interval) FLAG(37, 0);
			if (cantus_high) {
				if (cf_nmax - nmin > sum_interval) FLAG(7, 0);
			}
			else {
				if (nmax - cf_nmin > sum_interval) FLAG(7, 0);
			}
			if (c_len == ep2 && nmax - nmin < min_interval) FLAG(38, 0);
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
		// Save last second for analysis
		if (m_testing && task == tCor && m_algo_id != 121 && 
			time - gen_start_time > (m_test_sec - ANALYZE_RESERVE) * 1000) break;
		// Limit SAS correction time
		if (task == tCor && max_correct_ms && time - correct_start_time > max_correct_ms) break;
		if (FailDiatonic(ac[cpv], acc[cpv], 0, ep2, minor_cur)) goto skip;
		GetPitchClass(ac[cpv], acc[cpv], apc[cpv], apcc[cpv], 0, ep2);
		CreateLinks(acc[cpv], 1);
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
		GetVIntervals();
		DetectPatterns();
		GetBasicRpos();
		ApplyFixedPat();
		if (FailMultiCulm(acc[cpv], aslur[cpv])) goto skip;
		if (FailVMotion()) goto skip;
		if (FailVIntervals()) goto skip;
		if (FailTonic(acc[cpv], apc[cpv])) goto skip;
		if (FailLastIntervals()) goto skip;
		//if (FailNoteSeq(apc[cpv])) goto skip;
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
		if (FailFirstIntervals()) goto skip;
		if (FailLeap(ac[cpv], aleap[cpv], asmooth[cpv], nstat2, nstat3)) goto skip;
		//if (FailMelodyHarm(apc[cpv], 0, ep2)) goto skip;
		MakeMacc(acc[cpv]);
		if (FailLocalMacc(notes_arange, min_arange, 15)) goto skip;
		if (FailLocalMacc(notes_arange2, min_arange2, 16)) goto skip;
		if (FailHarm()) goto skip;

		//LogCantus("Rpenalty", rpenalty_cur, flags);
		SaveBestRejected(acc[cpv]);
		if (task == tCor && method == mSWA) {
			if (skip_flags) {
				if (ep2 < smap[swa2 - 1] + 1) {
					NextWindow(acc[cpv]);
					goto check;
				}
			}
			else {
				CalcRpenalty(acc[cpv]);
				if (ep2 < smap[swa2 - 1] + 1) {
					if (rpenalty_cur > src_rpenalty_step[smap[swa1]]) goto skip;
					NextWindow(acc[cpv]);
					goto check;
				}
			}
		}
		// If we are window-scanning
		else if (task == tGen || task == tCor) {
			++accepted2;
			CalcFlagStat();
			if (FailFlagBlock()) goto skip;
			if (FailAccept()) goto skip;
			++accepted4[wid];
			//LogCantus(acc[cpv]);
			// If this is not last window, go to next window
			if (ep2 < c_len) {
				NextWindow(acc[cpv]);
				goto check;
			}
			// Check random_choose
			if (random_choose < 100) if (rand2() >= (float)RAND_MAX*random_choose / 100.0) goto skip;
		}
		else {
			// Evaluation
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
					ReseedCP();
					// Start evaluating without scan
					goto check;
				}
			}
			else {
				// Calculate dpenalty if this is evaluation
				if (task == tEval && cpoint.size()) dpenalty_cur = CalcDpenalty(cpoint[cantus_id][cpv], acc[cpv], 0, c_len - 1);
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
			// Finish if this is last variant in first window and not SWA
			if ((p == 0) || (wid == 0)) {
				// Sliding Windows Approximation
				if (method == mSWA) {
					if (NextSWA(acc[cpv], acc_old[cpv])) {
						scan_full = 1;
						break;
					}
					goto check;
				}
				if (random_seed && random_range && accept_reseed) {
					// Infinitely cycle through ranges
					ReseedCP();
					// Start evaluating without scan
					goto check;
				}
				WriteLog(0, "Last variant in first window reached");
				scan_full = 1;
				break;
			}
			ShowBestRejectedCP();
			BackWindow(acc[cpv]);
			// Goto next variant calculation
			goto skip;
		} // if (finished)
		ScanRight(acc[cpv]);
	}
	if (accepted3 > 100000) {
		ShowScanStatus();
		WritePerfLog();
	}
	WriteFlagCor();
	ShowFlagStat();
	ShowFlagBlock();
	tcycle += cycle;
}

void CGenCP1::LoadCantusHigh() {
	CString st;
	specified_high = 0;
	if (cantus_incom[cantus_id].size()) {
		if (cantus_incom[cantus_id][0].Find("cf=low") != -1) {
			cantus_high = 0;
			specified_high = 1;
		}
		else if (cantus_incom[cantus_id][0].Find("cf=high") != -1) {
			cantus_high = 1;
			specified_high = 1;
		}
		else {
			specified_high = 0;
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
	if (cantus_incom[cantus_id].size()) {
		if (cantus_incom[cantus_id][0].Find("sp1") != -1) species = 1;
		else if (cantus_incom[cantus_id][0].Find("sp2") != -1) species = 2;
		else if (cantus_incom[cantus_id][0].Find("sp3") != -1) species = 3;
		else if (cantus_incom[cantus_id][0].Find("sp4") != -1) species = 4;
		else if (cantus_incom[cantus_id][0].Find("sp5") != -1) species = 5;
	}
}

void CGenCP1::ShrinkCantus() {
	int l, min_len, max_len;
	// Detect minimum note length
	for (int i = 0; i < cantus.size(); ++i) {
		min_len = INT_MAX;
		max_len = 1;
		// Get lengths
		for (s = 0; s < cantus[i].size(); ++s) {
			if (cantus_len[i][s] < min_len) min_len = cantus_len[i][s];
			if (cantus_len[i][s] > max_len) max_len = cantus_len[i][s];
		}
		// Check lengths
		// Shrink cantus
		for (s = 0; s < cantus[i].size(); ++s) {
			l = cantus_len[i][s] / min_len;
			cantus_tempo[i][s] = cantus_tempo[i][s] * l / cantus_len[i][s];
			cantus_len[i][s] = l;
		}
	}
}

void CGenCP1::Generate() {
	//CString st = "57 59 60 62 64 65 67 69 71 69 66 68 69 67 65 64 65 67 69 71 72 60 64 65 67 64 65 67 65 64 62 65 64 57 59 60 62 64 66 68 69";
	//test_cc.resize(41);
	//StringToVector(&st, " ", test_cc);

	if (error) return;
	if (InitCP()) return;
	int fn0 = fn;
	SetStatusText(8, "MIDI file: " + fname_from_path(midi_file));
	LoadCantus(midi_file);
	ShrinkCantus();
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
		fn = fn0;
		vector<int> cc_len_old = cc_len;
		vector<float> cc_tempo_old = cc_tempo;
		vector<vector<int>> anflags_old = anflags[cfv];
		c_len = m_c.size() * npm - (npm - 1) - fn;
		ac[cfv].clear();
		acc[cfv].clear();
		apc[cfv].clear();
		apcc[cfv].clear();
		cc_len.clear();
		cc_tempo.clear();
		anflags[cfv].clear();
		// Create empty arrays
		anflags[cfv].resize(m_c.size()*npm);
		int npm2;
		for (int i = 0; i < m_c.size(); ++i) {
			npm2 = npm;
			// First measure should have less notes
			if (i == 0) npm2 = npm - fn;
			for (int x = 0; x < npm2; ++x) {
				ac[cfv].push_back(m_c[i]);
				acc[cfv].push_back(m_cc[i]);
				apc[cfv].push_back(m_pc[i]);
				apcc[cfv].push_back(m_pcc[i]);
				cc_len.push_back(cc_len_old[i]);
				cc_tempo.push_back(cc_tempo_old[i]);
				if (!x) {
					int y = i*npm + x;
					anflags[cfv][y] = anflags_old[i];
				}
			}
		}
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
	LogPerf();
}
