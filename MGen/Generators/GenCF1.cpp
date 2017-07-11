#include "../stdafx.h"
#include "GenCF1.h"

#ifdef _DEBUG
#define new DEBUG_NEW 
#endif

// Unskippable internal rules:
// Note repeats note of previous measure

CGenCF1::CGenCF1()
{
	cpv = 0;
	//midifile_tpq_mul = 8;
	accept.resize(MAX_FLAGS);
	FlagName.resize(MAX_FLAGS);
	FlagComment.resize(MAX_FLAGS);
	FlagGComment.resize(MAX_FLAGS);
	ssf.resize(MAX_FLAGS);
	severity.resize(MAX_FLAGS);
	flag_color.resize(MAX_SEVERITY);
	accepts.resize(MAX_RULESETS);
	// Start severity
	severity[0] = 0;
}

CGenCF1::~CGenCF1()
{
}

// Load variants of possible harmonic meaning
void CGenCF1::LoadHarmVar()
{
	hv.resize(7);
	if (cantus_high) {
		// Create harmonic meaning variants for higher cantus
		for (int i = 0; i < 7; i++) {
			hv[i].clear();
			if (i != 1) hv[i].push_back((i + 5) % 7);
			hv[i].push_back((i + 3) % 7);
			hv[i].push_back(i);
		}
	}
	else {
		// Create harmonic meaning variants for lower cantus
		for (int i = 0; i < 7; i++) {
			hv[i].clear();
			hv[i].push_back((i + 5) % 7);
			if (i != 6) hv[i].push_back(i);
		}
	}
}

// Load harmonic sequence penalties
void CGenCF1::LoadHSP(CString fname)
{
	CString st, st2, st3, iname, est, rule;
	vector<CString> ast;
	int pos = 0;
	int i = 0;
	hsp.resize(7);
	ifstream fs;
	// Check file exists
	if (!fileExists(fname)) {
		CString est;
		est.Format("LoadHSP cannot find file: %s", fname);
		WriteLog(1, est);
		return;
	}
	fs.open(fname);
	char pch[2550];
	// Load header
	fs.getline(pch, 2550);
	while (fs.good()) {
		i++;
		// Get line
		fs.getline(pch, 2550);
		st = pch;
		st.Trim();
		pos = 0;
		if (st.Find(";") != -1) {
			Tokenize(st, ast, ";");
			if (ast.size() != 9) {
				est.Format("Wrong column count at line in hsp file %s: '%s'", fname, st);
				WriteLog(1, est);
				return;
			}
			if (i > 7) {
				est.Format("Wrong line count at line %d in hsp file %s: '%s'", i, fname, st);
				WriteLog(1, est);
				return;
			}
			hsp[i - 1].clear();
			for (int x = 0; x < 7; ++x) {
				hsp[i - 1].push_back(atoi(ast[x + 1]));
			}
		}
	}
	fs.close();
	est.Format("LoadHSP loaded %d lines from %s", i, fname);
	WriteLog(0, est);
}

// Load rules
void CGenCF1::LoadRules(CString fname)
{
	CString st, st2, st3, iname, est, rule;
	vector<CString> ast;
	int pos = 0;
	int i = 0;
	int sev = 0;
	int set = 0;
	int flag = 0;
	int rid;
	ifstream fs;
	// Check file exists
	if (!fileExists(fname)) {
		CString est;
		est.Format("LoadRules cannot find file: %s", fname);
		WriteLog(1, est);
		return;
	}
	fs.open(fname);
	char pch[2550];
	// Load header
	fs.getline(pch, 2550);
	while (fs.good()) {
		i++;
		// Get line
		fs.getline(pch, 2550);
		st = pch;
		st.Trim();
		pos = 0;
		if (st.Find(";") != -1) {
			Tokenize(st, ast, ";");
			if (ast.size() != 9) {
				est.Format("Wrong column count at line in rules file %s: '%s'", fname, st);
				WriteLog(1, est);
				return;
			}
			set = atoi(ast[0]);
			rid = atoi(ast[1]);
			sev = atoi(ast[2]);
			rule = ast[5];
			flag = atoi(ast[6]);
			// Find rule id
			//rid = distance(FlagName.begin(), find(FlagName.begin(), FlagName.end(), rule));
			if (rid >= max_flags) {
				max_flags = rid + 1;
				if (max_flags >= MAX_FLAGS) {
					est.Format("Rule id (%d) is equal or greater than MAX_FLAGS (%d). Consider increasing MAX_FLAGS", rid, MAX_FLAGS);
					WriteLog(1, est);
					return;
				}
			}
			//est.Format("Found rule %s - %d", rule, rid);
			//WriteLog(1, est);
			FlagName[rid] = rule;
			FlagGComment[rid] = ast[7];
			FlagComment[rid] = ast[8];
			if (accepts[set].size() < MAX_FLAGS) accepts[set].resize(MAX_FLAGS);
			accepts[set][rid] = flag;
			severity[rid] = sev;
		}
	}
	fs.close();
	est.Format("LoadRules loaded %d lines from %s", i, fname);
	WriteLog(0, est);
}

// Select rules
void CGenCF1::SelectRuleSet(int rs)
{
	rule_set = rs;
	if (!accepts[rule_set].size()) {
		CString est;
		est.Format("Cannot select rule set %d. It was not loaded from rules configuration file.", rule_set);
		WriteLog(1, est);
	}
	else {
		// Load rule set
		for (int i = 0; i < max_flags; ++i) {
			accept[i] = accepts[rule_set][i];
		}
		// Check that at least one rule is accepted
		for (int i = 0; i < max_flags; ++i) {
			if (accept[i]) break;
			if (i == max_flags - 1) WriteLog(1, "Warning: all rules are rejected (0) in configuration file");
		}
		// Calculate second level flags count
		flags_need2 = 0;
		for (int i = 0; i < max_flags; ++i) {
			if (accept[i] == 2) ++flags_need2;
		}
	}
}

void CGenCF1::LoadConfigLine(CString* sN, CString* sV, int idata, float fdata)
{
	CheckVar(sN, sV, "cantus_high", &cantus_high);
	CheckVar(sN, sV, "rpenalty_accepted", &rpenalty_accepted);
	LoadRange(sN, sV, "interval", &min_interval, &max_interval);
	CheckVar(sN, sV, "c_len", &c_len);
	CheckVar(sN, sV, "s_len", &s_len);
	LoadNote(sN, sV, "first_note", &first_note);
	LoadNote(sN, sV, "last_note", &last_note);
	CheckVar(sN, sV, "fill_steps_mul", &fill_steps_mul);
	CheckVar(sN, sV, "max_repeat_mul", &max_repeat_mul);
	CheckVar(sN, sV, "max_smooth_direct", &max_smooth_direct);
	CheckVar(sN, sV, "max_smooth", &max_smooth);
	CheckVar(sN, sV, "max_leaps", &max_leaps);
	CheckVar(sN, sV, "max_leaps2", &max_leaps2);
	CheckVar(sN, sV, "cse_leaps", &cse_leaps);
	CheckVar(sN, sV, "cse_leaps2", &cse_leaps2);
	CheckVar(sN, sV, "max_leap_steps", &max_leap_steps);
	CheckVar(sN, sV, "stag_notes", &stag_notes);
	CheckVar(sN, sV, "stag_note_steps", &stag_note_steps);
	LoadRange(sN, sV, "tempo", &min_tempo, &max_tempo);
	CheckVar(sN, sV, "random_choose", &random_choose);
	CheckVar(sN, sV, "random_key", &random_key);
	CheckVar(sN, sV, "random_seed", &random_seed);
	CheckVar(sN, sV, "random_range", &random_range);
	CheckVar(sN, sV, "accept_reseed", &accept_reseed);
	CheckVar(sN, sV, "repeat_steps2", &repeat_steps2);
	CheckVar(sN, sV, "repeat_steps3", &repeat_steps3);
	CheckVar(sN, sV, "repeat_steps5", &repeat_steps5);
	CheckVar(sN, sV, "repeat_steps7", &repeat_steps7);
	CheckVar(sN, sV, "shuffle", &shuffle);
	CheckVar(sN, sV, "first_steps_tonic", &first_steps_tonic);
	CheckVar(sN, sV, "show_severity", &show_severity);
	CheckVar(sN, sV, "calculate_correlation", &calculate_correlation);
	CheckVar(sN, sV, "calculate_stat", &calculate_stat);
	CheckVar(sN, sV, "calculate_ssf", &calculate_ssf);
	CheckVar(sN, sV, "best_rejected", &best_rejected);
	CheckVar(sN, sV, "calculate_blocking", &calculate_blocking);
	CheckVar(sN, sV, "late_require", &late_require);
	// Random SWA
	//CheckVar(sN, sV, "fullscan_max", &fullscan_max);
	CheckVar(sN, sV, "approximations", &approximations);
	CheckVar(sN, sV, "swa_steps", &swa_steps);
	CheckVar(sN, sV, "correct_range", &correct_range);
	CheckVar(sN, sV, "correct_inrange", &correct_inrange);
	CheckVar(sN, sV, "optimize_dpenalty", &optimize_dpenalty);
	CheckVar(sN, sV, "cf_rule_set", &cf_rule_set);
	CheckVar(sN, sV, "cp_rule_set", &cp_rule_set);
	CheckVar(sN, sV, "optimize_dpenalty", &optimize_dpenalty);

	// Load HSP
	if (*sN == "hsp_file") {
		++parameter_found;
		LoadHSP(GetLinkedPath(*sV, m_current_config));
	}
	// Load rules
	if (*sN == "rules_file") {
		++parameter_found;
		LoadRules(GetLinkedPath(*sV, m_current_config));
	}
	// Load method
	if (*sN == "method") {
		++parameter_found;
		if (*sV == "window-scan") method = mScan;
		else if (*sV == "swa") method = mSWA;
		else {
			CString est;
			est.Format("Warning: method name unrecognized: %s", *sV);
			WriteLog(1, est);
		}
	}
	// Load tonic
	if (*sN == "key") {
		++parameter_found;
		if (sV->Right(1) == "m") {
			*sV = sV->Left(sV->GetLength() - 1);
			minor_cur = 1;
		}
		tonic_cur = GetPC(*sV);
	}
	// Load rule set
	if (*sN == "rule_set") {
		++parameter_found;
		SelectRuleSet(atoi(*sV));
	}
}

void CGenCF1::LogCantus(vector<int> &c)
{
	CString st, st2;
	for (int i = 0; i < c.size(); ++i) {
		st.Format("%d ", c[i]);
		st2 += st;
	}
	CGLib::AppendLineToFile("log/temp.log", st2 + " \n");
}

// Step2 must be exclusive
void CGenCF1::FillCantus(vector<int>& c, int step1, int step2, int value)
{
	for (int i = step1; i < step2; ++i) {
		c[i] = value;
	}
}

// Step2 must be exclusive
void CGenCF1::FillCantus(vector<int>& c, int step1, int step2, vector<int> value)
{
	for (int i = step1; i < step2; ++i) {
		c[i] = value[i];
	}
}

// Step2 must be exclusive
void CGenCF1::RandCantus(vector<int>& c, vector<int>& cc, int step1, int step2)
{
	for (int i = step1; i < step2; ++i) {
		for (int x = 0; x < 1000; ++x) {
			c[i] = randbw(min_c[i], max_c[i]);
			// Convert to chromatic
			cc[i] = C_CC(c[i], tonic_cur, minor_cur);
			// Prevent note repeats in the starting cantus
			if ((i == 0 || c[i] != c[i - 1]) && (i > c_len - 2 || c[i] != c[i + 1])) break;
		}
	}
}

