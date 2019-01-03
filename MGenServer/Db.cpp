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
	conn = mysql_init(NULL);
	if (conn == NULL) {
		WriteLog("Cannot initialize mysql", 1);
	}
	else {
		//WriteLog("Mysql init successful", 1);
	}
}

CDb::~CDb() {
	mysql_close(conn);
	mysql_library_end();
}

int CDb::Connect(CString server, CString port, CString dbname, CString login, CString pass) {
	m_server = server;
	m_dbname = dbname;
	m_port = port;
	m_login = login;
	m_pass = pass;

	// Do not reconnect, because automatic reconnect is enabled
	if (connected) return 0;
	if (!mysql_real_connect(conn, m_server, m_login, m_pass, m_dbname, atoi(m_port), NULL, 0)) {
		CString est;
		est.Format("Error: can't connect to database: %s\n", mysql_error(conn));
		WriteLog(est, 1);
		connected = 0;
		return 1;
	}
	else {
		// Set automatic reconnect
		my_bool reconnect = 1;
		mysql_options(conn, MYSQL_OPT_RECONNECT, &reconnect);
		WriteLog("Mysql connect successful", 1);
	}
	connected = 1;
	return 0;
}

int CDb::Query(CString q) {
	//WriteLog(q, 1);
	if (mysql_query(conn, q)) {
		CString est;
		est.Format("Error during query: %s: %s", q, mysql_error(conn));
		WriteLog(est, 1);
		return 1;
	}
	return 0;
}

int CDb::Fetch(CString q) {
	//WriteLog(q, 1);
	field.clear();
	result.clear();
	if (mysql_query(conn, q)) {
		CString est;
		est.Format("Error during fetch: %s: %s", q, mysql_error(conn));
		WriteLog(est, 1);
		return 1;
	}
	MYSQL_RES *res;
	res = mysql_store_result(conn);
	if (res) {
		MYSQL_FIELD *fields;
		int num_fields = mysql_num_fields(res);
		fields = mysql_fetch_fields(res);
		for (int i = 0; i < num_fields; ++i) {
			field.push_back(fields[i].name);
		}
		int x = 0;
		MYSQL_ROW row;
		while (row = mysql_fetch_row(res)) {
			for (int i = 0; i < mysql_num_fields(res); i++) {
				result[field[i]].push_back(row[i]);
				++x;
			}
		}
		mysql_free_result(res);
	}
	else {
		CString est;
		est.Format("Error during query: %s: %s", q, mysql_error(conn));
		WriteLog(est, 1);
		return 1;
	}
	return 0;
}

CString CDb::GetSt(CString fname) {
	return result[fname][0];
}

int CDb::GetInt(CString fname) {
	return atoi(GetSt(fname));
}

float CDb::GetFloat(CString fname) {
	return atof(GetSt(fname));
}

void CDb::WriteLog(CString st, int no_db) {
	if (connected && !no_db) {
		CString q;
		q.Format("INSERT INTO j_logs VALUES('','%d','%d',NOW(),'%s')",
			server_id, j_id, Escape(st));
		Query(q);
	}
	st = CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S") + " " + st;
	cout << st << "\n";
	CGLib::AppendLineToFile(log_fname, st + "\n");
}

CString CDb::Escape(CString st) {
	CString st2 = st;
	st2.Replace("\\", "\\\\");
	st2.Replace("\"", "\\\"");
	st2.Replace("'", "\\'");
	//if (st2 != st) WriteLog("Hello");
	return st2;
}
