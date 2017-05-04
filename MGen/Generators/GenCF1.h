#pragma once
#include "../GLibrary/GMidi.h"

#define MAX_FLAGS 42
#define MAX_WIND 50
#define MAX_NOTE 127

class CGenCF1 :
	public CGMidi
{
public:
	CGenCF1();
	~CGenCF1();
	void Generate() override;
protected:
	void LoadConfigLine(CString * sN, CString * sV, int idata, float fdata);
	void LogCantus(vector<int>& c);
	inline int FailNoteRepeat(vector<int> &c, int step1, int step2);
	inline void GetMelodyInterval(vector<int> &c, int step1, int step2, int &nmin, int &nmax);
	inline void ClearFlags(vector<int>& flags, vector<int>& nflagsc, int step1, int step2);
	inline int FailMelodyInterval(int nmin, int nmax, vector<int>& flags, vector<vector<int>>& nflags, vector<int>& nflagsc);
	inline void GetPitchClass(vector<int>& c, vector<int>& pc, int step1, int step2);
	inline int FailLastNotes(vector<int>& pc, int ep2, int c_len, vector<int>& flags, vector<vector<int>>& nflags, vector<int>& nflagsc);
	inline int FailMelodyHarmSeq(vector<int>& pc, int ep1, int ep2, vector<int>& flags, vector<vector<int>>& nflags, vector<int>& nflagsc);
	inline void GetChromatic(vector<int>& c, vector<int>& cc, int step1, int step2);
	inline int FailOutstandingLeap(vector<int>& c, vector<int>& leap, int ep2, vector<int>& flags, vector<vector<int>>& nflags, vector<int>& nflagsc);
	inline int FailManyLeaps(int max_leap_sum, int leap_sum_i, vector<int>& flags, vector<vector<int>>& nflags, vector<int>& nflagsc);
	inline int FailStagnation(vector<int>& c, vector<int>& nstat, int nmin, int nmax, int ep2, vector<int>& flags, vector<vector<int>>& nflags, vector<int>& nflagsc);
	inline int FailMultiCulm(vector<int>& c, int ep2, int nmax, vector<int>& flags, vector<vector<int>>& nflags, vector<int>& nflagsc);
	inline int FailFirstNotes(vector<int>& pc, int ep2, vector<int>& flags, vector<vector<int>>& nflags, vector<int>& nflagsc);
	void ScanCantus(vector<int>* pcantus, int use_matrix, int v);
	void SaveCantus();
	void SendCantus(int v, vector<int>* pcantus);
	void InitCantus();
	void FillCantus(vector<int>& c, int step1, int step2, int value);
	void FillCantusMap(vector<int>& c, vector<int>& smap, int step1, int step2, vector<int>& value);

	// Parameters
	int min_interval = 1; // Minimum diatonic interval in cantus (7 = octave)
	int max_interval = 5; // Maximum diatonic interval in cantus (7 = octave)
	int c_len = 9; // Number of measures in each cantus. Usually 9 to 11
	int s_len = 4; // Maximum number of measures to full scan
	int first_note = 72; // Starting note of each cantus
	int last_note = 72; // Ending note of each cantus
	vector <int> accept; // Each 1 allows showing canti with specific properties
	int fill_steps_mul = 2; // Multiply number of notes between leap notes to get steps for filling
	int max_repeat_mul = 2; // Allow repeat of X notes after at least X*max_repeat_mul steps if beats are different
	int max_smooth_direct = 5; // Maximum linear movement in one direction allowed (in steps)
	int max_smooth = 7; // Maximum linear movement allowed (in steps)
	int max_leaps = 2; // Maximum allowed max_leaps during max_leap_steps
	int max_leaps2 = 3; // Maximum allowed max_leaps2 during max_leap_steps for Many leaps+
	int max_leap_steps = 7;
	int stag_notes = 2; // Maximum allowed stag_notes (same notes) during stag_note_steps
	int stag_note_steps = 7;
	int tonic_cur = 0; // Tonic key
	int random_key = 0; // Allow CF1 to select random key and CA1 to select any of possible keys regardless of last note
	int min_tempo = 110;
	int max_tempo = 120;
	int first_steps_tonic = 3; // Number of first steps, which must contain tonic note
	int correct_range = 4; // Maximum interval allowed between each source and corrected note
	float random_choose = 100; // Percent of accepted canti to show and play
	int random_seed = 0; // Seed melody with random numbers. This ensures giving different results if generation is very slow.
	int calculate_correlation = 0; // Enables correlation calculation algorithm. Slows down generation. Outputs to cf1-cor.csv
	int calculate_blocking = 0; // Enables blocking flags calculation algorithm. Slows down generation.
	int calculate_stat = 0; // Enables flag statistics calculation algorithm. Slows down generation.
	int show_severity = 0; // =1 to show severity in square brackets in comments to notes (also when exporting to MIDI file)
	int repeat_steps = 8; // Prohibit repeating of 3 notes closer than repeat_steps between first notes(if beats are same)
	int late_require = 0; // Allow not-last scan window to have no needed tags, but no blocked tags 
	int approx_steps = 4; // Maximum number of steps to approximate corrections in one iteration

  // Local
	int sev_to_flag[MAX_FLAGS]; // Get flag ID by severity
	vector<int>  flag_to_sev; // Get severity by flag id
	vector<Color>  flag_color; // Flag colors
	int step = 0; // Global step
	long long accepted = 0; // Number of accepted canti
	int flags_need2 = 0; // Number of second level flags set
	vector<int> c; // Cantus diatonic
	vector<int> cc; // Cantus chromatic
	vector<vector<int>> nflags; // Note flags
	vector<int> nflagsc; // Note flags count
	float rpenalty_cur = 0; // Rules penalty
	float rpenalty_min; // Minimum rules penalty for this scan
	vector <float> rpenalty; // Penalty in terms of sum of flag severity
	vector <int> cc_len; // Length of each cantus step
	vector <float> cc_tempo; // Tempo of each cantus step
	int real_len; // Total length of cantus in steps
	int skip_flags;

	// Load severity
	int cur_severity = 0; // Current severity loaded from configuration file

	// Cantus correcting
	vector <int> smatrix; // Vector of links to steps that were selected for recalculation
	int smatrixc = 0; // Number of steps marked in smatrix
	vector<vector<int>> clib; // Library of cantus
};
