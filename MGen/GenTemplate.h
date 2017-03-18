#pragma once

#define GAlgNum 2
#define MAX_VOICE 16

#include "portmidi.h"
#include "porttime.h"

typedef  unsigned long int  ub4; // a ub4 is an unsigned 4-byte quantity

const CString GAlgName[] = {
	"No algorithm selected", // 0
	"Cantus firmus 1", // 1
	"Cantus firmus 2" // 2
};

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
	CGenTemplate();
	virtual ~CGenTemplate();
	void Init();
	virtual void Generate();
	void ResizeVectors(int size);
	// PortMIDI
	void StartMIDI(int midi_device_i, int latency);
	void SendMIDI(int step1, int step2);
	void StopMIDI();
	int randbw(int n1, int n2);
	// Random
	void isaac();
	void randinit(int flag);
	unsigned int rand2();

	HWND m_hWnd;
	UINT WM_GEN_FINISH;
	UINT WM_DEBUG_MSG;

public:
	// Interface
	short need_exit=0; // If thread needs to exit
	timed_mutex mutex_output;
	
	// PortMIDI
	PmStream * midi;
	int midi_sent = 0; // Steps already sent to midi
	int midi_sent_t = 0; // Timestamp of last event sent to midi
	int midi_start_time = 0; // Time when midi started to play

	// Main constants
	int v_cnt=1; // Voice count
	milliseconds time_started; // Time in milliseconds when generation started
	milliseconds time_stopped; // Time in milliseconds when generation stopped
	int t_cnt = 1600; // Timeslot count (eighth notes) to stop generation
	int t_allocated = 1600; // Timeslot count to initialize vectors
	int t_generated = 0; // Timeslots generated
	int t_send = 60; // Timeslot count to send
	int t_sent = 0; // Timeslot count sent to mainframe
	int ng_min = 1000; // Minimum generated note
	int ng_max = 0; // Maximum generated note
	int tg_min = 1000; // Minimum generated tempo
	int tg_max = 0; // Maximum generated tempo
	float basic_tempo = 100; // Basic tempo
	vector <string> instr; // Instruments for each voice
	// Output
	vector< vector <unsigned char> > pause; // 1 = pause, 0 = note
	vector< vector <unsigned char> > note; // Note values (midi)
	vector< vector <unsigned char> > len; // Note length in timeslots
	vector< vector <unsigned char> > coff; // Offset of current note start backward (0 = first timeslot of note)
	vector< vector <unsigned char> > poff; // Offset of previous note start (backward)
	vector< vector <unsigned char> > noff; // Offset of next note start (forward)
	vector< vector <unsigned char> > att; // Attack (velocity for piano)
	vector<unsigned short> tempo; // Tempo
	vector<double> stime; // Time of current step in ms
	vector<double> ntime; // Time of current step in ms
  
  // Random generator
	ub4 randrsl[256], randcnt; // external results
	ub4 mm[256];  // internal state
	ub4 aa = 0, bb = 0, cc = 0;
	int cur_rand = 0, cur_rand2 = 0;
};
