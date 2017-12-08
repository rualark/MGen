// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// MGenServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MGenServer.h"
#include "Db.h"
#include "../MGen/GLibrary/GLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// The one and only application object
CWinApp theApp;

using namespace std;

int nRetCode = 0;

long j_id = 0;
int j_timeout;
int j_priority;
CString j_type;
CString j_folder;
CString j_file;
CString share;
int server_id = 0;
CString db_driver, db_server, db_port, db_login, db_pass, db_name;

vector <CString> nChild; // Child process name
map <CString, long long> tChild; // Timestamp of last restart
map <CString, int> aChild; // If state process should be automatically restarted on crash
map <CString, int> rChild; // Is state process running?
map <CString, CString> fChild; // Child process folder
map <CString, CString> pChild; // Child process parameter string

long long server_start_time = CGLib::time();

map <CString, int> JobTimeout;
map <CString, int> JobTimeout2;

CDb db;

void WriteLog(CString st) {
	cout << st << "\n";
	CGLib::AppendLineToFile("log\\server.log", 
		CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S") + " " + st + "\n");
}

// Start process, wait a little and check if process exited with error prematurely
// Then report error
int Run(CString fname, CString par, int delay) {
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
		CString est;
		est.Format("Exit code %d: %s %s", ecode, fname, par);
		WriteLog(est);
		return ecode;
	}
	return 0;
}

// Start process, wait for some time. If process did not finish, this is an error
int RunTimeout(CString fname, CString par, int delay) {
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
	if (WaitForSingleObject(sei.hProcess, delay) == WAIT_TIMEOUT) {
		WriteLog(fname + " " + par + ": Timeout waiting for process\n");
		return 100;
	}
	if (ecode != 0 && ecode != STILL_ACTIVE) { // 259
		CString est;
		est.Format("Exit code %d: %s %s", ecode, fname, par);
		WriteLog(est);
		return ecode;
	}
	return 0;
}

HANDLE GetProcessHandle(CString pname) {
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (Process32First(snapshot, &entry) == TRUE)	{
		while (Process32Next(snapshot, &entry) == TRUE)	{
			if (stricmp(entry.szExeFile, pname) == 0)	{
				return OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
				//WaitForSingleObject(hProcess, 100000);
			}
		}
	}
	CloseHandle(snapshot);
	return NULL;
}

void RestartChild(CString cn) {
	if (Run(fChild[cn] + cn, pChild[cn], 200)) nRetCode = 7;
}

void CheckChildsPath() {
	for (int c = 0; c < nChild.size(); ++c) {
		CString cn = nChild[c];
		if (!CGLib::fileExists(fChild[cn] + cn)) {
			WriteLog("Not found program file: " + fChild[cn] + cn);
			nRetCode = 3;
			return;
		}
	}
}

void CheckChilds() {
	for (int c = 0; c < nChild.size(); ++c) {
		CString cn = nChild[c];
		HANDLE hProcess = GetProcessHandle(cn);
		if (hProcess == NULL) {
			rChild[cn] = 0;
			if (aChild[cn]) {
				WriteLog("Restarting process " + cn + "...");
				RestartChild(cn);
				tChild[cn] = CGLib::time();
			}
		}
		else {
			rChild[cn] = 1;
			CloseHandle(hProcess);
		}
	}
}

int GetTimeout(CString jobName) {
	if (JobTimeout.find(jobName) == JobTimeout.end()) return 600;
	return JobTimeout[jobName];
}

int GetTimeout2(CString jobName) {
	if (JobTimeout2.find(jobName) == JobTimeout2.end()) return 600;
	return JobTimeout2[jobName];
}

