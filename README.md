
# Table of Contents

1.  [General](#org1f8f3b0)
2.  [Software License and Legal Disclaimer](#orgaee1530)
3.  [Prerequisite](#orgf7f654f)
4.  [Program initialization](#org51fe305)



<a id="org1f8f3b0"></a>

# General

`vote` is a quick ballot tallying program that was originally written in
2014 for some troubled high rise community in the North East of the USA.
This community votes through weighted shares. Data entry is optimized for
the use of blind right-hand PC numeric-pad typing.  `vote` uses the SQLite
database library (sqlite3 version) and is written in C. To examine the
database file collected ballots use [SQLITEbrowser](http://sqlitebrowser.org/) or a similar tool.

`vote.exe`, the equivalent Microsoft Windows version, is built on Linux
using [MXE (M cross environment)](https://mxe.cc/). Here is the latest [Windows binary release](./Windows-binary-release.zip).


<a id="orgaee1530"></a>

# Software License and Legal Disclaimer

The software in this repository is in the public domain following the model
set by the [SQLite Project](http://www.sqlite.org/copyright.html). The software is offered “as-is”, without
warranty, and disclaiming liability for damages resulting from its use.


<a id="orgf7f654f"></a>

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
`share` value pairs. Ignore the `code` colum, it would hold random numbers
necessary for the anonymous tallying method.


<a id="org51fe305"></a>

# Program initialization

Run `vote -i <seats>:<candidates>[:x]` to initialize the program's
`condo.db` SQLite database file; this requires opening a terminal<sup><a id="fnr.1" class="footref" href="#fn.1" role="doc-backlink">1</a></sup>.

The optional third argument, `:x` or `:X`, initializes the program for the
use of secret (anonymous) apartment numbers rather than real numbers during
sensitive (publicly observed) tallying. **NOTE!** Only when using the upper
case `:X` argument would new secret apartment numbers be
generated. Lower-case `:x` argument initializes the program to use the
pre-existing secret numbers. This is convenient when needing to switch
between **secret** and **real** apartment numbers tally.

An "anon" table is created to relate the real apartment number with its
secret counterpart. 

A "bans" table of **real** apartment numbers can be provided to reject votes
from those apartments.

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
