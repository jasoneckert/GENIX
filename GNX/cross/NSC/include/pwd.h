/*
 * @(#)pwd.h	1.2	7/13/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

struct	passwd { /* see getpwent(3) */
	char	*pw_name;
	char	*pw_passwd;
	int	pw_uid;
	int	pw_gid;
	int	pw_quota;
	char	*pw_comment;
	char	*pw_gecos;
	char	*pw_dir;
	char	*pw_shell;
};
