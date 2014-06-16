#ifndef __MySQL_CAPI_H__
#define __MySQL_CAPI_H__


#include <WinSock2.h>	// Must be included before mysql.h
#include <mysql.h>

#define MySQL_DAILY_USER	"bloomus"
#define	MySQL_HOST		"flowergarden"
#define MySQL_BLOOMUS_DATABASE	"DBBloomUs"

class MySQLCAPI {
public:
	MySQLCAPI(const char *username, const char *password, int *lpiStatus);
	~MySQLCAPI();

private:
	bool		__bMySQLLibraryInit;
	MYSQL		*__mysql;
	MYSQL_RES	*__result;
	unsigned	__RowCount;

public:
	MYSQL		*__getMySQL(void)		{return __mysql;}

	void	__freeMySQLResult(void)	{
		if (__result) {
			mysql_free_result(__result);
			__result = NULL;
		}
	}
	void		__setMySQLResult(MYSQL_RES *result)	{__result = result;}
	MYSQL_RES	*__getMySQLResult(void)			{return __result;}
	void		__setRowCount(unsigned RowCount) {
		__RowCount = RowCount;

		// If needed, resize __dataRemoved array.
		if (RowCount > __sizeOfDataRemoved) {
			bool	*dataRemoved = (bool *) realloc(__dataRemoved, RowCount * sizeof(bool *));
			if (!dataRemoved) {
				*((int *) 0) = 0;	// Debug...
				exit(1);
			}
			__dataRemoved = dataRemoved;
		}
		__sizeOfDataRemoved = RowCount;

		// Initialize __dataRemoved array.
		for (unsigned i = 0; i < RowCount; ++i)
			__dataRemoved[i] = false;
	}

private:
	bool		*__dataRemoved;		// __dataRemoved[dataIndex] says if
						// the item is removed;
						// dynamically it is allocated.
	unsigned	__sizeOfDataRemoved;	// # of element of __dataRemoved array
public:
	void	__setDataRemoved(int itemIndex) {
		int	dataIndex = __getDataIndex(itemIndex);
		if (dataIndex == -1) {
			*((int *) 0) = 0;	// Debug...
			return;
		}
		__dataRemoved[dataIndex] = true;
	}
	int	__getDataIndex(int itemIndex) {
		int	dataIndex = -1;
		for (unsigned i = 0; i < __sizeOfDataRemoved; ++i) {
			if (__dataRemoved[i]) {
				// Skip if removed.
				continue;
			}
			if (++dataIndex == itemIndex) {
				// Found!
				return i;
			}
		}
		// Not found or invalid itemIndex
		return -1;
	}
};


#endif	// !__MySQL_CAPI_H__
