// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// MGenServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MGenServer.h"
#include "../MGen/GLibrary/GLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// The one and only application object
CWinApp theApp;

using namespace std;

int nRetCode = 0;

vector <int> aChild; // If state process should be automatically restarted on crash
vector <int> rChild; // Is state process running?
vector <CString> nChild; // Child process name
vector <CString> fChild; // Child process folder
vector <CString> pChild; // Child process parameter string

void Run(CString fname, CString par, int delay) {
	DWORD ecode;
	SHELLEXECUTEINFO sei = { 0 };
	sei.cbSize = sizeof(SHELLEXECUTEINFO);
	sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
	sei.hwnd = NULL;
	sei.lpVerb = NULL;
	sei.lpFile = fname;
	sei.lpParameters = par;
	sei.lpDirectory = NULL;
	sei.nShow = SW_SHOWNORMAL;
	sei.hInstApp = NULL;
	ShellExecuteEx(&sei);
	WaitForSingleObject(sei.hProcess, delay);
	if (!GetExitCodeProcess(sei.hProcess, &ecode)) ecode = 102;
	if (ecode != 0 && ecode != STILL_ACTIVE) { // 259
		cout << "Exit code " << ecode << ": " << fname << " " << par << "\n";
	}
}

HANDLE GetProcessHandle(CString pname) {
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (Process32First(snapshot, &entry) == TRUE)	{
		while (Process32Next(snapshot, &entry) == TRUE)	{
			if (stricmp(entry.szExeFile, "Reaper.exe") == 0)	{
				return OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
				//WaitForSingleObject(hProcess, 100000);
			}
		}
	}
	CloseHandle(snapshot);
	return NULL;
}

void RestartChild(int c) {
	Run(fChild[c] + "\\" + nChild[c], pChild[c], 200);
}

void CheckChilds() {
	for (int c = 0; c < nChild.size(); ++c) {
		HANDLE hProcess = GetProcessHandle(nChild[c]);
		if (hProcess == NULL) {
			rChild[c] = 0;
			if (aChild[c]) {
				RestartChild(c);
			}
		}
		else rChild[c] = 1;
		CloseHandle(hProcess);
	}
}

void LoadConfig()
{
	TCHAR buffer[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, buffer);
	CString current_dir = string(buffer).c_str();
	cout << "Current dir: " + current_dir + "\n";

	CString st, st2, st3;
	ifstream fs;
	CString fname = "server\\server.pl";
	// Check file exists
	if (!CGLib::fileExists(fname)) {
		cout << "LoadConfig cannot find file: " << fname << "\n";
		nRetCode = 3;
		return;
	}
	fs.open(fname);
	char pch[2550];
	int pos = 0;
	int i = 0;
	while (fs.good()) {
		i++;
		// Get line
		fs.getline(pch, 2550);
		st = pch;
		st.Replace("\"", "");
		// Remove unneeded
		pos = st.Find("#");
		// Check if it is first symbol
		if (pos == 0)	st = st.Left(pos);
		pos = st.Find(" #");
		// Check if it is after space
		if (pos > -1)	st = st.Left(pos);
		st.Trim();
		pos = st.Find("=");
		if (pos != -1) {
			// Get variable name and value
			st2 = st.Left(pos);
			st3 = st.Mid(pos + 1);
			st2.Trim();
			st3.Trim();
			st2.MakeLower();
			// Load general variables
			int idata = atoi(st2);
			float fdata = atof(st3);
			CGLib::parameter_found = 0;
			if (st2 == "childprocess") {
				nChild.push_back(st3);
				aChild.resize(nChild.size());
				rChild.resize(nChild.size());
				fChild.resize(nChild.size());
				pChild.resize(nChild.size());
			}
			CGLib::CheckVar(&st2, &st3, "childrestart", &aChild[aChild.size() - 1], 0, 1);
			CGLib::LoadVar(&st2, &st3, "childpath", &fChild[fChild.size() - 1]);
			CGLib::LoadVar(&st2, &st3, "childparams", &pChild[pChild.size() - 1]);
			if (!CGLib::parameter_found) {
				cout << "Unrecognized parameter '" + st2 + "' = '" + st3 + "' in file " + fname << "\n";
			}
			if (nRetCode) break;
		}
	}
	fs.close();
	cout << "LoadConfig loaded " << i << " lines from " << fname << "\n";
}

int main() {
  HMODULE hModule = ::GetModuleHandle(nullptr);

  if (hModule != nullptr) {
    // initialize MFC and print and error on failure
    if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0)) {
      // TODO: change error code to suit your needs
      wprintf(L"Fatal Error: MFC initialization failed\n");
			return 1;
		}
  }
  else {
    // TODO: change error code to suit your needs
    wprintf(L"Fatal Error: GetModuleHandle failed\n");
		return 2;
	}

	LoadConfig();
	if (nRetCode) {
		cout << "Press any key to continue... ";
		_getch();
		return nRetCode;
	}
	for (;;) {
		CheckChilds();
		Sleep(1000);
	}
	cout << "Press any key to continue... ";
	_getch();
	return nRetCode;
}
