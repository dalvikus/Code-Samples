#include "MySQLCAPI.h"

#define MySQL_USER		"root"
#define MySQL_PASSWORD		"hello, world"
#define	MySQL_HOST		"flowergarden"
#define MySQL_BLOOMUS_DATABASE	"DBBloomUs"	// No big deal!
						// Because we make clear a database in SQL query as
						//	mysql.user

MySQLCAPI::MySQLCAPI(
	int	*lpiStatus
)
{
	__bMySQLLibraryInit	= false;
	__mysql			= NULL;

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

	if (!mysql_real_connect(__mysql,
		MySQL_HOST,
		MySQL_USER,
		MySQL_PASSWORD,
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

		mysql_library_end();

		__bMySQLLibraryInit = false;
	}
}
