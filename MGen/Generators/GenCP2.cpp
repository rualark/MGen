#include "../stdafx.h"
#include "GenCP2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CGenCP2::CGenCP2() {
	av_cnt = 2;
	v_cnt = 2;
	track_id[0] = 2;
	track_id[1] = 1;
	vsize.resize(av_cnt);
}

CGenCP2::~CGenCP2() {
}

void CGenCP2::LoadConfigLine(CString * sN, CString * sV, int idata, float fdata) {
	CGenCP1::LoadConfigLine(sN, sV, idata, fdata);
}

void CGenCP2::InitMeasures() {
	// Set real voice length
	vsize[cfv] = c_len;
	vsize[cpv] = c_len * notes_per_measure;
	mid.resize(vsize[cpv]);
	mpos.resize(vsize[cpv]);
	for (int i = 0; i < c_len * notes_per_measure; i++) {
		mid[i] = i / notes_per_measure;
		mpos[i] = i % notes_per_measure;
	}
}

void CGenCP2::MakeNewCP2() {
	// Set pitch limits
	if (cantus_high) {
		for (int i = 0; i < vsize[cpv]; ++i) {
			max_cc[i] = acc[cfv][mid[i]] - min_between;
			min_cc[i] = acc[cfv][mid[i]] - max_between;
		}
	}
	else {
		for (int i = 0; i < vsize[cpv]; ++i) {
			min_cc[i] = acc[cfv][mid[i]] + min_between;
			max_cc[i] = acc[cfv][mid[i]] + max_between;
		}
	}
	// Convert limits to diatonic and recalibrate
	for (int i = 0; i < vsize[cpv]; ++i) {
		min_c[i] = CC_C(min_cc[i], tonic_cur, minor_cur);
		max_c[i] = CC_C(max_cc[i], tonic_cur, minor_cur);
		min_cc[i] = C_CC(min_c[i], tonic_cur, minor_cur);
		max_cc[i] = C_CC(max_c[i], tonic_cur, minor_cur);
	}
	if (random_seed) {
		RandCantus(ac[cpv], acc[cpv], 0, vsize[cpv]);
	}
	else {
		FillCantus(acc[cpv], 0, vsize[cpv], min_cc);
	}
}

