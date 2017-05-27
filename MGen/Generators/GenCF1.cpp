#include "../stdafx.h"
#include "GenCF1.h"

#ifdef _DEBUG
#define new DEBUG_NEW 
#endif

// Unskippable internal rules:
// Note repeats note of previous measure

CGenCF1::CGenCF1()
{
	av = 0;
	//midifile_tpq_mul = 8;
	accept.resize(MAX_FLAGS);
	FlagName.resize(MAX_FLAGS);
	accepts.resize(MAX_RULESETS);
	ssf.resize(MAX_FLAGS);
	severity.resize(MAX_FLAGS);
	flag_color.resize(MAX_SEVERITY);
	hvd.resize(7);
	hvs.resize(7);
	hvt.resize(7);
	hconst.resize(7);
	for (int i = 0; i < 7; ++i) hconst[i] = hUndefined;
	// Start severity
	severity[0] = 0;
}

CGenCF1::~CGenCF1()
{
}

// Load variants of possible harmonic meaning
void CGenCF1::LoadHarmVar(CString* sN, CString* sV)
{
	if (*sN == "harm_var") {
		++parameter_found;
		int pos = 0;
		CString st;
		for (int i = 0; i<1000; i++) {
			if (pos == -1) break;
			st = sV->Tokenize(",", pos);
			st.Trim();
			if (st.Find("D") >= 0) hvd[i] = 1;
			if (st.Find("S") >= 0) hvs[i] = 1;
			if (st.Find("T") >= 0) hvt[i] = 1;
		}
	}
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
			if (ast.size() != 7) {
				est.Format("Wrong line in rules file %s: '%s'", fname, st);
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
			if (rid >= MAX_FLAGS) {
				est.Format("Rule id (%d) is greater than MAX_FLAGS (%d). Consider increasing MAX_FLAGS", rid, MAX_FLAGS);
				WriteLog(1, est);
				return;
			}
			else {
				//est.Format("Found rule %s - %d", rule, rid);
				//WriteLog(1, est);
				FlagName[rid] = rule;
				if (accepts[set].size() < MAX_FLAGS) accepts[set].resize(MAX_FLAGS);
				accepts[set][rid] = flag;
				severity[rid] = sev;
			}
		}
	}
	fs.close();
	est.Format("LoadConfigFile loaded %d lines from %s", i, fname);
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
		for (int i = 0; i < MAX_FLAGS; ++i) {
			accept[i] = accepts[rule_set][i];
		}
		// Check that at least one rule is accepted
		for (int i = 0; i < MAX_FLAGS; ++i) {
			if (accept[i]) break;
			if (i == MAX_FLAGS - 1) WriteLog(1, "Warning: all rules are rejected (0) in configuration file");
		}
		// Calculate second level flags count
		flags_need2 = 0;
		for (int i = 0; i < MAX_FLAGS; ++i) {
			if (accept[i] == 2) ++flags_need2;
		}
		// Set rule colors
		for (int i = 0; i < MAX_SEVERITY; ++i) {
			flag_color[i] = Color(0, 255.0 / MAX_SEVERITY*i, 255 - 255.0 / MAX_SEVERITY*i, 0);
		}
	}
}

void CGenCF1::LoadConfigLine(CString* sN, CString* sV, int idata, float fdata)
{
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

	LoadHarmVar(sN, sV);
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
		if (c[i] == c[i + 1]) return 1;
	}
	return 0;
}

// Detect prohibited note sequences
int CGenCF1::FailNoteSeq(vector<int> &pc, int step1, int step2) {
	for (int i = step1; i < step2-2; ++i) {
		// Prohibit GC before cadence
		if (pc[i] == 4 && pc[i + 1] == 0) FLAG2(48, i);
		// Prohibit D-S
		if (hconst[pc[i]] == hDom && hconst[pc[i + 1]] == hSub) FLAG2(77, i);
	}
	return 0;
}

// Count limits
void CGenCF1::GetMelodyInterval(vector<int> &cc, int step1, int step2) {
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
		anflagsc[av][i] = 0;
	}
}

// Calculate pitch class
void CGenCF1::GetPitchClass(vector<int> &c, vector<int> &cc, vector<int> &pc, vector<int> &pcc, int step1, int step2) {
	for (int i = step1; i < step2; ++i) {
		pc[i] = c[i] % 7;
		pcc[i] = (cc[i] + 12 - tonic_cur) % 12;
	}
}

void CGenCF1::UpdateNoteHarm(int  i) {
	// We can't be T
	if (!hm2[i][hTon]) {
		if (i < ep2 - 1) {
			// Remove S
			hm2[i + 1][hSub] = 0;
		}
	}
	// We can be only D
	if (hm2[i][hDom] && !hm2[i][hSub] && !hm2[i][hTon]) {
		// Remove duplicates
		if (i>0) hm2[i - 1][hDom] = 0;
		if (i < ep2 - 1) {
			hm2[i + 1][hDom] = 0;
			// Remove S
			hm2[i + 1][hSub] = 0;
		}
	}
	// We can be only T
	if (hm2[i][hTon] && !hm2[i][hSub] && !hm2[i][hDom]) {
		// Remove duplicates
		if (i>0) hm2[i - 1][hTon] = 0;
		if (i < ep2 - 1) hm2[i + 1][hTon] = 0;
	}
	// We can be only S
	if (hm2[i][hSub] && !hm2[i][hTon] && !hm2[i][hDom]) {
		// Remove duplicates
		if (i > 0) {
			hm2[i - 1][hSub] = 0;
			// Remove D
			hm2[i - 1][hDom] = 0;
		}
		if (i < ep2 - 1) hm2[i + 1][hSub] = 0;
	}
}

