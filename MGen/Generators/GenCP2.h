#pragma once
#include "GenCP1.h"

class CGenCP2 :
	public CGenCP1
{
public:
	CGenCP2();
	~CGenCP2();

	void Generate() override;

protected:
	void LoadConfigLine(CString * sN, CString * sV, int idata, float fdata);

};

