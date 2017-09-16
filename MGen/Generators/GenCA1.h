#pragma once

#include "GenCF1.h"

class CGenCA1 :
	public CGenCF1
{
public:
	CGenCA1();
	~CGenCA1();

	void Generate() override;
	
protected:
	void LoadConfigLine(CString * sN, CString * sV, int idata, float fdata);
	void GetCPKey();
	void GetCPKey3(vector<int>& key_miss, int & min_miss, int & min_key, int minor_cur, int diatonic_repeat_check);
	int GetCPKey2(int & tonic_cur, CString & ext_st, int minor_cur);
	void CreateScanMatrix(int i);
	void SendCorrections(int i, long long time_start);

	void ParseExpect();

	inline void CheckSASEmulatorFlags();

	void ConfirmExpect();

	int corrections = 3; // Number of corrections to show
	int pre_bad = 6; // How many notes to recalculate before rule violation
	int post_bad = 8; // How many notes to recalculate after rule violation

	int step0 = 0; // Start of current cantus
	int step1 = 0; // Stop of current cantus

};
