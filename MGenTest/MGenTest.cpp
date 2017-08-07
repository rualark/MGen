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

void Run(CString fname, CString par, int delay) {
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
}

void Log(CString st, int level = 0) {
	cout << st;
	logfile << st;
	if (ci && level > 0) {
		CString cat;
		if (level == 1) cat = "Information";
		if (level == 2) cat = "Warning";
		if (level == 3) cat = "Error";
		CString par = "AddMessage \"" + st + "\" -Category " + cat;
		Run("appveyor", par, 1000);
	}
}

CString file(CString fname) {
	CString st, st2;
	ifstream fs;
	// Check file exists
	if (!CGLib::fileExists(fname)) {
		cout << "Not found file " << fname << "\n";
	}
	fs.open(fname);
	char pch[2550];
	while (fs.good()) {
		// Get line
		fs.getline(pch, 2550);
		st2 = pch;
		if (st2 != "") {
			st += "- " + st2 + "\n";
		}
	}
	fs.close();
	return st;
}

void ClearBuffer() {
	fstream fs;
	fs.open("autotest\\buffer.log", ios::out);
	fs.close();
}

void PublishTest(CString tname, int result, int tpassed) {
	CString st;
	CString st2;
	st2.Format("%s: code %d in %d ms\n", tname, result, tpassed);
	if (result) {
		nRetCode = 2;
		Log(st2, 3);
	}
	else {
		Log(st2, 1);
	}

	// Show errors
	CString errors = file("autotest/buffer.log");
	cout << errors;

	if (ci) {
		CString cat = "Passed";
		if (result) cat = "Failed";
		st.Format("UpdateTest \"%s\" -Framework MSTest -FileName MGen.exe -Duration %d -Outcome %s -ErrorMessage \"%d\"", tname, tpassed, cat, result);
		Run("appveyor", st, 1000);
		// Send errors separately in case of command line overflow
		st.Format("UpdateTest \"%s\" -Framework MSTest -FileName MGen.exe -Duration %d -Outcome %s -ErrorMessage \"%d\" -ErrorStackTrace \"%s\"", tname, tpassed, cat, result, errors);
		Run("appveyor", st, 1000);
	}

}

void LoadConfig() {
	milliseconds time_start, time_stop;
	CString fname = "autotest\\test.txt";
	// Check file exists
	if (!CGLib::fileExists(fname)) {
		cout << "Not found file " << fname << "\n";
	}
	ifstream fs;
	fs.open(fname);
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
			ClearBuffer();
			if (ci) Run("appveyor", "AddTest \"" + st + "\" -Framework MSTest -FileName MGen.exe -Outcome Running", 1000);
			Log("Starting test config: " + st + "\n");
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
			if (WaitForSingleObject(sei.hProcess, 60000) == WAIT_TIMEOUT) {
				Log(st + ": Timeout waiting for process\n", 3);
				exit(1);
			}

			time_stop = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
			passed = static_cast<int>((time_stop - time_start).count());
			GetExitCodeProcess(sei.hProcess, ecode);

			PublishTest(st, *ecode, passed);
		}
	}
	delete ecode;
	fs.close();
}

int test() {
	if (getenv("APPVEYOR_PROJECT_NAME") != NULL) {
		ci = 1;
	}
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
		_getch();
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