void CGenCF1::FillCantusMap(vector<int>& c, vector<int>& smap, int step1, int step2, vector<int>& value)
{
	// Step2 must be exclusive
	for (int i = step1; i < step2; ++i) {
		c[smap[i]] = value[smap[i]];
	}
}

// Detect repeating notes. Step2 excluding
int CGenCF1::FailNoteRepeat(vector<int> &c, int step1, int step2) {
	for (int i = step1; i < step2; ++i) {
		if (c[i] == c[i + 1]) FLAG2(93, i);
	}
	return 0;
}

// Detect prohibited note sequences
int CGenCF1::FailNoteSeq(vector<int> &pc) {
	for (int x = 0; x < fli_size-2; ++x) {
		s = fli[x];
		s1 = fli[x + 1];
		// Prohibit GC before cadence
		if (pc[s] == 4 && pc[s1] == 0) FLAG2(48, s);
		// Prohibit D-S
	}
	return 0;
}

// Count limits
void CGenCF1::GetMelodyInterval(vector<int> &cc, int step1, int step2, int &nmin, int &nmax) {
	// Calculate range
	nmin = MAX_NOTE;
	nmax = 0;
	for (int i = step1; i < step2; ++i) {
		if (cc[i] < nmin) nmin = cc[i];
		if (cc[i] > nmax) nmax = cc[i];
	}
}

// Clear flags
void CGenCF1::ClearFlags(int step1, int step2) {
	if (!skip_flags || flags_need2) {
		fill(flags.begin(), flags.end(), 0);
		fill(fpenalty.begin(), fpenalty.end(), 0);
	}
	flags[0] = 1;
	for (int i = step1; i < step2; ++i) {
		anflagsc[cpv][i] = 0;
	}
}

// Calculate pitch class
void CGenCF1::GetPitchClass(vector<int> &c, vector<int> &cc, vector<int> &pc, vector<int> &pcc, int step1, int step2) {
	for (int i = step1; i < step2; ++i) {
		pc[i] = c[i] % 7;
		pcc[i] = (cc[i] + 12 - tonic_cur) % 12;
	}
}

int CGenCF1::FailHarmStep(int i, const int* hv, int &count, int &wcount, int &last_flag, int &max_p) {
	if (hv[chm[i]]) {
		++count;
		wcount = 0;
	}
	else {
		++wcount;
		count = 0;
	}
	if (count == 3) FLAG3(15, i)
	else if (count == 4) FLAG3(16, i)
	else if (count > 4) FLAG3(17, i);
	if (wcount == 4) FLAG3(18, i)
	else if (wcount == 5) FLAG3(19, i)
	else if (wcount > 5) FLAG3(20, i);
	return 0;
}

int CGenCF1::EvalMelodyHarm(int p, int &last_flag, int &max_p) {
	int pen1, pen2;
	int dcount = 0;
	int scount = 0;
	int tcount = 0;
	int wdcount = 0;
	int wscount = 0;
	int wtcount = 0;
	for (int i = 0; i <= p; ++i) {
		// Check harmonic penalty
		if (i > 0) {
			pen1 = hsp[chm[i - 1]][chm[i]];
			if (pen1 == 3) FLAG3(99, i)
			else if (pen1 == 1) FLAG3(77, i)
			else if (pen1 == 2) {
				FLAG3(57, i);
				if (i < p) {
					pen2 = hsp[chm[i]][chm[i + 1]];
					if (pen2 == 2) FLAG3(92, i);
				}
			}
		}
		// Check harmony repeat and miss
		if (FailHarmStep(i, hvt, tcount, wtcount, last_flag, max_p)) return 1;
		if (FailHarmStep(i, hvd, dcount, wdcount, last_flag, max_p)) return 1;
		if (FailHarmStep(i, hvs, scount, wscount, last_flag, max_p)) return 1;
	}
	return 0;
}

int CGenCF1::FailMelodyHarm(vector<int> &pc) {
	CString st;
	int h;
	int first_tonic = 0;
	// Build hm vector
	for (int i = 0; i < ep2; ++i) {
		hm[i].clear();
		for (int x = 0; x < hv[pc[i]].size(); ++x) {
			h = hv[pc[i]][x];
			// Check tonic
			if (!h) {
				// Is this first or last tonic?
				if (!first_tonic || i == c_len - 1) {
					first_tonic = 1;
					// Set only tonic for this step
					hm[i].clear();
					hm[i].push_back(h);
					break;
				}
				// Is root tonic?
				else if (!pc[i] && !cantus_high) {
					// Is this prohibited?
					if (!accept[29]) continue;
				}
			}
			// If tonic allowed or not tonic
			hm[i].push_back(h);
		}
		// Shuffle
		if (task == tEval) {
			unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
			::shuffle(hm[i].begin(), hm[i].end(), default_random_engine(seed));
		}
	}
	// Scan vector
	chm.clear();
	chmp.clear();
	chm.resize(ep2, 0);
	chmp.resize(ep2, 0);
	for (int i = 0; i < ep2; ++i) chm[i] = hm[i][0];
	int p = 0;
	int finished = 0;
	int found = 0;
	int hcycle = 0;
	int last_flag = 0;
	int max_p = 0;
	//LogCantus(pc);
	while (true) {
	check:
		//st.Format("%d: ", p);
		//CGLib::AppendLineToFile("log/temp.log", st);
		//LogCantus(chmp);
		//LogCantus(chm);
		if (need_exit && task != tEval) return 1;
		if (!p) {
			++p;
			if (p > max_p) max_p = p;
			goto check;
		}
		if (EvalMelodyHarm(p, last_flag, max_p)) goto skip;
		// Success
		if (p == ep2-1) {
			found = 1;
			break;
		}
		else {
			++p;
			if (p > max_p) max_p = p;
			goto check;
		}
	skip:
		// ScanLeft
		while (true) {
			if (chmp[p] < hm[p].size()-1) break;
			// If current element is max, make it minimum
			chmp[p] = 0;
			// Get current value
			chm[p] = hm[p][chmp[p]];
			// Move left one element
			if (!p) {
				finished = 1;
				break;
			}
			p--;
		} // while (true)
		if (finished) {
			break;
		}
		// ScanRight
		// Increase rightmost element, which was not reset to minimum
		++chmp[p];
		// Get current value
		chm[p] = hm[p][chmp[p]];
		++hcycle;
	}
	// Detect possible variants
	if (!found) {
		if (max_p < ep2 - 1) {
			//fill(chm.begin(), chm.end(), -1);
		}
		// Increase penalty if flag was found at the beginning of melody
		fpenalty[last_flag] = ep2 - max_p;
		// Report one of last flags at highest position
		FLAG2(last_flag, max_p);
	}
	return 0;
}

void CGenCF1::CalcCcIncrement() {
	int pos;
	for (int i = 0; i < 127; ++i) {
		pos = (i + 13 - tonic_cur) % 12;
		if (minor_cur) {
			if (m_diatonic_full[pos]) cc_incr[i] = 1;
			else cc_incr[i] = 2;
		}
		else {
			if (diatonic[pos]) cc_incr[i] = 1;
			else cc_incr[i] = 2;
		}
	}
}

// Calculate chromatic positions
void CGenCF1::GetChromatic(vector<int> &c, vector<int> &cc, int step1, int step2, int minor_cur) {
	if (minor_cur) {
		for (int i = step1; i < step2; ++i) {
			cc[i] = m_C_CC(c[i], tonic_cur);
		}
	}
	else {
		for (int i = step1; i < step2; ++i) {
			cc[i] = maj_C_CC(c[i], tonic_cur);
		}
	}
}

// Calculate diatonic positions
int CGenCF1::FailDiatonic(vector<int> &c, vector<int> &cc, int step1, int step2, int minor_cur) {
	if (minor_cur) {
		// First note calculated separately because it does not need to be checked
		c[0] = m_CC_C(cc[0], tonic_cur);
		int step12 = step1 < 1 ? 1 : step1;
		for (int i = step12; i < step2; ++i) {
			c[i] = m_CC_C(cc[i], tonic_cur);
			// Check if diatonic step is the same
			if (c[i] == c[i - 1] && cc[i] != cc[i-1]) FLAG2(75, i);
		}
	}
	else {
		for (int i = step1; i < step2; ++i) {
			c[i] = maj_CC_C(cc[i], tonic_cur);
		}
	}
	return 0;
}

// Add minor alterations
void CGenCF1::AlterMinor(int ep2, vector<int> &cc) {
	int pcc;
	int pos1 = (tonic_cur + 10) % 12;
	int pos2 = (tonic_cur + 8) % 12;
	for (int i = ep2 - 2; i > 0; --i) {
		pcc = cc[i] % 12;
		if (pcc == pos1) {
			if (cc[i + 1] % 12 == tonic_cur) ++cc[i];
		}
		if (pcc == pos2) {
			if (cc[i + 1] % 12 == pos1+1) ++cc[i];
		}
	}
}

// Search for outstanding repeats
int CGenCF1::FailOutstandingRepeat(vector<int> &c, vector<int> &cc, vector<int> &leap, int ep2, int scan_len, int rlen, int fid) {
	int ok;
	if (ep2 > rlen*2) for (int i = 0; i < ep2 - rlen * 2; ++i) if (MELODY_SEPARATION(i)) {
		// Search for repeat of note at same beat until last three notes
		int finish = i + scan_len;
		if (finish > ep2 - rlen) finish = ep2 - rlen;
		for (int x = i + 2; x <= finish; x += 2) if (MELODY_SEPARATION(x)) {
			// Check if same note
			if (cc[x] == cc[i]) {
				// Check that more notes repeat
				ok = 0;
				for (int z = 1; z < rlen; ++z) {
					if (cc[x + z] != cc[i + z]) {
						ok = 1;
						break;
					}
				}
				if (!ok) {
					FLAG2(fid, i);
				}
			}
		}
	}
	return 0;
}

int CGenCF1::FailLongRepeat(vector<int> &cc, vector<int> &leap, int ep2, int scan_len, int rlen, int fid) {
	int ok;
	if (ep2 > rlen + 1) for (int i = 0; i < ep2 - rlen - 1; ++i) {
		// Search for repeat of note at same beat until last three notes
		int finish = i + scan_len;
		if (finish > ep2 - rlen) finish = ep2 - rlen;
		for (int x = i + rlen; x <= finish; ++x) {
			// Check if same note
			if (cc[x] == cc[i]) {
				// Check that more notes repeat
				ok = 0;
				for (int z = 1; z < rlen; ++z) {
					if (cc[x + z] != cc[i + z]) {
						ok = 1;
						break;
					}
				}
				if (!ok) {
					FLAG2(fid, i);
				}
			}
		}
	}
	return 0;
}

