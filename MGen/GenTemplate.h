#pragma once

const int GAlgNum = 2;

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

class CGenTemplate
{
public:
	CGenTemplate();
	virtual ~CGenTemplate();
	virtual void Generate();
	void Init();

	HWND m_hWnd;
	UINT WM_GEN_FINISH;
	UINT WM_DEBUG_MSG;
	UINT WM_WARN_MSG;

public:
	// Main constants
	int v_cnt=1; // Voice count
	int t_cnt = 1600; // Timeslot count (eighth notes) to stop generation
	int t_init = 1600; // Timeslot count to initialize vectors
	int t_send = 10; // Timeslot count to send
	int t_generated = 0; // Timeslots generated
	float basic_tempo = 100; // Basic tempo
	vector <string> instr; // Instruments for each voice
	// Output
	vector< vector <unsigned char> > pause; // 1 = pause, 0 = note
	vector< vector <unsigned char> > note; // Note values (midi)
	vector< vector <unsigned char> > len; // Note length in timeslots
	vector< vector <unsigned char> > coff; // Offset of current note start backward (0 = first timeslot of note)
	vector< vector <unsigned char> > poff; // Offset of previous note start (backward)
	vector< vector <unsigned char> > noff; // Offset of next note start (forward)
	vector< vector <unsigned char> > tempo; // Tempo
	vector< vector <unsigned char> > att; // Attack (velocity for piano)
};