void CGenCP2::SingleCP2Init() {
	// Copy cantus
	acc = scpoint;
	// Get diatonic steps from chromatic
	for (int v = 0; v < acc.size(); ++v) {
		for (int i = 0; i < vsize[v]; ++i) {
			ac[v][i] = CC_C(acc[v][i], tonic_cur, minor_cur);
		}
	}
	// Save value for future use;
	acc_old = acc;
	// Calculate limits
	if (cantus_high) {
		for (int i = 0; i < vsize[cpv]; ++i) {
			max_cc[i] = min(acc[cfv][mid[i]] - min_between, acc[cpv][i] + correct_range);
			min_cc[i] = max(cf_nmax - sum_interval, acc[cpv][i] - correct_range);
		}
	}
	else {
		for (int i = 0; i < vsize[cpv]; ++i) {
			min_cc[i] = max(acc[cfv][mid[i]] + min_between, acc[cpv][i] - correct_range);
			max_cc[i] = min(cf_nmin + sum_interval, acc[cpv][i] + correct_range);
		}
	}
	// Convert limits to diatonic and recalibrate
	for (int i = 0; i < vsize[cpv]; ++i) {
		min_c[i] = CC_C(min_cc[i], tonic_cur, minor_cur);
		max_c[i] = CC_C(max_cc[i], tonic_cur, minor_cur);
		min_cc[i] = C_CC(min_c[i], tonic_cur, minor_cur);
		max_cc[i] = C_CC(max_c[i], tonic_cur, minor_cur);
	}
	sp1 = 0;
	sp2 = vsize[cpv];
	ep1 = max(0, sp1 - 1);
	ep2 = vsize[cpv];
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
		for (int i = 0; i < vsize[cpv]; ++i) if (smatrix[i]) {
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
		// Add last note if this is last window
		// End of evaluation window
		if (method == mScan) {
			ep2 = GetMaxSmap() + 1;
			if (sp2 == smatrixc) ep2 = vsize[cpv];
			// Clear scan steps
			FillCantusMap(acc[cpv], smap, 0, smatrixc, min_cc);
			// Can skip flags - full scan must remove all flags
		}
		// For sliding windows algorithm evaluate whole melody
		if (method == mSWA) {
			ep2 = vsize[cpv];
			// Cannot skip flags - need them for penalty if cannot remove all flags
			skip_flags = 0;
			// Clear scan steps of current window
			FillCantusMap(acc[cpv], smap, sp1, sp2, min_cc);
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
	}
}

void CGenCP2::MultiCP2Init() {
	MakeNewCP2();
	sp1 = 0; // Start of search window
	sp2 = sp1 + s_len; // End of search window
	if (sp2 > vsize[cpv] - 1) sp2 = vsize[cpv] - 1;
	// Record window
	wid = 0;
	wpos1[wid] = sp1;
	wpos2[wid] = sp2;
	// Add last note if this is last window
	ep1 = max(0, sp1 - 1);
	ep2 = sp2; // End of evaluation window
	if (ep2 == vsize[cpv] - 1) ep2 = vsize[cpv];
	p = sp2 - 1; // Minimal position in array to cycle
}

void CGenCP2::ScanCP2Init() {
	// Get cantus size
	if (task != tGen) c_len = scpoint[0].size();
	InitMeasures();
	// Resize global vectors
	for (int i = 0; i < av_cnt; ++i) {
		ac[i].resize(vsize[i]); // cantus (diatonic)
		acc[i].resize(vsize[i]); // cantus (chromatic)
		anflags[i].resize(vsize[i], vector<int>(max_flags)); // Flags for each note
		anflagsc[i].resize(vsize[i]); // number of flags for each note
		acc_old[i].resize(vsize[i]); // Cantus diatonic saved for SWA
		apc[i].resize(vsize[i]);
		apcc[i].resize(vsize[i]);
		aleap[i].resize(vsize[i]);
		asmooth[i].resize(vsize[i]);
	}
	hm.resize(vsize[cpv]);
	hm2.resize(vsize[cpv]);
	for (int x = 0; x < vsize[cpv]; ++x) {
		hm[x].resize(3);
		hm2[x].resize(3);
	}
	fpenalty.resize(max_flags);
	wpos1.resize(vsize[cpv] / s_len + 1);
	wpos2.resize(vsize[cpv] / s_len + 1);
	ivl.resize(vsize[cpv]);
	ivlc.resize(vsize[cpv]);
	civl.resize(vsize[cpv]);
	civlc.resize(vsize[cpv]);
	tivl.resize(vsize[cpv]);
	motion.resize(vsize[cpv]);
	min_c.resize(vsize[cpv]);
	max_c.resize(vsize[cpv]);
	min_cc.resize(vsize[cpv]);
	max_cc.resize(vsize[cpv]);
	accepted4.resize(MAX_WIND); // number of accepted canti per window
	accepted5.resize(MAX_WIND); // number of canti with neede flags per window
	flags.resize(max_flags); // Flags for whole cantus
	fstat.resize(max_flags); // number of canti with each flag
	fcor.resize(max_flags, vector<long long>(max_flags)); // Flags correlation matrix
	seed_cycle = 0; // Number of cycles in case of random_seed
	reseed_count = 0;
	nstat.resize(MAX_NOTE);
	nstat2.resize(MAX_NOTE);
	nstat3.resize(MAX_NOTE);
	cycle = 0;
	wscans.resize(MAX_WIND); // number of full scans per window
	wcount = 1; // Number of windows created
	accepted = 0;
	accepted2 = 0;
	accepted3 = 0;
	// Can we skip flags?
	skip_flags = !calculate_blocking && !calculate_correlation && !calculate_stat;
	// Initialize fblock if calculation is needed
	if (calculate_blocking) {
		fblock = vector<vector<vector<long>>>(MAX_WIND, vector<vector<long>>(max_flags, vector<long>(max_flags)));
	}
	// Init best rejected results
	if (best_rejected) {
		rcycle = 0;
		accept_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		rpenalty_min = MAX_PENALTY;
	}
}

int CGenCP2::SendCP2() {
	CString st, info, rpst;
	int pos, cclen, v, mea;
	int real_len2 = real_len * notes_per_measure;
	Sleep(sleep_ms);
	for (int av = 0; av < av_cnt; ++av) {
		if (cpv) {
			v = svoice + av;
		}
		else {
			v = svoice + 1 - av;
		}
		// Copy cantus to output
		if (step + real_len2 >= t_allocated) ResizeVectors(t_allocated * 2);
		for (int x = 0; x < vsize[av]; ++x) {
			if (av == cfv) {
				mea = x;
				cclen = cc_len[mea] * notes_per_measure;
			}
			else {
				mea = mid[x];
				cclen = cc_len[mea];
			}
			pos = step + x * cclen;
			for (int i = 0; i < cclen; ++i) {
				int current_severity = -1;
				if (av == cpv) {
					// Set color
					color[pos + i][v] = Color(0, 100, 100, 100);
				}
				// Set nflag color
				note[pos + i][v] = acc[av][x];
				tonic[pos + i][v] = tonic_cur;
				minor[pos + i][v] = minor_cur;
				if (anflagsc[av][x] > 0) for (int f = 0; f < anflagsc[av][x]; ++f) {
					int fl = anflags[av][x][f];
					if (!i) {
						st = "+ ";
						if (!accept[fl]) st = "- ";
						comment[pos][v] += "\n" + st + FlagName[fl];
						if (show_severity) {
							st.Format(" [%d]", severity[fl]);
							comment[pos][v] += st;
						}
						if (FlagGComment[fl] != "") comment[pos][v] += ". " + FlagGComment[fl];
						if (FlagComment[fl] != "") comment[pos][v] += ". " + FlagComment[fl];
						comment[pos][v] += ". ";
					}
					// Do not show colors for base voice
					if (av == cpv) {
						// Set note color if this is maximum flag severity
						if (severity[fl] > current_severity) {
							current_severity = severity[fl];
							color[pos + i][v] = flag_color[severity[fl]];
						}
					}
				}
				// Add scan range
				if (!i) {
					nsr1[pos][v] = min_cc[x];
					nsr2[pos][v] = max_cc[x];
				}
				len[pos + i][v] = cclen;
				pause[pos + i][v] = 0;
				coff[pos + i][v] = i;
				if (x < vsize[av] / 2)	dyn[pos + i][v] = 60 + 40 * (pos + i - step) / real_len2 + 20 * rand2() / RAND_MAX;
				else dyn[pos + i][v] = 60 + 40 * (real_len2 - pos - i + step) / real_len2 + 20 * rand2() / RAND_MAX;
				// Assign source tempo if exists
				if (cc_tempo[mea]) {
					tempo[pos + i] = cc_tempo[mea];
				}
				// Generate tempo if no source
				else {
					if (pos + i == 0) {
						tempo[pos + i] = min_tempo + (float)(max_tempo - min_tempo) * (float)rand2() / (float)RAND_MAX;
					}
					else {
						tempo[pos + i] = tempo[pos + i - 1] + randbw(-1, 1);
						if (tempo[pos + i] > max_tempo) tempo[pos + i] = 2 * max_tempo - tempo[pos + i];
						if (tempo[pos + i] < min_tempo) tempo[pos + i] = 2 * min_tempo - tempo[pos + i];
					}
				}
			}
		}
		pos += cclen;
		// Create pause
		note[pos][v] = 0;
		len[pos][v] = 1;
		pause[pos][v] = 1;
		dyn[pos][v] = 0;
		tempo[pos] = tempo[pos - 1];
		coff[pos][v] = 0;
	}
	step = pos;
	++step;
	// Count additional variables
	CountOff(step - real_len2 - 1, step - 1);
	CountTime(step - real_len2 - 1, step - 1);
	UpdateNoteMinMax(step - real_len2 - 1, step - 1);
	UpdateTempoMinMax(step - real_len2 - 1, step - 1);
	++cantus_sent;
	if (task == tGen) {
		if (!shuffle) {
			Adapt(step - real_len2 - 1, step - 1);
		}
		// If  window-scan
		st.Format("#%d\nCantus: %s\nHarmonic difficulty: %.0f", cantus_sent, cantus_high ? "high" : "low", hdif);
		AddMelody(step - real_len2 - 1, step - 1, svoice + 1, st);
	}
	else if (task == tEval) {
		if (m_algo_id == 101) {
			// If RSWA
			st.Format("#%d\nCantus: %s\nHarmonic difficulty: %.0f", cantus_sent, cantus_high ? "high" : "low", hdif);
		}
		else {
			for (int x = 0; x < max_flags; ++x) {
				if (!accept[x] && fpenalty[x]) {
					st.Format("%d=%.0f", x, fpenalty[x]);
					if (rpst != "") rpst += ", ";
					rpst += st;
				}
			}
			st.Format("%.0f (", rpenalty_cur);
			rpst = st + rpst + ")";
			if (key_eval == "") {
				// If SWA
				st.Format("#%d (from MIDI file %s)\nCantus: %s\nRule penalty: %s\nDistance penalty: %.0f\nHarmonic difficulty: %.0f", cantus_id + 1, midi_file, cantus_high ? "high" : "low", rpst, dpenalty_cur, hdif);
			}
			else {
				// If evaluating
				st.Format("#%d (from MIDI file %s)\nCantus: %s\nRule penalty: %s\nHarmonic difficulty: %.0f\nKey selection: %s", cantus_id + 1, midi_file, cantus_high ? "high" : "low", rpst, hdif, key_eval);
			}
		}
		AddMelody(step - real_len2 - 1, step - 1, svoice + 1, st);
	}
	// Send
	t_generated = step;
	if (task == tGen) {
		if (!shuffle) {
			// Add line
			linecolor[t_sent] = Color(255, 0, 0, 0);
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

void CGenCP2::ReseedCP2()
{
	CString st;
	MultiCP2Init();
	// Allow two seed cycles for each accept
	seed_cycle = 0;
	++reseed_count;
	st.Format("Reseed: %d", reseed_count);
	SetStatusText(4, st);
}

void CGenCP2::ScanCP2(int t, int v) {
	CString st, st2;
	int finished = 0;
	// Load master parameters
	task = t;
	svoice = v;

	ScanCP2Init();
	if (task == tGen) MultiCP2Init();
	else SingleCP2Init();
	if (FailWindowsLimit()) return;
	// Need to clear flags, because if skip_flags, they can contain previous prohibited flags
	fill(flags.begin(), flags.end(), 0);
	// Analyze combination
check:
	while (true) {
		//LogCantus(acc[cpv]);
		GetMelodyInterval(acc[cpv], 0, ep2, nmin, nmax);
		// Limit melody interval
		if (task == tGen) {
			if (nmax - nmin > max_interval) goto skip;
			if (vsize[cpv] == ep2 && nmax - nmin < min_interval) goto skip;
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
			if (nmax - nmin > max_interval) FLAG(37, 0);
			if (cantus_high) {
				if (cf_nmax - nmin > sum_interval) FLAG(7, 0);
			}
			else {
				if (nmax - cf_nmin > sum_interval) FLAG(7, 0);
			}
			if (c_len == ep2 && nmax - nmin < min_interval) FLAG(38, 0);
		}
		//if (FailCPInterval(0, ep2)) goto skip;
		if (FailSlurs(acc[cpv], 0, ep2 - 1)) goto skip;
		++accepted3;
		if (need_exit && task != tEval) break;
		// Show status
		if (FailDiatonic(ac[cpv], acc[cpv], 0, ep2, minor_cur)) goto skip;
		GetPitchClass(ac[cpv], acc[cpv], apc[cpv], apcc[cpv], 0, ep2);
		if (minor_cur && FailMinor(apcc[cpv])) goto skip;
		//if (MatchVectors(acc[cpv], test_cc, 0, 2)) 
		//WriteLog(1, "Found");
		if (FailLastNotes(apc[cpv], ep2)) goto skip;
		if (FailNoteSeq(apc[cpv], 0, ep2)) goto skip;
		if (FailIntervals(ep2, ac[cpv], acc[cpv], apc[cpv], apcc[cpv])) goto skip;
		if (FailLeapSmooth(ac[cpv], acc[cpv], ep2, aleap[cpv], asmooth[cpv])) goto skip;
		if (FailOutstandingRepeat(ac[cpv], acc[cpv], aleap[cpv], ep2, repeat_steps2, 2, 76)) goto skip;
		if (FailOutstandingRepeat(ac[cpv], acc[cpv], aleap[cpv], ep2, repeat_steps3, 3, 36)) goto skip;
		if (FailLongRepeat(acc[cpv], aleap[cpv], ep2, repeat_steps5, 5, 72)) goto skip;
		if (FailLongRepeat(acc[cpv], aleap[cpv], ep2, repeat_steps7, 7, 73)) goto skip;
		// Calculate diatonic limits
		nmind = CC_C(nmin, tonic_cur, minor_cur);
		nmaxd = CC_C(nmax, tonic_cur, minor_cur);
		GlobalFill(ac[cpv], ep2, nstat2);
		//if (FailVIntervals()) goto skip;
		//if (FailOverlap()) goto skip;
		if (FailStagnation(acc[cpv], nstat, ep2)) goto skip;
		if (FailMultiCulm(acc[cpv], ep2)) goto skip;
		if (FailFirstNotes(apc[cpv], ep2)) goto skip;
		if (FailLeap(ac[cpv], ep2, aleap[cpv], asmooth[cpv], nstat2, nstat3)) goto skip;
		//if (FailMelodyHarm(apc[cpv], 0, ep2)) goto skip;

		SaveBestRejected();
		// If we are window-scanning
		if ((task == tGen || task == tCor) && method == mScan) {
			++accepted2;
			CalcFlagStat();
			if (FailFlagBlock()) goto skip;
			if (FailAccept()) goto skip;
			++accepted4[wid];
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
			CalcRpenalty();
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
			if (task == tGen && accept_reseed) {
				if (clib_vs.Insert(acc[cpv])) {
					if (SendCP2()) break;
					ReseedCP2();
					// Start evaluating without scan
					goto check;
				}
				else {
					++cantus_ignored;
				}
			}
			else {
				if (SendCP2()) break;
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
				// If we started from random seed, allow one more full cycle
				if (random_seed) {
					if (seed_cycle) {
						// Infinitely cycle through ranges
						if (random_range && accept_reseed) {
							ReseedCP();
							// Start evaluating without scan
							goto check;
						}
						break;
					}
					// Dont show log if we are reseeding after each accept
					if (!accept_reseed) WriteLog(3, "Random seed allows one more full cycle: restarting");
					++seed_cycle;
				}
				else break;
			}
			BackWindow(acc[cpv]);
			// Goto next variant calculation
			goto skip;
		} // if (finished)
		ScanRight(acc[cpv]);
	}
	if (accepted3 > 100000) ShowScanStatus(acc[cpv]);
	WriteFlagCor();
	ShowFlagStat();
	ShowFlagBlock();
}

void CGenCP2::Generate() {
	CString st;
	InitCP();
	LoadCantus(midi_file);
	if (cantus.size() < 1) return;
	// Choose cantus to use
	cantus_id = randbw(0, cantus.size() - 1);
	if (cantus_id2) {
		if (cantus_id2 < cantus.size()) {
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
	GetCantusKey(cantus[cantus_id]);
	// Get cantus interval
	GetMelodyInterval(cantus[cantus_id], 0, cantus[cantus_id].size(), cf_nmin, cf_nmax);
	if (tonic_cur == -1) return;
	CalcCcIncrement();
	// Show imported melody
	cc_len = cantus_len[cantus_id];
	cc_tempo = cantus_tempo[cantus_id];
	real_len = accumulate(cantus_len[cantus_id].begin(), cantus_len[cantus_id].end(), 0);
	dpenalty_cur = 0;
	// Create pause
	FillPause(0, real_len, 1);
	// Select rule set
	SelectRuleSet(cf_rule_set);
	ScanCantus(tEval, 0, &(cantus[cantus_id]));
	// Show cantus id
	st.Format("Cantus %d. ", cantus_id + 1);
	comment[0][0] = st + comment[0][0];
	// Go forward
	Adapt(0, real_len);
	t_generated = real_len;
	t_sent = t_generated;
	// Choose level
	if (cantus_high) {
		cpv = 0;
		cfv = 1;
	}
	else {
		cpv = 1;
		cfv = 0;
	}
	// Load first voice
	ac[cfv] = c;
	acc[cfv] = cc;
	apc[cfv] = pc;
	apcc[cfv] = pcc;
	// Generate second voice
	rpenalty_cur = MAX_PENALTY;
	SelectRuleSet(cp_rule_set);
	ScanCP2(tGen, 0);
}