// Check if too many leaps
int CGenCF1::FailLeapSmooth(vector<int> &c, vector<int> &cc, int ep2, vector<int> &leap, vector<int> &smooth, vector<int> &slur) {
	// Clear variables
	int leap_sum = 0;
	int leap_sum2 = 0;
	int max_leap_sum = 0;
	int max_leap_sum2 = 0;
	int smooth_sum = 0;
	int smooth_sum2 = 0;
	int leap_sum_i = 0;
	int leap_sum_s2 = 0;
	for (int i = 0; i < ep2 - 1; ++i) {
		// Find all leaps
		leap[i] = 0;
		smooth[i] = 0;
		slur[i + 1] = 0;
		if (cc[i] == cc[i + 1]) slur[i + 1] = 1;
		if (c[i + 1] - c[i] > 1) leap[i] = 1;
		else if (c[i + 1] - c[i] < -1) leap[i] = -1;
		// Find all smooth
		else if (c[i + 1] - c[i] == 1) smooth[i] = 1;
		else if (c[i + 1] - c[i] == -1) smooth[i] = -1;
	}
	leap[ep2 - 1] = 0;
	smooth[ep2 - 1] = 0;
	slur[0] = 0;
	for (int i = 0; i < ep2 - 1; ++i) {
		// Add new leap
		if (leap[i] != 0) {
			++leap_sum;
			++leap_sum2;
		}
		else {
			leap_sum2 = 0;
		}
		// Subtract old leap
		if ((i >= max_leap_steps) && (leap[i - max_leap_steps] != 0)) leap_sum--;
		// Get maximum leap_sum
		if (leap_sum > max_leap_sum) {
			max_leap_sum = leap_sum;
			leap_sum_i = i;
		}
		if (leap_sum2 > max_leap_sum2) {
			max_leap_sum2 = leap_sum2;
			leap_sum_s2 = i;
		}
		// Calculate penalty
		if (leap_sum > max_leaps) {
			++fpenalty[3];
			if (leap_sum > max_leaps2) ++fpenalty[25];
		}
		if (leap_sum2 > cse_leaps) {
			++fpenalty[70];
			if (leap_sum2 > cse_leaps2) ++fpenalty[71];
		}
		// Prohibit long smooth movement
		if (smooth[i] != 0) {
			++smooth_sum;
			if (smooth_sum >= max_smooth) FLAG2(4, i);
		}
		else if (leap[i]) smooth_sum = 0;
		if (i < ep2 - 2) {
			// Prohibit long smooth movement in one direction
			if (smooth[i] == smooth[i + 1]) {
				++smooth_sum2;
				if (smooth_sum2 >= max_smooth_direct - 1) FLAG2(5, i);
			}
			else if (smooth[i] || leap[i]) smooth_sum2 = 0;
			// Check if two notes repeat
			if ((i > 0) && (cc[i] == cc[i + 2]) && (cc[i - 1] == cc[i + 1])) FLAG2(9, i);
		}
	}
	if (max_leap_sum > max_leaps) {
		if (max_leap_sum > max_leaps2) FLAG2(25, leap_sum_i)
		else FLAG2(3, leap_sum_i);
	}
	if (max_leap_sum2 > cse_leaps) {
		if (max_leap_sum2 > cse_leaps2) FLAG2(71, leap_sum_s2)
		else FLAG2(70, leap_sum_s2);
	}
	return 0;
}

int CGenCF1::FailStagnation(vector<int> &cc, vector<int> &nstat, int ep2) {
	// Clear nstat
	for (int i = nmin; i <= nmax; ++i) nstat[i] = 0;
	// Prohibit stagnation only for non-slurred notes
	++nstat[cc[0]];
	for (int i = 1; i < ep2; ++i) if (cc[i-1] != cc[i]) {
		// Add new note to stagnation array
		++nstat[cc[i]];
		// Subtract old note
		if ((i >= stag_note_steps)) --nstat[cc[i - stag_note_steps]];
		// Check if too many repeating notes
		if (nstat[cc[i]] > stag_notes) FLAG2(10, i);
	}
	return 0;
}

// Prohibit multiple culminations
int CGenCF1::FailMultiCulm(vector<int> &cc, int ep2) {
	int culm_sum = 0;
	if (ep2 < c_len) {
		// Find multiple culminations at highest note
		if (nmax == max_cc[0] || nmax - nmin == max_interval) {
			for (int i = 0; i < ep2; ++i) {
				if (cc[i] == nmax) {
					++culm_sum;
					culm_step = i;
					if (culm_sum > 1) FLAG2(12, culm_step);
				}
			}
		}
	}
	else {
		for (int i = 0; i < ep2; ++i) {
			if (cc[i] == nmax) {
				++culm_sum;
				culm_step = i;
				if (culm_sum > 1) FLAG2(12, culm_step);
			}
		}
		// Prohibit culminations at first steps
		if (culm_step < 2) FLAG2(78, culm_step);
		if (culm_step == 2) FLAG2(79, culm_step);
		// Prohibit culminations at last steps
		if (culm_step > c_len - 4) FLAG2(21, culm_step);
		// Prohibit synchronized culminations
		if (av_cnt > 1 && culm_step == cf_culm) FLAG2(26, culm_step);
	}
	return 0;
}

int CGenCF1::FailFirstNotes(vector<int> &pc, int ep2) {
	// Prohibit first note not tonic
	if (pc[0] != 0) {
		FLAG2(49, 0);
		if (pc[0] != 4) FLAG2(90, 0);
		// Calculate steps to search for tonic
		int fst = first_steps_tonic;
		if (c_len > 10) ++fst;
		// Do not search if cantus is short
		if (fli_size <= fst) return 0;
		// Prohibit tonic miss at start
		int c_pos = -1;
		int e_pos = -1;
		for (int i = 0; i < fst; ++i) {
			// Detect C note
			if (pc[fli[i]] == 0) c_pos = i;
			// Detect E note
			if (pc[fli[i]] == 2) e_pos = i;
		}
		int ok = 0;
		// No C ?
		if (c_pos == -1) FLAG2(40, 0)
		else {
			// If C found, check previous note
			if (c_pos > 0) {
				if (pc[fli[c_pos - 1]] != 6 || pc[fli[c_pos - 1]] != 1 || pc[fli[c_pos - 1]] == 4) ok = 1;
			}
			// If C is first note, it does not need to be prepared (actually, this cannot happen because of flag 49)
			else ok = 1;
		}
		// No E ?
		if (e_pos == -1) FLAG2(41, 0)
		else {
			// If E found, check previous note
			if (e_pos > 0) {
				if (pc[fli[e_pos - 1]] == 1 || pc[fli[e_pos - 1]] == 4) ok = 1;
			}
			// If E is first note, it does not need to be prepared
			else ok = 1;
		}
		// Is E or C prepared?
		if (!ok) FLAG2(52, 0);
	}
	return 0;
}

int CGenCF1::FailLastNotes(vector<int> &pc, int ep2) {
	// Prohibit last note not tonic
	if (ep2 > c_len - 1)
		if (pc[c_len - 1] != 0) FLAG2(50, c_len - 1); 
	// Wrong second to last note (last note never can be slurred)
	if (ep2 > c_len - 2) {
		if ((pc[c_len - 2] == 0) || (pc[c_len - 2] == 2) || (pc[c_len - 2] == 3) || (pc[c_len - 2] == 5)) FLAG2(13, c_len - 2);
		if (pc[c_len - 2] == 4) FLAG2(51, c_len - 2);
	}
	// Wrong third to last note
	if (ep2 > c_len - 3) {
		if ((pc[c_len - 3] == 0) || (pc[c_len - 3] == 2) || (pc[c_len - 3] == 4)) FLAG2(14, c_len - 3);
		// Leading third to last note
		if (pc[c_len - 3] == 6) FLAG2(34, c_len - 3);
	}
	return 0;
}

void CGenCF1::CreateLinks(vector<int> &cc) {
	int prev_note = -1;
	int lpos = 0;
	fli_size = 0;
	for (int i = 0; i < ep2; ++i) {
		if (prev_note != cc[i]) {
			prev_note = cc[i];
			fli[lpos] = i;
			++lpos;
		}
		bli[i] = lpos-1;
	}
	fli_size = lpos;
}

void CGenCF1::CountFillInit(vector<int> &c, int tail_len, int pre, int &t1, int &t2, int &fill_to, int &fill_from, int &fill_finish) {
	// Create leap tail
	tc.clear();
	int prev_note = -1;
	if (pre) {
		int pos1 = fleap_start - 1;
		int pos2 = max(fleap_start - tail_len, 0);
		if (c[leap_end] > c[leap_start]) {
			for (int i = pos1; i >= pos2; --i) {
				tc.push_back(128 - c[fli[i]]);
			}
			t1 = 128 - c[leap_end];
			t2 = 128 - c[leap_start];
		}
		else {
			for (int i = pos1; i >= pos2; --i) {
				tc.push_back(c[fli[i]]);
			}
			t1 = c[leap_end];
			t2 = c[leap_start];
		}
	}
	else {
		int pos1 = fleap_end + 1;
		int pos2 = min(fleap_end + tail_len, fli_size - 1);
		if (c[leap_end] > c[leap_start]) {
			for (int i = pos1; i <= pos2; ++i) {
				tc.push_back(c[fli[i]]);
			}
			t1 = c[leap_start];
			t2 = c[leap_end];
		}
		else {
			for (int i = pos1; i <= pos2; ++i) {
				tc.push_back(128 - c[fli[i]]);
			}
			t1 = 128 - c[leap_start];
			t2 = 128 - c[leap_end];
		}
	}
	for (int x = t1; x <= t2; ++x) nstat3[x] = 0;
	fill_finish = -1;
}
	
void CGenCF1::CountFill(vector<int> &c, int tail_len, vector<int> &nstat2, vector<int> &nstat3, int &skips, int &fill_to, int pre, int &fill_to_pre, int &fill_from, int &deviates, int &dev_count, int leap_prev, int &fill_finish)
{
	int t1, t2;
	int dev_state = 0;
	CountFillInit(c, tail_len, pre, t1, t2, fill_to, fill_from, fill_finish);
	// Detect fill_finish
	// Deviation state: 0 = before deviation, 1 = in deviation, 2 = after deviation, 3 = multiple deviations
	deviates = 0;
	for (int x = 0; x < tc.size(); ++x) {
		++nstat3[tc[x]];
		if (tc[x] <= t1 + 2) {
			// Set fill finish if we approach 3rd or lower first time
			if (fill_finish == -1) fill_finish = x;
			// Stop collecting note stat if we are below the border
			if (tc[x] < t1) break;
		}
	}
	// Calculate deviation
	for (int x = 0; x < fill_finish; ++x) {
		// If deviating, start deviation state and calculate maximum deviation
		if (tc[x] > t2) {
			deviates = max(deviates, tc[x] - t2);
			if (dev_state == 0) dev_state = 1;
			else if (dev_state == 2) dev_state = 3;
		}
		// If not deviating, stop deviation state
		else {
			if (dev_state == 1) dev_state = 2;
		}
	}
	// Get deviations count
	if (dev_state == 0) dev_count = 0;
	else if (dev_state < 3) dev_count = 1;
	else dev_count = 2;

	CountFillSkips(leap_prev, skips, t1, t2);
	CountFillLimits(c, pre, t1, t2, fill_to, fill_to_pre, fill_from);
}

void CGenCF1::CountFillSkips(int leap_prev, int &skips, int t1, int t2) {
	skips = 0;
	for (int x = t1 + 1; x < t2; ++x) if (!nstat3[x]) {
		++skips;
	}
}

