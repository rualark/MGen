#pragma once

#include "GenCF1.h"

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
	int pre_bad = 3; // How many notes to recalculate before rule violation
	int post_bad = 8; // How many notes to recalculate after rule violation
	double step_penalty = 1; // Penalty for adding one more changing step while correcting cantus
	double pitch_penalty = 1; // Penalty for changing note one more diatonic step while correcting cantus

};

