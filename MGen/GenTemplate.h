#pragma once

const int GAlgNum = 2;

const CString GAlgName[] = {
	L"No algorithm selected", // 0
	L"Cantus firmus 1", // 1
	L"Cantus firmus 2" // 2
};

class CGenTemplate
{
public:
	CGenTemplate();
	virtual ~CGenTemplate();
	virtual void Generate();
};
