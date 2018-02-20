#pragma once
#include "../GLibrary/GMidi.h"
#include "../GLibrary/VSet.h"

// THESE MACROS CAN BE DISABLED TO IMPROVE PERFORMANCE

// Check data ready
#define MAX_DATA_READY 30
#define MAX_DATA_READY_PERSIST 20

// Persistent
#define DP_Config				1
#define DP_ConfigTest		2
#define DP_Rules				3
#define DP_RuleSet			4
#define DP_RuleParam		5
#define DP_RuleSetParam 6
#define DP_cc_old				7
#define DP_cc_incr			8
#define DP_hv						9
#define DP_hsp					10
#define DP_fstat				11
#define DP_fcor					12
#define DP_fblock				13
#define DP_mli					14
#define DP_cfli					15

// Non-persistent
#define DR_c						1
#define DR_fli					2
#define DR_macc					3
#define DR_pc						4
#define DR_nmin					5
#define DR_cc_order			7
#define DR_leap					8
#define DR_slur					9
#define DR_culm_ls			10
#define DR_ivl					11
#define DR_sus					12
#define DR_motion				13
#define DR_beat					14
#define DR_rpenalty_cur 15
#define DR_msh					17
#define DR_uli					18
#define DR_pat					19
#define DR_mshb				  20
#define DR_mshf				  21
#define DR_retrigger    22
#define DR_hbc			    23
#define DR_hli			    24

#ifdef CF_DEBUG

#define CLEAR_READY() ClearReady()
#define CLEAR_READY_PERSIST(...) ClearReadyPersist(##__VA_ARGS__)
#define SET_READY(...) SetReady(##__VA_ARGS__)
#define SET_READY_PERSIST(...) SetReadyPersist(##__VA_ARGS__)
#define CHECK_READY(...) CheckReady(##__VA_ARGS__)
#define CHECK_READY_PERSIST(...) CheckReadyPersist(##__VA_ARGS__)

// Check rule usage
#define ASSERT_RULE(id) { if (SubRuleName[rule_set][id].IsEmpty() && warn_rule_undefined < 5) { ++warn_rule_undefined; CString est; est.Format("Detected undefined rule usage: %d", id); WriteLog(5, est); ASSERT(0); } }

#else

#define CLEAR_READY() 
#define SET_READY(...) 
#define CLEAR_READY_PERSIST(...) 
#define SET_READY_PERSIST(...) 
#define CHECK_READY(...) 
#define CHECK_READY_PERSIST(...) 

#define ASSERT_RULE(id) 

#endif

// Checks if we have leap or melody direction change here: needs to be not first and not last note
#define MELODY_SEPARATION(s, s1) (!s || (leap[s - 1]) || ((c[s] - c[s - 1])*(c[s1] - c[s]) < 0))

// This variant does not use SWA rpenalty optimization
//#define SWA_OPTIMIZER(i)
// This variant skips flags inside SWA window when evaluating not full cp (performance increase)
//#define SWA_OPTIMIZER(i) || (task == tCor && method == mSWA && i >= swa1 && i < swa2 && ep2 < c_len)
// This variant always skips flags inside SWA window (more performance increase
#define SWA_OPTIMIZER(i) || (task == tCor && method == mSWA && (i) >= swa1 && (i) < swa2)

// Report violation from main function
#define FLAG(id, i) do {  \
	ASSERT_RULE(id);  \
	if ((skip_flags SWA_OPTIMIZER(i)) && (accept[id] == 0)) goto skip;  \
	flags[0] = 0;  \
	++flags[id];  \
	anflags[cpv][i].push_back(id);  \
	anfl[cpv][i].push_back(i);  \
} while (0)

// Report violation from child function
#define FLAG2(id, i) do { \
  ASSERT_RULE(id);  \
  if ((skip_flags SWA_OPTIMIZER(i)) && (accept[id] == 0)) return 1;  \
	flags[0] = 0;  \
	++flags[id];  \
	anflags[cpv][i].push_back(id);  \
	anfl[cpv][i].push_back(i);  \
} while (0)

// Report violation and save link
#define FLAG2L(id, i, i2) do { \
  ASSERT_RULE(id);  \
  if ((skip_flags SWA_OPTIMIZER(i)) && (accept[id] == 0)) return 1;  \
	flags[0] = 0;  \
	++flags[id];  \
	anflags[cpv][i].push_back(id);  \
	anfl[cpv][i].push_back(i2);  \
} while (0)

// For harmony
#define FLAG3(id, i) do {  \
	ASSERT_RULE(id);  \
	if (!accept[id]) {  \
		last_flag=id;  \
		return 1;  \
	}  \
} while (0)

// Simply flag
#define FLAG4(id, i) do {  \
	ASSERT_RULE(id);  \
	flags[0] = 0;  \
	++flags[id];  \
	anflags[cpv][i].push_back(id);  \
} while (0)

// This value has to be greater than any penalty. May need correction if step_penalty or pitch_penalty changes
#define MAX_PENALTY 10000000.0
#define MAX_RULES 500

// Maximum clib size without warning
#define MAX_CLIB_WARN 500

// Time to reserve for analysis for autotest
#define ANALYZE_RESERVE 1

#define MAX_RULESETS 100
#define MAX_SEVERITY 101
#define MAX_WIND 500
#define MAX_NOTE 127
// How often to show statistics (ms)
#define STATUS_PERIOD 100

const CString MethodNames[] = {
	"window-scan", // 0
	"swa" // 1
};

// Letters in harmonies
const int hvt[] = { 1, 0, 1, 0, 0, 1, 0 };
const int hvd[] = { 0, 0, 1, 0, 1, 0, 1 };
const int hvs[] = { 0, 1, 0, 1, 0, 1, 0 };

#define SCAN_VISUAL_CODE_BASE 3
const char scan_visual_code[] = { '!', '.', ',', ':', ';', '`', '|', '(', ')', '[', ']', '{', '}', ' / ', '\\' };

#define mUndefined -1
#define mScan 0
#define mSWA 1

const CString TaskNames[] = {
	"evaluate", // 0
	"correct", // 1
	"generate" // 2
};

// Task
#define tGen 0
#define tEval 1
#define tCor 2

