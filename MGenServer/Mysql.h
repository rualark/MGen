#pragma once

#include "stdafx.h"
//#include "mysql_connection.h"

//#include <cppconn/driver.h>
//#include <cppconn/exception.h>
//#include <cppconn/resultset.h>
//#include <cppconn/statement.h>

class CMysql 
{
public:
	CMysql();
	~CMysql();
	int Connect(CString server, CString port, CString login, CString pass);

	CString m_server;
	CString m_port;
	CString m_login;
	CString m_pass;
};

