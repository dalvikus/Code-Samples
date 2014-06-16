#ifndef __MySQL_CAPI_H__
#define __MySQL_CAPI_H__


#include <WinSock2.h>	// Must be included before mysql.h
#include <mysql.h>

class MySQLCAPI {
public:
	MySQLCAPI(int *lpiStatus);
	~MySQLCAPI();

private:
	bool		__bMySQLLibraryInit;
	MYSQL		*__mysql;

public:
	MYSQL		*__getMySQL(void)		{return __mysql;}
};


#endif	// !__MySQL_CAPI_H__
