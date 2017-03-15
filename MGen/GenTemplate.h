#pragma once

const int GAlgNum = 2;

const CString GAlgName[] = {
	L"No algorithm selected", // 0
	L"Cantus firmus 1", // 1
	L"Cantus firmus 2" // 2
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

protected:
	// Main constants
	int v_cnt=1; // Voice count
	int t_cnt = 1600; // Timeslot count (eighth notes) to stop generation
	int t_cnt2 = 1600; // Timeslot count to start with
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
