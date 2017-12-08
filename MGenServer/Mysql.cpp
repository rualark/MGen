// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Mysql.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CMysql::CMysql() {
}

CMysql::~CMysql() {
}

int CMysql::Connect(CString server, CString port, CString login, CString pass) {
	m_server = server;
	m_port = port;
	m_login = login;
	m_pass = pass;
	/*
	try {
		sql::Driver *driver;
		sql::Connection *con;
		sql::Statement *stmt;
		sql::ResultSet *res;

		// Create a connection
		driver = get_driver_instance();
		con = driver->connect("tcp://127.0.0.1:3306", "root", "root");
		// Connect to the MySQL test database
		con->setSchema("test");

		stmt = con->createStatement();
		res = stmt->executeQuery("SELECT 'Hello World!' AS _message");
		while (res->next()) {
			cout << "\t... MySQL replies: ";
			// Access column data by alias or column name
			cout << res->getString("_message") << endl;
			cout << "\t... MySQL says it again: ";
			// Access column data by numeric offset, 1 is the first column
			cout << res->getString(1) << endl;
		}
		delete res;
		delete stmt;
		delete con;

	}
	catch (sql::SQLException &e) {
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " 
			<< __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}
	*/
	return 0;
}