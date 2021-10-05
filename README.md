
# Table of Contents

1.  [Election SQLite Loader](#orgbdbe5b7)
    1.  [General](#orga06ddf1)
    2.  [Software License](#orgca5efb3)
    3.  [sqlite3.exe mycondo.db](#org95b8e4f)
    4.  [vote [-i <seats>:<candidates>]](#orgc9a091d)



<a id="orgbdbe5b7"></a>

# Election SQLite Loader


<a id="orga06ddf1"></a>

## General

The following is a "quick & dirty" ballot tallying program for the benefit
of some troubled high rise community in the North East of the USA. Data
entry is optimized for the use of "blind" right-hand PC numeric-pad typing.
This program uses the SQLite data base and is written in C. To examine
collected votes use a program like [SQLITEbrowser](http://sqlitebrowser.org/) on the database file.

This community votes through weighted shares.


<a id="orgca5efb3"></a>

## Software License

The software in this repository is in the public domain, following the
example set by the [SQLite Project](http://www.sqlite.org/copyright.html).


<a id="org95b8e4f"></a>

## sqlite3.exe mycondo.db

Create a database with a table of shares:

    CREATE TABLE "shares" (
    	`apt`	INTEGER,
    	`share`	INTEGER,
    	`code`	BLOB,
    	PRIMARY KEY(apt)
    )


<a id="orgc9a091d"></a>

## vote [-i <seats>:<candidates>]

Initialize the "mycondo.db" database file; This includes changing the
security code (two byte BLOB field) in the table of shares.

