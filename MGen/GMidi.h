#pragma once
#include "GAdapt.h"
#include "MidiOut.h"

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

// PortMIDI
#define MIN_MIDI_BUF_MSEC 6000
#define MAX_MIDI_BUF_MSEC 12000
#define MIDI_BUF_PROTECT 500 // Number of ms to postpone playback on start
#define TIME_START Pt_Start(1, 0, 0) /* timer started w/millisecond accuracy */

class CGMidi :
	public CGAdapt
{
public:
	CGMidi();
	~CGMidi();
	virtual void Generate() = 0;

	// Midi files
	void SaveMidi(CString dir, CString fname);
	void LoadMidi(CString path);

	void LoadCantus(CString path);

	// RtMidi
	CMidiOut *mo;
	void StartMIDI(int midi_device_i, int from);
	void LogInstruments();
	void CheckDstime(int i, int v);
	void SendMIDI(int step1, int step2);
	void InterpolateCC(int CC, int ma, float rnd, int step1, int step2, vector<vector<unsigned char>>& dv, int ii, int v);
	void StopMIDI();
	int GetPlayStep();
	
	// PortMIDI
	int midi_sent = 0; // Steps already sent to midi
	int midi_sent_t = 0; // Timestamp of last event sent to midi in previous SendMIDI
	int midi_sent_t2 = 0; // Timestamp of last event sent to midi in current SendMIDI
	int midi_sent_t3 = 0; // Timestamp of last event postponed in current SendMIDI
	PmMessage midi_sent_msg; // Last event sent to midi in previous SendMIDI
	PmMessage midi_sent_msg2; // Last event sent to midi in current SendMIDI
	PmMessage midi_sent_msg3; // Last event postponed in current SendMIDI
	int midi_current_step; // Current step processed by SendMIDI (for logs)
	int midi_start_time = 0; // Time when midi started to play
	int midi_last_run = 0; // If current SendMIDI is last
	int midi_first_run = 0; // If current SendMIDI is first
	int buf_underrun = 0; // Shows if current playback had an issue with buffer underrun
	int midi_play_step = 0; // Current step being played by midi

	// Midi files
	int midifile_loaded = 0; // If MIDI was loaded from file
	int midifile_type = 1; // Type of MIDI file loaded

protected:
	// Warnings
	int warning_loadmidi_align = 0;
	int warning_loadmidi_short = 0;
	int warning_loadmidi_long = 0;
	int warning_ahead = 0;

	// PortMIDI internal
	void AddMidiEvent(PmTimestamp timestamp, int mm_type, int data1, int data2);
	void AddNoteOn(PmTimestamp timestamp, int data1, int data2);
	void AddKsOn(PmTimestamp timestamp, int data1, int data2);
	void AddNoteOff(PmTimestamp timestamp, int data1, int data2);
	void AddKsOff(PmTimestamp timestamp, int data1, int data2);
	void AddCC(PmTimestamp timestamp, int data1, int data2);
	void AddKs(int stimestamp, int ks);
	void AddTransitionCC(int i, int stimestamp, int CC, int value1, int value2);
	void AddTransitionKs(int i, PmTimestamp timestamp, int ks);
	static int PmEvent_comparator(const void *v1, const void *v2);
	void LoadWav(CString fname);

	// Information for current note in SendMIDI
	vector <PmEvent> midi_buf;
	vector <PmEvent> midi_buf_next; // Buffer for next SendMIDI run
	int midi_buf_lim; // After this timestamp information goes to midi_buf_next
	int midi_channel = 0;
	int midi_voice = 0;

	// Cantus
	vector< vector <int> > cantus; // Cantus loaded from midi file
	vector< vector <int> > cantus_len; // Cantus loaded from midi file
	vector< vector <float> > cantus_tempo; // Cantus loaded from midi file
};

