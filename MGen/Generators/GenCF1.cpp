// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../stdafx.h"
#include "GenCF1.h"

#ifdef _DEBUG
#define new DEBUG_NEW 
#endif

CGenCF1::CGenCF1()
{
	progress.resize(MAX_PROGRESS);
	av_cnt = 1;
	v_cnt = 1;
	ngraph_size = 3;
	RegisterGraph("Tonic rating", 2);
	RegisterGraph("Leaps rating", 1);
	RegisterGraph("Leaped rating", 0.2);
	cpv = 0;
	//midifile_tpq_mul = 8;
	accept.resize(MAX_RULES);
	rule_viz.resize(MAX_RULES);
	rule_viz_v2.resize(MAX_RULES);
	rule_viz_int.resize(MAX_RULES);
	rule_viz_t.resize(MAX_RULES);
	false_positives_ignore.resize(MAX_RULES);
	false_positives_global.resize(MAX_RULES);
	sas_emulator_max_delay.resize(MAX_RULES);
	sas_emulator_move_ignore.resize(MAX_RULES);
	sas_emulator_replace.resize(MAX_RULES);
	flag_replace.resize(MAX_RULES);
	sas_emulator_unstable.resize(MAX_RULES);
	flag_delay.resize(MAX_RULES);
	flag_delay_st.resize(MAX_RULES);
	RuleParam.resize(MAX_RULESETS);
	SubRuleName.resize(MAX_RULESETS);
	RuleName.resize(MAX_RULESETS);
	SubRuleComment.resize(MAX_RULESETS);
	RuleComment.resize(MAX_RULES);
	RuleGroup.resize(MAX_RULES);
	ssf.resize(MAX_RULES);
	severity.resize(MAX_RULES);
	flag_color.resize(MAX_SEVERITY);
	accepts.resize(MAX_RULESETS);
	max_note_len.resize(6);
	// Start severity
	severity[0] = 0;
	// Data ready
	data_ready.resize(MAX_DATA_READY);
	data_ready_persist.resize(MAX_DATA_READY_PERSIST);
	warn_data_ready.resize(MAX_DATA_READY);
	warn_data_ready_persist.resize(MAX_DATA_READY_PERSIST);
	// Harmony notation
	HarmName.resize(7);
	HarmName_m.resize(7);
	HarmName_ma.resize(7);
}

CGenCF1::~CGenCF1()
{
}

// Load variants of possible harmonic meaning
void CGenCF1::LoadHarmVar() {
	SET_READY_PERSIST(DP_hv);
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
	SET_READY_PERSIST(DP_hsp);
	CString st, est;
	vector<CString> ast;
	int i = 0;
	hsp.resize(7);
	ifstream fs;
	// Check file exists
	if (!fileExists(fname)) {
		est.Format("LoadHSP cannot find file: %s", fname);
		WriteLog(5, est);
		error = 1;
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
				error = 1;
				return;
			}
			if (i > 7) {
				est.Format("Wrong line count at line %d in hsp file %s: '%s'", i, fname, st);
				WriteLog(5, est);
				error = 1;
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
	SET_READY_PERSIST(DP_Rules);
	CString st, est, rule, subrule;
	vector<CString> ast, ast2;
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
		error = 1;
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
			if (ast.size() != 23) {
				est.Format("Wrong column count at line %d in rules file %s: '%s'", i, fname, st);
				WriteLog(5, est);
				error = 1;
				return;
			}
			set = atoi(ast[0]);
			rid = atoi(ast[1]);
			sev = atoi(ast[2]);
			rule = ast[5];
			subrule = ast[6];
			flag = atoi(ast[7]);
			if (sev < prohibit_min_severity && flag == 0) flag = 1;
			// Find rule id
			if (rid >= max_flags) {
				max_flags = rid + 1;
				if (max_flags >= MAX_RULES) {
					est.Format("Rule id (%d) is equal or greater than MAX_RULES (%d). Consider increasing MAX_RULES", rid, MAX_RULES);
					WriteLog(5, est);
					error = 1;
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
			RuleGroup[rid] = ast[4];
			RuleComment[rid] = ast[8];
			SubRuleComment[set][rid] = ast[9];
			// If testing, enable all disabled rules so that expected violations can be confirmed
			//if (m_testing && flag == -1) flag = 1;
			accepts[set][rid] = flag;
			severity[rid] = sev;
			rule_viz[rid] = atoi(ast[10]);
			rule_viz_int[rid] = atoi(ast[11]);
			rule_viz_v2[rid] = atoi(ast[12]);
			rule_viz_t[rid] = ast[13];
			if (viz_space[rule_viz[rid]] && rule_viz_t[rid].IsEmpty()) rule_viz_t[rid] = " ";
			rule_viz_t[rid].Replace("!rn!", RuleName[set][rid]);
			rule_viz_t[rid].Replace("!srn!", SubRuleName[set][rid]);
			rule_viz_t[rid].Replace("!rg!", RuleGroup[rid]);
			rule_viz_t[rid].Replace("!rc!", RuleComment[rid]);
			rule_viz_t[rid].Replace("!src!", SubRuleComment[set][rid]);
			false_positives_global[rid] = atoi(ast[16]);
			false_positives_ignore[rid] = atoi(ast[17]);
			sas_emulator_max_delay[rid] = atoi(ast[18]);
			sas_emulator_move_ignore[rid] = atoi(ast[19]);
			if (!ast[20].IsEmpty()) {
				Tokenize(ast[20], ast2, ",");
				for (int x = 0; x < ast2.size(); ++x) {
					int fl = atoi(ast2[x]);
					if (fl) sas_emulator_replace[fl].push_back(rid);
				}
			}
			sas_emulator_unstable[rid] = atoi(ast[21]);
			if (!ast[22].IsEmpty()) {
				Tokenize(ast[22], ast2, ",");
				for (int x = 0; x < ast2.size(); ++x) {
					int fl = atoi(ast2[x]);
					if (fl) flag_replace[fl].push_back(rid);
				}
			}
		}
	}
	fs.close();
	est.Format("LoadRules loaded %d lines from %s", i, fname);
	WriteLog(0, est);
	// Check that all rules in lists exist
	CheckRuleList("flag_replace", flag_replace);
	CheckRuleList("sas_emulator_replace", sas_emulator_replace);
}

void CGenCF1::CheckRuleList(CString list_name, vector<vector<int>> &v) {
	for (int rs = 0; rs < accepts.size(); ++rs) if (accepts[rs].size()) {
		for (int f = 0; f < v.size(); ++f) {
			for (int f2 = 0; f2 < v[f].size(); ++f2) {
				if (SubRuleName[rs][v[f][f2]].IsEmpty()) {
					CString est;
					est.Format("Detected undefined rule %d in rule list %s for rule %d",
						v[f][f2], list_name, f);
					WriteLog(5, est);
				}
			}
		}
	}
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
		error = 1;
		return 0;
	}
	return RuleParam[rset][rid][type][id];
}

// Parse rules
void CGenCF1::ParseRules() {
	SET_READY_PERSIST(DP_RuleParam);
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
	CHECK_READY_PERSIST(DP_RuleParam);
	SET_READY_PERSIST(DP_RuleSetParam);
	lclimax_notes = GetRuleParam(rule_set, 32, rsSubComment, 0);
	lclimax_mea = GetRuleParam(rule_set, 32, rsSubComment, 1);
	lclimax_mea2 = GetRuleParam(rule_set, 325, rsComment, 0);
	mea_per_sus = GetRuleParam(rule_set, 341, rsSubName, 0);
	max_note_len[1] = GetRuleParam(rule_set, 336, rsSubName, 1);
	max_note_len[2] = GetRuleParam(rule_set, 337, rsSubName, 1);
	max_note_len[3] = GetRuleParam(rule_set, 338, rsSubName, 1);
	max_note_len[4] = GetRuleParam(rule_set, 339, rsSubName, 1);
	max_note_len[5] = GetRuleParam(rule_set, 340, rsSubName, 1);
	tritone_res_quart = GetRuleParam(rule_set, 2, rsSubComment, 0);
	sus_last_measures = GetRuleParam(rule_set, 139, rsSubName, 0);
	sus_insert_max_leap = Interval2Chromatic(GetRuleParam(rule_set, 295, rsSubComment, 0));
	dnt_max_leap = Interval2Chromatic(GetRuleParam(rule_set, 260, rsSubName, 0));
	cambiata_max_leap3 = Interval2Chromatic(GetRuleParam(rule_set, 264, rsSubComment, 1));
	cambiata_max_leap4 = Interval2Chromatic(GetRuleParam(rule_set, 265, rsSubComment, 1));
	pco_apart = GetRuleParam(rule_set, 248, rsName, 0);
	thirds_ignored = GetRuleParam(rule_set, 70, rsName, 0);
	c4p_last_meas = GetRuleParam(rule_set, 144, rsName, 1);
	fill_pre3_notes = GetRuleParam(rule_set, 100, rsComment, 0);
	fill_pre4_int = GetRuleParam(rule_set, 144, rsComment, 0) - 1;
	fill_pre4_notes = GetRuleParam(rule_set, 144, rsComment, 1);
	c4p_last_notes = GetRuleParam(rule_set, 144, rsName, 2);
	pre_last_leaps = GetRuleParam(rule_set, 204, rsName, 0);
	max_smooth = GetRuleParam(rule_set, 4, rsSubName, 0);
	max_smooth_direct = GetRuleParam(rule_set, 5, rsSubName, 0);
	max_smooth2 = GetRuleParam(rule_set, 302, rsSubName, 0);
	max_smooth_direct2 = GetRuleParam(rule_set, 303, rsSubName, 0);
	max_leap_steps = GetRuleParam(rule_set, 3, rsName, 0);
	max_leaps = GetRuleParam(rule_set, 3, rsSubName, 0);
	max_leaped = GetRuleParam(rule_set, 25, rsSubName, 0);
	max_leap_steps2 = GetRuleParam(rule_set, 202, rsName, 0);
	max_leaps2 = GetRuleParam(rule_set, 202, rsSubName, 0);
	max_leaped2 = GetRuleParam(rule_set, 203, rsSubName, 0);
	max_leaps3 = GetRuleParam(rule_set, 311, rsSubName, 0);
	max_leaped3 = GetRuleParam(rule_set, 312, rsSubName, 0);
	max_leaps4 = GetRuleParam(rule_set, 313, rsSubName, 0);
	max_leaped4 = GetRuleParam(rule_set, 314, rsSubName, 0);
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
	max_interval_cf = Interval2Chromatic(GetRuleParam(rule_set, 37, rsSubName, 0));
	max_interval_cp = Interval2Chromatic(GetRuleParam(rule_set, 304, rsSubName, 0));
	sum_interval = Interval2Chromatic(GetRuleParam(rule_set, 7, rsSubName, 0));
	max_between = Interval2Chromatic(GetRuleParam(rule_set, 11, rsSubName, 0));
	burst_between = Interval2Chromatic(GetRuleParam(rule_set, 11, rsSubComment, 0));
	burst_steps = GetRuleParam(rule_set, 11, rsSubComment, 1);
	slurs_window = GetRuleParam(rule_set, 93, rsName, 0);
	miss_slurs_window = GetRuleParam(rule_set, 188, rsName, 0);
	contrary_min = GetRuleParam(rule_set, 35, rsSubName, 0);
	contrary_min2 = GetRuleParam(rule_set, 46, rsSubName, 0); 

	notes_picount = GetRuleParam(rule_set, 344, rsSubName, 0);
	min_picount = GetRuleParam(rule_set, 344, rsSubName, 1);
	notes_picount2 = GetRuleParam(rule_set, 345, rsSubName, 0);
	min_picount2 = GetRuleParam(rule_set, 345, rsSubName, 1);
	notes_picount3 = GetRuleParam(rule_set, 346, rsSubName, 0);
	min_picount3 = GetRuleParam(rule_set, 346, rsSubName, 1);

	notes_lrange0 = GetRuleParam(rule_set, 396, rsSubName, 0);
	min_lrange0 = abs(GetRuleParam(rule_set, 396, rsSubName, 1)) - 1;
	notes_lrange02 = GetRuleParam(rule_set, 397, rsSubName, 0);
	min_lrange02 = abs(GetRuleParam(rule_set, 397, rsSubName, 1)) - 1;
	notes_lrange03 = GetRuleParam(rule_set, 395, rsSubName, 0);
	min_lrange03 = abs(GetRuleParam(rule_set, 395, rsSubName, 1)) - 1;

	notes_lrange = GetRuleParam(rule_set, 98, rsSubName, 0);
	min_lrange = abs(GetRuleParam(rule_set, 98, rsSubName, 1)) - 1;
	notes_lrange2 = GetRuleParam(rule_set, 198, rsSubName, 0);
	min_lrange2 = abs(GetRuleParam(rule_set, 198, rsSubName, 1)) - 1;
	notes_lrange3 = GetRuleParam(rule_set, 300, rsSubName, 0);
	min_lrange3 = abs(GetRuleParam(rule_set, 300, rsSubName, 1)) - 1;

	notes_lrange1 = GetRuleParam(rule_set, 352, rsSubName, 0);
	min_lrange1 = abs(GetRuleParam(rule_set, 352, rsSubName, 1)) - 1;
	notes_lrange12 = GetRuleParam(rule_set, 353, rsSubName, 0);
	min_lrange12 = abs(GetRuleParam(rule_set, 353, rsSubName, 1)) - 1;
	notes_lrange13 = GetRuleParam(rule_set, 351, rsSubName, 0);
	min_lrange13 = abs(GetRuleParam(rule_set, 351, rsSubName, 1)) - 1;

	notes_arange = GetRuleParam(rule_set, 15, rsSubName, 0);
	min_arange = GetRuleParam(rule_set, 15, rsSubName, 1) / 10.0;
	notes_arange2 = GetRuleParam(rule_set, 16, rsSubName, 0);
	min_arange2 = GetRuleParam(rule_set, 16, rsSubName, 1) / 10.0;

	dev_late2 = GetRuleParam(rule_set, 191, rsSubName, 0);
	dev_late3 = GetRuleParam(rule_set, 192, rsSubName, 0);
	dev2_maxlen = GetRuleParam(rule_set, 386, rsSubComment, 0);
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
	tonic_max_cp = GetRuleParam(rule_set, 310, rsSubName, 0);
	tonic_window_cp = GetRuleParam(rule_set, 310, rsSubName, 1);
	tonic_leap = Interval2Chromatic(GetRuleParam(rule_set, 196, rsSubComment, 0));
	tonic_wei_leap = GetRuleParam(rule_set, 196, rsSubComment, 1);
	tonic_wei_len = GetRuleParam(rule_set, 196, rsSubComment, 2);
	tonic_wei_beat = GetRuleParam(rule_set, 196, rsSubComment, 3);
	tonic_wei_long = GetRuleParam(rule_set, 196, rsSubComment, 4);
	tonic_wei_pco = GetRuleParam(rule_set, 196, rsSubComment, 5);
	fis_gis_max = GetRuleParam(rule_set, 199, rsSubName, 0);
	fis_g_max = GetRuleParam(rule_set, 349, rsSubName, 0);
	fis_g_max2 = GetRuleParam(rule_set, 350, rsSubName, 0);
}

void CGenCF1::ProcessSpecies() {
	// GenCP1
	if (m_algo_id == 121) {
		if (species == 1) {
			npm = 1;
			fn = 0;
			midifile_out_mul2 = 8;
		}
		if (species == 2) {
			npm = 2;
			fn = 1;
			if (accept[273] && rand() > RAND_MAX / 2) fn = 0;
			midifile_out_mul2 = 4;
		}
		if (species == 3) {
			npm = 4;
			fn = 1;
			if (accept[273] && rand() > RAND_MAX / 2) fn = 0;
			midifile_out_mul2 = 2;
		}
		if (species == 4) {
			npm = 2;
			fn = 1;
			if (accept[273] && rand() > RAND_MAX / 2) fn = 0;
			midifile_out_mul2 = 4;
		}
		if (species == 5) {
			npm = 8;
			if (accept[273]) fn = randbw(0, 2);
			else fn = randbw(1, 2);
			if (fn == 2) fn = 4;
			else if (fn == 1) fn = 2;
			midifile_out_mul2 = 1;
		}
	}
}

