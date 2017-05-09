#pragma once
#include "GenCF1.h"

class CGenCP1 :
	public CGenCF1
{
public:
	CGenCP1();
	~CGenCP1();

	void Generate() override;

protected:
	void LoadConfigLine(CString * sN, CString * sV, int idata, float fdata);

};