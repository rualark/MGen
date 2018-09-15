// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// MGenClean.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MGenClean.h"
#include "../MGenServer/Db.h"
#include "../MGen/GLibrary/GLib.h"
#include "../MGenServer/EnumWindows.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MAX_TRACK 1000

// The one and only application object
CWinApp theApp;

using namespace std;

CString ReaperTempFolder = "server\\Reaper\\";

// Global
volatile int close_flag = 0;
int nRetCode = 0;
CString est;
CString client_host;
vector<CString> errorMessages;
long long time_job0;
CString j_basefile;

// Parameters
int clean_every_minutes = 1000;
CString reaperbuf;
CString share;
CString db_driver, db_server, db_port, db_login, db_pass, db_name;
int daw_wait = 200;
int run_minimized = 0;
int screenshots_enabled = 0;
float rms_exp = 10;

// Job
int j_timeout;
int j_timeout2;
int j_engrave = 0;
int j_render = 0;
int j_priority;
int j_autorestart = 0;
CString progress_fname;
CString j_type;
CString f_folder;
CString j_folder;
CString f_name;
CString j_progress;
int f_stems = 0;
int j_stages = 0;

int can_render = 1;
int screenshot_id = 0;
int max_screenshot = 10;

map <int, map<int, int>> st_used; // [stage][track]
map <int, float> st_reverb; // [stage]
map <int, CString> tr_name; // [track]
map <int, vector<int>> dyn; // [track][time]

// Children
vector <CString> nChild; // Child process name
map <CString, long long> tChild; // Timestamp of last restart
map <CString, int> aChild; // If state process should be automatically restarted on crash
map <CString, int> rChild; // Is state process running?
map <CString, CString> fChild; // Child process folder
map <CString, CString> pChild; // Child process parameter string

// Time
long long clean_start = 0;
long long server_start_time = CGLib::time();

// Objects
CDb db;

void InitErrorMessages() {
	errorMessages.resize(1000);
	errorMessages[0] = "OK";
}

CString GetErrorMessage(int e) {
	if (e < errorMessages.size()) return errorMessages[e];
	else return "";
}

void WriteLog(CString st) {
	db.WriteLog(st);
	//st = CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S") + " " + st;
	//cout << st << "\n";
	//CGLib::AppendLineToFile("server\\MGenClean.log", st + "\n");
}

void WriteLogLocal(CString st) {
	st = CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S") + " " + st;
	cout << st << "\n";
	CGLib::AppendLineToFile("server\\MGenClean.log", st + "\n");
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
	if (run_minimized) sei.nShow = SW_SHOWMINNOACTIVE;
	else sei.nShow = SW_SHOWNORMAL;
	sei.hInstApp = NULL;
	ShellExecuteEx(&sei);
	WaitForSingleObject(sei.hProcess, delay);
	if (!GetExitCodeProcess(sei.hProcess, &ecode)) ecode = 102;
	if (ecode != 0 && ecode != STILL_ACTIVE) { // 259
		est.Format("Exit code %d: %s %s", ecode, fname, par);
		WriteLogLocal(est);
		return ecode;
	}
	return 0;
}

HANDLE GetProcessHandle(CString pname) {
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (Process32First(snapshot, &entry) == TRUE) {
		while (Process32Next(snapshot, &entry) == TRUE) {
			if (stricmp(entry.szExeFile, pname) == 0) {
				return OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
				//WaitForSingleObject(hProcess, 100000);
			}
		}
	}
	CloseHandle(snapshot);
	return NULL;
}

void KillProcessByName(const char *filename) {
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	PROCESSENTRY32 pEntry;
	pEntry.dwSize = sizeof(pEntry);
	BOOL hRes = Process32First(hSnapShot, &pEntry);
	while (hRes)
	{
		if (strcmp(pEntry.szExeFile, filename) == 0)
		{
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
				(DWORD)pEntry.th32ProcessID);
			if (hProcess != NULL)
			{
				TerminateProcess(hProcess, 9);
				CloseHandle(hProcess);
			}
		}
		hRes = Process32Next(hSnapShot, &pEntry);
	}
	CloseHandle(hSnapShot);
}

