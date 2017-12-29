#pragma once
#include "GAdapt.h"
#include "MidiOut.h"

#define MAX_WARN_MIDI_OVERLAP 5
#define MAX_WARN_MIDI_ALIGN 5
#define MAX_WARN_MIDI_SHORT 5
#define MAX_WARN_MIDI_LONG 5
#define MAX_WARN_MIDI_AHEAD 5
// Maximum delta from midi file position in ms without warning
#define MAX_ALLOW_DELTA 30
// Maximum delay (ms) between transition and keyswitch
#define MAX_TRANS_DELAY 10
// Maximum time (ms) allowed to move note and linked events (ks/cc) left
#define MAX_AHEAD 1000
// Maximum allowed overlap for monophonic instrument (1 = 100% note length)
#define MAX_OVERLAP_MONO 0.2
// Maximum allowed overlap for melody in polyphonic instrument (1 = 100% note length)
#define MAX_OVERLAP_POLY 0.2
// Trailing pauses to add at the end of track. Should be 2 or greater
#define TAIL_STEPS 4
// Minimum cantus size in steps to allow loading
#define MIN_CANTUS_SIZE 5
// Minimum counterpoint size in steps to allow loading
#define MIN_CP_SIZE 5

// PortMIDI
#define MIN_MIDI_BUF_MSEC 6000
#define MAX_MIDI_BUF_MSEC 12000
#define MIDI_BUF_PROTECT 500 // Number of ms to postpone playback on start
#define TIME_START Pt_Start(1, 0, 0) /* timer started w/millisecond accuracy */

// Rule visualization
#define vDefault 0
#define vHarm 1
#define vLine 2
#define vLines 3
#define MAX_VIZ 4

// Bass instruments
const int bass_program[] = { 45, 33, 70, 58, 34, 35, 36, 37, 38 };

#define MAX_CLEF 6
const int LyClefCenter[] = { 26, 38, 50, 71, 83, 95 };
const CString LyClef[] = { "bass_15", "bass_8", "bass", "treble", "treble^8", "treble^15" };

const int key_base[] = {
	0, // 0
	2, // 1
	2, // 2
	4, // 3
	4, // 4
	5, // 5
	7, // 6
	7, // 7
	9, // 8
	9, // 9
	11, // 10
	11 // 11
};

const CString LyMajorKey[] = {
	"c", // 0
	"df", // 1
	"d", // 2
	"ef", // 3
	"e", // 4
	"f", // 5
	"gf", // 6
	"g", // 7
	"af", // 8
	"a", // 9
	"bf", // 10
	"b" // 11
};

const int key_base_m[] = {
	0, // 0
	0, // 1
	2, // 2
	4, // 3
	4, // 4
	5, // 5
	5, // 6
	7, // 7
	7, // 8
	9, // 9
	11, // 10
	11 // 11
};

const CString LyMinorKey[] = {
	"c", // 0
	"cs", // 1
	"d", // 2
	"ef", // 3
	"e", // 4
	"f", // 5
	"fs", // 6
	"g", // 7
	"gs", // 8
	"a", // 9
	"bf", // 10
	"b" // 11
};

const int LyMajorKeySharp[] = {
	1, // 0
	0, // 1
	1, // 2
	0, // 3
	1, // 4
	0, // 5
	0, // 6
	1, // 7
	0, // 8
	1, // 9
	0, // 10
	1 // 11
};

const int LyMinorKeySharp[] = {
	0, // 0
	1, // 1
	0, // 2
	0, // 3
	1, // 4
	0, // 5
	1, // 6
	0, // 7
	1, // 8
	1, // 9
	0, // 10
	1 // 11
};

const CString LyNoteSharp[] = {
	"c", // 0
	"cs", // 1
	"d", // 2
	"ds", // 3
	"e", // 4
	"f", // 5
	"fs", // 6
	"g", // 7
	"gs", // 8
	"a", // 9
	"as", // 10
	"b" // 11
};

const CString LyNoteFlat[] = {
	"c", // 0
	"df", // 1
	"d", // 2
	"ef", // 3
	"e", // 4
	"f", // 5
	"gf", // 6
	"g", // 7
	"af", // 8
	"a", // 9
	"bf", // 10
	"b" // 11
};

const CString LyOctave[] = {
	",,,,", // 0
	",,,", // 1
	",,", // 2
	",", // 3
	"", // 4
	"'", // 5
	"''", // 6
	"'''", // 7
	"''''", // 8
	"'''''", // 9
	"''''''", // 10
	"'''''''" // 11
};

