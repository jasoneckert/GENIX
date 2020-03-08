/*
 * stat.c: version 1.3 of 3/28/83
 * Section 1 Command (Local)
 */
# ifdef SCCS
static char *sccsid = "@(#)stat.c	1.3 (NSC) 3/28/83";
# endif

/*
 * Note: this version of the program runs on UCB release 4 systems;
 * PWB and V6 are not supported.  It also makes use of routines
 * for accessing the password and group files.
 */

# include <stdio.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <pwd.h>
# include <grp.h>

extern struct passwd *getpwuid ();
extern struct group  *getgrgid ();
extern char *ctime   (),
            *putperm ();

main (argc, argv)
    int  argc;
    char **argv;
{
    int uid, gid;
    register char *p;
    register struct passwd *pentp;
    register struct group  *gentp;
    struct stat sb;

    argc--, argv++;

    /* If no arguments are supplied, assume ".": */
    if (argc == 0)
    {
        argc++;
        *--argv = ".";
    }

    /* Report status for each argument in turn: */
    while (argc--)
    {
        p = *argv++;

        /* Gather the fundamental data: */
        if (stat (p, &sb) < 0)
        {
            fprintf (stderr, "%s: Cannot stat\n", p);
            fflush (stderr);
            continue;
        }
           
        /* Put out the display: */
        printf ("%s:\n\n", p);

        printf ( "major:  %3d   minor:  %3d\n"
               , major(sb.st_dev)
               , minor(sb.st_dev)
               );
        printf ( "rmajor: %3d   rminor: %3d\n\n"
               , major(sb.st_rdev)
               , minor(sb.st_rdev)
               );
        printf ("inode:  %d\n", sb.st_ino);
        printf ("links:  %d\n",sb.st_nlink);
        printf ("size:   %d\n\n", sb.st_size);
           
        /* Gather and print the user line: */
        printf ("user:   %u\t", sb.st_uid);
        pentp = getpwuid (sb.st_uid);
        printf ("(%s)\n", (pentp ? pentp->pw_name : "??"));
           
        /* Gather and print the group line: */
        printf ("group:  %u\t", sb.st_gid);
        gentp = getgrgid (sb.st_gid);
        printf ("(%s)\n\n", (gentp ? gentp->gr_name : "??"));

        /* Put out the mode bits: */
        printf ("mode: %o ", sb.st_mode);
        switch (sb.st_mode & S_IFMT)
        {
        case S_IFDIR:  p = "directory";           break;
        case S_IFCHR:  p = "character special";   break;
        case S_IFBLK:  p = "block special";       break;
        case S_IFREG:  p = "plain";               break;
#       ifdef notdef
        case S_IFMPC:  p = "character multiplex"; break;
        case S_IFMPB:  p = "block multiplex";     break;
#       endif notdef
        }
        printf ( "(%s%s%s%s)\n\n"
               , ((sb.st_mode & S_ISUID) ? "setuid " : "")
               , ((sb.st_mode & S_ISGID) ? "setgid " : "")
               , ((sb.st_mode & S_ISVTX) ? "sticky " : "")
               , p
               );

        printf ("permissions:\n");
        printf ("  owner: %s\n",   putperm ((sb.st_mode >> 6) & 07));
        printf ("  group: %s\n",   putperm ((sb.st_mode >> 3) & 07));
        printf ("  other: %s\n\n", putperm ((sb.st_mode >> 0) & 07));

        printf ("atime:  %s", ctime (&sb.st_atime));
        printf ("mtime:  %s", ctime (&sb.st_mtime));
        printf ("ctime:  %s", ctime (&sb.st_ctime));

        /* If the loop will iterate again, spew out some white space: */
        if (argc > 0)
            printf ("\n\n\n");
    }

    exit (0);
}

/*
 * Construct a string of the form [-r][-w][-x] from the access bits in mode.
 * The string is static, and must be copied if it is to be saved.
 */
char *
putperm (mode)
    register unsigned mode;
{
    static char rwx [3];

    rwx[0] = (mode & 04) ? 'r' : '-';
    rwx[1] = (mode & 02) ? 'w' : '-';
    rwx[2] = (mode & 01) ? 'x' : '-';

    return (rwx);
}
