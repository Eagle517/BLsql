<<<<<<< HEAD
#include <stdio.h>
#include <string>
#include <queue>
#include "Torque.h"

#include <sqlite3.h>

static sqlite3 *db;
static std::queue<std::string> queryResults;

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

static bool sqlite_open(SimObject *obj, int argc, const char *argv[])
{
	if (!argv[1])
		return 0;

	if(db != NULL)
		sqlite3_close(db);

	int rc = sqlite3_open(argv[1], &db);
	if (rc)
	{
		Printf("Unable to open sqlite3 database: %s", sqlite3_errmsg(db));
		return 0;
	}
	else
		return 1;
}

static const char *sqlite_exec(SimObject *obj, int argc, const char *argv[])
{
	if (!argv[1])
		return 0;

	while (!queryResults.empty())
		queryResults.pop();

	char *err = 0;
	int rc;

	if (argc == 4 && argv[3] && _stricmp(argv[3], "") != 0)
	{
		sqlite3 *file;
		if (sqlite3_open(argv[3], &file))
		{
			Printf("Unable to open sqlite3 database: %s", sqlite3_errmsg(db));
			return "0";
		}
		rc = sqlite3_exec(file, argv[1], sqlite3Callback, 0, &err);
		sqlite3_close(file);
	}
	else
	{
		if(db == NULL)
			return "0";
		rc = sqlite3_exec(db, argv[1], sqlite3Callback, 0, &err);
	}

	if (rc != SQLITE_OK)
	{
		Printf("SQL error: %s", err);
		sqlite3_free(err);
		return "0";
	}
	else
	{
		if ( argc >= 3 && (!_stricmp(argv[2], "true") || !_stricmp(argv[2], "1") || (0 != atoi(argv[2]))))
		{
			std::string result = "";
			int size = queryResults.size()-1;
			if(size < 0)
				return "";

			if(size > 0)
			{
				for (int i = 0; i < size; i++)
				{
					result.append(std::string(queryResults.front() + "\n"));
					queryResults.pop();
				}
			}
			result.append(queryResults.front());
			queryResults.pop();
			return result.c_str();
		}
		else
			return "1";
	}
}

static const char *sqlite_getResult(SimObject *obj, int argc, const char *argv[])
{
	if(queryResults.empty())
		return "";

	std::string result = queryResults.front();
	queryResults.pop();
	
	char ret[1024];
	sprintf_s(ret, "%s", result.c_str());
	return ret;
}

static int sqlite_getResultCount(SimObject *obj, int argc, const char *argv[])
{
	return queryResults.size();
}

static bool sqlite_close(SimObject *obj, int argc, const char *argv[])
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

bool init()
{
	if (!torque_init())
		return 0;

	ConsoleFunction(NULL, "sqlite_open", sqlite_open,
		"bool sqlite_open(file) - Closes the current database and opens a database for use", 2, 2);

	ConsoleFunction(NULL, "sqlite_query", sqlite_exec,
		"bool sqlite_exec(stmt [, bool return [, string file]]) - Executes a SQL statement on the currently opened database if no file is specified", 2, 4);

	ConsoleFunction(NULL, "sqlite_getResult", sqlite_getResult,
		"string sqlite_getResult() - Returns the result of a query one row at a time", 1, 1);

	ConsoleFunction(NULL, "sqlite_getResultCount", sqlite_getResultCount,
		"int sqlite_getResultCount() - Returns the number of results still available", 1, 1);

	ConsoleFunction(NULL, "sqlite_close", sqlite_close,
		"bool sqlite_close() - Closes the currently opened database", 1, 1);

	Printf("SQL | Attached");
	return true;
}

int WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
	if (reason == DLL_PROCESS_ATTACH)
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)init, NULL, 0, NULL);
	return true;
}
=======
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

int sqlite_getResultCount(SimObject *obj, int argc, const char *argv[])
{
	return queryResults.size();
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

	ConsoleFunction(NULL, "sqlite_getResultCount", sqlite_getResultCount,
		"int sqlite_getResultCount() - Returns the number of results still available", 1, 1);

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
>>>>>>> origin/master
