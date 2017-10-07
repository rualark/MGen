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
	
	int min_pitch;
	int max_pitch;
	int distr_type_pitch;
	int distr_depth_pitch;

	int min_dyn;
	int max_dyn;
	int distr_type_dyn;
	int distr_depth_dyn;

	int min_tempo = 160;
	int max_tempo = 200;
	int distr_type_tempo;
	int distr_depth_tempo;

	int min_len;
	int max_len;
	int distr_type_len;
	int distr_depth_len;
	int note_step = 3;
	int pause_freq = 10;
};
