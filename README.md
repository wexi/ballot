
# Table of Contents

<<<<<<< HEAD
1.  [General](#orgf619053)
2.  [Software License](#org6336cda)
3.  [TODO](#orgf1b9359)
4.  [sqlite3.exe mycondo.db](#orgec1e111)
5.  [vote [-i <seats>:<candidates>]](#org0f97000)
6.  [Examples](#orgc50460f)



<a id="orgf619053"></a>

# General

The following is a "quick & dirty" ballot tallying program for the benefit
of some troubled high rise community in the North East of the USA.  This
community votes through weighted shares. Data entry is optimized for the
use of "blind" right-hand PC numeric-pad typing.  This program uses the
SQLite data base and is written in C. To examine the collected votes use a
program like [SQLITEbrowser](http://sqlitebrowser.org/) on the database file.

The program (vote) can be built on Linux also for Windows using [MXE (M
cross environment](https://mxe.cc/) (vote.exe). See the Makefile.


<a id="org6336cda"></a>

# Software License

The software in this repository is in the public domain, following the
example set by the [SQLite Project](http://www.sqlite.org/copyright.html).


<a id="orgf1b9359"></a>

# TODO

Add vote logging. Not done since there is no ready made C library for
logging that works the same in Linux and in Windows. [log4c](http://log4c.sourceforge.net/) is a good
candidate but it currently has a build issue on MXE.


<a id="orgec1e111"></a>

# sqlite3.exe mycondo.db

Create a database with a table of shares:

    CREATE TABLE "shares" (
    	`apt`	INTEGER,
    	`share`	INTEGER,
    	`code`	BLOB,
    	PRIMARY KEY(apt)
    )


<a id="org0f97000"></a>

# vote [-i <seats>:<candidates>]

Initialize the "mycondo.db" database file; This includes changing the
security code (two byte BLOB field) in the table of shares. The security
code is just being displayed.


<a id="orgc50460f"></a>

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
=======
1.  [Election SQLite Loader](#org615900e)
    1.  [General](#org4dcbcaa)
    2.  [Software License](#orgf6862c2)
    3.  [sqlite3.exe mycondo.db](#org88d7d3f)
    4.  [vote [-i <seats>:<candidates>]](#org54a64fb)



<a id="org615900e"></a>

# Election SQLite Loader


<a id="org4dcbcaa"></a>

## General

The following is a "quick & dirty" ballot tallying program for the benefit
of some troubled high rise community in the North East of the USA. Data
entry is optimized for the use of "blind" right-hand PC numeric-pad typing.
This program uses the SQLite data base and is written in C. To examine
collected votes use a program like [SQLITEbrowser](http://sqlitebrowser.org/) on the database file.

This community votes through weighted shares.


<a id="orgf6862c2"></a>

## Software License

The software in this repository is in the public domain, following the
example set by the [SQLite Project](http://www.sqlite.org/copyright.html).


<a id="org88d7d3f"></a>

## sqlite3.exe mycondo.db

Create your own database including "shares" & "votes" tables. To be
continued.


<a id="org54a64fb"></a>

## vote [-i <seats>:<candidates>]

Initialize program.
>>>>>>> 4aacb878e94fa392e870d3c64effacc70eae68d4

