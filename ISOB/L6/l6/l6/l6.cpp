#include <iostream>
#include <windows.h>
#include <sqlext.h>

void executeUnsafeQuery(SQLHDBC hDbc, const std::string& userInput) {
    std::string query = "SELECT * FROM users WHERE username = '" + userInput + "'";

    SQLHSTMT hStmt;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    SQLExecDirect(hStmt, (SQLCHAR*)query.c_str(), SQL_NTS);

    std::cout << "Executed unsafe query: " << query << std::endl;
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

void executeSafeQuery(SQLHDBC hDbc, const std::string& userInput) {
    SQLHSTMT hStmt;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    SQLCHAR* query = (SQLCHAR*)"SELECT * FROM users WHERE username = ?";
    SQLPrepare(hStmt, query, SQL_NTS);
    SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, (SQLPOINTER)userInput.c_str(), 0, NULL);
    SQLExecute(hStmt);

    std::cout << "Executed safe query with parameterized input." << std::endl;
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

int main() {
    SQLHENV hEnv;
    SQLHDBC hDbc;

    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
    SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);

    SQLConnect(hDbc, (SQLCHAR*)"DSN=OracleDB;UID=user;PWD=password", SQL_NTS, NULL, 0, NULL, 0);

    std::string input;
    std::cout << "Enter username: ";
    std::cin >> input;

    executeUnsafeQuery(hDbc, input); // Атака
    executeSafeQuery(hDbc, input);   // Защита

    SQLDisconnect(hDbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
    SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
    return 0;
}
