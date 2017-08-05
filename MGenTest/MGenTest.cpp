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

void Log(CString st) {
	cout << st;
	logfile << st;
	::ShellExecute(GetDesktopWindow(), "open", "appveyor", "AddMessage \"" + st + "\"", NULL, SW_SHOWNORMAL);
}

void LoadConfig() {
	ifstream fs;
	fs.open("autotest\\test.txt");
	LPDWORD ecode = new DWORD;
	CString st, st2;
	char pch[2550];
	int pos = 0;
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
			ShellExecuteEx(&sei);
			if (WaitForSingleObject(sei.hProcess, 10000) == WAIT_TIMEOUT) {
				Log("Timeout waiting for process\n");
				return;
			}

			GetExitCodeProcess(sei.hProcess, ecode);
			st2.Format("%d", *ecode);
			Log("Exit code: " + st2 + "\n");
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
    int nRetCode = 0;

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