void CGenCF1::CheckConfig() {
	CHECK_READY_PERSIST(DP_Config, DP_RuleSet);
	SET_READY_PERSIST(DP_ConfigTest);
	// GenCP1
	if (m_algo_id == 121) {
		ProcessSpecies();
		if (accept_cantus_rechoose && cantus_id2) {
			WriteLog(1, "Warning: accept_cantus_rechoose will not work with cantus_id above zero (check config)");
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
	}
	if (s_len != 1) {
		WriteLog(5, "Warning: s_len should equal 1. Other values are not tested and usually do not have any advantages");
	}
	if (best_rejected) {
		WriteLog(5, "Warning: Best rejected algorithm is not tested and can have bugs. Please set best_rejected to 0");
	}
	if (corrections > 1) {
		WriteLog(5, "Warning: Corrections > 1 algorithm is not tested and can have bugs. Please set corrections to 0 or 1. Also, it is not optimized for fn_source.");
	}
	if (swa_steps < s_len) {
		WriteLog(5, "Warning: Swa_steps cannot be below s_len. Changed to ");
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
	if (log_pmap && !calculate_correlation && !calculate_blocking && !calculate_stat) {
		WriteLog(1, "Log_pmap will not work correctly if all these flags are reset: calculate_correlation, calculate_blocking, calculate_stat. Enable at least one of them.");
	}
	if (shuffle && random_seed) {
		WriteLog(1, "Shuffling after random_seed will not add randomness (check config)");
	}
	// Check rule parameters
	if (burst_between <= max_between) {
		WriteLog(5, "Warning: maximum burst interval should be greater than maximum interval between voices (check config)");
	}
	if (tonic_wei_len > 30) {
		WriteLog(5, "Warning: tonic length decrease percent should be between 0 and 30");
	}
}

// Select rules
int CGenCF1::SelectRuleSet(int rs) {
	CHECK_READY_PERSIST(DP_Rules);
	SET_READY_PERSIST(DP_RuleSet);
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
			if (i == max_flags - 1) {
				WriteLog(5, "Warning: all rules are rejected (0) in configuration file");
				error = 1;
			}
		}
		// Calculate second level flags count
		flags_need2 = 0;
		for (int i = 0; i < max_flags; ++i) {
			if (accept[i] == 2) ++flags_need2;
		}
	}
	return error;
}

void CGenCF1::LoadHarmNotation() {
	CString fname = "configs\\harm\\harm-notation.csv";
	if (!CGLib::fileExists(fname)) {
		CString est;
		est.Format("Cannot find file: %s", fname);
		WriteLog(5, est);
		error = 1;
		return;
	}
	vector <CString> sv;
	ifstream fs;
	int cur_nid = -1;
	fs.open(fname);
	CString st;
	char pch[2550];
	int pos = 0;
	// Load header
	//fs.getline(pch, 2550);
	while (fs.good()) {
		fs.getline(pch, 2550);
		st = pch;
		// Skip comments
		pos = st.Find("#");
		if (pos == 0)	continue;
		st.Trim();
		pos = 0;
		if (st.Find(";") != -1) {
			Tokenize(st, sv, ";");
			if (sv.size() != 8) {
				CString est;
				est.Format("Wrong count of columns (%d) in file %s", sv.size(), fname);
				WriteLog(5, est);
				error = 1;
				return;
			}
			for (int i = 0; i < sv.size(); ++i) sv[i].Trim();
			if (sv[0] == "Major") ++cur_nid;
			if (cur_nid == harm_notation) {
				if (sv[0] == "Major")
					for (int i = 1; i < 8; ++i) HarmName[i - 1] = sv[i];
				if (sv[0] == "Minor natural")
					for (int i = 1; i < 8; ++i) HarmName_m[i - 1] = sv[i];
				if (sv[0] == "Minor altered")
					for (int i = 1; i < 8; ++i) HarmName_ma[i - 1] = sv[i];
			}
		}
	}
	fs.close();
	if (HarmName[6].IsEmpty() || HarmName_m[6].IsEmpty() || HarmName_ma[6].IsEmpty()) {
		CString est;
		est.Format("Error loading harmonic notation");
		WriteLog(5, est);
		error = 1;
	}
}

void CGenCF1::LoadConfigLine(CString* sN, CString* sV, int idata, float fdata) {
	SET_READY_PERSIST(DP_Config);
	CheckVar(sN, sV, "reduce_between", &reduce_between, 0, 100);
	CheckVar(sN, sV, "confirm_mode", &confirm_mode, 0, 2);
	CheckVar(sN, sV, "ly_msh", &ly_msh, 0, 1); 
	CheckVar(sN, sV, "harm_notation", &harm_notation, 0, 5);
	CheckVar(sN, sV, "show_harmony_bass", &show_harmony_bass, 0, 2);
	CheckVar(sN, sV, "log_pmap", &log_pmap, 0, 1);
	CheckVar(sN, sV, "show_correct_hatch", &show_correct_hatch, 0, 1);
	CheckVar(sN, sV, "show_hatch", &show_hatch, 0, 2);
	CheckVar(sN, sV, "show_min_severity", &show_min_severity, 0, 100);
	CheckVar(sN, sV, "ly_flag_style", &ly_flag_style, 0, 2);
	CheckVar(sN, sV, "ly_dominant_letter", &ly_dominant_letter, 0, 1);
	CheckVar(sN, sV, "cor_ack", &cor_ack, 0, 1);
	CheckVar(sN, sV, "show_ignored_flags", &show_ignored_flags, 0, 1);
	CheckVar(sN, sV, "show_allowed_flags", &show_allowed_flags, 0, 1);
	CheckVar(sN, sV, "emulate_sas", &emulate_sas, 0, 1);
	CheckVar(sN, sV, "max_correct_ms", &max_correct_ms, 0);
	CheckVar(sN, sV, "animate", &animate, 0);
	CheckVar(sN, sV, "prohibit_min_severity", &prohibit_min_severity, 0, 101);
	CheckVar(sN, sV, "animate_delay", &animate_delay, 0);
	CheckVar(sN, sV, "cantus_high", &cantus_high, 0, 1);
	CheckVar(sN, sV, "rpenalty_accepted", &rpenalty_accepted, 0);
	CheckVar(sN, sV, "c_len", &c_len, 1);
	CheckVar(sN, sV, "s_len", &s_len, 1);
	CheckVar(sN, sV, "transpose_cantus", &transpose_cantus, 0, 127);
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

	// Load harmonic notation
	if (*sN == "harm_notation") {
		LoadHarmNotation();
	}
	// Load HSP
	if (*sN == "hsp_file") {
		++parameter_found;
		LoadHSP("configs\\harm\\" + *sV);
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
			error = 1;
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

void CGenCF1::LogCantus(CString st3, int x, int size, vector<int> &c) {
	CString st, st2;
	st2.Format("%s %d: ", st3, x);
	for (int i = 0; i < size; ++i) {
		st.Format("%d ", c[i]);
		st2 += st;
	}
	CGLib::AppendLineToFile("log/temp.log", st2 + " \n");
}

CString CGenCF1::vint2st(int size, vector<int> &c) {
	CString st, st2;
	for (int i = 0; i < size; ++i) {
		st.Format("%d ", c[i]);
		st2 += st;
	}
	return st2;
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
	CHECK_READY(DR_fli);
	for (int x = 0; x < fli_size-2; ++x) {
		s = fli[x];
		s1 = fli[x + 1];
		// Prohibit GC before cadence
		//if (pc[s] == 4 && pc[s1] == 0) FLAG2(48, s);
	}
	return 0;
}

int CGenCF1::FailLocalRange(vector<int> &c, int notes, int mrange, int flag) {
	CHECK_READY(DR_fli, DR_c);
	// Do not test if flag disabled and not testing
	if (task != tEval && accept[flag] == -1) return 0;
	// Do not test if not enough notes. If melody is short, than global range check is enough
	if (fli_size < notes) return 0;
	int lmin, lmax, s;
	int ls_max = fli_size - notes;
	int ls_max2;
	int fired = 0;
	// Loop through windows
	for (ls = 0; ls <= ls_max; ++ls) {
		lmin = MAX_NOTE;
		lmax = 0;
		ls_max2 = ls + notes;
		// Loop inside each window
		for (int ls2 = ls; ls2 < ls_max2; ++ls2) {
			s = fli[ls2];
			if (c[s] < lmin) lmin = c[s];
			if (c[s] > lmax) lmax = c[s];
		}
		// Check range
		if (lmax - lmin < mrange) {
			if (fired) {
				fpenalty[flag] += severity[flag] + 1;
			}
			else {
				FLAG2L(flag, fli[ls], fli[ls_max2 - 1]);
				fired = 1;
			}
		}
	}
	return 0;
}

int CGenCF1::FailLocalPiCount(vector<int> &cc, int notes, int picount, int flag) {
	CHECK_READY(DR_fli, DR_nmin);
	// Do not test if flag disabled and not testing
	if (task != tEval && accept[flag] == -1) return 0;
	// Do not test if not enough notes
	if (fli_size < notes) return 0;
	int picount2, i;
	// Clear nstat
	for (i = nmin; i <= nmax; ++i) nstat[i] = 0;
	for (ls = 0; ls < fli_size; ++ls) {
		s = fli[ls];
		// Add new note to stagnation array
		++nstat[cc[s]];
		// Subtract old note
		if (ls >= notes) --nstat[cc[fli[ls - notes]]];
		// Check if little pitches
		if (ls >= notes - 1) {
			picount2 = 0;
			for (i = nmin; i <= nmax; ++i) if (nstat[i]) ++picount2;
			if (picount2 < picount) FLAG2L(flag, fli[ls - notes + 1], fli[ls]);
		}
	}
	return 0;
}

// Moving average
void CGenCF1::maVector(vector<float> &v, vector<float> &v2, int range) {
	int pos1, pos2;
	float ma, maw_sum;
	for (int s = 0; s < ep2; ++s) {
		pos1 = max(0, s - range);
		pos2 = min(ep2 - 1, s + range);
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
	for (int s = 0; s < ep2; ++s) {
		pos1 = max(0, s - range);
		pos2 = min(ep2 - 1, s + range);
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
	for (int s = 0; s < ep2; ++s) {
		pos1 = max(0, s - range);
		pos2 = min(ep2 - 1, s + range);
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
	for (int s = 0; s < ep2; ++s) {
		pos1 = max(0, s - range);
		pos2 = min(ep2 - 1, s + range);
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
	SET_READY(DR_macc);
	int pos1, pos2;
	int ma_range = 2 * minl;
	macc_range = ma_range;
	macc2_range = ma_range * 2;
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
	for (int s = 0; s < ep2; ++s) {
		pos1 = max(0, s - ma_range);
		pos2 = min(ep2 - 1, s + ma_range);
		de = 0;
		maw_sum = 0;
		for (int x = pos1; x <= pos2; ++x) if (cc[x]) {
			de += maw[abs(x - s)] * SQR(cc[x] - macc[s]);
			maw_sum += maw[abs(x - s)];
		}
		if (maw_sum) decc[s] = SQRT(de / maw_sum);
		else decc[s] = 0;
	}
	// Smooth
	mawVector(decc, decc2, ma_range);
}

int CGenCF1::FailLocalMacc(int notes, float mrange, int flag) {
	CHECK_READY(DR_fli, DR_macc);
	// Do not test if flag disabled and not testing
	if (task != tEval && accept[flag] == -1) return 0;
	// Do not test if not enough notes. If melody is short, than global range check is enough
	if (fli_size < notes) return 0;
	float lmin, lmax, maccr;
	int maccr_count = 0;
	int s;
	int ls_max = fli_size - notes;
	int ls_max2;
	int fired = 0;
	pm_maccr_min = INT_MAX;
	pm_maccr_max = 0;
	pm_maccr_av = 0;
	// Loop through windows
	for (ls = 0; ls <= ls_max; ++ls) {
		lmin = MAX_NOTE;
		lmax = 0;
		ls_max2 = ls + notes;
		// Do not check if later notes are not created
		if (ep2 < c_len && fli2[ls_max2 - 1] + macc2_range >= ep2) continue;
		// Loop inside each window
		for (s = fli[ls]; s <= fli2[ls_max2 - 1]; ++s) {
			if (macc2[s] < lmin) lmin = macc2[s];
			if (macc2[s] > lmax) lmax = macc2[s];
		}
		// Record range
		maccr = lmax - lmin;
		pm_maccr_av += maccr;
		++maccr_count;
		if (pm_maccr_min > maccr) pm_maccr_min = maccr;
		if (pm_maccr_max < maccr) pm_maccr_max = maccr;
		// Check range
		if (lmin < MAX_NOTE && lmax > 0 && maccr < mrange) {
			if (fired) {
				fpenalty[flag] += severity[flag] + 1;
			}
			else {
				FLAG2L(flag, fli[ls], fli[ls_max2 - 1]);
				fired = 1;
			}
		}
	}
	pm_maccr_av /= maccr_count;
	return 0;
}

// Count limits
void CGenCF1::GetMelodyInterval(vector<int> &cc, int step1, int step2, int &nmin, int &nmax) {
	SET_READY(DR_nmin);
	// Calculate range
	nmin = MAX_NOTE;
	nmax = 0;
	for (int i = step1; i < step2; ++i) {
		if (cc[i] < nmin) nmin = cc[i];
		if (cc[i] > nmax) nmax = cc[i];
	}
	// Calculate diatonic limits
	nmind = CC_C(nmin, tonic_cur, minor_cur);
	nmaxd = CC_C(nmax, tonic_cur, minor_cur);
}

// Clear flags
void CGenCF1::ClearFlags(int step1, int step2) {
	if (!skip_flags || flags_need2) {
		fill(flags.begin(), flags.end(), 0);
		fill(fpenalty.begin(), fpenalty.end(), 0.0);
	}
	flags[0] = 1;
	for (int i = step1; i < step2; ++i) {
		anflags[cpv][i].clear();
		anfl[cpv][i].clear();
	}
	rpenalty_cur = 0;
}

// Calculate pitch class
void CGenCF1::GetPitchClass(vector<int> &c, vector<int> &cc, vector<int> &pc, vector<int> &pcc, int step1, int step2) {
	CHECK_READY(DR_c);
	SET_READY(DR_pc);
	for (int i = step1; i < step2; ++i) {
		pc[i] = c[i] % 7;
		pcc[i] = (cc[i] + 12 - tonic_cur) % 12;
	}
}

int CGenCF1::FailGisTrail(vector<int> &pcc) {
	CHECK_READY(DR_fli, DR_pc);
	int gis_trail = 0;
	for (ls = 0; ls < fli_size; ++ls) {
		s = fli[ls];
		if (pcc[s] == 11) {
			// Set to maximum on new G# note
			gis_trail = gis_trail_max;
		}
		else {
			if (pcc[s] == 10) {
				// Prohibit G note close to G#
				if (gis_trail) FLAG2L(200, s, fli[max(0, ls - gis_trail_max + gis_trail)]);
			}
		}
		// Decrease if not zero
		if (gis_trail) --gis_trail;
	}
	return 0;
}

int CGenCF1::FailFisTrail(vector<int> &pcc) {
	CHECK_READY(DR_fli, DR_pc);
	int pos1, pos2, found;
	for (ls = 0; ls < fli_size; ++ls) {
		s = fli[ls];
		if (pcc[s] == 9) {
			// Find VII#
			pos1 = max(0, ls - fis_gis_max);
			pos2 = min(fli_size - 1, ls + fis_gis_max);
			found = 0;
			for (int x = pos1; x <= pos2; ++x) {
				if (pcc[fli[x]] == 11) {
					found = 1;
					break;
				}
			}
			if (!found) {
				// Flag only if full melody analysis or window is not cut
				if (ls + fis_gis_max <= fli_size - 1 || ep2 == c_len)	FLAG2(199, s);
			}
			// Find VII before
			pos1 = max(0, ls - fis_g_max);
			if (svoices > 1) {
				for (int x = pos1; x < ls; ++x) {
					if (apcc[0][fli[x]] == 10 || apcc[1][fli[x]] == 10) {
						FLAG2L(349, s, fli[x]);
						break;
					}
				}
			}
			else {
				for (int x = pos1; x < ls; ++x) {
					if (pcc[fli[x]] == 10) {
						FLAG2L(349, s, fli[x]);
						break;
					}
				}
			}
			// Find VII after
			pos2 = min(fli_size - 1, ls + fis_g_max2);
			if (svoices > 1) {
				for (int x = ls + 1; x <= pos2; ++x) {
					if (apcc[0][fli[x]] == 10 || apcc[1][fli[x]] == 10) {
						FLAG2(350, s);
						break;
					}
				}
			}
			else {
				for (int x = ls + 1; x <= pos2; ++x) {
					if (pcc[fli[x]] == 10) {
						FLAG2(350, s);
						break;
					}
				}
			}
		}
	}
	return 0;
}

int CGenCF1::FailMelodyHarmStep(int i, const int* hv, int &count, int &wcount, int &last_flag, int &max_p) {
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
				if (m_pc[fli[i]] == 0 && m_pc[fli[i - 1]] == 4) FLAG3(48, i);
			}
			// Check harmonic penalty	
			pen1 = hsp[chm[i - 1]][chm[i]];
			if (pen1 == 3) FLAG3(99, i);
			else if (pen1 == 1) FLAG3(77, i);
			if (pen1 == 2) {
				FLAG3(57, i);
				if (culm_ls == i) FLAG3(195, i);
				if (abs(m_cc[fli[i]]-m_cc[fli[i-1]]) > hsp_leap) FLAG3(194, i);
				++p2c;
				if (p2c == 2) FLAG3(92, i + 1);
				else if (p2c == 3) FLAG3(23, i + 1);
			}
			else {
				p2c = 0;
			}
		}
		// Check letter repeat and miss
		if (FailMelodyHarmStep(i, hvt, tcount, wtcount, last_flag, max_p)) return 1;
		if (FailMelodyHarmStep(i, hvd, dcount, wdcount, last_flag, max_p)) return 1;
		if (FailMelodyHarmStep(i, hvs, scount, wscount, last_flag, max_p)) return 1;
	}
	return 0;
}

int CGenCF1::FailMelodyHarm(vector<int> &pc, vector<int> &pcc) {
	CHECK_READY(DR_fli, DR_pc);
	CHECK_READY_PERSIST(DP_hv, DP_hsp);
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
			random_shuffle(hm[ls].begin(), hm[ls].end());
		}
	}
	// Scan vector
	vector<int> chm_saved;
	chm.clear();
	chm_alter.clear();
	chm_alter.resize(fli_size, 0);
	chmp.clear();
	chm.resize(fli_size, 0);
	chmp.resize(fli_size, 0);
	for (int i = 0; i < fli_size; ++i) {
		chm[i] = hm[i][0];
		if (minor_cur && (pcc[fli[i]] == 9 || pcc[fli[i]] == 11)) chm_alter[i] = 1;
		if (minor_cur && (pcc[fli[i]] == 8 || pcc[fli[i]] == 10)) chm_alter[i] = -1;
	}
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
		if (need_exit) return 1;
		if (!hp) {
			++hp;
			if (hp > max_p) max_p = hp;
			goto check;
		}
		//if (task == tEval) {
		//	LogCantus("chm", hp, hp + 1, chm);
		//	LogCantus("cc", 0, ep2, m_cc);
		//}
		//LogCantus(chm);
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
	SET_READY_PERSIST(DP_cc_incr);
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
	CHECK_READY(DR_c);
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
	SET_READY(DR_c);
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
	CHECK_READY(DR_fli, DR_c);
	// Do not test if flag disabled and not testing
	if (task != tEval && accept[flag] == -1) return 0;
	int ok, f, f1;
	if (fli_size > rlen*2) for (ls = 0; ls < fli_size - rlen * 2; ++ls) {
		s = fli2[ls];
		s1 = fli2[ls + 1];
		if (MELODY_SEPARATION(s, s1)) {
			// Search for repeat of note at same beat until last three notes
			int finish = ls + scan_len;
			if (finish > fli_size - rlen) finish = fli_size - rlen;
			// Do not analyse last note if end of scan window
			if (finish >= fli_size - rlen && ep2 < c_len) --finish;
			for (int x = ls + 2; x <= finish; ++x) {
				f = fli2[x];
				f1 = fli2[x + 1];
				// Check rhythm
				if ((f - s) % 2) continue;
				if (MELODY_SEPARATION(f, f1)) {
					// Check if same note
					if (c[f] == c[s] && llen[x] == llen[ls]) {
						// Check that more notes repeat
						ok = 0;
						for (int z = 1; z < rlen; ++z) {
							if (c[fli[x + z]] != c[fli[ls + z]] || llen[x+z] != llen[ls+z]) {
								ok = 1;
								break;
							}
						}
						if (!ok) {
							FLAG2L(flag, fli[ls], fli[ls + rlen - 1]);
						}
					}
				}
			}
		}
	}
	return 0;
}

int CGenCF1::FailLongRepeat(vector<int> &c, vector<int> &cc, vector<int> &leap, int scan_len, int rlen, int flag) {
	CHECK_READY(DR_fli, DR_c);
	// Do not test if flag disabled and not testing
	if (task != tEval && accept[flag] == -1) return 0;
	int ok;
	int f, f1;
	if (fli_size > rlen + 1) for (ls = 0; ls < fli_size - rlen - 1; ++ls) {
		s = fli2[ls];
		s1 = fli2[ls + 1];
		int finish = ls + scan_len;
		if (finish > fli_size - rlen) finish = fli_size - rlen;
		// Do not analyse last note if end of scan window
		if (finish >= fli_size - rlen && ep2 < c_len) --finish;
		for (int x = ls + rlen; x <= finish; ++x) {
			f = fli2[x];
			f1 = fli2[x + 1];
			// Check if same note
			if (c[f] == c[s] && llen[x] == llen[ls]) {
				// Check that more notes repeat
				ok = 0;
				for (int z = 1; z < rlen; ++z) {
					if (c[fli2[x + z]] != c[fli2[ls + z]] || llen[x + z] != llen[ls + z]) {
						ok = 1;
						break;
					}
				}
				if (!ok) {
					FLAG2L(flag, fli[ls], fli[ls + rlen - 1]);
				}
			}
		}
	}
	return 0;
}

int CGenCF1::FailManyLeaps(vector<int> &c, vector<int> &cc, vector<int> &leap, vector<int> &smooth, vector<int> &slur, int mleaps, int mleaped, int mleaps2, int mleaped2, int mleapsteps, int flag1, int flag2, int flag3, int flag4) {
	CHECK_READY(DR_fli, DR_c);
	int leap_sum = 0;
	int leaped_sum = 0;
	pm_win_leaps = 0;
	pm_win_leapnotes = 0;
	int leap_sum_i = 0;
	g_leaps[fli_size - 1] = 0;
	g_leaped[fli_size - 1] = 0;
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
			leaped_sum -= abs(c[fli[ls - mleapsteps]] - c[fli[ls - mleapsteps + 1]]) + 1;
		}
		// Get maximum leap_sum
		if (leap_sum > pm_win_leaps) {
			pm_win_leaps = leap_sum;
			leap_sum_i = ls;
		}
		if (leaped_sum > pm_win_leapnotes) {
			pm_win_leapnotes = leaped_sum;
			leap_sum_i = ls;
		}
		// Record for graph
		g_leaps[ls] = leap_sum;
		g_leaped[ls] = leaped_sum;
		// Calculate penalty 
		if (leap_sum > mleaps) {
			if (!accept[flag1]) ++fpenalty[flag1];
		}
		else if (leap_sum > mleaps2) {
			if (!accept[flag3]) ++fpenalty[flag3];
		}
		if (leaped_sum > mleaped) {
			if (!accept[flag2]) ++fpenalty[flag2];
		}
		else if (leaped_sum > mleaped2) {
			if (!accept[flag4]) ++fpenalty[flag4];
		}
	}
	if (pm_win_leaps > mleaps) 
		FLAG2L(flag1, fli[leap_sum_i + 1], fli[max(0, leap_sum_i - mleapsteps)]);
	else if (pm_win_leaps > mleaps2) 
		FLAG2L(flag3, fli[leap_sum_i + 1], fli[max(0, leap_sum_i - mleapsteps)]);
	if (pm_win_leapnotes > mleaped) 
		FLAG2L(flag2, fli[leap_sum_i + 1], fli[max(0, leap_sum_i - mleapsteps)]);
	else if (pm_win_leapnotes > mleaped2) 
		FLAG2L(flag4, fli[leap_sum_i + 1], fli[max(0, leap_sum_i - mleapsteps)]);
	return 0;
}