void LoadConfig()
{
	TCHAR buffer[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, buffer);
	CString current_dir = string(buffer).c_str();
	WriteLog("Started MGenServer in current dir: " + current_dir );

	CString st, st2, st3, cur_child;
	ifstream fs;
	CString fname = "server\\server.pl";
	// Check file exists
	if (!CGLib::fileExists(fname)) {
		WriteLog("LoadConfig cannot find file: " + fname);
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
				aChild[st3] = 0;
				rChild[st3] = 0;
				fChild[st3] = "";
				pChild[st3] = "";
				tChild[st3] = CGLib::time();
				cur_child = st3;
				++CGLib::parameter_found;
			}
			if (st2 == "job") {
				vector <CString> sv;
				CGLib::Tokenize(st3, sv, ";");
				JobTimeout[sv[0]] = atoi(sv[1]);
				JobTimeout2[sv[0]] = atoi(sv[2]);
				++CGLib::parameter_found;
			}
			CGLib::CheckVar(&st2, &st3, "server_id", &server_id, 0, 1000000);
			if (aChild.size()) {
				CGLib::CheckVar(&st2, &st3, "childrestart", &aChild[cur_child], 0, 1);
				CGLib::LoadVar(&st2, &st3, "childpath", &fChild[cur_child]);
				CGLib::LoadVar(&st2, &st3, "childparams", &pChild[cur_child]);
			}
			CGLib::LoadVar(&st2, &st3, "db_driver", &db_driver);
			CGLib::LoadVar(&st2, &st3, "share", &share);
			CGLib::LoadVar(&st2, &st3, "db_server", &db_server);
			CGLib::LoadVar(&st2, &st3, "db_port", &db_port);
			CGLib::LoadVar(&st2, &st3, "db_login", &db_login);
			CGLib::LoadVar(&st2, &st3, "db_pass", &db_pass);
			CGLib::LoadVar(&st2, &st3, "db_name", &db_name);
			if (!CGLib::parameter_found) {
				WriteLog("Unrecognized parameter '" + st2 + "' = '" + st3 + "' in file " + fname);
			}
			if (nRetCode) break;
		}
	}
	fs.close();
	CString est;
	est.Format("LoadConfig loaded %d lines from %s", i, fname);
	WriteLog(est);
	// Check config
	if (!CGLib::dirExists(share)) {
		WriteLog("Shared folder not found: " + share);
		nRetCode = 6;
	}
}

int Pause() {
	cout << "Press any key to continue... ";
	_getch();
	return nRetCode;
}

int Connect() {
	if (db.Connect(db_driver, db_server, db_port, db_name, db_login, db_pass)) {
		nRetCode = 4;
	}
	return nRetCode;
}

void SendStatus() {
	CString q;
	long long timestamp = CGLib::time();
	q.Format("REPLACE INTO s_status VALUES('%d',NOW(),'host','%ld','%lld','%lld','%lld','%lld','%lld','%ld')",
		server_id, GetTickCount() / 1000, (timestamp - server_start_time) / 1000,
		(timestamp - tChild["Reaper.exe"]) / 1000,
		(timestamp - tChild["AutoHotkey.exe"]) / 1000,
		(timestamp - tChild["MGen.exe"]) / 1000,
		(timestamp - tChild["lilypond-windows.exe"]) / 1000,
		j_id);
	db.Query(q);
}

void FinishJob(int res, CString est) {
	CString q;
	q.Format("UPDATE job SET j_duration=NOW() - j_started, j_state=3, j_result='%d', j_progress='%s' WHERE j_id='%d'",
		res, db.Escape(est), j_id);
	db.Query(q);
}

