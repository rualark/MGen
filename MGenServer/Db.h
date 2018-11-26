#pragma once

#include "stdafx.h"

class CDb
{
public:
	CDb();
	~CDb();

	int Connect(CString driver, CString server, CString port, CString dbname, CString login, CString pass);
	int RealConnect();
	int Query(CString q);
	int Fetch(CString q);
	CString GetSt(CString fname);
	int GetInt(CString fname);
	float GetFloat(CString fname);
	void GetFields();
	CString Escape(CString st);
	void WriteLog(CString st, int no_db = 0);

	CString m_driver;
	CString m_server;
	CString m_port;
	CString m_dbname;
	CString m_login;
	CString m_pass;
	CString log_fname;

	CDatabase* db = 0;
	CRecordset* rs = 0;
	vector <CString> field;

	static int server_id;
	static long j_id;
};