// Calculate global leap smooth slur variables
void CGenCF1::GetLeapSmooth(vector<int> &c, vector<int> &cc, vector<int> &leap, vector<int> &smooth, vector<int> &slur) {
	CHECK_READY(DR_c);
	SET_READY(DR_leap, DR_slur);
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
}

// Check if too many leaps
int CGenCF1::FailLeapSmooth(vector<int> &c, vector<int> &cc, vector<int> &leap, vector<int> &smooth, vector<int> &slur, int l_max_smooth, int l_max_smooth_direct, int flag1, int flag2, int first_run) {
	CHECK_READY(DR_leap, DR_c, DR_fli);
	// Clear variables
	int leap_sum2 = 0;
	int thirds_sum = 0;
	int leap_sum_corrected = 0;
	int max_leap_sum2 = 0;
	int smooth_sum = 0;
	int smooth_sum2 = 0;
	int leap_sum_s2 = 0;
	int fired4 = 0, fired5 = 0;
	pm_leaps2 = 0;
	pm_leaps3 = 0;
	for (ls = 0; ls < fli_size - 2; ++ls) {
		s = fli2[ls];
		s1 = fli2[ls+1];
		// Add new leap
		if (leap[s] != 0) {
			++leap_sum2;
			if (abs(c[s1] - c[s]) == 2) ++thirds_sum;
		}
		else {
			leap_sum2 = 0;
			thirds_sum = 0;
		}
		// Record
		if (leap_sum2 == 2) ++pm_leaps2;
		else if (leap_sum2 == 3) ++pm_leaps3;
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
			if (smooth_sum >= l_max_smooth) {
				if (fired4) {
					fpenalty[flag1] += severity[flag1] + 1;
				}
				else {
					FLAG2L(flag1, fli[ls + 1], fli[ls - smooth_sum + 1]);
					fired4 = 1;
				}
			}
		}
		else if (leap[s]) smooth_sum = 0;
		if (ls < fli_size - 2) {
			// Prohibit long smooth movement in one direction
			if (smooth[s] != 0 && smooth[s] == smooth[fli2[ls+1]]) {
				++smooth_sum2;
				if (smooth_sum2 >= l_max_smooth_direct) {
					if (fired5) {
						fpenalty[flag2] += severity[flag2] + 1;
					}
					else {
						FLAG2L(flag2, fli[ls + 1], fli[ls - smooth_sum2 + 1]);
						fired5 = 1;
					}
				}
			}
			else if (smooth[s] || leap[s]) smooth_sum2 = 0;
			// Check if two notes repeat
			if (first_run && ls > 0 && (cc[s] == cc[fli2[ls + 2]]) && 
				(cc[fli2[ls - 1]] == cc[fli2[ls + 1]]) &&
				(ep2 == c_len || ls + 2 < fli_size - 1)) {
				if (svoices == 1 || species == 1 || species == 4) {
					// Same rhythm in first notes of repeat?
					if (llen[ls - 1] == llen[ls + 1]) {
						if (llen[ls - 1] == llen[ls]) {
							FLAG2L(402, fli[ls - 1], fli[ls + 2]);
						}
						else FLAG2L(403, fli[ls - 1], fli[ls + 2]);
					}
					else FLAG2L(404, fli[ls - 1], fli[ls + 2]);
				}
				else if (species == 2 || species == 3) {
					if (bmli[fli[ls - 1]] == bmli[fli[ls + 2]]) {
						// Same rhythm in first notes of repeat?
						if (llen[ls - 1] == llen[ls + 1]) {
							if (llen[ls - 1] == llen[ls]) {
								FLAG2L(411, fli[ls - 1], fli[ls + 2]);
							}
							else FLAG2L(412, fli[ls - 1], fli[ls + 2]);
						}
						else FLAG2L(413, fli[ls - 1], fli[ls + 2]);
					}
					else {
						// Same rhythm in first notes of repeat?
						if (llen[ls - 1] == llen[ls + 1]) {
							if (llen[ls - 1] == llen[ls]) {
								FLAG2L(408, fli[ls - 1], fli[ls + 2]);
							}
							else FLAG2L(409, fli[ls - 1], fli[ls + 2]);
						}
						else FLAG2L(410, fli[ls - 1], fli[ls + 2]);
					}
				}
				else if (species == 5) {
					if (bmli[fli[ls - 1]] == bmli[fli[ls + 2]]) {
						// Same rhythm in first notes of repeat?
						if (llen[ls - 1] == llen[ls + 1]) {
							if (llen[ls - 1] == llen[ls]) {
								FLAG2L(405, fli[ls - 1], fli[ls + 2]);
							}
							else FLAG2L(406, fli[ls - 1], fli[ls + 2]);
						}
						else FLAG2L(407, fli[ls - 1], fli[ls + 2]);
					}
					else {
						// Same rhythm in first notes of repeat?
						if (llen[ls - 1] == llen[ls + 1]) {
							if (llen[ls - 1] == llen[ls]) {
								FLAG2L(9, fli[ls - 1], fli[ls + 2]);
							}
							else FLAG2L(320, fli[ls - 1], fli[ls + 2]);
						}
						else FLAG2L(319, fli[ls - 1], fli[ls + 2]);
					}
				}
			}
		}
	}
	if (first_run && max_leap_sum2 >= cse_leaps) {
		if (max_leap_sum2 > cse_leaps2) 
			FLAG2L(71, fli[bli[leap_sum_s2] + 1], fli[max(0, bli[leap_sum_s2] - max_leap_sum2 + 1)]);
		else FLAG2L(70, fli[bli[leap_sum_s2] + 1], fli[max(0, bli[leap_sum_s2] - max_leap_sum2 + 1)]);
	}
	return 0;
}

int CGenCF1::FailStagnation(vector<int> &cc, vector<int> &nstat, int steps, int notes, int flag) {
	CHECK_READY(DR_nmin, DR_fli);
	// Do not test if flag disabled and not evaluating
	if (task != tEval && accept[flag] == -1) return 0;
	// Clear nstat
	for (int i = nmin; i <= nmax; ++i) nstat[i] = 0;
	for (ls = 0; ls < fli_size; ++ls) {
		s = fli[ls];
		// Add new note to stagnation array
		++nstat[cc[s]];
		// Subtract old note
		if (ls >= steps) --nstat[cc[fli[ls - steps]]];
		// Check if too many repeating notes
		if (nstat[cc[s]] > notes) FLAG2L(flag, s, fli[max(0, ls - steps)]);
	}
	return 0;
}

// Prohibit multiple culminations
int CGenCF1::FailMultiCulm(vector<int> &cc, vector<int> &slur) {
	CHECK_READY(DR_fli);
	SET_READY(DR_culm_ls);
	pm_culm_count = 0;
	culm_ls = -1;
	// Do not find culminations if too early
	if (ep2 < c_len) return 0;
	for (ls = 0; ls < fli_size; ++ls) {
		if (cc[fli[ls]] == nmax) {
			++pm_culm_count;
			culm_ls = ls;
			if (pm_culm_count > 1) {
				if (voice_high) FLAG2(12, fli[culm_ls]);
				else FLAG2(305, fli[culm_ls]);
			}
		}
	}
	if (culm_ls == -1) {
		culm_ls = 0;
		CString est;
		est.Format("Warning: culm_ls cannot be detected at step %d", step);
		WriteLog(5, est);
	}
	if (voice_high) {
		// Prohibit culminations at first steps
		if (culm_ls < (early_culm3 * fli_size) / 100) FLAG2(193, fli[culm_ls]);
		if (culm_ls < early_culm - 1) FLAG2(78, fli[culm_ls]);
		else if (culm_ls < early_culm2 - 1) FLAG2(79, fli[culm_ls]);
		// Prohibit culminations at last steps
		if (culm_ls >= fli_size - late_culm) FLAG2(21, fli[culm_ls]);
	}
	// Prohibit synchronized culminations
	if (av_cnt > 1 && fli[culm_ls] == cf_culm_s) FLAG2(26, fli[culm_ls]);
	return 0;
}

int CGenCF1::FailFirstNotes(vector<int> &pc) {
	CHECK_READY(DR_fli, DR_pc);
	// Prohibit first note not tonic
	if (pc[0] != 0) {
		if (cantus_high) {
			if (pc[0] == 4) FLAG2(49, 0);
			else if (pc[0] == 2) FLAG2(90, 0);
			else FLAG2(33, 0);
		}
		else {
			FLAG2(33, 0);
		}
	}
	return 0;
}

int CGenCF1::FailLastNotes(vector<int> &pc, vector<int> &pcc) {
	CHECK_READY(DR_fli, DR_pc);
	// Do not check if melody is short yet
	if (fli_size < 3) return 0;
	// Prohibit last note not tonic
	if (ep2 > c_len - 1) {
		s = fli[fli_size - 1];
		s_1 = fli[fli_size - 2];
		s_2 = fli[fli_size - 3];
		if (pc[s] != 0) FLAG2(50, s);
		if (minor_cur) {
			// Prohibit major second up before I
			if (pcc[s] == 0 && pcc[s_1] == 10) FLAG2(74, s_1, s);
			if (pcc[s] == 0 && pcc[s_2] == 10) FLAG2(74, s_2, s);
		}
	}
	// Wrong second to last note (last note never can be slurred)
	if (ep2 > c_len - 2) {
		s_1 = c_len - 2;
		if ((pc[s_1] == 0) || (pc[s_1] == 2) || (pc[s_1] == 3) || (pc[s_1] == 5)) FLAG2(13, s_1);
		if (pc[s_1] == 4) FLAG2(51, s_1);
	}
	// Wrong third to last note
	if (ep2 > c_len - 3) {
		s_2 = c_len - 3;
		if ((pc[s_2] == 0) || (pc[s_2] == 2) || (pc[s_2] == 4)) FLAG2(14, s_2);
		// Leading third to last note
		if (pc[s_2] == 6) FLAG2(34, s_2);
	}
	return 0;
}