int RunJobCA2() {
	CString fname = share + j_folder + j_file;
	CString fname_pl = share + j_folder + CGLib::bname_from_path(j_file) + ".pl";
	CString fname_pl2 = "configs\\GenCA2\\" + CGLib::bname_from_path(j_file) + ".pl";
	// Check input file exists
	if (!CGLib::fileExists(fname)) {
		CString est = "File not found: " + fname;
		WriteLog(est);
		FinishJob(1, est);
		return 1;
	}
	if (!CGLib::fileExists(fname_pl)) {
		CString est = "File not found: " + fname_pl;
		WriteLog(est);
		FinishJob(1, est);
		return 1;
	}
	// Copy config
	CGLib::copy_file(fname_pl, fname_pl2);
	// Run MGen
	CString par;
	par.Format("-test=%d %s", GetTimeout(j_type), fname_pl2);
	int ret = Run(fChild["MGen.exe"] + "MGen.exe", par, GetTimeout2(j_type));
	if (ret) {
		CString est;
		est.Format("Error during MGen run: %d", ret);
		WriteLog(est);
		FinishJob(1, est);
		return 1;
	}
	// Get autosave
	CString as_fname, as_dir;
	if (!CGLib::fileExists("log\\autosave.txt")) {
		CString est = "File not found: log\\autosave.txt";
		WriteLog(est);
		FinishJob(1, est);
		return 1;
	}
	vector <CString> sv;
	CGLib::read_file_sv("log\\autosave.txt", sv);
	if (sv.size() != 3) {
		CString est = "Wrong row count in file: log\\autosave.txt";
		WriteLog(est);
		FinishJob(1, est);
		return 1;
	}
	as_dir = sv[0];
	as_fname = sv[1];
	if (!CGLib::fileExists(as_dir + "\\" + as_fname + ".ly")) {
		CString est = "Autosave file not found: " + as_dir + "\\" + as_fname + ".ly";
		WriteLog(est);
		FinishJob(1, est);
		return 1;
	}
	FinishJob(0, "Success");
	return 0;
}

int RunJob() {
	// Check that folder exists
	if (!CGLib::dirExists(share + j_folder)) {
		CString est = "Folder not found: " + share + j_folder;
		WriteLog(est);
		FinishJob(1, est);
		return 1;
	}
	if (j_type == "CA2") RunJobCA2();
	return 0;
}

void TakeJob() {
	CString q, est;
	db.Query("LOCK TABLES job WRITE");
	db.Fetch("SELECT * FROM job WHERE j_state=1 ORDER BY j_priority DESC, j_id LIMIT 1");
	if (!db.rs.IsEOF()) {
		// Load job
		j_id = db.GetInt("j_id");
		j_priority = db.GetInt("j_priority");
		j_timeout = db.GetInt("j_timeout");
		j_type = db.GetSt("j_type");
		j_folder = db.GetSt("j_folder");
		j_file = db.GetSt("j_file");
		// Take job
		q.Format("UPDATE job SET j_started=NOW(), s_id='%d', j_state=2, j_progress='Job assigned' WHERE j_id='%d'",
			server_id, j_id);
		db.Query(q);
		db.Query("UNLOCK TABLES");
		// Log
		est.Format("Taking job #%ld: %s, %s (priority %d)", 
			j_id, j_type, j_folder, j_priority);
		WriteLog(est);
		// Update status
		SendStatus();
		RunJob();
	}
	else {
		db.Query("UNLOCK TABLES");
	}
}

void Init() {
	// On start, reset all jobs that did not finish correctly
	CString q;
	q.Format("SELECT COUNT(*) as cnt FROM job WHERE s_id='%d' AND j_state=2", server_id);
	db.Fetch(q);
	if (!db.rs.IsEOF()) {
		db.GetFields();
		int cnt = db.GetInt("cnt");
		if (cnt) {
			CString est;
			est.Format("Detected and cleared %d jobs that did not finish correctly on this server #%d",
				cnt, server_id);
			WriteLog(est);
		}
	}
	q.Format("UPDATE job SET j_state=1 WHERE s_id='%d' AND j_state=2", server_id);
	db.Query(q);
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
	CheckChildsPath();
	if (Connect()) return Pause();
	if (nRetCode) {
		return Pause();
	}
	Init();
	for (;;) {
		CheckChilds();
		if (nRetCode) return Pause();
		SendStatus();
		TakeJob();
		SendStatus();
		Sleep(1000);
	}
	return Pause();
}
