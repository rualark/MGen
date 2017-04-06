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
	
};

