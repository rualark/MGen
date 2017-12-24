#include "EnumWindows.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CEnumWindows::CEnumWindows(HWND hWndParent) {
	EnumChildWindows(hWndParent, TheEnumProc, reinterpret_cast<LPARAM>(this));
}

CEnumWindows::~CEnumWindows() {
}

BOOL CALLBACK CEnumWindows::TheEnumProc(HWND hWnd, LPARAM lParam) {
	CEnumWindows *pEnumWindows = reinterpret_cast<CEnumWindows *>(lParam);
	pEnumWindows->m_apWindowHandles.Add((void *)hWnd);
	return TRUE;
}