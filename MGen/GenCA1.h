#pragma once

#include "GenCF1.h"

// This value has to be greater than any penalty. May need correction if step_penalty or pitch_penalty changes
#define MAX_PENALTY 10000000.0

class CGenCA1 :
	public CGenCF1
{
public:
	CGenCA1();
	~CGenCA1();

	void Generate() override;
	void LoadConfigLine(CString * sN, CString * sV, int idata, double fdata);
	
	CString midi_file;

protected:
	int corrections = 3; // Number of corrections to show
	int pre_bad = 6; // How many notes to recalculate before rule violation
	int post_bad = 8; // How many notes to recalculate after rule violation
	double step_penalty = 3; // Penalty for adding one more changing step while correcting cantus
	double pitch_penalty = 1; // Penalty for changing note one more diatonic step while correcting cantus
	int correct_transpose = 12; // Transpose corrected canti semitones up for display
	int algorithm = 1; // Type of algorithm to use for searching of corrected variants
};

