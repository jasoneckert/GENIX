: #! /bin/sh

# diction.sh: version 1.1 of 1/12/83
# Unix System Command Source File
#
# @(#)diction.sh	1.1 (NSC) 1/12/83

B=/usr/lib
echo $*
rest=
flag=
nflag=
mflag=-mm
lflag=
file=
for i
do case $i in
 -f) flag=-f;shift; file=$1; shift; continue;;
-n) nflag=-n;shift; continue;;
 -mm) mflag=$1; shift; continue;;
-ms) mflag=$1;shift;continue;;
-ml) lflag=$1;shift;continue;;
*) rest=$*; break;;
esac
done
 $B/deroff $mflag $lflag $rest^$B/dprog -d $nflag $flag $file
