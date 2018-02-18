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
	CheckVar(sN, sV, "species", &species, 0, 5);

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
	anfl.resize(av_cnt);
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
		anfl[cpv][i].clear();
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
			// Next line is always false here, but this code is part of consistent solution
			//if (swa1 > 0) dpenalty_outside_swa += CalcDpenalty(cpoint[cantus_id][cpv], acc[cpv], 0, smap[swa1 - 1]);
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
	mshb.resize(c_len);
	mshf.resize(c_len);
	msh.resize(c_len);
	bhli.resize(c_len);
	pat.resize(c_len);
	pat_state.resize(c_len);
	ivl.resize(c_len);
	ivlc.resize(c_len);
	civl.resize(c_len);
	civlc.resize(c_len);
	civlc2.resize(c_len);
	tivl.resize(c_len);
	motion.resize(c_len);
	beat.resize(c_len);
	sus.resize(c_len);
	isus.resize(c_len);
	ep2 = c_len;
	voice_high = cpv;
	max_interval = max_interval_cp;
	// Calculate last steps that are allowed to have C4P
	c4p_last_steps = c4p_last_meas * npm;
	// Set green leap limits
	if (npm > 2) {
		max_leaps5 = max_leaps3;
		max_leaped5 = max_leaped3;
		max_leaps6 = max_leaps4;
		max_leaped6 = max_leaped4;
	}
	else {
		max_leaps5 = max_leaps;
		max_leaped5 = max_leaped;
		max_leaps6 = max_leaps2;
		max_leaped6 = max_leaped2;
	}
	// Set scan voices count
	svoices = av_cnt;
}

void CGenCP1::SendMsh(int pos, int i, int v, int av, int x) {
	if (show_hatch == 1) {
		if (tivl[x] == iDis) lining[pos + i][v] = HatchStyleNarrowHorizontal;
		else lining[pos + i][v] = 0;
	}
	if (show_hatch == 2) {
		if (msh[bli[x]] < 0) lining[pos + i][v] = HatchStyleLargeConfetti;
		else lining[pos + i][v] = 0;
	}
}

// Calculate parameter map
void CGenCP1::CalcPmap2() {
	CalcPmap(apcc[cpv], acc[cpv], ac[cpv], asmooth[cpv], aleap[cpv]);
	pm_sumint = max(cf_nmax, nmax) - min(cf_nmin, nmin);
	pm_dis = 0;
	pm_pco = 0;
	pm_ico = 0;
	pm_llen = 0;
	pm_croche = 0;
	pm_anti = 0;
	pm_sus = 0;
	for (ls = 0; ls < fli_size; ++ls) {
		s = fli[ls];
		// Intervals
		if (tivl[s] == iDis) ++pm_dis;
		else if (tivl[s] == iPco) ++pm_pco;
		else if (tivl[s] == iIco || tivl[s] == iHarm4) ++pm_ico;
		if (sus[ls]) {
			if (retrigger[sus[ls]]) ++pm_anti;
			else ++pm_sus;
			if (tivl[sus[ls]] == iDis) ++pm_dis;
			else if (tivl[sus[ls]] == iPco) ++pm_pco;
			else if (tivl[sus[ls]] == iIco || tivl[sus[ls]] == iHarm4) ++pm_ico;
		}
		// Length
		pm_llen += llen[ls];
		if (llen[ls] == 1) ++pm_croche;
	}
	pm_llen /= fli_size;
}

// Get parameter map string
void CGenCP1::GetPmap2() {
	CString st;
	GetPmap();
	st.Format("Sum interval: %d semitones\n", pm_sumint);
	pmap += st;
	st.Format("Min / max between voices: %d / %d semitones\n", pm_between_min, pm_between_max);
	pmap += st;
	st.Format("Contrary / direct / parallel ico motions: %d / %d / %d\n", 
		pm_contrary, pm_direct, pm_pico);
	pmap += st;
	st.Format("Dissonances / imperfect / perfect consonances: %d / %d / %d\n",
		pm_dis, pm_ico, pm_pco);
	pmap += st;
	st.Format("Av note length / croches count: %.5f / %d\n",
		pm_llen, pm_croche);
	pmap += st;
	st.Format("Suspensions / anticipations: %d / %d\n",
		pm_sus, pm_anti);
	pmap += st;
	st.Format("Compensated to 3rd / after 3rd / deviation to 2nd / deviation to 3rd: %d / %d / %d / %d\n",
		flags[100] + flags[101] + flags[102] + flags[103] + 
		flags[104] + flags[105] + flags[106] + flags[107],
		flags[53] + flags[54] + flags[55] + flags[56],
		flags[42] + flags[43] + flags[44] + flags[45],
		flags[120] + flags[121] + flags[122] + flags[123]
	);
	pmap += st;
	st.Format("Uncompensated precompensated / child: %d / %d\n",
		flags[144] + flags[145] + flags[146] + flags[147] +
		flags[112] + flags[113] + flags[114] + flags[115] +
		flags[204] + flags[205] + flags[206] + flags[207],
		flags[116] + flags[117] + flags[118] + flags[119]
	);
	pmap += st;
	st.Format("Close + 1far / 2far / no MDC: %d / %d / %d\n",
		flags[128] + flags[129] + flags[130] + flags[131],
		flags[140] + flags[141] + flags[142] + flags[143],
		flags[108] + flags[109] + flags[110] + flags[111]
	);
	pmap += st;
	st.Format("No / far + close MDC: %d / %d\n",
		flags[132] + flags[133] + flags[134] + flags[135],
		flags[59] + flags[60] + flags[61] + flags[62]
	);
	pmap += st;
	st.Format("No close MDC: %d\n",
		flags[63] + flags[64] + flags[65] + flags[66] +
		flags[148] + flags[149] + flags[150] + flags[151]
	);
	pmap += st;
}

CString CGenCP1::GetPmapLogHeader2() {
	CString st = GetPmapLogHeader();
	st += "Sum_int;Between_min;Between_max;";
	st += "Contra;Direct;Pico;";
	st += "Dis;Ico;Pco;";
	st += "Llen;Croches;Sus;Anti;";
	st += "to3;after3;dev2;dev3;uncomp_precomp;uncomp_child;";
	st += "Close+1far;Close+2far;Close+no;No+close;Far+close;No close;"; 
	return st;
}

CString CGenCP1::GetPmapLogSt2() {
	CString st, st2 = GetPmapLogSt();
	st.Format("%d;%d;%d;",
		pm_sumint, pm_between_min, pm_between_max);
	st2 += st;
	st.Format("%.0f%%;%.0f%%;%.0f%%;%.0f%%;%.0f%%;%.0f%%;",
		100.0 * pm_contrary / (pm_contrary + pm_direct), 
		100.0 * pm_direct / (pm_contrary + pm_direct), 
		100.0 * pm_pico / (pm_contrary + pm_direct),
		100.0 * pm_dis / (pm_dis + pm_ico + pm_pco), 
		100.0 * pm_ico / (pm_dis + pm_ico + pm_pco),
		100.0 * pm_pco / (pm_dis + pm_ico + pm_pco));
	st2 += st;
	st.Format("%.3f;%.0f%%;%.0f%%;%.0f%%;",
		pm_llen, 
		100.0 * pm_croche / fli_size, 
		100.0 * pm_sus / fli_size, 
		100.0 * pm_anti / fli_size);
	st2 += st;
	st.Format("%.0f%%;%.0f%%;%.0f%%;%.0f%%;%.0f%%;%.0f%%;",
		100.0 * (flags[100] + flags[101] + flags[102] + flags[103] +
		flags[104] + flags[105] + flags[106] + flags[107]) / pm_leaps,
		100.0 * (flags[53] + flags[54] + flags[55] + flags[56]) / pm_leaps,
		100.0 * (flags[42] + flags[43] + flags[44] + flags[45]) / pm_leaps,
		100.0 * (flags[120] + flags[121] + flags[122] + flags[123]) / pm_leaps,
		100.0 * (flags[144] + flags[145] + flags[146] + flags[147] +
		flags[112] + flags[113] + flags[114] + flags[115] +
		flags[204] + flags[205] + flags[206] + flags[207]) / pm_leaps,
		100.0 * (flags[116] + flags[117] + flags[118] + flags[119]) / pm_leaps
	);
	st2 += st;
	st.Format("%.0f%%;%.0f%%;%.0f%%;",
		100.0 * (flags[128] + flags[129] + flags[130] + flags[131]) / pm_leaps,
		100.0 * (flags[140] + flags[141] + flags[142] + flags[143]) / pm_leaps,
		100.0 * (flags[108] + flags[109] + flags[110] + flags[111]) / pm_leaps
	);
	st2 += st;
	st.Format("%.0f%%;%.0f%%;",
		100.0 * (flags[132] + flags[133] + flags[134] + flags[135]) / pm_leaps,
		100.0 * (flags[59] + flags[60] + flags[61] + flags[62]) / pm_leaps
	);
	st2 += st;
	st.Format("%.0f%%;",
		100.0 * (flags[63] + flags[64] + flags[65] + flags[66] +
		flags[148] + flags[149] + flags[150] + flags[151]) / pm_leaps
	);
	st2 += st;
	st2.Replace(".", ",");
	return st2;
}

// Log parameter map
void CGenCP1::LogPmap2() {
	CString fname = "log\\cp-pmap.csv";
	// Header
	if (!fileExists(fname)) {
		//AppendLineToFile(fname, GetPmapLogHeader2() + "\n");
	}
	//AppendLineToFile(fname, GetPmapLogSt2() + "\n");
}

void CGenCP1::SendHarmColorCP(int pos, int v, int chm_id) {
	DWORD fc;
	mark_color[pos][v] = MakeColor(255, 170, 170, 170);
	// Scan flags
	int s = hli[chm_id];
	int f_cnt = anflags[cpv][s].size();
	int max_severity = -1;
	int fl;
	for (int f = 0; f < f_cnt; ++f) {
		fl = anflags[cpv][s][f];
		if (rule_viz[fl] == vHarm && !accept[fl] && severity[fl] >= show_min_severity) {
			if (severity[fl] > max_severity) max_severity = severity[fl];
		}
	}
	if (max_severity > -1) {
		fc = flag_color[max_severity];
		mark_color[pos][v] = MakeColor(GetAlpha(fc), GetRed(fc),
			GetGreen(fc) / 1.5, GetBlue(fc));
	}
}

