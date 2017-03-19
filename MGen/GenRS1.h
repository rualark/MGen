#pragma once
#include "GenTemplate.h"
class CGenRS1 :
	public CGenTemplate
{
public:
	CGenRS1();
	~CGenRS1();
	void Generate() override;
};

