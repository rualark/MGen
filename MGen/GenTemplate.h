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

	static UINT GenThread(LPVOID pParam);
	HWND m_hWnd;
	//static UINT WM_GEN_FINISH;

protected:
	int v_cnt=1; // Voice count
	int t_count = 96000; // Timeslot count
};
