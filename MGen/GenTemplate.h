#pragma once

#define MAX_VOICE 16

#include "portmidi.h"
#include "porttime.h"

typedef  unsigned long int  ub4; // a ub4 is an unsigned 4-byte quantity

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
	static void CGenTemplate::CheckVar(CString* sName, CString* sValue, char* sSearch, int* Dest, int vmin = -1, int vmax = -1);
	static void LoadVar(CString * sName, CString * sValue, char * sSearch, CString * Dest);
	static bool dirExists(CString dirName_in);
	static bool fileExists(CString dirName_in);
  static bool nodeExists(CString dirName_in);

protected:
	void SaveVector2C(ofstream * fs, vector<vector<unsigned char>>& v2D, int i);
	void SaveVectorD(ofstream &fs, vector<double> &v);
	void LoadVector2C(ifstream & fs, vector<vector<unsigned char>>& v2D, int i, int v_cnt);
	void LoadVectorD(ifstream & fs, vector<double>& v, int t_generated);

public:
	CGenTemplate();
	virtual ~CGenTemplate();
	virtual void LoadConfigLine(CString* sN, CString* sV) = 0;
	virtual void Generate() = 0;

	void InitRandom();
	void InitVectors();
	void LoadConfig(CString fname);
	void ResizeVectors(int size);
	void SaveResults(CString dir, CString fname);
	void LoadResults(CString dir, CString fname);

	// PortMIDI
	void StartMIDI(int midi_device_i, int latency);
	void SendMIDI(int step1, int step2);
	void StopMIDI();
	int randbw(int n1, int n2);
	int GetPlayStep();
	// Random
	void isaac();
	void randinit(int flag);
	unsigned int rand2();

	HWND m_hWnd;
	UINT WM_GEN_FINISH;
	UINT WM_DEBUG_MSG;

public:
	// Interface
	int need_exit=0; // If thread needs to exit
	timed_mutex mutex_output;
	int m_algo_id;
	CString m_config;
	
	// PortMIDI
	PmStream * midi = 0;
	int midi_sent = 0; // Steps already sent to midi
	int midi_sent_t = 0; // Timestamp of last event sent to midi
	int midi_start_time = 0; // Time when midi started to play
	int buffer_underrun = 0; // Shows if current playback had an issue with buffer underrun
	int midi_play_step = 0; // Current step being played by midi

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
	vector<double> tempo; // Tempo
	vector<double> stime; // Time of current step in ms
	vector<double> ntime; // Time of current step in ms
  
  // Random generator
	ub4 randrsl[256], randcnt; // external results
	ub4 mm[256];  // internal state
	ub4 aa = 0, bb = 0, cc = 0;
	int cur_rand = 0, cur_rand2 = 0;
};
