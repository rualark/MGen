// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Db.h"
#include "../MGen/GLibrary/GLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CDb::CDb() {
}

CDb::~CDb() {
}

int CDb::Connect(CString driver, CString server, CString port, CString dbname, CString login, CString pass) {
	m_driver = driver;
	m_server = server;
	m_dbname = dbname;
	m_port = port;
	m_login = login;
	m_pass = pass;

	CString connSt =
		"Driver=" + m_driver + ";"
		"Server=" + m_server + ";"
		"Database=" + m_dbname + ";"
		"Uid=" + m_login + ";"
		"Pwd=" + m_pass + ";";

	try {
		if (!db.OpenEx(connSt)) {
			WriteLog("Cannot open database: " + connSt);
			return 1;
		}
	}
	catch (CDBException* pEX) {
		WriteLog("Cannot open database: " + connSt + ": " + pEX->m_strError);
		pEX->Delete();
		return 1;
	}
	return 0;
}

int CDb::Query(CString q) {
	TRY	{
		db.ExecuteSQL(q);
	}
	CATCH_ALL(e) {
		TCHAR szCause[255];
		e->GetErrorMessage(szCause, 255);
		WriteLog(szCause);
	}
	END_CATCH_ALL
	return 0;
}

void CDb::WriteLog(CString st) {
	cout << st << "\n";
	CGLib::AppendLineToFile("log\\server.log",
		CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S") + " " + st + "\n");
}
