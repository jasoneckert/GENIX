set path=(. ~/bin /usr/local/bin /usr/nsc /usr/ucb /bin /usr/bin)
if ( $?prompt ) then
	set history = 100
	set mail = (60 /usr/spool/mail/$USER)
	set notify
	set prompt  = '=>> '
	alias l ls -lis
	alias L ls -lisa
	alias create : \>
endif
