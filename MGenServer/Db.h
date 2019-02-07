#pragma once

#include "stdafx.h"
#include <mysql.h>

class CDb
{
public:
	CDb();
	~CDb();

	int Connect(CString server, CString port, CString dbname, CString login, CString pass);
	int Query(CString q);
	long long GetInsertId();
	int Fetch(CString q);
	CString GetSt(CString fname);
	int GetInt(CString fname);
	long long GetLongLong(CString fname);
	float GetFloat(CString fname);
	CString Escape(CString st);
	void WriteLog(CString st, int no_db = 0);

	CString m_server;
	CString m_port;
	CString m_dbname;
	CString m_login;
	CString m_pass;
	CString log_fname;

	int connected = 0;
	MYSQL *conn;
	map<CString, vector<CString>> result;
	vector <CString> field;

	static int server_id;
	static long long session_id;
	static long long j_id;
};

