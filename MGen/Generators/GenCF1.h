#pragma once
#include "../GLibrary/GMidi.h"
#include "../GLibrary/VSet.h"

// Checks if we have leap or melody direction change here: needs to be not first and not last note
#define MELODY_SEPARATION(i) ((leap[i - 1]) || i >= ep2-1 || ((c[i] - c[i - 1])*(c[i + 1] - c[i]) < 0))

// Report violation
#define FLAG(id, i) { if ((skip_flags) && (accept[id] == 0)) goto skip; if (accept[id] > -1) { flags[0] = 0; flags[id] = 1; anflags[av][i][anflagsc[av][i]] = id; ++anflagsc[av][i]; } }
#define FLAG2(id, i) { if ((skip_flags) && (accept[id] == 0)) return 1; if (accept[id] > -1) { flags[0] = 0; flags[id] = 1; anflags[av][i][anflagsc[av][i]] = id; ++anflagsc[av][i]; } }

// This value has to be greater than any penalty. May need correction if step_penalty or pitch_penalty changes
#define MAX_PENALTY 10000000.0
#define MAX_FLAGS 200

#define MAX_RULESETS 80
#define MAX_SEVERITY 101
#define MAX_WIND 500
#define MAX_NOTE 127

const CString MethodNames[] = {
	"window-scan", // 0
	"swa" // 1
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
	void LoadHarmVar(CString * sN, CString * sV);
	void LoadRules(CString fname);
	void SelectRuleSet(int rs);
	void LoadConfigLine(CString * sN, CString * sV, int idata, float fdata);
	void LogCantus(vector<int>& c);
	inline int FailNoteRepeat(vector<int> &c, int step1, int step2);
	inline int FailNoteSeq(vector<int>& pc, int step1, int step2);
	inline void GetMelodyInterval(vector<int>& cc, int step1, int step2);
	inline void ClearFlags(int step1, int step2);
	inline void GetPitchClass(vector<int>& c, vector<int>& cc, vector<int>& pc, vector<int>& pcc, int step1, int step2);
	inline void UpdateNoteHarm(int i);
	inline int FailMelodyHarm(vector<int>& pc, int ep1, int ep2);
	inline int FailMelodyHarmMiss(vector<int>& pc, int i, int harm, int & count, int & wcount);
	inline int FixNoteHarmRepeat(vector<int>& pc, int i, int harm, int & count, int & wcount);
	void CalcCcIncrement();
	inline void GetChromatic(vector<int>& c, vector<int>& cc, int step1, int step2, int minor_cur);
	inline int FailDiatonic(vector<int>& c, vector<int>& cc, int step1, int step2, int minor_cur);
	inline void AlterMinor(int ep2, vector<int>& cc);
	inline int FailOutstandingRepeat(vector<int>& c, vector<int>& cc, vector<int>& leap, int ep2, int scan_len, int rlen, int fid);
	inline int FailLongRepeat(vector<int>& cc, vector<int>& leap, int ep2, int scan_len, int rlen, int fid);
	inline int FailLeapSmooth(vector<int>& c, int ep2, vector<int>& leap, vector<int>& smooth);
	inline int FailStagnation(vector<int>& cc, vector<int>& nstat, int ep2);
	inline int FailMultiCulm(vector<int>& cc, int ep2);
	inline int FailFirstNotes(vector<int>& pc, int ep2);
	inline int FailLastNotes(vector<int>& pc, int ep2);
	inline void CountFill(vector<int>& c, int i, int pos1, int pos2, int leap_size, int leap_start, vector<int>& nstat2, vector<int>& nstat3, int & skips, int & skips2, int & ffinished, int pre);
	inline int FailLeap(vector<int>& c, int ep2, vector<int>& leap, vector<int>& smooth, vector<int>& nstat2, vector<int>& nstat3);
	inline int FailIntervals(int ep2, vector<int>& c, vector<int>& cc, vector<int>& pc);
	inline void GlobalFill(vector<int>& c, int ep2, vector<int>& nstat2);
	void ScanCantusInit();
	int GetMinSmap();
	int GetMaxSmap();
	void GetRealRange(vector<int>& c, vector<int>& cc);
	void GetSourceRange();
	void ApplySourceRange();
	void SingleCantusInit();
	void MakeNewCantus(vector<int>& c, vector<int>& cc);
	void MultiCantusInit(vector<int>& c, vector<int>& cc);
	int FailWindowsLimit();
	inline void CalcFlagStat();
	inline int FailFlagBlock();
	inline int FailAccept();
	inline void NextWindow();
	inline void CalcRpenalty();
	inline void ScanLeft(vector<int> &cc, int &finished);
	inline void BackWindow(vector<int>& cc);
	inline int NextSWA();
	inline void SaveBestRejected();
	inline int FailMinor();
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
	int SendCantus();
	void InitCantus();
	void TestDiatonic();
	void RandomSWA();
	void SWA(int i, int dp);
	void FillCantus(vector<int>& c, int step1, int step2, int value);
	void FillCantus(vector<int>& c, int step1, int step2, vector<int> value);
	void RandCantus(vector<int>& c, vector<int>& cc, int step1, int step2);
	void FillCantusMap(vector<int>& c, vector<int>& smap, int step1, int step2, vector<int>& value);
	
	// Rules
	vector <int> accept; // Each 1 allows showing canti with specific properties
	vector <int> severity; // Get severity by flag id
	vector <vector<int>> accepts; // Each 1 allows showing canti with specific properties
	int rule_set = 0; // id of current rule set
	vector <CString> FlagName; // Names of all rules
	int max_flags = 82; // Maximum number of rules

	// Parameters
	int method = mUndefined; // Which generation / analysis method to use
	int min_interval = 7; // Minimum chromatic interval in cantus (12 = octave)
	int max_interval = 12; // Maximum chromatic interval in cantus (12 = octave)
	int min_intervald = 4; // Minimum diatonic interval in cantus (7 = octave)
	int max_intervald = 7; // Maximum diatonic interval in cantus (7 = octave)
	int c_len = 9; // Number of measures in each cantus. Usually 9 to 11
	int s_len = 4; // Maximum number of measures to full scan
	int first_note = 72; // Starting note of each cantus
	int last_note = 72; // Ending note of each cantus
	int fill_steps_mul = 2; // Multiply number of notes between leap notes to get steps for filling
	int max_repeat_mul = 2; // Allow repeat of X notes after at least X*max_repeat_mul steps if beats are different
	int max_smooth_direct = 5; // Maximum linear movement in one direction allowed (in steps)
	int max_smooth = 7; // Maximum linear movement allowed (in steps)
	int max_leaps = 2; // Maximum allowed max_leaps during max_leap_steps
	int max_leaps2 = 3; // Maximum allowed max_leaps2 during max_leap_steps for Many leaps+
	int cse_leaps = 2; // Maximum allowed consecutive leaps for Consecutive leaps
	int cse_leaps2 = 3; // Maximum allowed consecutive leaps for Consecutive leaps+
	int max_leap_steps = 7;
	int stag_notes = 2; // Maximum allowed stag_notes (same notes) during stag_note_steps
	int stag_note_steps = 7;
	int tonic_cur = 0; // Tonic key
	int minor_cur = 0; // Key minor indicator (0 = major, 1 = minor)
	int random_key = 0; // Allow CF1 to select random key and CA1 to select any of possible keys regardless of last note
	int min_tempo = 110;
	int max_tempo = 120;
	int first_steps_tonic = 3; // Number of first steps, which must contain tonic note
	int correct_range = 4; // Maximum interval allowed between each source and corrected note
	int correct_inrange = 0; // Limit allowed range of corrected melody to range of source melody
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
	int show_severity = 0; // =1 to show severity in square brackets in comments to notes (also when exporting to MIDI file)
	int repeat_steps2 = 8; // Prohibit repeating of 2 notes closer than repeat_steps between first notes (if beats are same)
	int repeat_steps3 = 8; // Prohibit repeating of 3 notes closer than repeat_steps between first notes (if beats are same)
	int repeat_steps5 = 15; // Prohibit repeating of 5 notes closer than repeat_steps between first notes
	int repeat_steps7 = 100; // Prohibit repeating of 7 notes closer than repeat_steps between first notes
	int late_require = 0; // Allow not-last scan window to have no needed tags, but no blocked tags 
	int approx_steps = 4; // Maximum number of steps to approximate corrections in one iteration
	vector <int> hvd, hvs, hvt; //  Variants and constant harmonic meaning
	vector <int> hconst; // Absolute constant harmonic meaning by note
	// Random SWA
	//int fullscan_max = 7; // Maximum steps length to full scan. If melody is longer, use SWA
	int approximations = 30; // Maximum number of approximations to run if penalty decreases
	int swa_steps = 6; // Size of Sliding Window Approximation algorithm window in steps
	float step_penalty = 3; // Penalty for adding one more changing step while correcting cantus
	float pitch_penalty = 1; // Penalty for changing note one more diatonic step while correcting cantus
	int optimize_dpenalty = 1; // Saves only melodies closest to source melody. Decreases memory usage. Resetting allows for more close results when corrections>1

	// Master parameters
	vector <int> *scantus; // Source cantus for processing
	int task; // What task to accomplish using the method
	int svoice; // Voice to send cantus to

  // Local
	int av; // Current counterpoint voice
	int av_cnt = 1; // Number of voices in counterpoint
	int seed_cycle, reseed_count;
	long cantus_ignored = 0; // How many canti ignored and not sent
	long cantus_sent = 0; // How many cantus have been sent
	vector<Color>  flag_color; // Flag colors
	int step = 0; // Global step
	long long accepted = 0; // Number of accepted canti
	int flags_need2 = 0; // Number of second level flags set
	vector<int> c; // Cantus diatonic
	vector<int> cc; // Cantus chromatic
	vector<int> pc; // pitch class (diatonic)
	vector<int> pcc; // pitch class (chromatic)
	vector<int> leap;
	vector<int> smooth;
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
	vector<int> min_cc;
	vector<int> max_cc;
	int minc, maxc; // Real possible limits
	vector<vector<vector<long>>> fblock; // number of canti rejected with foreign flags
	vector<long long> fstat; // number of canti with each flag
	vector<vector<long long>> fcor; // Flags correlation matrix
	vector<long long> accepted4; // number of accepted canti per window
	vector<long long> accepted5; // number of canti with neede flags per window
	vector<long long> wscans; // number of full scans per window
	int wcount = 1; // Number of windows created
	long long cycle = 0; // Cycle number of full scan
	milliseconds accept_time; // Last accepted timestamp
	int rcycle = 0; // Rejected time divided by best_rejected (ms)
	int nmin, nmax, nmind, nmaxd;
	int src_nminc = 0, src_nmaxc = 0; // Source range (chromatic)
	int cc_incr[MAX_NOTE]; // cc increments for each step
	vector<int> test_cc;
	vector<int> nstat;
	vector<int> nstat2;
	vector<int> nstat3;
	vector<vector<int>> hm; // Available harmonic meanings for each note
	vector<vector<int>> hm2; // Approved harmonic meanings for each note
	float hdif;
	int cantus_id = 0;
	CString key_eval; // Results of key evaluation

	// Local SWA
	vector <long> cids;

	// Cantus correcting
	vector <int> smatrix; // Vector of links to steps that were selected for recalculation
	int smatrixc = 0; // Number of steps marked in smatrix
	vector<vector<int>> clib; // Library of cantus
	VSet<int> clib_vs; // Unique clib set
};
