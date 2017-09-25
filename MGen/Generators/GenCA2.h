#pragma once
#include "GenCP1.h"
class CGenCA2 :
	public CGenCP1
{
public:
	CGenCA2();
	~CGenCA2();

	void Generate() override;

protected:
	void MergeCantus();
	void ExplodeCP();
	void LinkCpPauses();
	void DetectSpecies();
	void LoadConfigLine(CString * sN, CString * sV, int idata, float fdata);
	void SendCorrectionsCP(int i, long long time_start);
	
	vector<int> min_vlen; // [v]
	vector<int> max_vlen; // [v]
};
