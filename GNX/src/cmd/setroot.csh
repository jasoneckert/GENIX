#! /bin/csh -f

# setroot.csh: version 2.14 of 11/18/83
# Local Command Script
#
# @(#)setroot.csh	2.14 (NSC) 11/18/83

#
# force correct permissions and ownership for all files.
#
# this script must be run by root.
#
# the flag -v will cause setroot to report which areas it is working
# on.  omitting the -v flag will not silence error messages.
#
# the -S flag will cause setroot to perform actions which should only
# be performed while in stand alone mode (i.e., single user).
# thses actions include the fixing of /dev.
#
# all error messages begin with the string:  `setroot:  '.
# exits with status 0 if all sets were successful, 1 if errors detected
#



# Establish execution environment:
set path = ( /etc /bin /usr/bin /usr/ucb )



# Prevent death from missing files:
set nonomatch



#
# process parameters
unset seterror
unset vmode standalone
while ( $#argv != 0 )
	if ( $argv[1] == '-v' ) then
		set vmode
	else if ( $argv[1] == '-S' ) then
		set standalone
	else if ( $argv[1] == '-Sv' || $argv[1] == '-vS' ) then
		set vmode
		set standalone
	else
		echo 'usage:  setroot [-v] [-S]'
		exit 1
	endif
	shift
end



#
if ( $?vmode ) echo 'fixing special programs'
if ( ! -e /dev/null ) then
	echo 'setroot:  fatal error: /dev/null not found'
	exit 1
endif
chmod 0777 /bin/chmod >& /dev/null
if ( $status == 0 && -e /bin/chmod ) then
	foreach file ( /etc/chown /etc/chgrp /bin/ls /usr/bin/wc /usr/ucb/grep )
		if ( -e $file ) then
			chmod 0777 $file
		else
			echo "setroot:  fatal error: $file not found"
			exit 1
		endif
	end
else
	echo 'setroot:  fatal error: can not execute /bin/chmod'
	exit 1
endif



#
if ( $?vmode ) echo 'checking for special user names'
foreach user ( root daemon uucp gameown )
#foreach user ( root daemon bin uucp gameown )
	if ( ! {`grep -c "^${user}:" /etc/passwd`} ) then
		echo "setroot:  fatal error: $user not a user in /etc/passwd"
		exit 1
	endif
end



#
if ( $?vmode ) echo 'checking for special group names'
foreach group ( sys daemon uucp games )
	if ( ! {`grep -c "^${group}:" /etc/group`} ) then
		echo "setroot:  fatal error: $user not a group in /etc/group"
		exit 1
	endif
end



#
if ( $?vmode ) echo 'fixing /.'
cd /
chown root .
chgrp sys .
chmod 0755 .



#
if ( $?vmode ) echo 'fixing /*'
foreach dir ( bin dev etc lib stand sys usr )
	if ( -d  $dir ) then
		chown root $dir
		chgrp sys $dir
		chmod 0755 $dir
	else
		echo "setroot:  /$dir not a directory"
		set seterror
	endif
end

foreach dir ( lost+found tmp )
	if ( -d  $dir ) then
		chown root $dir
		chgrp sys $dir
		chmod 0777 $dir
	else
		echo "setroot:  /$dir not a directory"
		set seterror
	endif
end

foreach file ( .cshrc .login vmsymbols )
	if ( -e $file ) then
		chown root $file
		chgrp sys $file
		chmod 0644 $file
	else
		echo "setroot:  /$file not found"
		set seterror
	endif
end

if ( -e vmunix ) then
	chown root vmunix
	chgrp sys vmunix
	chmod 0755 vmunix
else
	echo 'setroot:  /vmunix not found'
	set seterror
endif



#
if ( -d /bin ) then
	if ( $?vmode ) echo 'fixing /bin'
	cd /bin
	if ( {`/bin/ls|wc -l`} ) then
		chown root *
#		chown bin *
		chgrp sys *
		chmod 0755 *
	else
		echo 'setroot:  /bin is empty'
		set seterror
	endif
	foreach file ( df login mail mkdir mv passwd rmail rmdir su ps )
		if ( -e $file ) then
			chown root $file
			chgrp sys $file
			chmod 4755 $file
		else
			echo "setroot:  /bin/$file not found"
			set seterror
		endif
	end
	foreach file ( sh csh )
		if ( -e $file ) then
			chmod 1755 $file
		else
			echo "setroot:  /bin/$file not found"
			set seterror
		endif
	end
else
	echo 'setroot:  unable to set /bin'
	set seterror
endif



#
if ( -d /sys ) then
	if ( $?vmode ) echo 'fixing /sys'
	cd /sys
	chown root h
	chgrp sys h
	chmod 0755 h
	foreach dir ( sys dev SYS16 )
		chown root $dir
		chgrp sys $dir
		chmod 0755 $dir
		chown root $dir/*
		chgrp sys $dir/*
		chmod 0640 $dir/*
	end
else
	echo 'setroot:  unable to set /sys'
	set seterror
endif



#
if ( -d /dev ) then
	if ( $?standalone ) then
		if ( $?vmode ) echo 'fixing /dev'
		cd /dev
		if ( {`/bin/ls|wc -l`} ) then
			chown root *
#			chown bin *
			chgrp sys *
			chmod 0600 *
		else
			echo 'setroot:  /dev is empty'
			set seterror
		endif
		foreach file ( mem kmem )
			if ( -e $file ) then
				chmod 0600 $file
			else
				echo "setroot:  /dev/$file not found"
				set seterror
			endif
		end
		foreach file ( console null tty ttyp* pty* *tc* lp )
			if ( -e $file ) then
				chmod 0666 $file
			else
				echo "setroot:  /dev/$file not found"
				set seterror
			endif
		end
	else
		if ( $?vmode ) echo '-S option omitted, /dev skipped'
	endif
else
	echo 'setroot:  unable to set /dev'
	set seterror
endif




#
if ( -d /etc ) then
	if ( $?vmode ) echo 'fixing /etc'
	cd /etc
	if ( {`/bin/ls|wc -l`} ) then
		chown root *
#		chown bin *
		chgrp sys *
		chmod 0755 *
	else
		echo 'setroot:  /etc is empty'
		set seterror
	endif
	foreach file (fstab gettytab group passwd termcap\
	    ttys ttytype utmp)
		if ( -e $file ) then
			chown root $file
			chgrp sys $file
			chmod 0644 $file
		else
			echo "setroot:  /etc/$file not found"
			set seterror
		endif
	end
	if ( -e motd ) then
		chown root motd
#		chown bin motd
		chgrp sys motd
		chmod 0644 motd
	else
#  this file may not exist
#		echo 'setroot:  /etc/motd not found'
#		set seterror
	endif
	if ( -e dmesg ) then
		chown root dmesg
#		chmod bin dmesg
		chgrp sys dmesg
		chmod 4755 dmesg
	else
		echo "setroot:  /etc/dmesg not found"
		set seterror
	endif
else
	echo 'setroot:  unable to set /etc'
	set seterror
endif



#
if ( -d /lib ) then
	cd /lib
	if ( $?vmode ) echo 'fixing /lib'
	if ( {`/bin/ls|wc -l`} ) then
		chown root *
#		chown bin *
		chgrp sys *
		chmod 0755 *
	else
		echo 'setroot:  /lib is empty'
		set seterror
	endif
	foreach file ( asm ccom cpp )
		if ( -e $file ) then
			chmod 1755 $file
		else
			echo "setroot:  /lib/$file not found"
			set seterror
		endif
	end
else
	echo 'setroot:  unable to set /lib'
	set seterror
endif



#
if ( -d /stand ) then
	if ( $?vmode ) echo 'fixing /stand'
	cd /stand
	if ( {`/bin/ls|wc -l`} ) then
		chown root *
#		chown bin *
		chgrp sys *
		chmod 0755 *
	else
		echo 'setroot:  /stand is empty'
		set seterror
	endif
else
	echo 'setroot:  unable to set /stand'
	set seterror
endif



#
if ( -d /usr ) then
	if ( $?vmode ) echo 'fixing /usr'
	cd /usr
	foreach dir (adm bin crash dict games include lib local man nsc skel\
	    src ucb)
		if ( -d $dir ) then
			chown root $dir
			chgrp sys $dir
			chmod 0755 $dir
		else
			echo "setroot:  /usr/$dir not a directory"
			set seterror
		endif
	end
	foreach dir (msgs preserve pub spool tmp)
		if ( -d $dir ) then
			chown root $dir
			chgrp sys $dir
			chmod 0777 $dir
		else
			echo "setroot:  /usr/$dir not a directory"
			set seterror
		endif
	end
else
	echo 'setroot:  unable to set /usr'
	set seterror
endif



#
if ( -d /usr/adm ) then
	if ( $?vmode ) echo 'fixing /usr/adm'
	cd /usr/adm
	if ( {`/bin/ls|wc -l`} ) then
		chown root *
		chgrp sys *
		chmod 0644 *
	else
		echo 'setroot:  /usr/adm is empty'
		set seterror
	endif
else
	echo 'setroot:  unable to set /usr/adm'
	set seterror
endif



#
if ( -d /usr/bin ) then
	if ( $?vmode ) echo 'fixing /usr/bin'
	cd /usr/bin
	if ( {`/bin/ls|wc -l`} ) then
		chown root *
#		chown bin *
		chgrp sys *
		chmod 0755 *
	else
		echo 'setroot:  /usr/bin is empty'
		set seterror
	endif
	if ( -e newgrp ) then
		chown root newgrp
		chgrp sys newgrp
		chmod 4755 newgrp
	else
		echo 'setroot:  /usr/bin/newgrp not found'
		set seterror
	endif
	if ( -e at ) then
		chown root at
		chgrp sys at
		chmod 4711 at
	else
		echo 'setroot:  /usr/bin/at not found'
		set seterror
	endif
	foreach file ( uucp uulog uuname uustat uux )
		if ( -e $file ) then
			chown uucp $file
			chgrp uucp $file
			chmod 4111 $file
		else
			echo "setroot:  /usr/bin/$file not found"
			set seterror
		endif
	end
else
	echo 'setroot:  unable to set /usr/bin'
	set seterror
endif



#
if ( -d /usr/crash ) then
	if ( $?vmode ) echo 'fixing /usr/crash'
	cd /usr/crash
	if ( -e bounds ) then
		chown root bounds
		chgrp sys bounds
		chmod 0644 bounds
	else
		echo 'setroot:  /usr/crash/bounds not found'
		set seterror
	endif
else
	echo 'setroot:  unable to set /usr/crash'
	set seterror
endif



#
if ( -d /usr/dict ) then
	if ( $?vmode ) echo 'fixing /usr/dict'
	cd /usr/dict
	foreach file ( hlista hlistb hstop spellhist words )
		if ( -e $file ) then
			chown root $file
#			chown bin $file
			chgrp sys $file
			chmod 0644 $file
		else
			echo "setroot:  /usr/dict/$file not found"
			set seterror
		endif
	end
else
	echo 'setroot:  unable to set /usr/dict'
	set seterror
endif



#
if ( -d /usr/games ) then
	if ( $?vmode ) echo 'fixing /usr/games'
	cd /usr/games
	if ( {`/bin/ls|wc -l`} ) then
		chown gameown *
		chgrp games *
		chmod 0711 *
	else
		echo 'setroot:  /usr/games is empty'
		set seterror
	endif
	if ( -d lib ) then
		chown root lib
		chgrp sys lib
		chmod 0755 lib
	else
		echo 'setroot:  /usr/games/lib not a directory'
		set seterror
	endif
	if ( -e rogue ) then
		chown gameown rogue
		chgrp games rogue
		chmod 711 rogue
	else
		echo 'setroot:  /usr/games/rogue not found'
		set seterror
	endif
	if ( -e adventure ) then
		chown gameown adventure
		chgrp games adventure
		chmod 0755 adventure
	else
		echo 'setroot:  /usr/games/adventure not found'
		set seterror
	endif
else
	echo 'setroot:  unable to set /usr/games'
	set seterror
endif



#
if ( -d /usr/games/lib ) then
	if ( $?vmode ) echo 'fixing /usr/games/lib'
	cd /usr/games/lib
	if ( {`/bin/ls|wc -l`} ) then
		chown gameown *
		chgrp games *
	else
		echo 'setroot:  /usr/games/lib is empty'
		set seterror
	endif
	if ( -d quiz.k ) then
		chmod 0755  quiz.k
	else
		echo 'setroot:  /usr/games/lib/quiz.k not a directory'
		set seterror
	endif
	if ( -e rogue_roll ) then
		chmod 0666  rogue_roll
	else
		echo 'setroot:  /usr/games/lib/rogue_roll not found'
		set seterror
	endif
	if ( -e fortunes.dat ) then
		chmod 0644  fortunes.dat
	else
		echo 'setroot:  /usr/games/lib/fortunes.dat not found'
		set seterror
	endif
else
	echo 'setroot:  unable to set /usr/games/lib'
	set seterror
endif



#
if ( -d /usr/games/lib/quiz.k ) then
	if ( $?vmode ) echo 'fixing /usr/games/lib/quiz.k'
	cd /usr/games/lib/quiz.k
	if ( {`/bin/ls|wc -l`} ) then
		chown gameown *
		chgrp games *
		chmod 0644 *
	else
		echo 'setroot:  /usr/games/lib/quiz.k is empty'
		set seterror
	endif
else
	echo 'setroot:  unable to set /usr/games/lib/quiz.k'
	set seterror
endif



#
if ( -d /usr/include ) then
	if ( $?vmode ) echo 'fixing /usr/include'
	cd /usr/include
	if ( {`/bin/ls|wc -l`} ) then
		chown root *
		chgrp sys *
		chmod 0444 *
	else
		echo 'setroot:  /usr/include is empty'
		set seterror
	endif
	foreach dir ( local sys )
		if ( -d $dir ) then
			chown root $dir
			chgrp sys $dir
			chmod 0755 $dir
		else
			echo "setroot:  /usr/include/$dir not a directory"
			set seterror
		endif
	end
else
	echo 'setroot:  unable to set /usr/include'
	set seterror
endif



#
if ( -d /usr/include/local ) then
	if ( $?vmode ) echo 'fixing /usr/include/local'
	cd /usr/include/local
	if ( {`/bin/ls|wc -l`} ) then
		chown root *
#		chown bin *
		chgrp sys *
		chmod 0444 *
	else
		echo 'setroot:  /usr/include/local is empty'
		set seterror
	endif
else
	echo 'setroot:  unable to set /usr/include/local'
	set seterror
endif



#
if ( -d /usr/include/sys ) then
	if ( $?vmode ) echo 'fixing /usr/include/sys'
	cd /usr/include/sys
	if ( {`/bin/ls|wc -l`} ) then
		chown root *
#		chown bin *
		chgrp sys *
		chmod 0444 *
	else
		echo 'setroot:  /usr/include/sys is empty'
		set seterror
	endif
else
	echo 'setroot:  unable to set /usr/include/sys'
	set seterror
endif



#
if ( -d /usr/lib ) then
	if ( $?vmode ) echo 'fixing /usr/lib'
	cd /usr/lib
	if ( {`/bin/ls|wc -l`} ) then
		chown root *
#		chown bin *
		chgrp sys *
		chmod 0644 *
	else
		echo 'setroot:  /usr/lib is empty'
		set seterror
	endif
	foreach file (calendar deroff dict.d diff3 diffh dprog\
	    explain.d getNAME lpd lpf macros\
	    makekey makewhatis pcom pmsg slpd slpf spell)
		if ( -e $file ) then
			chmod 0755 $file
		else
			echo "setroot:  /usr/lib/$file not found"
			set seterror
		endif
	end
	foreach file ( atrun ex3.6preserve ex3.6recover )
		if ( -e $file ) then
			chown root $file
			chgrp sys $file
			chmod 4755 $file
		else
			echo "setroot:  /usr/lib/$file not found"
			set seterror
		endif
	end
	foreach dir ( font lex lint macros me tabset term tmac uucp )
		if ( -d $dir ) then
			chown root $dir
			chgrp sys $dir
			chmod 0755 $dir
		else
			echo "setroot:  /usr/lib/$dir not a directory"
			set seterror
		endif
	end
else
	echo 'setroot:  unable to set /usr/lib'
	set seterror
endif


foreach dir ( font lex macros me tabset term tmac )
	if ( -d /usr/lib/$dir ) then
		if ( $?vmode ) echo "fixing /usr/lib/$dir"
		cd /usr/lib/$dir
		if ( {`/bin/ls|wc -l`} ) then
			chown root *
#			chown bin *
			chgrp sys *
			chmod 0644 *
		else
			echo "setroot:  /usr/lib/$dir is empty"
			set seterror
		endif
	else
		echo "setroot:  unable to set /usr/lib/$dir"
		set seterror
	endif
end



#
if ( -d /usr/lib/lint ) then
	if ( $?vmode ) echo 'fixing /usr/lib/lint'
	cd /usr/lib/lint
	if ( {`/bin/ls|wc -l`} ) then
		chown root *
#		chown bin *
		chgrp sys *
		chmod 0644 *
	else
		echo 'setroot:  /usr/lib/lint is empty'
		set seterror
	endif
	foreach file ( lint1 lint2 )
		if ( -e $file ) then
			chmod 0755 $file
		else
			echo "setroot:  /usr/lib/lint/$file not found"
			set seterror
		endif
	end
else
	echo 'setroot:  unable to set /usr/lib/lint'
	set seterror
endif



#
if ( -d /usr/lib/uucp ) then
	if ( $?vmode ) echo 'fixing /usr/lib/uucp'
	cd /usr/lib/uucp
	if ( {`/bin/ls|wc -l`} ) then
		chown uucp *
		chgrp uucp *
	else
		echo 'setroot:  /usr/lib/uucp is empty'
		set seterror
	endif
	foreach file ( uuclean uucico uuxqt )
		if ( -e $file ) then
			chmod 4111 $file
		else
			echo "setroot:  /usr/lib/uucp/$file not found"
			set seterror
		endif
	end
	if ( -e uusub ) then
		chmod 0100 uusub
	else
		echo 'setroot:  /usr/lib/uucp/uusub not found'
		set seterror
	endif
	foreach file ( README L-devices SEQF )
		if ( -e $file ) then
			chmod 0644 $file
		else
			echo "setroot:  /usr/lib/uucp/$file not found"
			set seterror
		endif
	end
	foreach file ( L.sys L-dialcodes USERFILE )
		if ( -e $file ) then
			chmod 0600 $file
		else
			echo "setroot:  /usr/lib/uucp/$file not found"
			set seterror
		endif
	end
else
	echo 'setroot:  unable to set /usr/lib/uucp'
	set seterror
endif



#
if ( -d /usr/man ) then
	if ( $?vmode ) echo 'fixing /usr/man'
	cd /usr/man
	if ( {`/bin/ls|wc -l`} ) then
		chown root *
#		chown bin *
		chgrp sys *
	else
		echo 'setroot:  /usr/man is empty'
		set seterror
	endif
	foreach mandir ( 1 2 3 4 5 6 7 8 l )
		set dir = man$mandir
		if ( -d $dir ) then
			chown root $dir
			chgrp sys $dir
			chmod 0755 $dir
#		else
#  the man directories are not used at the present time
#			echo "setroot:  /usr/man/$dir not a directory"
#			set seterror
#  the cat directories contain pages with nroff filled files
		endif
		set dir = cat$mandir
		if ( -d $dir ) then
			chown root $dir
			chgrp sys $dir
			chmod 0777 $dir
		else
			echo "setroot:  /usr/man/$dir not a directory"
			set seterror
		endif
	end
else
	echo 'setroot:  unable to set /usr/man'
	set seterror
endif


foreach mandir ( 1 2 3 4 5 6 7 8 l )
	set dir = cat$mandir
	if ( -d /usr/man/$dir ) then
		if ( $?vmode ) echo "fixing /usr/man/$dir"
		cd /usr/man/$dir
		if ( {`/bin/ls|wc -l`} ) then
			chown root *
#			chown bin *
			chgrp sys *
			chmod 0666 *
		else
			echo "setroot:  /usr/cat/$dir is empty"
			set seterror
		endif
	else
		echo "setroot:  unable to set /usr/man/$dir"
		set seterror
	endif
	set dir = man$mandir
	if ( -d /usr/man/$dir ) then
		if ( $?vmode ) echo "fixing /usr/man/$dir"
		cd /usr/man/$dir
		if ( {`/bin/ls|wc -l`} ) then
			chown root *
#			chown bin *
			chgrp sys *
			chmod 0644 *
#		else
#  man files may not exist, no error if they do not
#			echo "setroot:  /usr/man/$dir is empty"
#			set seterror
		endif
	else
		echo "setroot:  unable to set /usr/man/$dir"
		set seterror
	endif
end



#
if ( -d /usr/nsc ) then
	if ( $?vmode ) echo 'fixing /usr/nsc'
	cd /usr/nsc
	if ( {`/bin/ls|wc -l`} ) then
		chown root *
#		chown bin *
		chgrp sys *
		chmod 0755 *
	else
		echo 'setroot:  /usr/nsc is empty'
		set seterror
	endif
	foreach file ( su1 uvers )
		if ( -e $file ) then
			chown root $file
			chgrp sys $file
			chmod 4755 $file
		else
			echo "setroot:  /usr/nsc/$file not found"
			set seterror
		endif
	end
	if ( -d /usr/nsc/lib ) then
		chown root lib
		chgrp sys lib
		chmod 0755 lib
	else
		echo 'setroot:  /usr/nsc/lib not a directory'
		set seterror
	endif
else
	echo 'setroot:  unable to set /usr/nsc'
	set seterror
endif



#
if ( -d /usr/nsc/lib ) then
	if ( $?vmode ) echo 'fixing /usr/nsc/lib'
	cd /usr/nsc/lib
	if ( {`/bin/ls|wc -l`} ) then
		chown root *
#		chown bin *
		chgrp sys *
		chmod 0755 *
	else
		echo 'setroot:  /usr/nsc/lib is empty'
		set seterror
	endif
	if ( -d help ) then
		chown root help
		chgrp sys help
		chmod 0755 help
	else
		echo 'setroot:  /usr/nsc/lib/help not a directory'
		set seterror
	endif
else
	echo 'setroot:  unable to set /usr/nsc/lib'
	set seterror
endif



#
if ( -d /usr/nsc/lib/help ) then
	if ( $?vmode ) echo 'fixing /usr/nsc/lib/help'
	cd /usr/nsc/lib/help
	if ( {`/bin/ls|wc -l`} ) then
		chown root *
#		chown bin *
		chgrp sys *
		chmod 0644 *
	else
		echo 'setroot:  /usr/nsc/lib/help is empty'
		set seterror
	endif
else
	echo 'setroot:  unable to set /usr/nsc/lib/help'
	set seterror
endif



if ( -d /usr/local ) then
	if ( $?vmmode ) echo 'fixing /usr/local'
	cd /usr/local
	if ( -e README ) then
		chown root README
#		chown bin README
		chgrp sys README
		chmod 0644 README
	else
		echo 'setroot:  /usr/local/README not found'
		set seterror
	endif
else
	echo 'setroot:  unable to set /usr/local'
	set seterror
endif



#
if ( -d /usr/pub ) then
	if ( $?vmode ) echo 'fixing /usr/pub'
	cd /usr/pub
	if ( {`/bin/ls|wc -l`} ) then
		chown root *
#		chown bin *
		chgrp sys *
		chmod 0644 *
	else
		echo 'setroot:  /usr/pub is empty'
		set seterror
	endif
else
	echo 'setroot:  unable to set /usr/pub'
	set seterror
endif



#
if ( -d /usr/skel ) then
	if ( $?vmode ) echo 'fixing /usr/skel'
	cd /usr/skel
	if ( {`/bin/ls|wc -l`} ) then
		chown root *
#		chown bin *
		chgrp sys *
		chmod 0644 *
	else
		echo 'setroot:  /usr/skel is empty'
		set seterror
	endif
else
	echo 'setroot:  unable to set /usr/skel'
	set seterror
endif



#
if ( -d /usr/spool ) then
	if ( $?vmode ) echo 'fixing /usr/spool'
	cd /usr/spool
	if ( -d at ) then
		chown root at
		chgrp sys at
		chmod 0755 at
	else
		echo 'setroot:  /usr/spool/at not a directory'
		set seterror
	endif
	foreach file ( lpd slp )
		if ( -d $file ) then
			chown daemon $file
			chgrp daemon $file
			chmod 0777 $file
		else
			echo "setroot:  /usr/spool/$file not a directory"
			set seterror
		endif
	end
	if ( -d uucp ) then
		chown uucp uucp
		chgrp uucp uucp
		chmod 0755 uucp
	else
		echo 'setroot:  /usr/spool/uucp not a directory'
		set seterror
	endif
	if ( -d uucppublic ) then
		chown uucp uucppublic
		chgrp uucp uucppublic
		chmod 0777 uucppublic
	else
		echo 'setroot:  /usr/spool/uucppublic not a directory'
		set seterror
	endif
	foreach dir ( mail secretmail )
		if ( -d $dir ) then
			chown root $dir
			chgrp sys $dir
			chmod 0777 $dir
		else
			echo "setroot:  /usr/spool/$dir not a directory"
			set seterror
		endif
	end
else
	echo 'setroot:  unable to set /usr/spool'
	set seterror
endif



#
if ( -d /usr/spool/at ) then
	if ( $?vmode ) echo 'fixing /usr/spool/at'
	cd /usr/spool/at
	if ( -d past ) then
		chown root past
		chgrp sys past
		chmod 0755 past
	else
		echo 'setroot:  /usr/spool/at/past not a directory'
		set seterror
	endif
else
	echo 'setroot:  unable to set /usr/spool/at'
	set seterror
endif



#
if ( -d /usr/spool/secretmail ) then
	if ( $?vmode ) echo 'fixing /usr/spool/secretmail'
	cd /usr/spool/secretmail
	if ( -e notice ) then
		chown root notice
#		chown bin notice
		chgrp sys notice
		chmod 0444 notice
	else
		echo 'setroot:  /usr/spool/secretmail/notice not found'
		set seterror
	endif
else
	echo 'setroot:  unable to set /usr/spool/secretmail'
	set seterror
endif



#
if ( -d /usr/spool/uucp ) then
	if ( $?vmode ) echo 'fixing /usr/spool/uucp'
	cd /usr/spool/uucp
	if ( -e .XQTDIR ) then
		chown uucp .XQTDIR
		chgrp uucp  .XQTDIR
		chmod 0777  .XQTDIR
	else
		echo 'setroot:  /usr/spool/uucp/.XQTDIR not found'
		set seterror
	endif
else
	echo 'setroot:  unable to set /usr/spool/uucp'
	set seterror
endif



#
if ( -d /usr/spool/uucppublic ) then
	if ( $?vmode ) echo 'fixing /usr/spool/uucppublic'
	cd /usr/spool/uucppublic
	if ( -e .hushlogin ) then
		chown uucp .hushlogin
		chgrp uucp .hushlogin
		chmod 0444 .hushlogin
	else
		echo 'setroot:  /usr/spool/uucppublic/.hushlogin not found'
		set seterror
	endif
else
	echo 'setroot:  unable to set /usr/spool/uucppublic'
	set seterror
endif



#
if ( -d /usr/src ) then
	if ( $?vmode ) echo 'fixing /usr/src'
	cd /usr/src
	if ( {`/bin/ls|wc -l`} ) then
		chown root *
#		chown bin *
		chgrp sys *
#	else
#  sources dont normally exist, not an error if they dont
#		echo 'setroot:  /usr/src is empty'
#		set seterror
#
	endif
	foreach dir ( cmd db16k games lib libc libp local sys )
		if ( -d $dir ) then
			chown root $dir
			chgrp sys $dir
			chmod 0755 $dir
#		else
#  sources dont normally exist, not an error if they dont
#			echo "setroot:  /usr/src/$dir not found"
#			set seterror
#
		endif
	end
#else
#  sources dont normally exist, not an error if they dont
#	echo 'setroot:  unable to set /usr/src'
#	set seterror
#
endif



#
if ( -d /usr/src/db16k ) then
	if ( $?vmode ) echo 'fixing /usr/src/db16k'
	cd /usr/src/db16k
	if ( {`/bin/ls|wc -l`} ) then
		chown root *
#		chown bin *
		chgrp sys *
		chmod 0755 *
#	else
#  sources dont normally exist, not an error if they dont
#		echo 'setroot:  /usr/src/db16k is empty'
#		set seterror
#
	endif
	foreach file ( README makefile pio.s pio.p rom.s sio.s vers.s version )
		if ( -e $file ) then
			chmod 0644 $file
#		else
#  sources dont normally exist, not an error if they dont
#			echo "setroot:  /usr/src/db16k/$file not found"
#			set seterror
#
		endif
	end
#else
#  sources dont normally exist, not an error if they dont
#	echo 'setroot:  unable to set /usr/src/db16k'
#	set seterror
#
endif



#
if ( -d /usr/tmp ) then
	if ( $?vmode ) echo 'fixing /usr/tmp'
	cd /usr/tmp
	if ( -e .hushlogin ) then
		chown root .hushlogin
#		chown bin .hushlogin
		chgrp sys .hushlogin
		chmod 0644 .hushlogin
	else
		echo 'setroot:  /usr/tmp/.hushlogin not found'
		set seterror
	endif
else
	echo 'setroot:  unable to set /usr/tmp'
	set seterror
endif



#
if ( -d /usr/ucb ) then
	if ( $?vmode ) echo 'fixing /usr/ucb'
	cd /usr/ucb
	if ( {`/bin/ls|wc -l`} ) then
		chown root *
#		chown bin *
		chgrp sys *
		chmod 0755 *
	else
		echo 'setroot:  /usr/ucb is empty'
		set seterror
	endif
	foreach file ( chfn chsh vmstat w )
		if ( -e $file ) then
			chown root $file
			chgrp sys $file
			chmod 4755 $file
		else
			echo "setroot:  /usr/ucb/$file not found"
			set seterror
		endif
	end
	foreach file ( vi ls )
		if ( -e $file ) then
			chown root $file
#			chown bin $file
			chgrp sys $file
			chmod 1755 $file
		else
			echo "setroot:  /usr/ucb/$file not found"
			set seterror
		endif
	end
else
	echo 'setroot:  unable to set /usr/ucb'
	set seterror
endif




#
if ( $?seterror ) then
	if ( $?vmode ) echo 'error detected while setting'
	if ( $?vmode ) echo "Done"
	exit 1
else
	sync
	if ( $?vmode ) echo 'all sets successful'
	if ( $?vmode ) echo "Done"
	exit 0
endif
