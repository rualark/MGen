#pragma once
#include "GLib.h"

#define MAX_WARN_LOADVECTORS 10
#define MAX_WARN_VALID 10
// Maximum allowed note length. Must be at least two times lower than 65 535 for poff to work
#define MAX_LEN 1024 

class CGVar :
	public CGLib
{
public:
	CGVar();
	~CGVar();

	void InitVectors();
	void LoadInstruments(); // Load instruments config
	void LoadConfig(CString fname);
	void SaveResults(CString dir, CString fname);
	void ExportVectorsCSV(CString dir, CString fname);
	void LoadResults(CString dir, CString fname);

	void LoadResultMusic(CString dir, CString fname);

	// Warnings
	int warning_loadvectors = 0;
	int warning_valid = 0;

	// Data interface
	int m_algo_id = -1; // Current algorithm id
	CString m_algo_insts; // Instruments of current algorithm from algorithms.txt
	CString m_config;
	int sleep_ms = 0;
	CString save_format_version; // Version of save format loaded

  // Main constants
	float m_pspeed = 100; // Playback speed in percent
	int v_cnt=1; // Voice count
	int32_t time_started; // Time in milliseconds when generation started
	int32_t time_stopped; // Time in milliseconds when generation stopped
	int t_cnt = 1600; // Timeslot count (eighth notes) to stop generation
	int t_allocated = 1600; // Timeslot count to initialize vectors
	int t_generated = 0; // Timeslots generated
	int t_send = 60; // Timeslot count to send
	int t_sent = 0; // Timeslot count sent to mainframe
	int ng_min = 1000; // Minimum generated note
	int ng_max = 0; // Maximum generated note
	int ngv_min[MAX_VOICE]; // Minimum generated note per voice
	int ngv_max[MAX_VOICE]; // Maximum generated note per voice
	float tg_min = 1000; // Minimum generated tempo
	float tg_max = 0; // Maximum generated tempo
	float basic_tempo = 100; // Basic tempo
	float midifile_in_mul = 1; // Multiply note length with this value when loading
	float midifile_out_mul = 1; // Multiply note length with this value when saving
	int shuffle = 0; // If you want to shuffle all canti after generation (can shuffle up to 32000 canti)
	int comment_adapt = 1; // If you want to have each adaptation decision commented
	
	// Output
	vector< vector <unsigned char> > pause; // 1 = pause, 0 = note
	vector< vector <unsigned char> > note; // Note values (midi)
	vector< vector <unsigned short> > len; // Note length in timeslots
	vector< vector <unsigned short> > coff; // Offset of current note start backward (0 = first timeslot of note)
	vector< vector <unsigned short> > poff; // Offset of previous note start (backward)
	vector< vector <unsigned short> > noff; // Offset of next note start (forward)
	vector< vector <unsigned char> > tonic; // Key tonic of current note (3 = D#)
	vector< vector <unsigned char> > dyn; // Dynamics (velocity for piano)
	vector< vector <unsigned char> > vel; // Velocity of midi notes
	vector< vector <unsigned char> > artic; // Velocity of midi notes
	vector< vector <unsigned char> > lining; // Visual lining pattern
	vector< vector <unsigned char> > midi_ch; // Midi channel of each note
	vector< Color > linecolor; // Shows color of bar line if not zero
	vector< vector <char> > lengroup; // How many notes left until last in lengroup
	vector< vector <CString> > comment; // Comment for note
	vector< vector <CString> > adapt_comment; // Adaptation comment for note
	vector< vector <Color> > color; // Note color (rgb ignored if all zero; alpha ignored if zero)
	vector<float> tempo; // Tempo
	vector<float> stime; // Time of current step start in ms
	vector<float> etime; // Time of current step ending in ms
	vector< vector <float> > dstime; // Delta of current step start in ms for playback
	vector< vector <float> > detime; // Delta of current step ending in ms for playback
	vector <int> show_transpose; // Semitone transpose for showing
	vector<CString> track_name; // Track names from midi file for each voice
	vector<int> track_id; // Track id from midi file for each voice
	vector<int> track_vid; // Voice id inside track from midi file for each voice

	// Instruments
	vector<int> instr; // Instruments for each voice
	vector<int> instr_channel;
	vector<int> instr_type;
	vector<int> instr_nmin;
	vector<int> instr_nmax;
	vector<int> instr_tmin;
	vector<int> instr_tmax;
	vector<int> instr_poly; // Maximum number of simultaneous voices
	vector<int> CC_steps;
	vector<int> CC_dyn_ma;
	vector<int> CC_dyn;
	vector<int> CC_retrigger;
	vector<int> retrigger_freq;
	vector<int> max_slur_count;
	vector<int> max_slur_interval;
	vector<int> slur_ks;
	vector<int> legato_ahead;
	vector<float> nonlegato_freq;
	vector<int> nonlegato_minlen;
	vector<int> lengroup2;
	vector<int> lengroup3;
	vector<int> lengroup4;
	vector<int> lengroup_edt1;
	vector<int> lengroup_edt2;
	vector<float> rand_start;
	vector<float> rand_end;
	vector<int> retrigger_min_len; // Minimum next note length in ms to use retrigger
	vector<int> retrigger_rand_end; // Maximum percent of note length to move note end to the left in case of nonlegato
	vector<int> retrigger_rand_max; // Maximum length in ms to move note end to the left in case of nonlegato retrigger
	vector<int> vel_harsh; // Velocity equal or above this value triggers harsh sustain
	vector<int> vel_immediate; // Velocity equal or above this value triggers immediate sustain
	vector<int> vel_normal; // Velocity equal or above this value triggers normal sustain
	vector<int> vel_gliss; // Velocity below this value triggers glissando transition
	vector<int> vel_normal_minlen; // Minimum note length that can have a normal or lower sustain
	vector<int> gliss_minlen; // Minimum note length that can have a gliss transition
	vector<int> gliss_freq; // Frequency of gliss articulation in percent
	vector<int> rand_start_max; // Maximum shift in ms
	vector<int> rand_end_max; // Maximum shift in ms
	vector<int> max_ahead_note; // Maximum chromatic interval having ahead property
	vector<float> bell_start_mul; // Multiply dynamics by this parameter at bell start
	vector<float> bell_end_mul; // Multiply dynamics by this parameter at bell end
	vector<float> bell_start_len; // Percent of notelength to use for slope at bell start
	vector<float> bell_end_len; // Percent of notelength to use for slope at bell end
	int adapt_enable = 1;

protected:
	void ValidateVectors(int step1, int step2);
	// File operations
	void SaveVector2C(ofstream & fs, vector<vector<unsigned char>>& v2D, int i);
	void SaveVector2S(ofstream & fs, vector<vector<unsigned short>>& v2D, int i);
	void SaveVector2Color(ofstream & fs, vector<vector<Color>>& v2D, int i);
	void SaveVector2ST(ofstream & fs, vector<vector<CString>>& v2D, int i);
	void SaveVector(ofstream &fs, vector<float> &v);
	void SaveVector(ofstream &fs, vector<Color> &v);
	void LoadVector2C(ifstream & fs, vector<vector<unsigned char>>& v2D, int i);
	void LoadVector2S(ifstream & fs, vector<vector<unsigned short>>& v2D, int i);
	void LoadVector2Color(ifstream & fs, vector<vector<Color>>& v2D, int i);
	void LoadVector2ST(ifstream & fs, vector<vector<CString>>& v2D, int i);
	void LoadVector(ifstream & fs, vector<Color>& v);
	void LoadVector(ifstream & fs, vector<float>& v);
	void LoadVector(ifstream & fs, vector<unsigned char>& v);
	virtual void LoadConfigLine(CString* sN, CString* sV, int idata, float fdata) = 0;
	// Helper functions for child generators
	void CountOff(int step1, int step2);
	void CountTime(int step1, int step2);
	void CopyVoice(int v1, int v2, int step1, int step2, int interval);
	void UpdateNoteMinMax(int step1, int step2);
	void UpdateTempoMinMax(int step1, int step2);
	void AddNote(int pos, int v, char note2, int len2, int dyn2);
	void ResizeVectors(int size, int vsize = -1);

};

