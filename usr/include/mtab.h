/*
 * @(#)mtab.h	1.2	7/13/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

struct mtab {
	char	m_path[32];		/* mounted on pathname */
	char	m_dname[32];		/* block device pathname */
};
