#! /bin/sh
# @(#)rc.sh	1.3	10/5/83
# @(#)Copyright (C) 1983 by National Semiconductor Corp.
PATH=/bin:/etc:/usr/bin; HOME=/; export PATH; export HOME
hostname anon
if [ $1x = autobootx ]
then
	echo Automatic reboot in progress... >/dev/console
	date >/dev/console
	/etc/fsck -p >/dev/console
	status=$?
	case $status in
	0)
		date >/dev/console
		;;
	4)
		/etc/reboot -n
		;;
	8)
		echo 'Automatic reboot failed... help!' > /dev/console
		exit 1
		;;
	12)
		echo 'Reboot interrupted' > /dev/console
		exit 1
		;;
	*)
		echo "Unknown error:$status in reboot" > /dev/console
		exit 1
		;;
	esac
else
	date >/dev/console
fi
cp /dev/null /etc/mtab
/etc/mount -a > /dev/console
/usr/lib/ex3.6preserve -a
rm -f /tmp/* /usr/tmp/* /usr/spool/lpd/lock /etc/nologin
touch /usr/tmp/.hushlogin
/etc/update
/etc/cron
: /etc/savecore /usr/crash 2>&1 > /dev/console
: in case you want to do it  /etc/accton /usr/adm/acct
exit 0
