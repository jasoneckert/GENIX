: #! /bin/sh

# explain.sh: version 1.1 of 1/12/83
# Unix System Command Source File
#
# @(#)explain.sh	1.1 (NSC) 1/12/83

trap 'rm $$; exit' 1 2 3 15
D=/usr/lib/explain.d
while echo "phrase?";read x
do
cat >$$ <<dn
/$x.*	/s/\(.*\)	\(.*\)/use "\2" for "\1"/p
dn
case $x in
[a-z]*)
sed -n -f $$ $D; rm $$;;
*) rm $$;;
esac
done