void CGenCF1::CountFillLimits(vector<int> &c, int pre, int t1, int t2, int &fill_to, int &fill_to_pre, int &fill_from) {
	fill_to = leap_size;
	fill_to_pre = 0;
	fill_from = leap_size;
	// Search for first compensated note
	for (int i = t2-1; i >= t1; --i) {
		if (nstat3[i]) {
			fill_from = t2 - i;
			break;
		}
	}
	for (int i = t1; i <= t2; ++i) {
		if (nstat3[i]) {
			fill_to = i - t1;
			break;
		}
	}
	// Check prepared fill to 3rd
	if (!pre && fill_to > 1) {
		int pos = max(0, leap_start - 2);
		if (c[leap_start] < c[leap_end]) {
			for (int x = pos; x < leap_start; ++x) if (c[x] == c[leap_end] - 1) fill_to_pre = 1;
		}
		else {
			for (int x = pos; x < leap_start; ++x) if (c[x] == c[leap_end] + 1) fill_to_pre = 1;
		}
	}
}

void CGenCF1::FailLeapInit(int i, vector<int> &c, int last_max, int &last_leap, int &child_leap, int &presecond, int &leap_next, int &leap_prev, int &arpeg, int &overflow, vector<int> &leap) {
	child_leap = 0; // If we have a child_leap
	presecond = 0; // If leap has a filled second
	leap_next = 0; // Multiply consecutive leaps
	leap_prev = 0; // Multiply consecutive leaps
	overflow = 0; // Leap back overflow
	arpeg = 0; // Arpeggio 3+3
	// Check if this leap is 3rd
	leap_size = abs(c[i + 1] - c[i]);
	leap_start = i; // First step of leap
	leap_end = i + 1; // Last step of leap
	fleap_start = bli[leap_start];
	fleap_end = bli[leap_end];
	last_leap = 0;
										// Next is leap?
	if (i < ep2 - 2) leap_next = leap[i] * leap[i + 1];
	// Prev is leap?
	if (i > 0) leap_prev = leap[i] * leap[i - 1];
	// Last leap?
	if (i >= last_max) last_leap = 1;
	// Check if we have a greater neighbouring leap
	if ((i < ep2 - 2 && abs(c[i + 2] - c[i + 1]) > leap_size && leap[i] * leap[i + 1]<0) ||
		(leap_start > 0 && abs(c[leap_start] - c[leap_start - 1]) > leap_size && leap[i] * leap[i - 1]<0)) {
		// Set that we are preleaped (even if we are postleaped)
		child_leap = 1;
	}
}

int CGenCF1::FailLeapMulti(int leap_next, int &arpeg, int &overflow, int i, vector<int> &c, vector<int> &leap) {
	// Check if leap is third
	if (leap_size == 2) {
		// Check if leap is second third
		if (fleap_start > 0 && abs(c[leap_end] - c[fli[fleap_start-1]]) == 4) {
			// Set leap start to first note of first third
			leap_start = fli[fleap_start - 1];
			--fleap_start;
			// Set leap size to be compound
			leap_size = 4;
			// If 6/8 goes before 2 thirds (tight)
			if ((fleap_start > 0) && ((leap[leap_start] * (c[leap_start] - c[fli[fleap_start-1]]) == -5) || 
				(leap[leap_start] * (c[leap_start] - c[fli[fleap_start - 1]]) == -7))) FLAG2(28, i)
				// Else mark simple 2x3rds
			else FLAG2(6, i);
		}
	}			
	leap_id = min(leap_size - 2, 3);
	if (i < ep2 - 2) {
		// Next leap in same direction
		if (leap_next > 0) {
			// Flag if greater than two thirds
			if (abs(c[i + 2] - c[i]) > 4) FLAG2(27, i + 2)
				// Allow if both thirds, without flags (will process next cycle)
			else arpeg=1;
		}
		// Next leap back
		else if (leap_next < 0) {
			int leap_size2 = abs(c[i + 2] - c[i + 1]);
			// Flag if back leap greater than 6th
			if (leap_size2 > 5) FLAG2(22, i + 1)
				// Flag if back leap equal or smaller than 6th
			else FLAG2(8, i + 1);
			// Flag leap back overflow
			if (leap_size2 > leap_size) {
				FLAG2(58, i + 1);
				overflow = 1;
			}
		}
	}
	return 0;
}

int CGenCF1::FailLeap(vector<int> &c, int ep2, vector<int> &leap, vector<int> &smooth, vector<int> &nstat2, vector<int> &nstat3)
{
	// Get leap size, start, end
	// Check if leap is compensated (without violating compensation rules)
	// If leap is not compensated, check uncompensated rules
	// If uncompensated rules not allowed, flag compensation problems detected (3rd, etc.)
	int child_leap, leap_next, leap_prev, unresolved, presecond;
	int overflow, arpeg, last_leap;
	// Last leap border
	int last_max = c_len - 4;
	for (int i = 0; i < ep2 - 1; ++i) {
		if (leap[i] != 0) {
			FailLeapInit(i, c, last_max, last_leap, child_leap, presecond, leap_next, leap_prev, 
				arpeg, overflow, leap);
			if (FailLeapMulti(leap_next, arpeg, overflow, i, c, leap)) return 1;
			// If leap back overflow or arpeggio, do not check leap compensation, because compensating next leap will be enough
			if (!overflow && !arpeg)
			if (FailLeapFill(i, c, last_leap, leap_prev, child_leap)) return 1;
			if (FailLeapMDC(i, leap, c)) return 1;
		}
	}
	return 0;
}

int CGenCF1::FailLeapFill(int i, vector<int> &c, int last_leap, int leap_prev, int child_leap) {
	// Prefill parameters
	int ptail_len, pfill_to, pfill_to_pre, pfill_from, pdeviates, pfill_finish, pdev_count;
	// Fill parameters
	int tail_len, fill_to, fill_to_pre, fill_from, deviates, fill_finish, dev_count;
	int prefilled_last = 0;
	int pskips = 10;
	int skips = 10;
	// Calculate allowed skips if this is not second leap and skips for second leap not allowed
	int pallowed_skips = 0;
	int allowed_skips = 0;
	if (leap_prev >= 0 || accept[108 + leap_id]) {
		if (leap_size > 2) ++allowed_skips;
		if (leap_size > 6) ++allowed_skips;
	}
	// Check if  leap is filled
	tail_len = 2 + (leap_size - 1) * fill_steps_mul;
	// Do not check fill if search window is cut by end of current not-last scan window
	if ((fleap_end + tail_len < fli_size) || (c_len == ep2)) {
		filled = 1;
		// Check fill only if enough length (checked second time in case of slurs)
		CountFill(c, tail_len, nstat2, nstat3, skips, fill_to, 0, fill_to_pre,
			fill_from, deviates, dev_count, leap_prev, fill_finish);
		if (skips > allowed_skips) filled = 0;
		else if (fill_to > 2) filled = 0;
		else if (fill_to == 2 && fill_to_pre && !accept[100 + leap_id]) filled = 0;
		else if (fill_to == 2 && !fill_to_pre && !accept[104 + leap_id]) filled = 0;
		else if (fill_from > 2) filled = 0;
		else if (fill_from == 2 && !accept[53 + leap_id]) filled = 0;
		else if (deviates > 2) filled = 0;
		else if (dev_count > 1) filled = 0;
		else if (deviates == 1 && !accept[42 + leap_id]) filled = 0;
		else if (deviates == 2 && !accept[120 + leap_id]) filled = 0;
		if (!filled) {
			// Check if  leap is prefilled
			if (i > 0) {
				ptail_len = 2 + (leap_size - 1) * fill_steps_mul;
				CountFill(c, ptail_len, nstat2, nstat3, pskips, pfill_to, 1, pfill_to_pre, pfill_from,
					pdeviates, pdev_count, leap_prev, pfill_finish);
				prefilled = 1;
				if (pskips > 0) prefilled = 0;
				else if (pdeviates > 1) prefilled = 0;
			}
			if (prefilled && !last_leap) FLAG2(112 + leap_id, i)
			else if (prefilled && last_leap) FLAG2(144 + leap_id, i)
			else if (child_leap) FLAG2(116 + leap_id, i)
			else FLAG2(124 + leap_id, i);
		}
		// Show compensation flags only if successfully compensated
		// This means that compensation errors are not shown if uncompensated (successfully or not)
		else {
			// Flag unfinished fill if it is not blocking
			if (fill_to == 2 && fill_to_pre) FLAG2(100 + leap_id, i)
			// Flag prepared unfinished fill if it is not blocking
			else if (fill_to == 2 && !fill_to_pre) FLAG2(104 + leap_id, i)
			// Flag after 3rd if it is not blocking
			if (fill_from == 2) FLAG2(53 + leap_id, i);
			// Flag deviation if it is not blocking
			if (deviates == 1) FLAG2(42 + leap_id, i);
			// Flag deviation if it is not blocking
			if (deviates == 2) FLAG2(120 + leap_id, i);
		}
	}
	return 0;
}

int CGenCF1::FailLeapMDC(int i, vector<int> &leap, vector<int> &c) {
	// Melody direction change (MDC)
	// 0 = close, 1 = far, 2 = no
	// Default mdc is close, because beginning equals to close mdc
	mdc1 = 0;
	int prev_note = c[leap_start];
	for (int pos = leap_start - 1; pos >= 0; --pos) {
		if (c[pos] != prev_note) {
			// Check if direction changes or long without changes
			if (leap[i] * (c[pos] - prev_note) > 0 || mdc1 > 1) break;
			prev_note = c[pos];
			++mdc1;
		}
	}
	mdc2 = 0;
	prev_note = c[leap_end];
	for (int pos = leap_end + 1; pos < ep2; ++pos) {
		if (c[pos] != prev_note) {
			// Check if direction changes or long without changes
			if (leap[i] * (c[pos] - prev_note) < 0 || mdc2 > 1) break;
			prev_note = c[pos];
			++mdc2;
		}
	}
	// Close + 1far
	if (mdc1 == 0 && mdc2 == 1) FLAG2(128 + leap_id, i)
		// Close + 2far
	else if (mdc1 == 0 && mdc2 == 2) FLAG2(140 + leap_id, i)
		// No + close
	else if (mdc1 == 2 && mdc2 == 0) FLAG2(132 + leap_id, i)
		// Far + close
	else if (mdc1 == 1 && mdc2 == 0) FLAG2(59 + leap_id, i)
		// No close
	else if (mdc1*mdc2) {
		if (filled || prefilled) FLAG2(136 + leap_id, i)
		else FLAG2(148 + leap_id, i);
	}
		// No MDC after
	else if (mdc2 == 3) FLAG2(63 + leap_id, i);
	return 0;
}

