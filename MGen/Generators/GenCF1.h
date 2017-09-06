#pragma once
#include "../GLibrary/GMidi.h"
#include "../GLibrary/VSet.h"

// Checks if we have leap or melody direction change here: needs to be not first and not last note
#define MELODY_SEPARATION(s, s1) (!s || (leap[s - 1]) || ((c[s] - c[s - 1])*(c[s1] - c[s]) < 0))

// Report violation
#define FLAG(id, i) { if ((skip_flags) && (accept[id] == 0)) goto skip; flags[0] = 0; ++flags[id]; anflags[cpv][i][anflagsc[cpv][i]] = id; ++anflagsc[cpv][i]; }
#define FLAG2(id, i) { if ((skip_flags) && (accept[id] == 0)) return 1; flags[0] = 0; ++flags[id]; anflags[cpv][i][anflagsc[cpv][i]] = id; ++anflagsc[cpv][i]; }
// For harmony
#define FLAG3(id, i) { if (!accept[id]) { if (i == max_p) { last_flag=id; } return 1; } }
// For rhythm
#define FLAG4(id, i) { if ((skip_flags) && (accept[id] == 0)) return 1; flags[0] = 0; ++flags[id]; if (i < fn) { local_flag_position = fn; } else { local_flag_position = i; } anflags[cpv][local_flag_position][anflagsc[cpv][local_flag_position]] = id; ++anflagsc[cpv][local_flag_position]; }

// This value has to be greater than any penalty. May need correction if step_penalty or pitch_penalty changes
#define MAX_PENALTY 10000000.0
#define MAX_RULES 500

#define MAX_RULESETS 100
#define MAX_SEVERITY 101
#define MAX_WIND 500
#define MAX_NOTE 127

const int hvt[] = { 1, 0, 1, 0, 0, 1, 0 };
const int hvd[] = { 0, 0, 1, 0, 1, 0, 1 };
const int hvs[] = { 0, 1, 0, 1, 0, 1, 0 };

const CString MethodNames[] = {
	"window-scan", // 0
	"swa" // 1
};

const CString HarmNames[] = {
	"T", // 0
	"SII", // 1
	"DTIII", // 2
	"S", // 3
	"D", // 4
	"TSVI", // 5
	"DVII", // 6
};

#define mUndefined -1
#define mScan 0
#define mSWA 1

const CString TaskNames[] = {
	"evaluate", // 0
	"correct", // 1
	"generate" // 2
};

#define tGen 0
#define tEval 1
#define tCor 2

// Harmonic functions
#define hUndefined -1
#define hTon 0
#define hSub 1
#define hDom 2

// Interval types
#define iDis 0 // Dissonance
#define iPco 1 // Perfect consonance
#define iIco 2 // Imperfect consonance

// Rule string types
#define rsName 0 // Rule name
#define rsSubName 1 // Subrule name
#define rsComment 2 // Rule comment
#define rsSubComment 3 // Subrule comment

// Convert chromatic to diatonic
#define CC_C(note, tonic, minor) (minor?m_CC_C(note, tonic):maj_CC_C(note, tonic))
#define maj_CC_C(note, tonic) (chrom_to_dia[(note + 12 - tonic) % 12] + ((note + 12 - tonic) / 12) * 7)
#define m_CC_C(note, tonic) (m_chrom_to_dia[(note + 12 - tonic) % 12] + ((note + 12 - tonic) / 12) * 7)

