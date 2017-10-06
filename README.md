# BLsql
Implementation of SQLite for Blockland

## Functions
`sqlite_open(database)` - Opens a .db file to perform SQL commands on

`sqlite_query(statement [, return [, file]])` - Performs a SQL command on the opened database

If return is true, it will return the query result in a string with rows seperated by a newline character.
If file is specified, it will query the database in that file.

`sqlite_getResult()` - Returns one row at a time with columns seperated by a tab character

`sqlite_getResultCount()` - Returns the number of available results from a query

`sqlite_close()` - Closes the opened database

## Examples
#### Basic table creation, insertion, and query
```
sqlite_open("config/server/Players.db");
sqlite_query("CREATE TABLE Players(ID INT PRIMARY KEY NOT NULL, BLID INT NOT NULL, Name TEXT NOT NULL);");
sqlite_query("INSERT INTO Players (ID, BLID, Name) VALUES (1, 25351, 'Eagle517');");

sqlite_query("SELECT * FROM Players WHERE BLID=25351");
sqlite_getResult(); -> "1\t25351\tEagle517"
```

#### Looping through a query result

| ID            | BLID          | Name     |
|:-------------:|:-------------:|:--------:|
| 1             | 25351         | Eagle517 |
| 2             | 0             | Badspot  |
| 3             | 103           | Rotondo  |

Assume that this table, called Players, is located in config/server/Players.db
```
sqlite_open("config/server/Players.db");
sqlite_query("SELECT * FROM Players");

while((%row = sqlite_getResult()) !$= "")
{
    echo(%row);
}

sqlite_close();
```

The above code will produce the following output
```
1\t25351\tEagle517
2\t0\tBadspot
3\t103\tRotondo
```

#### Speeding up multiple queries
In some cases, you may need to update multiple rows of a table or insert multiple rows at the same time. Normally, this will result in Blockland freezing for a few moments. To fix this, wrap your queries with the following two statements.
```
sqlite_query("BEGIN TRANSACTION");
//your queries
sqlite_query("END TRANSACTION");
```
This will result in a significant increase in speed.