int CGenCF1::FailMelodyHarm(vector<int> &pc, int ep1, int ep2) {
	// Calculate available harmonic meanings
	for (int i = 0; i < ep2; ++i) {
		if (hvd[pc[i]]) {
			hm[i][hDom] = 1;
			hm2[i][hDom] = 1;
		}
		else {
			hm[i][hDom] = 0;
			hm2[i][hDom] = 0;
		}
		if (hvs[pc[i]]) {
			hm[i][hSub] = 1;
			hm2[i][hSub] = 1;
		}
		else {
			hm[i][hSub] = 0;
			hm2[i][hSub] = 0;
		}
		if (hvt[pc[i]]) {
			hm[i][hTon] = 1;
			hm2[i][hTon] = 1;
		}
		else {
			hm[i][hTon] = 0;
			hm2[i][hTon] = 0;
		}
	}
	// Position of first tonic
	int first_tonic = 0;
	int last_tonic = 0;
	// Fix first tonic
	for (int i = 0; i < 4; ++i) {
		if (hm[i][hTon]) {
			first_tonic = i;
			// Zero other meanings
			hm2[i][hDom] = 0;
			hm2[i][hSub] = 0;
			break;
		}
	}
	// Fix dominant before tonic
	if (first_tonic && hm2[first_tonic - 1][hDom]) {
		hm2[first_tonic - 1][hTon] = 0;
		hm2[first_tonic - 1][hSub] = 0;
	}
	// Remove wrong harmony
	for (int i = 0; i < ep2; ++i) UpdateNoteHarm(i);
	for (int i = ep2 - 1; i >= 0; --i) UpdateNoteHarm(i);
	// Remove long repeats and limit misses
	int dcount = 0;
	int scount = 0;
	int tcount = 0;
	int wdcount = 0;
	int wscount = 0;
	int wtcount = 0;
	for (int i = 0; i < ep2; ++i) {
		if (FixNoteHarmRepeat(pc, i, hTon, tcount, wtcount)) return 1;
		if (FixNoteHarmRepeat(pc, i, hDom, dcount, wdcount)) return 1;
		if (FixNoteHarmRepeat(pc, i, hSub, scount, wscount)) return 1;
	}
	// Remove wrong harmony again
	for (int i = 0; i < ep2; ++i) UpdateNoteHarm(i);
	for (int i = ep2-1; i >= 0; --i) UpdateNoteHarm(i);
	// Detect conflicts
	for (int i = 0; i < ep2; ++i) {
		if (!hm2[i][hDom] && !hm2[i][hSub] && !hm2[i][hTon]) {
			FLAG2(77, i);
		}
	}
	// Detect long miss or repeat
	dcount = 0;
	scount = 0;
	tcount = 0;
	wdcount = 0;
	wscount = 0;
	wtcount = 0;
	for (int i = 0; i < ep2; ++i) {
		// Count same and missing letters in a row
		if (FailMelodyHarmMiss(pc, i, hTon, tcount, wtcount)) return 1;
		if (FailMelodyHarmMiss(pc, i, hDom, dcount, wdcount)) return 1;
		if (FailMelodyHarmMiss(pc, i, hSub, scount, wscount)) return 1;
	}
	// Check same letters
	if ((tcount == 3) || (dcount == 3) || (scount == 3)) FLAG2(15, ep2 - 1);
	if ((tcount == 4) || (dcount == 4) || (scount == 4)) FLAG2(16, ep2 - 1);
	if ((tcount > 4) || (dcount > 4) || (scount > 4)) FLAG2(17, ep2 - 1);
	// Check missing letters
	if ((wtcount == 4) || (wdcount == 4) || (wscount == 4)) FLAG2(18, ep2 - 1);
	if ((wtcount == 5) || (wdcount == 5) || (wscount == 5)) FLAG2(19, ep2 - 1);
	if ((wtcount > 5) || (wdcount > 5) || (wscount > 5)) FLAG2(20, ep2 - 1);
	// Calculate harmonic difficulty
	float proposed=0, approved=0, proposed_old, approved_old, proposed_total=0, approved_total=0;
	for (int i = 0; i < ep2; ++i) {
		// Assign variables of previous step
		proposed_old = proposed;
		approved_old = approved;
		// Calculate how many harmonies proposed and approved
		proposed = hm[i][hTon] + hm[i][hSub] + hm[i][hDom];
		approved = hm2[i][hTon] + hm2[i][hSub] + hm2[i][hDom];
		if (i > 0) {
			if (approved_old > 0.9) {
				// Decrease approved count
				if (hm2[i - 1][hDom] && hm2[i][hDom]) approved -= abs(1.0 / approved_old);
				if (hm2[i - 1][hTon] && hm2[i][hTon]) approved -= abs(1.0 / approved_old);
				if (hm2[i - 1][hSub] && hm2[i][hSub]) approved -= abs(1.0 / approved_old);
				if (hm2[i - 1][hDom] && hm2[i][hSub]) approved -= abs(1.0 / approved_old);
			}
		}
		//CString st;
		//st.Format("%d step %d Approved %f of %f", cantus_id, i, approved, proposed);
		//WriteLog(1, st);
		proposed_total += proposed;
		approved_total += approved;
	}
	hdif = (proposed_total - approved_total) / (proposed_total + 0.00001) * 100.0;
	if (debug_level > 1) {
		CString st;
		st.Format("%d Harmonic difficulty %.0f%%. Approved %f of %f", cantus_id, hdif, approved_total, proposed_total);
		WriteLog(1, st);
	}
	return 0;
}

