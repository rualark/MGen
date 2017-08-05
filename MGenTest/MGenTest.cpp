// MGenTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MGenTest.h"
#include "../MGen/GLibrary/GLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// The one and only application object

CWinApp theApp;

using namespace std;

CString current_dir;
ofstream logfile;
int ci = 0;
int nRetCode = 0;

void Log(CString st, int level = 0) {
	cout << st;
	logfile << st;
	if (ci && level > 0) {
		CString cat;
		if (level == 1) cat = "Information";
		if (level == 2) cat = "Warning";
		if (level == 3) cat = "Error";
		CString par = "AddMessage \"" + st + "\" -Category " + cat;
		SHELLEXECUTEINFO sei = { 0 };
		sei.cbSize = sizeof(SHELLEXECUTEINFO);
		sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
		sei.hwnd = NULL;
		sei.lpVerb = NULL;
		sei.lpFile = "appveyor";
		sei.lpParameters = par;
		sei.lpDirectory = NULL;
		sei.nShow = SW_SHOWNORMAL;
		sei.hInstApp = NULL;
		ShellExecuteEx(&sei);
		WaitForSingleObject(sei.hProcess, 1000);
	}
}

void LoadConfig() {
	milliseconds time_start, time_stop;
	ifstream fs;
	fs.open("autotest\\test.txt");
	LPDWORD ecode = new DWORD;
	CString st, st2;
	char pch[2550];
	int pos = 0;
	int passed;
	while (fs.good()) {
		fs.getline(pch, 2550);
		st = pch;
		pos = st.Find("#");
		if (pos != -1) st = st.Left(pos);
		st.Trim();
		if (st.GetLength()) {
			Log("Starting test config: " + st + "\n");
			//::ShellExecute(GetDesktopWindow(), "open", "MGen", "-test " + st, NULL, SW_SHOWNORMAL);
			st2 = "-test " + st;
			SHELLEXECUTEINFO sei = { 0 };
			sei.cbSize = sizeof(SHELLEXECUTEINFO);
			sei.fMask = SEE_MASK_NOCLOSEPROCESS;
			sei.hwnd = NULL;
			sei.lpVerb = NULL;
			sei.lpFile = "MGen.exe";
			sei.lpParameters = st2;
			sei.lpDirectory = NULL;
			sei.nShow = SW_SHOW;
			sei.hInstApp = NULL;
			time_start = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
			ShellExecuteEx(&sei);
			if (WaitForSingleObject(sei.hProcess, 10000) == WAIT_TIMEOUT) {
				Log(st + ": Timeout waiting for process\n", 3);
				exit(1);
			}

			time_stop = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
			passed = (time_stop - time_start).count();
			GetExitCodeProcess(sei.hProcess, ecode);
			st2.Format("%s: code %d in %d ms\n", st, *ecode, passed);
			if (*ecode) {
				nRetCode = 2;
				Log(st2, 3);
			}
			else {
				Log(st2, 1);
			}
		}
	}
	delete ecode;
	fs.close();
}

int test() {
	if (getenv("APPVEYOR_PROJECT_NAME") != NULL) ci = 1;
	logfile.open("autotest\\test.log", ios_base::app);

	TCHAR buffer[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, buffer);
	current_dir = string(buffer).c_str();
	Log("Current dir: " + current_dir + "\n");

	LoadConfig();
	logfile.close();
	// Do not pause if continuous integration
	if (!ci) {
		cout << "Press any key to continue... ";
		getch();
	}
	return 0;
}

int main()
{
    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // initialize MFC and print and error on failure
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: change error code to suit your needs
            wprintf(L"Fatal Error: MFC initialization failed\n");
            nRetCode = 1;
        }
        else
        {
					test();
				}
    }
    else
    {
        // TODO: change error code to suit your needs
        wprintf(L"Fatal Error: GetModuleHandle failed\n");
        nRetCode = 1;
    }

    return nRetCode;
}