void CGenCF1::CreateLinks(vector<int> &cc, int multivoice) {
	SET_READY(DR_fli);
	int prev_note = -1;
	int lpos = 0;
	int l = 0;
	int npm2 = (svoices == 1 ? 1 : npm);
	minl = 10000;
	maxl = 0;
	// Set first steps in case there is pause
	for (int i = 0; i < ep2; ++i) {
		if (prev_note != cc[i]) {
			// Save linked note length
			if (prev_note != -1) {
				llen[lpos - 1] = l;
				rlen[lpos - 1] = l * 8.0 / npm2;
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
	rlen[lpos - 1] = l * 8.0 / npm2;
	// Last note does not affect minl/maxl
	if (multivoice) {
		// For species 5 minl should be at least 4
		// For species 3 minl should be at least 2
		if (npm == 8) minl = max(4, minl);
		if (npm == 4) minl = max(2, minl);
	}
}

void CGenCF1::CountFillInit(vector<int> &c, int tail_len, int pre, int &t1, int &t2, int &fill_end) {
	// Create leap tail
	tc.clear();
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
	fill_end = -1;
}
	
void CGenCF1::CountFill(vector<int> &c, int tail_len, vector<int> &nstat2, vector<int> &nstat3, int &skips, int &fill_to, int pre, int &fill_to_pre, int &fill_from_pre, int &fill_from, int &deviates, int &dev_count, int leap_prev, int &fill_end) {
	// Leap starting and finishing note
	int t1, t2;
	int cur_deviation = 0;
	int dev_state = 0;
	int max_deviation = 0;
	if (accept[42 + leap_id]) max_deviation = 1;
	if (accept[120 + leap_id]) max_deviation = 2;
	CountFillInit(c, tail_len, pre, t1, t2, fill_end);
	// Detect fill_end
	deviates = 0;
	int dl2 = dev_late2;
	int dl3 = dev_late3;
	if (leap_size > 4) {
		dl2 = 1;
		dl3 = 2;
	}
	// Deviation state: 0 = before deviation, 1 = in deviation, 2 = after deviation, 3 = multiple deviations
	for (int x = 0; x < tc.size(); ++x) {
		// If deviating, start deviation state and calculate maximum deviation
		if (tc[x] > t2) {
			cur_deviation = tc[x] - t2;
			// Detect long deviation for >5th 2nd
			if (cur_deviation == 1 && x == 0 && leap_size > 4 && fleap_end < fli_size - 1 && 
				rlen[fleap_end + 1] > dev2_maxlen * 2 && !accept[386]) break;
			// Detect late deviation
			if (cur_deviation == 1 && x >= dl2 && !accept[191]) break;
			if (cur_deviation == 2 && x >= dl3 && !accept[192]) break;
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
		int pos;
		if (fill_to == 2) pos = max(0, fleap_start - fill_pre3_notes);
		else pos = max(0, fleap_start - fill_pre4_notes);
		vector<int> nstat4;
		nstat4.resize(2, 0);
		if (c[leap_start] < c[leap_end]) {
			for (int x = pos; x < fleap_start; ++x) {
				if (c[fli[x]] == c[leap_start] + 1) nstat4[0] = 1;
				else if (c[fli[x]] == c[leap_start] + 2) nstat4[1] = 1;
			}
		}
		else {
			for (int x = pos; x < fleap_start; ++x) {
				if (c[fli[x]] == c[leap_start] - 1) nstat4[0] = 1;
				else if (c[fli[x]] == c[leap_start] - 2) nstat4[1] = 1;
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
		int pos;
		if (fill_from == 2) pos = max(0, fleap_start - fill_pre3_notes);
		else pos = max(0, fleap_start - fill_pre4_notes);
		vector<int> nstat4;
		nstat4.resize(2, 0);
		if (c[leap_start] < c[leap_end]) {
			for (int x = pos; x < fleap_start; ++x) {
				if (c[fli[x]] == c[leap_end] - 1) nstat4[0] = 1;
				else if (c[fli[x]] == c[leap_end] - 2) nstat4[1] = 1;
			}
		}
		else {
			for (int x = pos; x < fleap_start; ++x) {
				if (c[fli[x]] == c[leap_end] + 1) nstat4[0] = 1;
				else if (c[fli[x]] == c[leap_end] + 2) nstat4[1] = 1;
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
	// leap_size = 4 for 5th
	leap_size = abs(c[leap_end] - c[s]);
	// Next is leap?
	if (fleap_end < fli_size - 1) leap_next = leap[leap_start] * leap[leap_end];
	// Prev is leap?
	if (fleap_start > 0) leap_prev = leap[leap_start] * leap[fli2[fleap_start] - 1];
	// Late leap?
	late_leap = fli_size - fleap_start;
	// Find late leap border 
	c4p_last_notes2 = min(c4p_last_notes, fli_size - bli[max(0, ep2 - c4p_last_steps)]);
}

int CGenCF1::FailLeapMulti(int leap_next, int &arpeg, int &overflow, int &child_leap, vector<int> &c, vector<int> &leap) {
	child_leap = 0; // If we have a child_leap
	// Check if leap is third
	if (leap_size == 2) {
		// Check if leap is second third
		if (fleap_start > 0 && abs(c[leap_end] - c[fli2[fleap_start-1]]) == 4 &&
			abs(c[leap_start] - c[fli2[fleap_start - 1]]) == 2) {
			// Set middle leap note
			leap_mid = leap_start;
			// Set leap start to first note of first third
			--fleap_start;
			leap_start = fli2[fleap_start];
			// Set leap size to be compound
			leap_size = 4;
			// If 6/8 goes before 2 thirds (tight)
			if ((fleap_start > 0) && ((leap[leap_start] * (c[leap_start] - c[fli2[fleap_start - 1]]) == -5) ||
				(leap[leap_start] * (c[leap_start] - c[fli2[fleap_start - 1]]) == -7))) 
				FLAG2L(28, fli[fleap_start], fli[fleap_start + 2]);
				// Else mark simple 2x3rds
			else FLAG2L(6, fli[fleap_start], fli[fleap_start + 2]);
		}
	}			
	leap_id = min(leap_size - 2, 3);
	if (fleap_end < fli_size - 1) {
		// Next leap in same direction
		if (leap_next > 0) {
			// Flag if greater than two thirds
			if (abs(c[fli2[fleap_end + 1]] - c[leap_start]) > 4) 
				FLAG2L(27, fli[fleap_start], fli[bli[leap_end] + 1]);
				// Allow if both thirds, without flags (will process next cycle)
			else arpeg=1;
		}
		// Next leap back
		else if (leap_next < 0) {
			int leap_size2 = abs(c[fli2[fleap_end + 1]] - c[leap_end]);
			// Flag if back leap greater than 6th
			if (leap_size2 > 5) FLAG2(22, fli[bli[leap_end]]);
				// Flag if back leap equal or smaller than 6th
			else FLAG2(8, fli[bli[leap_end]]);
			// Flag leap back overflow
			if (leap_size2 > leap_size) {
				FLAG2(58, fli[bli[leap_end]]);
				overflow = 1;
			}
		}
	}
	// Check if we have a greater opposite neighbouring leap
	if ((fleap_end < fli_size - 1 && abs(c[fli2[fleap_end + 1]] - c[leap_end]) >= leap_size-1 && leap[leap_start] * leap[leap_end]<0) ||
		(fleap_start > 0 && abs(c[leap_start] - c[fli2[fleap_start - 1]]) > leap_size && leap[leap_start] * leap[fli2[fleap_start - 1]]<0)) {
		// Set that we are preleaped (even if we are postleaped)
		child_leap = 1;
	}
	return 0;
}

int CGenCF1::FailLeap(vector<int> &c, vector<int>& cc, vector<int> &leap, vector<int> &smooth, vector<int> &nstat2, vector<int> &nstat3) {
	CHECK_READY(DR_leap, DR_c, DR_fli);
	// Get leap size, start, end
	// Check if leap is compensated (without violating compensation rules)
	// If leap is not compensated, check uncompensated rules
	// If uncompensated rules not allowed, flag compensation problems detected (3rd, etc.)
	int child_leap, leap_next, leap_prev, presecond;
	int overflow, arpeg, late_leap;
	for (s = 0; s < ep2 - 1; ++s) {
		if (leap[s] != 0) {
			ls = bli[s];
			FailLeapInit(c, late_leap, presecond, leap_next, leap_prev,
				arpeg, overflow, leap);
			if (FailLeapMulti(leap_next, arpeg, overflow, child_leap, c, leap)) return 1;
			// If leap back overflow or arpeggio, do not check leap compensation, because compensating next leap will be enough
			if (!overflow && !arpeg)
				if (FailLeapFill(c, late_leap, leap_prev, child_leap)) return 1;
			if (!arpeg) if (FailLeapMDC(leap, cc)) return 1;
		}
	}
	return 0;
}

int CGenCF1::FailLeapFill(vector<int> &c, int late_leap, int leap_prev, int child_leap) {
	// Prefill parameters
	int ptail_len, pfill_to, pfill_to_pre, pfill_from_pre, pfill_from, pdeviates, pfill_end, pdev_count;
	// Fill parameters
	int tail_len, fill_to, fill_to_pre, fill_from_pre, fill_from, deviates, fill_end, dev_count;
	filled = 1;
	prefilled = 0;
	int pskips = 10;
	int skips = 10;
	// Calculate allowed skips 
	int allowed_skips = 1;
	if (leap_size > 6) ++allowed_skips;
	if (late_leap <= c4p_last_notes2 + 1) ++allowed_skips;
	int allowed_pskips = 1;
	if (leap_size > 6) ++allowed_pskips;
	// Check if leap is filled
	tail_len = 2 + (leap_size - 1) * fill_steps_mul;
	// Do not check fill if search window is cut by end of current not-last scan window
	if ((fleap_end + tail_len < fli_size) || (c_len == ep2)) {
		// Check fill only if enough length (checked second time in case of slurs)
		CountFill(c, tail_len, nstat2, nstat3, skips, fill_to, 0, fill_to_pre, fill_from_pre,
			fill_from, deviates, dev_count, leap_prev, fill_end);
		if (skips > allowed_skips) filled = 0;
		else if (fill_to > 3) filled = 0;
		else if (fill_to >= 3 && fill_to <= fill_pre4_int && (!fill_to_pre || late_leap > c4p_last_notes2 + 1 || !accept[144 + leap_id])) filled = 0;
		else if (fill_to == 2 && (fill_to_pre || !fleap_start) && !accept[100 + leap_id]) filled = 0;
		else if (fill_to == 2 && !fill_to_pre && fleap_start && !accept[104 + leap_id]) filled = 0;
		else if (fill_from > 3) filled = 0;
		else if (fill_from >= 3 && fill_from <= fill_pre4_int && (!fill_from_pre || late_leap > c4p_last_notes2 + 1 || !accept[144 + leap_id])) filled = 0;
		else if (fill_from == 2 && !accept[53 + leap_id]) filled = 0;
		else if (deviates > 2) filled = 0;
		else if (deviates == 1 && !accept[42 + leap_id]) filled = 0;
		else if (deviates == 2 && !accept[120 + leap_id]) filled = 0;
		if (!filled) {
			// If starting 3rd
			if (fleap_start == 0 && leap_size == 2 && accept[1]) {
				FLAG2(1, fli[fleap_start]);
				return 0;
			}
			if (child_leap && accept[116 + leap_id]) FLAG2(116 + leap_id, fli[fleap_start]);
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
					if (late_leap <= pre_last_leaps + 1) FLAG2(204 + leap_id, fli[fleap_start]);
					else FLAG2(112 + leap_id, fli[fleap_start]);
				}
				else FLAG2(124 + leap_id, fli[fleap_start]);
			}
		}
		// Show compensation flags only if successfully compensated
		// This means that compensation errors are not shown if uncompensated (successfully or not)
		else {
			// Flag late uncompensated precompensated leap
			if (fill_to >= 3 && fill_to <= fill_pre4_int && late_leap <= c4p_last_notes2 + 1)
				FLAG2(144 + leap_id, fli[fleap_start]);
			else if (fill_from >= 3 && fill_from <= fill_pre4_int && late_leap <= c4p_last_notes2 + 1)
				FLAG2(144 + leap_id, fli[fleap_start]);
			// Flag prepared unfinished fill if it is not blocking 
			else if (fill_to == 2 && (fill_to_pre || !fleap_start)) FLAG2(100 + leap_id, fli[fleap_start]);
			// Flag unfinished fill if it is not blocking
			else if (fill_to == 2 && !fill_to_pre) FLAG2(104 + leap_id, fli[fleap_start]);
			// Flag after 3rd if it is not blocking
			if (fill_from == 2) FLAG2(53 + leap_id, fli[fleap_start]);
			// Flag deviation if it is not blocking
			if (deviates == 1) FLAG2(42 + leap_id, fli[fleap_start]);
			// Flag deviation if it is not blocking
			if (deviates == 2) FLAG2(120 + leap_id, fli[fleap_start]);
		}
	}
	return 0;
}

int CGenCF1::FailLeapMDC(vector<int> &leap, vector<int> &cc) {
	// Melody direction change (MDC)
	// 0 = close, 1 = far, 2 = no
	// Default mdc is close, because beginning equals to close mdc
	mdc1 = 0;
	int prev_note = cc[leap_start];
	for (int pos = leap_start - 1; pos >= 0; --pos) {
		if (cc[pos] != prev_note) {
			// Check if direction changes or long without changes
			if (leap[leap_start] * (cc[pos] - prev_note) > 0 || mdc1 > 1) break;
			prev_note = cc[pos];
			++mdc1;
		}
	}
	mdc2 = 0;
	prev_note = cc[leap_end];
	for (int pos = leap_end + 1; pos < ep2; ++pos) {
		if (cc[pos] != prev_note) {
			// Check if direction changes or long without changes
			if (leap[leap_start] * (cc[pos] - prev_note) < 0 || mdc2 > 2) break;
			prev_note = cc[pos];
			++mdc2;
		}
	}
	// Do not flag last 3rd in SAS, because it can be later converted to 5th
	if (fleap_end == fli_size - 1 && ep2 < c_len && !leap_id) return 0;
	// Close + next
	if (!mdc1 && mdc2 == 1) FLAG2(128 + leap_id, fli[fleap_start]);
		// Close + far
	else if (!mdc1 && mdc2 == 2) FLAG2(140 + leap_id, fli[fleap_start]);
		// Close + no
	else if (!mdc1 && mdc2 == 3) FLAG2(108 + leap_id, fli[fleap_start]);
		// Far + close
	else if (mdc1 == 1 && !mdc2) FLAG2(59 + leap_id, fli[fleap_start]);
		// Far + close
	else if (mdc1 == 2 && !mdc2) FLAG2(132 + leap_id, fli[fleap_start]);
		// Next + next
	else if (mdc1 == 1 && mdc2 == 1) FLAG2(63 + leap_id, fli[fleap_start]);
	// Next + far
	else if (mdc1 == 1 && mdc2 >= 2) FLAG2(391 + leap_id, fli[fleap_start]);
	// Far + next
	else if (mdc1 >= 2 && mdc2 == 1) FLAG2(148 + leap_id, fli[fleap_start]);
	// Far + far
	else if (mdc1 >= 2 && mdc2 >= 2) FLAG2(398 + leap_id, fli[fleap_start]);
	return 0;
}

void CGenCF1::PrepareTonicWeight() {
	tonic_weight.resize(9);
	for (int i = 1; i <= 8; ++i) {
		tonic_weight[i] = 1.0 - tonic_wei_len / 100.0 * (3.0 - log2(i));
	}
}

float CGenCF1::GetTonicWeight(int l_ls, vector<int> &cc, vector<int> &pc) {
	int l_s = fli[l_ls];
	// Get note length
	float len2 = rlen[l_ls];
	// If suspension, decrease note length
	if (svoices > 1 && sus[l_ls]) len2 /= 2;
	// Get initial weight based on length
	float tw = tonic_weight[min(8, len2)];
	// Correct weight based on leap
	if (l_ls > 0 && abs(cc[l_s] - cc[fli[l_ls] - 1]) > tonic_leap)
		tw *= 1.0 + tonic_wei_leap / 100.0;
	if (svoices > 1) {
		if (npm > 1 && !beat[l_ls]) tw *= 1.0 + tonic_wei_beat / 100.0;
		if (l_ls > 0 && llen[l_ls] > llen[l_ls - 1]) 
			tw *= 1.0 + tonic_wei_long / 100.0;
		if (tivl[l_s] == iPco) tw *= 1.0 + tonic_wei_pco / 100.0;
	}
	return tw;
}

int CGenCF1::FailTonic(vector<int> &cc, vector<int> &pc) {
	vector<float> tcount;
	int s9;
	pm_tw_max = 0;
	tcount.resize(13);
	int fire, fired = 0;
	// Do not check if melody is short
	if (fli_size < 3) return 0;
	tweight[0] = 0;
	tweight[fli_size - 1] = 0;
	// Loop from second to second to last note
	for (ls = 1; ls < fli_size-1; ++ls) {
		s = fli[ls];
		s_1 = fli[ls-1];
		// Decrement for previous tonic note
		if (ls > tonic_window) {
			s9 = fli[ls - tonic_window];
			if (!pc[s9]) {
				tcount[cc[s9] / 12] -= GetTonicWeight(ls - tonic_window, cc, pc);
			}
		}
		if (!pc[s]) {
			// Increment for current tonic note
			tcount[cc[s] / 12] += GetTonicWeight(ls, cc, pc);
			if (tcount[cc[s] / 12] > pm_tw_max) pm_tw_max = tcount[cc[s] / 12];
			// Check count of tonic notes
			if (tcount[cc[s] / 12] > tonic_max) {
				// Grant one more tonic in first window if first note not tonic
				fire = 0;
				if (ls < tonic_window && !pc[0]) {
					if (tcount[cc[s] / 12] > tonic_max + 1)	fire = 1;
				}
				else fire = 1;
				if (fire) {
					if (fired) {
						fpenalty[196] += severity[196] + 1;
					}
					else {
						FLAG2L(196, s, fli[max(0, ls - tonic_window)]);
						fired = 1;
					}
				}
			}
		}
		tweight[ls] = vmax(tcount);
	}
	return 0;
}

int CGenCF1::FailLastNoteRes(vector<int> &pc) {
	CHECK_READY(DR_pc, DR_fli);
	if (ep2 < c_len) return 0;
	if (fli_size < 2) return 0;
	if (pc[fli[fli_size - 2]] == 6 && pc[c_len - 1] != 0) FLAG2(52, fli[fli_size - 2]);
	if (pc[fli[fli_size - 2]] == 3 && pc[c_len - 1] != 2) FLAG2(87, fli[fli_size - 2]);
	return 0;
}

int CGenCF1::FailIntervals(vector<int> &c, vector<int> &cc, vector<int> &pc, vector<int> &pcc) {
	CHECK_READY(DR_pc, DR_c, DR_fli);
	for (ls = 0; ls < fli_size - 1; ++ls) {
		s0 = fli[ls];
		s = fli2[ls];
		s1 = fli2[ls + 1];
		// Leap size prohibit
		if (cc[s1] - cc[s] == 8) FLAG2(175, s0);
		else if (cc[s1] - cc[s] == -8) FLAG2(181, s0);
		else if (cc[s1] - cc[s] == 9) FLAG2(176, s0);
		else if (cc[s1] - cc[s] == -9) FLAG2(182, s0);
		else if (cc[s1] - cc[s] == 10) FLAG2(177, s0);
		else if (cc[s1] - cc[s] == -10) FLAG2(183, s0);
		else if (cc[s1] - cc[s] == 11) FLAG2(178, s0);
		else if (cc[s1] - cc[s] == -11) FLAG2(184, s0);
		else if (cc[s1] - cc[s] == 12) FLAG2(179, s0);
		else if (cc[s1] - cc[s] == -12) FLAG2(185, s0);
		else if (cc[s1] - cc[s] > 12) FLAG2(180, s0);
		else if (cc[s1] - cc[s] < -12) FLAG2(186, s0);
		// Prohibit BB
		if (pcc[fli[ls + 1]] == 11 && pcc[s] == 11) FLAG2(348, s0);
	}
	return 0;
}

// Check tritone t1-t2 which has to resolve from ta to tb. Use fleap_start/fleap_end
void CGenCF1::GetTritoneResolution(int ta, int t1, int t2, int tb, int &res1, int &res2, vector<int> &c, vector<int> &cc, vector<int> &pc, vector<int> &pcc) {
	res1 = 0;
	res2 = 0;
	// Real resolution notes
	int ta2, tb2; 
	// Get real resolution notes
	if (pcc[fli[fleap_end]] == t2) {
		ta2 = ta;
		tb2 = tb;
	}
	else {
		ta2 = tb;
		tb2 = ta;
	}
	// Outer resolution is not a resolution
	if (cc[fli[fleap_end]] > cc[fli[fleap_start]] && ta2 < pcc[fli[fleap_start]]) return;
	// Get resolution window
	int rwin = 1;
	if (svoices > 1) rwin = max(1, (npm * tritone_res_quart) / 4);
	// Scan preparation
	if (fleap_start > 0) {
		int pos1 = max(0, fli[fleap_start] - rwin);
		int pos2 = min(ep2, fli[fleap_end]);
		for (int i = pos1; i < pos2; ++i) {
			if (pcc[i] == ta2 && abs(cc[i] - cc[fli[fleap_start]]) < 5) {
				res1 = 1;
				break;
			}
		}
	} 
	// Do not check if cut by scan window
	if (fli2[fleap_end] + 1 + rwin > ep2 && ep2 < c_len) {
		res2 = 1;
		return;
	}
	// Scan resolution
	if (fleap_end < fli_size - 1) {
		int pos1 = max(0, fli2[fleap_start] + 1);
		int pos2 = min(ep2, fli2[fleap_end] + 1 + rwin);
		for (int i = pos1; i < pos2; ++i) {
			if (pcc[i] == tb2 && abs(cc[i] - cc[fli[fleap_end]]) < 5) {
				res2 = 1;
				break;
			}
		}
	}
}

// Check tritone t1-t2 which has to resolve from ta to tb
int CGenCF1::FailTritone(int ta, int t1, int t2, int tb, vector<int> &c, vector<int> &cc, vector<int> &pc, vector<int> &pcc, vector<int> &leap) {
	int found;
	int res1 = 0; // First note resolution flag
	int res2 = 0; // Second note resolution flag
								// Tritone prohibit
	leap_start = s;
	found = 0;
	// Check consecutive tritone
	if ((pcc[s1] == t2 && pcc[s] == t1) || (pcc[s1] == t1 && pcc[s] == t2)) found = 1;
	// Check tritone with additional note inside
	else if (ls > 0) {
		// Check pitches
		if ((pcc[s1] == t2 && pcc[s_1] == t1) || (pcc[s1] == t1 && pcc[s_1] == t2))
			// Check intermediate note and mdc
			if (c[s] > c[s1] && c[s] < c[s_1]) {
				if ((ls < 2 || c[s_2] < c[s_1] || leap[s_2]) && (ls > fli_size - 3 || c[s2] > c[s1] || leap[s1])) {
					found = 2;
					leap_start = s_1;
				}
			} 
			else if (c[s] < c[s1] && c[s] > c[s_1]) {
				if ((ls<2 || c[s_2] > c[s_1] || leap[s_2]) && (ls>fli_size - 3 || c[s2] < c[s1] || leap[s1])) {
					found = 2;
					leap_start = s_1;
				}
			}
	}
	fleap_start = bli[leap_start];
	fleap_end = bli[s1];
	// Do not check tritone if it is at the end of not-last window
	if (ls == fli_size - 2 && ep2 != c_len) return 0;
	if (found) {
		// Check if tritone is highest leap if this is last window
		if (ep2 == c_len && (svoices == 1 || !cantus_high)) {
			if ((cc[leap_start] >= lclimax[leap_start]) || (cc[s1] >= lclimax[leap_start])) {
				// Consecutive
				if (found == 1) FLAG2(32, s0);
				// Compound framed
				else if (found == 2) FLAG2L(373, fli[ls - 1], fli[ls + 1]); //-V547
			}
		}
		// Check if resolution is correct
		GetTritoneResolution(ta, t1, t2, tb, res1, res2, c, cc, pc, pcc);
		// Flag resolution for consecutive tritone
		if (found == 1) {
			if (res1*res2 == 0) FLAG2(31, s0);
			else FLAG2(2, s0);
		}
		// Flag resolution for tritone with intermediate note, framed
		else if (found == 2) { //-V547
			if (res1*res2 == 0) FLAG2L(372, fli[ls - 1], fli[ls + 1]);
			else FLAG2L(371, fli[ls - 1], fli[ls + 1]);
		}
	}
	return 0;
}

int CGenCF1::FailTritones(vector<int> &c, vector<int> &cc, vector<int> &pc, vector<int> &pcc, vector<int> &leap) {
	CHECK_READY(DR_pc, DR_c, DR_fli);
	CHECK_READY(DR_leap);
	for (ls = 0; ls < fli_size - 1; ++ls) {
		s0 = fli[ls];
		s = fli2[ls];
		s1 = fli2[ls + 1];
		if (ls > 0) s_1 = fli2[ls - 1];
		if (ls > 1) s_2 = fli2[ls - 2];
		if (ls < fli_size - 2) s2 = fli2[ls + 2];
		// Warning: tritone F#C in minor is not detected (can add FailTritone to detect) because it is already prohibited by Unaltered near altered.
		// If you allow Unaltered near altered, you should implement FailTritone for F#C.
		if (minor_cur) {
			if (FailTritone(3, 5, 11, 0, c, cc, pc, pcc, leap)) return 1;
			if (FailTritone(7, 8, 2, 3, c, cc, pc, pcc, leap)) return 1;
		}
		else {
			if (FailTritone(4, 5, 11, 0, c, cc, pc, pcc, leap)) return 1;
		}
	}
	return 0;
}

// Calculate global fill
int CGenCF1::FailGlobalFill(vector<int> &c, vector<int> &nstat2) {
	CHECK_READY(DR_nmin, DR_c);
	// Clear nstat2
	for (int i = nmind; i <= nmaxd; ++i) nstat2[i] = 0;
	// Count nstat2
	for (int x = 0; x < ep2; ++x) ++nstat2[c[x]];
	// Check nstat2
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
	if (skips == 1) FLAG2(67, 0);
	else if (skips >= 2) FLAG2(68, 0);
	return 0;
}


void CGenCF1::ScanInit() {
	if (!is_animating) {
		progress_size = c_len;
		fill(progress.begin(), progress.end(), 0);
		PrepareTonicWeight();
		if (task != tEval) scan_full = 0;
		scan_start_time = time();
		anflags.resize(av_cnt);
		anfl.resize(av_cnt);
		for (int i = 0; i < av_cnt; ++i) {
			anflags[i].resize(c_len, vector<int>(MAX_RULES)); // Flags for each note
			anfl[i].resize(c_len, vector<int>(MAX_RULES)); // Flags for each note
		}
		src_rpenalty_step.resize(c_len);
		uli.resize(c_len);
		fli.resize(c_len);
		fli2.resize(c_len);
		tweight.resize(c_len);
		g_leaps.resize(c_len);
		g_leaped.resize(c_len);
		macc.resize(c_len);
		macc2.resize(c_len);
		decc.resize(c_len);
		decc2.resize(c_len);
		cpos.resize(c_len);
		llen.resize(c_len);
		rlen.resize(c_len);
		lclimax.resize(c_len);
		lclimax2.resize(c_len);
		cc_order.resize(c_len);
		dpenalty_step.resize(c_len);
		cc_id.resize(c_len);
		bli.resize(c_len);
		fpenalty.resize(max_flags);
		wpos1.resize(c_len / s_len + 1);
		wpos2.resize(c_len / s_len + 1);
		swpos1.resize(c_len / s_len + 1);
		swpos2.resize(c_len / s_len + 1);
		min_c.resize(c_len);
		max_c.resize(c_len);
		min_cc.resize(c_len);
		max_cc.resize(c_len);
		hm.resize(c_len);
		//hm2.resize(c_len);
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
		swcount = 1; // Number of SWA windows created
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
		for (int x = 0; x < c_len; ++x) {
			hm[x].resize(3);
			//hm2[x].resize(3);
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
	ep2 = c_len;
	voice_high = cantus_high;
	max_interval = max_interval_cf;
	// Calculate last steps that are allowed to have C4P
	c4p_last_steps = c4p_last_meas;
	// Set green leap limits
	max_leaps5 = max_leaps;
	max_leaped5 = max_leaped;
	max_leaps6 = max_leaps2;
	max_leaped6 = max_leaped2;
	// Set scan voices count
	svoices = 1;
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
	if (minc > src_nmin) minc = src_nmin;
	if (maxc < src_nmax) maxc = src_nmax;
}

void CGenCF1::SingleCantusInit() {
	SET_READY_PERSIST(DP_cc_old);
	// Copy cantus
	m_cc = *scantus;
	// Get diatonic steps from chromatic
	first_note = m_cc[0];
	last_note = m_cc[c_len - 1];
	for (int i = 0; i < c_len; ++i) {
		m_c[i] = CC_C(m_cc[i], tonic_cur, minor_cur);
		// Save value for future use;
		m_cc_old[i] = m_cc[i];
	}
	if (!swa_inrange) {
		minc = src_nmin;
		maxc = src_nmax;
		//GetRealRange(m_c, m_cc);
		//ApplySourceRange();
	}
	// Set pitch limits
	// If too wide range is not accepted, correct range to increase scan performance
	for (int i = 0; i < c_len; ++i) {
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
	for (int i = 0; i < c_len; ++i) {
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
		CalculateCcOrder(m_cc_old, 0, c_len);
		// Clear scan steps
		FillCantusMap(cc_id, smap, 0, smatrixc, 0);
		FillCantusMap(m_cc, smap, 0, smatrixc, cc_order);
		ep2 = GetMaxSmap() + 1;
		if (sp2 == smatrixc) ep2 = c_len;
		dpenalty_step.clear();
		dpenalty_step.resize(c_len, 0);		// End of evaluation window
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
			// Next line is always false, but it is an important part of consistent approach
			//if (swa1 > 0) dpenalty_outside_swa += CalcDpenalty(cantus[cantus_id], m_cc, 0, smap[swa1 - 1]);
			if (swa2 < smap.size()) dpenalty_outside_swa += CalcDpenalty(cantus[cantus_id], m_cc, smap[swa2], c_len - 1);
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

// Resize main scan vectors to current evaluation window size (ep2)
void CGenCF1::ResizeToWindow() {
	if (av_cnt > 1) {
		acc[cpv].resize(ep2);
		ac[cpv].resize(ep2);
		apcc[cpv].resize(ep2);
		apc[cpv].resize(ep2);
		aleap[cpv].resize(ep2);
		asmooth[cpv].resize(ep2);
		aslur[cpv].resize(ep2);
	}
	else {
		m_cc.resize(ep2);
		m_c.resize(ep2);
		m_pcc.resize(ep2);
		m_pc.resize(ep2);
		m_leap.resize(ep2);
		m_smooth.resize(ep2);
		m_slur.resize(ep2);
	}
	bli.resize(ep2);
	macc.resize(ep2);
	macc2.resize(ep2);
	decc.resize(ep2);
	decc2.resize(ep2);
	anflags[cpv].resize(ep2);
	anfl[cpv].resize(ep2);
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
	SET_READY(DR_cc_order);
	int x, x2;
	// First algorithm is needed when you correct existing melody with SAS or ASWA
	if (task == tCor) {
		CHECK_READY_PERSIST(DP_cc_old);
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
	// Second algorithm is needed when you create new melody for RSWA or SAS
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
	for (int i = 0; i < c_len; ++i) {
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
	for (int i = 0; i < c_len; ++i) {
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
	// Absolute maximum of scan range for culminations
	max_cc2 = vmax(max_cc);
	ResizeToWindow();
}

// Calculate flag statistics
void CGenCF1::CalcFlagStat() {
	if (calculate_stat || calculate_correlation) {
		SET_READY_PERSIST(DP_fstat);
		for (int i = 0; i < max_flags; ++i) if (!accept[i]) {
			if (flags[i]) {
				++fstat[i];
				// Calculate correlation
				if (calculate_correlation) {
					SET_READY_PERSIST(DP_fcor);
					for (int z = 0; z < max_flags; ++z) {
						if (flags[z]) ++fcor[i][z];
					}
				}
			}
		}
	}
}

// Calculate flag blocking
int CGenCF1::FailFlagBlock() {
	if (calculate_blocking) {
		SET_READY_PERSIST(DP_fblock);
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
		error = 1;
		return 1;
	}
	return 0;
}

void CGenCF1::NextWindow(vector<int> &cc) {
	if (task == tCor) {
		sp1 = sp2;
		sp2 = sp1 + min(swa_len, s_len);
		if (sp2 > smatrixc) sp2 = smatrixc;
		// Record window
		++wid;
		wpos1[wid] = sp1;
		wpos2[wid] = sp2;
		++wscans[wid];
		// End of evaluation window
		ep2 = GetMaxSmap() + 1;
		ep1 = max(0, GetMinSmap() - 1);
		if (method == mSWA) {
			if (sp2 == swa2) ep2 = c_len;
		}
		else {
			// Reserve last window with maximum length
			if ((smatrixc - sp1 < s_len * 2) && (smatrixc - sp1 > s_len)) sp2 = (smatrixc + sp1) / 2;
			if (sp2 == smatrixc) ep2 = c_len;
		}
		// Minimal position in array to cycle
		pp = sp2 - 1;
		p = smap[pp];
	}
	else {
		sp1 = sp2;
		sp2 = sp1 + s_len; // End of search window
		// Move window forward
		if (!accept[267] && svoices > 1 && sp1 == mli.back()) {
			sp1 = c_len - s_len;
			sp2 = c_len;
		}
		if (sp2 > c_len) sp2 = c_len;
		// Reserve last window with maximum length
		//if ((c_len - sp1 < s_len * 2) && (c_len - sp1 > s_len)) sp2 = (c_len + sp1) / 2;
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
		if (ep2 == c_len && method == mScan) {
			// Show window statistics
			CString est;
			CString st, st2;
			for (int i = 0; i < wcount; ++i) {
				if (i > 0) st2 += ", ";
				st.Format("%d-%d", wpos1[i], wpos2[i]);
				st2 += st;
			}
			//est.Format("Algorithm created %d windows: %s", wcount, st2);
			est.Format("Algorithm created %d windows", wcount);
			WriteLog(0, est);
		}
	}
	if (method == mScan) {
		ResizeToWindow();
		cc[ep2 - 1] = cc_order[ep2 - 1][cc_id[p]];
	}
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
			for (int x = 0; x < ep2; ++x) {
				if (anflags[cpv][x].size()) for (int i = 0; i < anflags[cpv][x].size(); ++i) if (anflags[cpv][x][i] == z) {
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
	SET_READY(DR_rpenalty_cur);
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
		if (anflags[cpv][x].size()) for (int i = 0; i < anflags[cpv][x].size(); ++i) if (!accept[anflags[cpv][x][i]]) {
			rpenalty_cur += severity[anflags[cpv][x][i]] + 1;
		}
	}
	// Add flags penalty
	for (int x = 0; x < max_flags; ++x) {
		if (!accept[x]) rpenalty_cur += fpenalty[x];
	}
	// Save rpenalty for first scan position in swa window
	if (method == mSWA && !src_rpenalty_step[ep2 - 1] && sp1 == swa1 && cc_id[ep2 - 1] == 0) {
		src_rpenalty_step[ep2 - 1] = rpenalty_cur;
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
				anfl[cpv] = br_nfl;
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
	if (method == mScan) ResizeToWindow();
}

void CGenCF1::WritePerfLog() {
	if (cycle > 100) {
		CString est;
		long long ms = time() - scan_start_time;
		est.Format("Scan speed: %.1f cycles/ms (%lld cycles during %lld ms)", (double)cycle / ms, cycle, ms);
		WriteLog(2, est);
	}
}

int CGenCF1::CalcDpenalty(vector<int> &cc1, vector<int> &cc2, int s1, int s2) {
	int dpe = 0;
	for (int z = s1; z <= s2; ++z) {
		int dif = abs(cc1[z] - cc2[z]);
		if (dif) dpe += step_penalty + pitch_penalty * dif;
	}
	return dpe;
}

void CGenCF1::CalcStepDpenalty(vector<int> &cc1, vector<int> &cc2, int i) {
	int dif = abs(cc1[i] - cc2[i]);
	int dpe = 0;
	if (dif) dpe = step_penalty + pitch_penalty * dif;
	if (i > 0) dpenalty_step[i] = dpenalty_step[i - 1] + dpe;
	else dpenalty_step[i] = dpe;
}

int CGenCF1::NextSWA(vector<int> &cc, vector<int> &cc_old) {
	// If we slided to the end, break
	if (swa2 == smatrixc) return 1;
	// Slide window further
	++swa1;
	++swa2;
	ep1 = max(0, GetMinSmap() - 1);
	// Minimal position in array to cycle
	pp = swa2 - 1;
	p = smap[pp];
	// Restore previous step after sliding window
	cc[smap[swa1 - 1]] = cc_old[smap[swa1 - 1]];
	// Clear scan steps of current window
	FillCantusMap(cc_id, smap, swa1, swa2, 0);
	FillCantusMap(cc, smap, swa1, swa2, cc_order);
	// Init new scan window
	sp1 = swa1;
	sp2 = swa1 + min(swa_len, s_len);
	if (sp2 > smatrixc) sp2 = smatrixc;
	wcount = 1;
	wid = 0;
	wpos1[wid] = sp1;
	wpos2[wid] = sp2;
	ep2 = GetMaxSmap() + 1;
	if (sp2 == swa2) ep2 = c_len;
	// Clear scan steps
	FillCantusMap(cc_id, smap, swa1, swa2, 0);
	FillCantusMap(cc, smap, swa1, swa2, cc_order);
	dpenalty_step.clear();
	dpenalty_step.resize(c_len, 0);
	// Prepare dpenalty
	dpenalty_outside_swa = 0;
	if (av_cnt == 2) {
		if (swa1 > 0) dpenalty_outside_swa += CalcDpenalty(cpoint[cantus_id][cpv], acc[cpv], 0, smap[swa1 - 1]);
		if (swa2 < smap.size()) dpenalty_outside_swa += CalcDpenalty(cpoint[cantus_id][cpv], acc[cpv], smap[swa2], c_len - 1);
	}
	else {
		if (swa1 > 0) dpenalty_outside_swa += CalcDpenalty(cantus[cantus_id], m_cc, 0, smap[swa1 - 1]);
		if (swa2 < smap.size()) dpenalty_outside_swa += CalcDpenalty(cantus[cantus_id], m_cc, smap[swa2], c_len - 1);
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
			br_nfl = anfl[cpv];
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
	CHECK_READY(DR_pc, DR_fli);
	for (ls = 1; ls < fli_size; ++ls) {
		s = fli[ls];
		s_1 = fli[ls - 1];
		// Prohibit leap to VI#
		if (pcc[s] == 9 && abs(cc[s] - cc[s_1]) > fis_leap) FLAG2L(201, s_1, s);
		// Prohibit minor second up before VII - absorbed
		// Prohibit augmented second up before VII - absorbed
		// Prohibit unaltered VI or VII two steps from altered VI or VII
		if (pcc[s] == 11) {
			if (pcc[s_1] == 10) FLAG2L(153, s_1, s);
			if (pcc[s_1] == 8) FLAG2L(154, s_1, s);
			if (pcc[s_1] == 3) FLAG2L(157, s_1, s);
			if (ls > 1) {
				s_2 = fli[ls - 2];
				if (pcc[s_2] == 10) FLAG2L(159, s_2, s);
				if (pcc[s_2] == 8) FLAG2L(160, s_2, s);
				if (pcc[s_2] == 3) FLAG2L(163, s_2, s);
			}
			if (ls < fli_size - 1) {
				s1 = fli[ls + 1];
				if (pcc[s1] == 10) FLAG2L(153, s1, s);
				if (pcc[s1] == 8) FLAG2L(154, s1, s);
				if (pcc[s1] == 3) FLAG2L(156, s1, s);
				if (ls < fli_size - 2) {
					s2 = fli[ls + 2];
					if (pcc[s2] == 10) FLAG2L(159, s2, s);
					if (pcc[s2] == 8) FLAG2L(160, s2, s);
					if (pcc[s2] == 3) FLAG2L(162, s2, s);
				}
			}
		}
		if (pcc[s] == 9) {
			if (pcc[s_1] == 8) FLAG2L(152, s_1, s);
			if (pcc[s_1] == 3) FLAG2L(155, s_1, s);
			if (ls > 1) {
				s_2 = fli[ls - 2];
				if (pcc[s_2] == 8) FLAG2L(158, s_2, s);
				if (pcc[s_2] == 3) FLAG2L(161, s_2, s);
			}
			if (ls < fli_size - 1) {
				s1 = fli[ls + 1];
				if (pcc[s1] == 8) FLAG2L(152, s1, s);
				if (pcc[s1] == 3) FLAG2L(155, s1, s);
				if (ls < fli_size - 2) {
					s2 = fli[ls + 2];
					if (pcc[s2] == 8) FLAG2L(158, s2, s);
					if (pcc[s2] == 3) FLAG2L(161, s2, s);
				}
			}
		}
		// Prohibit unresolved minor tritone DG# (direct or with inserted note)
	}
	return 0;
}

void CGenCF1::ShowScanSpeed() {
	CString st;
	long long scan_time;
	if (task == tCor) scan_time = time() - correct_start_time;
	else scan_time = time() - scan_start_time;
	// Push new values
	q_scan_cycle.push_back(tcycle);
	q_scan_ms.push_back(time());
	// Get delta
	long long dms = q_scan_ms.back() - q_scan_ms.front();
	long long dcycle = q_scan_cycle.back() - q_scan_cycle.front();
	if (dcycle < 0) {
		WriteLog(5, "Error calculating scan speed: dcycle < 0");
	}
	// If delta big, remove old
	if (dms > 2000 && q_scan_ms.size() > 2) {
		q_scan_cycle.pop_front();
		q_scan_ms.pop_front();
	}
	// Create string
	CString speed_st;
	if (dms) {
		double sspeed = (double)dcycle / (double)dms;
		speed_st = HumanFloat(sspeed) + "/ms";
	}
	// Create status
	if (clib.size() > 0) st.Format("CL %d, ST %lld, CY %s", clib.size(), scan_time / 1000, speed_st);
	else st.Format("ST %lld, CY %s", scan_time / 1000, speed_st);
	// Send status
	SetStatusText(5, st);
}

char CGenCF1::GetScanVisualCode(int i) {
	if (!i) return '\'';
	else return scan_visual_code[(i - 1) % SCAN_VISUAL_CODE_BASE];
}

void CGenCF1::ShowScanStatus() {
	if (task == tEval) return;
	CString st;
	//CString progress_st;
	if (task == tGen && method == mScan) {
		for (int i = 0; i < wcount; ++i) {
			//st.Format("%d ", cc_id[wpos1[i]]);
			//progress_st += st;
			//progress_st += GetScanVisualCode(cc_id[wpos1[i]]);
			SetProgress(i, cc_id[wpos1[i]]);
		}
	}
	else if (task == tCor && method == mScan) {
		for (int i = 0; i < smatrixc; ++i) {
			//st.Format("%d ", cc_id[smap[i]]);
			//progress_st += st;
			//progress_st += GetScanVisualCode(cc_id[smap[i]]);
			SetProgress(i, cc_id[smap[i]]);
		}
	}
	else if (method == mSWA) {
		for (int i = 0; i < smatrixc; ++i) {
			//st.Format("%d ", cc_id[smap[i]]);
			//progress_st += st;
			//progress_st += GetScanVisualCode(cc_id[smap[i]]);
			SetProgress(i, cc_id[smap[i]]);
		}
	}
	//if (!progress_st.IsEmpty()) SetStatusText(2, progress_st + " (Scan progress)");
	if (task == tCor) st.Format("WI %d/%d, RP %.0f, DP %d", wid + 1, wcount, rpenalty_min, dpenalty_min);
	else st.Format("WI %d/%d", wid + 1, wcount);
	SetStatusText(1, st);
	st.Format("Sent: %ld (ignored %ld)", cantus_sent, cantus_ignored);
	SetStatusText(0, st);
	ShowScanSpeed();
}

void CGenCF1::CheckClibSize() {
	if (warn_clib_max) return;
	if (clib.size() > MAX_CLIB_WARN) { 
		warn_clib_max = 1;
		CString st;
		st.Format("Clib size (%zu) is greater than allowed size (%d). Please check your algorithm or increase MAX_CLIB_WARN", clib.size(), (int)MAX_CLIB_WARN);
		WriteLog(5, st);
	}
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
		CHECK_READY_PERSIST(DP_fcor);
		DeleteFile("cf1-cor.csv");
		CString st, st2, st3;
		st3 = "Flag; Total; ";
		for (int i = 0; i < max_flags; ++i) {
			int f1 = i;
			st2.Format("%s; %lld; ", 
				RuleName[rule_set][f1] + " (" + SubRuleName[rule_set][f1] + ")", 
				fcor[f1][f1]);
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
	if (calculate_stat && data_ready_persist[DP_fstat]) {
		//CHECK_READY_PERSIST(DP_fstat);
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
	if (calculate_blocking && method == mScan && data_ready_persist[DP_fblock]) {
		//CHECK_READY_PERSIST(DP_fblock);
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
						ngraph[x][v][n] = 
							(ngraph[pos1 - 1][v][n] * (pos2 - x) + 
								ngraph[pos2][v][n] * (x - pos1 + 1)) / 
								(pos2 - pos1 + 1);
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

void CGenCF1::SendGraph(int pos, int i, int v, int x) {
	graph[pos + i][v][0] = tweight[bli[x]];
	graph[pos + i][v][1] = g_leaps[bli[x]];
	graph[pos + i][v][2] = g_leaped[bli[x]];
}

void CGenCF1::SendLyrics(int pos, int v, int av, int x) {
	if (cantus_incom.size() > cantus_id && cantus_incom[cantus_id].size() > x) {
		lyrics[pos][v] = cantus_incom[cantus_id][x];
	}
}

void CGenCF1::SendComment(int pos, int v, int av, int x, int i) {
	CString st, com;
	int current_severity = -1;
	// Clear
	nlink[pos + i][v].clear();
	comment2[pos + i][v].Empty();
	comment[pos + i][v].clear();
	ccolor[pos + i][v].clear();
	if (anflags[av][x].size() > 0) for (int f = 0; f < anflags[av][x].size(); ++f) {
		// Do not show colors and comments for base voice
		if (av == cpv) {
			int fl = anflags[av][x][f];
			// Send comments and color only if rule is not ignored
			if (accept[fl] == -1 && !show_ignored_flags) continue;
			// Send comments and color only if rule is not ignored
			if (accept[fl] == 1 && !show_allowed_flags) continue;
			// Do not send if ignored
			if (severity[fl] < show_min_severity) continue;
			if (!i) {
				if (!accept[fl]) st = "- ";
				else if (accept[fl] == -1) st = "$ ";
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
				nlink[pos][v][fl] = anfl[av][x][f] - x;
			}
			// Set note color if this is maximum flag severity
			if (severity[fl] > current_severity && severity[fl] >= show_min_severity
				&& rule_viz[fl] != vHarm) {
				current_severity = severity[fl];
				color[pos + i][v] = flag_color[severity[fl]];
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
			if (nmax + trans < first_note0) trans = 0;
		}
		if (nmax < first_note0) {
			trans = floor((first_note0 - nmax) / 12 + 1) * 12;
			if (nmin + trans > first_note0) trans = 0;
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
	midifile_out_mul[pos + i] = midifile_out_mul0 * midifile_out_mul2;
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
		if (av_cnt != 1 && svoices == 1) tempo[pos + i] = cc_tempo[x] * 4;
		else tempo[pos + i] = cc_tempo[x];
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

void CGenCF1::MakeLenExport(vector<int> &cc, int av, int retr_on)
{
	int len_temp, last_pos;
	// Create note length
	last_pos = 0;
	len_temp = 0;
	for (s = 0; s < c_len; ++s) {
		if (cc[s] != cc[last_pos] || (retr_on && retrigger[s])) {
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

void CGenCF1::SendHarmColor(int pos, int v) {
	DWORD fc;
	mark_color[pos][v] = MakeColor(255, 150, 150, 150);
	// Scan flags
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
			GetGreen(fc)/1.5, GetBlue(fc));
	}
}

// Merge notes of same pitch, that do not have pauses between them. Step2 inclusive
void CGenCF1::MergeNotes(int step1, int step2, int v) {
	// Start of current note
	int first_pos = step1;
	DWORD col = color[step1][v];
	for (int x = step1 + 1; x <= step2; ++x) {
		// Detect steps that have same pitch and there is no retrigger 
		if (coff[x][v]) { 
			// select best color: gray is ignored, then most red is selected
			if (color[x][v] != color_noflag &&
				(col == color_noflag || GetRed(color[x][v]) > GetRed(col))) {
				col = color[x][v];
				// update color of previous steps
				for (int z = first_pos; z < x; ++z) {
					color[z][v] = col;
				}
			}
			coff[x][v] = coff[x - 1][v] + 1;
			/*
			if (comment[x][v].size()) {
				comment[first_pos][v].insert(end(comment[first_pos][v]), begin(comment[x][v]), end(comment[x][v]));
				ccolor[first_pos][v].insert(end(ccolor[first_pos][v]), begin(ccolor[x][v]), end(ccolor[x][v]));
			}
			comment2[first_pos][v] += comment2[x][v];
			*/
			// Copy color forward
			color[x][v] = col;
		}
		// New note
		else {
			first_pos = x;
			col = color[x][v];
		}
	}
}

// Calculate parameter map
void CGenCF1::CalcPmap(vector<int> &pcc, vector<int> &cc, vector<int> &c, vector<int> &smooth, vector<int> &leap) {
	pm_range = nmax - nmin;
	pm_tonic = 0;
	pm_sharp6 = 0;
	pm_sharp7 = 0;
	pm_flat6 = 0;
	pm_flat7 = 0;
	pm_decc_min = INT_MAX;
	pm_decc_max = 0;
	pm_decc_av = 0;
	pm_leapsum = 0;
	pm_leaps = 0;
	pm_smooth = 0;
	for (ls = 0; ls < fli_size; ++ls) {
		s = fli[ls];
		// Note frequency
		if (!pcc[s]) ++pm_tonic;
		else if (pcc[s] == 11) ++pm_sharp7;
		else if (pcc[s] == 10) ++pm_flat7;
		else if (pcc[s] == 9) ++pm_sharp6;
		else if (pcc[s] == 8) ++pm_flat6;
		// Average decc
		pm_decc_av += decc2[s];
		if (ls) {
			if (smooth[s - 1]) ++pm_smooth;
			else if (leap[s - 1]) {
				++pm_leaps;
				pm_leapsum += abs(c[s] - c[s - 1]) + 1;
			}
		}
	}
	pm_decc_av /= fli_size;
	for (s = 0; s < ep2; ++s) {
		if (pm_decc_min > decc2[s]) pm_decc_min = decc2[s];
		if (pm_decc_max < decc2[s]) pm_decc_max = decc2[s];
	}
}

// Get parameter map string
void CGenCF1::GetPmap() {
	CString st;
	pmap.Empty();
	st.Format("Range: %d semitones\n", pm_range);
	pmap += st;
	st.Format("Culminations: %d\n", pm_culm_count);
	pmap += st;
	st.Format("Notes: %d\n", fli_size);
	pmap += st;
	st.Format("Tonic notes / max tonic weight: %d / %s\n", 
		pm_tonic, HumanFloat(pm_tw_max));
	pmap += st;
	st.Format("Min / av / max pitch deviation: %s / %s / %s\n", 
		HumanFloatPrecision(pm_decc_min), HumanFloatPrecision(pm_decc_av), 
		HumanFloatPrecision(pm_decc_max));
	pmap += st;
	st.Format("Min / av / max MA pitch range in window %d: %s / %s / %s\n", 
		notes_arange2, HumanFloatPrecision(pm_maccr_min), 
		HumanFloatPrecision(pm_maccr_av), HumanFloatPrecision(pm_maccr_max));
	pmap += st;
	st.Format("Smooth / leaps / leaped: %d / %d / %d\n", pm_smooth, pm_leaps, pm_leapsum);
	pmap += st;
	st.Format("2 / 3 consecutive leaps: %d / %d\n", pm_leaps2, pm_leaps3);
	pmap += st;
	st.Format("Leaps / leaped in window %d: %d / %d\n", max_leap_steps2, pm_win_leaps, pm_win_leapnotes);
	pmap += st;
	if (minor_cur) {
		st.Format("VI / VI# / VII / VII#: %d / %d / %d / %d\n", 
			pm_flat6, pm_sharp6, pm_flat7, pm_sharp7);
		pmap += st;
	}
}

CString CGenCF1::GetPmapLogHeader() {
	CString st;
	st += "Time;Algorithm;Config;Midi;Voice;ID;High;Steps;Notes;Tonic;Minor;";
	st += "Range;Culminations;";
	st += "Decc_min;Decc_av;Decc_max;Maccr_min;Maccr_av;Maccr_max;";
	st += "Tonics;VI;VI#;VII;VII#;";
	st += "Smooth;Leaps;2leaps;3leaps;Leap size;Leaps_win;Leaped_win;";
	return st;
}

CString CGenCF1::GetPmapLogSt() {
	CString st, st2;
	st.Format("%s;%s;%s;%s;%d;%d;%d;%d;%d;%d;%d;",
		CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S"),
		m_algo_folder, m_config, bname_from_path(midi_file), svoice, cantus_id + 1, cantus_high, ep2, 
		fli_size, tonic_cur, minor_cur);
	st2 += st;
	st.Format("%d;%d;",
		pm_range, pm_culm_count);
	st2 += st;
	st.Format("%s;%s;%s;%s;%s;%s;",
		HumanFloatPrecision(pm_decc_min), HumanFloatPrecision(pm_decc_av), 
		HumanFloatPrecision(pm_decc_max), HumanFloatPrecision(pm_maccr_min), 
		HumanFloatPrecision(pm_maccr_av), HumanFloatPrecision(pm_maccr_max));
	st2 += st;
	st.Format("%d;%d;%d;%d;%d;",
		pm_tonic, pm_flat6, pm_sharp6, pm_flat7, pm_sharp7);
	st2 += st;
	st.Format("%.0f%%;%.0f%%;%.0f%%;%.0f%%;%.2f;%d;%d;",
		100.0 * pm_smooth / (pm_leaps + pm_smooth), 
		100.0 * pm_leaps / (pm_leaps + pm_smooth), 
		100.0 * pm_leaps2 / pm_leaps, 
		100.0 * pm_leaps3 / pm_leaps, 
		1.0 * pm_leapsum / pm_leaps, 
		pm_win_leaps, pm_win_leapnotes);
	st2 += st;
	st2.Replace(".", ",");
	return st2;
}

// Log parameter map
void CGenCF1::LogPmap() {
	CString fname = "log\\cf-pmap.csv";
	// Header
	if (!fileExists(fname)) {
		//AppendLineToFile(fname, GetPmapLogHeader() + "\n");
	}
	//AppendLineToFile(fname, GetPmapLogSt() + "\n");
}

CString CGenCF1::GetHarmName(int pitch, int alter) {
	if (minor_cur) {
		if (alter == 1) return HarmName_ma[pitch];
		else return HarmName_m[pitch];
	}
	else return HarmName[pitch];
}

int CGenCF1::SendCantus() {
	int step000 = step;
	float l_rpenalty_cur;
	// Save culmination position
	cf_culm_cfs = culm_ls;
	if (svoice < 0) return 0;
	CString st, st2, rpst;
	int v = svoice;
	Sleep(sleep_ms);
	TransposeCantusBack();
	len_export.resize(c_len);
	coff_export.resize(c_len);
	CalcPmap(m_pcc, m_cc, m_c, m_smooth, m_leap);
	GetPmap();
	LogPmap();
	MakeLenExport(m_cc, 0, 0);
	// Copy cantus to output
	int pos = step;
	if (step + real_len >= t_allocated) ResizeVectors(t_allocated * 2);
	for (s = 0; s < ep2; ++s) {
		cpos[s] = pos;
		if (chm.size() > bli[s] && chm[bli[s]] > -1) {
			mark[pos][v] = GetHarmName(chm[bli[s]], m_cc[s] == 9 || m_cc[s] == 11);
			SendHarmColor(pos, v);
		}
		SendLyrics(pos, v, cpv, s);
		for (int i = 0; i < cc_len[s]; ++i) {
			if (av_cnt != 1) note_muted[pos + i][v] = 1; 
			color[pos + i][v] = MakeColor(0, 100, 100, 100);
			SendNotes(pos, i, v, cpv, s, m_cc);
			SendNgraph(pos, i, v, s);
			SendGraph(pos, i, v, s);
			SendComment(pos, v, cpv, s, i);
		}
		pos += cc_len[s];
	}
	MakeBellDyn(v, step, pos - 1, 40, 100, 20);
	step = pos + SendPause(pos, v);
	InterpolateNgraph(v, step000, step);
	// Count additional variables
	CountOff(step000, step - 1);
	CountTime(step000, step - 1);
	UpdateNoteMinMax(step000, step - 1, !is_animating);
	UpdateTempoMinMax(step000, step - 1);
	// Increment cantus_sent only if is not animating
	if (!is_animating) ++cantus_sent;
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
	// Save source fpenalty 
	if (!v) fpenalty_source = rpst;
	if (task == tGen) {
		if (!shuffle) {
			Adapt(step000, step - 1);
		}
		// If  window-scan
		st.Format("#%d\nRule penalty: %.0f\nCantus: %s", 
			cantus_sent, l_rpenalty_cur, cantus_high?"high":"low");
		st2.Format("Flags penalty: %s\n%s", rpst, pmap);
		AddMelody(step000, pos - 1, v, st, st2);
		if (v) AddMelody(step000, pos - 1, 0, st);
	}
	else if (task == tEval) {
		if (m_algo_id == 101) {
			// If RSWA
			st.Format("#%d\nRule penalty: %.0f\nCantus: %s", 
				cantus_sent, l_rpenalty_cur, cantus_high ? "high" : "low");
			st2.Format("Flags penalty: %s\n%s", rpst, pmap);
		}
		else {
			if (key_eval.IsEmpty()) {
				// If SWA
				st.Format("#%d (from %s)\nRule penalty: %.0f => %.0f\nDistance penalty: %d\nCantus: %s", 
					cantus_id+1, bname_from_path(midi_file), rpenalty_source, l_rpenalty_cur,
					dpenalty_cur, cantus_high ? "high" : "low");
				st2.Format("Flags penalty: %s => %s\n%s", fpenalty_source, rpst, pmap);
			}
			else {
				// If evaluating
				st.Format("#%d (from %s)\nRule penalty: %.0f\nCantus: %s", 
					cantus_id + 1, bname_from_path(midi_file), l_rpenalty_cur, cantus_high ? "high" : "low");
				st2.Format("Flags penalty: %s\nKey selection: %s\n%s", rpst, key_eval, pmap);
			}
		}
		AddMelody(step000, pos - 1, v, st, st2);
		if (v) AddMelody(step000, pos - 1, 0, st);
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
		return error;
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

void CGenCF1::LogPerf() {
	CString st, st2;
	long long ms = time() - gen_start_time;
	if (!ms) ms = 1;
	st.Format("%s\\%s.pl %s CY %s/ms, sent %s/s, c_len %d, swa_steps %d, sent %d, ignored %d, ",
		m_algo_folder, m_config, method == mScan ? "SAS" : "SWA",
		HumanFloat(tcycle/(float)ms), HumanFloat(cantus_sent / (float)ms * 1000),
		c_len, swa_steps, cantus_sent, cantus_ignored);
	st2 += st;
	if (m_algo_id == 111 || m_algo_id == 112) {
		st.Format("analy %d/%d, cor %d, cor_full %d, cor_rp0 %d, ", 
			cantus_id + 1, cpoint.size() + cantus.size(), cor_sent,
			cor_full, cor_rp0);
		st2 += st;
		if (cor_sent) {
			st.Format("~swa %s, ~rp %s, ~dp %s, ",
				HumanFloat(swa_sum/cor_sent), HumanFloat(rp_sum / cor_sent), 
				HumanFloat(dp_sum / cor_sent));
			st2 += st;
		}
	}
	if (m_testing) {
		st.Format("time_res %ds, ", ANALYZE_RESERVE);
		st2 += st;
	}
	if (cantus_id) {
		st.Format("%s #%d, ", midi_file, cantus_id);
		st2 += st;
	}
	st.Format("time %ss, cor_ack%d, fstat%d, fblock%d, fcor%d, ssf%d ", 
		HumanFloat(ms / 1000.0), cor_ack, calculate_stat, calculate_blocking,
		calculate_correlation, calculate_ssf);
	st2 += st;
#ifdef CF_DEBUG
	st2 += "DCF ";
#endif

#if defined(_DEBUG)
	st2 += "Debug ";
#else
	st2 += "Release ";
#endif

#ifdef _WIN64
	st2 += "x64 ";
#else
	st2 += "x86 ";
#endif
	WriteLog(2, st2);
	if (m_testing == 1) AppendLineToFile("autotest\\perf.log", st2 + "\n");
}

void CGenCF1::CheckSASEmulatorFlags(vector<int> &cc) {
	int fl, fl2, found, delay, good, error_level;
	CString error_st, est;
	if (ep2 == c_len) for (s = 0; s < ep2; ++s) {
		// Loop through all source flags if last emulator run
		for (int f = 0; f < nflags_full[s].size(); ++f) {
			fl = nflags_full[s][f];
			found = 0;
			for (int f2 = 0; f2 < anflags[cpv][s].size(); ++f2) {
				if (anflags[cpv][s][f2] == fl) {
					found = 1;
					break;
				}
			}
			// Stop processing if this flag is found
			if (found) continue;
			// Not found in same position: can it be replaced?
			if (flag_replace[fl].size()) {
				for (int i = 0; i < flag_replace[fl].size(); ++i) {
					fl2 = flag_replace[fl][i];
					for (int f2 = 0; f2 < anflags[cpv][s].size(); ++f2) if (anflags[cpv][s][f2] == fl2) {
						found = 1;
						break;
					}
				}
			}
			if (found) {
				est.Format("+ Second scan at step %d replaced flag [%d] with: [%d] %s %s (%s) at %d:%d (beat %d:%d) %s",
					ep2, fl2, fl, accept[fl] ? "+" : "-", RuleName[rule_set][fl], SubRuleName[rule_set][fl],
					cantus_id + 1, s + 1, cpos[s] / 8 + 1, cpos[s] % 8 + 1, midi_file);
				WriteLog(7, est);
				if (m_testing == 1) AppendLineToFile("autotest\\sas-emu.log", est + "\n");
				continue;
			}
			est.Format("- Flag does not appear on second full evaluation: [%d] %s %s (%s) at %d:%d (beat %d:%d) %s",
				fl, accept[fl] ? "+" : "-", RuleName[rule_set][fl], SubRuleName[rule_set][fl],
				cantus_id + 1, s + 1, cpos[s] / 8 + 1, cpos[s] % 8 + 1, midi_file);
			WriteLog(5, est);
			if (m_testing == 1) AppendLineToFile("autotest\\sas-emu.log", est + "\n");
		}
	}
	for (s = 0; s < ep2; ++s) {
		// Loop through all current flags
		for (int f = 0; f < anflags[cpv][s].size(); ++f) {
			fl = anflags[cpv][s][f];
			// Do not analyse unstable flags
			if (sas_emulator_unstable[fl]) continue;
			// Check that flag exists in previous SAS run if this is not last step and previous run exists
			found = 0;
			good = 0;
			error_st.Empty();
			error_level = 0;
			if (s < ep2 - 1 && nflags_prev.size() > s) {
				for (int f2 = 0; f2 < nflags_prev[s].size(); ++f2) if (nflags_prev[s][f2] == fl) {
					found = 1;
					break;
				}
			}
			// Stop processing if this flag is not new
			if (found) continue;
			// Find longest delay for each new flag
			delay = ep2 - 1 - s;
			if (delay > flag_delay[fl]) {
				CString est;
				est.Format("SAS emulator at step %d has delay %d steps: [%d] %s %s (%s) at %d:%d (beat %d:%d) %s",
					ep2, delay, fl, accept[fl] ? "+" : "-", RuleName[rule_set][fl], SubRuleName[rule_set][fl],
					cantus_id + 1, s + 1, cpos[s] / 8 + 1, cpos[s] % 8 + 1, midi_file);
				//WriteLog(1, est);
				flag_delay[fl] = delay;
				flag_delay_st[fl] = est;
			}
			// Check that flag exists in full analysis in same position
			for (int f2 = 0; f2 < nflags_full[s].size(); ++f2) if (nflags_full[s][f2] == fl) {
				found = 1;
				break;
			}
			// Stop processing if this flag is found
			if (found) continue;
			// Not found in same position: can it be replaced?
			if (sas_emulator_replace[fl].size()) {
				for (int i = 0; i < sas_emulator_replace[fl].size(); ++i) {
					fl2 = sas_emulator_replace[fl][i];
					for (int f2 = 0; f2 < nflags_full[s].size(); ++f2) if (nflags_full[s][f2] == fl2) {
						found = 1;
						break;
					}
				}
			}
			if (!found && flag_replace[fl].size()) {
				for (int i = 0; i < flag_replace[fl].size(); ++i) {
					fl2 = flag_replace[fl][i];
					for (int f2 = 0; f2 < nflags_full[s].size(); ++f2) if (nflags_full[s][f2] == fl2) {
						found = 1;
						break;
					}
				}
			}
			if (found) {
				est.Format("+ SAS emulator at step %d replaced flag [%d] with: [%d] %s %s (%s) at %d:%d (beat %d:%d) %s",
					ep2, fl2, fl, accept[fl] ? "+" : "-", RuleName[rule_set][fl], SubRuleName[rule_set][fl],
					cantus_id + 1, s + 1, cpos[s] / 8 + 1, cpos[s] % 8 + 1, midi_file);
				WriteLog(7, est);
				if (m_testing == 1) AppendLineToFile("autotest\\sas-emu.log", est + "\n");
				continue;
			}
			error_st.Format("- SAS emulator at step %d assigned wrong flag: [%d] %s %s (%s) at %d:%d (beat %d:%d) %s",
				ep2, fl, accept[fl] ? "+" : "-", RuleName[rule_set][fl], SubRuleName[rule_set][fl],
				cantus_id + 1, s + 1, cpos[s] / 8 + 1, cpos[s] % 8 + 1, midi_file);
			error_level = 5;
			// Not found in same position: does it exist in any position?
			if (flags_full[fl]) {
				if (sas_emulator_move_ignore[fl]) {
					est.Format("+ SAS emulator at step %d assigned moved flag: [%d] %s %s (%s) at %d:%d (beat %d:%d) %s",
						ep2, fl, accept[fl] ? "+" : "-", RuleName[rule_set][fl], SubRuleName[rule_set][fl],
						cantus_id + 1, s + 1, cpos[s] / 8 + 1, cpos[s] % 8 + 1, midi_file);
					WriteLog(7, est);
					if (m_testing == 1) AppendLineToFile("autotest\\sas-emu.log", est + "\n");
					continue;
				}
				else {
					error_st.Format("- SAS emulator at step %d assigned moved flag: [%d] %s %s (%s) at %d:%d (beat %d:%d) %s",
						ep2, fl, accept[fl] ? "+" : "-", RuleName[rule_set][fl], SubRuleName[rule_set][fl],
						cantus_id + 1, s + 1, cpos[s] / 8 + 1, cpos[s] % 8 + 1, midi_file);
					error_level = 5;
				}
			}
			// Not found in any position: can it be replaced in any position?
			if (sas_emulator_replace[fl].size() && sas_emulator_move_ignore[fl]) {
				for (int i = 0; i < sas_emulator_replace[fl].size(); ++i) {
					fl2 = sas_emulator_replace[fl][i];
					if (flags_full[fl2]) {
						found = 1;
						break;
					}
				}
				if (!found && flag_replace[fl].size()) {
					for (int i = 0; i < flag_replace[fl].size(); ++i) {
						fl2 = flag_replace[fl][i];
						if (flags_full[fl2]) {
							found = 1;
							break;
						}
					}
				}
				if (found) {
					CString est;
					est.Format("+ SAS emulator at step %d replaced and moved flag %d with: [%d] %s %s (%s) at %d:%d (beat %d:%d) %s",
						ep2, fl2, fl, accept[fl] ? "+" : "-", RuleName[rule_set][fl], SubRuleName[rule_set][fl],
						cantus_id + 1, s + 1, cpos[s] / 8 + 1, cpos[s] % 8 + 1, midi_file);
					WriteLog(7, est);
					if (m_testing == 1) AppendLineToFile("autotest\\sas-emu.log", est + "\n");
					continue;
				}
			}
			// Show error
			error_st += " [" + vint2st(c_len, cc) + "]";
			error_st += " {" + vint2st(nflags_full[s].size(), nflags_full[s]) + "}";
			WriteLog(error_level, error_st);
			if (m_testing == 1) AppendLineToFile("autotest\\sas-emu.log", error_st + "\n");
		}
	}
}

void CGenCF1::OutputFlagDelays() {
	for (int f = 0; f < MAX_RULES; ++f) if (flag_delay[f] > sas_emulator_max_delay[f]) {
		//WriteLog(6, flag_delay_st[f]); // 1
	}
}

void CGenCF1::EmulateSAS() {
	if (v_cnt == 1) {
		// Evaluate for CF1
		ScanCantus(tEval, -1, &(m_cc));
	}
	// Save full analysis flags
	nflags_full = anflags[cpv];
	flags_full = flags;
	nflags_prev.clear();
	for (fixed_ep2 = 1; fixed_ep2 <= m_cc.size(); ++fixed_ep2) {
		// Show emulator status
		CString est;
		est.Format("SAS emulator: %d of %d", fixed_ep2, m_cc.size());
		SetStatusText(7, est);
		// Visible emulation
		if (emulate_sas) {
			step0 = step;
			FillPause(step0, floor((real_len + 1) / 8 + 1) * 8, 0);
			if (v_cnt > 1) FillPause(step0, floor((real_len + 1) / 8 + 1) * 8, 1);
			ScanCantus(tEval, 0, &(m_cc));
		}
		// Hidden emulation
		else {
			ScanCantus(tEval, -1, &(m_cc));
		}
		if (need_exit) break;
		CheckSASEmulatorFlags(m_cc);
		nflags_prev = anflags[cpv];
	}
	if (!need_exit) OutputFlagDelays();
	fixed_ep2 = 0;
	SetStatusText(7, "SAS emulator: finished");
}

// Create random cantus and optimize it using SWA
void CGenCF1::RandomSWA()
{
	// Init animation
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
		last_note = last_note0;
		// Create random cantus
		task = tGen;
		MakeNewCantus(m_c, m_cc);
		EmulateSAS();
		task = tCor;
		min_cc0 = min_cc;
		max_cc0 = max_cc;
		cantus[0] = m_cc;
		// Set scan matrix to scan all
		smatrixc = c_len;
		smatrix.resize(c_len);
		for (int x = 0; x < c_len; ++x) {
			smatrix[x] = 1;
		}
		// Optimize cantus
		rpenalty_cur = MAX_PENALTY;
		correct_start_time = time();
		SWA(0, 0);
		// Show cantus if it is perfect
		if (rpenalty_min <= rpenalty_accepted) {
			if (vs.Insert(m_cc)) {
				int step = t_generated;
				// Add line
				linecolor[t_generated] = MakeColor(255, 0, 0, 0);
				ScanCantus(tEval, 0, &(m_cc));
				if (rpenalty_cur > rpenalty_accepted) {
					st.Format("Error calculating rpenalty %f min %f at step %d", 
						rpenalty_cur, rpenalty_min, t_generated);
					WriteLog(5, st);
					TestRpenalty();
				}
				Adapt(step, t_generated - 1);
				t_sent = t_generated;
				EmulateSAS();
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
	swa_len = 1;
	// Save source rpenalty
	rpenalty_source = rpenalty_cur;
	long cnum = 0;
	// Save cantus only if its penalty is less or equal to source rpenalty
	rpenalty_min = rpenalty_cur;
	dpenalty_min = 0;
	m_cc = cantus[i];
	swa_full = 0;
	best_flags.clear();
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
		// Add current cantus if this is not first run
		if (a > 0) {
			clib.push_back(m_cc);
			clib_vs.Insert(m_cc);
			rpenalty.push_back(rpenalty_min_old);
			dpenalty.push_back(dpenalty_min_old);
		}
		// Sliding Windows Approximation
		ScanCantus(tCor, 0, &m_cc);
		CheckClibSize();
		cnum = clib.size();
		if (cnum) {
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
			est.Format("SWA%d #%d: rp %.0f from %.0f, dp %d, cnum %ld", swa_len, a, rpenalty_min, rpenalty_source, dpenalty_min, cnum);
			WriteLog(3, est);
		}
		if (m_cc.size() > 60 || swa_len > 0) {
			st.Format("SWA%d attempt: %d", swa_len, a);
			SetStatusText(4, st);
		}
		// Animation
		long long time = CGLib::time();
		// Limit correction time
		if (max_correct_ms && time - gen_start_time > max_correct_ms) break;
		if (dp) {
			// Abort SWA if dpenalty and rpenalty not decreasing
			if (rpenalty_min >= rpenalty_min_old && dpenalty_min >= dpenalty_min_old) {
				if (swa_len >= swa_steps || swa_len >= smap.size()) {
					swa_full = 1;
					if (swa_len >= smap.size()) swa_full = 2;
					break;
				}
				else ++swa_len;
			}
		}
		else {
			// Abort SWA if rpenalty zero or not decreasing
			if (!rpenalty_min) break;
			if (rpenalty_min >= rpenalty_min_old) {
				if (swa_len >= swa_steps || swa_len >= smap.size()) {
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
	CString stuck_st = GetStuck();
	est.Format("Finished SWA%d #%d: rp %.0f from %.0f, dp %d, cnum %ld (in %lld ms): %s", 
		swa_len, a, rpenalty_min, rpenalty_source, dpenalty_min, cnum, time_stop - time_start, stuck_st);
	WriteLog(0, est);
	TestBestRpenalty();
}

// Save accepted time if we are showing best rejected
void CGenCF1::TimeBestRejected() {
	if (best_rejected) {
		accept_time = CGLib::time();
		rcycle = 0;
	}
}

void CGenCF1::SaveCantus() {
	if (method == mScan) dpenalty_cur = dpenalty_step[c_len - 1];
	if (!dpenalty_cur) dpenalty_cur = CalcDpenalty(cantus[cantus_id], m_cc, 0, c_len - 1);
	// If rpenalty is same as min, calculate dpenalty
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
	clib.push_back(m_cc);
	rpenalty.push_back(rpenalty_cur);
	rpenalty_min = rpenalty_cur;
}

void CGenCF1::SaveCantusIfRp() {
	CHECK_READY(DR_rpenalty_cur);
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
		CLEAR_READY();
		//LogCantus("CF1 ep2", ep2, m_cc);
		//if (method == mScan && task == tCor && cantus_id+1 == 1 && ep2 > 1 && MatchVectors(m_cc, test_cc, 0, ep2-1)) {
		//	CString est;
		//	est.Format("Found method %d id %d ep2 %d cycle %lld sp1 %d sp2 %d p %d", 
		//		method, cantus_id+1, ep2, cycle, sp1, sp2, p);
		//	WriteLog(1, est); 
		//}
		// Check if dpenalty is already too high
		if (task == tCor && !rpenalty_min) {
			if (method == mScan) {
				CalcStepDpenalty(cantus[cantus_id], m_cc, ep2 - 1);
				if (dpenalty_step[ep2 - 1] > dpenalty_min) goto skip;
			}
			else {
				dpenalty_cur = dpenalty_outside_swa + CalcDpenalty(cantus[cantus_id], m_cc, smap[swa1], smap[sp2 - 1]);
				if (dpenalty_cur > dpenalty_min) goto skip;
			}
		}
		else dpenalty_cur = 0;		
		ClearFlags(0, ep2);
		if (FailNoteRepeat(m_cc, 0, ep2 - 1)) goto skip;
		GetMelodyInterval(m_cc, 0, ep2, nmin, nmax);
		++accepted3;
		// Limit melody interval
		if (nmax - nmin > max_interval) FLAG(37, 0);
		if (c_len == ep2 && nmax - nmin < min_interval) FLAG(38, 0);
		// Show status
		long long time = CGLib::time();
		scycle = (time - gen_start_time) / STATUS_PERIOD;
		if (scycle > status_cycle) {
			ShowScanStatus();
			status_cycle = scycle;
		}
		// Save last second for analysis  
		if (m_testing && task == tCor && m_algo_id != 101 && 
			time - gen_start_time > (m_test_sec - ANALYZE_RESERVE) * 1000)
			break;
		// Limit SAS correction time
		if (task == tCor && max_correct_ms && time - correct_start_time > max_correct_ms) break;
		if (FailDiatonic(m_c, m_cc, 0, ep2, minor_cur)) goto skip;
		GetPitchClass(m_c, m_cc, m_pc, m_pcc, 0, ep2);
		CreateLinks(m_cc, 0);
		if (minor_cur) {
			if (FailMinor(m_pcc, m_cc)) goto skip;
			if (FailGisTrail(m_pcc)) goto skip;
			if (FailFisTrail(m_pcc)) goto skip;
		}
		GetMovingMax(m_cc, max(lclimax_notes, lclimax_mea*npm), lclimax);
		if (FailTonic(m_cc, m_pc)) goto skip;
		if (FailLastNotes(m_pc, m_pcc)) goto skip;
		//if (FailNoteSeq(m_pc)) goto skip;
		if (FailIntervals(m_c, m_cc, m_pc, m_pcc)) goto skip;
		if (FailLastNoteRes(m_pc)) goto skip;
		GetLeapSmooth(m_c, m_cc, m_leap, m_smooth, m_slur);
		if (FailTritones(m_c, m_cc, m_pc, m_pcc, m_leap)) goto skip;
		if (FailManyLeaps(m_c, m_cc, m_leap, m_smooth, m_slur, max_leaps, max_leaped, max_leaps, max_leaped, max_leap_steps, 3, 25, 3, 25)) goto skip;
		if (FailManyLeaps(m_c, m_cc, m_leap, m_smooth, m_slur, max_leaps2, max_leaped2, max_leaps2, max_leaped2, max_leap_steps2, 202, 203, 202, 203)) goto skip;
		if (FailLeapSmooth(m_c, m_cc, m_leap, m_smooth, m_slur, max_smooth2, max_smooth_direct2, 302, 303, 1)) goto skip;
		// Run green tests only if orange did not fire
		//if (skip_flags || (!flags[302] && flags[303])) {
			if (FailLeapSmooth(m_c, m_cc, m_leap, m_smooth, m_slur, max_smooth, max_smooth_direct, 4, 5, 0)) goto skip;
		//}
		if (FailOutstandingRepeat(m_c, m_cc, m_leap, repeat_steps2, repeat_notes2, 76)) goto skip;
		if (FailOutstandingRepeat(m_c, m_cc, m_leap, repeat_steps3, repeat_notes3, 36)) goto skip;
		if (FailLongRepeat(m_c, m_cc, m_leap, repeat_steps5, repeat_notes5, 72)) goto skip;
		if (FailLongRepeat(m_c, m_cc, m_leap, repeat_steps7, repeat_notes7, 73)) goto skip;
		if (FailGlobalFill(m_c, nstat2)) goto skip;
		if (FailLocalRange(m_c, notes_lrange1, min_lrange1, 352)) goto skip;
		if (FailLocalRange(m_c, notes_lrange12, min_lrange12, 353)) goto skip;
		if (FailLocalRange(m_c, notes_lrange13, min_lrange13, 351)) goto skip;
		if (FailLocalPiCount(m_cc, notes_picount, min_picount, 344)) goto skip;
		if (FailLocalPiCount(m_cc, notes_picount2, min_picount2, 345)) goto skip;
		if (FailLocalPiCount(m_cc, notes_picount3, min_picount3, 346)) goto skip;
		if (FailStagnation(m_cc, nstat, stag_note_steps, stag_notes, 10)) goto skip;
		if (FailStagnation(m_cc, nstat, stag_note_steps2, stag_notes2, 39)) goto skip;
		if (FailMultiCulm(m_cc, m_slur)) goto skip;
		if (FailFirstNotes(m_pc)) goto skip;
		if (FailLeap(m_c, m_cc, m_leap, m_smooth, nstat2, nstat3)) goto skip;
		if ((fli_size>1) && FailMelodyHarm(m_pc, m_pcc)) goto skip;
		MakeMacc(m_cc);
		if (FailLocalMacc(notes_arange, min_arange, 15)) goto skip;
		if (FailLocalMacc(notes_arange2, min_arange2, 16)) goto skip;

		SaveBestRejected(m_cc);
		if (task == tCor && method == mSWA) {
			if (skip_flags) {
				SET_READY(DR_rpenalty_cur);
				rpenalty_cur = 0;
				if (ep2 < smap[swa2 - 1] + 1) {
					NextWindow(m_cc);
					goto check;
				}
			}
			else {
				CalcRpenalty(m_cc);
				if (ep2 < smap[swa2 - 1] + 1) {
					if (rpenalty_cur > src_rpenalty_step[smap[swa1]]) goto skip;
					NextWindow(m_cc);
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
			// If this is not last window, go to next window
			if (ep2 < c_len) {
				NextWindow(m_cc);
				goto check;
			}
			// Check random_choose
			if (random_choose < 100) if (rand2() >= (float)RAND_MAX*random_choose / 100.0) goto skip;
		}
		// Calculate rules penalty if we evaluate
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
				// Calculate dpenalty if this is evaluation
				if (task == tEval && cantus.size()) dpenalty_cur = CalcDpenalty(cantus[cantus_id], m_cc, 0, ep2 - 1);
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
			// Finish if this is last variant in first window and not SWA
			if ((p == 0) || (wid == 0)) {
				// Sliding Windows Approximation
				if (method == mSWA) {
					if (NextSWA(m_cc, m_cc_old)) {
						scan_full = 1;
						break;
					}
					goto check;
				}
				if (random_seed && random_range && accept_reseed) {
					// Infinitely cycle through ranges
					ReseedCantus();
					// Start evaluating without scan
					goto check;
				}
				WriteLog(0, "Last variant in first window reached");
				scan_full = 1;
				break;
			}
			ShowBestRejected(m_cc);
			BackWindow(m_cc);
			// Goto next variant calculation
			goto skip;
		} // if (finished)
		ScanRight(m_cc);
	}
	if (accepted3 > 100000) {
		WritePerfLog();
		ShowScanStatus();
	}
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
	++tcycle;
}

void CGenCF1::Generate()
{
	CString test_st = "72 67 69 71 69 65 67 64 62 60";
	test_cc.resize(10);
	StringToVector(&test_st, " ", test_cc);

	first_note0 = first_note;
	last_note0 = last_note;
	if (error) return;
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
	LogPerf();
}

inline void CGenCF1::ClearReady() {
	fill(data_ready.begin(), data_ready.end(), 0);
}

inline void CGenCF1::SetReady(int id) {
	data_ready[id] = 1;
}

inline void CGenCF1::SetReady(int id, int id2) {
	data_ready[id] = 1;
	data_ready[id2] = 1;
}

inline void CGenCF1::SetReady(int id, int id2, int id3) {
	data_ready[id] = 1;
	data_ready[id2] = 1;
	data_ready[id3] = 1;
}

inline void CGenCF1::ClearReadyPersist(int id) {
	data_ready_persist[id] = 0;
}

inline void CGenCF1::ClearReadyPersist(int id, int id2) {
	data_ready_persist[id] = 0;
	data_ready_persist[id2] = 0;
}

inline void CGenCF1::ClearReadyPersist(int id, int id2, int id3) {
	data_ready_persist[id] = 0;
	data_ready_persist[id2] = 0;
	data_ready_persist[id3] = 0;
}

inline void CGenCF1::SetReadyPersist(int id) {
	data_ready_persist[id] = 1;
}

inline void CGenCF1::SetReadyPersist(int id, int id2) {
	data_ready_persist[id] = 1;
	data_ready_persist[id2] = 1;
}

inline void CGenCF1::SetReadyPersist(int id, int id2, int id3) {
	data_ready_persist[id] = 1;
	data_ready_persist[id2] = 1;
	data_ready_persist[id3] = 1;
}

inline void CGenCF1::CheckReady(int id) {
	if (!data_ready[id] && !warn_data_ready[id]) {
		++warn_data_ready[id];
		CString est;
		est.Format("Attemp to use data element '%d' while it is not ready yet", id);
		WriteLog(5, est);
		ASSERT(0);
	}
}

inline void CGenCF1::CheckReady(int id, int id2) {
	CheckReady(id);
	CheckReady(id2);
}

inline void CGenCF1::CheckReady(int id, int id2, int id3) {
	CheckReady(id);
	CheckReady(id2);
	CheckReady(id3);
}

inline void CGenCF1::CheckReadyPersist(int id) {
	if (!data_ready_persist[id] && !warn_data_ready_persist[id]) {
		++warn_data_ready_persist[id];
		CString est;
		est.Format("Attemp to use persistent data element '%d' while it is not ready yet", id);
		WriteLog(5, est);
		ASSERT(0);
	}
}

inline void CGenCF1::CheckReadyPersist(int id, int id2) {
	CheckReadyPersist(id);
	CheckReadyPersist(id2);
}

inline void CGenCF1::CheckReadyPersist(int id, int id2, int id3) {
	CheckReadyPersist(id);
	CheckReadyPersist(id2);
	CheckReadyPersist(id3);
}
