#include <stdio.h>
#include <stdlib.h>
#include <sql.h>
#include <sqlext.h>
#include <string.h>

#define MAX_NAMES 100
#define NAME_LENGTH 50

// odbc error handling for debuggin.
void show_odbc_error(SQLHANDLE handle, SQLSMALLINT type){
    SQLCHAR sqlstate[1024];
    SQLCHAR message[1024];
    if (SQLError(handle, type, NULL, sqlstate, NULL, message, sizeof(message), NULL) == SQL_SUCCESS){
        printf("ODBC error: %s - %s\n", sqlstate, message);
    }
}

void removeWhitespacesAndJoin(SQLHSTMT stmt){
    // Remove whitespaces from BusinessKeys in TableA and TableB
    SQLExecDirect(stmt, (SQLCHAR*)"UPDATE TableA SET BusinessKey = REPLACE(BusinessKey, ' ', '')", SQL_NTS);
    SQLExecDirect(stmt, (SQLCHAR*)"UPDATE TableB SET BusinessKey = REPLACE(BusinessKey, ' ', '')", SQL_NTS);

    // Execute the join query
    SQLExecDirect(stmt, (SQLCHAR*)"SELECT a.BusinessKey, a.DataFieldA, b.DataFieldB FROM TableA a JOIN TableB b ON a.BusinessKey = b.BusinessKey", SQL_NTS);

    // Assuming BusinessKey, DataFieldA, and DataFieldB are all strings
    char businessKey[100], dataFieldA[100], dataFieldB[100];
    while (SQLFetch(stmt) == SQL_SUCCESS){
        SQLGetData(stmt, 1, SQL_C_CHAR, businessKey, sizeof(businessKey), NULL);
        SQLGetData(stmt, 2, SQL_C_CHAR, dataFieldA, sizeof(dataFieldA), NULL);
        SQLGetData(stmt, 3, SQL_C_CHAR, dataFieldB, sizeof(dataFieldB), NULL);

        printf("BusinessKey: %s, DataFieldA: %s, DataFieldB: %s\n", businessKey, dataFieldA, dataFieldB);
    }
}

// Function to Store Vectors as Scalar Columns
void storVectorAsScalars(SQLHSTMT stmt, int id, double vec[3]){
    SQLCHAR sql[256];
    sprintf((char *)sql, "INSERT INTO VectorTableScalar (ID, Dimension1, Dimesion2, Dimension3) VALUES (%d, %f, %f, %f)", id, vec[0], vec[1], vec[2]);
    SQLExecDirect(stmt, sql, SQL_NTS);
}

// Function to Store Vectors as a Serialized JSON Object
void storeVectorsAsJson(SQLHSTMT stmt, int id, double* vec, int vecLength){
    // Start building the JSON string
    char vectorData[1024] = "[";
    char buffer[64];
    for (int i= 0; i < vecLength; ++i){
        sprintf(buffer, "%s%f", (i > 0 ? ", " : ""), vec[i]);
        strcat(vectorData, buffer);
    }
    strcat(vectorData, "]");

    // Prepare SQL statement with placeholders for ID and VectorData
    SQLCHAR *sql = (SQLCHAR*)"INSERT INTO VectorTableJson (ID, VectorData) VALUES (?, ?)";
    SQLPrepare(stmt, sql, SQL_NTS);

    // Bind ID
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &id, 0, NULL);

    // Bind VectorData as JSON string
    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, strlen(vectorData), 0, vectorData, 0, NULL);

    // Execute
    SQLExecute(stmt);
}


int main()
{
    printf(" ODBC Libraries Linked. ");
    SQLHENV env;
    SQLHDBC dbc;
    SQLHSTMT stmt;
    SQLRETURN ret; /* ODBC API return status */
    char name[NAME_LENGTH];
    char *names[MAX_NAMES];
    int names_count = 0;

    // Allocate an environment handle
    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);

    // Set the ODBC version to 3.0
    SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void *)SQL_OV_ODBC3, 0);

    // Allocate a connection handle
    SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);

    // Connect to the database
    SQLConnect(dbc, (SQLCHAR *) "DSN=YourDSNName", SQL_NTS, (SQLCHAR *)"user", SQL_NTS, (SQLCHAR *)"password", SQL_NTS);

    // Allocate a statement Handle
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    // Execute a query to retrieve names
    SQLExecDirect(stmt, (SQLCHAR *)"SELECT Name FROM People", SQL_NTS);

    // Fetch and store names
    while (SQLFetch(stmt) == SQL_SUCCESS){
        SQLGetData(stmt, 1, SQL_C_CHAR, name, sizeof(name), NULL);
        names[names_count] = strdup(name);
        names_count++;
    }

    // Sort names alphabetically
    qsort(names, names_count, sizeof(char *), (int (*)(const void *, const void *))strcmp);

    // Print sorted names
    for (int i = 0; i < names_count; i++){
        printf("%s\n", names[i]);
        free(names[i]); // Free the dynamically allocated memory
    }

    // Cleanup
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    SQLDisconnect(dbc);
    SQLFreeHandle(SQL_HANDLE_DBC, dbc);
    SQLFreeHandle(SQL_HANDLE_ENV, env);

    return 0;

}