// Start process, wait for some time. If process did not finish, this is an error
int RunTimeout(CString path, CString par, int delay) {
	DWORD ecode;
	SHELLEXECUTEINFO sei = { 0 };
	sei.cbSize = sizeof(SHELLEXECUTEINFO);
	sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
	sei.hwnd = NULL;
	sei.lpVerb = NULL;
	sei.lpFile = path;
	sei.lpParameters = par;
	sei.lpDirectory = NULL;
	if (run_minimized) sei.nShow = SW_SHOWMINNOACTIVE;
	else sei.nShow = SW_SHOWNORMAL;
	sei.hInstApp = NULL;
	ShellExecuteEx(&sei);
	if (WaitForSingleObject(sei.hProcess, delay) == WAIT_TIMEOUT) {
		WriteLogLocal(path + " " + par + ": Timeout waiting for process\n");
		return 100;
	}
	if (!GetExitCodeProcess(sei.hProcess, &ecode)) ecode = 102;
	if (ecode != 0 && ecode != STILL_ACTIVE) { // 259
		CString est;
		est.Format("Exit code %ld: %s %s", 
			ecode, path, par);
		WriteLogLocal(est);
		return ecode;
	}
	return 0;
}

void LoadConfig()
{
	TCHAR buffer[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, buffer);
	CString current_dir = string(buffer).c_str();
	WriteLogLocal("Started MGenClean in current dir: " + current_dir);

	CString st, st2, st3, cur_child;
	ifstream fs;
	CString fname = "server\\server.pl";
	// Check file exists
	if (!CGLib::fileExists(fname)) {
		WriteLogLocal("LoadConfig cannot find file: " + fname);
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
				fChild[st3].Empty();
				pChild[st3].Empty();
				tChild[st3] = CGLib::time();
				cur_child = st3;
				++CGLib::parameter_found;
			}
			CGLib::CheckVar(&st2, &st3, "server_id", &CDb::server_id, 0, 1000000);
			if (aChild.size()) {
				CGLib::CheckVar(&st2, &st3, "childrestart", &aChild[cur_child], 0, 1);
				CGLib::LoadVar(&st2, &st3, "childpath", &fChild[cur_child]);
				CGLib::LoadVar(&st2, &st3, "childparams", &pChild[cur_child]);
			}
			CGLib::CheckVar(&st2, &st3, "clean_every_minutes", &clean_every_minutes);
			CGLib::LoadVar(&st2, &st3, "reaperbuf", &reaperbuf);
			CGLib::LoadVar(&st2, &st3, "db_driver", &db_driver);
			CGLib::CheckVar(&st2, &st3, "daw_wait", &daw_wait, 0, 6000);
			CGLib::CheckVar(&st2, &st3, "run_minimized", &run_minimized, 0, 1);
			CGLib::CheckVar(&st2, &st3, "screenshots_enabled", &screenshots_enabled, 0, 1);
			CGLib::LoadVar(&st2, &st3, "share", &share);
			CGLib::LoadVar(&st2, &st3, "db_server", &db_server);
			CGLib::LoadVar(&st2, &st3, "db_port", &db_port);
			CGLib::LoadVar(&st2, &st3, "db_login", &db_login);
			CGLib::LoadVar(&st2, &st3, "db_pass", &db_pass);
			CGLib::LoadVar(&st2, &st3, "db_name", &db_name);
			if (!CGLib::parameter_found) {
				WriteLogLocal("Unrecognized parameter '" + st2 + "' = '" + st3 + "' in file " + fname);
			}
			if (nRetCode) break;
		}
	}
	fs.close();
	est.Format("LoadConfig loaded %d lines from %s", i, fname);
	WriteLogLocal(est);
	// Check config
	if (!CGLib::dirExists(share)) {
		WriteLogLocal("Shared folder not found: " + share);
		nRetCode = 6;
	}
}

int PauseClose() {
	est.Format("MGenClean is exiting with return code %d", nRetCode);
	cout << est << "\n";
	//cout << "Press any key to continue... ";
	//_getch();
	return nRetCode;
}

int Connect() {
	if (db.Connect(db_driver, db_server, db_port, db_name, db_login, db_pass)) {
		nRetCode = 4;
	}
	return nRetCode;
}

