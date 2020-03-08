/*
 * uid.c: version 1.1 of 3/28/83
 * Section 1 Command (Local)
 */
# ifdef SCCS
static char *sccsid = "@(#)uid.c	1.1 (NSC) 3/28/83";
# endif

/*
 * uid:    report effective and real user and group ids
 *
 * Note: this version of the program runs on version 7 systems;
 * PWB and V6 are not supported.  It also makes use of UCB routines
 * for accessing the password and group files.
 */

# include <stdio.h>
# include <pwd.h>
# include <grp.h>

extern struct passwd *getpwuid ();
extern struct group  *getgrgid ();

main (argc, argv)
    int  argc;
    char **argv;
{
    int uid, euid, gid, egid;
    register struct passwd *pentp;
    register struct group  *gentp;

    /* Gather the fundamental data: */
    uid  = getuid  ();
    euid = geteuid ();
    gid  = getgid  ();
    egid = getegid ();

    /* Put out the display header: */
    printf ("      |       real       |    effective\n"    );
    printf ("------+------------------+-----------------\n");

    /* Gather and print the user line: */
    printf (" user | %5u ", uid);
    pentp = getpwuid (uid);
    printf ("(%8s) | %5u ", pentp ? pentp->pw_name : "", euid);
    pentp = getpwuid (euid);
    printf ("(%8s)\n", pentp ? pentp->pw_name : "");

    /* Gather and print the group line: */
    printf ("group | %5u ", gid);
    gentp = getgrgid (gid);
    printf ("(%8s) | %5u ", gentp ? gentp->gr_name : "", egid);
    gentp = getgrgid (egid);
    printf ("(%8s)\n", gentp ? gentp->gr_name : "");

    exit (0);
}
