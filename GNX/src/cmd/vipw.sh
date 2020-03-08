#! /bin/sh

# vipw.sh: version 1.2 of 2/7/83
# Mesa Unix System Command Shell Script
#
# @(#)vipw.sh	1.2 (NSC) 2/7/83

cp /dev/null /etc/vipw.lock
chmod 0 /etc/vipw.lock
ln /etc/vipw.lock /etc/ptmp > /dev/null 2>& 1
case $? in
0)
	trap '' 1 2 3 15
	vi /etc/passwd
	rm /etc/ptmp
	;;
*)
	echo Temporary file busy, try again later.
	;;
esac
