#pragma once

#include "GMidi.h"

class CGenCA1 :
	public CGMidi
{
public:
	CGenCA1();
	~CGenCA1();

	void Generate() override;
	void LoadConfigLine(CString * sN, CString * sV, int idata, double fdata);
	
	CString midi_file;

protected:
	void FlagCantus(vector<unsigned char>& c);
	
	// Local variables
	int step = 0; // Current step

	// Parameters
	int min_interval = 1; // Minimum diatonic interval in cantus (7 = octave)
	int max_interval = 5; // Maximum diatonic interval in cantus (7 = octave)
	int fill_steps_mul = 2; // Multiply number of notes between leap notes to get steps for filling
	int max_repeat_mul = 2; // Allow repeat of X notes after at least X*max_repeat_mul steps if beats are different
	int max_smooth_direct = 5; // Maximum linear movement in one direction allowed (in steps)
	int max_smooth = 7; // Maximum linear movement allowed (in steps)
	int max_leaps = 2; // Maximum allowed max_leaps during max_leap_steps
	int max_leaps2 = 3; // Maximum allowed max_leaps2 during max_leap_steps for Many leaps+
	int max_leap_steps = 7;
	int stag_notes = 2; // Maximum allowed stag_notes (same notes) during stag_note_steps
	int stag_note_steps = 7;
	int min_tempo = 110;
	int max_tempo = 120;
	int show_severity = 0; // =1 to show severity in square brackets in comments to notes (also when exporting to MIDI file)
	int repeat_steps = 8; // Prohibit repeating of 3 notes closer than repeat_steps between first notes (if beats are same)
};