// Convert diatonic to chromatic
#define C_CC(note, tonic, minor) (minor?m_C_CC(note, tonic):maj_C_CC(note, tonic))
#define maj_C_CC(note, tonic) (dia_to_chrom[note % 7] + (note / 7 - 1) * 12 + tonic)
#define m_C_CC(note, tonic) (m_dia_to_chrom[note % 7] + (note / 7 - 1) * 12 + tonic)

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
	int Interval2Chromatic(int iv);
	void ParseRule(int rset, int rid, int type);
	int GetRuleParam(int rset, int rid, int type, int id);
	void ParseRules();
	void SetRuleParams();
	void CheckConfig();
	int SelectRuleSet(int rs);
	void LoadConfigLine(CString * sN, CString * sV, int idata, float fdata);
	void LogCantus(CString st3, int x, vector<int>& c);
	inline int FailNoteRepeat(vector<int> &c, int step1, int step2);
	inline int FailNoteSeq(vector<int>& pc);
	inline int FailLocalRange(vector<int>& cc, int notes, int mrange, int flag);
	inline int FailLocalMacc(int notes, float mrange, int flag);
	inline void GetMelodyInterval(vector<int>& cc, int step1, int step2, int & nmin, int & nmax);
	inline void ClearFlags(int step1, int step2);
	inline void GetPitchClass(vector<int>& c, vector<int>& cc, vector<int>& pc, vector<int>& pcc, int step1, int step2);
	inline int FailHarmStep(int i, const int * hv, int & count, int & wcount, int & last_flag, int & max_p);
	inline int FailGisTrail(vector<int>& pcc);
	inline int FailFisTrail(vector<int>& pcc);
	inline int EvalMelodyHarm(int p, int & last_flag, int & max_p);
	inline int FailMelodyHarm(vector<int>& pc);
	//inline void UpdateNoteHarm(int i);
	//inline int FailMelodyHarmMiss(vector<int>& pc, int i, int harm, int & count, int & wcount);
	//inline int FixNoteHarmRepeat(vector<int>& pc, int i, int harm, int & count, int & wcount);
	void CalcCcIncrement();
	inline void GetChromatic(vector<int>& c, vector<int>& cc, int step1, int step2, int minor_cur);
	inline int FailDiatonic(vector<int>& c, vector<int>& cc, int step1, int step2, int minor_cur);
	inline int FailOutstandingRepeat(vector<int>& c, vector<int>& cc, vector<int>& leap, int scan_len, int rlen, int fid);
	inline int FailLongRepeat(vector<int>& cc, vector<int>& leap, int scan_len, int rlen, int fid);
	inline int FailManyLeaps(vector<int>& c, vector<int>& cc, vector<int>& leap, vector<int>& smooth, vector<int>& slur, int mleaps, int mleaped, int mleapsteps, int flag1, int flag2);
	inline void GetLeapSmooth(vector<int>& c, vector<int>& cc, vector<int>& leap, vector<int>& smooth, vector<int>& slur);
	inline int FailLeapSmooth(vector<int>& c, vector<int>& cc, vector<int>& leap, vector<int>& smooth, vector<int>& slur);
	inline int FailStagnation(vector<int>& cc, vector<int>& nstat, int steps, int notes, int flag);
	inline int FailMultiCulm(vector<int>& cc, vector<int>& slur);
	inline int FailFirstNotes(vector<int>& pc);
	inline int FailLastNotes(vector<int>& pc, vector<int>& pcc);
	inline void CreateLinks(vector<int>& c);
	inline void CountFillInit(vector<int>& c, int tail_len, int pre, int & t1, int & t2, int & fill_finish);
	inline void CountFill(vector<int>& c, int tail_len, vector<int>& nstat2, vector<int>& nstat3, int & skips, int & fill_to, int pre, int & fill_to_pre, int & fill_from_pre, int & fill_from, int & deviates, int & dev_count, int leap_prev, int & fill_finish);
	inline void CountFillSkips(int leap_prev, int & skips, int t1, int t2);
	inline void CountFillLimits(vector<int>& c, int pre, int t1, int t2, int & fill_to, int & fill_to_pre, int & fill_from_pre, int & fill_from);
	inline void FailLeapInit(vector<int>& c, int & late_leap, int & presecond, int & leap_next, int & leap_prev, int & arpeg, int & overflow, vector<int>& leap);
	inline int FailLeapMulti(int leap_next, int & arpeg, int & overflow, int & child_leap, vector<int>& c, vector<int>& leap);
	inline int FailLeap(vector<int>& c, vector<int>& leap, vector<int>& smooth, vector<int>& nstat2, vector<int>& nstat3);
	inline int FailLeapFill(vector<int>& c, int late_leap, int leap_prev, int child_leap);
	inline int FailLeapMDC(vector<int>& leap, vector<int>& c);
	inline int FailTritone(int ta, int t1, int t2, int tb, vector<int>& c, vector<int>& cc, vector<int>& pc, vector<int>& pcc);
	inline int FailTonic(vector<int> &cc, vector<int>& pc);
	inline int FailIntervals(vector<int>& c, vector<int>& cc, vector<int>& pc, vector<int>& pcc);
	inline int FailGlobalFill(vector<int>& c, vector<int>& nstat2);
	void ScanInit();
	void ScanCantusInit();
	int GetMinSmap();
	int GetMaxSmap();
	void GetRealRange(vector<int>& c, vector<int>& cc);
	void GetSourceRange(vector<int>& cc);
	void ApplySourceRange();
	void SingleCantusInit();
	void MakeNewCantus(vector<int>& c, vector<int>& cc);
	void MultiCantusInit(vector<int>& c, vector<int>& cc);
	int FailWindowsLimit();
	inline void CalcFlagStat();
	inline int FailFlagBlock();
	inline int FailAccept();
	inline void NextWindow();
	inline void CalcRpenalty(vector<int>& cc);
	inline void ScanLeft(vector<int> &cc, int &finished);
	inline void BackWindow(vector<int>& cc);
	inline int NextSWA(vector<int>& cc, vector<int>& cc_old);
	inline void SaveBestRejected(vector<int>& cc);
	inline int FailMinor(vector<int>& pcc, vector<int>& cc);
	inline void ShowScanStatus(vector<int>& cc);
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
	void CalcDpenalty();
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
	void SendLyrics(int pos, int v, int av, int x);
	void SendComment(int pos, int v, int av, int x, int i);
	void TransposeCantusBack();
	void SendNotes(int pos, int i, int v, int av, int x, vector<int>& cc);
	void MakeBellDyn(int v, int step1, int step2, int dyn1, int dyn2, int dyn_rand);
	int SendPause(int pos, int v);
	int SendCantus();
	int InitGen();
	int InitCantus();
	void TestDiatonic();
	void RandomSWA();
	void SWA(int i, int dp);
	void FillCantus(vector<int>& c, int step1, int step2, int value);
	void FillCantus(vector<int>& c, int step1, int step2, vector<int> &value);
	void FillCantus(vector<int>& c, int step1, int step2, vector<vector<int>> &value);
	void RandCantus(vector<int>& c, vector<int>& cc, int step1, int step2);
	void CalculateCcRand(int step1, int step2);
	void FillCantusMap(vector<int>& c, vector<int>& smap, int step1, int step2, vector<vector<int>>& value);
	void FillCantusMap(vector<int>& c, vector<int>& smap, int step1, int step2, vector<int>& value);
	void FillCantusMap(vector<int>& c, vector<int>& smap, int step1, int step2, int value);
	
	// Rules
	vector <int> accept; // Each 1 allows showing canti with specific properties
	vector <int> severity; // Get severity by flag id
	vector <vector<int>> accepts; // Each 1 allows showing canti with specific properties
	int rule_set = 0; // id of current rule set
	int cf_rule_set = 0; // id of current rule set for cantus
	int cp_rule_set = 0; // id of current rule set for cpoint
	vector<vector <CString>> RuleName; // Names of all rules
	vector<vector <CString>> SubRuleName; // Names of all rules
	vector<vector<vector<vector<int>>>> RuleParam; // Parsed rule parameters
	vector <CString> RuleComment; // Comments for flag groups
	vector<vector <CString>> SubRuleComment; // Comments for flags
	int max_flags = 82; // Maximum number of rules

	// Parameters
	int cantus_high = 0; // Set to 1 to consider cantus to be higher voice
	int method = mUndefined; // Which generation / analysis method to use
	int min_interval = 7; // Minimum chromatic interval in cantus (12 = octave)
	int max_interval = 12; // Maximum chromatic interval in cantus (12 = octave)
	int min_intervald = 4; // Minimum diatonic interval in cantus (7 = octave)
	int max_intervald = 7; // Maximum diatonic interval in cantus (7 = octave)
	int c_len = 9; // Number of measures in each cantus. Usually 9 to 11
	int s_len = 4; // Maximum number of measures to full scan
	int first_note = 72; // Starting note of each cantus
	int first_note0 = 0; // Saved first note during generation
	int last_note = 72; // Ending note of each cantus
	int fill_steps_mul = 2; // Multiply number of notes between leap notes to get steps for filling
	int max_repeat_mul = 2; // Allow repeat of X notes after at least X*max_repeat_mul steps if beats are different
	int max_smooth_direct = 5; // Maximum linear movement in one direction allowed (in steps)
	int max_smooth = 7; // Maximum linear movement allowed (in steps)
	int max_leaps = 2; // Maximum allowed leaps during max_leap_steps
	int max_leaped = 3; // Maximum allowed leaped-over-notes during max_leap_steps
	int max_leap_steps = 7;
	int max_leaps2 = 2; // Maximum allowed leaps during max_leap_steps2
	int max_leaped2 = 3; // Maximum allowed leaped-over-notes during max_leap_steps
	int max_leap_steps2 = 7;
	int cse_leaps = 2; // Maximum allowed consecutive leaps for Consecutive leaps
	int cse_leaps2 = 3; // Maximum allowed consecutive leaps for Consecutive leaps+
	int hsp_leap = 5; // Maximum allowed leap before bad harmonic sequence
	int early_culm = 3; // Early culmination step
	int late_culm = 3; // Late culmination step

	int show_note_scan_range = 1; // Internal variable - allows to disable showing note scan range
	int early_culm2 = 4; // Early culmination step (second rule)
	int early_culm3 = 50; // Early culmination step percent
	int stag_notes = 2; // Maximum allowed stag_notes (same notes) during stag_note_steps
	int stag_note_steps = 7;
	int stag_notes2 = 3; // Maximum allowed stag_notes2 (same notes) during stag_note_steps2
	int stag_note_steps2 = 10;
	int tonic_cur = 0; // Tonic key
	int minor_cur = 0; // Key minor indicator (0 = major, 1 = minor)
	int random_key = 0; // Allow CF1 to select random key and CA1 to select any of possible keys regardless of last note
	int min_tempo = 110;
	int max_tempo = 120;
	int rpenalty_accepted = 0; // Maximum accepted rule penalty for RandomSWA
	int first_steps_tonic = 3; // Number of first steps, which must contain tonic note
	int notes_lrange = 5; // Maximum number of consecutive notes having low range
	int min_lrange = 5; // Minimum allowed local range of notes_lrange consecutive notes
	int notes_lrange2 = 5; // Maximum number of consecutive notes having low range
	int min_lrange2 = 5; // Minimum allowed local range of notes_lrange consecutive notes
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
	int c4p_last_leaps = 3; // Last leaps that can be c4p compensated
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
	int tonic_leap = 4; // Maximum allowed leap to tonic (chromatic)
	int tonic_leap_weight = 2; // Weight of leap to tonic
	int thirds_ignored = 1; // Number of thirds ignored for consecutive leaps rule
	int fis_gis_max = 3; // Maximum allowed distance between F# and G#
	int dev_late2 = 3; // Maximum note count to consider non-late leap compensation deviation to 2nd
	int dev_late3 = 3; // Maximum note count to consider non-late leap compensation deviation to 3rd
	int late_require = 0; // Allow not-last scan window to have no needed tags, but no blocked tags 
	int approx_steps = 4; // Maximum number of steps to approximate corrections in one iteration
	vector <vector <int>> hv; //  Variants of note harmonic meaning
	vector <vector <int>> hsp; // Harmonic sequence penalty
	vector <int> fli; // [ls] Forward links to start of each non-slurred note
	vector <int> fli2; // [ls] Forward links to end of each non-slurred note
	vector <int> llen; // [ls] Length of each linked note
	vector <int> bli; // [s] Back links from each step to fli2
	vector <int> uli; // [us] Forward links to start of each unique note column
	int minl = 0, maxl = 0;
	int fli_size; // Size of filled fli2 vector
	// Random SWA
	//int fullscan_max = 7; // Maximum steps length to full scan. If melody is longer, use SWA
	int approximations = 30; // Maximum number of approximations to run if penalty decreases
	int swa_steps = 6; // Size of Sliding Window Approximation algorithm window in steps
	float step_penalty = 3; // Penalty for adding one more changing step while correcting cantus
	float pitch_penalty = 1; // Penalty for changing note one more diatonic step while correcting cantus
	int optimize_dpenalty = 1; // Saves only melodies closest to source melody. Decreases memory usage. Resetting allows for more close results when corrections>1
	int transpose_back = 0; // Set to 1 to transpose generated melody closer to initial first note. Can be set to 1 only for CF1 generation algorithms

	// CA1
	int correct_range = 4; // Maximum interval allowed between each source and corrected note
	int correct_inrange = 0; // Limit allowed range of corrected melody to range of source melody
	int animate = 100; // Draw animation of preliminary result every X ms (0 to show each change, high to disable animation)
	int animate_delay = 0; // Delay in ms after each animation step

	// CA1 local variables
	PmTimestamp animate_time; // Last animate timestamp
	int acycle = 0; // Animation time divided by animate (ms)
									
	// Master parameters
	vector <int> *scantus; // Source cantus for processing
	int task; // What task to accomplish using the method
	int svoice; // Voice to send cantus to

	// CA1
	CString midi_file;

  // Local
	int cpv; // Current counterpoint voice
	int cfv; // Current cantus voice
	int av_cnt = 1; // Number of voices in counterpoint
	int seed_cycle, reseed_count;
	long cantus_ignored = 0; // How many canti ignored and not sent
	long cantus_sent = 0; // How many cantus have been sent
	vector<DWORD>  flag_color; // Flag colors
	int step = 0; // Global step
	long long accepted = 0; // Number of accepted canti
	int flags_need2 = 0; // Number of second level flags set
	vector<int> m_c; // Cantus diatonic
	vector<int> m_cc; // Cantus chromatic
	vector<int> m_pc; // pitch class (diatonic)
	vector<int> m_pcc; // pitch class (chromatic)
	vector<int> m_leap;
	vector<int> m_smooth;
	vector<int> m_slur;
	vector<float> macc; // CC moving average
	vector<float> macc2; // CC moving average smoothed
	vector<float> decc; // CC deviation
	vector<float> decc2; // CC deviation smoothed
	vector<float> fpenalty; // Additional penalty for flags
	vector<int>  flags; // Flags for whole cantus
	vector<vector<vector<int>>> anflags; // Note flags
	vector<vector<int>> anflagsc; // Note flags count
	vector<int> br_cc; // Cantus chromatic (best rejected)
	vector<int>  br_f; // Flags for whole cantus (best rejected)
	vector<long>  ssf; // SWA stuck flags
	vector<int>  best_flags; // best flags of saved cantus for swa
	vector<vector<int>> br_nf; // Note flags (best rejected)
	vector<int> br_nfc; // Note flags count (best rejected)
	float rpenalty_cur = 0; // Rules penalty
	float rpenalty_min; // Minimum rules penalty for this scan
	vector <float> rpenalty; // Penalty in terms of sum of flag severity
	float dpenalty_cur = 0; // Distance from source penalty
	float dpenalty_min; // Minimum distance penalty for this scan
	vector <float> dpenalty; // Penalty in terms of distance from source
	vector <int> cc_len; // Length of each cantus note
	vector <float> cc_tempo; // Tempo of each cantus note
	int real_len; // Total length of cantus in steps
	int skip_flags, clear_flags;
	int sp1, sp2, ep1, ep2, p, pp;
	long long accepted2 = 0, accepted3 = 0;
	int first_note_dia, first_note_oct;
	int wid; // Window id
	vector<int> cc_old; // Cantus chromatic saved for SWA
	vector<int> wpos1;
	vector<int> wpos2;
	vector <int> smap; // Map of links from matrix local IDs to cantus step IDs
	vector<int> min_c;
	vector<int> max_c;
	vector<int> min_cc; // Current range
	vector<int> max_cc;
	vector<int> min_cc0; // Source range for RSWA
	vector<int> max_cc0;
	int minc, maxc; // Real possible limits
	vector<vector<vector<long>>> fblock; // number of canti rejected with foreign flags
	vector<long long> fstat; // number of canti with each flag
	vector<vector<long long>> fcor; // Flags correlation matrix
	vector<long long> accepted4; // number of accepted canti per window
	vector<long long> accepted5; // number of canti with neede flags per window
	vector<long long> wscans; // number of full scans per window
	int wcount = 1; // Number of windows created
	long long cycle = 0; // Cycle number of full scan
	PmTimestamp accept_time; // Last accepted timestamp
	int rcycle = 0; // Rejected time divided by best_rejected (ms)
	int nmin, nmax, nmind, nmaxd;
	int src_nmin = 0, src_nmax = 1000; // Source range (chromatic)
	int cc_incr[MAX_NOTE]; // cc increments for each step
	vector<int> test_cc;
	vector<int> nstat;
	vector<int> nstat2;
	vector<int> nstat3;
	vector<vector<int>> hm; // Available harmonic meanings for each note
	vector<vector<int>> hm2; // Required harmonic meanings for each note
	vector <int> chm, chmp; // Current harmonic meaning and its position in hm
	float hdif;
	int cantus_id = 0;
	CString key_eval; // Results of key evaluation
	int culm_step; // Position of culmination after FailMultiCulm
	int cf_culm = 0; // Position of cantus firmus culmination
	int fn = 0; // First note of analyzed melody
	int error = 0;
	vector<float> maw; // Moving average weight

	// Random s_len=1 scan
	vector<int> cc_id; // Current successive identifier of chromatic step
	vector<vector<int>> cc_rand; // Randomized chromatic steps

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
	vector <int> tc; // Tail diatonic notes

	// Local link steps
	int ls; // Link step inside fli2
	int s; // Current step
	int s1, s2; // +1, +2 steps
	int s_1, s_2; // -1, -2 steps

	// Local SWA
	vector <long> cids;

	// CA1
	vector<vector<int>> enflags; // Expected note flags

	// Cantus correcting
	vector <int> smatrix; // Vector of links to steps that were selected for recalculation
	int smatrixc = 0; // Number of steps marked in smatrix
	vector<vector<int>> clib; // Library of cantus
	VSet<int> clib_vs; // Unique clib set

	// CP1 parameters
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
	int c_repeats = 1; // Maximum number of C note repeats within window
	int ca_repeats = 1; // Maximum number of accented C note repeats within window
	int c_window = 10; // Maximum number of C note repeats within window
	int ca_window = 10; // Maximum number of C note repeats within window
	int contrary_min = 30; // Minimum percent of contrary motion (little)
	int contrary_min2 = 60; // Minimum percent of contrary motion (medium)
	int cantus_id2 = 0; // Select particular cantus id. Set to -1 to select random
	int accept_cantus_rechoose = 1; // Choose new random cantus after accepting counterpoint
	int pco_apart = 4; // Minimum allowed distance between pco in quarters

	// Counterpoint
	vector<vector<int>> ac; // Diatonic
	vector<vector<int>> acc; // Chromatic
	vector<vector<int>> acc_old; // Chromatic
	vector<vector<int>> apc; // Pitch class (diatonic)
	vector<vector<int>> apcc; // Pitch class (chromatic)
	vector<vector<int>> aleap;
	vector<vector<int>> asmooth;
	vector<vector<int>> aslur;
	int species = 0; // Counterpoint species
	int species_detected = 0; // Counterpoint species detected in CA2
	int sus_count = 0; // Number of suspensions detected in ExplodeCP
	int local_flag_position; // For FLAG4 macro
	void CreateULinks();
};
