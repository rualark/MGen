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
	void Generate() override;

protected:
	void MergeCantus();
	void ShrinkCantus2();
	void ShrinkCP();
	void GetVlen();
	void ExplodeCP();
	void LinkCpPauses();
	void DetectSpecies();
	void ApplyMidiOutSpecies();
	void LoadConfigLine(CString * sN, CString * sV, int idata, float fdata);
	void SendCorrectionsCP(int i, long long time_start);
	
	vector<int> min_vlen; // [v]
	vector<int> max_vlen; // [v]

	int species_applied = 0; // If species midi out multiplier already applied

	// Save configuration parameters
	int npm_conf, species_conf, cantus_high_conf;
};
