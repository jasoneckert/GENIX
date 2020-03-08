#! /bin/sh

# mkl+s.sh: version 1.5 of 2/7/83
# Unix System Command Source File
#
# @(#)mkl+s.sh	1.5 (NSC) 2/7/83

mkdir lost+found
cd lost+found
echo creating slots...
for i in 1 2 3 4 5 6 7 8 9 0 a b c d e f
do
	tee ${i}1 ${i}2 ${i}3 ${i}4 ${i}5 ${i}6 ${i}7 ${i}8 < /dev/null
	tee ${i}9 ${i}a ${i}b ${i}c ${i}d ${i}e ${i}f ${i}0 < /dev/null
done
echo removing dummy files...
for i in 1 2 3 4 5 6 7 8 9 0 a b c d e f
do
	rm ${i}1 ${i}2 ${i}3 ${i}4 ${i}5 ${i}6 ${i}7 ${i}8
	rm ${i}9 ${i}a ${i}b ${i}c ${i}d ${i}e ${i}f ${i}0
done
cd ..
echo done
ls -ld `pwd`/lost+found
