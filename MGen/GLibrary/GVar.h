#pragma once
#include "GLib.h"

// Maximum size of progress
#define MAX_PROGRESS 1000

// Maximum number of lines to load from saved logs
#define MAX_LOAD_LOG 10

#define MAX_WARN_LOADVECTORS 10
#define MAX_WARN_VALID 10
// Maximum allowed note length. Must be at least two times lower than 65 535 for poff to work
#define MAX_LEN 16384 

class CGVar :
	public CGLib
{
public:
	CGVar();
	~CGVar();

	void InitVectors();
	void AddMelody(int step1, int step2, int v, CString info, CString info2 = "");
	void LoadInstruments(); // Load instruments config
	void LoadInstrument(int i, CString fname);
	void LoadConfig(CString fname, int load_includes = 1);
	void LoadInstrumentLayout();
	void SaveResults(CString dir, CString fname);
	void ExportVectorsCSV(CString dir, CString fname);
	void LoadResults(CString dir, CString fname);
	void LoadResultLogs(CString dir, CString fname);
	void LoadResultMusic(CString dir, CString fname);

	// Warnings
	int warning_loadvectors = 0;
	int warning_valid = 0;

	// Data interface
	CString m_current_config; // Current loading config
	int m_loading = 0; // If we are loading saved results
	int m_algo_id = -1; // Current algorithm id
	CString m_algo_folder; // Current algorithm folder
	CString m_algo_insts; // Instruments of current algorithm from algorithms.txt
	CString m_config;
	CString as_fname; // Directory of autosave document
	CString as_dir; // Directory of autosave document
	CString save_format_version; // Version of save format loaded

	// Configuration parameters
	int t_cnt = 1600; // Timeslot count (eighth notes) to stop generation
	int v_cnt = 1; // Voice count
	int t_allocated = 1600; // Timeslot count to initialize vectors
	int t_send = 60; // Timeslot count to send
	float midifile_in_mul = 1; // Multiply note length with this value when loading
	int shuffle = 0; // If you want to shuffle all canti after generation (can shuffle up to 32000 canti)
	int sleep_ms = 0;
	int adapt_enable = 1;
	int comment_adapt = 1; // If you want to have each adaptation decision commented
	vector <int> show_transpose; // Semitone transpose for showing
	int midifile_export_marks = 0; // Set to export marks (harmony) to midifile lyrics
	int midifile_export_comments = 1; // Set to export comments (violations) to midifile lyrics

  // Main variables
	vector<int> progress; // Show progress
	int progress_size = 0; // Effective progress size to use
	float m_pspeed = 100; // Playback speed in percent
	long long gen_start_time = 0; // Time in milliseconds when generation started
	long long time_stopped = 0; // Time in milliseconds when generation stopped
	int t_generated = 0; // Timeslots generated
	int t_adapted = 0; // Timeslot count adapted
	int t_sent = 0; // Timeslot count sent to mainframe
	int ng_min = 1000; // Minimum generated note
	int ng_max = 0; // Maximum generated note
	int ng_min2 = 1000; // Minimum generated note (with note scan range)
	int ng_max2 = 0; // Maximum generated note (with note scan range)
	vector<int> ngv_min; // Minimum generated note per voice
	vector<int> ngv_max; // Maximum generated note per voice
	float tg_min = 1000; // Minimum generated tempo
	float tg_max = 0; // Maximum generated tempo
	float basic_tempo = 100; // Basic tempo
	int tonic_cur = 0; // Tonic key
	int minor_cur = 0; // Key minor indicator (0 = major, 1 = minor)
	float midifile_out_mul0 = 1; // Multiply note length with this value when saving
	float midifile_out_mul2 = 1; // Multiply note length with this value when saving

	// Output
	vector<float> midifile_out_mul; // [s] Multiply note length with this value when saving
	vector <int> nfreq; // Note frequency
	vector< vector <unsigned char> > pause; // 1 = pause, 0 = note
	vector< vector <unsigned char> > note; // Note values (midi)
	vector< vector <unsigned short> > len; // Note length in timeslots
	vector< vector <unsigned short> > coff; // Offset of current note start backward (0 = first timeslot of note)
	vector< vector <unsigned short> > poff; // Offset of previous note start (backward)
	vector< vector <unsigned short> > noff; // Offset of next note start (forward)
	vector< vector <unsigned char> > tonic; // Key tonic of current note (3 = D#)
	vector< vector <unsigned char> > minor; // Key minor indicator of current note (0 = major, 1 = minor)
	vector< vector <unsigned char> > dyn; // Dynamics (velocity for piano)
	vector< vector <unsigned char> > vel; // Velocity of midi notes
	vector< vector <unsigned char> > vib; // Vibrato intensity
	vector< vector <unsigned char> > vibf; // Vibrato frequency
	vector< vector <unsigned char> > artic; // Velocity of midi notes
	vector< vector <vector<float>> > ngraph; // [s][v][] Graph using chromatic scale
	vector< vector <vector<float>> > graph; // [s][v][] Graph using arbitrary scale
	vector<CString> graph_name; // [] Graph name
	vector<float> graph_max; // [] Maximum graph value
	vector<float> graph_scale; // [] Maximum graph scale (semitones per unit)
	int ngraph_size = 0; // Number of ngraphs
	int graph_size = 0; // Number of graphs
	vector< vector <int> > mel_id; // [i][v] Link from note step to melody id
	vector< vector <unsigned char> > lining; // Visual lining pattern
	vector< vector <CString> > mark; // Mark on note
	vector<CString> mel_info; // Information about melody
	vector<CString> mel_info2; // Information about melody
	vector< vector <DWORD> > mark_color; // Mark color
	vector< vector <unsigned char> > midi_ch; // Midi channel of each note
	vector< vector <short> > midi_delta; // Midi file delta in ms
	vector< DWORD > linecolor; // Shows color of bar line if not zero
	vector< vector <char> > lengroup; // How many notes left until last in lengroup
	vector< vector <CString> > lyrics; // Imported MIDI lyrics
	vector< vector <vector<CString>> > comment; // Comment for note
	vector< vector <vector<DWORD>> > ccolor; // Comment color for note
	vector< vector <CString> > comment2; // Comment for note (shorter)
	vector< vector <char> > nsr1; // Note scan range
	vector< vector <char> > nsr2; // Note scan range
	vector< vector <CString> > adapt_comment; // Adaptation comment for note
	vector< vector <DWORD> > color; // Note color (rgb ignored if all zero; alpha ignored if zero)
	vector<float> tempo; // Tempo
	vector<float> tempo_src; // Source tempo before randomization
	vector<float> stime; // Time of current step start in ms
	vector<float> etime; // Time of current step ending in ms
	vector< vector <float> > dstime; // Delta of current step start in ms for playback
	vector< vector <float> > detime; // Delta of current step ending in ms for playback
	vector<CString> track_name; // Track names from midi file for each voice
	vector<int> track_id; // Track id from midi file for each voice
	vector<int> track_vid; // Voice id inside track from midi file for each voice

	// Instruments
	vector<CString> InstGName; // Instrument groups 
	vector<CString> InstCName; // Instrument configs
	CString instr_layout; // Name of instruments/*.txt file to load instrument layout from
	CString m_config_insts; // String with instrument mapping from config
	vector<int> instr; // Instruments for each voice
	vector<int> instr_channel;
	vector<int> instr_type;
	vector<int> ks1; // First keyswitch pitch
	vector<int> instr_used; // For how many voices this instrument is already used
	vector<int> instr_nmin;
	vector<int> instr_nmax;
	vector<int> instr_tmin;
	vector<int> instr_tmax;
	vector<CString> instr_lib;
	vector<int> instr_poly; // Maximum number of simultaneous voices
	vector<int> splitpo_freq; // Frequency of split portamento in percent
	vector<int> splitpo_mindur; // Minimum legato duration to use split portamento
	vector<int> gliss_mindur; // Minimum legato duration to use gliss
	vector<int> splitpo_mixed_minint;
	vector<int> splitpo_pent_minint;
	vector<int> art_freq;
	vector<int> art_mindur;
	vector<int> art_minint;
	vector<int> art_maxint;
	vector<int> CC_vib; // CC number for vibrato intensity
	vector<int> CC_vibf; // CC number for vibrato speed
	vector<float> vib_bell_top1; // Leftmost - rightmost maximum vibrato intensity in note(percent of note duration)
	vector<float> vib_bell_top2; // Leftmost - rightmost maximum vibrato intensity in note(percent of note duration)
	vector<float> vibf_bell_top1; // Leftmost - rightmost maximum vibrato speed in note(percent of note duration)
	vector<float> vibf_bell_top2; // Leftmost - rightmost maximum vibrato speed in note(percent of note duration)
	vector<float> vib_bell_freq; // Frequency to apply vibrato bell when all conditions met
	vector<float> vib_bell_mindur; // Minimum note duration(ms) that can have a vibrato bell
	vector<float> vib_bell_dur; // Minimum note duration(ms) that can have a highest vibrato bell
	vector<float> vib_bell1; // Maximum vibrato intensity in vibrato bell
	vector<float> vibf_bell1; // Max vibrato frequency in vibrato bell
	vector<float> vib_bell2; // Maximum vibrato intensity in vibrato bell
	vector<float> vibf_bell2; // Max vibrato frequency in vibrato bell
	vector<float> vib_bell_exp; // Exponent to create non - linear bell shape
	vector<float> vibf_bell_exp; // Exponent to create non - linear bell shape
	vector<float> rnd_vib; // Randomize vibrato intensity not greater than this percent
	vector<float> rnd_vibf; // Randomize vibrato speed not greater than this percent
	vector<int> CC_steps;
	vector<int> CC_ma;
	vector<int> CC_dyn;
	vector<int> CC_retrigger;
	vector<int> retrigger_freq;
	vector<int> max_slur_count;
	vector<int> max_slur_interval;
	vector<int> slur_ks;
	vector<vector<int>> legato_ahead;
	vector<vector<int>> ahead_chrom;
	vector<float> legato_ahead_exp; // Exponent to interpolate note movement ahead from note velocity
	vector<float> leg_pdur; // Maximum percent of previous note duration, that legato transition can take
	vector<float> leg_cdur; // Maximum percent of current note duration, that legato transition can take
	vector<float> nonlegato_freq;
	vector<int> nonlegato_minlen;
	vector<int> lengroup2;
	vector<int> lengroup3;
	vector<int> lengroup4;
	vector<int> lengroup_edt1;
	vector<int> lengroup_edt2;
	vector<float> rand_start;
	vector<float> rand_end;
	vector<float> rnd_vel; // Randomize note velocity not greater than this percent
	vector<float> rnd_vel_repeat; // Randomize note velocity not greater than this percent for note retriggers
	vector<float> rnd_dyn; // Randomize step dynamics not greater than this percent
	vector<int> retrigger_min_len; // Minimum next note length in ms to use retrigger
	vector<int> retrigger_rand_end; // Maximum percent of note length to move note end to the left in case of nonlegato
	vector<int> retrigger_rand_max; // Maximum length in ms to move note end to the left in case of nonlegato retrigger
	vector<int> vel_harsh; // Velocity equal or above this value triggers harsh sustain
	vector<int> vel_immediate; // Velocity equal or above this value triggers immediate sustain
	vector<int> vel_normal; // Velocity equal or above this value triggers normal sustain
	vector<int> vel_gliss; // Velocity below this value triggers glissando transition
	vector<int> vel_normal_minlen; // Minimum note length that can have a normal or lower sustain
	vector<int> bell_mindur; // Minimum note duration (ms) that can have a bell
	vector<int> gliss_minlen; // Minimum note length that can have a gliss transition
	vector<int> gliss_freq; // Frequency of gliss articulation in percent
	vector<int> rand_start_max; // Maximum shift in ms
	vector<int> rand_end_max; // Maximum shift in ms
	vector<int> max_ahead_note; // Maximum chromatic interval having ahead property
	vector<float> bell_start_mul; // Multiply dynamics by this parameter at bell start
	vector<float> bell_end_mul; // Multiply dynamics by this parameter at bell end
	vector<float> bell_start_len; // Percent of notelength to use for slope at bell start
	vector<float> bell_end_len; // Percent of notelength to use for slope at bell end
	vector<float> bell_start_vel; // Set belled note velocity to random between these percents of starting dynamics
	vector<float> bell_end_vel; // Set belled note velocity to random between these percents of starting dynamics
	vector<float> rbell_mindur; // Minimum note duration(ms) that can have a reverse bell
	vector<float> rbell_dur; // Minimum note duration(ms) that will have deepest reverse bell
	vector<float> rbell_mul; // Multiply dynamics by this parameter at bell center with mindur
	vector<float> rbell_mul2; // Multiply dynamics by this parameter at bell center with dur
	vector<float> rbell_freq; // Frequency to apply reverse bell when all conditions met
	vector<float> rbell_pos1; // Leftmost - rightmost minimum reverse bell position inside window(percent of window duration)
	vector<float> rbell_pos2; // Leftmost - rightmost minimum reverse bell position inside window(percent of window duration)
	vector<float> end_sfl_dur; // Duration of short fall ending
	vector<float> end_sfl_freq; // Frequency of short fall ending
	vector<float> end_pbd_dur; // Duration of pitch bend down ending
	vector<float> end_pbd_freq; // Frequency of pitch bend down ending
	vector<float> end_vib2_dur; // Duration of vibrato ending2
	vector<float> end_vib2_freq; // Frequency of vibrato ending2
	vector<float> end_vib_dur; // Duration of vibrato ending
	vector<float> end_vib_freq; // Frequency of vibrato ending

	// Global config
	int rnd_tempo = 6; // Randomize tempo not greater than this percent
	int rnd_tempo_step = 1; // Maximum difference in tempo between adjacent steps while randomizing

protected:
	void ValidateVectors(int step1, int step2);
	void ValidateVectors2(int step1, int step2);
	// File operations
	void SaveVector2C(ofstream & fs, vector<vector<unsigned char>>& v2D, int i);
	void SaveVector2S(ofstream & fs, vector<vector<unsigned short>>& v2D, int i);
	void SaveVector2Color(ofstream & fs, vector<vector<DWORD>>& v2D, int i);
	void SaveVector2ST(ofstream & fs, vector<vector<CString>>& v2D, int i);
	void SaveVector(ofstream &fs, vector<float> &v);
	void SaveVector(ofstream &fs, vector<DWORD> &v);
	void LoadVector2C(ifstream & fs, vector<vector<unsigned char>>& v2D, int i);
	void LoadVector2S(ifstream & fs, vector<vector<unsigned short>>& v2D, int i);
	void LoadVector2Color(ifstream & fs, vector<vector<DWORD>>& v2D, int i);
	void LoadVector2ST(ifstream & fs, vector<vector<CString>>& v2D, int i);
	void LoadVector(ifstream & fs, vector<DWORD>& v);
	void LoadVector(ifstream & fs, vector<float>& v);
	void LoadVector(ifstream & fs, vector<unsigned char>& v);
	// Load config
	void LoadConfigFile(CString fname, int load_includes = 1);
	void LoadVarInstr(CString * sName, CString * sValue, char * sSearch, vector<int>& Dest);
	virtual void LoadConfigLine(CString* sN, CString* sV, int idata, float fdata) = 0;
	// Helper functions for child generators
	void CountOff(int step1, int step2);
	void FixLen(int step1, int step2);
	void CountTime(int step1, int step2);
	void CopyVoice(int v1, int v2, int step1, int step2, int interval);
	void UpdateNoteMinMax(int step1, int step2, int final_run = 1);
	void UpdateTempoMinMax(int step1, int step2);
	void AddNote(int pos, int v, char note2, int len2, int dyn2);
	void FillPause(int start, int length, int v);
	void RegisterGraph(CString name, float scale);
	inline void SetProgress(int i, int value);
	inline void ResizeVectors(int size, int vsize = -1);

	DWORD color_noflag; // Color for notes with no flags

	// Warnings
	int warn_progress = 0; // Progress too long
};

void CGVar::SetProgress(int i, int value) {
	if (i > MAX_PROGRESS) {
		if (!warn_progress) {
			WriteLog(5, "Exceeded progress size. Please increase progress size or check algorithm.");
			++warn_progress;
		}
		return;
	}
	progress[i] = value;
}