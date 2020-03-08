.ig
	@(#)contents.t	1.4	11/22/83
	@(#)Copyright (C) 1983 by National Semiconductor Corp.
..
.TL
G\s-2ENIX\s0\s4\uTM\d\s0
Programmer's Manual
.sp
Volume 2 \- Supplementary Documents
.sp
\f3November, 1983\f1
.FS
G\s-2ENIX\s0 and NS16000 are trademarks of National Semiconductor Corporation.
.FE
.PP
This volume contains documents that supplement the information in
Volume 1 of
.I
The
\s-2GENIX\s0
Programmer's Manual,
.R
for an NS16000\s4\uTM\d\s0 microprocessor version of the 
\s-2GENIX\s0 
system distributed by
National Semiconductor Corporation.
The \s-2GENIX\s0 operating system was derived from 
4.1bsd which was developed at the University of 
California at Berkeley.
.PP
Many of these papers describe features of the 
.UX
operating system.  These features are available under
the \s-2GENIX\s0 operating system with few (if any) implementation
differences.
.nr MP 0 1
.SH
General Works.
.IP \n+(MP
The 
\s-2GENIX\s0
Operating System \- Summary
.in +5n
Basic survival information for users of the system.
.in -5n
.IP \n+(MP
The 
.UX
Time-Sharing System
.in +5n
The original
.UX
paper, reprinted from CACM.
.in -5n
.IP \n+(MP
G\s-2ENIX\s0
Cross-Support Software For an NS16000-Based System
.in +5n
Introduction and brief description of cross-support software.
.in -5n
.IP \n+(MP
Dbmon Reference Manual
.in +5n
Dbmon allows remote debugging of a target system with ddt.
.in -5n
.SH
Getting Started.
.in -5n
.IP \n+(MP
G\s-2ENIX\s0 
for Beginners
.in +5n
An introduction to the basic use of the system.
.in -5n
.IP \n+(MP
An Introduction to the 
.UX
Shell
.in +5n
An introduction to the capabilities of the command interpreter, the Shell.
.in -5n
.IP \n+(MP
An Introduction to the C Shell (Revised)
.in +5n
Introducing a popular command interpreter and many of the commonly used
commands, assuming little prior knowledge of
.UX.
.in -5n
.IP \n+(MP
An Introduction to Display Editing with Vi (Revised)
.in +5n
The document to read to learn to use the \f2vi\f1 screen editor.
.in -5n
.IP \n+(MP
Ex Reference Manual (Version 3.1 \- Oct. 1980)
.in +5n
The final reference for the \f2ex\f1 editor, which underlies both \f2edit\f1
and \f2vi\f1.
.in -5n
.IP \n+(MP
Edit: A Tutorial
.in +5n
For those who prefer line oriented editing, an introduction assuming
no previous knowledge of
.UX
or of text editing.
.in -5n
.IP \n+(MP
A Tutorial Introduction to the 
.UX
Text Editor
.in +5n
An easy way to get started with 
.I ed
editor.
.in -5n
.IP \n+(MP
Advanced Editing on 
.UX
.in +5n
The next step.
.in -5n
.IP \n+(MP
Mail Reference Manual
.in +5n
Complete details on the mail processing program.
.in -5n
.SH
Document Preparation Tools.
.IP \n+(MP
Typing Documents on the 
UNIX System
.in +5n
Describes the basic use of the formatting tools.  Also describes \*([o\-ms\*([c,
a standardized package of formatting requests that can be used to lay out most
documents (including those in this volume).
.in -5n
.IP \n+(MP
Writing Papers With \f2nroff\f1 Using \-me
.in +5n
A popular macro package for
.I nroff.
.in -5n
.IP \n+(MP
\-me Reference Manual
.in +5n
The final word on \-me.
.in -5n
.IP \n+(MP
NROFF/TROFF User's Manual
.in +5n
The basic formatting program.
.in -5n
.IP \n+(MP
A TROFF Tutorial
.in +5n
An introduction to
.I troff
for those who really want to know such things.
.in -5n
.IP \n+(MP
TBL \- A Program to Format Tables
.in +5n
A program to permit easy specification of tabular material for
typesetting.  Again, easy to learn and use.
.in -5n
.IP \n+(MP
Writing Tools \- The Style and Diction Programs
.in +5n
Description of programs which help you understand and improve your
writing style.
.in -5n
.IP \n+(MP
Some Applications of Inverted Indexes on the 
.UX
System
.in +5n
Describes, among other things, the program REFER which fills in bibliographic
citations from a data base automatically.
.in -5n
.IP \n+(MP
A System for Typesetting Mathematics
.in +5n
Describes EQN, an easy-to-learn language for doing high-quality mathematical
typesetting.
.in -5n
.SH
Language Tools.
.IP \n+(MP
The Programmers' Guide to the 
\s-2GENIX\s0 
C Language and Compiler  
.in +5n
A guide to C language features on the NS16000.
.in -5n
.IP \n+(MP
Lint, A C Program Checker
.in +5n
Checks C programs for syntax errors, type violations, portability
problems, and a variety of probable errors.
.in -5n
.IP \n+(MP
Pascal User's Manual (optional) 
.in +5n
Language features and user's guide for the NS16000 Pascal implementation.
.in -5n
.IP \n+(MP
G\s-2ENIX\s0 Assembler Reference Manual
.in +5n
For writing assembly language programs.
.in -5n
.IP \n+(MP
U\s-2NIX\s0
Programming
.in +5n
Describes the programming interface to the operating system and the
standard I/O library.
.in -5n
.IP \n+(MP
YACC: Yet Another Compiler-Compiler
.in +5n
Converts a BNF specification of a language and semantic actions written
in C into a compiler for the language.
.in -5n
.IP \n+(MP
Lex \- A Lexical Analyzer Generator
.in +5n
Creates a recognizer for a set of regular expressions; each regular expression
can be followed by arbitrary C code which will be executed when the regular
expression is found.
.in -5n
.IP \n+(MP
The M4 Macro Processor
.in +5n
A macro processor useful as a front end for C, Ratfor, Cobol, etc.
.in -5n
.IP \n+(MP
Awk \- A Pattern Scanning and Processing Language
.in +5n
Makes it easy to specify many data transformation and selection operations.
.in -5n
.IP \n+(MP
Screen Updating and Cursor Movement Optimization
.in +5n
An aide for writing screen-oriented, terminal independent programs.
.in -5n
.SH
Supporting Tools.
.IP \n+(MP
An Introduction to the Source Code Control System
.in +5n
A quick introduction to SCCS.  Tells how to set up SCCS and use basic
SCCS commands.
.in -5n
.IP \n+(MP
Source Code Control System User's Guide
.in +5n
Describes a collection of commands that controls retrieval of particular
versions of files, administers changes to them, controls updating privileges
to them, and records historical data about the changes.
.in -5n
.IP \n+(MP
Function and Use of an SCCS Interface Program
.in +5n
Describes the use of a Source Code Control System Interface Program to
allow more than one user to use SCCS commands upon the same set of files.
.in -5n
.IP \n+(MP
SED \- A Non-Interactive Text Editor
.in +5n
A variant of the text editor for processing large inputs and stream editing.
.in -5n
.IP \n+(MP
DC \- An Interactive Desk Calculator
.in +5n
Interactive desk calculator program that does arbitrary-precision integer
arithemetic.
.in -5n
.IP \n+(MP
BC \- An Arbitrary Precision Desk-Calculator Language
.in +5n
A front end for DC (above) that provides infix notation, control flow, and
built-in functions.
.in -5n
.IP \n+(MP
Make \- A Program for Maintaining Computer Programs
.in +5n
Indispensable tool for making sure that large programs are properly
compiled with minimal effort.
.in -5n
.SH
Administration, Maintenance and Implementation.
.IP \n+(MP
SYS16 Systems Manual
.in +5n
Procedures used to install the 
\s-2GENIX\s0 
operation system on a SYS16 Development System and the steps necessary to 
regenerate the software.
This document is bound separately for convenience.
.in -5n
.IP \n+(MP
FSCK \- The 
.UX
File System Check Program
.in +5n
How to check and fix the file systems.
.in -5n
.IP \n+(MP
U\s-2NIX\s0
Implementation
.in +5n
How the system actually works inside.
.in -5n
.IP \n+(MP
The 
.UX
I/O System
.in +5n
Provides guidance to writers of device driver routines, and is oriented
more toward describing the environment and nature of device drivers.
.in -5n
.IP \n+(MP
On the Security of 
.UX
.in +5n
Hints on how to break \s-2UNIX\s0, 
and how to avoid doing so.
.in -5n
.IP \n+(MP
Password Security: A Case History
.in +5n
How the bad guys used to be able to break the password algorithm, and why
they can't now, at least not so easily.
.in -5n
.IP \n+(MP
Virtual Memory Management in the \s-2GENIX\s0 Operating System
.in +5n
National Semiconductor's implementation of virtual memory management for 
\s-2GENIX\s0.
.in -5n
.IP \n+(MP
Uucp Implementation
.in +5n
A look at the 
\s-2GENIX\s0
uucp implementation.
This document is aimed at system administrators/installers, no users 
allowed.
