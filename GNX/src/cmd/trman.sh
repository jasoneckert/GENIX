#! /bin/sh

# trman.sh: version 1.1 of 3/1/83
# Mesa Unix System Command Shell Script
#
# @(#)trman.sh	1.1 (NSC) 3/1/83

sed -n -f /usr/man/man0/m1.sed $1\
|sed -n -f /usr/man/man0/m2.sed \
|sed -f /usr/man/man0/trref.sed
