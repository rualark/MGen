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
	int GetCantusKey2(vector<int>& cc, int & tonic_cur, int minor_cur);
	void CreateScanMatrix(int i);
	void SWA(int i);
	void SendCorrections(int i, milliseconds time_start);

	int corrections = 3; // Number of corrections to show
	int pre_bad = 6; // How many notes to recalculate before rule violation
	int post_bad = 8; // How many notes to recalculate after rule violation
	float step_penalty = 3; // Penalty for adding one more changing step while correcting cantus
	float pitch_penalty = 1; // Penalty for changing note one more diatonic step while correcting cantus

	// Local
	vector <float> dpenalty; // Penalty in terms of difference from user melody
	vector <long> cids;
	float dpenalty_min;
	// These are temporary vectors for removing duplicates
	vector<vector<int>> clib2; // Library of cantus
	vector <float> rpenalty2;
	vector<int> c; // Local cantus
};

