#pragma once

#define MAX_VOICE 16
#define MAX_INSTR 16

typedef  unsigned long int  ub4; // a ub4 is an unsigned 4-byte quantity

const int dia_to_chrom[] = { 0, 2, 4, 5, 7, 9, 11 };
const int chrom_to_dia[] = { 0, 0, 1, 1, 2, 3, 3, 4, 4, 5, 5, 6 };

#define ARTIC_LEGATO 0
#define ARTIC_SLUR 1
#define ARTIC_NONLEGATO 2
#define ARTIC_EXACT 3
#define ARTIC_STACCATO 4
#define ARTIC_RETRIGGER 5

#define INSTR_PIANO 0
#define INSTR_VIOLIN 1
#define INSTR_TRUMPET 11
#define INSTR_FLUTE 6

// MIDI PORT STATUS
#define MIDI_NOTEON 0x90
#define MIDI_NOTEOFF 0x90
#define MIDI_CC 0xB0

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

class CGLib
{
public:
	static void copy_file(CString sName, CString dName);
	static void AppendLineToFile(CString fname, CString st);
	static void CheckVar(CString* sName, CString* sValue, char* sSearch, int* Dest, int vmin = -1, int vmax = -1);
	static void LoadRange(CString * sName, CString * sValue, char * sSearch, int * vmin, int * vmax);
	static void CheckVar(CString * sName, CString * sValue, char * sSearch, double * Dest);
	static void LoadVar(CString * sName, CString * sValue, char * sSearch, CString * Dest);
	static void LoadVarInstr(CString * sName, CString * sValue, char * sSearch, vector<int>& Dest);
	static void LoadNote(CString * sName, CString * sValue, char * sSearch, int * Dest);
	static CString FormatTime(int sec);
	static bool dirExists(CString dirName_in);
	static bool fileExists(CString dirName_in);
	static bool nodeExists(CString dirName_in);
	static CString fname_from_path(CString path);
	static CString bname_from_path(CString path);
	static CString dir_from_path(CString path);
	static CString GetNoteName(int n);
	static int GetNoteI(CString st);
	static int GetInstrumentI(CString st);

protected:
	// Mathematics
	int randbw(int n1, int n2); // Random between two numbers
	double rand01(); // Random double between 0 and 1
	// Random
	void isaac();
	void randinit(int flag);
	unsigned int rand2();

public:
	CGLib();
	virtual ~CGLib();

	void InitRandom();
	void TestRandom(); // Tests random generator
	static void WriteLog(int i, CString * pST);
	static void WriteLog(int i, CString st);

public:
	// Thread interface
	static HWND m_hWnd;
	static UINT WM_DEBUG_MSG;
	static int can_send_log; // If thread can send log to MainFrame (disabled OnClose)
	UINT WM_GEN_FINISH;
	timed_mutex mutex_output;
	int need_exit = 0; // If thread needs to exit due to generation abort

  // Random generator
	ub4 randrsl[256], randcnt; // external results
	ub4 mm[256];  // internal state
	ub4 aa = 0, bb = 0, cc = 0;
	int cur_rand = 0, cur_rand2 = 0;
};
