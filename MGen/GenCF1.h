#pragma once
#include "GenTemplate.h"

const CString FlagName[] = {
	"Strict", // 0 S
	"Seventh", // 1 p
	"Tritone", // 2 t
	"Many leaps", // 3 j
	"Long smooth", // 4 o
	"Long line", // 5 l
	"Leaps chain", // 6 c
	"Late leap resolution", // 7 a
	"Leap back", // 8 r
	"Close repeat", // 9 d
	"Stagnation", // 10 g
	"Unfilled leap", // 11 f
	"Multiple culminations" // 12 m
};

// 0  S - strict
// 1  p - sept
// 2  t - tritone
// 3  j - too many leaps
// 4  o - too long smooth movement
// 5  l - too long smooth movement in one direction (linear)
// 6  c - chain of leaps in one direction
// 7  a - leap is resolved after a second note
// 8  r - leap returns to same note
// 9  d - two notes repeat in contact
// 10 g - stagnation on one note
// 11 f - leap is not filled
// 12 m - multiple culminations

class CGenCF1 :
	public CGenTemplate
{
public:
	CGenCF1();
	~CGenCF1();
	void Generate() override;
	void LoadConfigLine(CString * sN, CString * sV, int idata, double fdata);

protected:
	// Parameters
	int max_interval = 5; // Maximum diatonic interval in cantus (7 = octave)
	int c_len = 9; // Number of measures in each cantus. Usually 9 to 11
	int first_note = 72; // Starting note of each cantus
	int last_diatonic_int = 0; // Diatonic interval between first and last note
	CString accept = "spTjolcardgfm"; // Each upper case letter allows showing canti with specific properties:
	int max_unfill_steps = 7; // Number of steps to check filling
	int max_repeat_mul = 2; // Allow repeat of X notes after at least X*max_repeat_mul steps if beats are different
	int max_smooth_direct = 5; // Maximum linear movement in one direction allowed (in steps)
	int max_smooth = 7; // Maximum linear movement allowed (in steps)
	int max_leaps = 2; // Maximum allowed max_leaps during max_leap_steps
	int max_leap_steps = 7;
	int stag_notes = 2; // Maximum allowed stag_notes (same notes) during stag_note_steps
	int stag_note_steps = 7;
	int min_tempo = 100;
	int max_tempo = 130;
};