int CGenCF1::FailMelodyHarmMiss(vector<int> &pc, int i, int harm, int &count, int &wcount) {
	if (hm2[i][harm]) {
		if (wcount == 4) FLAG2(18, i - 1);
		if (wcount == 5) FLAG2(19, i - 1);
		if (wcount > 5) FLAG2(20, i - 1);
		wcount = 0;
		++count;
	}
	else {
		++wcount;
		if (count == 3) FLAG2(15, i - 1);
		if (count == 4) FLAG2(16, i - 1);
		if (count > 4) FLAG2(17, i - 1);
		count = 0;
	}
	return 0;
}

int CGenCF1::FixNoteHarmRepeat(vector<int> &pc, int i, int harm, int &count, int &wcount) {
	if (hm2[i][harm]) {
		++count;
		// Try to resolve harmony repeat
		if (count > 2) {
			// Get other two harmonies
			int h1 = (harm + 1) % 3;
			int h2 = (harm + 2) % 3;
			// Find harmony that can be removed
			for (int x = i; x > i - count; --x) {
				// There must be an alternative harmony to remove
				if (hm2[x][h1] || hm2[x][h2]) {
					// Remove my harmony
					hm2[x][harm] = 0;
					// Now there are less repeats
					count = i - x;
					// Stop searching
					break;
				}
			}
		}
		// Try to limit harmony miss
		if (count == 1 && wcount > 3) {
			// Get other two harmonies
			int h1 = (harm + 1) % 3;
			int h2 = (harm + 2) % 3;
			// Remove alternative melodies
			hm2[i][h1] = 0;
			hm2[i][h2] = 0;
		}
		wcount = 0;
	}
	else {
		count = 0;
		wcount++;
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
			if (c[i] == c[i - 1]) return 1;
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
	if (ep2 > rlen*2) for (int i = 0; i < ep2 - rlen * 2; ++i) {
		// Check if note changes direction or is a leap
		if ((i == 0) || MELODY_SEPARATION(i) || MELODY_SEPARATION(i+rlen)) {
			// Search for repeat of note at same beat until last three notes
			int finish = i + scan_len;
			if (finish > ep2 - rlen) finish = ep2 - rlen;
			for (int x = i + 2; x < finish; x += 2) {
				// Check if same note with direction change or leap
				if ((cc[x] == cc[i]) && (MELODY_SEPARATION(x) || MELODY_SEPARATION(x+rlen))) {
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
	}
	return 0;
}

int CGenCF1::FailLongRepeat(vector<int> &cc, vector<int> &leap, int ep2, int scan_len, int rlen, int fid) {
	int ok;
	if (ep2 > rlen + 1) for (int i = 0; i < ep2 - rlen - 1; ++i) {
		// Search for repeat of note at same beat until last three notes
		int finish = i + scan_len;
		if (finish > ep2 - rlen + 1) finish = ep2 - rlen + 1;
		for (int x = i + rlen; x < finish; ++x) {
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
int CGenCF1::FailLeapSmooth(vector<int> &c, int ep2, vector<int> &leap, vector<int> &smooth) {
	// Clear variables
	int leap_sum = 0;
	int leap_sum2 = 0;
	int max_leap_sum = 0;
	int max_leap_sum2 = 0;
	int smooth_sum = 0;
	int smooth_sum2 = 0;
	int leap_sum_i = 0;
	int leap_sum_i2 = 0;
	for (int i = 0; i < ep2 - 1; ++i) {
		// Find all leaps
		leap[i] = 0;
		smooth[i] = 0;
		if (c[i + 1] - c[i] > 1) leap[i] = 1;
		else if (c[i + 1] - c[i] < -1) leap[i] = -1;
		// Find all smooth
		else if (c[i + 1] - c[i] == 1) smooth[i] = 1;
		else if (c[i + 1] - c[i] == -1) smooth[i] = -1;
	}
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
			leap_sum_i2 = i;
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
		if (smooth[i] != 0) ++smooth_sum;
		else smooth_sum = 0;
		if (smooth_sum >= max_smooth) FLAG2(4, i);
		if (i < ep2 - 2) {
			// Prohibit long smooth movement in one direction
			if (smooth[i] == smooth[i + 1]) ++smooth_sum2;
			else smooth_sum2 = 0;
			if (smooth_sum2 >= max_smooth_direct - 1) FLAG2(5, i);
			// Check if two notes repeat
			if ((i > 0) && (c[i] == c[i + 2]) && (c[i - 1] == c[i + 1])) FLAG2(9, i);
		}
	}
	if (max_leap_sum > max_leaps) {
		if (max_leap_sum > max_leaps2) FLAG2(25, leap_sum_i)
		else FLAG2(3, leap_sum_i);
	}
	if (max_leap_sum2 > cse_leaps) {
		if (max_leap_sum2 > cse_leaps2) FLAG2(71, leap_sum_i2)
		else FLAG2(70, leap_sum_i2);
	}
	return 0;
}

int CGenCF1::FailStagnation(vector<int> &cc, vector<int> &nstat, int ep2) {
	// Clear nstat
	for (int i = nmin; i <= nmax; ++i) nstat[i] = 0;
	// Prohibit stagnation
	for (int i = 0; i < ep2; ++i) {
		// Add new note to stagnation array
		++nstat[cc[i]];
		// Subtract old note
		if ((i >= stag_note_steps)) nstat[cc[i - stag_note_steps]]--;
		// Check if too many repeating notes
		if (nstat[cc[i]] > stag_notes) FLAG2(10, i);
	}
	return 0;
}

// Prohibit multiple culminations
int CGenCF1::FailMultiCulm(vector<int> &cc, int ep2) {
	int culm_sum = 0, culm_step;
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
	}
	return 0;
}

int CGenCF1::FailFirstNotes(vector<int> &pc, int ep2) {
	// Prohibit first note not tonic
	if (pc[0] != 0) {
		FLAG2(49, 0);
		// Calculate steps to search for tonic
		int fst = first_steps_tonic;
		if (c_len > 10) ++fst;
		// Prohibit tonic miss at start
		int c_pos = -1;
		int e_pos = -1;
		for (int i = 0; i < fst; ++i) {
			// Detect C note
			if (pc[i] == 0) c_pos = i;
			// Detect E note
			if (pc[i] == 2) e_pos = i;
		}
		int ok = 0;
		// No C ?
		if (c_pos == -1) FLAG2(40, 0)
		else {
			// If C found, check previous note
			if (c_pos > 0) {
				if (pc[c_pos - 1] != 6 || pc[c_pos - 1] != 1 || pc[c_pos - 1] == 4) ok = 1;
			}
			// If C is first note, it does not need to be prepared (actually, this cannot happen because of flag 49)
			else ok = 1;
		}
		// No E ?
		if (e_pos == -1) FLAG2(41, 0)
		else {
			// If E found, check previous note
			if (e_pos > 0) {
				if (pc[e_pos - 1] == 1 || pc[e_pos - 1] == 4) ok = 1;
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
	// Prohibit first note not tonic
	if (ep2 > c_len - 1)
		if (pc[c_len - 1] != 0) FLAG2(50, c_len - 1); 
	// Wrong second to last note
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

void CGenCF1::CountFill(vector<int> &c, int i, int pos1, int pos2, int leap_size, int leap_start, vector<int> &nstat2, vector<int> &nstat3, int &skips, int &skips2, int &ffinished, int pre)
{
	int leap_finish = i + 1;
	if (pos2 < pos1) pos2 = pos1;
	// Clear stat
	int n1 = min(c[leap_start], c[leap_finish]);
	int n2 = max(c[leap_start], c[leap_finish]);
	// Calculate finishing pitches
	int c3 = c[leap_finish]; // compensation start
	int c4 = c[leap_start]; // compensation finish
	if (c[leap_finish] < c[leap_start]) {
		++c3;
		--c4;
	}
	else {
		--c3;
		++c4;
	}
	int fill_finish = leap_finish;
	for (int x = n1; x <= n2; ++x) nstat3[x] = 0;
	// Fill all notes (even those outside pos1-pos2 window)
	if (pre) {
		for (int x = pos2; x >= pos1; --x) {
			++nstat3[c[x]];
			if (fill_finish >= leap_finish && (c[x] == c3 || c[x] == c[leap_finish])) {
				fill_finish = x;
			}
		}
	}
	else {
		for (int x = pos1; x <= pos2; ++x) {
			++nstat3[c[x]];
			if (fill_finish <= leap_finish && (c[x] == c4 || c[x] == c[leap_start])) {
				fill_finish = x;
			}
		}
	}
	// Local fill
	skips = 0; 
	// Add allowed skips
	if (leap_size > 2) --skips;
	if (leap_size > 6) --skips;
	// Global fill
	skips2 = skips;
	for (int x = n1 + 1; x < n2; ++x) if (!nstat3[x]) {
		++skips;
		if (!nstat2[x]) ++skips2;
	}
	// Check if fill does not deviate
	ffinished = 1;
	if (pre) {
		int pos3 = leap_finish - 2;
		if (pos3 < 0) pos3 = 0;
		if (pos3 > fill_finish) {
			int npoint = c[leap_start];
			if (c[leap_finish] > c[leap_start]) {
				// Get note point
				if (c[leap_start - 1] < npoint) npoint = c[leap_start - 1];
				// Detect deviation below note point
				for (int x = fill_finish+1; x <= pos3; ++x)
					if (c[x] < npoint) ffinished = 0;
			}
			else {
				// Get note point
				if (c[leap_start - 1] > npoint) npoint = c[leap_start - 1];
				// Detect deviation below note point
				for (int x = fill_finish + 1; x <= pos3; ++x)
					if (c[x] > npoint) ffinished = 0;
			}
		}
	}
	else {
		int pos3 = leap_finish + 2;
		if (pos3 > ep2) pos3 = ep2;
		if (pos3 < fill_finish) {
			int npoint = c[leap_finish];
			if (c[leap_finish] < c[leap_start]) {
				// Get note point
				if (c[leap_finish + 1] < npoint) npoint = c[leap_finish + 1];
				// Detect deviation below note point
				for (int x = pos3; x < fill_finish; ++x)
					if (c[x] < npoint) ffinished = 0;
			}
			else {
				// Get note point
				if (c[leap_finish + 1] > npoint) npoint = c[leap_finish + 1];
				// Detect deviation below note point
				for (int x = pos3; x < fill_finish; ++x)
					if (c[x] > npoint) ffinished = 0;
			}
		}
	}
	// Check if fill is finished
	if ((!nstat3[c3] && !nstat3[c[leap_finish]]) || (!nstat3[c4] && !nstat3[c[leap_start]])) ffinished = 0;
	// Add skip penalty for not starting/finishing globally
	if ((!nstat2[c3] && !nstat2[c[leap_finish]]) || (!nstat2[c4] && !nstat2[c[leap_start]])) skips2 += 10;
}

int CGenCF1::FailLeap(vector<int> &c, int ep2, vector<int> &leap, vector<int> &smooth, vector<int> &nstat2, vector<int> &nstat3)
{
	int preleap, leap_size, leap_start, leap_next, leap_prev, unresolved, prefilled, skips, skips2, pos, ffinished;
	for (int i = 0; i < ep2 - 1; ++i) {
		if (leap[i] != 0) {
			// Check if this leap is 3rd
			leap_size = abs(c[i + 1] - c[i]);
			leap_start = i;
			preleap = 0;
			prefilled = 0;
			leap_next = 0;
			leap_prev = 0;
			// Prev is leap?
			if (i > 0) leap_prev = leap[i] * leap[i - 1];
			// Check preleap (current leap does not exceed previous close leap)
			if ((i > 0) && ((c[i - 1] - c[i + 1])*leap[i] > 0)) preleap = 1;
			else if ((i > 1) && ((c[i - 2] - c[i + 1])*leap[i] > 0)) preleap = 1;
			else if ((i > 2) && ((c[i - 3] - c[i + 1])*leap[i] > 0)) preleap = 1;
			// Check if leap is third
			if (leap_size == 2) {
				// Check if leap is second third
				if (i > 0 && abs(c[i + 1] - c[i - 1]) == 4) {
					// Set leap start to first note of first third
					leap_start = i - 1;
					// Set leap size to be compound
					leap_size = 4;
					// If 6/8 goes before 2 thirds
					if ((i > 1) && ((leap[i] * (c[i - 1] - c[i - 2]) == -5) || (leap[i] * (c[i - 1] - c[i - 2]) == -7))) FLAG2(28, i)
						// Else mark simple 2x3rds
					else FLAG2(6, i);
				}
			}
			// Check if we have a greater neighbouring leap
			if ((i < ep2 - 2 && abs(c[i + 2] - c[i + 1]) > leap_size) ||
				(leap_start > 0 && abs(c[leap_start] - c[leap_start - 1]) > leap_size)) {
				// Set that we are preleaped (even if we are postleaped)
				preleap = 1;
			}
			if (i > 0) {
				// Check if  leap is prefilled
				pos = i - 2 - (leap_size - 1) * fill_steps_mul;
				if (pos < 0) pos = 0;
				CountFill(c, i, pos, i - 1, leap_size, leap_start, nstat2, nstat3, skips, skips2, ffinished, 1);
				// Do we have not too many skips?
				if (skips <= 0) {
					// Is fill finished or unfinished leaps allowed?
					if (ffinished || accept[33]) prefilled = 1;
				}
			}
			if (i < ep2 - 2) {
				// Check if  leap is filled
				pos = i + 2 + (leap_size - 1) * fill_steps_mul;
				// Do not check fill if search window is cut by end of current not-last scan window
				if ((pos < ep2) || (c_len == ep2)) {
					if (pos > ep2 - 1) pos = ep2 - 1;
					CountFill(c, i, i + 2, pos, leap_size, leap_start, nstat2, nstat3, skips, skips2, ffinished, 0);
					// Local not filled?
					if (skips > 0 || (!ffinished && !accept[33])) {
						// Local not filled. Prefilled?
						if (prefilled) {
							if (leap_size == 2) FLAG2(61, i)
							else if (leap_size == 3) FLAG2(62, i)
							else FLAG2(63, i);
						}
						// Local not filled. Not prefilled. Preleaped?
						else if (preleap) {
							if (leap_size == 2) FLAG2(35, i)
							else if (leap_size == 3) FLAG2(64, i)
							else FLAG2(65, i);
						}
						// Local not filled. Not prefilled. Not preleaped. Global filled?
						else if (skips2 <= 0) {
							if (leap_size == 2) FLAG2(66, i)
							else if (leap_size == 3) FLAG2(67, i)
							else FLAG2(11, i);
						}
						// Local not filled. Not prefilled. Not preleaped. Global not filled.
						else {
							if (leap_size == 2) FLAG2(68, i)
							else if (leap_size == 3) FLAG2(69, i)
							else FLAG2(24, i);
						}
					}
					// Flag unfinished fill if it is not blocking
					if (!ffinished && accept[33] > 0) FLAG2(33, i);
				}
			}
			// Check leap resolution if it is not last note
			if (i < ep2 - 2) {
				leap_next = leap[i] * leap[i + 1];
				// Next leap in same direction
				if (leap_next > 0) {
					// Flag if greater than two thirds
					if (abs(c[i + 2] - c[i]) > 4) FLAG2(27, i + 2);
					// Allow if both thirds, without flags (will process next cycle)
				}
				// Next leap back
				else if (leap_next < 0) {
					// Flag if back leap greater than 6th
					int leap_size2 = abs(c[i + 2] - c[i + 1]);
					if (leap_size2 > 5) FLAG2(22, i + 1)
						// Flag if back leap equal or smaller than 6th
					else FLAG2(8, i + 1);
					if (leap_size2 > leap_size) FLAG2(58, i + 1);
				}
				// Next linear in same direction
				else if (leap[i] * (c[i + 2] - c[i + 1]) > 0) {
					// Flag if 2nd after next back
					unresolved = 0;
					if (i < ep2 - 3) {
						// Check if melody direction does not change second note after leap
						if (leap[i] * (c[i + 3] - c[i + 2]) > 0) unresolved = 1;
						// If direction changes second note after leap
						else {
							// Check leap size
							if (leap_size > 4) FLAG2(29, i)
							else FLAG2(7, i);
						}
					}
					else {
						// Mark leap unresolved if this is end of cantus
						if (c_len == ep2) unresolved = 1;
					}
					if (unresolved) {
						if (leap_size == 2) {
							// Flag if third preleaped unresolved
							if (preleap) FLAG2(30, i)
								// Flag if third prefilled unresolved
							else if (prefilled) FLAG2(53, i)
								// Flag if third unresolved
							else FLAG2(54, i);
						}
						else if (leap_size == 3) {
							// Flag if 4th preleaped unresolved
							if (preleap) FLAG2(55, i)
								// Flag if 4th prefilled unresolved
							else if (prefilled) FLAG2(56, i)
								// Flag if 4th unresolved
							else FLAG2(57, i);
						}
						else {
							// Flag if >4th preleaped unresolved
							if (preleap) FLAG2(59, i)
								// Flag if >4th prefilled unresolved
							else if (prefilled) FLAG2(60, i)
								// Flag if >4th unresolved
							else FLAG2(26, i);
						}
					} // if unresolved
				}
				// Next linear back - no flag
			}
		}
	}
	// Prohibit last leap
	if (ep2 == c_len)
		if (leap[c_len - 2]) FLAG2(23, c_len - 1);
	return 0;
}

int CGenCF1::FailIntervals(int ep2, vector<int> &c, vector<int> &cc, vector<int> &pc)
{
	int leap_start;
	int found;
	for (int i = 0; i < ep2 - 1; ++i) {
		// Tritone prohibit
		leap_start = i;
		found = 0;
		// Check consecutive tritone
		if ((pc[i + 1] == 6 && pc[i] == 3) || (pc[i + 1] == 3 && pc[i] == 6)) found = 1;
		// Check tritone with additional note inside
		if (i > 0) {
			if ((pc[i + 1] == 6 && pc[i - 1] == 3) || (pc[i + 1] == 3 && pc[i - 1] == 6)) 
			  if ((c[i] > c[i+1] && c[i] < c[i-1]) || (c[i] < c[i+1] && c[i] > c[i-1])) {
					found = 1;
					leap_start = i - 1;
				}
		}
		if (found) {
			// Check if tritone is highest leap if this is last window
			if (ep2 == c_len)
				if ((c[leap_start] == nmax) || (c[i + 1] == nmax)) FLAG2(32, i);
				// Check if tritone is last step
				if (i > c_len - 3) FLAG2(31, i)
				// Check if resolution is correct
				else if (i < ep2 - 2) {
					if (pc[i + 1] == 3) FLAG2(31, i)
					else if (pc[i + 2] != 0) FLAG2(31, i)
					else if (!leap_start || pc[leap_start - 1] != 2) FLAG2(31, i)
					// Record resolved tritone
					else FLAG2(2, i);
				}
			// Do not check tritone if it is at the end of not-last window (after ep2 - 2)
		}
		// Sept prohibit
		if (abs(cc[i + 1] - cc[i]) == 10) FLAG2(1, i)
		else if (abs(cc[i + 1] - cc[i]) == 11) FLAG2(39, i);
	}
	return 0;
}

// Calculate global fill
void CGenCF1::GlobalFill(vector<int> &c, int ep2, vector<int> &nstat2)
{
	// Clear nstat
	for (int i = nmind; i <= nmaxd; ++i) nstat2[i] = 0;
	for (int x = 0; x < ep2; ++x) ++nstat2[c[x]];
}

void CGenCF1::ScanCantusInit() {
	// Get cantus size
	if (task != tGen) c_len = scantus->size();
	// Resize global vectors
	anflags.resize(av_cnt);
	anflagsc.resize(av_cnt);
	for (int i = 0; i < av_cnt; ++i) {
		anflags[i].resize(c_len, vector<int>(MAX_FLAGS)); // Flags for each note
		anflagsc[i].resize(c_len); // number of flags for each note
	}
	c.resize(c_len); // cantus (diatonic)
	cc.resize(c_len); // cantus (chromatic)
	fpenalty.resize(MAX_FLAGS);
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
	flags.resize(MAX_FLAGS); // Flags for whole cantus
	fstat.resize(MAX_FLAGS); // number of canti with each flag
	fcor.resize(MAX_FLAGS, vector<long long>(MAX_FLAGS)); // Flags correlation matrix
	seed_cycle = 0; // Number of cycles in case of random_seed
	reseed_count = 0;
	pc.resize(c_len);
	pcc.resize(c_len);
	leap.resize(c_len);
	smooth.resize(c_len);
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
		fblock = vector<vector<vector<long>>>(MAX_WIND, vector<vector<long>>(MAX_FLAGS, vector<long>(MAX_FLAGS)));
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
		minc = c[0] - max_intervald + 1;
		maxc = c[0] + max_intervald;
	}
	else {
		minc = c[0] - max_intervald;
		maxc = c[c_len - 1] + max_intervald;
	}
	if (random_range) {
		if (maxc - minc > max_intervald) {
			int rstart = randbw(0, maxc - minc - max_intervald);
			minc += rstart;
			maxc = minc + max_intervald;
		}
	}
}

// Calculate source melody range
void CGenCF1::GetSourceRange() {
	// Get source melody range
	GetMelodyInterval(cc, 0, c_len);
	// Convert range to diatonic
	src_nminc = CC_C(nmin, tonic_cur, minor_cur) - correct_inrange;
	src_nmaxc = CC_C(nmax, tonic_cur, minor_cur) + correct_inrange;
}

// Apply source melody range
void CGenCF1::ApplySourceRange() {
	if (!src_nmaxc) return;
	// Decrease current range if it is bigger
	if (minc < src_nminc) minc = src_nminc;
	if (maxc > src_nmaxc) maxc = src_nmaxc;
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
		// Check duplicate
		if (i > 0 && cc[i] == cc[i - 1]) return;
		// Check minor step repeated
		if (i > 0 && c[i] == c[i - 1]) return;
	}
	if (!swa_inrange) {
		GetRealRange(c, cc);
		ApplySourceRange();
	}
	// Set pitch limits
	// If too wide range is not accepted, correct range to increase scan performance
	if (!accept[37]) {
		for (int i = 0; i < c_len; ++i) {
			min_c[i] = max(minc, c[i] - correct_range);
			max_c[i] = min(maxc, c[i] + correct_range);
		}
	}
	else {
		for (int i = 0; i < c_len; ++i) {
			min_c[i] = c[i] - correct_range;
			max_c[i] = c[i] + correct_range;
		}
	}
	// Convert limits to chromatic
	for (int i = 0; i < c_len; ++i) {
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
		anflagsc[av][i] = 0;
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
		min_c[i] = minc;
		max_c[i] = maxc;
	}
	// Convert limits to chromatic
	for (int i = 0; i < c_len; ++i) {
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
		if (ep2 == c_len) for (int i = 0; i < MAX_FLAGS; ++i) {
			if (flags[i]) {
				++fstat[i];
				// Calculate correlation
				if (calculate_correlation) for (int z = 0; z < MAX_FLAGS; ++z) {
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
		for (int i = 0; i < MAX_FLAGS; ++i) {
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
		for (int i = 0; i < MAX_FLAGS; ++i) {
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
	for (int i = 1; i < MAX_FLAGS; ++i) {
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

void CGenCF1::CalcRpenalty() {
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
		if (anflagsc[av][x] > 0) for (int i = 0; i < anflagsc[av][x]; ++i) if (!accept[anflags[av][x][i]]) {
			rpenalty_cur += severity[anflags[av][x][i]];
		}
	}
	// Add flags penalty
	for (int x = 0; x < MAX_FLAGS; ++x) {
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
				anflags[av] = br_nf;
				anflagsc[av] = br_nfc;
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

int CGenCF1::NextSWA() {
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
		CalcRpenalty();
		// Add result only if there is penalty, it is low and there are not note repeats
		if (rpenalty_cur < rpenalty_min && rpenalty_cur) {
			br_cc = cc;
			br_f = flags;
			br_nf = anflags[av];
			br_nfc = anflagsc[av];
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

int CGenCF1::FailMinor() {
	for (int i = 1; i < ep2; ++i) {
		// Prohibit major second up before I (in last steps and other places)
		if (pcc[i] == 0 && pcc[i - 1] == 10) FLAG2(74, i);
		// Prohibit minor second up before VII - absorbed
		// Prohibit augmented second up before VII - absorbed
		// Prohibit unaltered VI or VII two steps from altered VI or VII
		if (pcc[i] == 11 || pcc[i] == 9) {
			if (pcc[i - 1] == 10 || pcc[i - 1] == 8) FLAG2(75, i);
			if (i > 1) if (pcc[i - 2] == 10 || pcc[i - 2] == 8) FLAG2(75, i);
			if (i < ep2 - 1) {
				if (pcc[i + 1] == 10 || pcc[i + 1] == 8) FLAG2(75, i);
				if (i < ep2 - 2)
					if (pcc[i + 2] == 10 || pcc[i + 2] == 8) FLAG2(75, i);
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
		for (int i = 0; i < MAX_FLAGS; ++i) {
			int f1 = i;
			st2.Format("%s; %d; ", FlagName[f1], fcor[f1][f1]);
			st3 += FlagName[f1] + "; ";
			for (int z = 0; z < MAX_FLAGS; ++z) {
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
		for (int i = 0; i < MAX_FLAGS; ++i) {
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
		for (int x = 0; x < MAX_FLAGS; ++x) {
			max_value = -1;
			// Find biggest value
			for (int i = 0; i < MAX_FLAGS; ++i) {
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
	for (int x = 0; x < MAX_FLAGS; ++x) {
		max_value = -1;
		// Find biggest value
		for (int i = 0; i < MAX_FLAGS; ++i) {
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
			for (int d = 1; d < MAX_FLAGS; ++d) {
				if (lines > 100) break;
				int flagc = 0;
				for (int x = 0; x < MAX_FLAGS; ++x) {
					if (fblock[w][d][x] > 0) ++flagc;
				}
				if (!flagc) continue;
				int max_flag = 0;
				long max_value = -1;
				st.Format("\nTIER %d: ", d);
				st2 += st;
				for (int x = 0; x < MAX_FLAGS; ++x) {
					max_value = -1;
					// Find biggest value
					for (int i = 0; i < MAX_FLAGS; ++i) {
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
			est.Format("Window %d: %lld scans, %lld of %lld variants blocked: %s", w, wscans[w], accepted5[w] - accepted4[w], accepted5[w], st2);
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
	if (svoice < 0) return 0;
	CString st, info;
	int v = svoice;
	Sleep(sleep_ms);
	// Copy cantus to output
	int pos = step;
	if (step + real_len >= t_allocated) ResizeVectors(t_allocated * 2);
	for (int x = 0; x < c_len; ++x) {
		if (hm2[x][hDom]) mark[pos][v] += "D";
		if (hm2[x][hTon]) mark[pos][v] += "T";
		if (hm2[x][hSub]) mark[pos][v] += "S";
		for (int i = 0; i < cc_len[x]; ++i) {
			// Set color
			color[pos + i][v] = Color(0, 100, 100, 100);
			int current_severity = -1;
			// Set nflag color
			note[pos + i][v] = cc[x];
			tonic[pos + i][v] = tonic_cur;
			minor[pos + i][v] = minor_cur;
			if (anflagsc[av][x] > 0) for (int f = 0; f < anflagsc[av][x]; ++f) {
				if (!i) {
					comment[pos][v] += FlagName[anflags[av][x][f]];
					if (show_severity) {
						st.Format(" [%d]", severity[anflags[av][x][f]]);
						comment[pos][v] += st;
					}
					comment[pos][v] += ". ";
				}
				// Set note color if this is maximum flag severity
				if (severity[anflags[av][x][f]] > current_severity) {
					current_severity = severity[anflags[av][x][f]];
					color[pos + i][v] = flag_color[severity[anflags[av][x][f]]];
				}
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
	step = pos;
	note[step][v] = 0;
	len[step][v] = 1;
	pause[step][v] = 1;
	dyn[step][v] = 0;
	tempo[step] = tempo[step - 1];
	coff[step][v] = 0;
	++step;
	// Count additional variables
	CountOff(step - real_len - 1, step - 1);
	CountTime(step - real_len - 1, step - 1);
	UpdateNoteMinMax(step - real_len - 1, step - 1);
	UpdateTempoMinMax(step - real_len - 1, step - 1);
	++cantus_sent;
	if (task == tGen) {
		if (!shuffle) {
			Adapt(step - real_len - 1, step - 1);
		}
		// If  window-scan
		st.Format("#%d\nHarmonic difficulty: %.0f", cantus_sent, hdif);
		AddMelody(step - real_len - 1, step - 1, v, st);
	}
	else if (task == tEval) {
		if (m_algo_id == 101) {
			// If RSWA
			st.Format("#%d\nHarmonic difficulty: %.0f", cantus_sent, hdif);
		}
		else {
			if (key_eval == "") {
				// If SWA
				st.Format("#%d\nRule penalty: %.0f\nDistance penalty: %.0f\nHarmonic difficulty: %.0f", cantus_id, rpenalty_cur, dpenalty_cur, hdif);
			}
			else {
				// If evaluating
				st.Format("#%d\nRule penalty: %.0f\nHarmonic difficulty: %.0f\nKey selection: %s", cantus_id, rpenalty_cur, hdif, key_eval);
			}
		}
		AddMelody(step - real_len - 1, step - 1, v, st);
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
	// Check that method is selected
	if (method == mUndefined) WriteLog(1, "Error: method not specified in algorithm configuration file");
	// Check harmonic meaning loaded
	if (hvd[0] + hvt[0] + hvs[0] == 0) {
		CString est;
		est.Format("Warning: no harmonic meaning (variants) in configuration file.");
		WriteLog(1, est);
	}
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
		if (rpenalty_min == 0) {
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
					for (int x = 0; x < MAX_FLAGS; ++x) {
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
		if (FailNoteRepeat(cc, ep1, ep2 - 1)) goto skip;
		GetMelodyInterval(cc, 0, ep2);
		++accepted3;
		// Limit melody interval
		if (task == tGen) {
			if (nmax - nmin > max_interval) goto skip;
			if (c_len == ep2 && nmax - nmin < min_interval) goto skip;
			ClearFlags(0, ep2);
		}
		else {
			ClearFlags(0, ep2);
			if (nmax - nmin > max_interval) FLAG(37, 0);
			if (c_len == ep2 && nmax - nmin < min_interval) FLAG(38, 0);
		}
		if (need_exit && task != tEval) break;
		// Show status
		if (accepted3 % 100000 == 0) ShowScanStatus(cc);
		// Calculate diatonic limits
		nmind = CC_C(nmin, tonic_cur, minor_cur);
		nmaxd = CC_C(nmax, tonic_cur, minor_cur);
		if (FailDiatonic(c, cc, 0, ep2, minor_cur)) goto skip;
		GetPitchClass(c, cc, pc, pcc, 0, ep2);
		if (minor_cur && FailMinor()) goto skip;
		//if (MatchVectors(cc, test_cc, 0, 2)) 
		//WriteLog(1, "Found");
		if (FailLastNotes(pc, ep2)) goto skip;
		if (FailNoteSeq(pc, 0, ep2)) goto skip;
		if (FailIntervals(ep2, c, cc, pc)) goto skip;
		if (FailLeapSmooth(c, ep2, leap, smooth)) goto skip;
		if (FailOutstandingRepeat(c, cc, leap, ep2, repeat_steps2, 2, 76)) goto skip;
		if (FailOutstandingRepeat(c, cc, leap, ep2, repeat_steps3, 3, 36)) goto skip;
		if (FailLongRepeat(cc, leap, ep2, repeat_steps5, 5, 72)) goto skip;
		if (FailLongRepeat(cc, leap, ep2, repeat_steps7, 7, 73)) goto skip;
		GlobalFill(c, ep2, nstat2);
		if (FailStagnation(cc, nstat, ep2)) goto skip;
		if (FailMultiCulm(cc, ep2)) goto skip;
		if (FailFirstNotes(pc, ep2)) goto skip;
		if (FailLeap(c, ep2, leap, smooth, nstat2, nstat3)) goto skip;
		if (FailMelodyHarm(pc, 0, ep2)) goto skip;
		//if (FailMelodyHarmSeq(pc, 0, ep2)) goto skip;
		//if (FailMelodyHarmSeq2(pc, 0, ep2)) goto skip;

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
			// Sliding Windows Approximation
			if (method == mSWA) {
				if (NextSWA()) break;
			}
			// Finish if this is last variant in first window and not SWA
			else if ((p == 1) || (wid == 0)) {
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
			// Clear flag to prevent coming here again
			finished = 0;
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
		int i1, i2;
		for (int i = 0; i < accepted; ++i) {
			for (int x = 0; x < c_len; ++x) {
				i1 = i*(c_len + 1) + x;
				i2 = ci[i]*(c_len + 1) + x;
				note2[i1] = note[i2][v];
				comment2[i1] = comment[i2][v];
				color2[i1] = color[i2][v];
			}
		}
		// Replace
		for (int i = 0; i < accepted; ++i) {
			for (int x = 0; x < c_len; ++x) {
				i1 = i*(c_len + 1) + x;
				note[i1][v] = note2[i1];
				comment[i1][v] = comment2[i1];
				color[i1][v] = color2[i1];
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
