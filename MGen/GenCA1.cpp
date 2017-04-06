#include "stdafx.h"
#include "GenCA1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MAX_FLAGS 39
#define FLAG(id, i) { flags[0] = 0; flags[id] = 1; nflags[i][nflagsc[i]] = id; nflagsc[i]++; }

const CString FlagName[MAX_FLAGS] = {
	"Strict", // 0
	"Seventh", // 1
	"Tritone resolved", // 2 
	"Many leaps", // 3 
	"Long smooth", // 4 
	"Long line", // 5 
	"Two 3rds", // 6 
	"Late <6th resolution", // 7 
	"Leap back <5th", // 8 
	"Close repeat", // 9 
	"Stagnation", // 10 
	"Leap pre-late fill", // 11 
	"Multiple culminations", // 12 
	"2nd to last not D", // 13
	"3rd to last is CEG", // 14
	"3 letters in a row", // 15
	"4 letters in a row", // 16
	">4 letters in a row", // 17
	"4 step miss", // 18
	"5 step miss", // 19
	">5 step miss", // 20
	"Late culmination", // 21
	"Leap back >4th", // 22
	"Last leap", // 23
	"Unfilled leap", // 24
	"Many leaps+", // 25
	"Leap unresolved", // 26
	"Leap chain", // 27
	"Two 3rds after 6/8", // 28
	"Late >5th resolution", // 29
	"Prepared unresolved 3rd", // 30
	"Tritone unresolved", // 31
	"Tritone culmination", // 32
	"Leap to leap resolution", // 33
	"3rd to last is leading", // 34
	"Prepared unfilled 3rd", // 35
	"Outstanding repeat", // 36
	"Too wide range", // 37
	"Too tight range", // 38
};

const int SeverityFlag[MAX_FLAGS] = {
	0, // "Strict", // 0
	35, // "Prepared unfilled 3rd", // LEAP FILL
	30, // "Prepared unresolved 3rd", // LEAP RESOLUTION
	28, // "Two 3rds after 6/8", // LEAP RESOLUTION
	7, // "Late <6th resolution", // LEAP RESOLUTION
	8, // "Leap back <5th", // LEAP RESOLUTION 
	1, // "Seventh", // LEAPS
	11, // "Leap pre/late fill", // LEAP FILL 
	3, // "Many leaps", // LEAPS 
	14, // "3rd to last is CEG", // HARMONY
	23, // "Last leap", // END
	2, // "Tritone resolved", // TRITONE 
	15, // "3 letters in a row", // HARMONY
	24, // "Unfilled leap", // LEAP FILL
	33, // "Leap to leap resolution", // LEAP RESOLUTION
	6, // "Two 3rds", // LEAP RESOLUTION 
	18, // "4 step miss", // HARMONY
	36, // "Outstanding repeat", // REPEATS

	36, // "Too wide range", // RANGE
	37, // "Too tight range", // RANGE
	4, // "Long smooth", // LEAPS 
	5, // "Long line", // LEAPS 
	9, // "Close repeat", // REPEATS 
	10, // "Stagnation", // REPEATS 
	12, // "Multiple culminations", // REPEATS 
	13, // "2nd to last not D", // HARMONY
	16, // "4 letters in a row", // HARMONY
	17, // ">4 letters in a row", // HARMONY
	19, // "5 step miss", // HARMONY
	20, // ">5 step miss", // HARMONY
	21, // "Late culmination", // END
	22, // "Leap back >4th", // LEAP RESOLUTION
	25, // "Many leaps+", // LEAPS
	26, // "Leap unresolved", // LEAP RESOLUTION
	27, // "Leap chain", // LEAP RESOLUTION
	29, // "Late >5th resolution", // LEAP RESOLUTION
	31, // "Tritone unresolved", // TRITONE
	32, // "Tritone culmination", // TRITONE
	34, // "3rd to last is leading", // HARMONY
};

const Color FlagColor[] = {
	Color(0, 100, 100, 100), // 0 S
};

CGenCA1::CGenCA1()
{
}

CGenCA1::~CGenCA1()
{
}

void CGenCA1::LoadConfigLine(CString* sN, CString* sV, int idata, double fdata)
{
	LoadVar(sN, sV, "midi_file", &midi_file);
	CheckVar(sN, sV, "min_interval", &min_interval);
	CheckVar(sN, sV, "max_interval", &max_interval);
	CheckVar(sN, sV, "fill_steps_mul", &fill_steps_mul);
	CheckVar(sN, sV, "max_repeat_mul", &max_repeat_mul);
	CheckVar(sN, sV, "max_smooth_direct", &max_smooth_direct);
	CheckVar(sN, sV, "max_smooth", &max_smooth);
	CheckVar(sN, sV, "max_leaps", &max_leaps);
	CheckVar(sN, sV, "max_leaps2", &max_leaps2);
	CheckVar(sN, sV, "max_leap_steps", &max_leap_steps);
	CheckVar(sN, sV, "repeat_steps", &repeat_steps);
	CheckVar(sN, sV, "stag_notes", &stag_notes);
	CheckVar(sN, sV, "stag_note_steps", &stag_note_steps);
	LoadRange(sN, sV, "tempo", &min_tempo, &max_tempo);
	CheckVar(sN, sV, "show_severity", &show_severity);
}

void CGenCA1::Generate()
{
	InitCantus();
	LoadCantus(midi_file);
	if (cantus.size() < 1) return;
	//for (int i = 0; i < cantus.size(); i++) FlagCantus(cantus[i]);
	// This flag is needed to prevent flag skipping
	calculate_stat = 1;
	for (int i = 1; i <= cantus.size(); i++) ScanCantus(i);
	Adapt(0, t_generated - 1);
	t_sent = t_generated;
}
