#include <stdio.h>
#include <string>
#include <queue>
#include "Torque.h"

#include <sqlite3.h>

sqlite3 *db;
std::queue<std::string> queryResults;

static int sqlite3Callback(void *unused, int argc, char **argv, char **azColName)
{
	std::string result = std::string(argv[0]);

	for (int i = 1; i < argc; i++)
	{
		result += std::string("\t");
		result += std::string(argv[i]);
	}
	
	queryResults.push(result);
	return 0;
}

bool sqlite_open(SimObject *obj, int argc, const char *argv[])
{
	if (!argv[1] || db != NULL)
		return 0;

	int rc = sqlite3_open(argv[1], &db);
	if (rc)
	{
		Printf("Unable to open sqlite3 database: %s", sqlite3_errmsg(db));
		return 0;
	}
	else
		return 1;
}

bool sqlite_exec(SimObject *obj, int argc, const char *argv[])
{
	if (!argv[1] || db == NULL)
		return 0;

	while(!queryResults.empty())
		queryResults.pop();

	char *err = 0;
	int rc = sqlite3_exec(db, argv[1], sqlite3Callback, 0, &err);
	if (rc != SQLITE_OK)
	{
		Printf("SQL error: %s", err);
		sqlite3_free(err);
		return 0;
	}
	else
		return 1;
}

const char *sqlite_getResult(SimObject *obj, int argc, const char *argv[])
{
	if(queryResults.empty())
		return "";

	std::string result = queryResults.front();
	queryResults.pop();
	
	char ret[1024];
	sprintf_s(ret, "%s", result.c_str());
	return ret;
}

bool sqlite_close(SimObject *obj, int argc, const char *argv[])
{
	int rc = sqlite3_close(db);
	if (rc != SQLITE_OK)
	{
		Printf("Error closing sqlite3 database: %s", sqlite3_errmsg(db));
		return 0;
	}
	
	db = NULL;
	return 1;
}

DWORD Init(LPVOID args)
{
	if (!torque_init())
		return 0;

	ConsoleFunction(NULL, "sqlite_open", sqlite_open,
		"bool sqlite_open(file) - Opens a database for use", 2, 2);

	ConsoleFunction(NULL, "sqlite_query", sqlite_exec,
		"bool sqlite_exec(stmt) - Executes a SQL statement on the currently opened database", 2, 2);

	ConsoleFunction(NULL, "sqlite_getResult", sqlite_getResult,
		"string sqlite_getResult() - Returns the result of a query one row at a time", 1, 1);

	ConsoleFunction(NULL, "sqlite_close", sqlite_close,
		"bool sqlite_close() - Closes the currently opened database", 1, 1);

	Printf("SQL | Attached");
	return 0;
}

int WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
	if (reason == DLL_PROCESS_ATTACH)
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Init, NULL, 0, NULL);
	return true;
}