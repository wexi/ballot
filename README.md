
# Table of Contents

1.  [General](#orge81ffee)
2.  [Software License and Legal Disclaimer](#org6941afa)
3.  [Prerequisite](#org9aef7ce)
4.  [Program initialization](#orgb11752d)



<a id="orge81ffee"></a>

# General

`vote` is a quick paper ballot tallying program that was originally written
in 2014 for some troubled high rise community in the North East of the USA.
This community votes through weighted shares. Data entry is optimized for
numeric-pad PC typing. `vote` uses the SQLite database library (sqlite3
version) and is written in C. To examine the database file collected
ballots use [SQLITEbrowser](http://sqlitebrowser.org/) or a similar tool.

`vote.exe`, the equivalent Microsoft Windows version, is built on Linux
using [MXE (M cross environment)](https://mxe.cc/). Here is the latest [Windows binary release](./Windows-binary-release.zip).


<a id="org6941afa"></a>

# Software License and Legal Disclaimer

The software in this repository is in the public domain following the model
set by the [SQLite Project](http://www.sqlite.org/copyright.html). The software is offered “as-is”, without
warranty, and disclaiming liability for damages resulting from its use.


<a id="org9aef7ce"></a>

# Prerequisite

If not provided, create the SQLite data base file `condo.db` (default
Makefile DBNAME) with the following table of shares:

    CREATE TABLE "shares" (
    	"apt"	INTEGER,
    	"share"	INTEGER,
    	"code"	INTEGER UNIQUE,
    	PRIMARY KEY("apt")
    );

Fill the `shares` table with your community corresponding `apt` (apartment
number) and `share` value pairs. Ignore the `code` column, a column with
unique random numbers which are necessary for the **secret** tallying method.

`condo.db` can also include two tables, "evotes" (those voting
electronically) and "arrears" (those not in good standing), with an "apt"
column of apartment numbers which are not eligible to vote or can vote
electronically only.


<a id="orgb11752d"></a>

# Program initialization

Run `vote -i <seats>:<candidates>[:x]` to initialize `condo.db`, the
database file; this would require you to open a terminal<sup><a id="fnr.1" class="footref" href="#fn.1" role="doc-backlink">1</a></sup>.

The optional third argument, lower-case `:x` or upper-case `:X`,
initializes the program for secret (anonymous) **Unit#** entries rather than
real **Apt#** entries. This separation is needed for sensitive (publicly
observed) tallying. **NOTE!** Only when using the upper case `:X` argument
would actually new secret **Unit#** numbers be generated. Lower-case `:x`
argument initializes the program to use the preexisting secret
numbers. This is convenient when needing to switch between **Anon** (secret)
and **Open** (real) numbering tally. `Apt#` refers to the real apartment
number, `Unit#` refers to the secret apartment number.

An "anons" table is created in `condo.db` to relate the **real** apartment
numbers ("apt") with their **secret** counterparts ("unit"); example:

    sqlite> .mode col
    sqlite> .head on
    sqlite> .output "anons.txt"   
    sqlite> select * from anons order by apt;
    apt  unit
    ---  ----
    7    69  
    8    468 
    9    50  
    10   81  
    11   256

An empty table of `votes` would be created or **recreated if it already
exists**. The `votes` table has the following structure:

    CREATE TABLE votes (
          apt INTEGER PRIMARY KEY,
          vtime INTEGER,
          can1 INTEGER DEFAULT 0,
          ...
    );

vtime is the "unix time" of entering the apt vote; It is captured for
forensic purposes only.


# Footnotes

<sup><a id="fn.1" href="#fnr.1">1</a></sup> To open a Windows Terminal type `WIN+R` and then the `cmd` command.
