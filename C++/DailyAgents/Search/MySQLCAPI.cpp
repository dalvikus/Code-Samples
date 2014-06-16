#include "MySQLCAPI.h"

#define MySQL_DAILY_PASSWORD	"daily"

MySQLCAPI::MySQLCAPI(
	const char	*username,
	const char	*password,
	int		*lpiStatus
)
{
	__bMySQLLibraryInit	= false;
	__mysql			= NULL;
	__result		= NULL;
	__dataRemoved		= 0;
	__sizeOfDataRemoved	= 0;

	*lpiStatus = 0;
__try {
	if (mysql_library_init(0, NULL, NULL)) {
//		(void) fprintf(stderr, "Error: mysql_library_init(0, NULL, NULL): Failure\n");

		*lpiStatus = -1;
		return;
	}
	__bMySQLLibraryInit = true;
	if (!(__mysql = mysql_init(NULL))) {	// Equivalently,
						// if (!mysql_init(&mysql)) {
						// }
//		(void) fprintf(stderr, "Error: mysql_init(NULL): Failure\n");

		*lpiStatus = -2;
		return;
	}

	bool	bDailyUser = strcmp(username, MySQL_DAILY_USER) == 0 ? true : false;
	if (!mysql_real_connect(__mysql,
		MySQL_HOST,
		bDailyUser ? MySQL_DAILY_USER : username,
		bDailyUser ? MySQL_DAILY_PASSWORD : password,
		MySQL_BLOOMUS_DATABASE,
		0,
		NULL,
		0		// CLIENT_MULTI_RESULTS, CLIENT_MULTI_STATEMENTS
	)) {
#if 0
		(void) fprintf(stderr,
			"Error: mysql_real_connect(\"" MySQL_HOST "\", \"" MySQL_USER "\", \"" MySQL_DATABASE "\"): Failure: (%u) %s\n"
			,
			mysql_errno(mysql),
			mysql_error(mysql)
		);
#endif

		*lpiStatus = -3;
		return;
	}
}

__finally {
	if (*lpiStatus != 0) {	// There is a error!
		if (__bMySQLLibraryInit) {
			if (__mysql) {
				mysql_close(__mysql);
				__mysql = NULL;
			}

			mysql_library_end();

			__bMySQLLibraryInit = false;
		}
	}
}
}

MySQLCAPI::~MySQLCAPI()
{
	if (__bMySQLLibraryInit) {
		if (__mysql) {
			mysql_close(__mysql);
			__mysql = NULL;
		}
		if (__result) {
			mysql_free_result(__result);
			__result = NULL;
		}

		mysql_library_end();

		__bMySQLLibraryInit = false;
	}

	if (__dataRemoved) {
		free(__dataRemoved);
		__dataRemoved = 0;
		__sizeOfDataRemoved = 0;
	}
}
