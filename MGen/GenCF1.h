#pragma once
#include "GenTemplate.h"
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
	int first_note = 60; // Starting note of each cantus
	int allow_tritone = 0; // If tritone can be used (correctly prepared and released)
	int allow_sept = 0; // If sept can be used (correctly prepared and released)
	int max_leaps = 2; // Maximum allowed max_leaps during max_leap_steps
	int max_leap_steps = 5;
	int stag_notes = 2; // Maximum allowed stag_notes (same notes) during stag_note_steps
	int stag_note_steps = 5;
	int min_tempo = 160;
	int max_tempo = 200;
};