int SendKeyToWindowClass(CString wClass, short vk) {
	HWND hWindow = FindWindow(wClass, NULL);
	if (hWindow) {
		PostMessage(hWindow, WM_KEYDOWN, VK_F12, 0);
		return 0;
	}
	else return 1;
}

void Init() {
	db.log_fname = "server\\MGenClean.log";
	// Get client hostname
	db.Fetch("SELECT SUBSTRING_INDEX(host,':',1) as 'ip' from information_schema.processlist WHERE ID=connection_id()");
	if (!db.rs.IsEOF()) {
		client_host = db.GetSt("ip");
	}
}

BOOL WINAPI ConsoleHandlerRoutine(DWORD dwCtrlType) {
	if (dwCtrlType == CTRL_CLOSE_EVENT) {
		WriteLogLocal("User initiated MGenClean exit");
		close_flag = 1;
		while (close_flag != 2)
			Sleep(100);
		//WriteLog("Exiting server");
		return TRUE;
	}
	return FALSE;
}

void ProcessTask(path path_info) {
	CString est, st, st2, st3, q;
	int pos;
	CString ext = path_info.extension().string().c_str();
	CString pth = path_info.string().c_str();
	CString fname = CGLib::fname_from_path(pth);
	CString fnoext = CGLib::noext_from_path(pth);
	vector<CString> sa;
	CGLib::Tokenize(pth, sa, "\\");
	if (sa.size() < 3) {
		WriteLogLocal("Cannot parse: " + pth);
		return;
	}
	ext.MakeLower();
	//cout << pth << "\n";
	pos = fname.Find("-");
	if (pos == -1) {
		WriteLogLocal("Cannot parse: " + pth);
		return;
	}
	CString u_id = fname.Left(pos);
	CDb::j_id = atoi(fname.Mid(pos + 1));
	pos = sa[sa.size() - 2].Find("-");
	if (pos == -1) {
		WriteLogLocal("Cannot parse: " + pth);
		return;
	}
	CString month = sa[sa.size() - 2].Left(pos);
	CString day = sa[sa.size() - 2].Mid(pos + 1);
	CString year = sa[sa.size() - 3];
	// Pause to not overload mysql
	Sleep(10);
	// Query
	q.Format("SELECT j_size, TIMESTAMPDIFF(DAY, GREATEST(j_added, j_queued, j_finished), NOW()) AS j_passed, f_name, f_store, j_cleaned, files.f_id, j_class FROM jobs LEFT JOIN files USING (f_id) WHERE j_id='%ld'",
		CDb::j_id);
	db.Fetch(q);
	// Database error
	if (db.rs.IsEOF()) {
		// Get passed time
		q.Format("SELECT TIMESTAMPDIFF(DAY, '%s', NOW()) AS j_passed",
			year + "-" + month + "-" + day);
		db.Fetch(q);
		if (db.rs.IsEOF()) {
			WriteLogLocal("Error accessing mysql server");
		}
		int j_passed = db.GetInt("j_passed");
		// Delete unknown files only after a month
		if (j_passed > 30) {
			est.Format("Task %ld [%s-%s-%s]: passed %d days not found in database, deleting",
				CDb::j_id, year, month, day, j_passed);
			WriteLogLocal(est);
			//Run("cmd.exe", "/c rmdir /s /q \"" + pth + "\"", 5000);
			CGLib::RmDir(pth + "\\");
			//CGLib::CleanFolder(pth + "\\*.*");
			//RemoveDirectory(pth);
			//abort();
		}
		else {
			est.Format("Task %ld [%s-%s-%s]: passed %d days not found in database",
				CDb::j_id, year, month, day, j_passed);
			//cout << est << "\n";
		}
		return;
	}
	CString f_name = db.GetSt("f_name");
	int f_id = db.GetInt("f_id");
	int j_class = db.GetInt("j_class");
	int j_passed = db.GetInt("j_passed");
	int f_store = db.GetInt("f_store");
	int j_size = db.GetInt("j_size");
	int j_cleaned = db.GetInt("j_cleaned");
	if (!f_id) {
		if (j_passed > 31) {
			est.Format("Task %ld [%s-%s-%s]: passed %d days does not have file in database, deleted",
				CDb::j_id, year, month, day, j_passed);
			WriteLogLocal(est);
			q.Format("DELETE FROM jobs WHERE j_id='%ld'", CDb::j_id);
			db.Query(q);
		}
		else {
			est.Format("Task %ld [%s-%s-%s]: passed %d days does not have file in database",
				CDb::j_id, year, month, day, j_passed);
			//cout << est << "\n";
		}
		return;
	}
	if (j_cleaned == 2) {
		return;
	}
	// Collect size
	if (!j_size) {
		q.Format("UPDATE jobs SET j_size='%llu' WHERE j_id='%ld'",
			CGLib::FolderSize(pth + "\\"), CDb::j_id);
		db.Query(q);
		cout << "Task " << CDb::j_id << ": collected size\n";
	}
	//cout << "Task ID: " << CDb::j_id << " f_id " << f_id << " found in database\n";
	int need_clean = 0;
	if (f_store == 366000) {
		if (j_passed > 366) {
			// Check if task is last successful of this class
			q.Format("SELECT j_id FROM jobs WHERE f_id = %d AND j_id > %ld AND j_state = 3 AND j_result = 0 AND j_class = %d",
				f_id, CDb::j_id, j_class);
			db.Fetch(q);
			if (!db.rs.IsEOF()) {
				est.Format("Archived task %ld file %d: passed %d days, cleaned",
					CDb::j_id, f_id, j_passed);
				WriteLog(est);
				need_clean = 2;
			}
			else {
				if (!j_cleaned) {
					est.Format("Task %ld file %d: passed %d days, cleaning stems",
						CDb::j_id, f_id, j_passed);
					WriteLog(est);
					need_clean = 1;
				}
			}
		}
	}
	else {
		if (j_passed > f_store) {
			need_clean = 2;
			est.Format("Task %ld file %d: passed %d > %d days, cleaned",
				CDb::j_id, f_id, j_passed, f_store);
			WriteLog(est);
		}
	}
	if (need_clean == 2) {
		CGLib::RmDir(pth + "\\", ".pl");
		q.Format("UPDATE jobs SET j_cleaned=2 WHERE j_id='%ld'", CDb::j_id);
		db.Query(q);
		// Collect size
		q.Format("UPDATE jobs SET j_size='%llu' WHERE j_id='%ld'",
			CGLib::FolderSize(pth + "\\"), CDb::j_id);
		db.Query(q);
	}
	if (need_clean == 1) {
		CGLib::CleanFolder(pth + "\\" + CGLib::bname_from_path(f_name) + "*_*.mp3");
		q.Format("UPDATE jobs SET j_cleaned=1 WHERE j_id='%ld'", CDb::j_id);
		db.Query(q);
		// Collect size
		q.Format("UPDATE jobs SET j_size='%llu' WHERE j_id='%ld'",
			CGLib::FolderSize(pth + "\\"), CDb::j_id);
		db.Query(q);
	}
}