class CGMidi :
	public CGAdapt
{
public:
	CGMidi();
	void BuildKeyMatrix();
	~CGMidi();
	void GetLyRange(int step1, int step2, vector<int>& vm_min, vector<int>& vm_max);
	int GetLyVcnt(int step1, int step2, vector<int>& vm_max);
	CString GetLyAlter(int alter);
	CString GetLyAlterVisual(int alter);
	CString GetLyNote(int i, int v);
	CString GetLyNoteVisual(int i, int v);
	CString GetLyLen(int length);
	void SplitLyNote5(int pos, vector<int>& la);
	void SplitLyNoteMeasure(int pos, int le, vector<int>& la);
	void SplitLyNote(int pos, int le, vector<int>& la);
	void GetLySev(ofstream & fs, int pos, CString & ev, int le, int i, int v);
	void SendLyViz(ofstream & fs, int pos, CString & ev, int le, int i, int v, int phase);
	void SendLyViz(ofstream & fs, int pos, CString & ev, int le, int i, int v);
	void SendLyEvent(ofstream & fs, int pos, CString ev, int le, int i, int v);
	CString GetLyColor(DWORD col);
	CString GetLyMarkColor(DWORD col);
	CString GetLyMarkColor2(DWORD col);
	void SendLyNoteColor(ofstream & fs, DWORD col);
	CString GetIntName(int iv);
	void SendLyFlagColor(ofstream & fs, int i, int v);
	void ParseLyComments(int i, int v, int foreign);
	void SaveLyComments(int i, int v, int pos);
	CString DetectLyClef(int vmin, int vmax);
	void SaveLySegment(ofstream & fs, CString st, CString st2, int step1, int step2);
	void SaveLy(CString dir, CString fname);
	void ExportAdaptedMidi(CString dir, CString fname);
	virtual void Generate() = 0;

	// Midi files
	void SaveMidi(CString dir, CString fname);
	void LoadMidi(CString path);
	void MergeSmallOverlaps(int step1, int step2);

	void LoadCantus(CString path);
	void LoadCP(CString path);

	void ProcessInter(int pos, int pos_old, std::vector<std::vector<std::pair<int, int>>> &inter, int hid, std::vector<int> &min_len, std::vector<int> &max_len);

	// RtMidi
	CMidiOut *mo;
	void StartMIDI(int midi_device_i, int from);
	void LogInstruments();
	void CheckDstime(int i, int v);
	void SendMIDI(int step1, int step2);
	void InterpolateCC(int CC, float rnd, int step1, int step2, vector<vector<unsigned char>>& dv, int ii, int v);
	void StopMIDI();
	int GetPlayStep();
	
	// PortMIDI
	int midi_sent = 0; // Steps already sent to midi
	long long midi_sent_t = 0; // Timestamp of last event sent to midi in previous SendMIDI
	long long midi_sent_t2 = 0; // Timestamp of last event sent to midi in current SendMIDI
	long long midi_sent_t3 = 0; // Timestamp of last event postponed in current SendMIDI
	PmMessage midi_sent_msg = 0; // Last event sent to midi in previous SendMIDI
	PmMessage midi_sent_msg2 = 0; // Last event sent to midi in current SendMIDI
	PmMessage midi_sent_msg3 = 0; // Last event postponed in current SendMIDI
	int midi_current_step = 0; // Current step processed by SendMIDI (for logs)
	long long midi_start_time = 0; // Time when midi started to play
	int midi_last_run = 0; // If current SendMIDI is last
	int midi_first_run = 0; // If current SendMIDI is first
	int buf_underrun = 0; // Shows if current playback had an issue with buffer underrun
	int midi_play_step = 0; // Current step being played by midi

	// Lilypond
	int ly_flag_style = 1; // 0 - no flag visualisation, 1 - color note, 2 - x above note
	int ly_msh = 1; // 0 - do not show msh, 1 - show msh
	vector<int> ly_fa; // Flags array
	vector<int> ly_fa2; // Flags array for whole note
	vector<int> rule_viz; // [r_id] If this rule violation should be marked with harmony color, not note color, even if its group is not Harmony
	vector<int> vtype_sev; // Worst severity for each vizualization types
	vector <int> severity; // Get severity by flag id
	vector<DWORD>  flag_color; // Flag colors
	int ly_nnum; // Note number
	CString ly_com_st;

	// Midi files
	vector<vector<vector<PmEvent>>> midifile_buf;
	int amidi_export = 0; // Exporting adapted midi
	int midifile_loaded = 0; // If MIDI was loaded from file
	int midifile_type = 1; // Type of MIDI file loaded

protected:
	// Warnings
	int warning_loadmidi_align = 0;
	int warning_loadmidi_short = 0;
	int warning_loadmidi_long = 0;
	int warning_ahead = 0;
	int warning_loadmidi_overlap = 0;

	// PortMIDI internal
	void AddMidiEvent(long long timestamp, int mm_type, int data1, int data2);
	void AddNoteOn(long long timestamp, int data1, int data2);
	void AddKsOn(long long timestamp, int data1, int data2);
	void AddNoteOff(long long timestamp, int data1, int data2);
	void AddKsOff(long long timestamp, int data1, int data2);
	void AddCC(long long timestamp, int data1, int data2);
	void AddKs(long long stimestamp, int ks);
	void AddTransitionCC(int i, long long stimestamp, int CC, int value1, int value2);
	void AddTransitionKs(int i, long long timestamp, int ks);
	static int PmEvent_comparator(const void *v1, const void *v2);
	void LoadWav(CString fname);

	// Information for current note in SendMIDI
	vector <PmEvent> midi_buf;
	vector <PmEvent> midi_buf_next; // Buffer for next SendMIDI run
	long long midi_buf_lim = 0; // After this timestamp information goes to midi_buf_next
	int midi_channel = 0;
	int midi_stage = 0;
	int midi_voice = 0;

	// Cantus
	vector< vector <int> > cantus; // Cantus loaded from midi file
	vector< vector <CString> > cantus_incom; // Cantus lyrics loaded from midi file
	vector< vector <int> > cantus_len; // Cantus loaded from midi file
	vector< vector <float> > cantus_tempo; // Cantus loaded from midi file
	vector< vector <vector<int>>> cpoint; // Counterpoint loaded from midi file

	// Lilypond
	int vm_cnt = 1; // Number of voices in segment to display
};

