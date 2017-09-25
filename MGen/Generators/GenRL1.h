#pragma once
#include "../GLibrary/GMidi.h"

class CGenRL1 :
	public CGMidi
{
public:
	CGenRL1();
	~CGenRL1();
	void LoadConfigLine(CString * sN, CString * sV, int idata, float fdata);
	void Generate() override;
	unsigned int GetVaue();
	int CGenRL1::GetVaueInt(int iMin, int iMax, int iType, int iDepth = 1);
private:
	int min_dyn; /*= 90*/
	int max_dyn;  /*= 120*/
	int min_tempo;
	int max_tempo;
};
