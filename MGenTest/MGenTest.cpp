// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// MGenTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MGenTest.h"
#include "../MGen/GLibrary/GLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
int is_debug = 1;
#else
int is_debug = 0;
#endif

// The one and only application object

CWinApp theApp;

using namespace std;

#define MAX_FLAGS 1000

CString current_dir;
ofstream logfile;
int continuous_integration = 0;
int nRetCode = 0;
int debug_level = 0;
vector<CString> errorMessages;
vector<CString> lyLogs;
vector<CString> lyConfigs;
CString pname;

// Default time to start aborting generation
int wait_sec_default = 60;
int wait_sec = wait_sec_default;
// Default time to consider generation timeout
int wait_sec2_default = 120;
int wait_sec2 = wait_sec2_default;

void InitErrorMessages() {
	errorMessages.resize(1000);
	errorMessages[0] = "OK";
	errorMessages[10] = "MGen detected critical errors during run";
	errorMessages[11] = "MGen generator freeze on exit - possible error in generator";
	errorMessages[100] = "GetExitCodeProcess error (for MGen.exe)";
	errorMessages[101] = "MGen process did not exit correctly - possible crash";
	errorMessages[102] = "GetExitCodeProcess error";
}

CString GetErrorMessage(int e) {
	if (e < errorMessages.size()) return errorMessages[e];
	else return "";
}

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
	if (ecode != 0 && ecode != 259) {
		nRetCode = 3;
		cout << "Exit code " << ecode << ": " << fname << " " << par << "\n";
	}
}

void Log(CString st, int level = 0) {
	cout << st;
	logfile << st;
	if (continuous_integration && level > 0) {
		CString cat;
		if (level == 1) cat = "Information";
		if (level == 2) cat = "Warning";
		if (level == 3) cat = "Error";
		CString par = "AddMessage \"" + st + "\" -Category " + cat + " >> autotest\\run.log 2>&1";
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
		if (!st2.IsEmpty()) {
			st += "> " + st2 + "\n";
		}
	}
	fs.close();
	return st;
}

void ClearBuffer() {
	fstream fs;
	fs.open("autotest\\buffer.log", ios::out);
	fs.close();
	remove("autotest\\exit.log");
}

void PublishTest(CString tname, int result, int tpassed, CString params) {
	CString emes = GetErrorMessage(result);
	CString st;
	CString st2;
	st2.Format("%s: code %d (%s) in %d ms\n", tname, result, emes, tpassed);
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

	if (continuous_integration) {
		CString cat = "Passed";
		if (result) cat = "Failed";
		st.Format("UpdateTest \"%s\" -Framework MSTest -FileName MGen.exe -Duration %d -Outcome %s -ErrorMessage \"%d: %s\" >> autotest\\run.log 2>&1", 
			tname, tpassed, cat, result, emes);
		// First run is done without error stack to ensure that too long error stack will not prevent test from reporting
		//Run("appveyor", st, 1000);
		// Send errors separately in case of command line overflow
		st.Format("UpdateTest \"%s\" -Framework MSTest -FileName MGen.exe -Duration %d -Outcome %s -ErrorMessage \"%d: %s\" -StdOut \"MGen.exe %s\" -ErrorStackTrace \"%s\" >> autotest\\run.log 2>&1", 
			tname, tpassed, cat, result, emes, params, errors);
		Run("appveyor", st, 1000);
	}
}

void ParseLyLogs() {
	Sleep(1000);
	CString st;
	vector<CString> sv;
	int error;
	for (int i = 0; i < lyLogs.size(); ++i) 
		if (CGLib::fileExists("autotest\\ly\\" + lyLogs[i] + ".log")) {
			// Send log
			if (continuous_integration && debug_level > 0) {
				CString suffix = "-release";
#ifdef _DEBUG
				suffix = "-debug";
#endif
				suffix += "-" + CTime::GetCurrentTime().Format("%Y-%m-%d_%H-%M-%S");
				Run("appveyor", "PushArtifact autotest\\ly\\" + lyLogs[i] +
					".log -Verbosity Normal -Type Auto -FileName " + lyLogs[i] +
					suffix + ".log >> run.log 2>&1", 1000);
			}
			CGLib::read_file_sv("autotest\\ly\\" + lyLogs[i] + ".log", sv);
			CString errors;
			for (int x = 0; x < sv.size(); ++x) {
				error = 0;
				if (sv[x].Find(" error: ") > -1) error = 1;
				if (sv[x].Find(" warning: ") > -1) error = 2;
				if (sv[x].Find("Exited with return code") > -1) error = 3;
				if (error) {
					if (errors.GetLength() < 1000) errors += "- " + sv[x] + "\n";
					CGLib::AppendLineToFile("autotest\\ly.log", lyLogs[i] + ": " + sv[x] + "\n");
				}
			}
			if (continuous_integration) {
				CString cat = "Passed";
				if (!errors.IsEmpty()) cat = "Failed";
				st.Format("AddTest \"LY: %s\" -Framework MSTest -FileName MGen.exe -Duration 0 -Outcome %s >> autotest\\run.log 2>&1",
					lyLogs[i], cat);
				Run("appveyor", st, 1000);
				// Send errors separately in case of command line overflow
				st.Format("AddTest \"LY: %s\" -Framework MSTest -FileName MGen.exe -Duration 0 -Outcome %s -ErrorStackTrace \"%s\" >> autotest\\run.log 2>&1",
					lyLogs[i], cat, errors);
				Run("appveyor", st, 1000);
			}
		}
}