void CleanFolders() {
	clean_start = CGLib::time();
	cout << "Starting folders cleaning...\n";
	CString pth = share + "jobs\\";
	for (recursive_directory_iterator i(pth.GetBuffer()), end; i != end; ++i) {
		if (i.depth() == 2 && is_directory(i->path())) {
			i.disable_recursion_pending();
			ProcessTask(i->path());
			CDb::j_id = 0;
			if (nRetCode) return;
		}
	}
	cout << "Finished cleaning in " << (CGLib::time() - clean_start) / 1000 << " seconds. Waiting " << clean_every_minutes << " minutes...\n";
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

	InitErrorMessages();
	BOOL ret = SetConsoleCtrlHandler(ConsoleHandlerRoutine, TRUE);
	LoadConfig();
	if (nRetCode) {
		return PauseClose();
	}
	if (Connect()) return PauseClose();
	if (nRetCode) {
		return PauseClose();
	}
	Init();
	//cout << "Folder size: " << CGLib::FolderSize("share\\jobs\\2017\\") << "\n";
	//return PauseClose();
	for (;;) {
		if (Connect()) return PauseClose();
		CleanFolders();
		if (nRetCode) return PauseClose();
		for (int i = 0; i < clean_every_minutes * 60; ++i) {
			if (close_flag == 1) {
				close_flag = 2;
				break;
			}
			Sleep(1000);
		}
		// Close always
		return PauseClose();
		if (close_flag) {
			close_flag = 2;
			break;
		}
	}
	return PauseClose();
}
