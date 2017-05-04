#pragma once
#include "../GLibrary/GMidi.h"

class CGenRS1 :
	public CGMidi
{
public:
	CGenRS1();
	~CGenRS1();
	void LoadConfigLine(CString * sN, CString * sV, int idata, float fdata);
	void Generate() override;

	int min_tempo = 160;
	int max_tempo = 200;
	int min_len = 1;
	int max_len = 5;
	int min_note = 60;
	int max_note = 71;
	int note_step = 3;
	int min_dyn = 60;
	int max_dyn = 110;
	int dyn_step = 10;
	int pause_freq = 10;
};

