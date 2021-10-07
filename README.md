
# Table of Contents

1.  [General](#orga9e2cf5)
2.  [Software License](#org52c9761)
3.  [TODO](#org5daf69d)
4.  [sqlite3.exe mycondo.db](#orgc4f7100)
5.  [vote [-i <seats>:<candidates>]](#orgbcb0c5f)
6.  [Examples](#org841f668)



<a id="orga9e2cf5"></a>

# General

The following is a "quick & dirty" ballot tallying program for the benefit
of some troubled high rise community in the North East of the USA.  This
community votes through weighted shares. Data entry is optimized for the
use of "blind" right-hand PC numeric-pad typing.  This program uses the
SQLite data base and is written in C. To examine the collected votes use a
program like [SQLITEbrowser](http://sqlitebrowser.org/) on the database file.

The program (vote) can be built on Linux also for Windows using [MXE (M
cross environment](https://mxe.cc/) (vote.exe). See the Makefile.


<a id="org52c9761"></a>

# Software License

The software in this repository is in the public domain, following the
example set by the [SQLite Project](http://www.sqlite.org/copyright.html).


<a id="org5daf69d"></a>

# TODO

Add vote logging. Not done since there is no ready made C library for
logging that works the same in Linux and in Windows. [log4c](http://log4c.sourceforge.net/) is a good
candidate but it currently has a build issue on MXE.


<a id="orgc4f7100"></a>

# sqlite3.exe mycondo.db

Create a database with a table of shares:

    CREATE TABLE "shares" (
    	`apt`	INTEGER,
    	`share`	INTEGER,
    	`code`	BLOB,
    	PRIMARY KEY(apt)
    )


<a id="orgbcb0c5f"></a>

# vote [-i <seats>:<candidates>]

Initialize the "mycondo.db" database file; This includes changing the
security code (two byte BLOB field) in the table of shares. The security
code is just being displayed.


<a id="org841f668"></a>

# Examples

Initialize and vote:

    $ ./vote -i 2:4
    mycondo.db loader. Exit by Ctrl-Z (EOF). EW Oct  7 2021.
    mycondo: apts=1000 shares=1000005 
    Enter negative apt to delete the apt vote.
    Seats = 2, Candidates = 4
    
    Voted: apts=0 shares=N/A c1=N/A c2=N/A c3=N/A 
    Apt.Can1.Can2... 1901.1.2
    Ballot: share=1469 code=0xBEC5 
    
    Apt.Can1.Can2... EOF
    mycondo.db changed.

Just Use:

    
    $ ./vote
    mycondo.db loader. Exit by Ctrl-Z (EOF). EW Oct  7 2021.
    Enter negative apt to delete the apt vote.
    Seats = 2, Candidates = 4
    
    Voted: apts=1 shares=1469 c1=1469 c2=1469 c3=0 
    Apt.Can1.Can2... 1901 
    APT ALREADY VOTED
    
    Voted: apts=1 shares=1469 c1=1469 c2=1469 c3=0 
    Apt.Can1.Can2... -1901
    DELETED APT NO 1901
    
    Voted: apts=0 shares=N/A c1=N/A c2=N/A c3=N/A 
    Apt.Can1.Can2... EOF
    mycondo.db changed.

