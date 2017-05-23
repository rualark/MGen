#pragma once

#include "GenCF1.h"

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
	int GetCantusKey2(vector<int>& cc, int & tonic_cur, CString & st, int minor_cur);
	void CreateScanMatrix(int i);
	void SendCorrections(int i, milliseconds time_start);

	int corrections = 3; // Number of corrections to show
	int pre_bad = 6; // How many notes to recalculate before rule violation
	int post_bad = 8; // How many notes to recalculate after rule violation

};

