#pragma once

#include "stdafx.h"

class CEnumWindows
{
public:
	CEnumWindows(HWND hWndParent);
	virtual ~CEnumWindows();

	HWND Window(int nIndex) const { return (HWND)m_apWindowHandles.GetAt(nIndex); }
	int Count() const { return m_apWindowHandles.GetCount(); }

private:
	static BOOL CALLBACK TheEnumProc(HWND hWnd, LPARAM lParam);

	CPtrArray m_apWindowHandles;
};