// Check tritone t1-t2 which has to resolve from ta to tb
int CGenCF1::FailTritone(int ta, int t1, int t2, int tb, vector<int> &c, vector<int> &cc, vector<int> &pc, vector<int> &pcc) {
	int leap_start;
	int found;
	// Tritone prohibit
	leap_start = s;
	found = 0;
	// Check consecutive tritone
	if ((pcc[s1] == t2 && pcc[s] == t1) || (pcc[s1] == t1 && pcc[s] == t2)) found = 1;
	// Check tritone with additional note inside
	if (s > 0) {
		// Check pitches
		if ((pcc[s1] == t2 && pcc[s_1] == t1) || (pcc[s1] == t1 && pcc[s_1] == t2))
			// Check intermediate note and mdc
			if ((c[s] > c[s1] && c[s] < c[s_1] && (ls<2 || c[s_2] < c[s_1]) && (ls>fli_size - 3 || c[s2] > c[s1])) ||
				(c[s] < c[s1] && c[s] > c[s_1] && (ls<2 || c[s_2] > c[s_1]) && (ls>fli_size - 3 || c[s2] < c[s1]))) {
				found = 1;
				leap_start = s_1;
			}
	}
	if (found) {
		// Check if tritone is highest leap if this is last window
		if (ep2 == c_len)
			if ((cc[leap_start] == nmax) || (cc[s1] == nmax)) FLAG2(32, s);
		// Check if tritone is last step
		if (s > c_len - 3) FLAG2(31, s)
			// Check if resolution is correct
		else if (ls < fli_size - 2) {
			if (pcc[s1] == t1) FLAG2(31, s)
			else if (pcc[s2] != tb) FLAG2(31, s)
			else if (!leap_start || pcc[leap_start - 1] != ta) FLAG2(31, s)
				// Record resolved tritone
			else FLAG2(2, s);
		}
		// Do not check tritone if it is at the end of not-last window (after ep2 - 2)
	}
	return 0;
}

int CGenCF1::FailIntervals(vector<int> &c, vector<int> &cc, vector<int> &pc, vector<int> &pcc)
{
	for (ls = 0; ls < fli_size - 1; ++ls) {
		s = fli[ls];
		s1 = fli[ls + 1];
		if (ls > 0) s_1 = fli[ls - 1];
		if (ls > 1) s_2 = fli[ls - 2];
		if (ls < fli_size - 2) s2 = fli[ls + 2];
		// Warning: tritone F#C in minor is not detected (can add FailTritone to detect) because it is already prohibited by Unaltered near altered.
		// If you allow Unaltered near altered, you should implement FailTritone for F#C.
		if (FailTritone(4, 5, 11, 0, c, cc, pc, pcc)) return 1;
		if (minor_cur) {
			if (FailTritone(7, 8, 2, 3, c, cc, pc, pcc)) return 1;
		}
		// Sept prohibit
		if (abs(cc[s1] - cc[s]) == 10) FLAG2(1, s)
		else if (abs(cc[s1] - cc[s]) == 11) FLAG2(39, s);
	}
	return 0;
}

// Calculate global fill
int CGenCF1::FailGlobalFill(vector<int> &c, int ep2, vector<int> &nstat2)
{
	// Clear nstat
	for (int i = nmind; i <= nmaxd; ++i) nstat2[i] = 0;
	// Count nstat
	for (int x = 0; x < ep2; ++x) ++nstat2[c[x]];
	// Check nstat
	if (ep2 < c_len) return 0;
	int skips = 0;
	int skips2 = 0;
	for (int x = nmind + 1; x < nmaxd; ++x) if (!nstat2[x]) {
		if (!nstat2[x + 1]) {
			++skips2;
			++x;
		}
		else ++skips;
	}
	// Set flags
	if (skips2) FLAG2(69, 0);
	if (skips == 1) FLAG2(67, 0)
	else if (skips == 2) FLAG2(68, 0);
	return 0;
}

