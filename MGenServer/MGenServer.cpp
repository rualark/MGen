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
int server_id = 0;
CString db_driver, db_server, db_port, db_login, db_pass, db_name;

vector <long long> tChild; // Timestamp of last restart
vector <int> aChild; // If state process should be automatically restarted on crash
vector <int> rChild; // Is state process running?
vector <CString> nChild; // Child process name
vector <CString> fChild; // Child process folder
vector <CString> pChild; // Child process parameter string
long long server_start_time = CGLib::time();

CDb db;

void WriteLog(CString st) {
	cout << st << "\n";
	CGLib::AppendLineToFile("log\\server.log", 
		CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S") + " " + st + "\n");
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
	if (ecode != 0 && ecode != STILL_ACTIVE) { // 259
		CString est;
		est.Format("Exit code %d: %s %s", ecode, fname, par);
		WriteLog(est);
	}
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

void RestartChild(int c) {
	Run(fChild[c] + "\\" + nChild[c], pChild[c], 200);
}

void CheckChilds() {
	for (int c = 0; c < nChild.size(); ++c) {
		HANDLE hProcess = GetProcessHandle(nChild[c]);
		if (hProcess == NULL) {
			rChild[c] = 0;
			if (aChild[c]) {
				WriteLog("Restarting process " + nChild[c] + "...");
				RestartChild(c);
				tChild[c] = CGLib::time();
			}
		}
		else {
			rChild[c] = 1;
			CloseHandle(hProcess);
		}
	}
}

void LoadConfig()
{
	TCHAR buffer[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, buffer);
	CString current_dir = string(buffer).c_str();
	WriteLog("Started MGenServer in current dir: " + current_dir );

	CString st, st2, st3;
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
				aChild.resize(nChild.size());
				rChild.resize(nChild.size());
				fChild.resize(nChild.size());
				pChild.resize(nChild.size());
				tChild.resize(nChild.size());
				tChild[nChild.size() - 1] = CGLib::time();
				++CGLib::parameter_found;
			}
			CGLib::CheckVar(&st2, &st3, "server_id", &server_id, 0, 1000000);
			if (aChild.size()) {
				CGLib::CheckVar(&st2, &st3, "childrestart", &aChild[aChild.size() - 1], 0, 1);
				CGLib::LoadVar(&st2, &st3, "childpath", &fChild[fChild.size() - 1]);
				CGLib::LoadVar(&st2, &st3, "childparams", &pChild[pChild.size() - 1]);
			}
			CGLib::LoadVar(&st2, &st3, "db_driver", &db_driver);
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
}

int Pause() {
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
		(timestamp - tChild[0]) / 1000,
		(timestamp - tChild[1]) / 1000,
		(timestamp - tChild[2]) / 1000,
		(timestamp - tChild[3]) / 1000,
		j_id);
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
	if (Connect()) return Pause();
	if (nRetCode) {
		cout << "Press any key to continue... ";
		return Pause();
	}
	for (;;) {
		CheckChilds();
		SendStatus();
		Sleep(1000);
	}
	cout << "Press any key to continue... ";
	return Pause();
}
