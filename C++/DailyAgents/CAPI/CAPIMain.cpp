#include <stdio.h>
#include <WinSock2.h>	// Must be included before mysql.h
#include <mysql.h>

#define MySQL_USER	"bloomus"
#define MySQL_PASSWORD	"daily"
#define	MySQL_HOST	"flowergarden"
#define MySQL_DATABASE	"DBBloomUs"
int
main(void)
{
	bool	bMySQLLibraryInit = false;
	MYSQL	*mysql = NULL;

__try {
	if (mysql_library_init(0, NULL, NULL)) {
		(void) fprintf(stderr, "Error: mysql_library_init(0, NULL, NULL): Failure\n");

		return 1;
	}
	bMySQLLibraryInit = true;
	if (!(mysql = mysql_init(NULL))) {	// Equivalently,
						// if (!mysql_init(&mysql)) {
						// }
		(void) fprintf(stderr, "Error: mysql_init(NULL): Failure\n");

		return 1;
	}

	if (!mysql_real_connect(mysql,
		MySQL_HOST,
		MySQL_USER,
		MySQL_PASSWORD,
		MySQL_DATABASE,
		0,
		NULL,
		0		// CLIENT_MULTI_RESULTS, CLIENT_MULTI_STATEMENTS
	)) {
		(void) fprintf(stderr,
			"Error: mysql_real_connect(\"" MySQL_HOST "\", \"" MySQL_USER "\", \"" MySQL_DATABASE "\"): Failure: (%u) %s\n"
			,
			mysql_errno(mysql),
			mysql_error(mysql)
		);

		return 1;
	}


	MYSQL_RES	*result = NULL;
__try {
	if (mysql_query(mysql, "SELECT * FROM TDaily")) {
		(void) fprintf(stderr,
			"Error: mysql_query(\"SELECT UUID()\"): Failure: (%u) %s\n"
			,
			mysql_errno(mysql),
			mysql_error(mysql)
		);

		return 1;
	}
	result = mysql_store_result(mysql);
	if (result) {
//		my_ulonglong	affected_rows = mysql_affected_rows(mysql);
		my_ulonglong	num_rows = mysql_num_rows(result);
//		unsigned int	field_count = mysql_field_count(mysql);
		unsigned int	num_fields = mysql_num_fields(result);
		// retrieve rows, then call mysql_free_result(result)
		MYSQL_ROW	row;
		while ((row = mysql_fetch_row(result))) {
			unsigned long	*lengths;
			if (!(lengths = mysql_fetch_lengths(result))) {
				(void) fprintf(stderr,
					"Error: mysql_fetch_lengths(): Failure: (%u) %s\n"
					,
					mysql_errno(mysql),
					mysql_error(mysql)
				);
			}
			for (unsigned int i = 0; i < num_fields; ++i) {
				(void) printf("(%u) [%.*s] ", lengths[i], (int) lengths[i], row[i] ? row[i] : "NULL");
			}
			(void) printf("\n");
		}
	} else {
		unsigned int	errno = mysql_errno(mysql);
#if 1
		if (!errno) {
#else
		if(mysql_field_count(mysql) == 0) {
			// query does not return data
			// (it was not a SELECT)
			my_ulonglong	affected_rows = mysql_affected_rows(mysql);
#endif
		} else {	// mysql_store_result() should have returned data
			(void) fprintf(stderr,
				"Error: mysql_store_result(): Faiure: (%u) %s\n"
				,
				errno,
				mysql_error(mysql)
			);

			return 1;
		}
	}
}
__finally {
	if (result) {
		mysql_free_result(result);
		result = NULL;
	}
}
	

	int	i = 0;
}



__finally {
	if (bMySQLLibraryInit) {
		if (mysql) {
			mysql_close(mysql);
			mysql = NULL;
		}

		mysql_library_end();
	}
}

	return 0;
}
