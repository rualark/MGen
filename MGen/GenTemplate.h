#pragma once

const char* GType[] = { 
	"No algorithm selected" // 0
	"Cantus firmus 1", // 1
	"Cantus firmus 2" // 2
};

class CGenTemplate
{
public:
	CGenTemplate();
	virtual ~CGenTemplate();
	virtual void Generate();
};