void CGenCF1::ScanCantusInit() {
	// Get cantus size
	if (task != tGen) c_len = scantus->size();
	// Resize global vectors
	anflags.resize(av_cnt);
	anflagsc.resize(av_cnt);
	for (int i = 0; i < av_cnt; ++i) {
		anflags[i].resize(c_len, vector<int>(max_flags)); // Flags for each note
		anflagsc[i].resize(c_len); // number of flags for each note
	}
	c.resize(c_len); // cantus (diatonic)
	cc.resize(c_len); // cantus (chromatic)
	fli.resize(c_len);
	bli.resize(c_len);
	fpenalty.resize(max_flags);
	cc_old.resize(c_len); // Cantus diatonic saved for SWA
	wpos1.resize(c_len / s_len + 1);
	wpos2.resize(c_len / s_len + 1);
	min_c.resize(c_len);
	max_c.resize(c_len);
	min_cc.resize(c_len);
	max_cc.resize(c_len);
	hm.resize(c_len);
	hm2.resize(c_len);
	for (int i = 0; i < c_len; ++i) {
		hm[i].resize(3);
		hm2[i].resize(3);
	}
	accepted4.resize(MAX_WIND); // number of accepted canti per window
	accepted5.resize(MAX_WIND); // number of canti with neede flags per window
	flags.resize(max_flags); // Flags for whole cantus
	fstat.resize(max_flags); // number of canti with each flag
	fcor.resize(max_flags, vector<long long>(max_flags)); // Flags correlation matrix
	seed_cycle = 0; // Number of cycles in case of random_seed
	reseed_count = 0;
	pc.resize(c_len);
	pcc.resize(c_len);
	leap.resize(c_len);
	smooth.resize(c_len);
	slur.resize(c_len);
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

// Get minimum element in SWA window
int CGenCF1::GetMinSmap() {
	int my_ep1 = ep2;
	for (int i = sp1; i < sp2; ++i) {
		if (my_ep1 > smap[i]) my_ep1 = smap[i];
	}
	return my_ep1;
}

// Get maximum element in SWA window
int CGenCF1::GetMaxSmap() {
	int my_ep2 = 0;
	for (int i = sp1; i < sp2; ++i) {
		if (my_ep2 < smap[i]) my_ep2 = smap[i];
	}
	return my_ep2;
}

// Calculate real possible range
void CGenCF1::GetRealRange(vector<int>& c, vector<int>& cc) {
	// Get diatonic interval
	max_intervald = CC_C(cc[0] + max_interval, tonic_cur, minor_cur) - c[0];
	min_intervald = CC_C(cc[0] + min_interval, tonic_cur, minor_cur) - c[0];
	// If max_interval is below octave, you cannot go to the lowest note to avoid multiple culminations flag
	if (max_interval < 12) {
		minc = cc[0] - max_interval + 2;
		maxc = cc[0] + max_interval;
	}
	else {
		minc = cc[0] - max_interval;
		maxc = cc[c_len - 1] + max_interval;
	}
	if (random_range) {
		if (maxc - minc > max_interval) {
			int rstart = randbw(0, maxc - minc - max_interval);
			minc += rstart;
			maxc = minc + max_interval;
		}
	}
}

// Calculate source melody range
void CGenCF1::GetSourceRange(vector<int> &cc) {
	// Get source melody range
	GetMelodyInterval(cc, 0, c_len, nmin, nmax);
	// Widen range
	src_nmin = nmin - correct_inrange;
	src_nmax = nmax + correct_inrange;
}

// Apply source melody range
void CGenCF1::ApplySourceRange() {
	if (!src_nmax) return;
	// Decrease current range if it is bigger
	if (minc < src_nmin) minc = src_nmin;
	if (maxc > src_nmax) maxc = src_nmax;
}

void CGenCF1::SingleCantusInit() {
	// Copy cantus
	cc = *scantus;
	// Get diatonic steps from chromatic
	first_note = cc[0];
	last_note = cc[c_len - 1];
	for (int i = 0; i < c_len; ++i) {
		c[i] = CC_C(cc[i], tonic_cur, minor_cur);
		// Save value for future use;
		cc_old[i] = cc[i];
	}
	if (!swa_inrange) {
		GetRealRange(c, cc);
		ApplySourceRange();
	}
	// Set pitch limits
	// If too wide range is not accepted, correct range to increase scan performance
	if (!accept[37]) {
		for (int i = 0; i < c_len; ++i) {
			min_cc[i] = max(minc, cc[i] - correct_range);
			max_cc[i] = min(maxc, cc[i] + correct_range);
		}
	}
	else {
		for (int i = 0; i < c_len; ++i) {
			min_cc[i] = cc[i] - correct_range;
			max_cc[i] = cc[i] + correct_range;
		}
	}
	// Convert limits to diatonic and recalibrate
	for (int i = 0; i < c_len; ++i) {
		min_c[i] = CC_C(min_cc[i], tonic_cur, minor_cur);
		max_c[i] = CC_C(max_cc[i], tonic_cur, minor_cur);
		min_cc[i] = C_CC(min_c[i], tonic_cur, minor_cur);
		max_cc[i] = C_CC(max_c[i], tonic_cur, minor_cur);
	}
	sp1 = 0;
	sp2 = c_len;
	ep1 = max(0, sp1 - 1);
	ep2 = c_len;
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
		for (int i = 0; i < c_len; ++i) if (smatrix[i]) {
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
		// Add last note if this is last window
		// End of evaluation window
		if (method == mScan) {
			ep2 = GetMaxSmap() + 1;
			if (sp2 == smatrixc) ep2 = c_len;
			// Clear scan steps
			FillCantusMap(cc, smap, 0, smatrixc, min_cc);
			// Can skip flags - full scan must remove all flags
		}
		// For sliding windows algorithm evaluate whole melody
		if (method == mSWA) {
			ep2 = c_len;
			// Cannot skip flags - need them for penalty if cannot remove all flags
			skip_flags = 0;
			// Clear scan steps of current window
			FillCantusMap(cc, smap, sp1, sp2, min_cc);
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

void CGenCF1::MakeNewCantus(vector<int> &c, vector<int> &cc) {
	// Set first and last notes
	c[0] = CC_C(first_note, tonic_cur, minor_cur);
	c[c_len - 1] = CC_C(last_note, tonic_cur, minor_cur);
	cc[0] = first_note;
	cc[c_len - 1] = last_note;
	GetRealRange(c, cc);
	// Set pitch limits
	for (int i = 0; i < c_len; ++i) {
		min_cc[i] = minc;
		max_cc[i] = maxc;
	}
	// Convert limits to diatonic and recalibrate
	for (int i = 0; i < c_len; ++i) {
		min_c[i] = CC_C(min_cc[i], tonic_cur, minor_cur);
		max_c[i] = CC_C(max_cc[i], tonic_cur, minor_cur);
		min_cc[i] = C_CC(min_c[i], tonic_cur, minor_cur);
		max_cc[i] = C_CC(max_c[i], tonic_cur, minor_cur);
	}
	if (random_seed) {
		RandCantus(c, cc, 0, c_len);
	}
	else {
		// Set middle notes to minimum
		FillCantus(cc, 0, c_len, min_cc);
	}
}

void CGenCF1::MultiCantusInit(vector<int> &c, vector<int> &cc) {
	MakeNewCantus(c, cc);
	sp1 = 0; // Start of search window
	sp2 = sp1 + s_len; // End of search window
	if (sp2 > c_len) sp2 = c_len;
	// Record window
	wid = 0;
	wpos1[wid] = sp1;
	wpos2[wid] = sp2;
	// Add last note if this is last window
	ep1 = max(0, sp1 - 1);
	ep2 = sp2; // End of evaluation window
	p = sp2 - 1; // Minimal position in array to cycle
}

// Calculate flag statistics
void CGenCF1::CalcFlagStat() {
	if (calculate_stat || calculate_correlation) {
		if (ep2 == c_len) for (int i = 0; i < max_flags; ++i) {
			if (flags[i]) {
				++fstat[i];
				// Calculate correlation
				if (calculate_correlation) for (int z = 0; z < max_flags; ++z) {
					if (flags[z]) ++fcor[i][z];
				}
			}
		}
	}
}

// Calculate flag blocking
int CGenCF1::FailFlagBlock() {
	if (calculate_blocking) {
		int flags_found = 0;
		int flags_found2 = 0;
		int flags_conflict = 0;
		// Find if any of accepted flags set
		for (int i = 0; i < max_flags; ++i) {
			if ((flags[i]) && (accept[i] > 0)) ++flags_found;
			if ((flags[i]) && (!accept[i])) ++flags_conflict;
			if ((flags[i]) && (accept[i] == 2)) ++flags_found2;
		}
		// Skip only if flags required
		if ((!late_require) || (ep2 == c_len)) {
			// Check if no needed flags set
			if (flags_found == 0) return 1;
			// Check if not enough 2 flags set
			if (flags_found2 < flags_need2) return 1;
		}
		++accepted5[wid];
		// Find flags that are blocking
		for (int i = 0; i < max_flags; ++i) {
			if ((flags[i]) && (!accept[i]))
				++fblock[wid][flags_conflict][i];
		}
	}
	return 0;
}

// Check if flags are accepted
int CGenCF1::FailAccept() {
	// Check prohibited strict flag only if not late_require or last window
	if ((!late_require) || (ep2 == c_len))
		if (flags[0] && !accept[0]) return 1;
	for (int i = 1; i < max_flags; ++i) {
		if (flags[i] && !accept[i]) return 1;
		if (!late_require || ep2 == c_len)
			if (!flags[i] && accept[i] == 2) 
				return 1;
	}
	return 0;
}

// Check if too many windows
int CGenCF1::FailWindowsLimit() {
	if (((c_len - 2) / (float)s_len > MAX_WIND && task == tGen) || (method == mScan && task == tCor && smatrixc / s_len > MAX_WIND)) {
		CString est;
		est.Format("Error: generating %d notes with search window %d requires more than %d windows. Change MAX_WIND to allow more.",
			c_len, s_len, MAX_WIND);
		WriteLog(1, est);
		return 1;
	}
	return 0;
}

void CGenCF1::NextWindow() {
	if (task == tCor) {
		sp1 = sp2;
		sp2 = sp1 + s_len; // End of search window
		if (sp2 > smatrixc) sp2 = smatrixc;
		// Reserve last window with maximum length
		if ((smatrixc - sp1 < s_len * 2) && (smatrixc - sp1 > s_len)) sp2 = (smatrixc + sp1) / 2;
		// Record window
		++wid;
		wpos1[wid] = sp1;
		wpos2[wid] = sp2;
		++wscans[wid];
		// Add last note if this is last window
		// End of evaluation window
		ep2 = GetMaxSmap() + 1;
		ep1 = max(0, GetMinSmap() - 1);
		if (sp2 == smatrixc) ep2 = c_len;
		// Minimal position in array to cycle
		pp = sp2 - 1;
		p = smap[pp];
	}
	else {
		sp1 = sp2;
		sp2 = sp1 + s_len; // End of search window
		if (sp2 > c_len) sp2 = c_len;
		// Reserve last window with maximum length
		if ((c_len - sp1 < s_len * 2) && (c_len - sp1 > s_len)) sp2 = (c_len + sp1) / 2;
		// Record window
		++wid;
		wpos1[wid] = sp1;
		wpos2[wid] = sp2;
		++wscans[wid];
		// End of evaluation window
		ep1 = max(0, sp1 - 1);
		ep2 = sp2;
		// Go to rightmost element
		p = sp2 - 1;
	}
	if (wcount < wid + 1) {
		wcount = wid + 1;
		if (ep2 == c_len) {
			// Show window statistics
			CString est;
			CString st, st2;
			for (int i = 0; i < wcount; ++i) {
				if (i > 0) st2 += ", ";
				st.Format("%d-%d", wpos1[i], wpos2[i]);
				st2 += st;
			}
			est.Format("Algorithm created %d windows: %s", wcount, st2);
			WriteLog(3, est);
		}
	}
	/*
	if (debug_level > 0) {
		CString st;
		st.Format("Next window with ep2 %d", ep2);
		WriteLog(3, st);
		SendCantus(0, 0);
	}
	*/
}

void CGenCF1::CalcRpenalty(vector<int> &cc) {
	// Calculate out of range penalty
	int real_range = nmax - nmin;
	if (!accept[37] && real_range > max_interval) {
		int nminr = nmin + (real_range - max_interval) / 2;
		int nmaxr = nminr + max_interval;
		for (int i = 0; i < ep2; ++i) {
			if (cc[i] < nminr) fpenalty[37] += nminr - cc[i];
			if (cc[i] > nmaxr) fpenalty[37] += cc[i] - nmaxr;
		}
	}
	// Calculate flags penalty
	rpenalty_cur = 0;
	for (int x = 0; x < ep2; ++x) {
		if (anflagsc[cpv][x] > 0) for (int i = 0; i < anflagsc[cpv][x]; ++i) if (!accept[anflags[cpv][x][i]]) {
			rpenalty_cur += severity[anflags[cpv][x][i]] + 1;
		}
	}
	// Add flags penalty
	for (int x = 0; x < max_flags; ++x) {
		if (!accept[x]) rpenalty_cur += fpenalty[x];
	}
}

void CGenCF1::ScanLeft(vector<int> &cc, int &finished) {
	while (true) {
		if (cc[p] < max_cc[p]) break;
		// If current element is max, make it minimum
		cc[p] = min_cc[p];
		// Move left one element
		if (task == tCor) {
			if (pp == sp1) {
				finished = 1;
				break;
			}
			pp--;
			p = smap[pp];
		}
		else {
			if (p == sp1) {
				finished = 1;
				break;
			}
			p--;
		}
	} // while (true)
}

void CGenCF1::BackWindow(vector<int> &cc) {
	// Show best rejected variant
	if (best_rejected) {
		milliseconds time = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		int rc = (time - accept_time).count() / best_rejected;
		if (debug_level > 2) {
			CString st;
			st.Format("Back window with rc %d", rc);
			WriteLog(3, st);
		}
		if (rc > rcycle) {
			rcycle = rc;
			if (br_cc.size() > 0) {
				vector<int> cc_saved = cc;
				cc = br_cc;
				flags = br_f;
				anflags[cpv] = br_nf;
				anflagsc[cpv] = br_nfc;
				SendCantus();
				cc = cc_saved;
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
	if (method != mScan) return;
	if (task == tCor) {
		// Clear current window
		FillCantusMap(cc, smap, sp1, sp2, min_cc);
		// If this is not first window, go to previous window
		if (wid > 0) wid--;
		sp1 = wpos1[wid];
		sp2 = wpos2[wid];
		// End of evaluation window
		ep2 = GetMaxSmap() + 1;
		ep1 = max(0, GetMinSmap() - 1);
		if (sp2 == smatrixc) ep2 = c_len;
		// Minimal position in array to cycle
		pp = sp2 - 1;
		p = smap[pp];
	}
	// Normal full scan
	else {
		// Clear current window
		// When random seeding, even back window movement should be randomized to avoid autorestart window cycle
		//if (random_seed) RandCantus(c, sp1, sp2);
		//else
		FillCantus(cc, sp1, sp2, min_cc);
		// If this is not first window, go to previous window
		if (wid > 0) wid--;
		sp1 = wpos1[wid];
		sp2 = wpos2[wid];
		// End of evaluation window
		ep1 = max(0, sp1 - 1);
		ep2 = sp2;
		// Add last note if this is last window
		if (ep2 == c_len - 1) ep2 = c_len;
		// Go to rightmost element
		p = sp2 - 1;
	}
}

int CGenCF1::NextSWA(vector<int> &cc, vector<int> &cc_old) {
	// If we slided to the end, break
	if (sp2 == smatrixc) return 1;
	// Slide window further
	++sp1;
	++sp2;
	ep1 = max(0, GetMinSmap() - 1);
	// Minimal position in array to cycle
	pp = sp2 - 1;
	p = smap[pp];
	// Restore previous step after sliding window
	cc[smap[sp1 - 1]] = cc_old[smap[sp1 - 1]];
	// Clear scan steps of current window
	FillCantusMap(cc, smap, sp1, sp2, min_cc);
	return 0;
}

void CGenCF1::SaveBestRejected() {
	// Save best rejected results if we can analyze full cantus
	if (best_rejected && ep2 == c_len) {
		CalcRpenalty(cc);
		// Add result only if there is penalty, it is low and there are not note repeats
		if (rpenalty_cur < rpenalty_min && rpenalty_cur) {
			br_cc = cc;
			br_f = flags;
			br_nf = anflags[cpv];
			br_nfc = anflagsc[cpv];
			rpenalty_min = rpenalty_cur;
			// Log
			if (debug_level > 1) {
				CString st;
				st.Format("Saving best rejected results with rpenalty %.0f", rpenalty_min);
				WriteLog(3, st);
			}
		}
	}
}

int CGenCF1::FailMinor(vector<int> &pcc) {
	for (int x = 1; x < fli_size; ++x) {
		s = fli[x];
		s_1 = fli[x - 1];
		// Prohibit major second up before I (in last steps and other places)
		if (pcc[s] == 0 && pcc[s_1] == 10) FLAG2(74, s_1);
		// Prohibit minor second up before VII - absorbed
		// Prohibit augmented second up before VII - absorbed
		// Prohibit unaltered VI or VII two steps from altered VI or VII
		if (pcc[s] == 11) {
			if (pcc[s_1] == 10) FLAG2(153, s_1);
			if (pcc[s_1] == 8) FLAG2(154, s_1);
			if (pcc[s_1] == 3) FLAG2(157, s_1);
			if (x > 1) {
				s_2 = fli[x - 2];
				if (pcc[s_2] == 10) FLAG2(159, s_2);
				if (pcc[s_2] == 8) FLAG2(160, s_2);
				if (pcc[s_2] == 3) FLAG2(163, s_2);
			}
			if (x < fli_size - 1) {
				s1 = fli[x+1];
				if (pcc[s1] == 10) FLAG2(153, s1);
				if (pcc[s1] == 8) FLAG2(154, s1);
				if (pcc[s1] == 3) FLAG2(156, s1);
				if (x < fli_size - 2) {
					s2 = fli[x+2];
					if (pcc[s2] == 10) FLAG2(159, s2);
					if (pcc[s2] == 8) FLAG2(160, s2);
					if (pcc[s2] == 3) FLAG2(162, s2);
				}
			}
		}
		if (pcc[s] == 9) {
			if (pcc[s_1] == 8) FLAG2(152, s_1);
			if (pcc[s_1] == 3) FLAG2(155, s_1);
			if (x > 1) {
				s_2 = fli[x - 2];
				if (pcc[s_2] == 8) FLAG2(158, s_2);
				if (pcc[s_2] == 3) FLAG2(161, s_2);
			}
			if (x < fli_size - 1) {
				s1 = fli[x + 1];
				if (pcc[s1] == 8) FLAG2(152, s1);
				if (pcc[s1] == 3) FLAG2(155, s1);
				if (x < fli_size - 2) {
					s2 = fli[x + 2];
					if (pcc[s2] == 8) FLAG2(158, s2);
					if (pcc[s2] == 3) FLAG2(161, s2);
				}
			}
		}
		// Prohibit unresolved minor tritone DG# (direct or with inserted note)
	}
	return 0;
}

void CGenCF1::ShowScanStatus(vector<int> &cc) {
	CString st;
	if (method == mScan) {
		if (task == tGen) {
			st.Format("Scan progress: %d of %d", cc[wpos1[0]] - min_cc[wpos1[0]],
				max_cc[wpos1[0]] - min_cc[wpos1[0]]);
			SetStatusText(2, st);
		}
		else {
			st.Format("Scan progress: %d of %d", cc[smap[0]] - min_cc[smap[0]],
				max_cc[smap[wpos1[0]]] - min_cc[smap[0]]);
			SetStatusText(2, st);
		}
	}
	else {
		st.Format("Scan progress: %d of %d", cc[sp1] - min_cc[sp1],
			max_cc[sp1] - min_cc[sp1]);
		SetStatusText(2, st);
	}
	if (clib.size() > 0) st.Format("Cycles: %lld (clib %d)", cycle, clib.size());
	else st.Format("Cycles: %lld", cycle);
	SetStatusText(5, st);
	st.Format("Window %d of %d (rp %.0f)", wid + 1, wcount, rpenalty_min);
	SetStatusText(1, st);
	st.Format("Sent: %ld (ignored %ld)", cantus_sent, cantus_ignored);
	SetStatusText(0, st);
}

void CGenCF1::ReseedCantus()
{
	CString st;
	MultiCantusInit(c, cc);
	// Allow two seed cycles for each accept
	seed_cycle = 0;
	++reseed_count;
	st.Format("Reseed: %d", reseed_count);
	SetStatusText(4, st);
}

void CGenCF1::WriteFlagCor() {
	// Write flag correlation
	if (calculate_correlation) {
		DeleteFile("cf1-cor.csv");
		CString st, st2, st3;
		st3 = "Flag; Total; ";
		for (int i = 0; i < max_flags; ++i) {
			int f1 = i;
			st2.Format("%s; %d; ", FlagName[f1], fcor[f1][f1]);
			st3 += FlagName[f1] + "; ";
			for (int z = 0; z < max_flags; ++z) {
				int f2 = i;
				st.Format("%lld; ", fcor[f1][f2]);
				st2 += st;
			}
			CGLib::AppendLineToFile("cf1-cor.csv", st2 + "\n");
		}
		CGLib::AppendLineToFile("cf1-cor.csv", st3 + "\n");
	}
}

void CGenCF1::ShowFlagStat() {
	CString st, st2;
	// Show flag statistics
	if (calculate_stat) {
		CString est;
		for (int i = 0; i < max_flags; ++i) {
			int f1 = i;
			st.Format("\n%lld %s ", fstat[f1], FlagName[f1]);
			st2 += st;
		}
		est.Format("%d/%d: Accepted %lld/%lld/%lld/%lld variants of %lld: %s",
			c_len, max_interval, accepted4[wcount - 1], accepted, accepted2,
			accepted3, cycle, st2);
		WriteLog(3, est);
	}
}

void CGenCF1::ShowStuck() {
	if (!ssf.size()) return;
	CString st, st2;
	// Show flag statistics
	if (calculate_ssf) {
		st2 = "SWA stuck flags: ";
		int max_flag = 0;
		long max_value = -1;
		for (int x = 0; x < max_flags; ++x) {
			max_value = -1;
			// Find biggest value
			for (int i = 0; i < max_flags; ++i) {
				if (ssf[i] > max_value) {
					max_value = ssf[i];
					max_flag = i;
				}
			}
			if (max_value < 1) break;
			st.Format("\n%ld %s, ", max_value, FlagName[max_flag]);
			st2 += st;
			// Clear biggest value to search for next
			ssf[max_flag] = -1;
		}
		WriteLog(3, st2);
	}
}

CString CGenCF1::GetStuck() {
	if (!best_flags.size()) return "";
	CString st, st2;
	int max_flag = 0;
	long max_value = -1;
	for (int x = 0; x < max_flags; ++x) {
		max_value = -1;
		// Find biggest value
		for (int i = 0; i < max_flags; ++i) {
			if (best_flags[i] > max_value) {
				max_value = best_flags[i];
				max_flag = i;
			}
		}
		if (max_value < 1) break;
		st.Format("\n%ld %s, ", max_value, FlagName[max_flag]);
		st2 += st;
		// Clear biggest value to search for next
		best_flags[max_flag] = -1;
	}
	return st2;
}

void CGenCF1::ShowFlagBlock() {
	CString st, st2;
	// Show blocking statistics
	if (calculate_blocking) {
		for (int w = 0; w < wcount; ++w) {
			int lines = 0;
			CString est;
			st2 = "";
			for (int d = 1; d < max_flags; ++d) {
				if (lines > 100) break;
				int flagc = 0;
				for (int x = 0; x < max_flags; ++x) {
					if (fblock[w][d][x] > 0) ++flagc;
				}
				if (!flagc) continue;
				int max_flag = 0;
				long max_value = -1;
				st.Format("\nTIER %d: ", d);
				st2 += st;
				for (int x = 0; x < max_flags; ++x) {
					max_value = -1;
					// Find biggest value
					for (int i = 0; i < max_flags; ++i) {
						if (fblock[w][d][i] > max_value) {
							max_value = fblock[w][d][i];
							max_flag = i;
						}
					}
					if (max_value < 1) break;
					st.Format("\n%ld %s, ", max_value, FlagName[max_flag]);
					st2 += st;
					++lines;
					// Clear biggest value to search for next
					fblock[w][d][max_flag] = -1;
				}
			}
			est.Format("Window %d: %lld scans, %lld of %lld variants blocked: %s", w, wscans[w], 
				accepted5[w] - accepted4[w], accepted5[w], st2);
			WriteLog(3, est);
		}
	}
}

void CGenCF1::CalcDpenalty() {
	dpenalty_cur = 0;
	for (int z = 0; z < c_len; z++) {
		int dif = abs(cc_old[z] - cc[z]);
		if (dif) dpenalty_cur += step_penalty + pitch_penalty * dif;
	}
}

void CGenCF1::SaveCantus() {
	// If rpenalty is same as min, calculate dpenalty
	if (optimize_dpenalty) {
		if (rpenalty_cur == rpenalty_min) {
			CalcDpenalty();
			// Do not save cantus if it has higher dpenalty
			if (dpenalty_cur > dpenalty_min) return;
			// Do not save cantus if it is same as source
			if (!dpenalty_cur) return;
			dpenalty_min = dpenalty_cur;
		}
		// If rpenalty lowered, clear dpenalty
		else {
			dpenalty_min = MAX_PENALTY;
			dpenalty_cur = MAX_PENALTY;
		}
		dpenalty.push_back(dpenalty_cur);
	}
	clib.push_back(cc);
	rpenalty.push_back(rpenalty_cur);
	rpenalty_min = rpenalty_cur;
}

int CGenCF1::SendCantus() {
	int step0 = step;
	// Save culmination position
	cf_culm = culm_step;
	if (svoice < 0) return 0;
	CString st, info, rpst;
	int v = svoice;
	Sleep(sleep_ms);
	// Copy cantus to output
	int pos = step;
	if (step + real_len >= t_allocated) ResizeVectors(t_allocated * 2);
	for (int x = 0; x < c_len; ++x) {
		if (chm[x] > -1) mark[pos][v] = HarmNames[chm[x]];
		mark_color[pos][v] = Color(120, 120, 120);
		for (int i = 0; i < cc_len[x]; ++i) {
			// Set color
			color[pos + i][v] = Color(0, 100, 100, 100);
			int current_severity = -1;
			// Set nflag color
			note[pos + i][v] = cc[x];
			tonic[pos + i][v] = tonic_cur;
			minor[pos + i][v] = minor_cur;
			if (anflagsc[cpv][x] > 0) for (int f = 0; f < anflagsc[cpv][x]; ++f) {
				int fl = anflags[cpv][x][f];
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
				// Set note color if this is maximum flag severity
				if (severity[fl] > current_severity) {
					current_severity = severity[fl];
					color[pos + i][v] = flag_color[severity[fl]];
				}
			}
			// Add scan range
			if (!i) {
				nsr1[pos][v] = min_cc[x];
				nsr2[pos][v] = max_cc[x];
			}
			len[pos + i][v] = cc_len[x];
			pause[pos + i][v] = 0;
			coff[pos + i][v] = i;
			if (x < real_len / 2)	dyn[pos + i][v] = 60 + 40 * (pos + i - step) / real_len + 20 * rand2() / RAND_MAX;
			else dyn[pos + i][v] = 60 + 40 * (real_len - pos - i + step) / real_len + 20 * rand2() / RAND_MAX;
			// Assign source tempo if exists
			if (cc_tempo[x]) {
				tempo[pos + i] = cc_tempo[x];
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
		pos += cc_len[x];
	}
	// Create pause
	int pause_len = floor((pos + 1) / 8 + 1) * 8 - pos;
	FillPause(pos, pause_len, v);
	for (int i = pos; i <= pos + pause_len; ++i) tempo[i] = tempo[i - 1];
	step = pos + pause_len;
	// Count additional variables
	CountOff(step0, step - 1);
	CountTime(step0, step - 1);
	UpdateNoteMinMax(step0, step - 1);
	UpdateTempoMinMax(step0, step - 1);
	++cantus_sent;
	// Create rule penalty string
	for (int x = 0; x < max_flags; ++x) {
		if (!accept[x] && fpenalty[x]) {
			st.Format("%d=%.0f", x, fpenalty[x]);
			if (rpst != "") rpst += ", ";
			rpst += st;
		}
	}
	st.Format("%.0f (", rpenalty_cur);
	rpst = st + rpst + ")";
	if (rpenalty_cur == MAX_PENALTY) rpst = "0";
	if (task == tGen) {
		if (!shuffle) {
			Adapt(step0, step - 1);
		}
		// If  window-scan
		st.Format("#%d\nRule penalty: %s", cantus_sent, rpst);
		AddMelody(step0, step - 1, v, st);
	}
	else if (task == tEval) {
		if (m_algo_id == 101) {
			// If RSWA
			//st.Format("#%d\nHarmonic difficulty: %.0f", cantus_sent, hdif);
		}
		else {
			if (key_eval == "") {
				// If SWA
				st.Format("#%d (from MIDI file %s)\nRule penalty: %s\nDistance penalty: %.0f", cantus_id+1, midi_file, rpst, dpenalty_cur);
			}
			else {
				// If evaluating
				st.Format("#%d (from MIDI file %s)\nRule penalty: %s\nKey selection: %s", cantus_id+1, midi_file, rpst, key_eval);
			}
		}
		AddMelody(step0, step - 1, v, st);
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

void CGenCF1::InitCantus()
{
	// Set rule colors
	for (int i = 0; i < MAX_SEVERITY; ++i) {
		flag_color[i] = Color(0, 255.0 / MAX_SEVERITY*i, 255 - 255.0 / MAX_SEVERITY*i, 0);
	}
	// Check that method is selected
	if (method == mUndefined) WriteLog(1, "Error: method not specified in algorithm configuration file");
	// Check harmonic meaning loaded
	LoadHarmVar();
}

void CGenCF1::TestDiatonic()
{
	CString st;
	tonic_cur = 2;
	minor_cur = 1;
	for (int i = 0; i < 32; ++i) {
		int d = CC_C(i, tonic_cur, minor_cur);
		int cc = C_CC(d, tonic_cur, minor_cur);
		st.Format("Test diatonic: %d [to d]-> %d [to c]-> %d", i, d, cc);
		WriteLog(1, st);
	}
}

// Create random cantus and optimize it using SWA
void CGenCF1::RandomSWA()
{
	CString st;
	VSet<int> vs; // Unique checker
	// Disable debug flags
	calculate_blocking = 0;
	calculate_correlation = 0;
	calculate_stat = 0;
	// Create single cantus
	cantus.resize(1);
	cantus[0].resize(c_len);
	ScanCantusInit();
	// Set random_seed to initiate random cantus
	random_seed = 1;
	// Set random_range to limit scanning to one of possible fast-scan ranges
	random_range = 1;
	// Prohibit limits recalculation during SWA
	swa_inrange = 1;
	for (int i = 0; i < INT_MAX; ++i) {
		if (need_exit) break;
		// Create random cantus
		MakeNewCantus(c, cc);
		// Convert cantus to chromatic
		for (int x = 0; x < c_len; ++x) {
			cantus[0][x] = C_CC(c[x], tonic_cur, minor_cur);
		}
		// Set scan matrix to scan all
		smatrixc = c_len - 2;
		smatrix.resize(c_len);
		smatrix[0] = 0;
		smatrix[c_len - 1] = 0;
		for (int x = 1; x < c_len - 1; ++x) {
			smatrix[x] = 1;
		}
		// Optimize cantus
		rpenalty_cur = MAX_PENALTY;
		SWA(0, 0);
		// Show cantus if it is perfect
		if (rpenalty_min <= rpenalty_accepted) {
			if (vs.Insert(cc)) {
				int step = t_generated;
				// Add line
				linecolor[t_generated] = Color(255, 0, 0, 0);
				ScanCantus(tEval, 0, &(cc));
				Adapt(step, t_generated - 1);
				t_sent = t_generated;
			}
			else {
				++cantus_ignored;
			}
		}
		st.Format("Random SWA: %d", i);
		SetStatusText(3, st);
		st.Format("Sent: %ld (ignored %ld)", cantus_sent, cantus_ignored);
		SetStatusText(0, st);
		//SendCantus(0, 0);
		// Check limit
		if (t_generated >= t_cnt) {
			WriteLog(3, "Reached t_cnt steps. Generation stopped");
			return;
		}
	}
	ShowStuck();
}

// Do not calculate dpenalty (dp = 0). Calculate dpenalty (dp = 1).
void CGenCF1::SWA(int i, int dp) {
	CString st;
	milliseconds time_start = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	s_len = 1;
	// Save source rpenalty
	float rpenalty_source = rpenalty_cur;
	long cnum;
	// Save cantus only if its penalty is less or equal to source rpenalty
	rpenalty_min = rpenalty_cur;
	dpenalty_min = MAX_PENALTY;
	cc = cantus[i];
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
		// Add current cantus if this is not first run
		if (a > 0) {
			clib.push_back(cc);
			clib_vs.Insert(cc);
			rpenalty.push_back(rpenalty_min_old);
			dpenalty.push_back(dpenalty_min_old);
		}
		// Sliding Windows Approximation
		ScanCantus(tCor, 0, &cc);
		dpenalty_min = MAX_PENALTY;
		cnum = clib.size();
		if (cnum) {
			if (dp) {
				// Count dpenalty for results, where rpenalty is minimal
				dpenalty.resize(cnum);
				for (int x = 0; x < cnum; x++) if (rpenalty[x] <= rpenalty_min) {
					dpenalty[x] = 0;
					for (int z = 0; z < c_len; z++) {
						int dif = abs(cantus[i][z] - clib[x][z]);
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
			for (int x = 0; x < cnum; x++) if (rpenalty[x] <= rpenalty_min && (!dp || dpenalty[x] == dpenalty_min)) {
				cids.push_back(x);
			}
			if (cids.size()) {
				// Get random cid
				int cid = randbw(0, cids.size() - 1);
				// Get random cantus to continue
				cc = clib[cids[cid]];
			}
		}
		// Send log
		if (debug_level > 1) {
			CString est;
			est.Format("SWA%d #%d: rp %.0f from %.0f, dp %.0f, cnum %ld", s_len, a, rpenalty_min, rpenalty_source, dpenalty_min, cnum);
			WriteLog(3, est);
		}
		if (cc.size() > 60) {
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

// Save accepted time if we are showing best rejected
void CGenCF1::TimeBestRejected() {
	if (best_rejected) {
		accept_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		rcycle = 0;
	}
}

void CGenCF1::SaveCantusIfRp() {
	// Is penalty not greater than minimum of all previous?
	if (rpenalty_cur <= rpenalty_min) {
		// If rpenalty 0, we can skip_flags (if allowed)
		if (!skip_flags && rpenalty_cur == 0)
			skip_flags = !calculate_blocking && !calculate_correlation && !calculate_stat;
		// Insert only if cc is unique
		if (clib_vs.Insert(cc))
			SaveCantus();
		// Save flags for SWA stuck flags
		if (rpenalty_cur) best_flags = flags;
	}
}

void CGenCF1::ScanCantus(int t, int v, vector<int>* pcantus) {
	CString st, st2;
	int finished = 0;
	// Load master parameters
	task = t;
	svoice = v;
	if (pcantus) scantus = pcantus;
	else scantus = 0;

	ScanCantusInit();
	if (task == tGen) MultiCantusInit(c, cc);
	else SingleCantusInit();
	if (FailWindowsLimit()) return;
	// Analyze combination
check:
	while (true) {
		//LogCantus(cc);
		ClearFlags(0, ep2);
		if (FailNoteRepeat(cc, ep1, ep2 - 1)) goto skip;
		GetMelodyInterval(cc, 0, ep2, nmin, nmax);
		++accepted3;
		// Limit melody interval
		if (nmax - nmin > max_interval) FLAG(37, 0);
		if (c_len == ep2 && nmax - nmin < min_interval) FLAG(38, 0);
		if (need_exit && task != tEval) break;
		// Show status
		if (accepted3 % 100000 == 0) ShowScanStatus(cc);
		// Calculate diatonic limits
		nmind = CC_C(nmin, tonic_cur, minor_cur);
		nmaxd = CC_C(nmax, tonic_cur, minor_cur);
		if (FailDiatonic(c, cc, 0, ep2, minor_cur)) goto skip;
		GetPitchClass(c, cc, pc, pcc, 0, ep2);
		CreateLinks(cc);
		if (minor_cur && FailMinor(pcc)) goto skip;
		//if (MatchVectors(cc, test_cc, 0, 2)) 
		//WriteLog(1, "Found");
		if (FailLastNotes(pc, ep2)) goto skip;
		if (FailNoteSeq(pc)) goto skip;
		if (FailIntervals(c, cc, pc, pcc)) goto skip;
		if (FailLeapSmooth(c, cc, ep2, leap, smooth, slur)) goto skip;
		if (FailOutstandingRepeat(c, cc, leap, ep2, repeat_steps2, 2, 76)) goto skip;
		if (FailOutstandingRepeat(c, cc, leap, ep2, repeat_steps3, 3, 36)) goto skip;
		if (FailLongRepeat(cc, leap, ep2, repeat_steps5, 5, 72)) goto skip;
		if (FailLongRepeat(cc, leap, ep2, repeat_steps7, 7, 73)) goto skip;
		if (FailGlobalFill(c, ep2, nstat2)) goto skip;
		if (FailStagnation(cc, nstat, ep2)) goto skip;
		if (FailMultiCulm(cc, ep2)) goto skip;
		if (FailFirstNotes(pc, ep2)) goto skip;
		if (FailLeap(c, ep2, leap, smooth, nstat2, nstat3)) goto skip;
		if (ep2>4 && FailMelodyHarm(pc)) goto skip;

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
			CalcRpenalty(cc);
		}
		// Accept cantus
		++accepted;
		TimeBestRejected();
		if (method == mScan && task == tCor) {
			SaveCantus();
		}
		else if (method == mSWA && task == tCor) {
			SaveCantusIfRp();
		}
		else {
			if (task == tGen && accept_reseed) {
				if (clib_vs.Insert(cc)) {
					if (SendCantus()) break;
					ReseedCantus();
					// Start evaluating without scan
					goto check;
				}
				else {
					++cantus_ignored;
				}
			}
			else {
				if (SendCantus()) break;
			}
			// Exit if this is evaluation
			if (task == tEval) return;
		}
	skip:
		ScanLeft(cc, finished);
		if (finished) {
			// Clear flag to prevent coming here again
			finished = 0;
			// Sliding Windows Approximation
			if (method == mSWA) {
				if (NextSWA(cc, cc_old)) break;
				goto check;
			}
			// Finish if this is last variant in first window and not SWA
			else if ((p == 0) || (wid == 0)) {
				// If we started from random seed, allow one more full cycle
				if (random_seed) {
					if (seed_cycle) {
						// Infinitely cycle through ranges
						if (random_range && accept_reseed) {
							ReseedCantus();
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
			BackWindow(cc);
			// Goto next variant calculation
			goto skip;
		} // if (finished)
		ScanRight(cc);
	}
	if (accepted3 > 100000) ShowScanStatus(cc);
	WriteFlagCor();
	ShowFlagStat();
	ShowFlagBlock();
}

void CGenCF1::ScanRight(vector<int> &cc) {
	// Increase rightmost element, which was not reset to minimum
	cc[p] += cc_incr[cc[p]];
	// Go to rightmost element
	if (task == tGen) {
		p = sp2 - 1;
	}
	else {
		pp = sp2 - 1;
		p = smap[pp];
	}
	++cycle;
}

void CGenCF1::Generate()
{
	test_cc.resize(10);
	test_cc[0] = 72;
	test_cc[1] = 74;
	test_cc[2] = 62;
	test_cc[3] = 64;
	test_cc[4] = 65;
	test_cc[5] = 69;
	test_cc[6] = 67;
	test_cc[7] = 69;
	test_cc[8] = 71;
	test_cc[9] = 72;
	// Voice
	int v = 0;
	//TestDiatonic();
	InitCantus();
	CalcCcIncrement();
	// Set uniform length of each cantus note
	cc_len.resize(c_len);
	cc_tempo.resize(c_len);
	real_len = c_len;
	for (int i = 0; i < c_len; ++i) cc_len[i] = 1;
	if (method == mSWA) {
		RandomSWA();
	}
	else {
		ScanCantus(tGen, 0, 0);
	}
	// Random shuffle
	if (shuffle) {
		vector<unsigned short> ci(accepted); // cantus indexes
		vector<unsigned char> note2(t_generated);
		vector<CString> comment2(t_generated);
		vector<Color> color2(t_generated);
		for (int i = 0; i < accepted; ++i) ci[i] = i;
		// Shuffled indexes
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		::shuffle(ci.begin(), ci.end(), default_random_engine(seed));
		// Swap
		int s1, s2;
		for (int i = 0; i < accepted; ++i) {
			for (int x = 0; x < c_len; ++x) {
				s1 = i*(c_len + 1) + x;
				s2 = ci[i]*(c_len + 1) + x;
				note2[s1] = note[s2][v];
				comment2[s1] = comment[s2][v];
				color2[s1] = color[s2][v];
			}
		}
		// Replace
		for (int i = 0; i < accepted; ++i) {
			for (int x = 0; x < c_len; ++x) {
				s1 = i*(c_len + 1) + x;
				note[s1][v] = note2[s1];
				comment[s1][v] = comment2[s1];
				color[s1][v] = color2[s1];
			}
		}
		// Adapt
		Adapt(0, t_generated-1);
		// Send
		t_sent = t_generated;
		::PostMessage(m_hWnd, WM_GEN_FINISH, 2, 0);
		CString est;
		est.Format("Shuffle of %lld melodies finished", accepted);
		WriteLog(3, est);
	}
}