int CGenCP1::SendCP() {
	int step000 = step;
	int pause_len = 0;
	float l_rpenalty_cur;
	CString st, st2, rpst;
	int pos = 0, plen;
	int v, x1;
	int chm_id = 0;
	if (svoice < 0) return 0;
	Sleep(sleep_ms);
	long long time_start = CGLib::time();
	long long time_stop;
	len_export.resize(c_len);
	coff_export.resize(c_len);
	if (!is_animating) {
		// Warning: corack will trigger duplicate pmap logs
		CalcPmap2();
		GetPmap2();
		LogPmap2();
	}
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
			mark[pos][v].Empty();
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
			else {
				if (chm.size() > chm_id && hli[chm_id] == x) {
					mark[pos][v] = GetHarmName(chm[chm_id], chm_alter[chm_id]);
					if (show_harmony_bass && hbc[chm_id] % 7 != chm[chm_id]) {
						if (show_harmony_bass == 1) {
							mark[pos][v] += "/" +
								GetRealNoteName(hbcc[chm_id] % 12, tonic_cur, minor_cur);
						}
						else {
							/*
							if ((hbc[chm_id] % 7 - chm[chm_id] + 7) % 7 == 2) {
								mark[pos][v] += "6";
							}
							else {
								mark[pos][v] += "64";
							}
							*/
						}
					}
					SendHarmColorCP(pos, v, chm_id);
					++chm_id;
				}
			}
			SendLyrics(pos, v, av, x);
			for (int i = 0; i < cc_len[x]; ++i) {
				if (av == cpv) {
					// Set color
					color[pos + i][v] = color_noflag;
					SendMsh(pos, i, v, av, x);
					SendGraph(pos, i, v, x);
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
	FixLen(step000, step - 1);
	// Count additional variables
	CountOff(step000, step - 1);
	CountTime(step000, step - 1);
	UpdateNoteMinMax(step000, step - 1, !is_animating);
	UpdateTempoMinMax(step000, step - 1);
	mutex_animate.unlock();
	// Increment cantus_sent only if is not animating
	if (!is_animating) 
		++cantus_sent;
	// Create rule penalty string
	l_rpenalty_cur = rpenalty_cur;
	if (!skip_flags) {
		for (int x = 0; x < max_flags; ++x) {
			if (!accept[x] && fpenalty[x]) {
				st.Format("%d=%.0f", x, fpenalty[x]);
				if (!rpst.IsEmpty()) rpst += ", ";
				rpst += st;
			}
		}
		if (rpenalty_cur == MAX_PENALTY) {
			l_rpenalty_cur = 0;
			rpst.Empty();
		}
	}
	if (!svoice) fpenalty_source = rpst;
	if (task == tGen) {
		if (!shuffle) {
			Adapt(step000, step - 1);
		}
		// If  window-scan
		st.Format("#%d (from %s) N%d\nCantus: %s\nSpecies: %d\nRule penalty: %.0f", 
			cantus_id + 1, bname_from_path(midi_file), cantus_sent - 1, cantus_high?"high":"low", species, l_rpenalty_cur);
		st2.Format("Flags penalty: %s\n%s", rpst, pmap);
		AddMelody(step000, pos - 1, svoice + cpv, st, st2);
		AddMelody(step000, pos - 1, 0, st);
	}
	else if (task == tEval) {
		if (m_algo_id == 121) {
			// If RSWA
			st.Format("#%d (from %s) N%d\nCantus: %s\nSpecies: %d\nRule penalty: %.0f", 
				cantus_id + 1, bname_from_path(midi_file), cantus_sent - 1, cantus_high ? "high" : "low", species, l_rpenalty_cur);
			st2.Format("Flags penalty: %s\n%s", rpst, pmap);
		}
		else {
			if (key_eval.IsEmpty()) {
				// If SWA
				st.Format("#%d (from %s)\nCantus: %s\nSpecies: %d\nRule penalty: %.0f => %.0f\nDistance penalty: %d", 
					cantus_id+1, bname_from_path(midi_file), cantus_high ? "high" : "low", species, rpenalty_source, l_rpenalty_cur, dpenalty_cur);
				st2.Format("Flags penalty: %s => %s\n%s", fpenalty_source, rpst, pmap);
			}
			else {
				// If evaluating
				st.Format("#%d (from %s)\nCantus: %s\nSpecies: %d\nRule penalty: %.0f", 
					cantus_id + 1, bname_from_path(midi_file), cantus_high ? "high" : "low", species, l_rpenalty_cur);
				st2.Format("Flags penalty: %s\nKey selection: %s\n%s", rpst, key_eval, pmap);
			}
		}
		AddMelody(step000, pos - 1, svoice + cpv, st, st2);
		AddMelody(step000, pos - 1, 0, st);
	}
	if (debug_level > 2) {
		time_stop = CGLib::time();
		st.Format("SendCP run time %lld ms", time_stop - time_start);
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
		FLAG2(flag, fli[bli[i]]);
	return 0;
}

// Fail vertical altered intervals
int CGenCP1::FailAlteredInt() {
	CHECK_READY(DR_pc, DR_ivl);
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
	if ((apcc[cfv][i_1] == c1 && apcc[cpv][i] == c2) || (apcc[cfv][i_1] == c2 && apcc[cpv][i] == c1)) 
		FLAG2(flag, fli[bli[i]]);
	else if ((apcc[cpv][i_1] == c1 && apcc[cfv][i] == c2) || (apcc[cpv][i_1] == c2 && apcc[cfv][i] == c1)) 
		FLAG2(flag, fli[bli[i_1]]);
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
		civlc2[i] = civl[i] ? (civlc[i] ? civlc[i] : 12) : 0;
		//if (civlc[i] == 1 || civlc[i] == 2 || civlc[i] == 5 || civlc[i] == 6 || civlc[i] == 10 || civlc[i] == 11) tivl[i] = iDis;
		if (civlc[i] == 3 || civlc[i] == 4 || civlc[i] == 8 || civlc[i] == 9) tivl[i] = iIco;
		else if (civlc[i] == 7 || civlc[i] == 0) tivl[i] = iPco;
		else tivl[i] = iDis;
	}
}

void CGenCP1::Get4th() {
	CHECK_READY(DR_fli, DR_sus, DR_ivl);
	// Do not process if upbeat not lowest 4th is prohibited
	if (!accept[331]) return;
	// Do not process lower cantus, because it forms downbeat 4th
	if (!cantus_high) return;
	int mea_end, found;
	// Calculate intervals
	for (ls = 0; ls < fli_size; ++ls) {
		s = fli[ls];
		// Do not process suspension and anticipation 4th
		if (sus[ls]) continue;
		// Process only 4th
		if (ivlc[s] != 3) continue;
		// Do not process downbeat
		if (!beat[ls]) continue;
		// Detect consonant note in measure that is lower than current
		ms = bmli[s];
		if (ms < mli.size() - 1) mea_end = mli[ms + 1] - 1;
		else mea_end = c_len - 1;
		// Prevent going out of window
		if (mea_end >= ep2) {
			tivl[s] = iHarm4;
			break;
		}
		found = 0;
		for (int s2 = mli[ms]; s2 <= mea_end; ++s2) {
			if (tivl[s2] != iDis && acc[cpv][s2] < acc[cpv][s]) {
				found = 1;
				break;
			}
		}
		if (found) tivl[s] = iHarm4;
	}
}

int CGenCP1::FailVMotion() {
	SET_READY(DR_motion, DR_c);
	int mtemp, vm1, vm2;
	pm_contrary = 0;
	pm_direct = 0;
	for (int i = 0; i < ep2; ++i) {
		if (i < ep2 - 1) {
			motion[i] = mStay;
			vm1 = ac[cfv][i + 1] - ac[cfv][i];
			vm2 = ac[cpv][i + 1] - ac[cpv][i];
			if (vm1 || vm2) {
				mtemp = vm1 * vm2;
				if (mtemp > 0) {
					if (vm1 == vm2)	motion[i] = mParallel;
					else motion[i] = mDirect;
					++pm_direct;
				}
				else if (mtemp < 0) {
					motion[i] = mContrary;
					++pm_contrary;
				}
				else motion[i] = mOblique;
			}
		}
	}
	// Check how many contrary if full melody analyzed
	if (ep2 == c_len) {
		if (pm_contrary + pm_direct) { //-V793
			int pcontra = (pm_contrary * 100) / (pm_contrary + pm_direct);
			if (pcontra < contrary_min2) FLAG2(46, 0);
			else if (pcontra < contrary_min) FLAG2(35, 0);
		}
	}
	return 0;
}

int CGenCP1::FailSusResolution(int s3) {
	// Check if suspension second part is discord
	if (tivl[s2] < 0) {
		// Mark resolution as obligatory harmonic in basic msh
		if (tivl[s3] > 0) mshb[bli[s3]] = pSusRes;
		// Resolution to discord
		if (tivl[s3] < 0) FLAG2(220, sus[ls]);
			// Resolution by leap
		else if (abs(ac[cpv][s3] - ac[cpv][s2]) > 1) FLAG2(221, sus[ls]);
		else {
			// Resolution up
			if (acc[cpv][s3] > acc[cpv][s2]) {
				// Allowed only for resolution of leading tone
				if (apcc[cpv][s2] == 11) FLAG2(222, sus[ls]);
				else FLAG2L(219, s3, sus[ls]);
			}
			// 9th to 8va
			if (ivlc[s2] == 1 && ivlc[s3] == 0) {
				if (ivl[s2] > 7) {
					if (cantus_high) 
						FLAG2(216, sus[ls]);
				}
				// 2nd to unison
				else FLAG2(218, sus[ls]);
			}
			// 7th to 8va
			else if (cantus_high && ivlc[s2] == 6 && ivlc[s3] == 0) 
				FLAG2(217, sus[ls]);
		}
	}
	return 0;
}

int CGenCP1::FailSus1() {
	CHECK_READY_PERSIST(DP_mli);
	CHECK_READY(DR_fli, DR_ivl, DR_sus);
	CHECK_READY(DR_leap, DR_beat);
	int last_cf;
	for (ls = 0; ls < fli_size; ++ls) if (sus[ls]) {
		s = fli[ls];
		s2 = fli2[ls];
		// Flag suspension
		FLAG2(225, s);
		// Check if sus starts from discord
		if (tivl[s] < 0) FLAG2(224, s);
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
		// Do not check last note in scan window, because llen can change
		if (ep2 < c_len && ls == fli_size - 1) break;
		antici = 0;
		// If sus starts with dissonance, it is anticipation
		if (tivl[s] < 0) antici = 1;
		// If both consonances
		else if (tivl[sus[ls]] > 0) {
			// If sus starts with note shorter than 1/2, it is anticipation
			if (sus[ls] - s < npm / 2) antici = 1;
			// If sus second part is equal or longer than whole note
			else if (s2 - sus[ls] >= npm - 1) antici = 1;
		}
		// Check if sus starts from discord
		if (antici) {
			// Long start
			if (sus[ls] - fli[ls] > npm / 2) FLAG2(333, s);
			// Long finish
			if (fli2[ls] - sus[ls] + 1 > npm) FLAG2(334, s);
			// Mark anticipation start as non-harmonic always
			mshb[ls] = pPass;
			// Check if start and end of slur is a discord - then it is interbar discord
			if (tivl[sus[ls]] == iDis) {
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
				if (llen[ls] == (sus[ls] - s) * 2) FLAG2(293, s);
				else if (llen[ls] < (sus[ls] - s) * 2) FLAG2(288, s);
				// Anticipation should be shorter then previous note
				if (ls > 0 && sus[ls] - s > llen[ls - 1]) FLAG2(294, s);
			}
		}
		else {
			// Species 2
			if (species == 2) {
				if (bmli[sus[ls]] == mli.size() - 2 && ls < fli_size - 1 && 
					apcc[cpv][sus[ls]] == 0 && apcc[cpv][fli[ls + 1]] == 11) FLAG2(387, s);
					else FLAG2(388, s);
			}
			else {
				FLAG2(225, s);
			}
			// Suspension not in last measures
			if (species < 4) {
				if (bmli[s] < mli.size() - sus_last_measures) FLAG2(139, s);
			}
			// Mark sus start as harmonic always
			mshb[ls] = pSusStart;
			// 1/4 + ?
			if (sus[ls] - fli[ls] == npm / 4) FLAG2(251, s);
			// Long start
			if (sus[ls] - fli[ls] > npm / 2) FLAG2(274, s);
			// Long finish
			if (fli2[ls] - sus[ls] + 1 > 3 * npm / 4) FLAG2(332, s);
			// If sus is not last note
			if (ls < fli_size - 1) {
				// If mid-bar already generated (sus cannot be in first measure, thus npm usage is correct)
				s3 = sus[ls] + npm / 2;
				if (s3 < ep2) {
					// If second part is 3/4 in species 5
					if (npm == 8 && s2 - sus[ls] == 5) {
						// If next note is 1/8
						if (llen[ls + 1] == 1 && ls < fli_size - 2) FLAG2L(291, fli[ls + 1], sus[ls]);
						if (FailSusResolution(fli[ls + 1])) return 1;
						// Stop processing this sus
						continue;
					}
					// If this step does not start new note
					if (acc[cpv][s3] == acc[cpv][s3 - 1]) FLAG2L(286, s2 + 1, sus[ls]);
					// Suspension of non-leading tone in species 2
					if (species == 2) {
						if (apcc[cpv][s3] != 11) FLAG2(299, s);
					}
					// If resolution note is too short
					ls3 = bli[s3];
					if (llen[ls3] < npm / 4 && ls3 < fli_size - 1) FLAG2L(291, s3, sus[ls]);
					if (FailSusResolution(s3)) return 1;
					// If there is one intermediate step
					if (ls3 - ls == 2) {
						// If leap is too long
						if (abs(acc[cpv][fli[ls + 1]] - acc[cpv][s2]) > sus_insert_max_leap) 
							FLAG2L(295, fli[ls + 1], sus[ls]);
						// If second movement is leap
						if (aleap[cpv][fli2[ls + 1]] > 0) FLAG2L(136, fli[ls + 1], sus[ls]);
						else if (aleap[cpv][fli2[ls + 1]] < 0) FLAG2L(296, fli[ls + 1], sus[ls]);
						else {
							// Mark insertion as non-harmonic in basic msh if resolution is harmonic and sus ends with dissonance
							if (tivl[s2] < 0 && tivl[s3] > 0) mshb[ls + 1] = pAux;
							if (asmooth[cpv][fli2[ls + 1]] > 0) FLAG2L(137, fli[ls + 1], sus[ls]);
							else if (asmooth[cpv][fli2[ls + 1]] < 0) FLAG2L(138, fli[ls + 1], sus[ls]);
						}
					}
					// If there are two intermediate steps - do nothing
					// If there are more than two intermediate steps
					else if (ls3 - ls > 3) {
						FLAG2L(292, s3, sus[ls]);
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

int CGenCP1::FailSusCount() {
	CHECK_READY(DR_sus, DR_retrigger);
	pm_sus = 0;
	pm_anti = 0;
	for (ls = 0; ls < fli_size; ++ls) {
		if (sus[ls]) {
			if (retrigger[sus[ls]]) ++pm_anti;
			else ++pm_sus;
		}
	}
	int mcount = bmli[ep2 - 1];
	// Do not check for first measure
	if (!mcount) return 0;
	// Check for not enough sus
	if ((pm_sus + pm_anti + 1) * 1.0 / mcount < 1.0 / mea_per_sus) 
		FLAG2(341, 0);
	return 0;
}

int CGenCP1::FailSus() {
	CHECK_READY(DR_mshb);
	SET_READY(DR_retrigger);
	fill(retrigger.begin(), retrigger.end(), 0);
	if (species == 1) {
		if (FailSus1()) return 1;
	}
	else {
		if (FailSus2()) return 1;
		if (species == 5) {
			if (FailSusCount()) return 1;
		}
	}
	return 0;
}

int CGenCP1::FailDisSus() {
	// Discord
	if (tivl[s] < 0) {
		FLAG2(83, s);
	}
	return 0;
}

int CGenCP1::FailUnison() {
	// Unison
	if (!civl[s]) {
		// 2nd -> unison
		if (s > 0 && ivl[s - 1] == 1) 
			FLAG2L(275, s, isus[ls - 1]);
		// Unison near m2 in same measure
		if (ls < fli_size - 1 && civl[fli[ls + 1]] == 1 && bmli[s] == bmli[fli[ls + 1]]) 
			FLAG2L(277, s, fli[ls + 1]);
		else if (s > 0 && civl[s - 1] == 1 && bmli[s] == bmli[s - 1]) 
			FLAG2L(275, s, isus[ls - 1]);
		// Inside downbeat without suspension
		if (!beat[ls] && ls > 0 && ls < fli_size - 1 && !sus[ls]) 
			FLAG2(91, s);
	}
	return 0;
}

int CGenCP1::FailDis() {
	// Discord
	if (tivl[s] == iDis) {
		// Do not flag discord if suspension, because anticipation will flag it
		// Do not flag discord if last note, because it can become suspension
		if (sus[ls] || ls == fli_size - 1) return 0;
		if (msh[ls] == pLeap) FLAG2(187, s);
			// pLastLT cannot be dissonance, because it is set only if it is not dissonance
			// pSusStart does not have separate flag, because it is marked as interbar
			// Here pSusStart will never fire flag, because pSusStart is set only if sus start is not a dissonance
		else if (msh[ls] == pSusStart) FLAG2(224, s);
			// pSusRes does not have separate flag, because it is marked as not resolved
			// Here pSusRes will never fire flag, because pSusRes is set only if sus resolution is harmonic
		else if (msh[ls] == pSusRes) FLAG2(220, s);
			// This is protection against wrong melodic shape value
		else if (msh[ls] > 0) FLAG2(83, s);
		else {
			// Stepwize
			if (civl[s] == 1) FLAG2(276, s);
			else {
				if (msh[ls] < -30) FLAG2(256, s);
				else if (msh[ls] < -20) FLAG2(258, s);
				else if (msh[ls] < -10) FLAG2(282, s);
				else FLAG2(169, s);
			}
			if (!sus[ls]) {
				// Check if discord is longer than neighboring consonance
				if (ls > 0 && llen[ls] > llen[ls - 1] && tivl[fli2[ls - 1]] != iDis) 
					FLAG2(223, s);
				if (ls < fli_size - 2 && beat[ls + 1] && llen[ls] > llen[ls + 1] && tivl[fli[ls + 1]] != iDis)
					FLAG2(384, s);
			}
		}
	}
	return 0;
}

int CGenCP1::FailPcoSus() {
	// Perfect consonance
	if (tivl[s] == iPco) {
		// Do not need to prohibit parallel, because one of note is slurred from previous step
		// Prohibit combinatory
		if (civlc[s] == civlc[s - 1]) FLAG2L(85, s - 1, s);
		// Prohibit different
		else if (tivl[s - 1] == iPco)
			FLAG2L(86, s - 1, s);
	}
	return 0;
}

int CGenCP1::FailPco() {
	// Suspension to pco
	if (sus[ls] && tivl[sus[ls]] == iPco && !retrigger[sus[ls]]) {
		// Do not check last note in scan window, because retrigger has not been set
		if (ep2 >= c_len || ls < fli_size - 1) {
			// Prohibit leading tone octave on suspension
			if (apcc[0][sus[ls]] == 11 && apcc[1][sus[ls]] == 11)
				FLAG2(324, sus[ls]);
			// Prohibit parallel pco on suspension
			if (ivl[sus[ls]] == ivl[fli2[ls - 1]])
				FLAG2L(385, sus[ls], isus[ls - 1]);
		}
	}
	if (tivl[s] == iPco) {
		// Prohibit long downbeat octave except last measure
		if (!cantus_high && species == 5 && !beat[ls] && bmli[s] < mli.size() - 1 && 
			acc[cpv][s] >= lclimax2[s] && rlen[ls] > 3 && ls > 0 && rlen[ls] >= rlen[ls - 1]) {
			if (ivlc[s]) FLAG2(325, s);
			else FLAG2(326, s);
		}
		// Prohibit leading tone octave
		if (apcc[0][s] == 11 && apcc[1][s] == 11) {
			// Downbeat
			if (!beat[ls]) FLAG2(324, s);
			// Leaps
			else if (s > 0 && aleap[cpv][s - 1]) FLAG2(324, s);
			else if (ls < fli_size - 1 && aleap[cpv][fli2[ls]]) 
				FLAG2(324, s);
			// Suspension resolution
			else if (mshb[ls] > 0) FLAG2(324, s);
		}
		// Do not prohibit parallel first - first (this is for sus notes, which starts are parallel)
		// because they are detected as pco apart now
			// Prohibit parallel last - first
		if (ivl[s] == ivl[fli2[ls - 1]]) 
			FLAG2L(84, isus[ls - 1], s);
		else {
			// Prohibit contrary movement
			if (bmli[s] - 1 == bmli[fli2[ls - 1]] && civlc[s] == civlc[fli2[ls - 1]]) 
				FLAG2L(85, isus[ls - 1], s);
				// Prohibit different
			else if (tivl[fli2[ls - 1]] == iPco) 
				FLAG2L(86, isus[ls - 1], s);
			// All other cases if previous interval is not pco
			// Direct movement to pco
			if (motion[fli2[ls - 1]] == mDirect) {
				// Stepwize
				if (abs(acc[1][s] - acc[1][s - 1]) < 3) {
					if (s2 == c_len - 1 && cfli[cfli.size()-1] == s) {
						if (civlc[s] == 0) FLAG2L(209, isus[ls - 1], s);
						else FLAG2L(208, isus[ls - 1], s);
					}
					else if (ls < fli_size - 1 || ep2 == c_len) {
						if (civlc[s] == 0) FLAG2L(211, isus[ls - 1], s);
						else FLAG2L(210, isus[ls - 1], s);
					}
				}
				// Non-stepwize
				else {
					if (s2 == c_len - 1 && cfli[cfli.size() - 1] == s) {
						if (civlc[s] == 0) FLAG2L(213, isus[ls - 1], s);
						else FLAG2L(212, isus[ls - 1], s);
					}
					else if (ls < fli_size - 1 || ep2 == c_len) {
						if (civlc[s] == 0) FLAG2L(215, isus[ls - 1], s);
						else FLAG2L(214, isus[ls - 1], s);
					}
				}
			}
		}
		// Prohibit downbeats and culminations only if not last step
		if (ls < fli_size - 1) {
			if (beat[ls]) {
				// Prohibit culmination
				if (culm_ls == ls || (cfv == 1 && cf_culm_s == s)) 
					FLAG2(81, s);
			}
			else {
				// Prohibit downbeat culmination
				if (culm_ls == ls || (cfv == 1 && cf_culm_s == s))
					FLAG2(82, s);
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
int CGenCP1::DetectPDD() {
	if (!accept[282]) return 0;
	int pattern_needed;
	// Do not detect PDD in lower voice
	if (!cpv) return 0;
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
		if (ls < fli_size - 2) {
			// Stepwize downward movement
			if (ac[cpv][fli[ls + 2]] - ac[cpv][fli[ls + 1]] != -1) continue;
			// Note 2 is not longer than 3
			if (llen[ls + 1] > llen[ls + 2] && (ep2 == c_len || ls < fli_size - 3)) continue;
			// Third note must be consonance
			if (tivl[fli[ls + 2]] < 0) continue;
		}
		pattern_needed = 0;
		if (tivl[fli[ls + 1]] < 0) pattern_needed = 1;
		// Parallel motion - flag but allow (this will make dissonance legal, but will add flag with problem)
		if (ac[cfv][fli[ls + 1]] - ac[cfv][s] == -1) {
			if (pattern_needed) FLAG2(298, fli[ls + 1]);
			//if (!accept[298]) continue;
		}
		// Direct motion - flag but allow (this will make dissonance legal, but will add flag with problem)
		else if (ac[cfv][fli[ls + 1]] - ac[cfv][s] < 0) {
			if (pattern_needed) FLAG2(297, fli[ls + 1]);
			//if (!accept[297]) continue;
		}
		SavePattern(pPDD);
	}
	return 0;
}

// Detect downbeat neighbour tone
int CGenCP1::DetectDNT() {
	if (!accept[258]) return 0;
	int pattern_needed;
	int max_ls = fli_size - 1;
	if (ep2 == c_len) max_ls = fli_size - 3;
	for (ls = 0; ls < max_ls; ++ls) {
		if (ls < fli_size - 3 && SkipSus(3)) continue;
		s = fli[ls];
		s2 = fli2[ls];
		// Note 2 is long
		if (llen[ls + 1] > npm / 2) continue;
		// Note 1 is not dissonance
		if (tivl[s] < 0) continue;
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
			pattern_needed = 0;
			if (tivl[fli[ls + 1]] < 0) pattern_needed = 1;
			else if (tivl[fli[ls + 2]] < 0) pattern_needed = 1;
			// Maximum leap
			int lp = abs(acc[cpv][fli[ls + 2]] - acc[cpv][fli[ls + 1]]);
			if (lp < 3) continue;
			if (lp > 4) {
				if (lp > dnt_max_leap) continue;
				if (pattern_needed) FLAG2(260, fli[ls]);
				if (!accept[260]) continue;
			}
			if (ls < fli_size - 3) {
				// Note 3 is longer than 4
				if (llen[ls + 2] > llen[ls + 3] && (ep2 == c_len || ls < fli_size - 4)) continue;
				// Note 4 is not dissonance
				if (tivl[fli[ls + 3]] < 0) continue;
				// Movements are stepwize
				if (!asmooth[cpv][fli2[ls + 2]]) continue;
				// Both movements have same direction
				if (asmooth[cpv][s2] != asmooth[cpv][fli2[ls + 2]]) continue;
				// Mixed rhythm
				if (llen[ls] != npm / 4 || llen[ls] != llen[ls + 1] ||
					llen[ls] != llen[ls + 2] ||
					(llen[ls] != llen[ls + 3] && (ep2 == c_len || ls < fli_size - 4))) {
					if (pattern_needed) FLAG2(280, fli[ls]);
					if (!accept[280]) continue;
				}
				// Cross-bar
				if (bmli[s] != bmli[fli2[ls + 3]]) {
					if (bmli[s] == bmli[fli2[ls + 2]]) {
						if (pattern_needed) FLAG2(328, fli[ls]);
						if (!accept[328]) continue;
					}
					else {
						if (pattern_needed) FLAG2(281, fli[ls]);
						if (!accept[281]) continue;
					}
				}
				if (ls < fli_size - 4) {
					// Leap from note 4
					if (aleap[cpv][fli2[ls + 3]] || 
						asmooth[cpv][fli2[ls + 3]] * asmooth[cpv][fli2[ls]] < 1) {
						if (pattern_needed) FLAG2(97, fli[ls]);
						//if (!accept[97]) continue;
					}
				}
			}
		}
		SavePattern(pDNT);
	}
	return 0;
}

int CGenCP1::DetectCambiata() {
	if (!accept[256]) return 0;
	int pattern_needed;
	int max_ls = fli_size - 1;
	if (ep2 == c_len) max_ls = fli_size - 3;
	for (ls = 0; ls < max_ls; ++ls) {
		if (ls < fli_size - 3 && SkipSus(3)) continue;
		s = fli[ls];
		s2 = fli2[ls];
		// Second note is upbeat (beat 1 not allowed)
		if (beat[ls+1] < 1) continue;
		// Note 1 is not a discord
		if (tivl[s] < 0) continue;
		// Second note is created by stepwize movement from first
		if (aleap[cpv][s2]) continue;
		// Third note is created by leaping motion in same direction as second note moves
		if (!aleap[cpv][fli2[ls + 1]]) continue;
		if (asmooth[cpv][s2] * aleap[cpv][fli2[ls + 1]] < 0) continue;
		pattern_needed = 0;
		if (tivl[fli[ls + 1]] < 0) pattern_needed = 1;
		else if (tivl[fli[ls + 2]] < 0) pattern_needed = 1;
		// Inverted
		if (asmooth[cpv][s2] == 1) {
			if (pattern_needed) FLAG2(279, fli[ls]);
			//if (!accept[279]) continue;
		}
		if (ls < fli_size - 2) {
			// Leap from second note is longer than 4th
			if (abs(ac[cpv][fli2[ls + 2]] - ac[cpv][fli2[ls + 1]]) > 3) continue;
			// Non-harmonic note is not longer than harmonic
			if (llen[ls + 1] > llen[ls]) continue;
			// Leap 4th
			if (abs(ac[cpv][fli2[ls + 2]] - ac[cpv][fli2[ls + 1]]) == 3) {
				if (!accept[263] && !accept[323]) continue;
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
					if (pattern_needed) FLAG2(257, fli[ls]);
					if (!accept[257]) continue;
				}
				// Cross-bar
				if (bmli[s] != bmli[fli2[ls + 3]]) {
					if (bmli[s] == bmli[fli2[ls + 2]]) {
						if (pattern_needed) FLAG2(327, fli[ls]);
						if (!accept[327]) continue;
					}
					else {
						if (pattern_needed) FLAG2(259, fli[ls]);
						if (!accept[259]) continue;
					}
				}
				// Third diss or harmonic 4th
				if (tivl[fli[ls + 2]] < 0) {
					if (pattern_needed) FLAG2(261, fli[ls]);
					//if (!accept[261]) continue;
					// If third note is dissonance, it should not be downbeat
					if (!beat[ls + 2]) continue;
					// If third note is dissonance, it should resolve back stepwize
					if (aleap[cpv][fli2[ls + 1]] * asmooth[cpv][fli2[ls + 2]] >= 0) continue;
					// Non-harmonic note is not longer than harmonic
					if (llen[ls + 2] > llen[ls]) continue;
					if (llen[ls + 2] > llen[ls + 3] && (ep2 == c_len || ls < fli_size - 4)) continue;
					// Fourth note should not be dissonance
					if (tivl[fli[ls + 3]] < 0) continue;
				}
				// Leap from note 3
				if (aleap[cpv][fli2[ls + 2]]) {
					if (pattern_needed) FLAG2(264, fli[ls]);
					if (!accept[264]) continue;
					// Leap too long
					if (abs(acc[cpv][fli2[ls + 3]] - acc[cpv][fli2[ls + 2]]) > cambiata_max_leap3) continue;
					// Leap 4th
					if (abs(ac[cpv][fli2[ls + 2]] - ac[cpv][fli2[ls + 1]]) == 3) {
						if (pattern_needed) FLAG2(263, fli[ls]);
						if (!accept[263]) continue;
					}
				}
				if (ls < fli_size - 4) {
					// Leap from note 4
					if (aleap[cpv][fli2[ls + 3]]) {
						if (pattern_needed) FLAG2(265, fli[ls]);
						//if (!accept[265]) continue;
						// Leap too long
						if (abs(acc[cpv][fli2[ls + 4]] - acc[cpv][fli2[ls + 3]]) > cambiata_max_leap4) continue;
						// Leap 4th
						if (abs(ac[cpv][fli2[ls + 2]] - ac[cpv][fli2[ls + 1]]) == 3) {
							if (pattern_needed) FLAG2(263, fli[ls]);
							if (!accept[263]) continue;
						}
					}
					// Fourth note moves back
					if (ls < fli_size - 4 && (acc[cpv][fli[ls + 4]] - acc[cpv][fli[ls + 3]]) *
						(acc[cpv][fli[ls + 3]] - acc[cpv][fli[ls + 2]]) < 0) {
						if (pattern_needed) FLAG2(266, fli[ls]);
						//if (!accept[266]) continue;
						// Leap 4th
						if (abs(ac[cpv][fli2[ls + 2]] - ac[cpv][fli2[ls + 1]]) == 3) {
							if (pattern_needed) FLAG2(263, fli[ls]);
							if (!accept[263]) continue;
						}
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
	return 0;
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

int CGenCP1::DetectPatterns() {
	CHECK_READY_PERSIST(DP_mli);
	CHECK_READY(DR_beat, DR_ivl);
	CHECK_READY(DR_fli, DR_leap, DR_c);
	SET_READY(DR_pat);
	// Detect no patterns for species 1
	if (species == 1) return 0;
	fill(pat.begin(), pat.end(), 0);
	fill(pat_state.begin(), pat_state.end(), 0);
	if (DetectPDD()) return 1;
	// Detect next patterns only for species 3 and 5
	if (species != 3 && species != 5) return 0;
	if (DetectCambiata()) return 1;
	if (DetectDNT()) return 1;
	return 0;
}

void CGenCP1::GetBasicMsh() {
	CHECK_READY(DR_c, DR_fli, DR_leap);
	SET_READY(DR_mshb);
	// First note is always downbeat
	mshb[0] = pDownbeat;
	// Main calculation
	for (ls = 1; ls < fli_size; ++ls) {
		s = fli[ls];
		s2 = fli2[ls];
		if ((s + fn) % npm == 0) mshb[ls] = pDownbeat;
		else if (s > 0 && aleap[cpv][s - 1]) mshb[ls] = pLeap;
		else if (s2 < ep2 - 1 && aleap[cpv][s2]) mshb[ls] = pLeap;
		else {
			if (s > 0 && s2 < ep2 - 1 && ac[cpv][s - 1] == ac[cpv][s2 + 1]) mshb[ls] = pAux;
			else mshb[ls] = pPass;
		}
	}
	// Detect long notes
	for (ls = 1; ls < fli_size - 1; ++ls) {
		s = fli[ls];
		if (mshb[ls] < 0 && tivl[s] != iDis) {
			// Check longer than previous
			if (llen[ls] > llen[ls - 1] &&
				mshb[ls - 1] > 0 && tivl[fli2[ls - 1]] != iDis) 
				mshb[ls] = pLong;
			// Check longer than next
			//if (llen[ls] > llen[ls + 1] &&
				//mshb[ls + 1] > 0 && tivl[fli2[ls + 1]] != iDis) mshb[ls] = pLong;
		}
	}
}

int CGenCP1::FailAdjacentTritone2(int ta, int t1, int t2, int tb) {
	int found = 0;
	int res1 = 0;
	int res2 = 0;
	// Check consecutive tritone
	if ((apcc[cpv][s2] != t2 || apcc[cpv][s] != t1) &&
		(apcc[cpv][s2] != t1 || apcc[cpv][s] != t2)) return 0;
	// Check different measures
	//if (bmli[s] != bmli[s2]) return 0;
	fleap_start = ls;
	fleap_end = ls + 1;
	// Do not check tritone if it is at the end of not-last window
	if (ls >= fli_size - 2 && ep2 != c_len) return 0;
	// Check framed 
	if ((ls >= fli_size - 2 || aleap[cpv][s] * (acc[cpv][fli2[ls + 2]] - acc[cpv][fli2[ls + 1]]) < 0 || 
		aleap[cpv][fli2[ls + 1]]) &&
		(ls == 0 || aleap[cpv][s] * (acc[cpv][s] - acc[cpv][fli2[ls - 1]]) < 0 ||
			aleap[cpv][fli2[ls - 1]])) found = 1;
	if (!found) {
		if (species == 5) {
			// Is last note at least 1/2 and not shorter than previous?
			if (rlen[ls + 1] >= 4 && llen[ls + 1] >= llen[ls]) found = 2;
		}
		// Is last note longer than previous ?
		if (llen[ls + 1] > llen[ls]) found = 2;
	}
	// Search measure for repeat
	if (!found) {
		ms = bmli[s2];
		int mea_end, ls1, ls2;
		int note_count = 0;
		if (ms < mli.size() - 1) mea_end = mli[ms + 1] - 1;
		else mea_end = c_len - 1;
		// Prevent going out of window
		if (mea_end < ep2) {
			ls1 = bli[mli[ms]];
			ls2 = bli[mea_end];
			// Loop inside measure
			for (int ls3 = ls1; ls3 <= ls2; ++ls3) {
				if (acc[cpv][fli[ls3]] == acc[cpv][s2]) ++note_count;
			}
			if (note_count > 1) found = 3;
		}
	}
	//if (!found) return 0;
	// Check if tritone is highest leap if this is last window
	if (ep2 == c_len && !cantus_high) {
		if ((acc[cpv][s] >= lclimax[s]) || (acc[cpv][s2] >= lclimax[s2])) {
			if (found == 0) FLAG2L(370, fli[fleap_start], fli[fleap_end]);
			else if (found == 1) FLAG2L(367, fli[fleap_start], fli[fleap_end]);
			else FLAG2L(362, fli[fleap_start], fli[fleap_end]);
		}
	}
	GetTritoneResolution(ta, t1, t2, tb, res1, res2, ac[cpv], acc[cpv], apc[cpv], apcc[cpv]);
	// Flag resolution for normal tritone
	if (found == 0) {
		if (res1*res2 == 0) FLAG2L(369, fli[fleap_start], fli[fleap_end]);
		else FLAG2L(368, fli[fleap_start], fli[fleap_end]);
	}
	// Flag resolution for framed tritone
	else if (found == 1) {
		if (res1*res2 == 0) FLAG2L(366, fli[fleap_start], fli[fleap_end]);
		else FLAG2L(365, fli[fleap_start], fli[fleap_end]);
	}
	// Flag resolution for accented tritone
	else {
		if (res1*res2 == 0) FLAG2L(361, fli[fleap_start], fli[fleap_end]);
		else FLAG2L(360, fli[fleap_start], fli[fleap_end]);
	}
	return 0;
}

// This function is for species 2-5
int CGenCP1::FailAdjacentTritones() {
	// Find adjacent notes
	CHECK_READY(DR_pc, DR_c, DR_fli);
	CHECK_READY(DR_leap);
	for (ls = 0; ls < fli_size - 1; ++ls) {
		s = fli2[ls];
		s2 = fli[ls + 1];
		if (minor_cur) {
			if (FailAdjacentTritone2(3, 5, 11, 0)) return 1;
			if (FailAdjacentTritone2(7, 8, 2, 3)) return 1;
		}
		else {
			if (FailAdjacentTritone2(4, 5, 11, 0)) return 1;
		}
	}
	return 0;
}

// This function is for species 2-5
int CGenCP1::FailTritones2() {
	CHECK_READY(DR_pc, DR_c, DR_fli);
	CHECK_READY(DR_leap);
	// Find both tritone notes in measure (non-adjacent)
	int mea_end, ls1, ls2, lpcc, cch, ccl, exceed, found, res1, res2, last_repeat;
	for (ms = 0; ms < mli.size(); ++ms) {
		// Stop processing when last measure is not fully generated
		if (ms == mli.size() - 1 && ep2 < c_len) break;
		// Get first and last measure notes
		if (ms < mli.size() - 1) mea_end = mli[ms + 1] - 1;
		else mea_end = c_len - 1;
		// Prevent going out of window
		if (mea_end >= ep2) break;
		ls1 = bli[mli[ms]];
		ls2 = bli[mea_end];
		vector<vector<int>> tfound, tfound2;
		tfound.resize(2);
		tfound2.resize(2);
		// Loop inside measure
		for (ls = ls1; ls <= ls2; ++ls) {
			lpcc = apcc[cpv][fli[ls]];
			// Find first and last notes of major tritone
			if (lpcc == 5) tfound[0].push_back(ls);
			if (lpcc == 11) tfound2[0].push_back(ls);
			if (minor_cur) {
				// Find first and last notes of minor tritone
				if (lpcc == 8) tfound[1].push_back(ls);
				if (lpcc == 2) tfound2[1].push_back(ls);
			}
		}
		// Loop through tritone types 
		for (int tt = 0; tt < 2; ++tt) {
			// Check each note combination
			for (int tn = 0; tn < tfound[tt].size(); ++tn) {
				for (int tn2 = 0; tn2 < tfound2[tt].size(); ++tn2) {
					found = 0;
					// Do not check adjacent
					if (abs(tfound[tt][tn] - tfound2[tt][tn2]) == 1) continue;
					// Do intermediate notes exceed pitch range?
					if (tfound[tt][tn] > tfound2[tt][tn2]) {
						last_repeat = tfound[tt].size();
						fleap_start = tfound2[tt][tn2];
						fleap_end = tfound[tt][tn];
					}
					else {
						last_repeat = tfound2[tt].size();
						fleap_start = tfound[tt][tn];
						fleap_end = tfound2[tt][tn2];
					}
					// Do not check if fleap_end is last note in scan window
					if (fleap_end >= fli_size - 1 && ep2 != c_len) return 0;
					// Low / high notes
					ccl = min(acc[cpv][fli[tfound[tt][tn]]], acc[cpv][fli[tfound2[tt][tn2]]]);
					cch = max(acc[cpv][fli[tfound[tt][tn]]], acc[cpv][fli[tfound2[tt][tn2]]]);
					exceed = 0;
					for (ls = fleap_start + 1; ls < fleap_end; ++ls) {
						if (acc[cpv][fli[ls]] > cch || acc[cpv][fli[ls]] < ccl) {
							exceed = 1;
							break;
						}
					}
					if (exceed) continue;
					// Check framed 
					if ((fleap_end == fli_size - 1 || (acc[cpv][fli[fleap_end]] - acc[cpv][fli2[fleap_start]]) *
						(acc[cpv][fli2[fleap_end + 1]] - acc[cpv][fli[fleap_end]]) < 0 || 
						aleap[cpv][fli2[fleap_end]]) &&
						(fleap_start == 0 || (acc[cpv][fli[fleap_end]] - acc[cpv][fli2[fleap_start]]) *
						(acc[cpv][fli2[fleap_start]] - acc[cpv][fli[fleap_start - 1]]) < 0 || 
							aleap[cpv][fli2[fleap_start - 1]])) found = 1;
					if (!found) {
						if (species == 5) {
							// Is last note at least 1/2 and not shorter than previous?
							if (rlen[fleap_end] >= 4 && llen[fleap_end] >= llen[fleap_start]) found = 2;
						}
						// Is last note longer than previous ?
						if (llen[fleap_end] > llen[fleap_start]) found = 2;
					}
					// Check last note repeats
					if (!found) {
						if (last_repeat > 1) found = 3;
					}
					if (minor_cur) {
						if (tt == 0)
							GetTritoneResolution(3, 5, 11, 0, res1, res2, ac[cpv], acc[cpv], apc[cpv], apcc[cpv]);
						if (tt == 1)
							GetTritoneResolution(7, 8, 2, 3, res1, res2, ac[cpv], acc[cpv], apc[cpv], apcc[cpv]);
					}
					else {
						if (tt == 0)
							GetTritoneResolution(4, 5, 11, 0, res1, res2, ac[cpv], acc[cpv], apc[cpv], apcc[cpv]);
					}
					if (!found) continue;
					// Check if tritone is highest leap if this is last window
					if (ep2 == c_len && !cantus_high) {
						if ((acc[cpv][fli[fleap_start]] >= lclimax[fli[fleap_start]]) || 
							(acc[cpv][fli[fleap_end]] >= lclimax[fli[fleap_end]])) {
							if (found == 1) FLAG2L(363, fli[fleap_start], fli[fleap_end]);
							else FLAG2L(364, fli[fleap_start], fli[fleap_end]);
						}
					}
					// Flag resolution for framed tritone
					if (found == 1) {
						if (res1*res2 == 0) FLAG2L(19, fli[fleap_start], fli[fleap_end]);
						else FLAG2L(18, fli[fleap_start], fli[fleap_end]);
					}
					// Flag resolution for accented tritone
					else {
						if (res1*res2 == 0) FLAG2L(343, fli[fleap_start], fli[fleap_end]);
						else FLAG2L(342, fli[fleap_start], fli[fleap_end]);
					}
				}
			}
		}
	}
	return 0;
}

void CGenCP1::SetMsh(int ls, vector<int> &l_msh, int val) {
	// Check if in range
	if (ls >= fli_size) return;
	// Detect changing sign for dissonance
	if (l_msh[ls] * val < 0 && tivl[fli[ls]] < 0 ) {
		CString est;
		est.Format("Detected msh overwrite at note %d:%d (%s) with value %d (old value %d): %s", 
			cantus_id + 1, ls + 1, tivl[fli[ls]] < 0?"Dis":"non-Dis", 
			val, l_msh[ls], vint2st(ep2, acc[cpv]));
		WriteLog(5, est);
	}
	l_msh[ls] = val;
}

void CGenCP1::ApplyPDD(int ls, vector<int> &l_msh, int state) {
	SetMsh(ls + 1, l_msh, -11);
	SetMsh(ls + 2, l_msh, 12);
	pat_state[ls] = state;
}

void CGenCP1::ApplyDNT(int ls, vector<int> &l_msh, int state) {
	// Do not apply requirement if not fully generated
	if (ls < fli_size - 3 || ep2 == c_len) 
		SetMsh(ls,     l_msh, 21);
	SetMsh(ls + 1, l_msh, -22);
	SetMsh(ls + 2, l_msh, -23);
	SetMsh(ls + 3, l_msh, 24);
	pat_state[ls] = state;
}

void CGenCP1::ApplyCam(int ls, vector<int> &l_msh, int state) {
	// Do not apply requirement if not fully generated
	if (ls < fli_size - 4 || ep2 == c_len)
		SetMsh(ls, l_msh, 31);
	SetMsh(ls + 1, l_msh, -32);
	pat_state[ls] = state;
}

// Cambiata with third note non-harmonic allowed
void CGenCP1::ApplyCam2(int ls, vector<int> &l_msh, int state) {
	// Do not apply requirement if not fully generated
	if (ls < fli_size - 4 || ep2 == c_len)
		SetMsh(ls, l_msh, 41);
	SetMsh(ls + 1, l_msh, -42);
	SetMsh(ls + 2, l_msh, -43);
	SetMsh(ls + 3, l_msh, 44);
	pat_state[ls] = state;
	// Do not need to set note 1, because it can be dissonant
	// Do not need to set note 4, because it can be dissonant if note 3 is consonant
	// Do not need to set note 4, because if it is stepwize after dissonant, it can be only consonant. If it is not stepwize, it is not resolution
}

void CGenCP1::ApplyFixedPat() {
	CHECK_READY(DR_mshb, DR_ivl);
	CHECK_READY(DR_fli);
	SET_READY(DR_msh, DR_mshf);
	if (species == 1) {
		for (int ls = 0; ls < fli_size; ++ls) msh[ls] = mshb[ls];
		return;
	}
	// Clear mshf
	fill(mshf.begin(), mshf.end(), 0);
	// Walk through patterns
	for (int ls = 0; ls < fli_size; ++ls) {
		if (pat[ls] == 0) continue;
		if (pat[ls] == pPDD) {
			if (tivl[fli[ls + 1]] == iDis) ApplyPDD(ls, mshf, 1);
		}
		else if (pat[ls] == pDNT) {
			if (tivl[fli[ls + 1]] == iDis || tivl[fli[ls + 2]] == iDis) {
				ApplyDNT(ls, mshf, 1);
			}
		}
		else if (pat[ls] == pCam) {
			if (tivl[fli[ls + 2]] == iDis)
				ApplyCam2(ls, mshf, 1);
			else if (tivl[fli[ls + 1]] == iDis)
				ApplyCam(ls, mshf, 1);
		}
	}
	// Set mshf for empty
	for (int ls = 0; ls < fli_size; ++ls) if (!mshf[ls]) mshf[ls] = mshb[ls];
	// Set msh
	for (int ls = 0; ls < fli_size; ++ls) msh[ls] = mshf[ls];
}

int CGenCP1::FailRhythm() {
	CHECK_READY(DR_fli, DR_beat, DR_sus);
	CHECK_READY_PERSIST(DP_mli);
	CHECK_READY(DR_leap);
	if (species == 2) {
		if (FailRhythm2()) return 1;
	}
	else if (species == 3) {
		if (FailRhythm3()) return 1;
	}
	else if (species == 4) {
		if (FailRhythm4()) return 1;
	}
	else if (species == 5) {
		if (FailRhythm5()) return 1;
	}
	return 0;
}

// Fail rhythm for species 2
int CGenCP1::FailRhythm2() {
	// Last measure not whole
	if (c_len - fli[fli_size - 1] < npm) {
		FLAG2(267, fli[fli_size - 1]);
	}
	for (ls = 0; ls < fli_size - 1; ++ls) {
		s = fli[ls];
		// Whole inside
		if (!beat[ls] && llen[ls] == npm) FLAG2(236, s);
	}
	return 0;
}

// Fail rhythm for species 4
int CGenCP1::FailRhythm4() {
	// Last measure not whole
	if (c_len - fli[fli_size - 1] < npm) {
		FLAG2(267, fli[fli_size - 1]);
	}
	for (ls = 0; ls < fli_size - 1; ++ls) {
		s = fli[ls];
		// Whole inside
		if (!beat[ls] && llen[ls] == npm) FLAG2(236, s);
	}
	return 0;
}

// Fail rhythm for species 3
int CGenCP1::FailRhythm3() {
	// Check uneven pause
	if (fn && fn != llen[0] && fli_size > 1) FLAG2(237, 0);
	// Last measure not whole
	if (c_len - fli[fli_size - 1] < npm) {
		FLAG2(267, fli[fli_size - 1]);
		if (c_len - fli[fli_size - 1] == 1) FLAG2(252, fli[fli_size - 1]);
	}
	for (ls = 0; ls < fli_size; ++ls) {
		s = fli[ls];
		// 1/4 syncope (not for last 1/4 because it is applied with anticipation or sus)
		if (beat[ls] == 3 && llen[ls] > 1) FLAG2(235, s);
		// 1/2 after 1/4
		if (ls > 0 && beat[ls] == 1 && llen[ls] > 1 && llen[ls - 1] == 1) {
			if (bmli[s] >= mli.size() - 2) FLAG2L(238, s, mli[bmli[s]]);
				// Flag slurred if sus or note is cut by scan window
			else if (sus[ls] || (ls == fli_size - 1 && c_len > ep2)) FLAG2L(239, s, mli[bmli[s]]);
			else FLAG2L(240, s, mli[bmli[s]]);
		}
		// Non-uniform starting rhythm
		if (ls > 0 && bmli[s] == 0 && llen[ls] != llen[ls - 1] && ls < fli_size - 1) FLAG2L(254, s, 0);
	}
	return 0;
}

// Fail rhythm for species 5
int CGenCP1::FailRhythm5() {
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
	for (ms = 0; ms < mli.size(); ++ms) {
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
		int has_croche = 0;
		for (ls2 = ls; ls2 < fli_size; ++ls2) {
			if (!ms && fn) pos = fn + max(0, fli[ls2] - s);
			else pos = max(0, fli[ls2] - s);
			// Do not process last note if not full melody generated
			if (ep2 != c_len && ls2 == fli_size - 1) {
				// Last measure without whole note
				if (ms == mli.size() - 1 && l_len.size()) FLAG2(267, fli[fli_size - 1]);
				// Whole inside if it starts not from first measure, from first step and is not a suspension
				if (llen[ls2] >= 8 && ms && !pos && !sus[ls2]) FLAG2(236, s);
					// 1/8 syncope
				else if (llen[ls2] > 1 && pos % 2) FLAG2(232, fli[ls2]);
				// 1/4 syncope (not last, because it is flagged in suspension)
				else if (llen[ls2] > 2 && pos == 2) FLAG2(235, fli[ls2]);
				full_measure = 0;
				break;
			}
			s2 = fli[ls2];
			l_len.push_back(llen[ls2]);
			l_ls.push_back(ls2);
			if (llen[ls2] == 1) has_croche = 1;
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
		if (full_measure && sus[ls2] && fli[ls2] >= s) {
			l_len[l_len.size()-1] = min(8, sus[ls2] - s2);
			slur2 = fli2[ls2] - sus[ls2] + 1;
		}
		// Full evaluation?
		if (ep2 == c_len) {
			// Last measure
			if (ms == mli.size() - 1) {
				// Check last whole note
				if (l_len[0] != 8) 
					FLAG2(267, fli[fli_size - 1]);
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
			if (ep2 == c_len && ls2 == fli_size - 1 && ms == mli.size() - 1) {
				// Check length
				if (l_len[lp] == 1) FLAG2(253, fli[fli_size - 1]);
				else if (l_len[lp] == 2) FLAG2(252, fli[fli_size - 1]);
			}
			// Calculate rhythm id
			if (lp < l_len.size() - 1 || !slur2)
				rid_cur += 1 << (pos + l_len[lp]);
			// Check 1/8
			if (l_len[lp] == 1) {
				// Last 1/8 syncope
				if (pos == 7 && slur2) FLAG2(232, s2);
				// Other types of 1/8
				else {
					// If second 1/8
					if (pos % 2) {
						// Isolated 1/8
						if (l_len[lp - 1] != 1) FLAG2(231, s2);
					}
					// Too many 1/8
					++count8;
					if (count8 == 3) FLAG2(255, s2);
					else if (count8 > 3) ++fpenalty[255];
					// 1/8 in first measure
					if (ms == 0) FLAG2(230, s2);
						// If first 8th
					else {
						// 1/8 beats
						if (pos == 0) FLAG2(226, s2);
						else if (pos == 2) FLAG2(227, s2);
						else if (pos == 4) FLAG2(228, s2);
						else if (pos == 6) FLAG2(229, s2);
					}
				}
				// 1/8 on leap
				if (ls2 < fli_size - 1 && aleap[cpv][s2]) 
					FLAG2(88, s2);
				else if (ls2 > 0 && aleap[cpv][s2 - 1]) {
					if (llen[ls2 - 1] > 1) FLAG2(88, isus[bli[s2 - 1]]);
				}
			}
			else {
				// 1/8 syncope
				if (pos % 2) FLAG2(232, s2);
					// 1/4 syncope
				else if (l_len[lp] > 2 && pos == 2) FLAG2(235, s2);
				//else if (l_len[lp] == 2 && pos == 6 && slur2) FLAG2(235, s2);
			}
			// Uneven starting rhythm
			if (!ms && lp>0 && l_len[lp] != l_len[lp-1]) FLAG2L(254, s2, 0);
			pos += l_len[lp];
		}
		// Check rhythm repeat
		if (full_measure) {
			// Check only if no croches or less than 4 notes
			if (rid.size() && (!has_croche || l_len.size() <4)) {
				// Do not fire for first measure if measure starts with pause
				if (rid.back() == rid_cur && (ms > 1 || !fn)) FLAG2L(247, fli[bli[mli[ms] - 1]], mli[ms - 1]);
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
		if (l_len[0] >= 8 && ms < mli.size() - 1 && ms) FLAG2(236, s);
		// 1/2.
		else if (l_len[0] == 6 && !slur1) FLAG2(233, s);
		else if (l_len.size() > 1 && l_len[1] == 6) FLAG2(234, fli[l_ls[1]], fli[l_ls[0]]);
		else if (l_len.size() > 2 && l_len[2] == 6) FLAG2(234, fli[l_ls[2]], fli[l_ls[0]]);
		// 1/2 after 1/4 or 1/8 in measure
		else if (full_measure && l_len[l_len.size() - 1] == 4 && l_len[0] != 4) {
			s3 = fli[l_ls[l_ls.size() - 1]];
			if (ms >= mli.size() - 2) FLAG2L(238, s3, s);
			else if (slur2 != 0) FLAG2L(239, s3, s);
			else if (slur1 != 0) FLAG2L(278, s3, s);
			else FLAG2L(240, s3, s);
		}
		// Many notes in measure
		if (l_len.size() == 5) {
			if (slur1) FLAG2(301, s);
			else FLAG2L(245, s, fli[bli[s + npm - 1]]);
		}
		else if (l_len.size() > 5) FLAG2L(246, s, fli[bli[s + npm - 1]]);
		// Suspensions
		if (slur1 == 4 && l_len[0] == 2) FLAG2(241, s);
		else if (slur1 == 4 && l_len[0] == 4) FLAG2(242, s);
		//else if (slur1 == 2) FLAG2(251, s)
		if (slur1 && l_len[0] == 6) FLAG2(243, s);
		if (slur1 == 6) FLAG2(244, s);
	}
	// Check last measure
	return 0;
}

int CGenCP1::FailPcoApart() {
	CHECK_READY(DR_fli, DR_ivl, DR_beat);
	CHECK_READY(DR_sus);
	CHECK_READY(DR_msh);
	// Step index of last perfect consonance
	pco5_last = -1000;
	pco8_last = -1000;
	pco5_last2 = -1000;
	pco8_last2 = -1000;
	mli5_last = -1;
	mli8_last = -1;
	int max_ls = fli_size;
	if (ep2 < c_len) max_ls = fli_size - 1;
	for (ls = 0; ls < max_ls; ++ls) {
		s = fli[ls];
		if (sus[ls]) {
			s2 = sus[ls] - 1;
			if (FailPcoApartStep()) return 1;
			s = sus[ls];
			s2 = fli2[ls];
			if (FailPcoApartStep()) return 1;
		}
		else {
			s2 = fli2[ls];
			if (FailPcoApartStep()) return 1;
		}
	}
	return 0;
}

int CGenCP1::FailPcoApartStep2(int iv, int &pco_last, int &mli_last, int &pco_last2) {
	int skip_len;
	int ls_1;
	if (civlc[s] == iv) {
		if (pco_last > -1 && bmli[s] == mli_last + 1) {
			ls_1 = bli[pco_last];
			skip_len = s - pco_last2 - 1;
			if (skip_len > 0 && skip_len < (pco_apart * npm) / 4) {
				// Anticipation
				if (retrigger[s]) {
					// Last contrary
					if (ep2 == c_len && ls == fli_size - 1 && 
						(acc[0][s] - acc[0][pco_last]) * (acc[1][s] - acc[1][pco_last]) < 0) 
						FLAG2L(376, s, pco_last);
					// Other anticipation
					else FLAG2L(315, s, pco_last);
				}
				// Downbeat
				else if (fli[ls] == s && acc[cfv][s] != acc[cfv][s - 1]) {
					// Last contrary
					if (ep2 == c_len && ls == fli_size - 1 &&
						(acc[0][s] - acc[0][pco_last]) * (acc[1][s] - acc[1][pco_last]) < 0) 
						FLAG2L(376, s, pco_last);
					// Other downbeat
					else 
						FLAG2L(316, s, pco_last);
				}
				// Many notes in between
				else if (ls - ls_1 > 4) {}
				// Compound
				else if (civl[s] != civl[pco_last]) {
					// Direct compound
					if ((acc[0][s] - acc[0][pco_last]) * (acc[1][s] - acc[1][pco_last]) > 0) {
						FLAG2L(347, s, pco_last);
					}
					// Contrary compound
					else FLAG2L(248, s, pco_last);
				}
				// Stepwize
				else if ((!sus[ls_1] && (!pco_last || asmooth[cpv][pco_last - 1]) &&
					(pco_last2 + 1 >= ep2 || asmooth[cpv][pco_last2])) ||
					(!sus[ls] && (!s || asmooth[cpv][s - 1]) &&
					(s2 + 1 >= ep2 || asmooth[cpv][s2])))
					FLAG2L(249, s, pco_last);
				// Asymmetric
				else if (beat[ls_1] != beat[ls]) FLAG2L(374, s, pco_last);
				// Other
				else {
					if (iv == 7) {
						// In case of 6-5 resolution, allow parallel 5th
						if (bmli[sus[ls_1]] == bmli[s] && sus[ls_1] &&
							ivlc[sus[ls_1]] == 5 && ivlc[s] == 4 &&
							abs(ac[cpv][s] - ac[cpv][pco_last]) < 2)
							FLAG2L(330, s, pco_last);
						// Other
						else FLAG2L(250, s, pco_last);
					}
					else {
						FLAG2L(250, s, pco_last);
					}
				}
			}
		}
		pco_last = s;
		pco_last2 = s2;
		mli_last = bmli[s];
	}
	return 0;
}

int CGenCP1::FailPcoApartStep() {
	if (FailPcoApartStep2(7, pco5_last, mli5_last, pco5_last2)) return 1;
	if (FailPcoApartStep2(0, pco8_last, mli8_last, pco8_last2)) return 1;
	return 0;
}

int CGenCP1::FailVIntervals() {
	CHECK_READY(DR_fli, DR_ivl, DR_msh);
	CHECK_READY(DR_retrigger);
	CHECK_READY_PERSIST(DP_mli);
	CHECK_READY(DR_motion, DR_culm_ls, DR_sus);
	// Number of sequential parallel imperfect consonances
	int pico_count = 0;
	pm_pico = 0;
	int fired = 0;
	// Check first step
	if (tivl[0] == iDis) {
		if (fn) FLAG2(359, 0);
		else FLAG2(83, 0);
	}
	for (ls = 1; ls < fli_size; ++ls) {
		s = fli[ls];
		s2 = fli2[ls];
		if (FailUnison()) return 1;
		if (FailDis()) return 1;
		if (FailPco()) return 1;
		// Long parallel ico
		if (tivl[s] == iIco && ivl[s] == ivl[fli2[ls - 1]]) {
			++pico_count;
			++pm_pico;
			// Two same ico transitions means three intervals already
			if (pico_count == ico_chain-1) {
				if (civlc[s] == 3 || civlc[s] == 4) {
					FLAG2L(89, s, isus[ls - 2]);
				}
				else {
					FLAG2L(389, s, isus[ls - 2]);
				}
			}
			else if (pico_count >= ico_chain2) {
				if (!fired) {
					if (civlc[s] == 3 || civlc[s] == 4) {
						FLAG2L(96, s, isus[ls - 3]);
					}
					else {
						FLAG2L(390, s, isus[ls - 3]);
					}
					fired = 1;
				}
				else {
					fpenalty[96] += severity[96] + 1;
				}
			}
		}
		else {
			pico_count = 0;
			fired = 0;
		}
	}
	return 0;
}

int CGenCP1::FailVirtual4th() {
	CHECK_READY(DR_hli, DR_fli, DR_ivl);
	// Only for upper cantus
	if (!cantus_high) return 0;
	// Only for species 2-5
	if (species < 2) return 0;
	int ls1, ls2, ls6;
	for (int hs = 0; hs < hli.size(); ++hs) {
		ls1 = bli[hli[hs]];
		ls2 = bli[hli2[hs]];
		int s6 = -1;
		int s3 = -1;
		int s6_dupl = 0;
		// Loop inside harmony
		for (ls = ls1; ls <= ls2; ++ls) {
			s = fli[ls];
			// Do not go out of measure
			if (s < hli[hs]) s = hli[hs];
			// Find lowest 6th interval
			if (ivlc[s] == 5) {
				if (s6 == -1) s6 = s;
				else if (acc[0][s] < acc[0][s6]) s6 = s;
				else if (acc[0][s] == acc[0][s6]) ++s6_dupl;
			}
			// Find 3rd interval
			if (ivlc[s] == 2 && s3 == -1) s3 = s;
		}
		// Need to have both intervals
		if (s6 == -1 || s3 == -1) continue;
		// Chord 5th should be lowest harmonic note
		if (hbcc[hs] < acc[0][s6]) continue;
		// Correct interval (not tritone)
		if ((acc[0][s3] - acc[0][s6]) % 12 != 5) continue;
		ls6 = bli[s6];
		// Beat 1
		if (!beat[ls6]) FLAG2L(379, s6, s3);
		// Beat 3
		else if (beat[ls6] == 1) FLAG2L(380, s6, s3);
		// Long
		if ((ls6 == 0 || rlen[ls6] >= rlen[ls6 - 1]) &&
			(ls6 == fli_size - 1 || rlen[ls6] >= rlen[ls6 + 1]) &&
			rlen[ls6] >= 4) FLAG2L(381, s6, s3);
		// Repeats
		if (s6_dupl) 
			FLAG2L(382, s6, s3);
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
	CHECK_READY_PERSIST(DP_mli);
	CHECK_READY(DR_fli);
	// For species 5 there are separate rules (FailRhythm5)
	if (species == 5) return 0;
	// Number of sequential slurs 
	int scount = 0;
	// Number of slurs in window
	int scount2 = 0;
	int max_count = 0;
	int max_i = 0;
	// Check pause length
	if (fn * 2 > npm) FLAG2(197, 0);
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
			//if (scount > 1) FLAG2(97, i);
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
	max_i = fli[bli[max_i]];
	if (species != 4) {
		if (max_count == 1) FLAG2(93, max_i);
		else if (max_count == 2) FLAG2(94, max_i);
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
	int wsize = 0;
	// Number of slurs in window
	int scount = 0;
	int miss, max_miss=0;
	int max_i=0;
	for (int i = 0; i < ep2-1; ++i) if ((i + fn) % 2) { 
		if (i < miss_slurs_window * npm) ++wsize;
		// Subtract old slur
		if ((i >= miss_slurs_window * npm) && 
			(acc[cpv][i - miss_slurs_window * npm] == acc[cpv][i - miss_slurs_window * npm + 1])) 
			--scount;
		if (acc[cpv][i] == acc[cpv][i + 1]) {
			// Check slurs in window
			++scount;
		}
		else {
			miss = wsize - scount;
			if (miss > max_miss) {
				max_miss = miss;
				max_i = i;
			}
		}
	}
	if (max_miss == 1) FLAG2(188, max_i);
	else if (max_miss == 2) FLAG2(189, max_i);
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
	int between;
	int fired = 0;
	int to_fire = 0;
	int fire_start = 0;
	pm_between_min = INT_MAX;
	pm_between_max = 0;
	for (ls = 0; ls < fli_size; ++ls) {
		s = fli[ls];
		// Check between
		between = acc[1][s] - acc[0][s];
		// Record
		if (between > pm_between_max) pm_between_max = between;
		if (between < pm_between_min) pm_between_min = between;
		to_fire = 0;
		if (between > max_between) {
			++bsteps;
			// Flag very far burst
			if (acc[1][s] - acc[0][s] > burst_between || bsteps > burst_steps) to_fire = 1;
		}
		else bsteps = 0;
		if (to_fire) {
			if (!fired) fire_start = s;
			fired = 1;
			fpenalty[11] += severity[11] + 1;
		}
		if (fired) {
			if (!to_fire) {
				FLAG2L(11, fire_start, fli[ls - 1]);
				fired = 0;
			}
			else if (ls == fli_size - 1) {
				fired = 0;
				FLAG2L(11, fire_start, fli[ls]);
			}
		}
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
			if (acc[cpv][i] >= acc[cfv][i - 1]) FLAG2L(24, i, fli[bli[i - 1]]);
			else if (acc[cfv][i] <= acc[cpv][i - 1]) FLAG2L(24, i, fli[bli[i - 1]]);
		}
	}
	else {
		for (int i = fli[1]; i < ep2; ++i) {
			if (acc[cpv][i] == acc[cpv][i - 1] || acc[cfv][i] == acc[cfv][i - 1]) continue;
			if (acc[cpv][i] <= acc[cfv][i - 1]) FLAG2L(24, i, fli[bli[i - 1]]);
			else if (acc[cfv][i] >= acc[cpv][i - 1]) FLAG2L(24, i, fli[bli[i - 1]]);
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
	fn_source = fn;
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
				int step = t_sent;
				// Add line
				linecolor[t_sent] = MakeColor(255, 0, 0, 0);
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
	est.Format("Finished SWA%d #%d: rp %.0f from %.0f, dp %d, cnum %ld (in %lld ms): %s",
		swa_len, a+1, rpenalty_min, rpenalty_source, dpenalty_min, cnum, time_stop - time_start, sst);
	WriteLog(0, est);
	TestBestRpenalty();
}

int CGenCP1::FailFirstIntervals() {
	CHECK_READY(DR_fli, DR_pc, DR_ivl);
	if (apc[0][0] == 0) {
		if (apc[1][0] == 0) FLAG2(268, 0);
		else if (apc[1][0] == 4) FLAG2(269, 0);
		else if (apc[1][0] == 2) FLAG2(270, 0);
		else FLAG2(272, 0);
	}
	else if (apc[0][0] == 2) {
		if (apc[1][0] == 0) FLAG2(271, 0);
		else FLAG2(272, 0);
	}
	else FLAG2(272, 0);
	return 0;
}

int CGenCP1::FailLastIntervals() {
	CHECK_READY_PERSIST(DP_mli);
	CHECK_READY(DR_fli, DR_pc, DR_ivl);
	int fs;
	// Do not check if melody is short yet
	if (fli_size < 3) return 0;
	if (ep2 >= c_len) {
		s = fli[fli_size - 1];
		s_1 = fli[fli_size - 2];
		s_2 = fli[fli_size - 3];
		fs = max(mli[mli.size() - 1], fli[fli_size - 1]);
		// Check last intervals
		if (apc[0][c_len - 1] == 0) {
			if (apc[1][c_len - 1] == 0) FLAG2(354, fs);
			else if (apc[1][c_len - 1] == 4) FLAG2(355, fs);
			else if (apc[1][c_len - 1] == 2) FLAG2(356, fs);
			else FLAG2(358, fs);
		}
		else if (apc[0][c_len - 1] == 2) {
			if (apc[1][c_len - 1] == 0) FLAG2(357, fs);
			else FLAG2(358, fs);
		}
		else FLAG2(358, fs);
		// Prohibit major second up before I (applicable to major and minor)
		if (apcc[cpv][s] == 0 && apcc[cpv][s_1] == 10) FLAG2L(74, s_1, s);
		if (apcc[cpv][s] == 0 && apcc[cpv][s_2] == 10) FLAG2L(74, s_2, s);
		// Scan 2nd to last measure
		if (mli.size() > 1) {
			int start = mli[mli.size() - 2];
			int end = mli[mli.size() - 1] - 1;
			if (end < ep2) {
				int b_found = 0;
				int bb_found = 0;
				int bb_step = 0;
				int g_found = 0;
				int d_found = 0;
				for (int x = start; x <= end; ++x) {
					for (int v = 0; v < av_cnt; ++v) {
						if (apcc[v][x] == 11) b_found = 1;
						if (apcc[v][x] == 10 && !bb_found) {
							bb_found = 1;
							bb_step = x;
						}
						if (apc[v][x] == 4) g_found = 1;
						if (apc[v][x] == 1) d_found = 1;
					}
				}
				if (!b_found) {
					// Set B needed flag if last bass notes are not G-C
					if (apc[0][c_len - 1] != 0 || apc[0][fli[fli_size - 2]] != 4) 
						FLAG2(47, max(start, fli[bli[end]]));
				}
				if (bb_found) {
					FLAG2(317, bb_step);
				}
				if (!g_found && !d_found) FLAG2(75, max(start, fli[bli[end]]));
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
		// Beats for species 1: 0 0 0 0
		// Beats for species 2: 0 1 0 1
		// Beats for species 3: 0 3 1 5
		// Beats for species 4: 0 1 0 1
		// Beats for species 5: 0 10 3 11 1 12 5 13
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
		// Build isus
		isus[ls] = sus[ls] ? sus[ls] : fli[ls];
	}
}

// Create links to unique note columns
void CGenCP1::CreateULinks() {
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
	SET_READY_PERSIST(DP_mli);
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
		if (task != tEval && !warn_wrong_fn) {
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
				RuleName[rule_set][273], SubRuleName[rule_set][273], species);
			WriteLog(5, est);
			++warn_wrong_fn;
			return 1;
		}
		FLAG2(273, 0);
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
		CheckSASEmulatorFlags(scpoint[cpv]);
		nflags_prev = anflags[cpv];
	}
	if (!need_exit) OutputFlagDelays();
	fixed_ep2 = 0;
	SetStatusText(7, "SAS emulator: finished");
}

// Get links to cantus notes
void CGenCP1::GetCfli() {
	SET_READY_PERSIST(DP_cfli);
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
				if (gis_trail) FLAG2L(200, s, fli[max(0, ls - gis_trail_max + gis_trail)]);
			}
		}
		// Decrease if not zero
		if (gis_trail) --gis_trail;
	}
	return 0;
}

int CGenCP1::FailHarmStep(int i, const int* hv, int &count, int &wcount) {
	if (hv[chm[i]]) {
		++count;
		wcount = 0;
	}
	else {
		++wcount;
		count = 0;
	}
	if (count > repeat_letters && !hrepeat_fired) {
		FLAG2(17, s);
		hrepeat_fired = 1;
	}
	if (wcount > miss_letters && !hmiss_fired) {
		FLAG2(20, s);
		hmiss_fired = 1;
	}
	return 0;
}

int CGenCP1::EvalHarm() {
	int pen1;
	int p2c = 0; // Count of consecutive penalty 2
	int p3c = 0; // Count of consecutive penalty 3
	int dcount = 0;
	int scount = 0;
	int tcount = 0;
	int wdcount = 0;
	int wscount = 0;
	int wtcount = 0;
	int harm_end, found;
	for (int i = 0; i < chm.size(); ++i) {
		s = hli[i];
		ls = bli[s];
		if (i > 0) {
			// Check GC for low voice and not last note (last note in any window is ignored)
			if (ls < fli_size - 1 && 
				chm[i] == 0 && chm[i - 1] == 4 && 
				apc[0][s] == 0 && apc[1][s] == 0 &&
				s > 0 && apc[0][s - 1] == 4) FLAG2(48, s);
			// Prohibit 64 chord
			if ((hbc[i] % 7 - chm[i] + 7) % 7 == 4) {
				FLAG2(383, s);
			}
			if (minor_cur) {
				// Prohibit VI<->VI# containing progression
				if (chm[i] % 2 && chm[i - 1] % 2 && chm_alter[i] * chm_alter[i - 1] == -1) {
					FLAG2(377, s);
				}
				// Prohibit VII<->VII# containing progression
				if (chm[i] && chm[i] % 2 == 0 && chm[i - 1] && chm[i - 1] % 2 == 0 && 
					chm_alter[i] * chm_alter[i - 1] == -1) {
					FLAG2(378, s);
				}
				// Prohibit DTIII#5 augmented chord
				if (chm[i] == 2 && chm_alter[i] == 1) {
					FLAG2(375, s);
				}
				// Prohibit dVII (GBD) in root position after S (DF#A) in root position
				if (chm[i] == 6 && chm[i - 1] == 3 && chm_alter[i]<1 && chm_alter[i - 1] == 1) {
					if (ls > 0 && apc[0][s] == 6 && apc[0][fli[ls - 1]] == 3) FLAG2(308, s);
				}
				// Prohibit DTIII (CEG) in root position after dVII (GBD) in root position
				if (chm[i] == 2 && chm[i - 1] == 6 && chm_alter[i]<1 && chm_alter[i - 1]<1) {
					if (ls > 0 && apc[0][s] == 2 && apc[0][fli[ls - 1]] == 6) FLAG2(309, s);
				}
			}
			// Check harmonic penalty	
			pen1 = hsp[chm[i - 1]][chm[i]];
			if (pen1 == 1) FLAG2(77, s);
			if (pen1 == 2) {
				++p2c;
				if (p2c == 1) FLAG2(57, s);
				else if (p2c == 2) FLAG2(92, s);
				else if (p2c == 3) FLAG2(23, s);
			}
			else {
				p2c = 0;
			}
			if (pen1 == 3) {
				++p3c;
				if (p3c == 1) FLAG2(99, s);
				else if (p3c == 2) FLAG2(321, s);
			}
			else {
				p3c = 0;
			}
		}
		// Check letter repeat and miss
		hrepeat_fired = 0;
		hmiss_fired = 0;
		if (FailHarmStep(i, hvt, tcount, wtcount)) return 1;
		if (FailHarmStep(i, hvd, dcount, wdcount)) return 1;
		if (FailHarmStep(i, hvs, scount, wscount)) return 1;
	}
	return 0;
}


int CGenCP1::FailTonicCP() {
	int tcount = 0;
	int fire, fired = 0;
	// Do not check if melody is short
	if (hli.size() < 3) return 0;
	// Loop from second to second to last note
	for (int hs = 1; hs < hli.size() - 1; ++hs) {
		s = hli[hs];
		// Decrement for previous tonic note
		if (hs > tonic_window_cp) {
			if (!chm[hs - tonic_window_cp]) --tcount;
		}
		if (!chm[hs]) {
			// Increment for current tonic note
			++tcount;
			// Check count of tonic notes
			if (tcount > tonic_max_cp) {
				// Grant one more tonic in first window if first chord not tonic
				fire = 0;
				if (hs < tonic_window_cp && chm[0]) {
					if (tcount > tonic_max_cp + 1)	fire = 1;
				}
				else fire = 1;
				if (fire) {
					if (fired) {
						fpenalty[310] += severity[310] + 1;
					}
					else {
						FLAG2(310, s);
						fired = 1;
					}
				}
			}
		}
	}
	return 0;
}

void CGenCP1::RemoveHarmDuplicate() {
	int chm_id = hli.size() - 1;
	// Need to be at least two harmonies
	if (chm_id == 0) return;
	// Harmony should be not first in measure
	if (hli[chm_id] <= mli[ms]) return;
	// Harmonies should match
	if (chm[chm_id] != chm[chm_id - 1]) return;
	// Alterations should match
	if (chm_alter[chm_id] * chm_alter[chm_id - 1] == -1) return;
	// Remove duplicate
	hli.resize(chm_id);
	hli2.resize(chm_id);
	chm.resize(chm_id);
	chm_alter.resize(chm_id);
	hbc.resize(chm_id);
	hbcc.resize(chm_id);
}

int CGenCP1::FailHarm() {
	CHECK_READY(DR_fli, DR_c, DR_pc);
	CHECK_READY_PERSIST(DP_mli);
	SET_READY(DR_hli);
	int ls1, ls2 = 0;
	int s9, hs;
	int r, n, ns, harm_conflict, hcount;
	int last_b; // First harmony in measure has b
	vector<int> chn, cchn;
	int mea_end;
	chn.resize(7);
	cchn.resize(12);
	hli.clear();
	hli2.clear();
	chm.clear();
	hbcc.clear();
	hbc.clear();
	chm_alter.clear();
	// Build chm vector
	for (ms = 0; ms < mli.size(); ++ms) {
		// Stop processing when last measure is not fully generated
		if (ms == mli.size() - 1 && ep2 < c_len) break;
		// Get first and last measure notes
		if (ms < mli.size() - 1) mea_end = mli[ms + 1] - 1;
		else mea_end = c_len - 1;
		// Prevent going out of window
		if (mea_end >= ep2) break;
		ls1 = bli[mli[ms]];
		ls2 = bli[mea_end];
		// Do not process harmony if it can be influenced by interbar patterns
		if (ep2 < c_len && ls2 >= fli_size - 4) break;
		r = ac[cfv][mli[ms]] % 7;
		// Do not process end of sus
		//if (sus[ls1]) ls1++;
		// Clear harmonic notes vector
		fill(chn.begin(), chn.end(), 0);
		fill(cchn.begin(), cchn.end(), 0);
		hli.push_back(mli[ms]);
		hli2.push_back(0);
		hs = hli.size() - 1;
		if (hli2.size() > 1) hli2[hli2.size() - 2] = hli[hli.size() - 1] - 1;
		hbcc.push_back(acc[cfv][mli[ms]]);
		hbc.push_back(ac[cfv][mli[ms]]);
		chm.push_back(r);
		chm_alter.push_back(0);
		++chn[0];
		// Record cantus alteration
		++cchn[apcc[cfv][mli[ms]]];
		hcount = 0;
		// Make last leading tone in penultimate measure harmonic
		if (ms == mli.size() - 2 && fli_size > 1) {
			int s9 = fli[fli_size - 2];
			if (apcc[cpv][s9] == 11 && tivl[s9] > 0 && msh[fli_size - 2] < 0) {
				msh[fli_size - 2] = pLastLT;
			}
		}
		// Loop inside measure
		for (ls = ls1; ls <= ls2; ++ls) {
			// For first suspension in measure, evaluate last note. In other cases - first note
			if (ls == ls1 && sus[ls1]) s9 = fli2[ls];
			else s9 = fli[ls];
			// Do not process non-harmonic notes if they are not consonant ending of first sus
			// Sus ending 4th is also not processed
			// For first suspended note do not check msh
			if (ls == ls1 && sus[ls]) {
				if (tivl[s9] < 0) continue;
			}
			// For all other notes, check msh and iHarm4
			else {
				if (msh[ls] <= 0) continue;
			}
			s = fli[ls];
			// Pitch class
			n = ac[cpv][s9] % 7;
			// Pitch class from root
			ns = (n - r + 7) % 7;
			// Find conflicting notes
			harm_conflict = 0;
			if (chn[(ns + 1) % 7] || chn[(ns + 6) % 7]) harm_conflict = 1;
			// Start new harmony if harmonic conflict
			if (harm_conflict && s > mli[ms]) {
				RemoveHarmDuplicate();
				// More than two harmonies
				if (hcount) FLAG2(40, s);
				else {
					// Two harmonies penultimate
					if (ms == mli.size() - 2) FLAG2(306, s);
					else {
						// Stepwize resolution of 5th to 6th or 6th to 5th with two harmonies in measure
						if (sus[ls1] && (
							(ivlc[mli[ms]] == 4 && ivlc[mli[ms] + npm / 2] == 5) ||
							(ivlc[mli[ms]] == 5 && ivlc[mli[ms] + npm / 2] == 4)) &&
							abs(ac[cpv][mli[ms]] - ac[cpv][mli[ms] + npm / 2]) < 2)
							FLAG2(329, s);
						else FLAG2(307, s);
					}
				}
				harm_conflict = 0;
				fill(chn.begin(), chn.end(), 0);
				fill(cchn.begin(), cchn.end(), 0);
				hli.push_back(s);
				hli2.push_back(0);
				hs = hli.size() - 1;
				if (hli2.size() > 1) hli2[hli2.size() - 2] = hli[hli.size() - 1] - 1;
				chm.push_back(r);
				hbcc.push_back(acc[cfv][mli[ms]]);
				hbc.push_back(ac[cfv][mli[ms]]);
				chm_alter.push_back(0);
				++chn[0];
				// Record cantus alteration
				++cchn[apcc[cfv][mli[ms]]];
				// Next harmony counter
				++hcount;
			}
			// Record note
			++chn[ns];
			++cchn[apcc[cpv][s9]];
			// Detect harmony
			if (cantus_high) {
				if (chn[3]) chm[hs] = (r + 3) % 7;
				else if (chn[5]) chm[hs] = (r + 5) % 7;
				else if (chn[1]) chm[hs] = (r + 1) % 7;
				else chm[hs] = r;
			}
			else {
				if (chn[3]) chm[hs] = (r + 3) % 7;
				else if (chn[5]) chm[hs] = (r + 5) % 7;
				else chm[hs] = r;
			}
			// Detect altered chord
			if (minor_cur && (cchn[11] || cchn[9])) chm_alter[hs] = 1;
			// Detect unaltered chord
			if (minor_cur && (cchn[10] || cchn[8])) chm_alter[hs] = -1;
		}
		RemoveHarmDuplicate();
		// If penultimate measure
		if (ms == mli.size() - 2 && hcount) {
			// Prohibit D or DVII harmony in penultimate measure before non-D / DVII harmony
			if (chm.size() > 1 && (chm[chm.size() - 2] == 4 || chm[chm.size() - 2] == 6) &&
				(chm[chm.size() - 1] != 4 && chm[chm.size() - 1] != 6)) FLAG2(322, hli[chm.size() - 1]);
		}
		if (ls2 && hli2.size()) hli2[hli2.size() - 1] = fli2[ls2];
	}
	GetBhli();
	// Check penultimate harmony not D / DVII
	if (ep2 == c_len && hli.size() > 1) {
		hs = hli.size() - 2;
		if (chm[hs] != 4 && chm[hs] != 6) FLAG2(335, hli[hs]);
		// If penultimate cantus note is not LT
		if (mli.size() > 1 && apcc[cfv][mli[mli.size() - 2]] != 11) {
			// Find last leading tone
			last_b = -1;
			int min_ls = bli[mli[mli.size() - 2]];
			for (ls = fli_size - 1; ls >= min_ls; --ls) {
				s = fli[ls];
				if (apcc[cpv][s] == 11) {
					last_b = s;
					break;
				}
			}
			// Check if LT harmony is separated from last harmony with harmony without LT
			if (last_b > -1) {
				if (bhli[last_b] < hli.size() - 2) FLAG2(318, last_b);
			}
		}
	}
	GetHarmBass();
	if (EvalHarm()) return 1;
	if (FailTonicCP()) return 1;
	if (FailVirtual4th()) return 1;
	return 0;
}

void CGenCP1::GetBhli() {
	fill(bhli.begin(), bhli.end(), 0);
	for (int hs = 0; hs < chm.size(); ++hs) {
		for (s = hli[hs]; s <= hli2[hs]; ++s) {
			bhli[s] = hs;
		}
	}
} 

void CGenCP1::GetHarmBass() {
	SET_READY(DR_hbc);
	// Do not process for lower cantus, because in this case lowest note is cantus
	if (!cantus_high) return;
	int ls1, ls2;
	int harm_end, nt;
	int de1, de2, de3;
	for (int hs = 0; hs < hli.size(); ++hs) {
		// Get harmonic notes
		de1 = chm[hs];
		de2 = (de1 + 2) % 7;
		de3 = (de1 + 4) % 7;
		// Loop inside harmony
		for (ls = bli[hli[hs]]; ls <= bli[hli2[hs]]; ++ls) if (msh[ls] > 0) {
			s = fli[ls];
			nt = ac[cpv][s] % 7;
			// Do not process notes that are not harmonic
			if (nt != de1 && nt != de2 && nt != de3) continue;
			if (hbcc[hs] > acc[0][s]) hbcc[hs] = acc[0][s];
			if (hbc[hs] > ac[0][s]) hbc[hs] = ac[0][s];
		}
	}
}

void CGenCP1::OptimizeLastMeasure() {
	// Do not optimize when evaluating
	if (task == tEval || task == tCor) return;
	// Do not optimize if last note non-whole allowed
	if (accept[267]) return;
	// Optimize only starting step of last measure
	if (ep2 < c_len) return;
	// Duplicate notes
	for (s = mli.back(); s < c_len - 1; ++s) {
		acc[cpv][s] = acc[cpv][c_len - 1];
	}
}

int CGenCP1::FailMaxNoteLen() {
	CHECK_READY(DR_fli);
	// Never check last note, either end of scan window or end of counterpoint
	for (ls = 0; ls < fli_size - 1; ++ls) {
		if (rlen[ls] > max_note_len[species] * 2) FLAG2(336 + species - 1, fli[ls]);
		// Check notes crossing multiple measures
		if (bmli[fli2[ls]] - bmli[fli[ls]] > 1) FLAG2(41, fli[ls]);
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
		OptimizeLastMeasure();
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
			if (nmax - nmin > max_interval) FLAG(304, 0);
			if (cantus_high) {
				if (cf_nmax - nmin > sum_interval) FLAG(7, 0);
			}
			else {
				if (nmax - cf_nmin > sum_interval) FLAG(7, 0);
			}
			if (c_len == ep2 && nmax - nmin < min_interval) FLAG(38, 0);
		}
		CreateLinks(acc[cpv], 1);
		GetMovingMax(acc[cpv], max(lclimax_notes, lclimax_mea*npm), lclimax);
		GetMovingMax(acc[cpv], lclimax_mea2*npm, lclimax2);
		if (FailMaxNoteLen()) goto skip;
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
		//CreateULinks();
		if (minor_cur) {
			if (FailMinor(apcc[cpv], acc[cpv])) goto skip;
			//if (FailGisTrail2()) goto skip;
			if (FailGisTrail(apcc[cpv])) goto skip;
			if (FailFisTrail(apcc[cpv])) goto skip;
		}
		//if (acc[cpv][1] == 105 && acc[cpv][2] == 98)
			//WriteLog(1, "Found");
		if (FailCPInterval()) goto skip;
		GetNoteTypes();
		GetLeapSmooth(ac[cpv], acc[cpv], aleap[cpv], asmooth[cpv], aslur[cpv]);
		if (FailRhythm()) goto skip;
		GetVIntervals();
		Get4th();
		GetBasicMsh();
		if (FailSus()) goto skip;
		if (DetectPatterns()) goto skip;
		ApplyFixedPat();
		if (FailMultiCulm(acc[cpv], aslur[cpv])) goto skip;
		if (FailVMotion()) goto skip;
		if (FailVIntervals()) goto skip;
		if (FailTonic(acc[cpv], apc[cpv])) goto skip;
		if (FailLastIntervals()) goto skip;
		//if (FailNoteSeq(apc[cpv])) goto skip;
		if (FailLastNoteRes(apc[cpv])) goto skip;
		if (FailIntervals(ac[cpv], acc[cpv], apc[cpv], apcc[cpv])) goto skip;
		if (npm > 1) {
			if (FailAdjacentTritones()) goto skip;
			if (FailTritones2()) goto skip;
		}
		else {
			if (FailTritones(ac[cpv], acc[cpv], apc[cpv], apcc[cpv], aleap[cpv])) goto skip;
		}
		if (species == 5) {
			if (FailManyLeaps(ac[cpv], acc[cpv], aleap[cpv], asmooth[cpv], aslur[cpv], max_leaps, max_leaped, max_leaps, max_leaped, max_leap_steps, 3, 25, 3, 25)) goto skip;
			if (FailManyLeaps(ac[cpv], acc[cpv], aleap[cpv], asmooth[cpv], aslur[cpv], max_leaps2, max_leaped2, max_leaps2, max_leaped2, max_leap_steps2, 202, 203, 202, 203)) goto skip;
		}
		else {
			if (FailManyLeaps(ac[cpv], acc[cpv], aleap[cpv], asmooth[cpv], aslur[cpv], max_leaps, max_leaped, max_leaps5, max_leaped5, max_leap_steps, 3, 25, 311, 312)) goto skip;
			if (FailManyLeaps(ac[cpv], acc[cpv], aleap[cpv], asmooth[cpv], aslur[cpv], max_leaps2, max_leaped2, max_leaps6, max_leaped6, max_leap_steps2, 202, 203, 313, 314)) goto skip;
		}
		if (FailLeapSmooth(ac[cpv], acc[cpv], aleap[cpv], asmooth[cpv], aslur[cpv], max_smooth2, max_smooth_direct2, 302, 303, 1)) goto skip;
		if (FailOutstandingRepeat(ac[cpv], acc[cpv], aleap[cpv], repeat_steps2, 2, 76)) goto skip;
		if (FailOutstandingRepeat(ac[cpv], acc[cpv], aleap[cpv], repeat_steps3, 3, 36)) goto skip;
		if (FailLongRepeat(ac[cpv], acc[cpv], aleap[cpv], repeat_steps5, 5, 72)) goto skip;
		if (FailLongRepeat(ac[cpv], acc[cpv], aleap[cpv], repeat_steps7, 7, 73)) goto skip;
		// Calculate diatonic limits
		nmind = CC_C(nmin, tonic_cur, minor_cur);
		nmaxd = CC_C(nmax, tonic_cur, minor_cur);
		if (FailGlobalFill(ac[cpv], nstat2)) goto skip;
		if (npm == 1) {
			if (FailLocalRange(ac[cpv], notes_lrange1, min_lrange1, 352)) goto skip;
			if (FailLocalRange(ac[cpv], notes_lrange12, min_lrange12, 353)) goto skip;
			if (FailLocalRange(ac[cpv], notes_lrange13, min_lrange13, 351)) goto skip;
		}
		else if (npm == 2) {
			if (FailLocalRange(ac[cpv], notes_lrange, min_lrange, 98)) goto skip;
			if (FailLocalRange(ac[cpv], notes_lrange2, min_lrange2, 198)) goto skip;
			if (FailLocalRange(ac[cpv], notes_lrange3, min_lrange3, 300)) goto skip;
		}
		else {
			if (FailLocalRange(ac[cpv], notes_lrange0, min_lrange0, 396)) goto skip;
			if (FailLocalRange(ac[cpv], notes_lrange02, min_lrange02, 397)) goto skip;
			if (FailLocalRange(ac[cpv], notes_lrange03, min_lrange03, 395)) goto skip;
		}
		if (FailAlteredInt()) goto skip;
		if (FailCrossInt()) goto skip;
		if (FailPcoApart()) goto skip;
		if (FailOverlap()) goto skip;
		if (FailStagnation(acc[cpv], nstat, stag_note_steps, stag_notes, 10)) goto skip;
		if (FailStagnation(acc[cpv], nstat, stag_note_steps2, stag_notes2, 39)) goto skip;
		if (FailFirstIntervals()) goto skip;
		if (FailLeap(ac[cpv], acc[cpv], aleap[cpv], asmooth[cpv], nstat2, nstat3)) goto skip;
		//if (FailMelodyHarm(apc[cpv], 0, ep2)) goto skip;
		MakeMacc(acc[cpv]);
		if (FailLocalMacc(notes_arange, min_arange, 15)) goto skip;
		if (FailLocalMacc(notes_arange2, min_arange2, 16)) goto skip;
		if (FailHarm()) goto skip;

		//LogCantus("Rpenalty", rpenalty_cur, flags);
		SaveBestRejected(acc[cpv]);
		if (task == tCor && method == mSWA) {
			if (skip_flags) {
				SET_READY(DR_rpenalty_cur);
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
			WriteLog(0, st);
		}
	}
	else {
		st.Format("Warning: no CF=high or CF=low lyrics for counterpoint #%d. Assuming %s",
			cantus_id + 1, cantus_high ? "high" : "low");
		WriteLog(0, st);
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

void CGenCP1::TransposeCantus() {
	for (int cantus_id = 0; cantus_id < cantus.size(); ++cantus_id) {
		// Get current average pitch
		int pitch_sum = 0;
		// If reduce_between is zero, do not process
		if (!transpose_cantus) return;
		for (s = 0; s < cantus[cantus_id].size(); ++s) {
			pitch_sum += cantus[cantus_id][s];
		}
		int src_oct = pitch_sum / cantus[cantus_id].size() / 12;
		// Transpose
		for (s = 0; s < cantus[cantus_id].size(); ++s) {
			cantus[cantus_id][s] += (transpose_cantus - src_oct + cantus_high) * 12;
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
	CString st;
	LoadCantus(midi_file);
	ShrinkCantus();
	TransposeCantus();
	if (cantus.size() < 1) return;
	for (int a = 0; a < INT_MAX; ++a) {
		// Reset note scan range to ignore it for showing cantus
		min_cc0.clear();
		max_cc0.clear();
		// Choose cantus to use
		cantus_id = randbw2(0, cantus.size() - 1);
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
		// Show status
		st.Format("IN: %s #%d", fname_from_path(midi_file), cantus_id + 1);
		SetStatusText(8, st);
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
		midifile_out_mul2 = 8;
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
		ProcessSpecies();
		// Load first voice
		fn = fn0;
		vector<int> cc_len_old = cc_len;
		vector<float> cc_tempo_old = cc_tempo;
		vector<vector<int>> anflags_old = anflags[cfv];
		vector<vector<int>> anfl_old = anfl[cfv];
		c_len = m_c.size() * npm - fn;
		ac[cfv].clear();
		acc[cfv].clear();
		apc[cfv].clear();
		apcc[cfv].clear();
		cc_len.clear();
		cc_tempo.clear();
		anflags[cfv].clear();
		anfl[cfv].clear();
		// Create empty arrays
		anflags[cfv].resize(m_c.size()*npm);
		anfl[cfv].resize(m_c.size()*npm);
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
					anfl[cfv][y] = anfl_old[i];
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
