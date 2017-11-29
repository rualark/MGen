#pragma once
#include "GenCP1.h"
class CGenCA2 :
	public CGenCP1
{
public:
	CGenCA2();
	~CGenCA2();

	void FixUnisonPause();
	void FixStartPause();
	void ReduceBetween();
	int GetCantusVoice();
	void FindBestPause();
	void Generate() override;

protected:
	void MergeCantus();
	void ShrinkCantus2();
	void ShrinkCP();
	void GetVlen();
	void GetSusCount();
	void CheckCantusLonger();
	void SplitSpecies();
	void ExplodeCP();
	void LinkCpPauses();
	void DetectSpecies();
	void ApplyMidiOutSpecies();
	void LoadConfigLine(CString * sN, CString * sV, int idata, float fdata);
	void SendCorrectionsCP(int i, long long time_start);
	
	vector<int> min_vlen; // [v] Minimum note length in voice
	vector<int> max_vlen; // [v] Maximum note length in voice (except last note)
	vector<double> med_vlen; // [v] Medium note length in voice (except last note)

	int species_applied = 0; // If species midi out multiplier already applied

	// Save configuration parameters
	int npm_conf = 0;
	int species_conf = 0;
	int cantus_high_conf = 0;
};
