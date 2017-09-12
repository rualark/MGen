// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../stdafx.h"
#include "GenCF1.h"

#ifdef _DEBUG
#define new DEBUG_NEW 
#endif

CGenCF1::CGenCF1()
{
	av_cnt = 1;
	v_cnt = 1;
	ngraph_size = 3;
	cpv = 0;
	//midifile_tpq_mul = 8;
	accept.resize(MAX_RULES);
	RuleParam.resize(MAX_RULESETS);
	SubRuleName.resize(MAX_RULESETS);
	RuleName.resize(MAX_RULESETS);
	SubRuleComment.resize(MAX_RULESETS);
	RuleComment.resize(MAX_RULES);
	ssf.resize(MAX_RULES);
	severity.resize(MAX_RULES);
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
			hv[i].push_back((i + 5) % 7);
			if (i != 3) hv[i].push_back((i + 3) % 7);
			hv[i].push_back(i);
		}
	}
	else {
		// Create harmonic meaning variants for lower cantus
		for (int i = 0; i < 7; i++) {
			hv[i].clear();
			hv[i].push_back((i + 5) % 7);
			hv[i].push_back(i);
		}
	}
}

// Load harmonic sequence penalties
void CGenCF1::LoadHSP(CString fname)
{
	CString st, est;
	vector<CString> ast;
	int i = 0;
	hsp.resize(7);
	ifstream fs;
	// Check file exists
	if (!fileExists(fname)) {
		est.Format("LoadHSP cannot find file: %s", fname);
		WriteLog(5, est);
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
		if (st.Find(";") != -1) {
			Tokenize(st, ast, ";");
			if (ast.size() != 9) {
				est.Format("Wrong column count at line in hsp file %s: '%s'", fname, st);
				WriteLog(5, est);
				return;
			}
			if (i > 7) {
				est.Format("Wrong line count at line %d in hsp file %s: '%s'", i, fname, st);
				WriteLog(5, est);
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
	CString st, est, rule, subrule;
	vector<CString> ast;
	int i = 0;
	int sev = 0;
	int set = 0;
	int flag = 0;
	int rid;
	ifstream fs;
	// Check file exists
	if (!fileExists(fname)) {
		est.Format("LoadRules cannot find file: %s", fname);
		WriteLog(5, est);
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
		if (st.Find(";") != -1) {
			Tokenize(st, ast, ";");
			if (ast.size() != 10) {
				est.Format("Wrong column count at line in rules file %s: '%s'", fname, st);
				WriteLog(5, est);
				return;
			}
			set = atoi(ast[0]);
			rid = atoi(ast[1]);
			sev = atoi(ast[2]);
			rule = ast[5];
			subrule = ast[6];
			flag = atoi(ast[7]);
			// Find rule id
			if (rid >= max_flags) {
				max_flags = rid + 1;
				if (max_flags >= MAX_RULES) {
					est.Format("Rule id (%d) is equal or greater than MAX_RULES (%d). Consider increasing MAX_RULES", rid, MAX_RULES);
					WriteLog(5, est);
					return;
				}
			}
			//est.Format("Found rule %s - %d", rule, rid);
			//WriteLog(0, est);
			if (accepts[set].size() < MAX_RULES) {
				accepts[set].resize(MAX_RULES);
				RuleName[set].resize(MAX_RULES);
				SubRuleName[set].resize(MAX_RULES);
				SubRuleComment[set].resize(MAX_RULES);
			}
			RuleName[set][rid] = rule;
			SubRuleName[set][rid] = subrule;
			RuleComment[rid] = ast[8];
			SubRuleComment[set][rid] = ast[9];
			// If testing, enable all disabled rules so that expected violations can be confirmed
			//if (m_testing && flag == -1) flag = 1;
			accepts[set][rid] = flag;
			severity[rid] = sev;
		}
	}
	fs.close();
	est.Format("LoadRules loaded %d lines from %s", i, fname);
	WriteLog(0, est);
}

// Return chromatic length of an interval (e.g. return 4 from 3rd)
int CGenCF1::Interval2Chromatic(int iv) {
	if (iv > 0) --iv;
	else ++iv;
	int aiv = iv % 7;
	int res = 0;
	if (aiv == 1) res = 2;
	else if (aiv == 2) res = 4;
	else if (aiv == 3) res = 5;
	else if (aiv == 4) res = 7;
	else if (aiv == 5) res = 9;
	else if (aiv == 6) res = 11;
	else if (aiv == -1) res = 1;
	else if (aiv == -2) res = 3;
	else if (aiv == -3) res = 5;
	else if (aiv == -4) res = 6; // Tritone
	else if (aiv == -5) res = 8;
	else if (aiv == -6) res = 10;
	// Add octaves
	res += 12*abs(iv / 7);
	return res;
}

// Load rules
void CGenCF1::ParseRule(int rset, int rid, int type) {
	CString st;
	if (type == rsName) st = RuleName[rset][rid];
	if (type == rsSubName) st = SubRuleName[rset][rid];
	if (type == rsComment) st = RuleComment[rid];
	if (type == rsSubComment) st = SubRuleComment[rset][rid];
	vector<int> v;
	GetVint(st, v);
	if (v.size()) {
		// Create types
		if (!RuleParam[rset][rid].size()) RuleParam[rset][rid].resize(4);
		// Set params for type
		RuleParam[rset][rid][type] = v;
	}
}

int CGenCF1::GetRuleParam(int rset, int rid, int type, int id) {
	if (!RuleParam[rset][rid].size() || id >= RuleParam[rset][rid][type].size()) {
		CString est, rs;
		CString st;
		if (type == rsName) st = RuleName[rset][rid];
		if (type == rsSubName) st = SubRuleName[rset][rid];
		if (type == rsComment) st = RuleComment[rid];
		if (type == rsSubComment) st = SubRuleComment[rset][rid];
		if (type == rsName) rs = "rule name";
		if (type == rsSubName) rs = "subrule name";
		if (type == rsComment) rs = "rule comment";
		if (type == rsSubComment) rs = "subrule comment";
		est.Format("Error parsing integer #%d from %s %d: '%s' (rule set %d)", id+1, rs, rid, st, rset);
		WriteLog(5, est);
		return 0;
	}
	return RuleParam[rset][rid][type][id];
}

// Parse rules
void CGenCF1::ParseRules() {
	for (int rset = 0; rset < accepts.size(); ++rset) if (accepts[rset].size()) {
		RuleParam[rset].resize(MAX_RULES);
		for (int rid = 0; rid < MAX_RULES; ++rid) {
			for (int rs = 0; rs < 4; ++rs) {
				ParseRule(rset, rid, rs);
			}
		}
	}
}

// Set parsed parameters of current ruleset
void CGenCF1::SetRuleParams() {
	pco_apart = GetRuleParam(rule_set, 248, rsName, 0);
	thirds_ignored = GetRuleParam(rule_set, 70, rsName, 0);
	c4p_last_leaps = GetRuleParam(rule_set, 144, rsName, 1);
	pre_last_leaps = GetRuleParam(rule_set, 204, rsName, 0);
	max_smooth = GetRuleParam(rule_set, 4, rsSubName, 0);
	max_smooth_direct = GetRuleParam(rule_set, 5, rsSubName, 0);
	max_leap_steps = GetRuleParam(rule_set, 3, rsName, 0);
	max_leaps = GetRuleParam(rule_set, 3, rsSubName, 0);
	max_leaped = GetRuleParam(rule_set, 25, rsSubName, 0);
	max_leap_steps2 = GetRuleParam(rule_set, 202, rsName, 0);
	max_leaps2 = GetRuleParam(rule_set, 202, rsSubName, 0);
	max_leaped2 = GetRuleParam(rule_set, 203, rsSubName, 0);
	cse_leaps = GetRuleParam(rule_set, 70, rsSubName, 0);
	cse_leaps2 = GetRuleParam(rule_set, 71, rsSubName, 0);
	stag_notes = GetRuleParam(rule_set, 10, rsSubName, 0);
	stag_note_steps = GetRuleParam(rule_set, 10, rsSubName, 1);
	stag_notes2 = GetRuleParam(rule_set, 39, rsSubName, 0);
	stag_note_steps2 = GetRuleParam(rule_set, 39, rsSubName, 1);
	repeat_steps2 = GetRuleParam(rule_set, 76, rsSubName, 1);
	repeat_steps3 = GetRuleParam(rule_set, 36, rsSubName, 1);
	repeat_steps5 = GetRuleParam(rule_set, 72, rsSubName, 1);
	repeat_steps7 = GetRuleParam(rule_set, 73, rsSubName, 1);
	repeat_notes2 = GetRuleParam(rule_set, 76, rsSubName, 0);
	repeat_notes3 = GetRuleParam(rule_set, 36, rsSubName, 0);
	repeat_notes5 = GetRuleParam(rule_set, 72, rsSubName, 0);
	repeat_notes7 = GetRuleParam(rule_set, 73, rsSubName, 0);
	min_interval = Interval2Chromatic(GetRuleParam(rule_set, 38, rsSubName, 0));
	max_interval = Interval2Chromatic(GetRuleParam(rule_set, 37, rsSubName, 0));
	sum_interval = Interval2Chromatic(GetRuleParam(rule_set, 7, rsSubName, 0));
	max_between = Interval2Chromatic(GetRuleParam(rule_set, 11, rsSubName, 0));
	burst_between = Interval2Chromatic(GetRuleParam(rule_set, 11, rsSubComment, 0));
	burst_steps = GetRuleParam(rule_set, 11, rsSubComment, 1);
	slurs_window = GetRuleParam(rule_set, 93, rsName, 0);
	miss_slurs_window = GetRuleParam(rule_set, 188, rsName, 0);
	contrary_min = GetRuleParam(rule_set, 35, rsSubName, 0);
	contrary_min2 = GetRuleParam(rule_set, 46, rsSubName, 0);

	notes_lrange = GetRuleParam(rule_set, 98, rsSubName, 0);
	min_lrange = Interval2Chromatic(GetRuleParam(rule_set, 98, rsSubName, 1));
	notes_lrange2 = GetRuleParam(rule_set, 198, rsSubName, 0);
	min_lrange2 = Interval2Chromatic(GetRuleParam(rule_set, 198, rsSubName, 1));
	notes_arange = GetRuleParam(rule_set, 15, rsSubName, 0);
	min_arange = GetRuleParam(rule_set, 15, rsSubName, 1) / 10.0;
	notes_arange2 = GetRuleParam(rule_set, 16, rsSubName, 0);
	min_arange2 = GetRuleParam(rule_set, 16, rsSubName, 1) / 10.0;

	dev_late2 = GetRuleParam(rule_set, 191, rsSubName, 0);
	dev_late3 = GetRuleParam(rule_set, 192, rsSubName, 0);
	early_culm = GetRuleParam(rule_set, 78, rsSubName, 0);
	early_culm2 = GetRuleParam(rule_set, 79, rsSubName, 0);
	early_culm3 = GetRuleParam(rule_set, 193, rsSubName, 0);
	late_culm = GetRuleParam(rule_set, 21, rsSubName, 0);
	hsp_leap = Interval2Chromatic(GetRuleParam(rule_set, 194, rsSubName, 0));
	repeat_letters = GetRuleParam(rule_set, 17, rsSubName, 0);
	miss_letters = GetRuleParam(rule_set, 20, rsSubName, 0);
	ico_chain = GetRuleParam(rule_set, 89, rsSubName, 0);
	ico_chain2 = GetRuleParam(rule_set, 96, rsSubName, 0);
	gis_trail_max = GetRuleParam(rule_set, 200, rsSubName, 0);
	fis_leap = Interval2Chromatic(GetRuleParam(rule_set, 201, rsSubName, 0));
	tonic_max = GetRuleParam(rule_set, 196, rsSubName, 0);
	tonic_window = GetRuleParam(rule_set, 196, rsSubName, 1);
	tonic_leap = Interval2Chromatic(GetRuleParam(rule_set, 196, rsSubComment, 1));
	tonic_leap_weight = Interval2Chromatic(GetRuleParam(rule_set, 196, rsSubComment, 2));
	fis_gis_max = GetRuleParam(rule_set, 199, rsSubName, 0);
}

void CGenCF1::CheckConfig() {
	// GenCP1
	if (m_algo_id == 121) {
		if (accept_cantus_rechoose && cantus_id2) {
			WriteLog(1, "Warning: accept_cantus_rechoose will not work with cantus_id above zero (check config)");
		}
	}
	if (species == 1 && fn > 0) {
		WriteLog(5, "Warning: Counterpoint species 1 cannot have starting_pause (check config)");
	}
	if (fn >= npm) {
		WriteLog(5, "Warning: Starting_pause is greater or equals to notes_per_measure (check config)");
	}
	if (fn > 1 && npm < 8) {
		WriteLog(1, "Warning: Starting_pause > 1 is not recommended for 1-4 notes per measure (check config)");
	}
	if (fn == 1 && npm == 8) {
		WriteLog(1, "Warning: Starting_pause 1 is not recommended for 8 notes per measure (check config)");
	}
	if (species == 2 && npm != 2) {
		WriteLog(5, "Warning: Counterpoint species 2 should have notes_per_measure = 2 or just comment out notes_per_measure so that it is controlled by species parameter automatically (check config)");
	}
	if (species == 3 && npm != 4) {
		WriteLog(5, "Warning: Counterpoint species 3 should have notes_per_measure = 4 or just comment out notes_per_measure so that it is controlled by species parameter automatically (check config)");
	}
	if (species == 4 && npm != 2) {
		WriteLog(5, "Warning: Counterpoint species 4 should have notes_per_measure = 2 or just comment out notes_per_measure so that it is controlled by species parameter automatically (check config)");
	}
	if (species == 5 && npm != 8) {
		WriteLog(5, "Warning: Counterpoint species 5 should have notes_per_measure = 8 or just comment out notes_per_measure so that it is controlled by species parameter automatically (check config)");
	}
	// Check configuration parameters
	if (accept_reseed == 1 && random_seed == 0) {
		WriteLog(5, "Warning: accept_reseed=1 while random_seed=0. You will get same results after every reseed (check config)");
	}
	if (method == mScan) {
		//WriteLog(1, "Warning: Window-scan method is currently not working correctly (needs debugging). Please choose another method in config file.");
	}
	if (midifile_export_marks && midifile_export_comments) {
		WriteLog(5, "Warning: You are trying to export both marks and comments to MIDI file: midifile_export_marks and midifile_export_comments both set. They can overlap (check config)");
	}
	if (calculate_correlation || calculate_blocking || calculate_stat || calculate_ssf || best_rejected) {
		WriteLog(1, "Algorithm is running in low performance mode. To increase performance, reset calculate_correlation, calculate_blocking, calculate_stat, calculate_ssf, best_rejected (check config)");
	}
	if (shuffle && random_seed) {
		WriteLog(1, "Shuffling after random_seed will not add randomness (check config)");
	}
	// Check rule parameters
	if (burst_between <= max_between) {
		WriteLog(5, "Warning: maximum burst interval should be greater than maximum interval between voices (check config)");
	}
}

// Select rules
int CGenCF1::SelectRuleSet(int rs)
{
	rule_set = rs;
	if (!accepts[rule_set].size()) {
		CString est;
		est.Format("Cannot select rule set %d. It was not loaded from rules configuration file.", rule_set);
		WriteLog(5, est);
		error = 1;
	}
	else {
		// Load rule set
		for (int i = 0; i < max_flags; ++i) {
			accept[i] = accepts[rule_set][i];
		}
		// Check that at least one rule is accepted
		for (int i = 0; i < max_flags; ++i) {
			if (accept[i]) break;
			if (i == max_flags - 1) WriteLog(5, "Warning: all rules are rejected (0) in configuration file");
		}
		// Calculate second level flags count
		flags_need2 = 0;
		for (int i = 0; i < max_flags; ++i) {
			if (accept[i] == 2) ++flags_need2;
		}
	}
	return error;
}

void CGenCF1::LoadConfigLine(CString* sN, CString* sV, int idata, float fdata)
{
	CheckVar(sN, sV, "max_correct_ms", &max_correct_ms, 0);
	CheckVar(sN, sV, "animate", &animate, 0);
	CheckVar(sN, sV, "animate_delay", &animate_delay, 0);
	CheckVar(sN, sV, "cantus_high", &cantus_high, 0, 1);
	CheckVar(sN, sV, "rpenalty_accepted", &rpenalty_accepted, 0);
	CheckVar(sN, sV, "c_len", &c_len, 1);
	CheckVar(sN, sV, "s_len", &s_len, 1);
	LoadNote(sN, sV, "first_note", &first_note);
	LoadNote(sN, sV, "last_note", &last_note);
	CheckVar(sN, sV, "fill_steps_mul", &fill_steps_mul);
	CheckVar(sN, sV, "transpose_back", &transpose_back, 0, 1);
	CheckRange(sN, sV, "tempo", &min_tempo, &max_tempo, 1);
	CheckVar(sN, sV, "random_choose", &random_choose, 0, 1);
	CheckVar(sN, sV, "random_key", &random_key, 0, 1);
	CheckVar(sN, sV, "random_seed", &random_seed, 0, 1);
	CheckVar(sN, sV, "random_range", &random_range, 0, 1);
	CheckVar(sN, sV, "accept_reseed", &accept_reseed, 0, 1);
	CheckVar(sN, sV, "shuffle", &shuffle, 0, 1);
	CheckVar(sN, sV, "first_steps_tonic", &first_steps_tonic, 1);
	CheckVar(sN, sV, "show_severity", &show_severity, 0, 1);
	CheckVar(sN, sV, "calculate_correlation", &calculate_correlation, 0, 1);
	CheckVar(sN, sV, "calculate_stat", &calculate_stat, 0, 1);
	CheckVar(sN, sV, "calculate_ssf", &calculate_ssf, 0, 1);
	CheckVar(sN, sV, "best_rejected", &best_rejected, 0);
	CheckVar(sN, sV, "calculate_blocking", &calculate_blocking, 0, 1);
	CheckVar(sN, sV, "late_require", &late_require, 0, 1);
	// Random SWA
	//CheckVar(sN, sV, "fullscan_max", &fullscan_max);
	CheckVar(sN, sV, "approximations", &approximations, 1);
	CheckVar(sN, sV, "swa_steps", &swa_steps, 1);
	CheckVar(sN, sV, "correct_range", &correct_range, 1);
	CheckVar(sN, sV, "correct_inrange", &correct_inrange, 0);
	CheckVar(sN, sV, "optimize_dpenalty", &optimize_dpenalty, 0, 1);
	CheckVar(sN, sV, "cf_rule_set", &cf_rule_set, 0);
	CheckVar(sN, sV, "cp_rule_set", &cp_rule_set, 0);

	// Load HSP
	if (*sN == "hsp_file") {
		++parameter_found;
		LoadHSP("configs\\rules\\" + *sV);
	}
	// Load rules
	if (*sN == "rules_file") {
		++parameter_found;
		LoadRules("configs\\rules\\" + *sV);
		ParseRules();
	}
	// Load method
	if (*sN == "method") {
		++parameter_found;
		if (*sV == "window-scan") method = mScan;
		else if (*sV == "swa") method = mSWA;
		else {
			CString est;
			est.Format("Warning: method name unrecognized: %s", *sV);
			WriteLog(5, est);
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
		SetRuleParams();
	}
}

void CGenCF1::LogCantus(CString st3, int x, vector<int> &c)
{
	CString st, st2;
	st2.Format("%s %d: ", st3, x);
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
void CGenCF1::FillCantus(vector<int>& c, int step1, int step2, vector<int> &value)
{
	for (int i = step1; i < step2; ++i) {
		c[i] = value[i];
	}
}

// Step2 must be exclusive
void CGenCF1::FillCantus(vector<int>& c, int step1, int step2, vector<vector<int>> &value)
{
	for (int i = step1; i < step2; ++i) {
		c[i] = value[i][0];
	}
}

void CGenCF1::FillCantusMap(vector<int>& c, vector<int>& smap, int step1, int step2, vector<int>& value)
{
	// Step2 must be exclusive
	for (int i = step1; i < step2; ++i) {
		c[smap[i]] = value[smap[i]];
	}
}

void CGenCF1::FillCantusMap(vector<int>& c, vector<int>& smap, int step1, int step2, vector<vector<int>>& value)
{
	// Step2 must be exclusive
	for (int i = step1; i < step2; ++i) {
		c[smap[i]] = value[smap[i]][0];
	}
}

void CGenCF1::FillCantusMap(vector<int>& c, vector<int>& smap, int step1, int step2, int value)
{
	// Step2 must be exclusive
	for (int i = step1; i < step2; ++i) {
		c[smap[i]] = value;
	}
}

// Detect repeating notes. Step2 excluding
int CGenCF1::FailNoteRepeat(vector<int> &cc, int step1, int step2) {
	for (int i = step1; i < step2; ++i) {
		if (cc[i] == cc[i + 1]) FLAG2(30, i);
	}
	return 0;
}

// Detect prohibited note sequences
int CGenCF1::FailNoteSeq(vector<int> &pc) {
	for (int x = 0; x < fli_size-2; ++x) {
		s = fli2[x];
		s1 = fli2[x + 1];
		// Prohibit GC before cadence
		//if (pc[s] == 4 && pc[s1] == 0) FLAG2(48, s);
	}
	return 0;
}

int CGenCF1::FailLocalRange(vector<int> &cc, int notes, int mrange, int flag) {
	// Do not test if flag disabled and not testing
	if (!m_testing && accept[flag] == -1) return 0;
	// Do not test if not enough notes. If melody is short, than global range check is enough
	if (fli_size < notes) return 0;
	int lmin, lmax, s;
	int ls_max = fli_size - notes;
	int ls_max2;
	// Loop through windows
	for (ls = 0; ls <= ls_max; ++ls) {
		lmin = MAX_NOTE;
		lmax = 0;
		ls_max2 = ls + notes;
		// Loop inside each window
		for (int ls2 = ls; ls2 < ls_max2; ++ls2) {
			s = fli2[ls2];
			if (cc[s] < lmin) lmin = cc[s];
			if (cc[s] > lmax) lmax = cc[s];
		}
		// Check range
		if (lmax - lmin < mrange) FLAG2(flag, fli2[ls]);
	}
	return 0;
}

int CGenCF1::FailLocalMacc(int notes, float mrange, int flag) {
	// Do not test if flag disabled and not testing
	if (!m_testing && accept[flag] == -1) return 0;
	// Do not test if not enough notes. If melody is short, than global range check is enough
	if (fli_size < notes) return 0;
	float lmin, lmax;
	int s;
	int ls_max = fli_size - notes;
	int ls_max2;
	// Loop through windows
	for (ls = 0; ls <= ls_max; ++ls) {
		lmin = MAX_NOTE;
		lmax = 0;
		ls_max2 = ls + notes;
		// Loop inside each window
		for (int ls2 = ls; ls2 < ls_max2; ++ls2) {
			s = fli2[ls2];
			if (macc2[s] < lmin) lmin = macc2[s];
			if (macc2[s] > lmax) lmax = macc2[s];
		}
		// Check range
		if (lmax - lmin < mrange) FLAG2(flag, fli2[ls]);
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
		fill(fpenalty.begin(), fpenalty.end(), 0.0);
	}
	flags[0] = 1;
	for (int i = step1; i < step2; ++i) {
		anflagsc[cpv][i] = 0;
	}
	rpenalty_cur = 0;
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
	if (count > repeat_letters) FLAG3(17, i);
	if (wcount > miss_letters) FLAG3(20, i);
	return 0;
}

int CGenCF1::FailGisTrail(vector<int> &pcc) {
	int gis_trail = 0;
	for (ls = 0; ls < fli_size; ++ls) {
		s = fli2[ls];
		if (pcc[s] == 11) {
			// Set to maximum on new G# note
			gis_trail = gis_trail_max;
		}
		else {
			if (pcc[s] == 10) {
				// Prohibit G note close to G#
				if (gis_trail) FLAG2(200, s);
			}
		}
		// Decrease if not zero
		if (gis_trail) --gis_trail;
	}
	return 0;
}

int CGenCF1::FailFisTrail(vector<int> &pcc) {
	int pos1, pos2, found;
	for (ls = 0; ls < fli_size; ++ls) {
		s = fli2[ls];
		if (pcc[s] == 9) {
			pos1 = max(0, ls - fis_gis_max);
			pos2 = min(fli_size - 1, ls + fis_gis_max);
			found = 0;
			for (int x = pos1; x <= pos2; ++x) {
				if (pcc[fli[x]] == 11) {
					found = 1;
					break;
				}
			}
			if (!found) FLAG2(199, s);
		}
	}
	return 0;
}

int CGenCF1::EvalMelodyHarm(int hp, int &last_flag, int &max_p) {
	int pen1;
	int p2c = 0; // Count of consecutive penalty 2
	int dcount = 0;
	int scount = 0;
	int tcount = 0;
	int wdcount = 0;
	int wscount = 0;
	int wtcount = 0;
	for (int i = 0; i <= hp; ++i) {
		if (i > 0) {
			// Check GC for low cantus and not last note (last note in any window is ignored)
			if (!cantus_high && i < fli_size-1 && chm[i] == 0 && chm[i - 1] == 4) {
				if (m_pc[fli2[i]] == 0 && m_pc[fli2[i - 1]] == 4) FLAG3(48, i);
			}
			// Check harmonic penalty	
			pen1 = hsp[chm[i - 1]][chm[i]];
			if (pen1 == 3) FLAG3(99, i)
			else if (pen1 == 1) FLAG3(77, i);
			if (pen1 == 2) {
				FLAG3(57, i);
				if (culm_step == i) FLAG3(195, i);
				if (abs(m_cc[fli2[i]]-m_cc[fli2[i-1]]) > hsp_leap) FLAG3(194, i);
				++p2c;
				if (p2c == 2) FLAG3(92, i + 1)
				else if (p2c == 3) FLAG3(23, i + 1);
			}
			else {
				p2c = 0;
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
	int h;
	int first_tonic = 0;
	// Build hm vector
	for (ls = 0; ls < fli_size; ++ls) {
		s = fli2[ls];
		hm[ls].clear();
		for (int x = 0; x < hv[pc[s]].size(); ++x) {
			h = hv[pc[s]][x];
			// Check tonic
			if (!h) {
				// Is this first or last tonic?
				if (!first_tonic || s == c_len - 1) {
					first_tonic = 1;
					// Set only tonic for this step
					hm[ls].clear();
					hm[ls].push_back(h);
					break;
				}
			}
			// If tonic allowed or not tonic
			hm[ls].push_back(h);
		}
		// Shuffle
		if (task == tEval) {
			long long seed = CGLib::time();
			::shuffle(hm[ls].begin(), hm[ls].end(), default_random_engine(seed));
		}
	}
	// Scan vector
	vector<int> chm_saved;
	chm.clear();
	chmp.clear();
	chm.resize(fli_size, 0);
	chmp.resize(fli_size, 0);
	for (int i = 0; i < fli_size; ++i) chm[i] = hm[i][0];
	int hp = 0;
	int finished = 0;
	int found = 0;
	int hcycle = 0;
	int last_flag = 0;
	int last_flag2 = 0;
	int max_p = 0;
	int max_p2 = 0;
	//LogCantus(pc);
	while (true) {
	check:
		//st.Format("%d: ", hp);
		//CGLib::AppendLineToFile("log/temp.log", st);
		//LogCantus(chmp);
		//LogCantus(chm);
		if (need_exit) return 1;
		if (!hp) {
			++hp;
			if (hp > max_p) max_p = hp;
			goto check;
		}
		if (EvalMelodyHarm(hp, last_flag, max_p)) goto skip;
		// Success
		if (hp == fli_size-1) {
			found = 1;
			break;
		}
		else {
			++hp;
			if (hp > max_p) {
				max_p = hp;
			}
			goto check;
		}
	skip:
		// Save maximum flag
		if (hp > max_p2) {
			max_p2 = hp;
			if (last_flag) {
				last_flag2 = last_flag;
				chm_saved = chm;
			}
		}
		// ScanLeft
		while (true) {
			if (chmp[hp] < hm[hp].size()-1) break;
			// If current element is max, make it minimum
			chmp[hp] = 0;
			// Get current value
			chm[hp] = hm[hp][chmp[hp]];
			// Move left one element
			if (!hp) {
				finished = 1;
				break;
			}
			hp--;
		} // while (true)
		if (finished) {
			break;
		}
		// ScanRight
		// Increase rightmost element, which was not reset to minimum
		++chmp[hp];
		// Get current value
		chm[hp] = hm[hp][chmp[hp]];
		++hcycle;
	}
	// Detect possible variants
	if (!found) {
		// Load problematic chm if it exists
		if (chm_saved.size()) chm = chm_saved;
		if (max_p < fli_size - 1) {
			//fill(chm.begin(), chm.end(), -1);
		}
		// Increase penalty if flag was found at the beginning of melody
		fpenalty[last_flag2] = fli_size - max_p;
		// Report one of last flags at highest position
		FLAG2(last_flag2, max_p);
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
	// Build decrement
	for (int i = 0; i < 127; ++i) {
		pos = (i + 23 - tonic_cur) % 12;
		if (minor_cur) {
			if (m_diatonic_full[pos]) cc_decr[i] = 1;
			else cc_decr[i] = 2;
		}
		else {
			if (diatonic[pos]) cc_decr[i] = 1;
			else cc_decr[i] = 2;
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
		for (int i = step1; i < step2; ++i) {
			c[i] = m_CC_C(cc[i], tonic_cur);
		}
	}
	else {
		for (int i = step1; i < step2; ++i) {
			c[i] = maj_CC_C(cc[i], tonic_cur);
		}
	}
	return 0;
}

// Search for outstanding repeats
int CGenCF1::FailOutstandingRepeat(vector<int> &c, vector<int> &cc, vector<int> &leap, int scan_len, int rlen, int flag) {
	// Do not test if flag disabled and not testing
	if (!m_testing && accept[flag] == -1) return 0;
	int ok, f, f1;
	if (fli_size > rlen*2) for (ls = 0; ls < fli_size - rlen * 2; ++ls) {
		s = fli2[ls];
		s1 = fli2[ls + 1];
		if (MELODY_SEPARATION(s, s1)) {
			// Search for repeat of note at same beat until last three notes
			int finish = ls + scan_len;
			if (finish > fli_size - rlen) finish = fli_size - rlen;
			for (int x = ls + 2; x <= finish; ++x) {
				f = fli2[x];
				f1 = fli2[x + 1];
				// Check rhythm
				if ((f - s) % 2) continue;
				if (MELODY_SEPARATION(f, f1)) {
					// Check if same note
					if (cc[f] == cc[s] && llen[x] == llen[ls]) {
						// Check that more notes repeat
						ok = 0;
						for (int z = 1; z < rlen; ++z) {
							if (cc[fli2[x + z]] != cc[fli2[ls + z]] || llen[x+z] != llen[ls+z]) {
								ok = 1;
								break;
							}
						}
						if (!ok) {
							FLAG2(flag, s);
						}
					}
				}
			}
		}
	}
	return 0;
}

int CGenCF1::FailLongRepeat(vector<int> &cc, vector<int> &leap, int scan_len, int rlen, int flag) {
	// Do not test if flag disabled and not testing
	if (!m_testing && accept[flag] == -1) return 0;
	int ok;
	int f, f1;
	if (fli_size > rlen + 1) for (ls = 0; ls < fli_size - rlen - 1; ++ls) {
		s = fli2[ls];
		s1 = fli2[ls + 1];
		int finish = ls + scan_len;
		if (finish > fli_size - rlen) finish = fli_size - rlen;
		for (int x = ls + rlen; x <= finish; ++x) {
			f = fli2[x];
			f1 = fli2[x + 1];
			// Check if same note
			if (cc[f] == cc[s] && llen[x] == llen[ls]) {
				// Check that more notes repeat
				ok = 0;
				for (int z = 1; z < rlen; ++z) {
					if (cc[fli2[x + z]] != cc[fli2[ls + z]] || llen[x + z] != llen[ls + z]) {
						ok = 1;
						break;
					}
				}
				if (!ok) {
					FLAG2(flag, s);
				}
			}
		}
	}
	return 0;
}

int CGenCF1::FailManyLeaps(vector<int> &c, vector<int> &cc, vector<int> &leap, vector<int> &smooth, vector<int> &slur, int mleaps, int mleaped, int mleapsteps, int flag1, int flag2) {
	int leap_sum = 0;
	int leaped_sum = 0;
	int max_leap_sum = 0;
	int max_leaped_sum = 0;
	int leap_sum_i = 0;
	for (ls = 0; ls < fli_size - 1; ++ls) {
		s = fli2[ls];
		s1 = fli2[ls + 1];
		// Add new leap
		if (leap[s] != 0) {
			++leap_sum;
			leaped_sum += abs(c[s] - c[s1]) + 1;
		}
		// Subtract old leap
		if ((ls >= mleapsteps) && (leap[fli2[ls - mleapsteps]] != 0)) {
			leap_sum--;
			leaped_sum -= abs(c[fli2[ls - mleapsteps]] - c[fli2[ls - mleapsteps + 1]]) + 1;
		}
		// Get maximum leap_sum
		if (leap_sum >= mleaps && leap_sum > max_leap_sum) {
			max_leap_sum = leap_sum;
			leap_sum_i = s;
		}
		if (leaped_sum > mleaped && leaped_sum > max_leaped_sum) {
			max_leaped_sum = leaped_sum;
			leap_sum_i = s;
		}
		// Calculate penalty
		if (!accept[flag1] && leap_sum > mleaps) ++fpenalty[flag1];
		if (!accept[flag2] && leaped_sum > mleaped) ++fpenalty[flag2];
	}
	if (max_leap_sum > mleaps) FLAG2(flag1, leap_sum_i);
	if (max_leaped_sum > mleaped) FLAG2(flag2, leap_sum_i);
	return 0;
}

// Calculate global leap smooth slur variables
void CGenCF1::GetLeapSmooth(vector<int> &c, vector<int> &cc, vector<int> &leap, vector<int> &smooth, vector<int> &slur) {
	for (int i = fn; i < ep2 - 1; ++i) {
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
}

// Check if too many leaps
int CGenCF1::FailLeapSmooth(vector<int> &c, vector<int> &cc, vector<int> &leap, vector<int> &smooth, vector<int> &slur) {
	// Clear variables
	int leap_sum2 = 0;
	int thirds_sum = 0;
	int leap_sum_corrected = 0;
	int max_leap_sum2 = 0;
	int smooth_sum = 0;
	int smooth_sum2 = 0;
	int leap_sum_s2 = 0;
	for (ls = 0; ls < fli_size - 2; ++ls) {
		s = fli2[ls];
		s1 = fli2[ls+1];
		// Add new leap
		if (leap[s] != 0) {
			++leap_sum2;
			if (c[s1] - c[s] == 2) ++thirds_sum;
		}
		else {
			leap_sum2 = 0;
			thirds_sum = 0;
		}
		// Get maximum leap_sum
		leap_sum_corrected = leap_sum2 - min(thirds_sum, thirds_ignored);
		if (leap_sum_corrected > max_leap_sum2) {
			max_leap_sum2 = leap_sum_corrected;
			leap_sum_s2 = s;
		}
		// Calculate penalty
		if (leap_sum_corrected == cse_leaps) {
			if (accept[70] > 0) ++fpenalty[70];
			if (leap_sum_corrected > cse_leaps2 && accept[71] > 0) ++fpenalty[71];
		}
		// Prohibit long smooth movement
		if (smooth[s] != 0) {
			++smooth_sum;
			if (smooth_sum >= max_smooth) FLAG2(4, s);
		}
		else if (leap[s]) smooth_sum = 0;
		if (ls < fli_size - 2) {
			// Prohibit long smooth movement in one direction
			if (smooth[s] != 0 && smooth[s] == smooth[fli2[ls+1]]) {
				++smooth_sum2;
				if (smooth_sum2 >= max_smooth_direct) 
					FLAG2(5, s);
			}
			else if (smooth[s] || leap[s]) smooth_sum2 = 0;
			// Check if two notes repeat
			if ((ls > 0) && (cc[s] == cc[fli2[ls+2]]) && (cc[fli2[ls - 1]] == cc[fli2[ls+1]])) FLAG2(9, fli2[ls-1]);
		}
	}
	if (max_leap_sum2 >= cse_leaps) {
		if (max_leap_sum2 > cse_leaps2) FLAG2(71, leap_sum_s2)
		else FLAG2(70, leap_sum_s2);
	}
	return 0;
}

int CGenCF1::FailStagnation(vector<int> &cc, vector<int> &nstat, int steps, int notes, int flag) {
	// Do not test if flag disabled and not testing
	if (!m_testing && accept[flag] == -1) return 0;
	// Clear nstat
	for (int i = nmin; i <= nmax; ++i) nstat[i] = 0;
	// Prohibit stagnation only for non-slurred notes
	++nstat[cc[0]];
	for (int i = fn+1; i < ep2; ++i) if (cc[i-1] != cc[i]) {
		// Add new note to stagnation array
		++nstat[cc[i]];
		// Subtract old note
		if ((i >= steps)) --nstat[cc[i - steps]];
		// Check if too many repeating notes
		if (nstat[cc[i]] > notes) FLAG2(flag, i);
	}
	return 0;
}

// Prohibit multiple culminations
int CGenCF1::FailMultiCulm(vector<int> &cc, vector<int> &slur) {
	int culm_sum = 0;
	culm_step = -1;
	if (ep2 < c_len) {
		// Find multiple culminations at highest allowed note
		if (nmax == max_cc[0] || nmax - nmin == max_interval) {
			for (ls = 0; ls < fli_size; ++ls) {
				if (cc[fli2[ls]] == nmax) {
					++culm_sum;
					culm_step = ls;
					if (culm_sum > 1) FLAG2(12, fli2[culm_step]);
				}
			}
			if (culm_step == -1) {
				culm_step = 0;
				CString est;
				est.Format("Warning: culm_step cannot be detected at step %d", step);
				WriteLog(5, est);
			}
			// Prohibit culminations at first steps on highest notes
			if (cc[fli2[culm_step]] == nmax) {
				if (culm_step < (early_culm3*c_len) / 100) FLAG2(193, fli2[culm_step]);
				if (culm_step < early_culm - 1) FLAG2(78, fli2[culm_step])
				else if (culm_step < early_culm2 - 1) FLAG2(79, fli2[culm_step]);
			}
		}
	}
	else {
		for (ls = 0; ls < fli_size; ++ls) {
			if (cc[fli2[ls]] == nmax) {
				++culm_sum;
				culm_step = ls;
				if (culm_sum > 1) FLAG2(12, fli2[culm_step]);
			}
		}
		if (culm_step == -1) {
			culm_step = 0;
			CString est;
			est.Format("Warning: culm_step cannot be detected at step %d", step);
			WriteLog(5, est);
		}
		// Prohibit culminations at first steps
		if (culm_step < (early_culm3*c_len)/100) FLAG2(193, fli2[culm_step]);
		if (culm_step < early_culm - 1) FLAG2(78, fli2[culm_step])
		else if (culm_step < early_culm2 - 1) FLAG2(79, fli2[culm_step]);
		// Prohibit culminations at last steps
		if (culm_step >= fli_size - late_culm) FLAG2(21, fli2[culm_step]);
		// Prohibit synchronized culminationsnati
		if (av_cnt > 1 && culm_step == cf_culm) FLAG2(26, fli2[culm_step]);
	}
	return 0;
}

int CGenCF1::FailFirstNotes(vector<int> &pc) {
	// Prohibit first note not tonic
	if (pc[0] != 0) {
		FLAG2(49, fn);
		if (pc[0] != 4) FLAG2(90, fn);
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
			if (pc[fli2[i]] == 0) c_pos = i;
			// Detect E note
			if (pc[fli2[i]] == 2) e_pos = i;
		}
		int ok = 0;
		// No C ?
		if (c_pos == -1) FLAG2(40, fn)
		else {
			// If C found, check previous note
			if (c_pos > 0) {
				if (pc[fli2[c_pos - 1]] == 6 || pc[fli2[c_pos - 1]] == 1 || pc[fli2[c_pos - 1]] == 4) ok = 1;
			}
			// If C is first note, it does not need to be prepared (actually, this cannot happen because of flag 49)
			else ok = 1;
		}
		// No E ?
		if (e_pos == -1) FLAG2(41, fn)
		else {
			// If E found, check previous note
			if (e_pos > 0) {
				if (pc[fli2[e_pos - 1]] == 1 || pc[fli2[e_pos - 1]] == 4) ok = 1;
			}
			// If E is first note, it does not need to be prepared
			else ok = 1;
		}
		// Is E or C prepared?
		if (!ok) FLAG2(52, fn);
	}
	return 0;
}

int CGenCF1::FailLastNotes(vector<int> &pc, vector<int> &pcc) {
	// Do not check if melody is short yet
	if (fli_size < 3) return 0;
	// Prohibit last note not tonic
	if (ep2 > c_len - 1) {
		s = fli[fli_size - 1];
		s_1 = fli[fli_size - 2];
		s_2 = fli[fli_size - 3];
		if (pc[s] != 0) FLAG2(50, s);
		// Prohibit major second up before I (applicable to major and minor)
		if (pcc[s] == 0 && pcc[s_1] == 10) FLAG2(74, s_1);
		if (pcc[s] == 0 && pcc[s_2] == 10) FLAG2(74, s_2);
	}
	// Wrong second to last note (last note never can be slurred)
	if (ep2 > c_len - 2) {
		s_1 = fli[fli_size - 2];
		if ((pc[s_1] == 0) || (pc[s_1] == 2) || (pc[s_1] == 3) || (pc[s_1] == 5)) FLAG2(13, s_1);
		if (pc[s_1] == 4) FLAG2(51, s_1);
	}
	// Wrong third to last note
	if (ep2 > c_len - 3) {
		s_2 = fli[fli_size - 3];
		if ((pc[s_2] == 0) || (pc[s_2] == 2) || (pc[s_2] == 4)) FLAG2(14, s_2);
		// Leading third to last note
		if (pc[s_2] == 6) FLAG2(34, s_2);
	}
	return 0;
}

void CGenCF1::CreateLinks(vector<int> &cc) {
	int prev_note = -1;
	int lpos = 0;
	int l = 0;
	minl = 10000;
	maxl = 0;
	// Set first steps in case there is pause
	for (int i = 0; i < fn; ++i) bli[i] = 0;
	for (int i = fn; i < ep2; ++i) {
		if (prev_note != cc[i]) {
			// Save linked note length
			if (prev_note != -1) {
				llen[lpos - 1] = l;
				if (minl > l) minl = l;
				if (maxl < l) maxl = l;
				l = 0;
			}
			prev_note = cc[i]; 
			fli[lpos] = i;
			++lpos;
		}
		fli2[lpos - 1] = i;
		bli[i] = lpos-1;
		l++;
	}
	fli_size = lpos;
	llen[lpos - 1] = l;
	// Last note does not affect minl/maxl
}

void CGenCF1::CountFillInit(vector<int> &c, int tail_len, int pre, int &t1, int &t2, int &fill_end) {
	// Create leap tail
	tc.clear();
	if (pre) {
		int pos1 = fleap_start - 1;
		int pos2 = max(fleap_start - tail_len, 0);
		if (c[leap_end] > c[leap_start]) {
			for (int i = pos1; i >= pos2; --i) {
				tc.push_back(128 - c[fli2[i]]);
			}
			t1 = 128 - c[leap_end];
			t2 = 128 - c[leap_start];
		}
		else {
			for (int i = pos1; i >= pos2; --i) {
				tc.push_back(c[fli2[i]]);
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
				tc.push_back(c[fli2[i]]);
			}
			t1 = c[leap_start];
			t2 = c[leap_end];
		}
		else {
			for (int i = pos1; i <= pos2; ++i) {
				tc.push_back(128 - c[fli2[i]]);
			}
			t1 = 128 - c[leap_start];
			t2 = 128 - c[leap_end];
		}
	}
	for (int x = t1; x <= t2; ++x) nstat3[x] = 0;
	fill_end = -1;
}
	
void CGenCF1::CountFill(vector<int> &c, int tail_len, vector<int> &nstat2, vector<int> &nstat3, int &skips, int &fill_to, int pre, int &fill_to_pre, int &fill_from_pre, int &fill_from, int &deviates, int &dev_count, int leap_prev, int &fill_end)
{
	int t1, t2;
	int cur_deviation = 0;
	int dev_state = 0;
	int max_deviation = 0;
	if (accept[42 + leap_id]) max_deviation = 1;
	if (accept[120 + leap_id]) max_deviation = 2;
	CountFillInit(c, tail_len, pre, t1, t2, fill_end);
	// Detect fill_end
	deviates = 0;
	// Deviation state: 0 = before deviation, 1 = in deviation, 2 = after deviation, 3 = multiple deviations
	for (int x = 0; x < tc.size(); ++x) {
		// If deviating, start deviation state and calculate maximum deviation
		if (tc[x] > t2) {
			cur_deviation = tc[x] - t2;
			// Detect late deviation
			if (cur_deviation == 1 && x >= dev_late2 && !accept[191]) break;
			if (cur_deviation == 2 && x >= dev_late3 && !accept[192]) break;
			if (cur_deviation > deviates) {
				// If deviation is unacceptable, break leap compensation
				if (cur_deviation > max_deviation) break;
				// Save deviation for later use if it is acceptable
				deviates = cur_deviation;
			}
			if (dev_state == 0) dev_state = 1;
			else if (dev_state == 2) {
				break;
			}
		}
		// If not deviating, switch deviation state
		else {
			if (dev_state == 1) dev_state = 2;
		}
		// Break leap compensation if we are below the border
		if (tc[x] < t1) break;
		// Increment leap compensation window
		fill_end = x;
	}
	// Add middle note as compensation note if leap is compound
	if (leap_mid) {
		// Convert note to tc
		if (pre) {
			if (c[leap_end] > c[leap_start]) ++nstat3[128 - c[leap_mid]];
			else ++nstat3[c[leap_mid]];
		}
		else {
			if (c[leap_end] > c[leap_start]) ++nstat3[c[leap_mid]];
			else ++nstat3[128 - c[leap_mid]];
		}
	}
	// Calculate fill vector
	for (int x = 0; x <= fill_end; ++x) {
		++nstat3[tc[x]];
	}
	// Get deviations count
	if (dev_state == 0) dev_count = 0;
	else dev_count = 1;

	CountFillSkips(leap_prev, skips, t1, t2);
	CountFillLimits(c, pre, t1, t2, fill_to, fill_to_pre, fill_from_pre, fill_from);
}

void CGenCF1::CountFillSkips(int leap_prev, int &skips, int t1, int t2) {
	skips = 0;
	for (int x = t1 + 1; x < t2; ++x) if (!nstat3[x]) {
		++skips;
	}
}

void CGenCF1::CountFillLimits(vector<int> &c, int pre, int t1, int t2, int &fill_to, int &fill_to_pre, int &fill_from_pre, int &fill_from) {
	fill_to = leap_size;
	fill_to_pre = 0;
	fill_from_pre = 0;
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
	// Check prepared fill to
	if (!pre && fill_to > 1) {
		int pos = max(0, fleap_start - fill_to*2 + 1);
		vector<int> nstat4;
		nstat4.resize(2, 0);
		if (c[leap_start] < c[leap_end]) {
			for (int x = pos; x < fleap_start; ++x) {
				if (c[fli2[x]] == c[leap_start] + 1) nstat4[0] = 1;
				else if (c[fli2[x]] == c[leap_start] + 2) nstat4[1] = 1;
			}
		}
		else {
			for (int x = pos; x < fleap_start; ++x) {
				if (c[fli2[x]] == c[leap_start] - 1) nstat4[0] = 1;
				else if (c[fli2[x]] == c[leap_start] - 2) nstat4[1] = 1;
			}
		}
		if (fill_to == 2) {
			if (nstat4[0]) fill_to_pre = 1;
		}
		else if (fill_to == 3) {
			if (nstat4[0] && nstat4[1]) fill_to_pre = 1;
		}
	}
	// Check prepared fill from
	if (!pre && fill_from > 1) {
		int pos = max(0, fleap_start - fill_from*2 + 1);
		vector<int> nstat4;
		nstat4.resize(2, 0);
		if (c[leap_start] < c[leap_end]) {
			for (int x = pos; x < fleap_start; ++x) {
				if (c[fli2[x]] == c[leap_end] - 1) nstat4[0] = 1;
				else if (c[fli2[x]] == c[leap_end] - 2) nstat4[1] = 1;
			}
		}
		else {
			for (int x = pos; x < fleap_start; ++x) {
				if (c[fli2[x]] == c[leap_end] + 1) nstat4[0] = 1;
				else if (c[fli2[x]] == c[leap_end] + 2) nstat4[1] = 1;
			}
		}
		if (fill_from == 2) {
			if (nstat4[0]) fill_from_pre = 1;
		}
		else if (fill_from == 3) {
			if (nstat4[0] && nstat4[1]) fill_from_pre = 1;
		}
	}
}

void CGenCF1::FailLeapInit(vector<int> &c, int &late_leap, int &presecond, int &leap_next, int &leap_prev, int &arpeg, int &overflow, vector<int> &leap) {
	presecond = 0; // If leap has a filled second
	leap_next = 0; // Multiply consecutive leaps
	leap_prev = 0; // Multiply consecutive leaps
	overflow = 0; // Leap back overflow
	arpeg = 0; // Arpeggio 3+3
	// Check if this leap is 3rd
	leap_start = s; // First step of leap
	leap_end = fli2[ls + 1]; // Last step of leap
	leap_mid = 0;
	fleap_start = ls;
	fleap_end = ls + 1;
	leap_size = abs(c[leap_end] - c[s]);
										// Next is leap?
	if (fleap_end < fli_size - 1) leap_next = leap[leap_start] * leap[leap_end];
	// Prev is leap?
	if (fleap_start > 0) leap_prev = leap[leap_start] * leap[fli2[fleap_start] - 1];
	// Late leap?
	late_leap = fli_size - fleap_start;
}

int CGenCF1::FailLeapMulti(int leap_next, int &arpeg, int &overflow, int &child_leap, vector<int> &c, vector<int> &leap) {
	child_leap = 0; // If we have a child_leap
	// Check if leap is third
	if (leap_size == 2) {
		// Check if leap is second third
		if (fleap_start > 0 && abs(c[leap_end] - c[fli2[fleap_start-1]]) == 4) {
			// Set middle leap note
			leap_mid = leap_start;
			// Set leap start to first note of first third
			--fleap_start;
			leap_start = fli2[fleap_start];
			// Set leap size to be compound
			leap_size = 4;
			// If 6/8 goes before 2 thirds (tight)
			if ((fleap_start > 0) && ((leap[leap_start] * (c[leap_start] - c[fli2[fleap_start-1]]) == -5) || 
				(leap[leap_start] * (c[leap_start] - c[fli2[fleap_start - 1]]) == -7))) FLAG2(28, leap_start)
				// Else mark simple 2x3rds
			else FLAG2(6, leap_start);
		}
	}			
	leap_id = min(leap_size - 2, 3);
	if (fleap_end < fli_size - 1) {
		// Next leap in same direction
		if (leap_next > 0) {
			// Flag if greater than two thirds
			if (abs(c[fli2[fleap_end+1]] - c[leap_start]) > 4) FLAG2(27, leap_end)
				// Allow if both thirds, without flags (will process next cycle)
			else arpeg=1;
		}
		// Next leap back
		else if (leap_next < 0) {
			int leap_size2 = abs(c[fli2[fleap_end + 1]] - c[leap_end]);
			// Flag if back leap greater than 6th
			if (leap_size2 > 5) FLAG2(22, leap_end)
				// Flag if back leap equal or smaller than 6th
			else FLAG2(8, leap_end);
			// Flag leap back overflow
			if (leap_size2 > leap_size) {
				FLAG2(58, leap_end);
				overflow = 1;
			}
		}
	}
	// Check if we have a greater neighbouring leap
	if ((fleap_end < fli_size - 1 && abs(c[fli2[fleap_end + 1]] - c[leap_end]) >= leap_size-1 && leap[leap_start] * leap[leap_end]<0) ||
		(fleap_start > 0 && abs(c[leap_start] - c[fli2[fleap_start - 1]]) > leap_size && leap[leap_start] * leap[fli2[fleap_start - 1]]<0)) {
		// Set that we are preleaped (even if we are postleaped)
		child_leap = 1;
	}
	return 0;
}

int CGenCF1::FailLeap(vector<int> &c, vector<int> &leap, vector<int> &smooth, vector<int> &nstat2, vector<int> &nstat3)
{
	// Get leap size, start, end
	// Check if leap is compensated (without violating compensation rules)
	// If leap is not compensated, check uncompensated rules
	// If uncompensated rules not allowed, flag compensation problems detected (3rd, etc.)
	int child_leap, leap_next, leap_prev, presecond;
	int overflow, arpeg, late_leap;
	for (s = fn; s < ep2 - 1; ++s) {
		if (leap[s] != 0) {
			ls = bli[s];
			FailLeapInit(c, late_leap, presecond, leap_next, leap_prev,
				arpeg, overflow, leap);
			if (FailLeapMulti(leap_next, arpeg, overflow, child_leap, c, leap)) return 1;
			// If leap back overflow or arpeggio, do not check leap compensation, because compensating next leap will be enough
			if (!overflow && !arpeg)
				if (FailLeapFill(c, late_leap, leap_prev, child_leap)) return 1;
			if (!arpeg) if (FailLeapMDC(leap, c)) return 1;
		}
	}
	return 0;
}

int CGenCF1::FailLeapFill(vector<int> &c, int late_leap, int leap_prev, int child_leap) {
	// Do not check fill if this is first third and rule is allowed
	if (fleap_start == 0 && leap_size == 2 && accept[1]) {
		FLAG2(1, leap_start);
		return 0;
	}
	// Prefill parameters
	int ptail_len, pfill_to, pfill_to_pre, pfill_from_pre, pfill_from, pdeviates, pfill_end, pdev_count;
	// Fill parameters
	int tail_len, fill_to, fill_to_pre, fill_from_pre, fill_from, deviates, fill_end, dev_count;
	filled = 0;
	prefilled = 0;
	int pskips = 10;
	int skips = 10;
	// Calculate allowed skips 
	int allowed_skips = 0;
	if (leap_size > 2) ++allowed_skips;
	if (leap_size > 6) ++allowed_skips;
	if (late_leap <= c4p_last_leaps + 1) ++allowed_skips;
	int allowed_pskips = 0;
	if (leap_size > 2) ++allowed_pskips;
	if (leap_size > 6) ++allowed_pskips;
	// Check if leap is filled
	tail_len = 2 + (leap_size - 1) * fill_steps_mul;
	// Do not check fill if search window is cut by end of current not-last scan window
	if ((fleap_end + tail_len < fli_size) || (c_len == ep2)) {
		filled = 1;
		// Check fill only if enough length (checked second time in case of slurs)
		CountFill(c, tail_len, nstat2, nstat3, skips, fill_to, 0, fill_to_pre, fill_from_pre,
			fill_from, deviates, dev_count, leap_prev, fill_end);
		if (skips > allowed_skips) filled = 0;
		else if (fill_to > 3) filled = 0;
		else if (fill_to == 3 && (!fill_to_pre || late_leap > c4p_last_leaps + 1 || !accept[144 + leap_id])) filled = 0;
		else if (fill_to == 2 && fill_to_pre && !accept[100 + leap_id]) filled = 0;
		else if (fill_to == 2 && !fill_to_pre && !accept[104 + leap_id]) filled = 0;
		else if (fill_from > 3) filled = 0;
		else if (fill_from == 3 && (!fill_from_pre || late_leap > c4p_last_leaps + 1 || !accept[144 + leap_id])) filled = 0;
		else if (fill_from == 2 && !accept[53 + leap_id]) filled = 0;
		else if (deviates > 2) filled = 0;
		else if (deviates == 1 && !accept[42 + leap_id]) filled = 0;
		else if (deviates == 2 && !accept[120 + leap_id]) filled = 0;
		if (!filled) {
			if (child_leap && accept[116 + leap_id]) FLAG2(116 + leap_id, leap_start)
			// Check if  leap is prefilled
			else {
				if (ls > 0) {
					ptail_len = 2 + (leap_size - 1) * fill_steps_mul;
					CountFill(c, ptail_len, nstat2, nstat3, pskips, pfill_to, 1, pfill_to_pre, pfill_from_pre, pfill_from,
						pdeviates, pdev_count, leap_prev, pfill_end);
					prefilled = 1;
					if (pskips > allowed_pskips) prefilled = 0;
					else if (pfill_to > 2) prefilled = 0;
					else if (pfill_to == 2 && !accept[104 + leap_id]) prefilled = 0;
					else if (pfill_from > 2) prefilled = 0;
					else if (pfill_from == 2 && !accept[53 + leap_id]) prefilled = 0;
					else if (pdeviates > 2) prefilled = 0;
					else if (pdeviates == 1 && !accept[42 + leap_id]) prefilled = 0;
					else if (pdeviates == 2 && !accept[120 + leap_id]) prefilled = 0;
				}
				if (prefilled) {
					if (late_leap <= pre_last_leaps + 1) FLAG2(204 + leap_id, leap_start)
					else FLAG2(112 + leap_id, leap_start)
				}
				else FLAG2(124 + leap_id, leap_start);
			}
		}
		// Show compensation flags only if successfully compensated
		// This means that compensation errors are not shown if uncompensated (successfully or not)
		else {
			// Flag late uncompensated precompensated leap
			if (fill_to == 3 && late_leap <= c4p_last_leaps + 1) FLAG2(144 + leap_id, leap_start)
			else if (fill_from == 3 && late_leap <= c4p_last_leaps + 1) FLAG2(144 + leap_id, leap_start)
			// Flag unfinished fill if it is not blocking
			else if (fill_to == 2 && fill_to_pre) FLAG2(100 + leap_id, leap_start)
			// Flag prepared unfinished fill if it is not blocking
			else if (fill_to == 2 && !fill_to_pre) FLAG2(104 + leap_id, leap_start);
			// Flag after 3rd if it is not blocking
			if (fill_from == 2) FLAG2(53 + leap_id, leap_start);
			// Flag deviation if it is not blocking
			if (deviates == 1) FLAG2(42 + leap_id, leap_start);
			// Flag deviation if it is not blocking
			if (deviates == 2) FLAG2(120 + leap_id, leap_start);
		}
	}
	return 0;
}

int CGenCF1::FailLeapMDC(vector<int> &leap, vector<int> &c) {
	// Melody direction change (MDC)
	// 0 = close, 1 = far, 2 = no
	// Default mdc is close, because beginning equals to close mdc
	mdc1 = 0;
	int prev_note = c[leap_start];
	for (int pos = leap_start - 1; pos >= 0; --pos) {
		if (c[pos] != prev_note) {
			// Check if direction changes or long without changes
			if (leap[leap_start] * (c[pos] - prev_note) > 0 || mdc1 > 1) break;
			prev_note = c[pos];
			++mdc1;
		}
	}
	mdc2 = 0;
	prev_note = c[leap_end];
	for (int pos = leap_end + 1; pos < ep2; ++pos) {
		if (c[pos] != prev_note) {
			// Check if direction changes or long without changes
			if (leap[leap_start] * (c[pos] - prev_note) < 0 || mdc2 > 2) break;
			prev_note = c[pos];
			++mdc2;
		}
	}
	// Close + 1far
	if (mdc1 == 0 && mdc2 == 1) FLAG2(128 + leap_id, leap_start)
		// Close + 2far
	else if (mdc1 == 0 && mdc2 == 2) FLAG2(140 + leap_id, leap_start)
		// No + close
	else if (mdc1 == 2 && mdc2 == 0) FLAG2(132 + leap_id, leap_start)
		// Far + close
	else if (mdc1 == 1 && mdc2 == 0) FLAG2(59 + leap_id, leap_start)
		// Far + far1
	else if (mdc1 == 1 && mdc2 == 1) FLAG2(63 + leap_id, leap_start)
	// No close
	else if (mdc1*mdc2 != 0) {
		if (filled || prefilled) FLAG2(136 + leap_id, leap_start)
		else FLAG2(148 + leap_id, leap_start);
	}
	// Close + no
	else if (!mdc1 && mdc2 == 3) FLAG2(108 + leap_id, leap_start);
	return 0;
}

// Check tritone t1-t2 which has to resolve from ta to tb
int CGenCF1::FailTritone(int ta, int t1, int t2, int tb, vector<int> &c, vector<int> &cc, vector<int> &pc, vector<int> &pcc) {
	int found;
	int res1 = 0; // First note resolution flag
	int res2 = 0; // Second note resolution flag
	int ta2, tb2; // Real resolution notes
	// Tritone prohibit
	leap_start = s;
	found = 0;
	// Check consecutive tritone
	if ((pcc[s1] == t2 && pcc[s] == t1) || (pcc[s1] == t1 && pcc[s] == t2)) found = 1;
	// Check tritone with additional note inside
	if (ls > 0) {
		// Check pitches
		if ((pcc[s1] == t2 && pcc[s_1] == t1) || (pcc[s1] == t1 && pcc[s_1] == t2))
			// Check intermediate note and mdc
			if ((c[s] > c[s1] && c[s] < c[s_1] && (ls<2 || c[s_2] < c[s_1]) && (ls>fli_size - 3 || c[s2] > c[s1])) ||
				(c[s] < c[s1] && c[s] > c[s_1] && (ls<2 || c[s_2] > c[s_1]) && (ls>fli_size - 3 || c[s2] < c[s1]))) {
				found = 2;
				leap_start = s_1;
			}
	}
	fleap_start = bli[leap_start];
	fleap_end = bli[s1];
	// Do not check tritone if it is at the end of not-last window
	if (ls == fli_size - 2 && ep2 != c_len) return 0;
	if (found) {
		// Check if tritone is highest leap if this is last window
		if (ep2 == c_len) {
			if ((cc[leap_start] == nmax) || (cc[s1] == nmax)) FLAG2(32, s);
		}
		// Check if resolution is correct
		// Get real resolution notes
		if (pcc[s1] == t2) {
			ta2 = ta;
			tb2 = tb;
		}
		else {
			ta2 = tb;
			tb2 = ta;
		}
		if (fleap_start > 0 && pcc[fli[fleap_start - 1]] == ta2) res1 = 1;
		else if (pcc[fli[fleap_start + 1]] == ta2) res1 = 1;
		if (fleap_end < fli_size - 1 && pcc[fli[fleap_end + 1]] == tb2) res2 = 1;
		else if (pcc[fli[fleap_end - 1]] == tb2) res2 = 1;
		// Flag resolution for consecutive tritone
		if (found == 1) {
			if (res1*res2 == 0) FLAG2(31, s)
			else FLAG2(2, s);
		}
		// Flag resolution for tritone with intermediate note
		else {
			if (res1*res2 == 0) FLAG2(19, s)
			else FLAG2(18, s);
		}
	}
	return 0;
}

int CGenCF1::FailTonic(vector<int> &cc, vector<int> &pc) {
	int tcount = 0;
	// Do not check if melody is short
	if (fli_size < 3) return 0;
	// Loop from second to second to last note
	for (ls = 1; ls < fli_size-1; ++ls) {
		s = fli2[ls];
		s_1 = fli2[ls-1];
		// Decrement for previous tonic note
		if (ls > tonic_window) {
			if (!pc[fli2[ls - tonic_window]]) {
				if (ls > tonic_window+1 && abs(cc[fli2[ls - tonic_window]] - cc[fli2[ls - tonic_window - 1]]) > tonic_leap) tcount -= tonic_leap_weight;
				else --tcount;
			}
		}
		if (!pc[s]) {
			// Increment for current tonic note
			if (abs(cc[s] - cc[s_1]) > tonic_leap) tcount += tonic_leap_weight;
			else ++tcount;
			// Check count of tonic notes
			if (tcount > tonic_max) FLAG2(196, s);
		}
	}
	return 0;
}

int CGenCF1::FailIntervals(vector<int> &c, vector<int> &cc, vector<int> &pc, vector<int> &pcc)
{
	for (ls = 0; ls < fli_size - 1; ++ls) {
		s = fli2[ls];
		s1 = fli2[ls + 1];
		if (ls > 0) s_1 = fli2[ls - 1];
		if (ls > 1) s_2 = fli2[ls - 2];
		if (ls < fli_size - 2) s2 = fli2[ls + 2];
		// Warning: tritone F#C in minor is not detected (can add FailTritone to detect) because it is already prohibited by Unaltered near altered.
		// If you allow Unaltered near altered, you should implement FailTritone for F#C.
		if (minor_cur) {
			if (FailTritone(3, 5, 11, 0, c, cc, pc, pcc)) return 1;
			if (FailTritone(7, 8, 2, 3, c, cc, pc, pcc)) return 1;
		}
		else {
			if (FailTritone(4, 5, 11, 0, c, cc, pc, pcc)) return 1;
		}
		// Leap size prohibit
		if (cc[s1] - cc[s] == 8) FLAG2(175, s)
		else if (cc[s1] - cc[s] == -8) FLAG2(181, s)
		else if (cc[s1] - cc[s] == 9) FLAG2(176, s)
		else if (cc[s1] - cc[s] == -9) FLAG2(182, s)
		else if (cc[s1] - cc[s] == 10) FLAG2(177, s)
		else if (cc[s1] - cc[s] == -10) FLAG2(183, s)
		else if (cc[s1] - cc[s] == 11) FLAG2(178, s)
		else if (cc[s1] - cc[s] == -11) FLAG2(184, s)
		else if (cc[s1] - cc[s] == 12) FLAG2(179, s)
		else if (cc[s1] - cc[s] == -12) FLAG2(185, s)
		else if (cc[s1] - cc[s] > 12) FLAG2(180, s)
		else if (cc[s1] - cc[s] < -12) FLAG2(186, s)
	}
	return 0;
}

// Calculate global fill
int CGenCF1::FailGlobalFill(vector<int> &c, vector<int> &nstat2)
{
	// Clear nstat
	for (int i = nmind; i <= nmaxd; ++i) nstat2[i] = 0;
	// Count nstat
	for (int x = fn; x < ep2; ++x) ++nstat2[c[x]];
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
	if (skips2) FLAG2(69, fn);
	if (skips == 1) FLAG2(67, fn)
	else if (skips == 2) FLAG2(68, fn);
	return 0;
}


void CGenCF1::ScanInit() {
	if (!is_animating) {
		scan_start_time = time();
		anflags.resize(av_cnt);
		anflagsc.resize(av_cnt);
		for (int i = 0; i < av_cnt; ++i) {
			anflags[i].resize(c_len, vector<int>(MAX_RULES)); // Flags for each note
			anflagsc[i].resize(c_len); // number of flags for each note
		}
		uli.resize(c_len);
		fli.resize(c_len);
		fli2.resize(c_len);
		macc.resize(c_len);
		macc2.resize(c_len);
		decc.resize(c_len);
		decc2.resize(c_len);
		llen.resize(c_len);
		cc_order.resize(c_len);
		dpenalty_step.resize(c_len);
		cc_id.resize(c_len);
		bli.resize(c_len);
		fpenalty.resize(max_flags);
		wpos1.resize(c_len / s_len + 1);
		wpos2.resize(c_len / s_len + 1);
		min_c.resize(c_len);
		max_c.resize(c_len);
		min_cc.resize(c_len);
		max_cc.resize(c_len);
		hm.resize(c_len);
		hm2.resize(c_len);
		accepted4.resize(MAX_WIND); // number of accepted canti per window
		accepted5.resize(MAX_WIND); // number of canti with neede flags per window
		flags.resize(MAX_RULES); // Flags for whole cantus
		fstat.resize(MAX_RULES); // number of canti with each flag
		fcor.resize(MAX_RULES, vector<long long>(MAX_RULES)); // Flags correlation matrix
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
		// Initialize fblock if calculation is needed
		if (calculate_blocking) {
			fblock = vector<vector<vector<long>>>(MAX_WIND, vector<vector<long>>(max_flags, vector<long>(max_flags)));
		}
		// Init best rejected results
		if (best_rejected) {
			rcycle = 0;
			accept_time = CGLib::time();
			if (method == mScan) rpenalty_min = MAX_PENALTY;
		}
		for (int x = fn; x < c_len; ++x) {
			hm[x].resize(3);
			hm2[x].resize(3);
		}
	}
	// Can we skip flags?
	skip_flags = !calculate_blocking && !calculate_correlation && !calculate_stat;
}

void CGenCF1::ScanCantusInit() {
	// Get cantus size
	if (task != tGen) c_len = scantus->size();
	ScanInit();
	// Resize global vectors
	m_c.resize(c_len); // cantus (diatonic)
	m_cc.resize(c_len); // cantus (chromatic)
	m_cc_old.resize(c_len); // Cantus diatonic saved for SWA
	m_pc.resize(c_len);
	m_pcc.resize(c_len);
	m_leap.resize(c_len);
	m_smooth.resize(c_len);
	m_slur.resize(c_len);
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
	if (src_nmax > MAX_NOTE) return;
	// Decrease current range if it is bigger
	if (minc < src_nmin) minc = src_nmin;
	if (maxc > src_nmax) maxc = src_nmax;
}

void CGenCF1::SingleCantusInit() {
	// Copy cantus
	m_cc = *scantus;
	// Get diatonic steps from chromatic
	first_note = m_cc[0];
	last_note = m_cc[c_len - 1];
	for (int i = fn; i < c_len; ++i) {
		m_c[i] = CC_C(m_cc[i], tonic_cur, minor_cur);
		// Save value for future use;
		m_cc_old[i] = m_cc[i];
	}
	if (!swa_inrange) {
		GetRealRange(m_c, m_cc);
		ApplySourceRange();
	}
	// Set pitch limits
	// If too wide range is not accepted, correct range to increase scan performance
	if (!accept[37]) {
		for (int i = fn; i < c_len; ++i) {
			min_cc[i] = max(minc, m_cc[i] - correct_range);
			max_cc[i] = min(maxc, m_cc[i] + correct_range);
		}
	}
	else {
		for (int i = fn; i < c_len; ++i) {
			min_cc[i] = m_cc[i] - correct_range;
			max_cc[i] = m_cc[i] + correct_range;
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
		CalculateCcOrder(m_cc_old, 0, c_len);
		// End of evaluation window
		if (method == mScan) {
			ep2 = GetMaxSmap() + 1;
			if (sp2 == smatrixc) ep2 = c_len;
			// Clear scan steps
			FillCantusMap(cc_id, smap, 0, smatrixc, 0);
			FillCantusMap(m_cc, smap, 0, smatrixc, cc_order);
			// Can skip flags - full scan must remove all flags
		}
		// For sliding windows algorithm evaluate whole melody
		if (method == mSWA) {
			ep2 = c_len;
			// Cannot skip flags - need them for penalty if cannot remove all flags
			skip_flags = 0;
			// Clear scan steps of current window
			FillCantusMap(cc_id, smap, sp1, sp2, 0);
			FillCantusMap(m_cc, smap, sp1, sp2, cc_order);
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

// Step2 must be exclusive
void CGenCF1::RandCantus(vector<int>& c, vector<int>& cc, int step1, int step2)
{
	for (int i = step1; i < step2; ++i) { //-V756
		for (int x = 0; x < 1000; ++x) {
			c[i] = randbw(min_c[i], max_c[i]);
			// Convert to chromatic
			cc[i] = C_CC(c[i], tonic_cur, minor_cur);
			// Prevent note repeats in the starting cantus
			if ((i == 0 || cc[i] != cc[i - 1]) && (i > c_len - 2 || cc[i] != cc[i + 1])) break;
		}
	}
}

void CGenCF1::CalculateCcOrder(vector <int> &cc_old, int step1, int step2) {
	int x, x2;
	if (task == tCor) {
		int finished;
		// Fill notes starting with source melody, gradually moving apart
		for (int i = step1; i < step2; ++i) {
			cc_order[i].clear();
			// Send first note
			x = cc_old[i];
			if (x < min_cc[i]) x = min_cc[i];
			if (x > max_cc[i]) x = max_cc[i];
			cc_order[i].push_back(x);
			x2 = x;
			for (int z = 1; z < 1000; ++z) {
				finished = 1;
				x += cc_incr[x];
				x2 -= cc_decr[x2];
				if (rand() > RAND_MAX / 2) {
					if (x <= max_cc[i]) {
						cc_order[i].push_back(x);
						finished = 0;
					}
					if (x2 >= min_cc[i]) {
						cc_order[i].push_back(x2);
						finished = 0;
					}
				}
				else {
					if (x2 >= min_cc[i]) {
						cc_order[i].push_back(x2);
						finished = 0;
					}
					if (x <= max_cc[i]) {
						cc_order[i].push_back(x);
						finished = 0;
					}
				}
				if (finished) break;
			}
		}
	}
	else {
		// Fill consecutive notes
		for (int i = step1; i < step2; ++i) {
			cc_order[i].clear();
			x = min_cc[i];
			while (x <= max_cc[i]) {
				cc_order[i].push_back(x);
				x += cc_incr[x];
			}
			// Shuffle
			if (random_seed)
				random_shuffle(cc_order[i].begin(), cc_order[i].end());
		}
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
	for (int i = fn; i < c_len; ++i) {
		min_cc[i] = minc;
		max_cc[i] = maxc;
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
	CalculateCcOrder(m_cc_old, 0, c_len);
	FillCantus(cc_id, 0, c_len, 0);
	FillCantus(cc, 0, c_len, cc_order);
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
	ep1 = max(0, sp1 - 1);
	ep2 = sp2; // End of evaluation window
	p = sp2 - 1; // Minimal position in array to cycle
}

// Calculate flag statistics
void CGenCF1::CalcFlagStat() {
	if (calculate_stat || calculate_correlation) {
		for (int i = 0; i < max_flags; ++i) if (!accept[i]) {
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
		WriteLog(5, est);
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

// Check if rpenalty is not below than flags sum
void CGenCF1::TestRpenalty() {
	if (!flags.size()) return;
	int rp = 0;
	int found;
	//CString st, st2;
	for (int z = 0; z < max_flags; ++z) {
		if (!accept[z] && flags[z]) {
			rp += (severity[z] + 1) * flags[z] + fpenalty[z];
			found = 0;
			for (int x = fn; x < ep2; ++x) {
				if (anflagsc[cpv][x] > 0) for (int i = 0; i < anflagsc[cpv][x]; ++i) if (anflags[cpv][x][i] == z) {
					found = 1;
				}
			}
			if (!found) {
				CString est;
				est.Format("Error: not found flag %d (which is set to %d) in anflags vector",
					z, flags[z]);
				WriteLog(5, est);
			}
		}
	}
	if (rpenalty_cur < rp) {
		CString est;
		est.Format("Error: rpenalty_cur %.0f is below rpenalty evaluation based on flags (%d)",
			rpenalty_cur, rp);
		WriteLog(5, est);
	}
}

// Check if rpenalty is not below than flags sum
void CGenCF1::TestBestRpenalty() {
	// Do not test if rpenalty == 0, because in this case best_flags are not saved
	if (!rpenalty_min) return;
	if (!best_flags.size()) return;
	int rp = 0;
	//CString st, st2;
	for (int x = 0; x < max_flags; ++x) {
		if (!accept[x] && best_flags[x]) rp += severity[x] + 1;
	}
	if (rpenalty_min < rp) {
		CString est;
		est.Format("Error: rpenalty_min %.0f is below rpenalty evaluation based on best_flags (%d)",
			rpenalty_min, rp);
		WriteLog(5, est);
	}
}

void CGenCF1::CalcRpenalty(vector<int> &cc) {
	// Calculate out of range penalty
	int real_range = nmax - nmin;
	if (!accept[37] && real_range > max_interval) {
		int nminr = nmin + (real_range - max_interval) / 2;
		int nmaxr = nminr + max_interval;
		for (int i = fn; i < ep2; ++i) {
			if (cc[i] < nminr) fpenalty[37] += nminr - cc[i];
			if (cc[i] > nmaxr) fpenalty[37] += cc[i] - nmaxr;
		}
	}
	// Calculate flags penalty
	rpenalty_cur = 0;
	for (int x = fn; x < ep2; ++x) {
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
		if (cc_id[p] < cc_order[p].size() - 1) break;
		// If current element is max, make it minimum
		cc_id[p] = 0;
		cc[p] = cc_order[p][0];
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

// Show best rejected variant
void CGenCF1::ShowBestRejected(vector<int> &cc) {
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
				vector<int> cc_saved = cc;
				// Load best rejected cantus
				cc = br_cc;
				flags = br_f;
				anflags[cpv] = br_nf;
				anflagsc[cpv] = br_nfc;
				chm.clear();
				chm.resize(c_len, -1);
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
}

void CGenCF1::BackWindow(vector<int> &cc) {
	if (method != mScan) return;
	if (task == tCor) {
		// Clear current window
		FillCantusMap(cc_id, smap, sp1, sp2, 0);
		FillCantusMap(cc, smap, sp1, sp2, cc_order);
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
		FillCantus(cc_id, sp1, sp2, 0);
		FillCantus(cc, sp1, sp2, cc_order);
		// If this is not first window, go to previous window
		if (wid > 0) wid--;
		sp1 = wpos1[wid];
		sp2 = wpos2[wid];
		// End of evaluation window
		ep1 = max(0, sp1 - 1);
		ep2 = sp2;
		// Go to rightmost element
		p = sp2 - 1;
	}
}

int CGenCF1::CalcDpenaltyCP(vector<int> &cc1, vector<int> &cc2, int s1, int s2) {
	int dpe = 0;
	for (int z = s1; z <= s2; ++z) {
		int dif = abs(cc1[z] - cc2[z]);
		if (dif) dpe += step_penalty + pitch_penalty * dif;
	}
	return dpe;
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
	FillCantusMap(cc_id, smap, sp1, sp2, 0);
	FillCantusMap(cc, smap, sp1, sp2, cc_order);
	// Prepare dpenalty
	if (av_cnt == 2) {
		dpenalty_outside_swa = CalcDpenaltyCP(cpoint[cantus_id][cpv], acc[cpv], 0, sp1 - 1) +
			CalcDpenaltyCP(cpoint[cantus_id][cpv], acc[cpv], sp2, c_len - 1);
	}
	else {
		// TODO implement for CF1
	}
	return 0;
}

void CGenCF1::SaveBestRejected(vector<int> &cc) {
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

int CGenCF1::FailMinor(vector<int> &pcc, vector<int> &cc) {
	for (int x = 1; x < fli_size; ++x) {
		s = fli2[x];
		s_1 = fli2[x - 1];
		// Prohibit leap to VI#
		if (pcc[s] == 9 && abs(cc[s] - cc[s_1]) > fis_leap) FLAG2(201, s);
		// Prohibit minor second up before VII - absorbed
		// Prohibit augmented second up before VII - absorbed
		// Prohibit unaltered VI or VII two steps from altered VI or VII
		if (pcc[s] == 11) {
			if (pcc[s_1] == 10) FLAG2(153, s_1);
			if (pcc[s_1] == 8) FLAG2(154, s_1);
			if (pcc[s_1] == 3) FLAG2(157, s_1);
			if (x > 1) {
				s_2 = fli2[x - 2];
				if (pcc[s_2] == 10) FLAG2(159, s_2);
				if (pcc[s_2] == 8) FLAG2(160, s_2);
				if (pcc[s_2] == 3) FLAG2(163, s_2);
			}
			if (x < fli_size - 1) {
				s1 = fli2[x+1];
				if (pcc[s1] == 10) FLAG2(153, s1);
				if (pcc[s1] == 8) FLAG2(154, s1);
				if (pcc[s1] == 3) FLAG2(156, s1);
				if (x < fli_size - 2) {
					s2 = fli2[x+2];
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
				s_2 = fli2[x - 2];
				if (pcc[s_2] == 8) FLAG2(158, s_2);
				if (pcc[s_2] == 3) FLAG2(161, s_2);
			}
			if (x < fli_size - 1) {
				s1 = fli2[x + 1];
				if (pcc[s1] == 8) FLAG2(152, s1);
				if (pcc[s1] == 3) FLAG2(155, s1);
				if (x < fli_size - 2) {
					s2 = fli2[x + 2];
					if (pcc[s2] == 8) FLAG2(158, s2);
					if (pcc[s2] == 3) FLAG2(161, s2);
				}
			}
		}
		// Prohibit unresolved minor tritone DG# (direct or with inserted note)
	}
	return 0;
}

void CGenCF1::ShowScanStatus() {
	if (task == tEval) return;
	CString st, progress_st;
	if (task == tGen && method == mScan) {
		for (int i = 0; i < wcount; ++i) {
			st.Format("%d ", cc_id[wpos1[i]]);
			progress_st += st;
		}
	}
	else if (task == tCor && method == mScan) {
		for (int i = 0; i < smatrixc; ++i) {
			st.Format("%d ", cc_id[smap[i]]);
			progress_st += st;
		}
	}
	else if (method == mSWA) {
		for (int i = 0; i < smatrixc; ++i) {
			st.Format("%d ", cc_id[smap[i]]);
			progress_st += st;
		}
	}
	int scan_time;
	if (task == tCor) scan_time = (time() - correct_start_time) / 1000;
	else scan_time = (time() - scan_start_time) / 1000;
	if (!progress_st.IsEmpty()) SetStatusText(2, progress_st + "(Scan progress)");
	if (clib.size() > 0) st.Format("CY %lld, CL %d, ST %d", cycle, clib.size(), scan_time);
	else st.Format("CY %lld, ST %d", cycle, scan_time);
	SetStatusText(5, st);
	if (task == tCor) st.Format("WI %d/%d, RP %.0f, DP %d", wid + 1, wcount, rpenalty_min, dpenalty_min);
	else st.Format("WI %d/%d", wid + 1, wcount, rpenalty_min, dpenalty_min);
	SetStatusText(1, st);
	st.Format("Sent: %ld (ignored %ld)", cantus_sent, cantus_ignored);
	SetStatusText(0, st);
}

void CGenCF1::ReseedCantus()
{
	CString st;
	MultiCantusInit(m_c, m_cc);
	// Allow two seed cycles for each accept
	seed_cycle = 0;
	++reseed_count;
	st.Format("Reseed: %d", reseed_count);
	SetStatusText(4, st);
	//CString est;
	//est.Format("Reseed: ignored cantus %d while sent=%d", cantus_ignored, cantus_sent);
	//WriteLog(1, est);
}

void CGenCF1::WriteFlagCor() {
	// Write flag correlation
	if (calculate_correlation) {
		DeleteFile("cf1-cor.csv");
		CString st, st2, st3;
		st3 = "Flag; Total; ";
		for (int i = 0; i < max_flags; ++i) {
			int f1 = i;
			st2.Format("%s; %d; ", RuleName[rule_set][f1] + " (" + SubRuleName[rule_set][f1] + ")", fcor[f1][f1]);
			st3 += RuleName[rule_set][f1] + " (" + SubRuleName[rule_set][f1] + "); ";
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
	int lines = 0;
	// Show flag statistics
	if (calculate_stat) {
		for (int d = 1; d < max_flags; ++d) {
			if (lines > 100) break;
			int flagc = 0;
			for (int x = 0; x < max_flags; ++x) {
				if (fstat[x] > 0) ++flagc;
			}
			if (!flagc) continue;
			int max_flag = 0;
			long max_value = -1;
			for (int x = 0; x < max_flags; ++x) {
				max_value = -1;
				// Find biggest value
				for (int i = 0; i < max_flags; ++i) {
					if (fstat[i] > max_value) {
						max_value = fstat[i];
						max_flag = i;
					}
				}
				if (max_value < 1) break;
				st.Format("\n%ld %s, ", max_value, RuleName[rule_set][max_flag] + " (" + SubRuleName[rule_set][max_flag] + ")");
				st2 += st;
				++lines;
				// Clear biggest value to search for next
				fstat[max_flag] = -1;
			}
		}
		CString est;
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
			st.Format("\n%ld %s, ", max_value, RuleName[rule_set][max_flag] + " (" + SubRuleName[rule_set][max_flag] + ")");
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
		if (!accept[max_flag]) {
			st.Format("\n%ld %s, ", max_value, RuleName[rule_set][max_flag] + " (" + SubRuleName[rule_set][max_flag] + ")");
			st2 += st;
		}
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
			st2.Empty();
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
					st.Format("\n%ld %s, ", max_value, RuleName[rule_set][max_flag] + " (" + SubRuleName[rule_set][max_flag] + ")");
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
	for (int z = fn; z < c_len; z++) {
		int dif = abs(m_cc_old[z] - m_cc[z]);
		if (dif) dpenalty_cur += step_penalty + pitch_penalty * dif;
	}
}

void CGenCF1::SaveCantus() {
	// If rpenalty is same as min, calculate dpenalty
	if (method == mScan || optimize_dpenalty) {
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
	clib.push_back(m_cc);
	rpenalty.push_back(rpenalty_cur);
	rpenalty_min = rpenalty_cur;
}

void CGenCF1::TransposeVector(vector<int> &v, int t) {
	for (int i = 0; i < v.size(); ++i) {
		v[i] += t;
	}
}

void CGenCF1::TransposeVector(vector<float> &v, int t) {
	for (int i = 0; i < v.size(); ++i) {
		v[i] += t;
	}
}

// Moving average
void CGenCF1::maVector(vector<float> &v, vector<float> &v2, int range) {
	int pos1, pos2;
	float ma, maw_sum;
	for (int s = 0; s < v.size(); ++s) {
		pos1 = max(0, s - range);
		pos2 = min(v.size() - 1, s + range);
		ma = 0;
		maw_sum = 0;
		for (int x = pos1; x <= pos2; ++x) if (v[x]) {
			ma += v[x];
			++maw_sum;
		}
		if (maw_sum) v2[s] = ma / maw_sum;
		else v2[s] = 0;
	}
}

void CGenCF1::maVector(vector<int> &v, vector<float> &v2, int range) {
	int pos1, pos2;
	float ma, maw_sum;
	for (int s = 0; s < v.size(); ++s) {
		pos1 = max(0, s - range);
		pos2 = min(v.size() - 1, s + range);
		ma = 0;
		maw_sum = 0;
		for (int x = pos1; x <= pos2; ++x) if (v[x]) {
			ma += v[x];
			++maw_sum;
		}
		if (maw_sum) v2[s] = ma / maw_sum;
		else v2[s] = 0;
	}
}

void CGenCF1::mawVector(vector<int> &v, vector<float> &v2, int range) {
	int pos1, pos2;
	float ma, maw_sum;
	for (int s = 0; s < v.size(); ++s) {
		pos1 = max(0, s - range);
		pos2 = min(v.size() - 1, s + range);
		ma = 0;
		maw_sum = 0;
		for (int x = pos1; x <= pos2; ++x) if (v[x]) {
			ma += maw[abs(x - s)] * v[x];
			maw_sum += maw[abs(x - s)];
		}
		if (maw_sum) v2[s] = ma / maw_sum;
		else v2[s] = 0;
	}
}

void CGenCF1::mawVector(vector<float> &v, vector<float> &v2, int range) {
	int pos1, pos2;
	float ma, maw_sum;
	for (int s = 0; s < v.size(); ++s) {
		pos1 = max(0, s - range);
		pos2 = min(v.size() - 1, s + range);
		ma = 0;
		maw_sum = 0;
		for (int x = pos1; x <= pos2; ++x) if (v[x]) {
			ma += maw[abs(x - s)] * v[x];
			maw_sum += maw[abs(x - s)];
		}
		if (maw_sum) v2[s] = ma / maw_sum;
		else v2[s] = 0;
	}
}

void CGenCF1::MakeMacc(vector<int> &cc) {
	int pos1, pos2;
	int ma_range = 2*minl;
	// Deviation weight
	maw.clear();
	for (int i = 0; i <= ma_range; ++i) {
		maw.push_back(1 - i*0.5 / ma_range);
	}
	float de, maw_sum;
	// Moving average
	mawVector(cc, macc, ma_range);
	// Smooth
	mawVector(macc, macc2, ma_range);
	// Deviation
	for (int s = fn; s < ep2; ++s) {
		pos1 = max(0, s - ma_range);
		pos2 = min(ep2 - 1, s + ma_range);
		de = 0;
		maw_sum = 0;
		for (int x = pos1; x <= pos2; ++x) if (cc[x]) {
			de += maw[abs(x-s)]*SQR(cc[x]-macc[s]);
			maw_sum += maw[abs(x - s)];
		}
		if (maw_sum) decc[s] = SQRT(de / maw_sum);
		else decc[s] = 0;
	}
	// Smooth
	mawVector(decc, decc2, ma_range);
}

void CGenCF1::InterpolateNgraph(int v, int step0, int step) {
	// Interpolate ngraph
	int pos1, pos2;
	for (int n = 0; n < ngraph_size; ++n) {
		pos1 = 0;
		pos2 = 0;
		for (int i = step0; i < step; ++i) {
			if (!ngraph[i][v][n]) {
				// Detect start
				if (!pos1 && pos2) pos1 = i;
			}
			else {
				// Detect finish
				pos2 = i;
				if (pos1) {
					// Detected start and finish
					for (int x = pos1; x < pos2; ++x) {
						ngraph[x][v][n] = (ngraph[pos1 - 1][v][n] * (pos2 - x) + ngraph[pos2][v][n] * (x - pos1 + 1)) / (pos2 - pos1 + 1);
					}
				}
				pos1 = 0;
			}
		}
	}
}

void CGenCF1::SendNgraph(int pos, int i, int v, int x) {
	if (i == (cc_len[x] - 1) / 2) {
		float ma = macc2[x];
		float de = decc2[x];
		ngraph[pos + i][v][0] = ma - de;
		ngraph[pos + i][v][1] = ma;
		ngraph[pos + i][v][2] = ma + de;
	}
	else {
		ngraph[pos + i][v][0] = 0;
		ngraph[pos + i][v][1] = 0;
		ngraph[pos + i][v][2] = 0;
	}
}

void CGenCF1::SendLyrics(int pos, int v, int av, int x) {
	if (cantus_incom.size() > cantus_id && cantus_incom[cantus_id].size() > x) {
		lyrics[pos][v] = cantus_incom[cantus_id][x];
	}
}

void CGenCF1::SendComment(int pos, int v, int av, int x, int i)
{
	CString st, com;
	int current_severity = -1;
	// Clear
	comment2[pos+i][v].Empty();
	comment[pos + i][v].clear();
	ccolor[pos + i][v].clear();
	if (anflagsc[av][x] > 0) for (int f = 0; f < anflagsc[av][x]; ++f) {
		// Do not show colors and comments for base voice
		if (av == cpv) {
			int fl = anflags[av][x][f];
			// Send comments and color only if rule is not ignored
			if (accept[fl] != -1) {
				if (!i) {
					if (!accept[fl]) st = "- ";
					else st = "+ ";
					com = st + RuleName[rule_set][fl] + " (" + SubRuleName[rule_set][fl] + ")";
					if (!comment2[pos][v].IsEmpty()) comment2[pos][v] += ", ";
					comment2[pos][v] += RuleName[rule_set][fl] + " (" + SubRuleName[rule_set][fl] + ")";
					if (show_severity) {
						st.Format(" [%d/%d]", fl, severity[fl]);
						com += st;
					}
					if (!RuleComment[fl].IsEmpty()) com += ". " + RuleComment[fl];
					if (!SubRuleComment[rule_set][fl].IsEmpty()) com += " (" + SubRuleComment[rule_set][fl] + ")";
					//com += ". ";
					comment[pos][v].push_back(com);
					ccolor[pos][v].push_back(flag_color[severity[fl]]);
				}
				// Set note color if this is maximum flag severity
				if (severity[fl] > current_severity) {
					current_severity = severity[fl];
					color[pos + i][v] = flag_color[severity[fl]];
				}
			}
		}
	}
}

void CGenCF1::TransposeCantusBack() {
	// Transpose cantus
	if (transpose_back && first_note) {
		int trans = 0;
		if (nmin > first_note0) {
			trans = -floor((nmin - first_note0) / 12 + 1) * 12;
		}
		if (nmax < first_note0) {
			trans = floor((first_note0 - nmax) / 12 + 1) * 12;
		}
		TransposeVector(m_cc, trans);
		TransposeVector(macc2, trans);
		TransposeVector(min_cc, trans);
		TransposeVector(max_cc, trans);
		TransposeVector(min_cc0, trans);
		TransposeVector(max_cc0, trans);
	}
}

void CGenCF1::SendNotes(int pos, int i, int v, int av, int x, vector<int> &cc) {
	note[pos + i][v] = cc[x];
	tonic[pos + i][v] = tonic_cur;
	minor[pos + i][v] = minor_cur;
	len[pos + i][v] = len_export[x];
	pause[pos + i][v] = 0;
	coff[pos + i][v] = coff_export[x] + i;
	// Add scan range
	if (show_note_scan_range) {
		if (av == cpv) {
			// Send source scan range in all cases
			if (min_cc0.size()) nsr1[pos + i][v] = min_cc0[x];
			else nsr1[pos + i][v] = min_cc[x];
			if (max_cc0.size()) nsr2[pos + i][v] = max_cc0[x];
			else nsr2[pos + i][v] = max_cc[x];
		}
		else {
			// Send rswa scan range if rswa
			if (min_cc0.size()) nsr1[pos + i][v] = min_cc[x];
			if (max_cc0.size()) nsr2[pos + i][v] = max_cc[x];
		}
	}
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

// Create bell dynamics curve
void CGenCF1::MakeBellDyn(int v, int step1, int step2, int dyn1, int dyn2, int dyn_rand) {
	// Do not process if steps are equal or wrong
	if (step2 <= step1) return;
	int mids = (step1 + step2) / 2;
	int counts = step2 - step1;
	for (int s = step1; s <= step2; ++s) {
		if (s < mids)	dyn[s][v] = dyn1 + min(dyn2 - dyn1, (dyn2-dyn1) * (s - step1) / counts*2) + dyn_rand * rand2() / RAND_MAX;
		else dyn[s][v] = dyn1 + min(dyn2-dyn1, (dyn2-dyn1) * (step2 - s) / counts*2) + dyn_rand * rand2() / RAND_MAX;
	}
}

// Send inter-cantus pause
int CGenCF1::SendPause(int pos, int v) {
	int pause_len = floor((pos + 1) / 8 + 1) * 8 - pos;
	FillPause(pos, pause_len, v);
	for (int i = pos; i <= pos + pause_len; ++i) tempo[i] = tempo[i - 1];
	return pause_len;
}

void CGenCF1::MakeLenExport(vector<int> &cc, int step1, int av)
{
	int len_temp, last_pos;
	// Create note length
	last_pos = 0;
	len_temp = 0;
	for (s = step1; s < c_len; ++s) {
		if (cc[s] != cc[last_pos]) {
			for (int s2 = last_pos; s2 < s; ++s2) {
				len_export[s2] = len_temp;
			}
			last_pos = s;
			len_temp = 0;
		}
		coff_export[s] = len_temp;
		len_temp += cc_len[s];
	}
	for (int s2 = last_pos; s2 < c_len; ++s2) {
		len_export[s2] = len_temp;
	}
}

int CGenCF1::SendCantus() {
	int step00 = step;
	// Save culmination position
	cf_culm = culm_step;
	if (svoice < 0) return 0;
	CString st, rpst;
	int v = svoice;
	Sleep(sleep_ms);
	TransposeCantusBack();
	len_export.resize(c_len);
	coff_export.resize(c_len);
	MakeLenExport(m_cc, 0, 0);
	// Copy cantus to output
	int pos = step;
	if (step + real_len >= t_allocated) ResizeVectors(t_allocated * 2);
	for (int x = fn; x < c_len; ++x) {
		if (chm.size() > bli[x] && chm[bli[x]] > -1) mark[pos][v] = HarmNames[chm[bli[x]]];
		mark_color[pos][v] = MakeColor(255, 120, 120, 120);
		SendLyrics(pos, v, cpv, x);
		for (int i = 0; i < cc_len[x]; ++i) {
			color[pos + i][v] = MakeColor(0, 100, 100, 100);
			SendNotes(pos, i, v, cpv, x, m_cc);
			SendNgraph(pos, i, v, x);
			SendComment(pos, v, cpv, x, i);
		}
		pos += cc_len[x];
	}
	MakeBellDyn(v, step, pos - 1, 40, 100, 20);
	step = pos + SendPause(pos, v);
	InterpolateNgraph(v, step00, step);
	// Count additional variables
	CountOff(step00, step - 1);
	CountTime(step00, step - 1);
	UpdateNoteMinMax(step00, step - 1);
	UpdateTempoMinMax(step00, step - 1);
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
		st.Format("#%d\nRule penalty: %s", cantus_sent, rpst);
		AddMelody(step00, step - 1, v, st);
	}
	else if (task == tEval) {
		if (m_algo_id == 101) {
			// If RSWA
			st.Format("#%d\nRule penalty: %s", cantus_sent, rpst);
		}
		else {
			if (key_eval.IsEmpty()) {
				// If SWA
				st.Format("#%d (from MIDI file %s)\nRule penalty: %s\nDistance penalty: %d", cantus_id+1, midi_file, rpst, dpenalty_cur);
			}
			else {
				// If evaluating
				st.Format("#%d (from MIDI file %s)\nRule penalty: %s\nKey selection: %s", cantus_id+1, midi_file, rpst, key_eval);
			}
		}
		AddMelody(step00, step - 1, v, st);
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

// General init sequence for CF1/CP1/CA1/CA2...
int CGenCF1::InitGen() {
	// Set rule colors
	for (int i = 0; i < MAX_SEVERITY; ++i) {
		flag_color[i] = MakeColor(0, 255.0 / MAX_SEVERITY*i, 255 - 255.0 / MAX_SEVERITY*i, 0);
	}
	// Check that method is selected
	if (method == mUndefined) {
		WriteLog(5, "Error: method not specified in algorithm configuration file");
		error = 2;
	}
	// Check harmonic meaning loaded
	LoadHarmVar();
	// Check config
	CheckConfig();
	return error;
}

int CGenCF1::InitCantus()
{
	InitGen();
	return error;
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
		WriteLog(0, st);
	}
}

// Create random cantus and optimize it using SWA
void CGenCF1::RandomSWA()
{
	// Init animation
	correct_start_time = time();
	acycle = 0;
	CString st;
	// Unique checker
	VSet<int> vs; 
	// Disable debug flags
	calculate_blocking = 0;
	calculate_correlation = 0;
	calculate_stat = 0;
	// Create single cantus
	cantus.resize(1);
	cantus[0].resize(c_len);
	scantus = &(cantus[0]);
	ScanCantusInit();
	// Set random_seed to initiate random cantus
	random_seed = 1;
	// Set random_range to limit scanning to one of possible fast-scan ranges
	random_range = 1;
	// Prohibit limits recalculation during SWA
	swa_inrange = 1;
	for (int i = 0; i < INT_MAX; ++i) {
		if (need_exit) break;
		// Load first note, because it was overwritten by random generator
		first_note = first_note0;
		// Create random cantus
		MakeNewCantus(m_c, m_cc);
		min_cc0 = min_cc;
		max_cc0 = max_cc;
		cantus[0] = m_cc;
		// Set scan matrix to scan all
		smatrixc = c_len;
		smatrix.resize(c_len);
		for (int x = fn; x < c_len; ++x) {
			smatrix[x] = 1;
		}
		// Optimize cantus
		rpenalty_cur = MAX_PENALTY;
		SWA(0, 0);
		// Show cantus if it is perfect
		if (rpenalty_min <= rpenalty_accepted) {
			if (vs.Insert(m_cc)) {
				int step = t_generated;
				// Add line
				linecolor[t_generated] = MakeColor(255, 0, 0, 0);
				ScanCantus(tEval, 0, &(m_cc));
				if (rpenalty_cur > rpenalty_accepted) {
					st.Format("Error calculating rpenalty %f min %f at step %d", rpenalty_cur, rpenalty_min, t_generated);
					WriteLog(5, st);
				}
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
			return;
		}
	}
	ShowStuck();
}

// Do not calculate dpenalty (dp = 0). Calculate dpenalty (dp = 1).
void CGenCF1::SWA(int i, int dp) {
	CString st;
	long long time_start = CGLib::time();
	s_len = 1;
	// Save source rpenalty
	float rpenalty_source = rpenalty_cur;
	long cnum = 0;
	// Save cantus only if its penalty is less or equal to source rpenalty
	rpenalty_min = rpenalty_cur;
	dpenalty_min = MAX_PENALTY;
	m_cc = cantus[i];
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
			clib.push_back(m_cc);
			clib_vs.Insert(m_cc);
			rpenalty.push_back(rpenalty_min_old);
			dpenalty.push_back(dpenalty_min_old);
		}
		// Sliding Windows Approximation
		ScanCantus(tCor, 0, &m_cc);
		dpenalty_min = MAX_PENALTY;
		cnum = clib.size();
		if (cnum) {
			if (dp) {
				// Count dpenalty for results, where rpenalty is minimal
				dpenalty.resize(cnum);
				for (int x = 0; x < cnum; x++) if (rpenalty[x] <= rpenalty_min) {
					dpenalty[x] = 0;
					for (int z = fn; z < c_len; z++) {
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
				m_cc = clib[cids[cid]];
			}
		}
		// Send log
		if (debug_level > 1) {
			CString est;
			est.Format("SWA%d #%d: rp %.0f from %.0f, dp %d, cnum %ld", s_len, a, rpenalty_min, rpenalty_source, dpenalty_min, cnum);
			WriteLog(3, est);
		}
		if (m_cc.size() > 60) {
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
	long long time_stop = CGLib::time();
	CString est;
	// For successful rpenalty_cur == 0, show last flag that was fixed. For unsuccessful, show best variant
	CString stuck_st = GetStuck();
	est.Format("Finished SWA%d #%d: rp %.0f from %.0f, dp %d, cnum %ld (in %d ms): %s", 
		s_len, a, rpenalty_min, rpenalty_source, dpenalty_min, cnum, time_stop - time_start, stuck_st);
	WriteLog(3, est);
	TestBestRpenalty();
}

// Save accepted time if we are showing best rejected
void CGenCF1::TimeBestRejected() {
	if (best_rejected) {
		accept_time = CGLib::time();
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
		if (clib_vs.Insert(m_cc)) {
			SaveCantus();
			// Save flags for SWA stuck flags
			if (rpenalty_cur) best_flags = flags;
			TestRpenalty();
		}
	}
}

void CGenCF1::ScanCantus(int t, int v, vector<int>* pcantus) {
	int finished = 0;
	int scycle = 0;
	// Load master parameters
	task = t;
	svoice = v;
	if (pcantus) scantus = pcantus;
	else scantus = 0;

	ScanCantusInit();
	if (task == tGen) MultiCantusInit(m_c, m_cc);
	else SingleCantusInit();
	if (FailWindowsLimit()) return;
	// Analyze combination
check:
	while (true) {
		//LogCantus(cc);
		ClearFlags(0, ep2);
		if (FailNoteRepeat(m_cc, 0, ep2 - 1)) goto skip;
		GetMelodyInterval(m_cc, 0, ep2, nmin, nmax);
		++accepted3;
		// Limit melody interval
		if (nmax - nmin > max_interval) FLAG(37, fn);
		if (c_len == ep2 && nmax - nmin < min_interval) FLAG(38, fn);
		// Show status
		long long time = CGLib::time();
		scycle = (time - gen_start_time) / STATUS_PERIOD;
		if (scycle > status_cycle) {
			ShowScanStatus();
			status_cycle = scycle;
		}
		// Limit SAS correction time
		if (task == tCor && method == mScan && max_correct_ms && time - scan_start_time > max_correct_ms) break;
		// Calculate diatonic limits
		nmind = CC_C(nmin, tonic_cur, minor_cur);
		nmaxd = CC_C(nmax, tonic_cur, minor_cur);
		if (FailDiatonic(m_c, m_cc, fn, ep2, minor_cur)) goto skip;
		GetPitchClass(m_c, m_cc, m_pc, m_pcc, fn, ep2);
		CreateLinks(m_cc);
		if (minor_cur) {
			if (FailMinor(m_pcc, m_cc)) goto skip;
			if (FailGisTrail(m_pcc)) goto skip;
			if (FailFisTrail(m_pcc)) goto skip;
		}
		//if (MatchVectors(cc, test_cc, 0, 2)) 
		//WriteLog(1, "Found");
		if (FailTonic(m_cc, m_pc)) goto skip;
		if (FailLastNotes(m_pc, m_pcc)) goto skip;
		if (FailNoteSeq(m_pc)) goto skip;
		if (FailIntervals(m_c, m_cc, m_pc, m_pcc)) goto skip;
		GetLeapSmooth(m_c, m_cc, m_leap, m_smooth, m_slur);
		if (FailManyLeaps(m_c, m_cc, m_leap, m_smooth, m_slur, max_leaps, max_leaped, max_leap_steps, 3, 25)) goto skip;
		if (FailManyLeaps(m_c, m_cc, m_leap, m_smooth, m_slur, max_leaps2, max_leaped2, max_leap_steps2, 202, 203)) goto skip;
		if (FailLeapSmooth(m_c, m_cc, m_leap, m_smooth, m_slur)) goto skip;
		if (FailOutstandingRepeat(m_c, m_cc, m_leap, repeat_steps2, repeat_notes2, 76)) goto skip;
		if (FailOutstandingRepeat(m_c, m_cc, m_leap, repeat_steps3, repeat_notes3, 36)) goto skip;
		if (FailLongRepeat(m_cc, m_leap, repeat_steps5, repeat_notes5, 72)) goto skip;
		if (FailLongRepeat(m_cc, m_leap, repeat_steps7, repeat_notes7, 73)) goto skip;
		if (FailGlobalFill(m_c, nstat2)) goto skip;
		if (FailLocalRange(m_cc, notes_lrange, min_lrange, 98)) goto skip;
		if (FailLocalRange(m_cc, notes_lrange2, min_lrange2, 198)) goto skip;
		if (FailStagnation(m_cc, nstat, stag_note_steps, stag_notes, 10)) goto skip;
		if (FailStagnation(m_cc, nstat, stag_note_steps2, stag_notes2, 39)) goto skip;
		if (FailMultiCulm(m_cc, m_slur)) goto skip;
		if (FailFirstNotes(m_pc)) goto skip;
		if (FailLeap(m_c, m_leap, m_smooth, nstat2, nstat3)) goto skip;
		if ((fli_size>1) && FailMelodyHarm(m_pc)) goto skip;
		MakeMacc(m_cc);
		if (FailLocalMacc(notes_arange, min_arange, 15)) goto skip;
		if (FailLocalMacc(notes_arange2, min_arange2, 16)) goto skip;

		SaveBestRejected(m_cc);
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
			CalcRpenalty(m_cc);
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
				if (clib_vs.Insert(m_cc)) {
					if (SendCantus()) break;
					ReseedCantus();
					// Start evaluating without scan
					goto check;
				}
				else {
					++cantus_ignored;
					//CString est;
					//est.Format("Ignored cantus %d while sent=%d", cantus_ignored, cantus_sent);
					//WriteLog(1, est);
					//if (SendCantus()) break;
					ReseedCantus();
					// Start evaluating without scan
					goto check;
				}
			}
			else {
				if (SendCantus()) break;
			}
			// Exit if this is evaluation
			if (task == tEval) return;
		}
	skip:
		if (need_exit) break;
		ScanLeft(m_cc, finished);
		if (finished) {
			// Clear flag to prevent coming here again
			finished = 0;
			// Sliding Windows Approximation
			if (method == mSWA) {
				if (NextSWA(m_cc, m_cc_old)) break;
				goto check;
			}
			// Finish if this is last variant in first window and not SWA
			else if ((p == 0) || (wid == 0)) {
				if (random_seed && random_range && accept_reseed) {
					// Infinitely cycle through ranges
					ReseedCantus();
					// Start evaluating without scan
					goto check;
				}
				WriteLog(3, "Last variant in first window reached");
				break;
			}
			ShowBestRejected(m_cc);
			BackWindow(m_cc);
			// Goto next variant calculation
			goto skip;
		} // if (finished)
		ScanRight(m_cc);
	}
	if (accepted3 > 100000) ShowScanStatus();
	WriteFlagCor();
	ShowFlagStat();
	ShowFlagBlock();
}

void CGenCF1::ScanRight(vector<int> &cc) {
	// Increase rightmost element, which was not reset to minimum
	++cc_id[p];
	cc[p] = cc_order[p][cc_id[p]];
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
	//test_cc[1000] = 1;
	// Save first note because it will be overwritten by random generator
	first_note0 = first_note;
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
	// If error, return;
	if (InitCantus()) return;
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
		vector<vector<CString>> comment3(t_generated);
		vector<CString> comment4(t_generated);
		vector<DWORD> color2(t_generated);
		for (int i = 0; i < accepted; ++i) ci[i] = i;
		// Shuffled indexes
		long long seed = CGLib::time();
		::shuffle(ci.begin(), ci.end(), default_random_engine(seed));
		// Swap
		int s1, s2;
		for (int i = 0; i < accepted; ++i) {
			for (int x = 0; x < c_len; ++x) {
				s1 = i*(c_len + 1) + x;
				s2 = ci[i]*(c_len + 1) + x;
				note2[s1] = note[s2][v];
				comment3[s1] = comment[s2][v];
				comment4[s1] = comment2[s2][v];
				color2[s1] = color[s2][v];
			}
		}
		// Replace
		for (int i = 0; i < accepted; ++i) {
			for (int x = 0; x < c_len; ++x) {
				s1 = i*(c_len + 1) + x;
				note[s1][v] = note2[s1];
				comment[s1][v] = comment3[s1];
				comment2[s1][v] = comment4[s1];
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
