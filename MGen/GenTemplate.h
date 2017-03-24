#pragma once

#define MAX_VOICE 16
#define MAX_INSTR 16

#include "portmidi.h"
#include "porttime.h"

typedef  unsigned long int  ub4; // a ub4 is an unsigned 4-byte quantity

const int dia_to_chrom[] = { 0, 2, 4, 5, 7, 9, 11 };

#define ARTIC_LEGATO 0
#define ARTIC_SLUR 1
#define ARTIC_NONLEGATO 2
#define ARTIC_EXACT 3
#define ARTIC_STACCATO 4

#define INSTR_PIANO 0
#define INSTR_VIOLIN 1
#define INSTR_TRUMPET 11
#define INSTR_FLUTE 6

const CString InstName[] = {
	"Piano", // 0
	"Violin", // 1
	"Violin2", // 2
	"Viola", // 3
	"Cello", // 4
	"Bass", // 5
	"Flute", // 6
	"Oboe", // 7
	"Clarinet", // 8
	"Bassoon", // 9
	"Horn", // 10
	"Trumpet", // 11
	"Trombone", // 12
	"Tuba", // 13
	"Harp", // 14
	"Percussion" // 15
};

const CString NoteName[] = {
	"C", // 0
	"C#", // 1
	"D", // 2
	"D#", // 3
	"E", // 4
	"F", // 5
	"F#", // 6
	"G", // 7
	"G#", // 8
	"A", // 9
	"A#", // 10
	"H" // 11
};

const CString NoteName2[] = {
	"C", // 0
	"DB", // 1
	"D", // 2
	"EB", // 3
	"E", // 4
	"F", // 5
	"GB", // 6
	"G", // 7
	"AB", // 8
	"A", // 9
	"BB", // 10
	"B" // 11
};

// PortMIDI
#define OUTPUT_BUFFER_SIZE 10000
#define MIN_MIDI_BUFFER_MSEC 10000
#define MAX_MIDI_BUFFER_MSEC 20000
#define DRIVER_INFO NULL
#define TIME_PROC ((int32_t (*)(void *)) Pt_Time)
#define TIME_INFO NULL
#define TIME_START Pt_Start(1, 0, 0) /* timer started w/millisecond accuracy */

class CGenTemplate
{
public:
	static void copy_file(CString sName, CString dName);
	static void AppendLineToFile(CString fname, CString st);
	static void CheckVar(CString* sName, CString* sValue, char* sSearch, int* Dest, int vmin = -1, int vmax = -1);
	static void CheckVar(CString * sName, CString * sValue, char * sSearch, double * Dest);
	static void LoadVar(CString * sName, CString * sValue, char * sSearch, CString * Dest);
	static void LoadNote(CString * sName, CString * sValue, char * sSearch, int * Dest);
	static bool dirExists(CString dirName_in);
	static bool fileExists(CString dirName_in);
  static bool nodeExists(CString dirName_in);
	static CString fname_from_path(CString path);
	static CString bname_from_path(CString path);
	static CString dir_from_path(CString path);
	static int PmEvent_comparator(const void *v1, const void *v2);
	static CString GetNoteName(int n);
	static int GetNoteI(CString st);

protected:
	// File operations
	void SaveVector2C(ofstream & fs, vector<vector<unsigned char>>& v2D, int i);
	void SaveVector2Color(ofstream & fs, vector<vector<Color>>& v2D, int i);
	void SaveVector2ST(ofstream & fs, vector<vector<CString>>& v2D, int i);
	void SaveVectorD(ofstream &fs, vector<double> &v);
	void LoadVector2C(ifstream & fs, vector<vector<unsigned char>>& v2D, int i);
	void LoadVector2Color(ifstream & fs, vector<vector<Color>>& v2D, int i);
	void LoadVector2ST(ifstream & fs, vector<vector<CString>>& v2D, int i);
	void LoadVectorD(ifstream & fs, vector<double>& v);
	virtual void LoadConfigLine(CString* sN, CString* sV, int idata, double fdata) = 0;
	// Helper functions for child generators
	void CountOff(int step1, int step2);
	void CountTime(int step1, int step2);
	void UpdateNoteMinMax(int step1, int step2);
	void UpdateTempoMinMax(int step1, int step2);
	void Adapt(int step1, int step2);
	// Mathematics
	int randbw(int n1, int n2);
	// Random
	void isaac();
	void randinit(int flag);
	unsigned int rand2();

public:
	CGenTemplate();
	virtual ~CGenTemplate();
	virtual void Generate() = 0;

	void InitRandom();
	void TestRandom(); // Tests random generator
	void InitVectors();
	void LoadInstruments(); // Load instruments config
	void LoadConfig(CString fname);
	void ResizeVectors(int size);
	void SaveResults(CString dir, CString fname);
	void LoadResults(CString dir, CString fname);
	void SaveMidi(CString dir, CString fname);

