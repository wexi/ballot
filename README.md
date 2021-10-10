
# Table of Contents

1.  [General](#orgd61aa36)
2.  [Software License](#org407f80f)
3.  [sqlite3 mycondo.db](#org7d427cd)
4.  [vote [-i <seats>:<candidates>]](#orge519ec8)
5.  [Examples](#orga08e9bf)



<a id="orgd61aa36"></a>

# General

The following is a "quick & dirty" ballot tallying program for the benefit
of some troubled high rise community in the North East of the USA.  This
community votes through weighted shares. Data entry is optimized for the
use of "blind" right-hand PC numeric-pad typing.  This program uses the
SQLite data base and is written in C. To examine the collected votes use a
program like [SQLITEbrowser](http://sqlitebrowser.org/) on the database file.

The program (vote) can be built on Linux also for Windows using [MXE (M
cross environment](https://mxe.cc/) (vote.exe). See the Makefile.


<a id="org407f80f"></a>

# Software License

The software in this repository is in the public domain, following the
example set by the [SQLite Project](http://www.sqlite.org/copyright.html).


<a id="org7d427cd"></a>

# sqlite3 mycondo.db

Create a table of shares in the database and fill it with the apt number
and the share value pairs of your community; Ignore the code blob.

    CREATE TABLE shares (
    	apt	INTEGER PRIMAY KEY,
    	share	INTEGER,
    	code	BLOB
    )


<a id="orge519ec8"></a>

# vote [-i <seats>:<candidates>]

Initialize the database file; This includes changing the security code in
the table of shares with a two byte random blob. A new votes table is then
created with the following fields:

    CREATE TABLE votes (
          apt INTEGER PRIMAY KEY,
          vtime INTEGER,
          can1 INTEGER DEFAULT 0,
          ...
    )

vtime is the "unix time" when accepting the apt no vote and it is captured
for forensic purposes only. can1, etc. are the total number of shares cast
for the first candidate, etc.


<a id="orga08e9bf"></a>

# Examples

Initialize and vote:

    $ ./vote -i 2:4
    mycondo.db loader. Exit by Ctrl-Z (EOF). EW Oct  7 2021.
    mycondo: apts=1000 shares=1000005 
    Enter negative apt to delete the apt vote.
    Seats = 2, Candidates = 4
    
    Voted: apts=0 shares=N/A c1=N/A c2=N/A c3=N/A 
    Apt.Can1.Can2... 1901.1.2
    Ballot: share=1469 code=0x09AF 
    
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

