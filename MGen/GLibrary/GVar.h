#pragma once
#include "GIConf.h"

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
	void AddIcf();
	void AddMelody(int step1, int step2, int v, CString info, CString info2 = "");
	void LoadInstruments(); // Load instruments config
	void LoadInstrument(int i, CString fname);
	void LoadCCName(CString * sName, CString * sValue, CString sSearch, int i);
	void LoadKswGroup(CString * sName, CString * sValue, CString sSearch, int i);
	PmMessage ParseMidiCommand(CString st, int i);
	void SaveInitCommand(PmMessage msg, int i);
	void LoadInitCommand(CString * sName, CString * sValue, CString sSearch, int i);
	void LoadTechnique(CString * sName, CString * sValue, CString sSearch, int i);
	void LoadInitTechnique(CString * sName, CString * sValue, CString sSearch, int i);
	void LoadInstrumentLine(CString st2, CString st3, int i);
	void LoadConfig(CString fname, int load_includes = 1);
	void LoadConfigFiles(CString fname, int load_includes);
	void LoadInstrumentLayout();
	void LoadInstrumentLayoutLine(CString & st2, CString & st3);
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
	int master_vol = 100; // Master volume for all tracks. 0 - automatic, 100 - maximum
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
	vector< vector <unsigned char> > note_muted; // Note values (midi)
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
	vector< vector <map<short, char>> > nlink; // [i][v] Link to other note for LY
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
	vector< vector <DWORD> > smst; // Source midi start tick
	vector< vector <DWORD> > smet; // Source midi end tick
	vector< vector <float> > dstime; // Delta of current step start in ms for playback
	vector< vector <float> > detime; // Delta of current step ending in ms for playback
	vector<CString> track_name; // Track names from midi file for each voice
	vector<int> track_id; // Track id from midi file for each voice
	vector<int> track_vid; // Voice id inside track from midi file for each voice
	vector<int> itrack; // Instrument local track number for each track
	int stages_calculated = 0;

	// Instruments
	map<CString, CString> InstDefaultConfig; // Instrument configs by name
	CString instr_layout; // Name of instruments/*.txt file to load instrument layout from
	CString m_config_insts; // String with instrument mapping from config
	vector<int> instr; // Instruments for each voice
	vector<int> v_stage; // Stage for each voice
	vector<int> v_itrack; // Instrument local track number for each voice
	vector<int> t_instr; // Instrument for each result track. This value can be overwritten by instruments sharing same track
	vector<IConf> icf;
	int virt_instr_count = 0;

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
	short CreateVirtualInstrument(int instr_id, int child_id);
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