# -*- mode: org; mode: auto-fill; fill-column: 75; -*- 
#+TITLE: WT200 Ballot Tallying Aid
#+AUTHOR: Enoch
#+EMAIL: ixew@hotmail.com
#+OPTIONS: email:t
#+STARTUP: indent

The following is a "quick & dirty" ballot tallying program for the benefit
of some troubled high rise community in the North East of the USA. Data
entry is optimized for the use of "blind" right-hand PC numeric-pad typing.
This program uses the SQLite data base and is written in C. To examine
collected votes use a program like as [[http://sqlitebrowser.org/][SQLITEbrowser]] on the database file.

This community votes through weighted shares.

* WT200 Election SQLite Loader
** General
   Use Windows console to run the programs.
** wt200.db
   Database including "shares" & "votes" tables.
** vote [-i <seats>:<candidates>]
   Initialize.
** sqlite3.exe wt200.db


