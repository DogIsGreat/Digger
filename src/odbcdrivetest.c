#include <stdio.h>
#include <stdlib.h>
#include <sql.h>
#include <sqlext.h>
#include <string.h>

int main()
{
    printf(" ODBC Libraries Linked. ");
    SQLHENV env;
    SQLHDBC dbc;
    SQLHSTMT stmt;
    SQLRETURN ret; /* ODBC API return status */
    char name[NAME_LENGTH];
    char *names[MAX_NAMES];
    int name_count = 0;

    // Allocate an environment handle
    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);

    // Set the ODBC version to 3.0
    SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void *)SQL_OV_ODBC3, 0);

    // Allocate a connection handle
    SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);

    // Connect to the database
    SQLConnect(dbc, (SQLCHAR *) "DSN=YourDSNName", SQL_INTS, (SQLCHAR *)"user", SQL_NTS, (SQLCHAR *)"password", SQL_NTS);



    return 0;
}