	// PortMIDI
	void StartMIDI(int midi_device_i, int latency, int from);
	void SendMIDI(int step1, int step2);
	static void WriteLog(int i, CString * pST);
	void StopMIDI();
	int GetPlayStep();

	static HWND m_hWnd;
	UINT WM_GEN_FINISH;
	static UINT WM_DEBUG_MSG;

public:
	// Interface
	int need_exit=0; // If thread needs to exit due to generation abort
	static int can_send_log; // If thread can send log to MainFrame (disabled OnClose)
	timed_mutex mutex_output;
	int m_algo_id = -1; // Current algorithm id
	CString m_config;
	int sleep_ms = 0;
	CString save_format_version; // Version of save format loaded

	// PortMIDI
	double m_pspeed = 100; // Playback speed in percent
	vector<int> play_transpose; // If generated notes are not in instrument range, playback is automatically transposed (semitones)
	vector<int> play_transpose_old; // Value for last note of previous SendMIDI call
	PmStream * midi = 0;
	int midi_sent = 0; // Steps already sent to midi
	int midi_sent_t = 0; // Timestamp of last event sent to midi
	int midi_start_time = 0; // Time when midi started to play
	int buffer_underrun = 0; // Shows if current playback had an issue with buffer underrun
	int midi_play_step = 0; // Current step being played by midi
	// MIDI play warnings for each voice show if warning was already fired to prevent repeated warnings
	vector<int> warning_note_range;
	vector<int> warning_note_short;

	// Main constants
	int v_cnt=1; // Voice count
	PmTimestamp time_started; // Time in milliseconds when generation started
	PmTimestamp time_stopped; // Time in milliseconds when generation stopped
	int t_cnt = 1600; // Timeslot count (eighth notes) to stop generation
	int t_allocated = 1600; // Timeslot count to initialize vectors
	int t_generated = 0; // Timeslots generated
	int t_send = 60; // Timeslot count to send
	int t_sent = 0; // Timeslot count sent to mainframe
	int ng_min = 1000; // Minimum generated note
	int ng_max = 0; // Maximum generated note
	int ngv_min[MAX_VOICE]; // Minimum generated note per voice
	int ngv_max[MAX_VOICE]; // Maximum generated note per voice
	double tg_min = 1000; // Minimum generated tempo
	double tg_max = 0; // Maximum generated tempo
	float basic_tempo = 100; // Basic tempo
	double midifile_tpq_mul = 1; // Multiplier of ticks per quarter notes in midi export
	int shuffle = 0; // If you want to shuffle all canti after generation (can shuffle up to 32000 canti)
	
	// Output
	vector< vector <unsigned char> > pause; // 1 = pause, 0 = note
	vector< vector <unsigned char> > note; // Note values (midi)
	vector< vector <unsigned char> > len; // Note length in timeslots
	vector< vector <unsigned char> > coff; // Offset of current note start backward (0 = first timeslot of note)
	vector< vector <unsigned char> > poff; // Offset of previous note start (backward)
	vector< vector <unsigned char> > noff; // Offset of next note start (forward)
	vector< vector <unsigned char> > dyn; // Dynamics (velocity for piano)
	vector< vector <unsigned char> > vel; // Velocity of midi notes
	vector< vector <unsigned char> > artic; // Velocity of midi notes
	vector< vector <char> > lengroup; // How many notes left until last in lengroup
	vector< vector <CString> > comment; // Comment for note
	vector< vector <Color> > color; // Note color (rgb ignored if all zero; alpha ignored if zero)
	vector<double> tempo; // Tempo
	vector<double> stime; // Time of current step start in ms
	vector<double> etime; // Time of current step ending in ms
	vector<double> dstime; // Delta of current step start in ms for playback
	vector<double> detime; // Delta of current step ending in ms for playback

	// Instruments
	vector<int> instr; // Instruments for each voice
	vector<int> instr_type;
	vector<int> instr_nmin;
	vector<int> instr_nmax;
	vector<int> instr_tmin;
	vector<int> instr_tmax;
	vector<int> CC_dynamics;
	vector<int> max_slur_count;
	vector<int> max_slur_interval;
	vector<int> slur_ks;
	vector<int> legato_ahead;
	vector<double> nonlegato_freq;
	vector<int> nonlegato_minlen;
	vector<int> lengroup2;
	vector<int> lengroup3;
	vector<int> lengroup4;
	vector<int> lengroup_edt1;
	vector<int> lengroup_edt2;

  // Random generator
	ub4 randrsl[256], randcnt; // external results
	ub4 mm[256];  // internal state
	ub4 aa = 0, bb = 0, cc = 0;
	int cur_rand = 0, cur_rand2 = 0;
};
