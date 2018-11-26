// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Db.h"
#include "../MGen/GLibrary/GLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int CDb::server_id = 0;
long CDb::j_id = 0;

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

	//RealConnect();
	return 0;
}

int CDb::RealConnect() {
	CString connSt =
		"Driver=" + m_driver + ";"
		"Server=" + m_server + ";"
		"Database=" + m_dbname + ";"
		"Uid=" + m_login + ";"
		"Pwd=" + m_pass + ";";

	try {
		if (db) {
			delete db;
		}
		db = new CDatabase();
		if (!db->OpenEx(connSt)) {
			WriteLog("Cannot open database: " + connSt, 1);
			abort();
			return 1;
		}
	}
	catch (CDBException* pEX) {
		WriteLog("Cannot open database: " + connSt + ": " + pEX->m_strError, 1);
		pEX->Delete();
		abort();
		return 1;
	}
	return 0;
}

int CDb::Query(CString q) {
	TRY	{
		RealConnect();
		db->ExecuteSQL(q);
	}
	CATCH_ALL(e) {
		TCHAR szCause[255];
		e->GetErrorMessage(szCause, 255);
		WriteLog(szCause, 1);
		abort();
		return 1;
	} 
	END_CATCH_ALL;
	return 0;
}

int CDb::Fetch(CString q) {
	TRY {
		RealConnect();
		if (rs) delete rs;
		rs = new CRecordset;
		rs->m_pDatabase = db;
		rs->Open(CRecordset::forwardOnly, q, CRecordset::readOnly);
	}
	CATCH(CDBException, e) {
		WriteLog("Error executing query '" + q + "': " + e->m_strError, 1);
		abort();
		return 1;
	}
	END_CATCH; 
	return 0;
}

CString CDb::GetSt(CString fname) {
	if (!rs || rs->IsEOF()) return "";
	CString st;
	TRY {
		rs->GetFieldValue(fname, st);
	}
	CATCH(CDBException, e) {
		WriteLog("Error parsing field '" + fname + "': " + e->m_strError, 1);
		abort();
		return "";
	}
	END_CATCH;
	return st;
}

int CDb::GetInt(CString fname) {
	return atoi(GetSt(fname));
}

float CDb::GetFloat(CString fname) {
	return atof(GetSt(fname));
}

void CDb::WriteLog(CString st, int no_db) {
	if (db && db->IsOpen() && !no_db) {
		CString q;
		q.Format("INSERT INTO j_logs VALUES('','%d','%d',NOW(),'%s')",
			server_id, j_id, Escape(st));
		Query(q);
	}
	st = CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S") + " " + st;
	cout << st << "\n";
	CGLib::AppendLineToFile(log_fname, st + "\n");
}

void CDb::GetFields() {
	field.clear();
	int nFields = rs->GetODBCFieldCount();
	for (short x = 0; x < nFields; x++) {
		CODBCFieldInfo fieldinfo;
		short pos = x;
		rs->GetODBCFieldInfo(pos, fieldinfo);
		field.push_back(fieldinfo.m_strName);
	}
}

CString CDb::Escape(CString st) {
	CString st2 = st;
	st2.Replace("\\", "\\\\");
	st2.Replace("\"", "\\\"");
	st2.Replace("'", "\\'");
	//if (st2 != st) WriteLog("Hello");
	return st2;
}