void LoadConfig() {
	long long time_start, time_stop;
	vector<CString> ast;
	CString fname = "autotest\\test.csv";
	// Check file exists
	if (!CGLib::fileExists(fname)) {
		cout << "Not found file " << fname << "\n";
	}
	// Clear expect.log
	remove("autotest\\analysis.log");
	remove("autotest\\flags.log");
	remove("autotest\\expect.log");
	remove("autotest\\sas-emu.log");
	remove("autotest\\cor-ack.log");
	remove("autotest\\perf.log");
	// Clear run.log
	fstream fs;
	fs.open("autotest\\ly.log", ios::out);
	fs.close();
	fs.open("autotest\\run.log", ios::out);
	fs.close();
	// Open file
	fs.open(fname);
	DWORD ecode;
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
			CGLib::Tokenize(st, ast, ";");
			pname = ast[0];
			wait_sec = wait_sec_default;
			wait_sec2 = wait_sec2_default;
			if (ast.size() > 1 && atoi(ast[1]) > 0) wait_sec = atoi(ast[1]);
			if (ast.size() > 2 && atoi(ast[2]) > 0) wait_sec2 = atoi(ast[2]);
			ClearBuffer();
			if (continuous_integration) Run("appveyor", "AddTest \"" + pname + "\" -Framework MSTest -FileName MGen.exe -Outcome Running >> autotest\\run.log 2>&1", 1000);
			// MGen.exe -test=5 configs\GenCA2\good-cp5.pl
			st2.Format("-test=%d %s", wait_sec, pname);
			Log("Starting test config: " + st2 + "\n");
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
			time_start = CGLib::time();
			ShellExecuteEx(&sei);
			if (WaitForSingleObject(sei.hProcess, wait_sec2 * 1000) == WAIT_TIMEOUT) {
				Log(pname + ": Timeout waiting for process\n", 3);
				exit(1);
			}

			time_stop = CGLib::time();
			passed = time_stop - time_start;
			if (!GetExitCodeProcess(sei.hProcess, &ecode)) ecode = 100;
			if (!CGLib::fileExists("autotest\\exit.log")) ecode = 101;

			if (!ecode) {
				CString pname2 = pname;
				pname2.Replace("configs\\", "");
				pname2.Replace(".pl", "");
				pname2.Replace("\\", "-");
				lyConfigs.push_back(pname);
				lyLogs.push_back(pname2);
			}

			PublishTest(pname, ecode, passed, st2);
		}
	}

	fs.close();
}

void PushArtifacts() {
	if (continuous_integration) {
		CString suffix = "-release";
#ifdef _DEBUG
		suffix = "-debug";
#endif
		suffix += "-" + CTime::GetCurrentTime().Format("%Y-%m-%d_%H-%M-%S");
		if (CGLib::fileExists("autotest\\fstat.csv"))
			Run("appveyor", "PushArtifact autotest\\fstat.csv -Verbosity Normal -Type Auto -FileName fstat" +
				suffix + ".csv", 1000);
		if (CGLib::fileExists("db\\expect.csv"))
			Run("appveyor", "PushArtifact db\\expect.csv -Verbosity Normal -Type Auto -FileName expect-db" +
				suffix + ".csv", 1000);
		if (CGLib::fileExists("autotest\\flags.log"))
			Run("appveyor", "PushArtifact autotest\\flags.log -Verbosity Normal -Type Auto -FileName flags" +
				suffix + ".log >> run.log 2>&1", 1000);
		if (CGLib::fileExists("autotest\\analysis.log"))
			Run("appveyor", "PushArtifact autotest\\analysis.log -Verbosity Normal -Type Auto -FileName analysis" +
				suffix + ".log >> run.log 2>&1", 1000);
		//if (CGLib::fileExists("autotest\\expect.log"))
		Run("appveyor", "PushArtifact autotest\\expect.log -Verbosity Normal -Type Auto -FileName expect" +
			suffix + ".log >> run.log 2>&1", 1000);
		//if (CGLib::fileExists("autotest\\sas-emu.log"))
		Run("appveyor", "PushArtifact autotest\\sas-emu.log -Verbosity Normal -Type Auto -FileName sas-emu" +
			suffix + ".log >> run.log 2>&1", 1000);
		// cor-ack log must exist only in release, because in debug it can be not enough time to be created
		if (CGLib::fileExists("autotest\\cor-ack.log") || !is_debug)
			Run("appveyor", "PushArtifact autotest\\cor-ack.log -Verbosity Normal -Type Auto -FileName cor-ack" +
				suffix + ".log >> run.log 2>&1", 1000);
		if (CGLib::fileExists("autotest\\ly.log"))
			Run("appveyor", "PushArtifact autotest\\ly.log -Verbosity Normal -Type Auto -FileName ly" +
				suffix + ".log >> run.log 2>&1", 1000);
		//if (CGLib::fileExists("autotest\\perf.log"))
		Run("appveyor", "PushArtifact autotest\\perf.log -Verbosity Normal -Type Auto -FileName perf" +
			suffix + ".log >> run.log 2>&1", 1000);
		if (CGLib::fileExists("autotest\\run.log"))
			Run("appveyor", "PushArtifact autotest\\run.log -Verbosity Normal -Type Auto -FileName run" +
				suffix + ".log", 1000);
	}
}

