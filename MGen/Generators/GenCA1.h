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
	
	CString midi_file;

protected:
	void LoadConfigLine(CString * sN, CString * sV, int idata, float fdata);

	void GetCantusKey(vector<int>& cc);
	int GetCantusKeyMajor(vector<int>& cc, int & tonic_cur, int & minor_cur);
	int GetCantusKeyMinor(vector<int>& cc, int & tonic_cur, int & minor_cur);

	int corrections = 3; // Number of corrections to show
	int pre_bad = 6; // How many notes to recalculate before rule violation
	int post_bad = 8; // How many notes to recalculate after rule violation
	float step_penalty = 3; // Penalty for adding one more changing step while correcting cantus
	float pitch_penalty = 1; // Penalty for changing note one more diatonic step while correcting cantus
	int fullscan_max = 7; // Maximum steps length to full scan. If melody is longer, use SWA
	int approximations = 30; // Maximum number of approximations to run if penalty decreases
	int swa_steps = 6; // Size of Sliding Window Approximation algorithm window in steps
};

