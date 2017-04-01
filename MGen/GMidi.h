#pragma once
#include "GAdapt.h"

#include "portmidi.h"
#include "porttime.h"

// PortMIDI
#define OUTPUT_BUF_SIZE 10000
#define MIN_MIDI_BUF_MSEC 10000
#define MAX_MIDI_BUF_MSEC 20000
#define MIDI_BUF_PROTECT 500 // Number of ms to postpone playback on start
#define DRIVER_INFO NULL
#define TIME_PROC ((int32_t (*)(void *)) Pt_Time)
#define TIME_INFO NULL
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

	// PortMIDI
	void StartMIDI(int midi_device_i, int latency, int from);
	void SendMIDI(int step1, int step2);
	void StopMIDI();
	int GetPlayStep();

	// PortMIDI
	PmStream * midi = 0;
	int midi_sent = 0; // Steps already sent to midi
	int midi_sent_t = 0; // Timestamp of last event sent to midi in previous SendMIDI
	int midi_sent_t2 = 0; // Timestamp of last event sent to midi in current SendMIDI
	PmMessage midi_sent_msg; // Last event sent to midi in previous SendMIDI
	PmMessage midi_sent_msg2; // Last event sent to midi in current SendMIDI
	int midi_current_step; // Current step processed by SendMIDI (for logs)
	int midi_start_time = 0; // Time when midi started to play
	int midi_last_run = 0; // If current SendMIDI is last
	int midi_first_run = 0; // If current SendMIDI is first
	int buf_underrun = 0; // Shows if current playback had an issue with buffer underrun
	int midi_play_step = 0; // Current step being played by midi

protected:
	// PortMIDI internal
	void AddMidiEvent(PmTimestamp timestamp, int mm_type, int data1, int data2);
	void AddNoteOn(PmTimestamp timestamp, int data1, int data2);
	void AddKsOn(PmTimestamp timestamp, int data1, int data2);
	void AddNoteOff(PmTimestamp timestamp, int data1, int data2);
	void AddKsOff(PmTimestamp timestamp, int data1, int data2);
	void AddCC(PmTimestamp timestamp, int data1, int data2);
	void AddTransitionCC(int i, int stimestamp, int CC, int value1, int value2);
	void AddTransitionKs(int i, PmTimestamp timestamp, int ks);
	static int PmEvent_comparator(const void *v1, const void *v2);

	// Information for current note in SendMIDI
	vector <PmEvent> midi_buf;
	vector <PmEvent> midi_buf_next; // Buffer for next SendMIDI run
	int midi_buf_lim; // After this timestamp information goes to midi_buf_next
	int midi_channel = 0;
	int midi_voice = 0;

	// Cantus
	vector< vector <unsigned char> > cantus; // Cantus loaded from midi file
};