void ParseFlags() {
	vector<CString> sv;
	vector<int> fstat, fconfirmed;
	fstat.resize(MAX_FLAGS);
	fconfirmed.resize(MAX_FLAGS);
	if (!CGLib::fileExists("autotest\\flags.log")) return;
	CGLib::read_file_sv("autotest\\flags.log", sv);
	for (int i = 0; i < sv.size(); ++i) {
		int pos = sv[i].Find("[");
		if (pos == -1) continue;
		CString fid = sv[i].Mid(pos + 1, 10);
		pos = fid.Find("]");
		fid = fid.Left(pos);
		int fidi = atoi(fid);
		if (fidi <= 0) {
			nRetCode = 5;
			cout << "Flag ID is below 1: " << sv[i] << "\n";
			continue;
		}
		if (fidi >= fstat.size()) {
			nRetCode = 5;
			cout << "Flag ID is above maximum flag count " << fstat.size() << ": " << sv[i] << "\n";
			continue;
		}
		++fstat[fidi];
	}
	if (CGLib::fileExists("autotest\\expect.log")) {
		CGLib::read_file_sv("autotest\\expect.log", sv);
		for (int i = 0; i < sv.size(); ++i) {
			int pos = sv[i].Find("[");
			if (pos == -1) continue;
			CString fid = sv[i].Mid(pos + 1, 10);
			pos = fid.Find("]");
			fid = fid.Left(pos);
			int fidi = atoi(fid);
			if (fidi <= 0) {
				nRetCode = 5;
				cout << "Flag ID is below 1: " << sv[i] << "\n";
				continue;
			}
			if (fidi >= fconfirmed.size()) {
				nRetCode = 5;
				cout << "Flag ID is above maximum flag count " << fconfirmed.size() <<
					": " << sv[i] << "\n";
				continue;
			}
			++fconfirmed[fidi];
		}
	}
	ofstream outfile;
	outfile.open("autotest\\fstat.csv");
	outfile << "Rule ID;Freq;Confirmed\n";
	for (int i = 0; i < fstat.size(); ++i) {
		outfile << i << ";" << fstat[i] << ";" << fconfirmed[i] << "\n";
	}
	outfile.close();
}

int test() {
	if (getenv("APPVEYOR_PROJECT_NAME") != NULL) {
		continuous_integration = 1;
	}
	logfile.open("autotest\\test.log", ios_base::app);

	TCHAR buffer[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, buffer);
	current_dir = string(buffer).c_str();
	Log("Current dir: " + current_dir + "\n");

	LoadConfig();
	ParseLyLogs();
	ParseFlags();
	PushArtifacts();

	CString outs = file("autotest\\run.log");
	cout << "Run log:\n";
	cout << outs;

	logfile.close();
	// Do not pause if continuous integration
	if (!continuous_integration) {
		cout << "Press any key to continue... ";
		_getch();
	}
	return 0;
}

int main()
{
	InitErrorMessages();
	HMODULE hModule = ::GetModuleHandle(nullptr);

  if (hModule != nullptr) {
    // initialize MFC and print and error on failure
    if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0)) {
        wprintf(L"Fatal Error: MFC initialization failed\n");
        nRetCode = 4;
    }
    else {
			test();
		}
  }
  else {
      wprintf(L"Fatal Error: GetModuleHandle failed\n");
      nRetCode = 1;
  }

  return nRetCode;
}
