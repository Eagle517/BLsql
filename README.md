# BLsql
Implementation of SQLite for Blockland

## Functions
`sqlite_open(database)` - Opens a .db file to perform SQL commands on

`sqlite_query(statement)` - Performs a SQL command on the opened database

`sqlite_getResult()` - Returns one row at a time with columns seperated by a tab character

`sqlite_close()` - Closes the opened database

## Example
```
sqlite_open("config/server/Players.db");
sqlite_query("CREATE TABLE Players(ID INT PRIMARY KEY NOT NULL, BLID INT NOT NULL, Name TEXT NOT NULL);");
sqlite_query("INSERT INTO Players (ID, BLID, Name) VALUES (1, 25351, 'Eagle517');

sqlite_query("SELECT * FROM Players WHERE BLID=25351");
sqlite_getResult(); -> "1\t25351\tEagle517"
```
