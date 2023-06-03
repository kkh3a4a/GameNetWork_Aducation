// SQLBindCol_ref.cpp  
// compile with: odbc32.lib  
#include<iostream>
#include<Windows.h>
#include <sqlext.h>  
#include<format>
#include<string>
using namespace std;

#define UNICODE  
#include <sqlext.h>  

#define NAME_LEN 20 


void show_error(SQLHSTMT hstmt) {
    std::wcout.imbue(std::locale("korean"));
    SQLWCHAR sqlState[6];
    SQLINTEGER nativeErr;
    SQLWCHAR errMsg[SQL_MAX_MESSAGE_LENGTH / sizeof(SQLWCHAR)];
    SQLSMALLINT msgLen;

    SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, sqlState, &nativeErr, (errMsg), SQL_MAX_MESSAGE_LENGTH / sizeof(SQLWCHAR), &msgLen);
    wcout<< (L"SQL error : %ls\n", errMsg);
    wcout << endl;
}
void HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode)
{
    SQLSMALLINT iRec = 0;
    SQLINTEGER iError;
    WCHAR wszMessage[1000];
    WCHAR wszState[SQL_SQLSTATE_SIZE + 1];
    if (RetCode == SQL_INVALID_HANDLE) {
        fwprintf(stderr, L"Invalid handle!\n");
        return;
    }
    while (SQLGetDiagRec(hType, hHandle, ++iRec, wszState, &iError, wszMessage, (SQLSMALLINT)(sizeof(wszMessage) / sizeof(WCHAR)), (SQLSMALLINT*)NULL) == SQL_SUCCESS) {
        // Hide data truncated..
        if (wcsncmp(wszState, L"01004", 5)) {
            fwprintf(stderr, L"[%5.5s] %s (%d)\n", wszState, wszMessage, iError);
        }
    }
}
int main() {
    std::wcout.imbue(std::locale("korean"));
    SQLHENV henv;
    SQLHDBC hdbc;
    SQLHSTMT hstmt = 0;
    SQLRETURN retcode;
    SQLCHAR szName[NAME_LEN];
    SQLINTEGER szId, szExp;
    SQLLEN cbName = 0, cbID = 0, cbExp = 0;
    // Allocate environment handle  
    retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

    // Set the ODBC version environment attribute  
    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

        // Allocate connection handle  
        if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
            retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

            // Set login timeout to 5 seconds  
            if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
                SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);

                // Connect to data source  
               //SQLConnect(hdbc, (SQLWCHAR*)L"DB_Master", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);
               retcode = SQLConnect(hdbc, (SQLWCHAR*)L"DB_GameServerProject", SQL_NTS, (SQLWCHAR*)L"2019180046", SQL_NTS, (SQLWCHAR*)L"2019180046", SQL_NTS);
                // Allocate statement handle  
                if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
                    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
                    
                    // DB SELECT 함수 실행 
                    //retcode = SQLExecDirect(hstmt, (SQLWCHAR*)L"EXEC over_exp 30000", SQL_NTS);
                    //retcode = SQLExecDirect(hstmt, (SQLWCHAR*)L"SELECT COUNT(*) FROM information_schema.tables WHERE table_name = 'user_info'", SQL_NTS);
                    retcode = SQLExecDirect(hstmt, (SQLWCHAR*)L"SELECT user_id, user_name, user_exp FROM user_info", SQL_NTS);
                    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

                        // Bind columns 1, 2, and 3  
                        retcode = SQLBindCol(hstmt, 1, SQL_INTEGER, &szId, 12, &cbID);
                        retcode = SQLBindCol(hstmt, 2, SQL_C_CHAR, szName, NAME_LEN, &cbName);
                        retcode = SQLBindCol(hstmt, 3, SQL_INTEGER, &szExp, 12, &cbExp);

                        // Fetch and print each row of data. On an error, display a message and exit. 
                        for (int i = 0; ; i++) {
                            retcode = SQLFetch(hstmt);  // 데이터 해석
                            if (retcode == SQL_ERROR)
                                cout << "Fetch error" << endl;
                            if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
                            {
                                cout << std::format("{} : {} {} {}\n", i + 1, szId, reinterpret_cast<char*>(szName), szExp);
                            }
                            else
                                break;
                        }
                    }
                    else
                    {
                        HandleDiagnosticRecord(hstmt, SQL_HANDLE_DBC,retcode);
                        show_error(hstmt);
                    }

                    // Process data  
                    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
                        SQLCancel(hstmt);
                        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
                    }

                    SQLDisconnect(hdbc);
                }

                SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
            }
        }
        SQLFreeHandle(SQL_HANDLE_ENV, henv);
    }
}