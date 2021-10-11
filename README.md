
# Table of Contents

1.  [General](#orgeb28208)
2.  [Software License](#orgaee87f8)
3.  [Prerequisite](#org59f3670)
4.  [Program initialization](#org7db48d0)
5.  [Examples](#org1a1f5a6)



<a id="orgeb28208"></a>

# General

`vote` is a quick ballot tallying program that was written for some
troubled high rise community in the North East of the USA.  This community
votes through weighted shares. Data entry is optimized for the use of blind
right-hand PC numeric-pad typing.  `vote` uses the SQLite database library
(sqlite3 version) and is written in C. To examine the database file
collected ballots use [SQLITEbrowser](http://sqlitebrowser.org/) or a similar tool.

`vote.exe`, the equivalent Microsoft Windows version, is built on Linux
using [MXE (M cross environment)](https://mxe.cc/). Here is the latest [Windows binary release](./Windows-binary-release.zip).


<a id="orgaee87f8"></a>

# Software License

The software in this repository is in the public domain following the
example set by the [SQLite Project](http://www.sqlite.org/copyright.html).


<a id="org59f3670"></a>

# Prerequisite

If not provided, create the SQLite data base file `condo.db` (this filename
is set in the Makefile) with the following table of shares:

    CREATE TABLE shares (
    	apt	INTEGER PRIMAY KEY,
    	share	INTEGER,
    	code	BLOB
    );

Fill the `shares` table with the your community `apt` (apartment number)
and `share` value pairs (ignore the `code` column).


<a id="org7db48d0"></a>

# Program initialization

Run `vote -i <seats>:<candidates>[:+]` <sup><a id="fnr.1" class="footref" href="#fn.1" role="doc-backlink">1</a></sup> to initialize the `condo.db`
database file. If the optional `:+` argument is added, the security `code~s
in ~shares` would be initialized with random two-byte BLOBs. These BLOBs
can be useful with the printed material mailed to the voters to enhance the
election security. It can substitute the need for using an expensive
special paper to prevent [Ballot Stuffing](https://ballotpedia.org/Ballot_stuffing). You can then import the (apt,
code) pairs to a `secure.csv` spreadsheet as follows:

    $ sqlite3 condo.db
    SQLite version 3.34.1 2021-01-20 14:10:07
    Enter ".help" for usage hints.
    sqlite> .output secure.csv
    sqlite> .headers on
    sqlite> .mode csv
    sqlite> select apt, hex(code) as code from shares order by apt;
    sqlite> .quit

An empty table of `votes` would then be created or recreated if it already
exists! The `votes` table has the following columns:

    CREATE TABLE votes (
          apt INTEGER PRIMAY KEY,
          vtime INTEGER,
          can1 INTEGER DEFAULT 0,
          ...
    );

vtime is the "unix time" of enering the apt vote; It is captured for
forensic purposes only.


<a id="org1a1f5a6"></a>

# Examples

Initialize (+ security codes):

    Microsoft Windows [Version 10.0.19043.1237]
    (c) Microsoft Corporation. All rights reserved.
    
    C:\Users\ixew>cd Desktop
    
    C:\Users\ixew\Desktop>vote -i 2:4:+
    condo.db loader. Exit by Ctrl-Z (EOF). EW Oct 11 2021.
    condo: apts=614 shares=1000005
    Enter negative apt to delete an apt vote.
    Seats = 2, Candidates = 4
    
    Voted: apts=0 shares=N/A c1=N/A c2=N/A c3=N/A
    Apt.Can1.Can2... ^Z
    EOF
    condo.db changed.
    Press Enter to end...
    
    C:\Users\ixew\Desktop>

Use:

    condo.db loader. Exit by Ctrl-Z (EOF). EW Oct 11 2021.
    Enter negative apt to delete an apt vote.
    Seats = 2, Candidates = 4
    
    Voted: apts=0 shares=N/A c1=N/A c2=N/A c3=N/A
    Apt.Can1.Can2... 3121.1.2
    Ballot: share=1371 code=0xDD17
    
    Voted: apts=1 shares=1371 c1=1371 c2=1371 c3=0
    Apt.Can1.Can2... 3120.3.4
    Ballot: share=2150 code=0xFA9E
    
    Voted: apts=2 shares=3521 c3=2150 c4=2150 c1=1371
    Apt.Can1.Can2... 2
    APT UNKNOWN
    
    Voted: apts=2 shares=3521 c3=2150 c4=2150 c1=1371
    Apt.Can1.Can2... 7.5.6
    Ballot: share=1340 code=0xED04
    ILL CAN
    
    Voted: apts=2 shares=3521 c3=2150 c4=2150 c1=1371
    Apt.Can1.Can2... 7
    Ballot: share=1340 code=0xED04
    
    Voted: apts=3 shares=4861 c3=2150 c4=2150 c1=1371
    Apt.Can1.Can2... -7
    DELETED APT NO 7
    
    Voted: apts=2 shares=3521 c3=2150 c4=2150 c1=1371
    Apt.Can1.Can2... ^Z
    EOF
    condo.db changed.
    Press Enter to end...


# Footnotes

<sup><a id="fn.1" href="#fnr.1">1</a></sup> To open a Windows Terminal type `WIN+R` and then the `cmd` command.
