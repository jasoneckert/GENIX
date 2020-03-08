#! /bin/csh

# ccat.sh: version 1.1 of 9/30/82
# Unix System Command Source File
#
# @(#)ccat.sh	1.1 (NSC) 9/30/82

foreach file ($argv)
/usr/ucb/uncompact < $file
end