// Harmonic functions
#define hUndefined -1
#define hTon 0
#define hSub 1
#define hDom 2

// Interval types
#define iDis   -2 // Dissonance
#define iHarm4 -1 // Harmonic 4th
#define iPco   2 // Perfect consonance
#define iIco   3 // Imperfect consonance

// Rule string types
#define rsName 0 // Rule name
#define rsSubName 1 // Subrule name
#define rsComment 2 // Rule comment
#define rsSubComment 3 // Subrule comment

class CGenCF1 :
	public CGMidi
{
public:
	CGenCF1();
	~CGenCF1();
	void Generate() override;

protected:
	void LoadHarmVar();
	void LoadHSP(CString fname);
	void LoadRules(CString fname);
	void CheckRuleList(CString list_name, vector<vector<int>>& v);
	int Interval2Chromatic(int iv);
	void ParseRule(int rset, int rid, int type);
	int GetRuleParam(int rset, int rid, int type, int id);
	void ParseRules();
	void SetRuleParams();
	inline void ProcessSpecies();
	void CheckConfig();
	int SelectRuleSet(int rs);
	void LoadHarmNotation();
	void LoadConfigLine(CString * sN, CString * sV, int idata, float fdata);
	void LogCantus(CString st3, int x, int size, vector<int>& c);
	CString vint2st(int size, vector<int>& c);
	inline int FailNoteRepeat(vector<int> &c, int step1, int step2);
	inline int FailNoteSeq(vector<int>& pc);
	inline int FailLocalRange(vector<int>& c, int notes, int mrange, int flag);
	inline int FailLocalPiCount(vector<int>& cc, int notes, int picount, int flag);
	inline int FailLocalMacc(int notes, float mrange, int flag);
	inline void GetMelodyInterval(vector<int>& cc, int step1, int step2, int & nmin, int & nmax);
	inline void ClearFlags(int step1, int step2);
	inline void GetPitchClass(vector<int>& c, vector<int>& cc, vector<int>& pc, vector<int>& pcc, int step1, int step2);
	inline int FailMelodyHarmStep(int i, const int * hv, int & count, int & wcount, int & last_flag, int & max_p);
	inline int FailGisTrail(vector<int>& pcc);
	inline int FailFisTrail(vector<int>& pcc);
	inline int EvalMelodyHarm(int p, int & last_flag, int & max_p);
	inline int FailMelodyHarm(vector<int>& pc, vector<int>& pcc);
	//inline void UpdateNoteHarm(int i);
	//inline int FailMelodyHarmMiss(vector<int>& pc, int i, int harm, int & count, int & wcount);
	//inline int FixNoteHarmRepeat(vector<int>& pc, int i, int harm, int & count, int & wcount);
	void CalcCcIncrement();
	inline void GetChromatic(vector<int>& c, vector<int>& cc, int step1, int step2, int minor_cur);
	inline int FailDiatonic(vector<int>& c, vector<int>& cc, int step1, int step2, int minor_cur);
	inline int FailOutstandingRepeat(vector<int>& c, vector<int>& cc, vector<int>& leap, int scan_len, int rlen, int fid);
	inline int FailLongRepeat(vector<int>& c, vector<int>& cc, vector<int>& leap, int scan_len, int rlen, int fid);
	inline int FailManyLeaps(vector<int>& c, vector<int>& cc, vector<int>& leap, vector<int>& smooth, vector<int>& slur, int mleaps, int mleaped, int mleaps2, int mleaped2, int mleapsteps, int flag1, int flag2, int flag3, int flag4);
	inline void GetLeapSmooth(vector<int>& c, vector<int>& cc, vector<int>& leap, vector<int>& smooth, vector<int>& slur);
	inline int FailLeapSmooth(vector<int>& c, vector<int>& cc, vector<int>& leap, vector<int>& smooth, vector<int>& slur, int l_max_smooth, int l_max_smooth_direct, int flag1, int flag2, int first_run);
	inline int FailStagnation(vector<int>& cc, vector<int>& nstat, int steps, int notes, int flag);
	inline int FailMultiCulm(vector<int>& cc, vector<int>& slur);
	inline int FailFirstNotes(vector<int>& pc);
	inline int FailLastNotes(vector<int>& pc, vector<int>& pcc);
	inline void CreateLinks(vector<int>& cc, int multivoice);
	inline void CountFillInit(vector<int>& c, int tail_len, int pre, int & t1, int & t2, int & fill_finish);
	inline void CountFill(vector<int>& c, int tail_len, vector<int>& nstat2, vector<int>& nstat3, int & skips, int & fill_to, int pre, int & fill_to_pre, int & fill_from_pre, int & fill_from, int & deviates, int & dev_count, int leap_prev, int & fill_finish);
	inline void CountFillSkips(int leap_prev, int & skips, int t1, int t2);
	inline void CountFillLimits(vector<int>& c, int pre, int t1, int t2, int & fill_to, int & fill_to_pre, int & fill_from_pre, int & fill_from);
	inline void FailLeapInit(vector<int>& c, int & late_leap, int & presecond, int & leap_next, int & leap_prev, int & arpeg, int & overflow, vector<int>& leap);
	inline int FailLeapMulti(int leap_next, int & arpeg, int & overflow, int & child_leap, vector<int>& c, vector<int>& leap);
	inline int FailLeap(vector<int>& c, vector<int>& cc, vector<int>& leap, vector<int>& smooth, vector<int>& nstat2, vector<int>& nstat3);
	inline int FailLeapFill(vector<int>& c, int late_leap, int leap_prev, int child_leap);
	inline int FailLeapMDC(vector<int>& leap, vector<int>& cc);
	inline void PrepareTonicWeight();
	inline float GetTonicWeight(int l_ls, vector<int>& cc, vector<int>& pc);
	inline int FailTonic(vector<int> &cc, vector<int>& pc);
	inline int FailLastNoteRes(vector<int>& pc);
	inline int FailIntervals(vector<int>& c, vector<int>& cc, vector<int>& pc, vector<int>& pcc);
	inline void GetTritoneResolution(int ta, int t1, int t2, int tb, int & res1, int & res2, vector<int>& c, vector<int>& cc, vector<int>& pc, vector<int>& pcc);
	inline int FailTritone(int ta, int t1, int t2, int tb, vector<int>& c, vector<int>& cc, vector<int>& pc, vector<int>& pcc, vector<int>& leap);
	inline int FailTritones(vector<int>& c, vector<int>& cc, vector<int>& pc, vector<int>& pcc, vector<int>& leap);
	inline int FailGlobalFill(vector<int>& c, vector<int>& nstat2);
	void ScanInit();
	void ScanCantusInit();
	int GetMinSmap();
	int GetMaxSmap();
	void GetRealRange(vector<int>& c, vector<int>& cc);
	void GetSourceRange(vector<int>& cc);
	void ApplySourceRange();
	void SingleCantusInit();
	inline void ResizeToWindow();
	void MakeNewCantus(vector<int>& c, vector<int>& cc);
	void MultiCantusInit(vector<int>& c, vector<int>& cc);
	int FailWindowsLimit();
	inline void CalcFlagStat();
	inline int FailFlagBlock();
	inline int FailAccept();
	inline void NextWindow(vector<int> &cc);
	inline void CalcRpenalty(vector<int>& cc);
	inline void ScanLeft(vector<int> &cc, int &finished);
	inline void ShowBestRejected(vector<int>& cc);
	inline void BackWindow(vector<int>& cc);
	void WritePerfLog();
	inline int CalcDpenalty(vector<int>& cc1, vector<int>& cc2, int s1, int s2);
	inline void CalcStepDpenalty(vector<int> &cc1, vector<int> &cc2, int i);
	inline int NextSWA(vector<int>& cc, vector<int>& cc_old);
	inline void SaveBestRejected(vector<int>& cc);
	inline int FailMinor(vector<int>& pcc, vector<int>& cc);
	inline void ShowScanSpeed();
	inline char GetScanVisualCode(int i);
	inline void ShowScanStatus();
	void CheckClibSize();
	inline void ReseedCantus();
	inline void TimeBestRejected();
	inline void SaveCantusIfRp();
	void ScanCantus(int t, int v, vector<int>* pcantus);
	inline void ScanRight(vector<int>& cc);
	void WriteFlagCor();
	void ShowFlagStat();
	void ShowStuck();
	CString GetStuck();
	void ShowFlagBlock();
	void SaveCantus();
	void TestRpenalty();
	void TestBestRpenalty();
	inline void TransposeVector(vector<int>& v, int t);
	inline void TransposeVector(vector<float>& v, int t);
	inline void maVector(vector<float>& v, vector<float>& v2, int range);
	inline void maVector(vector<int>& v, vector<float>& v2, int range);
	inline void mawVector(vector<int>& v, vector<float>& v2, int range);
	inline void mawVector(vector<float>& v, vector<float>& v2, int range);
	inline void MakeMacc(vector<int>& cc);
	inline void InterpolateNgraph(int v, int step0, int step);
	void SendNgraph(int pos, int i, int v, int x);
	void SendGraph(int pos, int i, int v, int x);
	void SendLyrics(int pos, int v, int av, int x);
	void SendComment(int pos, int v, int av, int x, int i);
	void TransposeCantusBack();
	void SendNotes(int pos, int i, int v, int av, int x, vector<int>& cc);
	void MakeBellDyn(int v, int step1, int step2, int dyn1, int dyn2, int dyn_rand);
	int SendPause(int pos, int v);
	inline void MakeLenExport(vector<int>& cc, int av, int retr_on);
	inline void SendHarmColor(int pos, int v);
	void MergeNotes(int step1, int step2, int v);
	inline void CalcPmap(vector<int>& pcc, vector<int>& cc, vector<int>& c, vector<int>& smooth, vector<int>& leap);
	inline void GetPmap();
	CString GetPmapLogHeader();
	CString GetPmapLogSt();
	inline void LogPmap();
	CString GetHarmName(int pitch, int alter);
	int SendCantus();
	int InitGen();
	int InitCantus();
	void TestDiatonic();
	void LogPerf();
	void CheckSASEmulatorFlags(vector<int> &cc);
	void OutputFlagDelays();
	void EmulateSAS();
	void RandomSWA();
	void SWA(int i, int dp);
	void FillCantus(vector<int>& c, int step1, int step2, int value);
	void FillCantus(vector<int>& c, int step1, int step2, vector<int> &value);
	void FillCantus(vector<int>& c, int step1, int step2, vector<vector<int>> &value);
	void RandCantus(vector<int>& c, vector<int>& cc, int step1, int step2);
	void CalculateCcOrder(vector<int>& cc_old, int step1, int step2);
	void FillCantusMap(vector<int>& c, vector<int>& smap, int step1, int step2, vector<vector<int>>& value);
	void FillCantusMap(vector<int>& c, vector<int>& smap, int step1, int step2, vector<int>& value);
	void FillCantusMap(vector<int>& c, vector<int>& smap, int step1, int step2, int value);
	
	// Rules
	vector <vector<int>> accepts; // Each 1 allows showing canti with specific properties
	int cf_rule_set = 0; // id of current rule set for cantus
	int cp_rule_set = 0; // id of current rule set for cpoint
	vector<vector<vector<vector<int>>>> RuleParam; // Parsed rule parameters
	vector <CString> RuleGroup; // Groups for flag groups
	int max_flags = 82; // Maximum number of rules

	// Parameters
	int sus_last_measures = 3; // Last measures in which sus is allowed in species 2 and 3
	int mea_per_sus = 5; // Maximum measures per suspension
	int lclimax_notes = 12; // Number of adjacent notes to calculate local climax
	int lclimax_mea = 6; // Number of adjacent measures to calculate local climax
	int lclimax_mea2 = 4; // Number of adjacent measures to calculate local climax
	int cantus_high = 0; // Set to 1 to consider cantus to be higher voice
	int voice_high = 0; // Shows if currently processed voice is high
	int specified_high = 0; // If cantus_high was specified in midi file
	int method = mUndefined; // Which generation / analysis method to use
	int min_interval = 7; // Minimum chromatic interval in cantus (12 = octave)
	int max_interval = 12; // Maximum chromatic interval (12 = octave)
	int max_interval_cf = 12; // Maximum chromatic interval in cf (12 = octave)
	int max_interval_cp = 12; // Maximum chromatic interval in cp (12 = octave)
	int min_intervald = 4; // Minimum diatonic interval in cantus (7 = octave)
	int max_intervald = 7; // Maximum diatonic interval in cantus (7 = octave)
	int c_len = 9; // Number of measures in each cantus. Usually 9 to 11
	int s_len = 4; // Maximum number of steps to full scan
	int swa_len = 4; // Maximum number of steps for SWA
	int first_note = 72; // Starting note of each cantus
	int first_note0 = 0; // Saved first note during generation
	int last_note = 72; // Ending note of each cantus
	int last_note0 = 0; // Saved last note during generation
	int fill_steps_mul = 2; // Multiply number of notes between leap notes to get steps for filling
	int max_repeat_mul = 2; // Allow repeat of X notes after at least X*max_repeat_mul steps if beats are different
	int max_smooth_direct = 5; // Maximum linear movement in one direction allowed (in steps)
	int max_smooth = 7; // Maximum linear movement allowed (in steps)
	int max_smooth_direct2 = 5; // Maximum linear movement in one direction allowed in cp (in steps)
	int max_smooth2 = 7; // Maximum linear movement allowed in cp (in steps)
	int max_leaps = 2; // Maximum allowed leaps during max_leap_steps
	int max_leaped = 3; // Maximum allowed leaped-over-notes during max_leap_steps
	int max_leap_steps = 7;
	int max_leaps2 = 2; // Maximum allowed leaps during max_leap_steps2
	int max_leaped2 = 3; // Maximum allowed leaped-over-notes during max_leap_steps
	int max_leap_steps2 = 7;
	int max_leaps3 = 2; // Maximum allowed leaps during max_leap_steps
	int max_leaped3 = 3; // Maximum allowed leaped-over-notes during max_leap_steps
	int max_leaps4 = 2; // Maximum allowed leaps during max_leap_steps
	int max_leaped4 = 3; // Maximum allowed leaped-over-notes during max_leap_steps
	int max_leaps5 = 2; // Maximum allowed leaps during max_leap_steps (virtual)
	int max_leaped5 = 3; // Maximum allowed leaped-over-notes during max_leap_steps (virtual)
	int max_leaps6 = 2; // Maximum allowed leaps during max_leap_steps (virtual)
	int max_leaped6 = 3; // Maximum allowed leaped-over-notes during max_leap_steps (virtual)
	int cse_leaps = 2; // Maximum allowed consecutive leaps for Consecutive leaps
	int cse_leaps2 = 3; // Maximum allowed consecutive leaps for Consecutive leaps+
	int hsp_leap = 5; // Maximum allowed leap before bad harmonic sequence
	int early_culm = 3; // Early culmination step
	int late_culm = 3; // Late culmination step
	int tritone_res_quart = 4; // Search X quarters for tritone resolution
	int show_harmony_bass = 1; // 0 = do not show bass, 1 = Show harmony bass for higher cantus only, 2 = always show harmony bass
	int fill_pre3_notes = 5; // How many notes to search for fill preparation for compensation to 3rd
	int fill_pre4_notes = 5; // How many notes to search for fill preparation for compensation to Xth in the end
	int fill_pre4_int = 4; // Interval to be compensated in the end

	int log_pmap = 0; // Set to 1 to enable logging parameter map to log folder. Needs canculate_stat to work correctly
	int show_allowed_flags = 0; // Show even allowed flags(bold in rules.xlsm)
	int show_ignored_flags = 0; // Show even ignored flags(with strikethrough in rules.xlsm)
	int show_note_scan_range = 1; // Internal variable - allows to disable showing note scan range
	int early_culm2 = 4; // Early culmination step (second rule)
	int early_culm3 = 50; // Early culmination step percent
	int stag_notes = 2; // Maximum allowed stag_notes (same notes) during stag_note_steps
	int stag_note_steps = 7;
	int stag_notes2 = 3; // Maximum allowed stag_notes2 (same notes) during stag_note_steps2
	int stag_note_steps2 = 10;
	int random_key = 0; // Allow CF1 to select random key and CA1 to select any of possible keys regardless of last note
	int confirm_mode = 1; // 0 - do not confirm expected flags; 1 - confirm global / local; 2 - confirm mistakes
	int min_tempo = 110;
	int max_tempo = 120;
	int rpenalty_accepted = 0; // Maximum accepted rule penalty for RandomSWA
	int first_steps_tonic = 3; // Number of first steps, which must contain tonic note
	int notes_picount = 5; // Maximum number of consecutive notes having low pitch count
	int min_picount = 3; // Minimum allowed pitch count of notes_picount consecutive notes
	int notes_picount2 = 5; // Maximum number of consecutive notes having low pitch count
	int min_picount2 = 3; // Minimum allowed pitch count of notes_picount2 consecutive notes
	int notes_picount3 = 5; // Maximum number of consecutive notes having low pitch count
	int min_picount3 = 3; // Minimum allowed pitch count of notes_picount3 consecutive notes

	int notes_lrange = 6; // Maximum number of consecutive notes having low range
	int min_lrange = 5; // Minimum allowed local range of notes_lrange consecutive notes
	int notes_lrange2 = 13; // Maximum number of consecutive notes having low range
	int min_lrange2 = 8; // Minimum allowed local range of notes_lrange consecutive notes
	int notes_lrange3 = 5; // Maximum number of consecutive notes having low range
	int min_lrange3 = 3; // Minimum allowed local range of notes_lrange consecutive notes

  // For sp3/5
	int notes_lrange0 = 6; // Maximum number of consecutive notes having low range
	int min_lrange0 = 5; // Minimum allowed local range of notes_lrange consecutive notes
	int notes_lrange02 = 13; // Maximum number of consecutive notes having low range
	int min_lrange02 = 8; // Minimum allowed local range of notes_lrange consecutive notes
	int notes_lrange03 = 5; // Maximum number of consecutive notes having low range
	int min_lrange03 = 3; // Minimum allowed local range of notes_lrange consecutive notes

	// For CF/sp1:
	int notes_lrange1 = 6; // Maximum number of consecutive notes having low range
	int min_lrange1 = 5; // Minimum allowed local range of notes_lrange consecutive notes
	int notes_lrange12 = 13; // Maximum number of consecutive notes having low range
	int min_lrange12 = 8; // Minimum allowed local range of notes_lrange consecutive notes
	int notes_lrange13 = 5; // Maximum number of consecutive notes having low range
	int min_lrange13 = 3; // Minimum allowed local range of notes_lrange consecutive notes

	int notes_arange = 5; // Maximum number of consecutive notes having low average range
	float min_arange = 5; // Minimum allowed local range of notes_arange consecutive notes
	int notes_arange2 = 5; // Maximum number of consecutive notes having low average range
	float min_arange2 = 5; // Minimum allowed local range of notes_arange consecutive notes
	float random_choose = 100; // Percent of accepted canti to show and play
	int random_seed = 0; // Seed melody with random numbers. This ensures giving different results if generation is very slow.
	int random_range = 0; // Limit scanning to one of possible fast-scan ranges
	int accept_reseed = 1; // After accepting first result reseed (if random_seed) and choose new range (if random_range)
	int swa_inrange = 0; // Do not update minc/maxc - stay with existing limits (used for Random SWA)
	int calculate_correlation = 0; // Enables correlation calculation algorithm. Slows down generation. Outputs to cf1-cor.csv
	int calculate_blocking = 0; // Enables blocking flags calculation algorithm. Slows down generation.
	int calculate_stat = 0; // Enables flag statistics calculation algorithm. Slows down generation.
	int calculate_ssf = 1; // Enables SWA stuck flags statistics calculation algorithm.
	int best_rejected = 0; // Show best rejected results if rejecting more than X ms. Set to 0 to disable. Slows down generation
	int show_severity = 0; // =1 to show severity and flag id in square brackets in comments to notes (also when exporting to MIDI file)
	int repeat_letters = 3; // Maximum repeated letters in a row of harmonies
	int miss_letters = 3; // Maximum steps with missed letters in a row of harmonies
	int gis_trail_max = 7; // Minimum notes between G# and next G note in Am
	int fis_leap = 5; // Maximum allowed chromatic leap size before F#
	int c4p_last_meas = 3; // Last measures that can have leap c4p compensated
	int c4p_last_steps = 3; // Last steps that can have leap c4p compensated (converted from measures)
	int c4p_last_notes = 3; // Last notes that can have leap c4p compensated
	int c4p_last_notes2 = 3; // Last notes that can have leap c4p compensated (corrected with regard to measures)
	int pre_last_leaps = 2; // Last leaps that can be precompensated
	int repeat_notes2 = 2; // Number of repeated notes
	int repeat_notes3 = 3; // Number of repeated notes
	int repeat_notes5 = 5; // Number of repeated notes
	int repeat_notes7 = 7; // Number of repeated notes
	int repeat_steps2 = 8; // Prohibit repeating of 2 notes closer than repeat_steps between first notes (if beats are same)
	int repeat_steps3 = 8; // Prohibit repeating of 3 notes closer than repeat_steps between first notes (if beats are same)
	int repeat_steps5 = 15; // Prohibit repeating of 5 notes closer than repeat_steps between first notes
	int repeat_steps7 = 100; // Prohibit repeating of 7 notes closer than repeat_steps between first notes
	int tonic_window = 9; // Number of notes that are searched for number of tonic notes
	int tonic_max = 1; // Maximum number of tonic notes that can be contained in tonic window
	int tonic_window_cp = 9; // Number of harmonies that are searched for number of tonic chords
	int tonic_max_cp = 1; // Maximum number of tonic chords that can be contained in tonic window
	vector<int> max_note_len; // [sp] Maximum note real length in croches by species

	int tonic_leap = 4; // Maximum allowed leap to tonic (chromatic) without weight change
	int tonic_wei_leap = 50; // Weight of leap to tonic
	int tonic_wei_pco = 30; // Weight of tonic perfect consonance
	int tonic_wei_beat = 30; // Weight of downbeat tonic
	int tonic_wei_long = 30; // Weight of tonic longer than left neighbor
	int tonic_wei_len = 20; // Tonic length decrease two times decreases weight by X

	int thirds_ignored = 1; // Number of thirds ignored for consecutive leaps rule
	int fis_gis_max = 3; // Maximum allowed distance between F# and G#
	int fis_g_max = 3; // Minimum distance from G to F# (+1 to allow)
	int fis_g_max2 = 3; // Minimum distance from F# to G (+1 to allow)
	int dev_late2 = 3; // Maximum note count to consider non-late leap compensation deviation to 2nd
	int dev_late3 = 3; // Maximum note count to consider non-late leap compensation deviation to 3rd
	int dev2_maxlen = 1; // Maximum >5th 2nd deviation length in number of 1/4
	int late_require = 0; // Allow not-last scan window to have no needed tags, but no blocked tags 
	int approx_steps = 4; // Maximum number of steps to approximate corrections in one iteration
	vector <vector <int>> hv; //  [pc][] Variants of note harmonic meaning
	vector <vector <int>> hsp; // [pc][pc] Harmonic sequence penalty
	vector <int> fli; // [ls] Forward links to start of each non-slurred note
	vector <int> fli2; // [ls] Forward links to end of each non-slurred note
	vector <int> llen; // [ls] Length of each linked note in steps
	vector <int> rlen; // [ls] Real length of each linked note (in croches)
	vector <int> bli; // [s] Back links from each step to fli2
	vector <int> uli; // [us] Forward links to start of each unique note column
	vector <float> tweight; // [ls] Tonic weight for each note
	vector <int> g_leaps; // [s] Number or leaps in window
	vector <int> g_leaped; // [s] Number or leaped notes in window
	int minl = 0, maxl = 0;
	int fli_size; // Size of filled fli2 vector
	// Random SWA
	//int fullscan_max = 7; // Maximum steps length to full scan. If melody is longer, use SWA
	int approximations = 30; // Maximum number of approximations to run if penalty decreases
	int swa_steps = 6; // Size of Sliding Window Approximation algorithm window in steps
	int step_penalty = 3; // Penalty for adding one more changing step while correcting cantus
	int pitch_penalty = 1; // Penalty for changing note one more diatonic step while correcting cantus
	int optimize_dpenalty = 1; // Saves only melodies closest to source melody. Decreases memory usage. Resetting allows for more close results when corrections>1
	int transpose_back = 0; // Set to 1 to transpose generated melody closer to initial first note. Can be set to 1 only for CF1 generation algorithms
	int transpose_cantus = 5; // Specify octave to transpose lower cantus to

	// CA1
	CString fpenalty_source; // Source fpenalty string
	int cor_ack = 0; // Acknowledge correction results by running both algorithms: SAS and ASWA
	int correct_range = 4; // Maximum interval allowed between each source and corrected note
	int correct_inrange = 0; // Limit allowed range of corrected melody to range of source melody
	int animate = 100; // Draw animation of preliminary result every X ms (0 to show each change, high to disable animation)
	int animate_delay = 0; // Delay in ms after each animation step
	int max_correct_ms = 0; // Maximum time in milliseconds to correct using window-scan (set to 0 to scan up to the end)

	// CA1 local variables
	long long correct_start_time; // Time when current correction started
	long long scan_start_time; // Time when current scan started
	int acycle = 0; // Animation time divided by animate (ms)
	int is_animating = 0; // Set to 1 to show than Send is animating
									
	// Master parameters
	vector <int> *scantus; // [s] Source cantus for processing
	int task; // What task to accomplish using the method
	int svoice; // Voice to send cantus to

	// CA1
	CString midi_file;
	int corrections = 1; // Number of corrections to show

	// Parameter map
	int pm_range, pm_sumint, pm_between_min, pm_between_max;
	int pm_culm_count;
	int pm_sharp6, pm_flat6, pm_tonic;
	int pm_sharp7, pm_flat7;
	int pm_contrary, pm_direct, pm_parallel;
	int pm_dis, pm_ico, pm_pco, pm_pico;
	float pm_llen;
	float pm_decc_min, pm_decc_max, pm_decc_av;
	float pm_maccr_min, pm_maccr_max, pm_maccr_av;
	int pm_sus, pm_anti;
	int pm_croche;
	int pm_leaps, pm_smooth, pm_leaps2, pm_leaps3;
	int pm_leapsum;
	int pm_win_leaps, pm_win_leapnotes;
	float pm_tw_max; // Maximum tonic weight

  // Local
	int hrepeat_fired = 0; // Harmonic repeat in step
	int hmiss_fired = 0; // Harmonic miss in step
	// Queues for calculating scan speed and displaying in status
	int svoices = 1; // Scan voices
	CString pmap;
	vector<float> tonic_weight; // Vector of tonic weights based on length
	deque<long long> q_scan_ms;
	deque<long long> q_scan_cycle;
	int step00 = 0; // Start of source cantus/counterpoint in case of SAS emulation
	int step0 = 0; // Start of current cantus
	int step1 = 0; // Stop of current cantus
	int warn_clib_max = 0; // If warning of maximum clib size fired
	int warn_rule_undefined = 0; // If warning of undefined rules fired
	int scan_full = 0; // If scan was completed without timeout and interruption
	int swa_full = 0; // If swa was completed up to maximum possible swa_steps (2 = swa_len cannot be increased)
	int status_cycle = 0; // Scan time divided by status period (ms)
	int cpv; // Current counterpoint voice
	int cfv; // Current cantus voice
	int av_cnt = 1; // Number of voices in counterpoint
	int seed_cycle, reseed_count;
	long cantus_ignored = 0; // How many canti ignored and not sent
	long cantus_sent = 0; // How many cantus have been sent
	int step = 0; // Global step
	long long accepted = 0; // Number of accepted canti
	int flags_need2 = 0; // Number of second level flags set
	vector<int> cpos; // [s] Position of cc step in output vectors
	vector<int> m_c; // [s] Cantus diatonic
	vector<int> m_cc; // [s] Cantus chromatic
	vector<int> m_pc; // [s] pitch class (diatonic)
	vector<int> m_pcc; // [s] pitch class (chromatic)
	vector<int> m_leap; // [s]
	vector<int> m_smooth; // [s] 
	vector<int> m_slur; // [s] 
	vector<float> macc; // [s] CC moving average
	vector<float> macc2; // [s] CC moving average smoothed
	vector<int> lclimax; // [s] Local highest note (chromatic)
	vector<int> lclimax2; // [s] Local highest note (chromatic)
	int macc_range = 0; // Steps outside window used to build macc
	int macc2_range = 0; // Steps outside window used to build macc2
	vector<float> decc; // [s] CC deviation
	vector<float> decc2; // [s] CC deviation smoothed
	vector<float> fpenalty; // [r_id] Additional penalty for flags
	vector<int>  flags; // [r_id] Flags for whole cantus
	vector<vector<vector<int>>> anflags; // [v][s][] Note flags
	vector<vector<vector<int>>> anfl; // [v][s][] Note flags links
	vector<int> br_cc; // [s] Cantus chromatic (best rejected)
	vector<int>  br_f; // [r_id] Flags for whole cantus (best rejected)
	vector<long>  ssf; // [r_id] SWA stuck flags
	vector<int>  best_flags; // [r_id] best flags of saved cantus for swa
	vector<vector<int>> br_nf; // [s][] Note flags (best rejected)
	vector<vector<int>> br_nfl; // [s][] Note flags links (best rejected)
	float rpenalty_cur = 0; // Rules penalty
	float rpenalty_source = 0; // Source melody rpenalty
	float rpenalty_min; // Minimum rules penalty for this scan
	vector <float> rpenalty; // [cid] Penalty in terms of sum of flag severity
	vector <float> src_rpenalty_step; // [s] Penalty in terms of sum of flag severity
	int dpenalty_cur = 0; // Distance from source penalty
	int dpenalty_min; // Minimum distance penalty for this scan
	vector <int> dpenalty; // [cid] Penalty in terms of distance from source
	vector <int> cc_len; // [s] Length of each cantus note
	vector <float> cc_tempo; // [s] Tempo of each cantus note
	int real_len; // Total length of cantus in steps
	int skip_flags, clear_flags;
	int sp1, sp2, swa1=0, swa2=0, ep1, ep2, p, pp;
	long long accepted2 = 0, accepted3 = 0;
	int first_note_dia, first_note_oct;
	int wid; // Window id
	int swid; // SWA Window id
	int dpenalty_outside_swa; // Sum of dpenalty outside SWA range
	vector<int> m_cc_old; // [s] Cantus chromatic saved for SWA
	vector<int> wpos1; // [wid]
	vector<int> wpos2; // [wid]
	vector<int> swpos1; // [swid]
	vector<int> swpos2; // [swid]
	vector<int> dpenalty_step; // [s] Dpenalty of all steps up to current
	vector <int> smap; // [sid] Map of links from matrix local IDs to cantus step IDs
	vector<int> min_c; // [s]
	vector<int> max_c; // [s]
	vector<int> min_cc; // [s] Current range
	vector<int> max_cc; // [s]
	int max_cc2; // Maximum of all max_cc values
	vector<int> min_cc0; // [s] Source range for RSWA
	vector<int> max_cc0; // [s] 
	int minc, maxc; // Real possible limits
	vector<vector<vector<long>>> fblock; // [wid][r_id][r_id] number of canti rejected with foreign flags
	vector<long long> fstat; // [r_id] number of canti with each flag
	vector<vector<long long>> fcor; // [r_id][r_id] Flags correlation matrix
	vector<long long> accepted4; // [wid] number of accepted canti per window
	vector<long long> accepted5; // [wid] number of canti with neede flags per window
	vector<long long> wscans; // [wid] number of full scans per window
	int wcount = 1; // Number of windows created
	int swcount = 0; // Number of SWA windows created
	long long cycle = 0; // Cycle number of full scan
	long long tcycle = 0; // Total cycles number
	long cor_sent = 0; // Total number of corrections sent
	long cor_full = 0; // Total number of full corrections sent
	long cor_rp0 = 0; // Total number of corrections up to rpenalty 0 sent
	long swa_sum = 0; // Total sum of swa levels
	long dp_sum = 0; // Total sum of dpenalty
	long rp_sum = 0; // Total sum of rpenalty
	long long accept_time; // Last accepted timestamp
	int rcycle = 0; // Rejected time divided by best_rejected (ms)
	int nmin, nmax, nmind, nmaxd;
	int src_nmin = 0, src_nmax = 1000; // Source range (chromatic)
	int cc_incr[MAX_NOTE]; // cc increments for each step
	int cc_decr[MAX_NOTE]; // cc decrements for each step
	vector<int> test_cc; // [s]
	vector<int> nstat; // [cc]
	vector<int> nstat2; // [c]
	vector<int> nstat3; // [c]
	vector<vector<int>> hm; // [ls][] Available harmonic meanings for each note
	//vector<vector<int>> hm2; // [ls][] Required harmonic meanings for each note
	vector <int> chm, chmp; // [ls/hs] Current harmonic meaning and its position in hm
	vector <int> chm_alter; // [ls/hs] Type of harmonic meaning
	float hdif;
	int cantus_id = 0;
	CString key_eval; // Results of key evaluation
	int culm_ls; // Position of culmination after FailMultiCulm
	int cf_culm_s = -1; // Position of cantus firmus culmination in cp steps
	int cf_culm_cfs = -1; // Position of cantus firmus culmination in cf notes
	int fn = 0; // First note of analyzed melody
	int fn_source = 0; // First note of source melody
	vector<float> maw; // [] Moving average weight
	vector<int> len_export; // [s] For Send
	vector<int> coff_export; // [s] For Send

	// Random s_len=1 scan
	vector<int> cc_id; // [s] Current successive identifier of chromatic step
	vector<vector<int>> cc_order; // [s][] Randomized chromatic steps

	// FailLeap local variables
	int leap_start; // Step where leap starts
	int leap_end; // Step where leap ends
	int leap_mid; // Middle step of leap in case when leap is created by two consecutive 3rds
	int fleap_start; // fli2 position where leap starts
	int fleap_end; // fli2 position where leap ends
	int leap_size; // Diatonic size of leap
	int leap_id; // Id of leap size
	int filled, prefilled; // If leap is filled and prefilled
	int mdc1, mdc2; // Status of melody direction change before and after leap
	vector <int> tc; // [] Tail diatonic notes

	// Local link steps
	int ms; // Link step inside mli
	int ls; // Link step inside fli
	int s; // Current step
	int s0, s1, s2; // +1, +2 steps
	int s_1, s_2; // -1, -2 steps

	// Local SWA
	vector <long> cids; // []

	// CA1 autotest
	vector<vector<int>> enflags; // [s][id] Expected note flags
	vector<vector<int>> enflags2; // [r_id][s] Expected note flags
	vector<int> enflags3; // [r_id] Expected flags
	int enflags_count = 0; // Number of expected flags for melody
	vector<int> false_positives_ignore; // [r_id] Ignore false positives for these flags
	vector<int> false_positives_global; // [r_id] Always check false positives for these flags
	vector<int> sas_emulator_max_delay; // [r_id] Specify rule identiefiers, which should not be tested for delay in SAS emulator
	vector<int> sas_emulator_move_ignore; // [r_id] Specify rule identiefiers, which should not be tested for moves in SAS emulator
	vector<int> sas_emulator_unstable; // [r_id] Specify rule identiefiers, which can appeare in emulator, but not in main analysis
	vector<vector<int>> sas_emulator_replace; // [r_id][] First flag can replace second in SAS emulator
	vector<vector<int>> flag_replace; // [r_id][] First flag can replace second on second scan or in SAS emulator
	vector<int> flags_full; // [r_id] Flags of full analysis
	vector<vector<int>> nflags_full; // [s][] Note flags of full analysis
	vector<vector<int>> nflags_prev; // [s][] Note flags of previous SAS run
	vector<int> flag_delay; // [r_id] Maximum flag delay in steps
	vector<CString> flag_delay_st; // [r_id] Information about maximum flag delay

	// Cantus correcting
	vector <int> smatrix; // [s] Vector of links to steps that were selected for recalculation
	int smatrixc = 0; // Number of steps marked in smatrix
	vector<vector<int>> clib; // [][s] Library of cantus
	VSet<int> clib_vs; // Unique clib set

	// CP1 parameters
	int sus_insert_max_leap = 9; // Maximum leap in sus resolution insertion
	int dnt_max_leap = 5; // Maximum leap (chromatic) allowed in double-neighbour tones
	int cambiata_max_leap3 = 5; // Maximum allowed leap from third note in cambiata
	int cambiata_max_leap4 = 5; // Maximum allowed leap from fourth note in cambiata
	int show_correct_hatch = 1; // Set to 1 to mark corrected notes with hatch
	int show_min_severity = 0; // Minimum severity to highlight note
	int prohibit_min_severity = 0; // Minimum severity to prohibit (below are marked as allowed)
	int harm_notation = 1; // 1 - wordwide harmonic notation(I, II...), 2 - Sposobin harmonic notation(T, SII...)
	int show_hatch = 1; // 0 - show no hatch, 1 = show dissonance hatch, 2 = show msh hatch
	int npm = 1; // Number of notes per measure
	int slurs_window = 10; // Number of steps to check for slur limit
	int miss_slurs_window = 10; // Number of steps to check for missing slur limit
	int ico_chain = 3; // Number of allowed consecutive imperfect consonances
	int ico_chain2 = 3; // Number of allowed consecutive imperfect consonances
	int min_between = 0; // Minimum diatonic interval between voices
	int max_between = 11; // Maximum diatonic interval between voices
	int sum_interval = 22; // Maximum chromatic range of cantus and counterpoint
	int burst_steps = 3; // Maximum number of steps for which max_between can be exceeded
	int burst_between = 11; // Maximum interval between voices for burst_steps steps
	int reduce_between = 50; // How many percent of notes have to be outsize max_between range to be moved closer. Set to 0 to disable reduction
	int c_repeats = 1; // Maximum number of C note repeats within window
	int ca_repeats = 1; // Maximum number of accented C note repeats within window
	int c_window = 10; // Maximum number of C note repeats within window
	int ca_window = 10; // Maximum number of C note repeats within window
	int contrary_min = 30; // Minimum percent of contrary motion (little)
	int contrary_min2 = 60; // Minimum percent of contrary motion (medium)
	int cantus_id2 = 0; // Select particular cantus id. Set to -1 to select random
	int accept_cantus_rechoose = 1; // Choose new random cantus after accepting counterpoint
	int pco_apart = 4; // Minimum allowed distance between pco in quarters
	int emulate_sas = 0; // 0 = disable emulator, 1 = Enables SAS algorithm emulator in CA2

	// Counterpoint
	vector<vector<int>> ac; // [v][s] Diatonic
	vector<vector<int>> acc; // [v][s] Chromatic
	vector<vector<int>> acc_old; // [v][s] Chromatic
	vector<vector<int>> apc; // [v][s] Pitch class (diatonic)
	vector<vector<int>> apcc; // [v][s] Pitch class (chromatic)
	vector<vector<int>> aleap; // [v][s] Leaps
	vector<vector<int>> asmooth; // [v][s] Smooth movements
	vector<vector<int>> aslur; // [v][s] Slurs
	vector<int> retrigger; // [s] Equals 1 if note should be retriggered
	int species = 0; // Counterpoint species
	int species_detected = 0; // Counterpoint species detected in CA2
	vector<int> species_pos; // Possible species
	int sus_count = 0; // Number of suspensions detected in ExplodeCP
	int local_flag_position;
	int fixed_ep2 = 0; // For SAS emulation
	// For FLAG2 macro

	// Check data ready
	vector<int> data_ready; // If data is ready to be used
	vector<int> warn_data_ready; // How many warnings of data ready fired
	inline void ClearReady();
	inline void SetReady(int id);
	inline void SetReady(int id, int id2);
	inline void SetReady(int id, int id2, int id3);
	inline void CheckReady(int id);
	inline void CheckReady(int id, int id2);
	inline void CheckReady(int id, int id2, int id3);

	vector<int> data_ready_persist; // If data is ready to be used (not cleared by ClearReady)
	vector<int> warn_data_ready_persist; // How many warnings of data ready fired
	inline void ClearReadyPersist(int id);
	inline void ClearReadyPersist(int id, int id2);
	inline void ClearReadyPersist(int id, int id2, int id3);
	inline void SetReadyPersist(int id);
	inline void SetReadyPersist(int id, int id2);
	inline void SetReadyPersist(int id, int id2, int id3);
	inline void CheckReadyPersist(int id);
	inline void CheckReadyPersist(int id, int id2);
	inline void CheckReadyPersist(int id, int id2, int id3);

	// CP1
	vector<int> ivl; // [s] Diatonic interval between voices
	vector<int> civl; // [s] Chromatic interval between voices
	vector<int> ivlc; // [s] Diatonic interval between voices (class)
	vector<int> civlc; // [s] Chromatic interval between voices (class)
	vector<int> civlc2; // [s] Shows lilypond optimized civlc (0 for unisone, 12 for octave)
	vector<int> tivl; // [s] Type of interval between voices
	vector<int> motion; // [s] Melody motion type
	vector<int> beat; // [ls] Beat type for each fli2: 0 = downbeat, 1 = beat 3
	vector<int> sus; // [ls] Note suspension flag (when above zero, links to first cantus-changing step)
	vector<int> isus; // [ls] Points to sus position or note start if there is not sus
	vector<int> mli; // [ms] Forward links to first steps of each measure
	vector<int> bmli; // [s] Backward links to measures from steps
	vector<int> cfli; // [cfs] Forward links to each cf note
	vector<int> hli; // [hs] Forward links to first notes of each harmony
	vector<int> hli2; // [hs] Forward links to last notes of each harmony
	vector<int> hbcc; // [hs] Bass note of each harmony (chromatic)
	vector<int> hbc; // [hs] Bass note of each harmony (diatonic)
	vector<int> bhli; // [s] Back links to first notes of each harmony
	vector<int> mshb; // [ls] Melody shape types for fli (basic without patterns)
	vector<int> mshf; // [ls] Melody shape types for fli (with fixed patterns)
	vector<int> msh; // [ls] Melody shape types for fli
	vector<int> pat; // [ls] Pattern (cambiata, dnt...) for fli
	vector<int> pat_state; // [ls] Pattern (cambiata, dnt...) for fli state: 0 - not applied, 1 - fixed, 2,3 - variants

	// Harmony notation
	vector <CString> HarmName;
	vector <CString> HarmName_m;
	vector <CString> HarmName_ma;
};
