.ig
	@(#)appen.C	1.4	11/29/83
	@(#)Copyright (C) 1983 by National Semiconductor Corp.
..
.ie t .he ''\f3Appendix C\f1''
.el .he ''\f2Appendix C\f1''
.bp
.(x
.ti 0
.b "Appendix C"
.)x
.sh 1 "Examples" 1
.pp
Here we present a few examples
of how to use the package.
They attempt to be representative,
though not comprehensive.
.sh 1 "Screen Updating"
.pp
The following examples are intended to demonstrate
the basic structure of a program
using the screen updating sections of the package.
Several of the programs require calculational sections
which are irrelevant of to the example,
and are therefore usually not included.
It is hoped that the data structure definitions
give enough of an idea to allow understanding
of what the relevant portions do.
The rest is left as an exercise to the reader,
and will not be on the final.
.sh 2 "Twinkle"
.pp
This is a moderately simple program which prints
pretty patterns on the screen
that might even hold your interest for 30 seconds or more.
It switches between patterns of asterisks,
putting them on one by one in random order,
and then taking them off in the same fashion.
It is more efficient to write this
using only the motion optimization,
as is demonstrated below.
.(l I
.vS
.so twinkle1.c
.vE
.)l
.sh 2 "Life"
.pp
This program plays the famous computer pattern game of life
(Scientific American, May, 1974).
The calculational routines create a linked list of structures
defining where each piece is.
Nothing here claims to be optimal,
merely demonstrative.
This program, however,
is a very good place to use the screen updating routines,
as it allows them to worry about what the last position looked like,
so you don't have to.
It also demonstrates some of the input routines.
.(l I
.vS
.so life.c
.vE
.)l
.sh 1 "Motion optimization"
.pp
The following example shows how motion optimization
is written on its own.
Programs which flit from one place to another without
regard for what is already there
usually do not need the overhead of both space and time
associated with screen updating.
They should instead use motion optimization.
.sh 2 "Twinkle"
.pp
The
.b twinkle
program
is a good candidate for simple motion optimization.
Here is how it could be written
(only the routines that have been changed are shown):
.(l
.vS
.so twinkle2.c
.vE
.)l
