
# Table of Contents

1.  [General](#orgfe10ef6)
2.  [Software License and Legal Disclaimer](#org01aa9ec)
3.  [Prerequisite](#org5031d6a)
4.  [Program initialization](#org1c727b8)



<a id="orgfe10ef6"></a>

# General

`vote` is a quick ballot tallying program that was originally written in
2014 for some troubled high rise community in the North East of the USA.
This community votes through weighted shares. Data entry is optimized for
the use of blind right-hand PC numeric-pad typing.  `vote` uses the SQLite
database library (sqlite3 version) and is written in C. To examine the
database file collected ballots use [SQLITEbrowser](http://sqlitebrowser.org/) or a similar tool.

`vote.exe`, the equivalent Microsoft Windows version, is built on Linux
using [MXE (M cross environment)](https://mxe.cc/). Here is the latest [Windows binary release](./Windows-binary-release.zip).


<a id="org01aa9ec"></a>

# Software License and Legal Disclaimer

The software in this repository is in the public domain following the model
set by the [SQLite Project](http://www.sqlite.org/copyright.html). The software is offered “as-is”, without
warranty, and disclaiming liability for damages resulting from its use.


<a id="org5031d6a"></a>

# Prerequisite

If not provided, create the SQLite data base file `condo.db` (this filename
is set in the Makefile) with the following table of shares:

    CREATE TABLE "shares" (
    	"apt"	INTEGER,
    	"share"	INTEGER,
    	"code"	INTEGER UNIQUE,
    	PRIMARY KEY("apt")
    );

Fill the `shares` table with your community `apt` (apartment number) /
`share` value pairs. Ignore the `code` column, it holds random numbers
needed by the **secret** tallying method.


<a id="org1c727b8"></a>

# Program initialization

Run `vote -i <seats>:<candidates>[:x]` to initialize the program's
`condo.db` SQLite database file; this requires opening a terminal<sup><a id="fnr.1" class="footref" href="#fn.1" role="doc-backlink">1</a></sup>.

The optional third argument, lower-case `:x` or upper-case `:X`,
initializes the program for secret (anonymous) apartment numbers entry
rather than their real numbers; this separation is needed during sensitive
(publicly observed) tallying. **NOTE!** Only when using the upper case `:X`
argument would actually new secret apartment numbers be
generated. Lower-case `:x` argument initializes the program to use the
preexisting secret numbers. This is convenient when needing to switch
between **secret** and **real** apartment numbering tally.

An "anon" table is created in `condo.db` to relate the **real** apartment
numbers ("apt") with their **secret** counterparts ("unit"); example:

    sqlite> .mode col
    sqlite> .head on
    sqlite> .output "anon.txt"   
    sqlite> select * from anon order by apt;
    apt  unit
    ---  ----
    7    69  
    8    468 
    9    50  
    10   81  
    11   256

A "bans" table of **real** apartment numbers can be provided in `condo.db` to
reject votes from apartments not "in good standing" or from apartments
voting electronically.

An empty table of `votes` would be created or **recreated if it already
exists**. The `votes` table has the following structure:

    CREATE TABLE votes (
          apt INTEGER PRIMAY KEY,
          vtime INTEGER,
          can1 INTEGER DEFAULT 0,
          ...
    );

vtime is the "unix time" of entering the apt vote; It is captured for
forensic purposes only.


# Footnotes

<sup><a id="fn.1" href="#fnr.1">1</a></sup> To open a Windows Terminal type `WIN+R` and then the `cmd` command.
