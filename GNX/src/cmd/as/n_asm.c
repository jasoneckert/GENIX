
#ifndef NOSCCS
static char *sccsid = "@(#)n_asm.c	3.31    9/20/83";
static char *cpyrid = "@(#)Copyright (C) 1983 by National Semiconductor Corp.";
#endif

#include <stdio.h>
#include "n_asm.h"
#include <sys/types.h>
#include <sys/times.h>


char *args[20] = {
	asmname,
        "-o"
};

char *sourcename=NULL;
char *objname = outname;
char *asname = asmpath;
char *lsname = listpath;
char *as1 = tmpas1;
char *as3 = tmpas3;
char *ls1 = tmpls1;
char *ls2 = tmpls2;
char *ls3 = tmpls3;
char *asfilename = asmname;
char *lsfilename = listname;

int dolist=0;
int sourcefound=0;
int vmflag_found=0;
int fd,ls,lstat;
struct tms buffer;

extern char *mktemp();

main(argc,argv)
register int argc;
register char **argv;
{
	register int i;
	int status,w,pid;
        if (argc==1) { printf("Usage: %s [-i] [-l] [-g] [-y] [-u] [-v] [-o objfile] [sourcefile]\n");
                     exit(1);
                    }
        times(&buffer);
	argv++;
	i = 3;
	while(--argc) {
		if (**argv=='-') {
			switch((*argv)[1]) {
			    default:
				break;
			    case 'o':
				if (argc < 2) {
					fprintf(stderr,"no object specified with -o option\n");
					argv++;
					continue;
				}
				objname = *++argv;
				/*printf("found -o %s\n",objname);*/
				argv++; argc--;
				continue;
			    case 'l':
				dolist++;
				break;
                            case 'v':
                                vmflag_found = 1;
                                break;
			}
			args[i] = *argv;
			i++;
		} else {
                        if (sourcefound!=0)  {
                               printf("ambigous source name \n");
                               exit(1);
                        }
			sourcename = *argv;
                        sourcefound = 1;
		}
		argv++;
	}
        if (sourcefound==0)  {
              printf("no source file specified %c",'\n');
              exit(1);
        }
	args[i] = sourcename;
        args[++i] = mktemp(as1);
        if (vmflag_found == 0) {
                args[++i] = mktemp(as3);
        };
	args[++i] = mktemp(ls1);
	args[++i] = mktemp(ls3);
	args[++i] = mktemp(ls2);
        args[++i] = NULL;
	args[2] = objname;
        /*
	i=0;
	while(args[i]) {
		printf("%s ",args[i]);
		i++;
	}
	printf("\n");
        */
	pid =vfork();
	if (pid  == 0) {
		execv(asname,args);
		fprintf(stderr,"exec of %s failed\n",asname);
		_exit(1);
	}
	while((w = wait(&status)) != pid && w != -1)
		;
        times(&buffer);
        fd = open(as1,0);
        ls = read(fd,&lstat,4);
        if (((status == 0) && (dolist ==0)) && (lstat == 0)) done(status);

        args[0] = lsfilename;
        if (dolist) {
             args[1] = "-l";
             i = 2;
        } else i = 1;
  	args[i] = sourcename;
  	args[++i] = objname;
	args[++i] = ls3;
	args[++i] = ls2;
	args[++i] = ls1;
	args[++i] = NULL;
        /*
              i = 0;
	while(args[i]) {
		printf("%s ",args[i]);
		i++;
	}
        */
	if ((pid = vfork()) == 0) {
		execv(lsname,args);
		fprintf(stderr,"exec of %s failed\n",lsname);
		_exit(1);
	}
		while((w = wait(&status)) != pid && w != -1)
			;
	
        times(&buffer);
	done(status);
}

done(status)
int status;
{
          unlink(as1);
        if ( vmflag_found == 0 ){
          unlink(as3);
        };
	unlink(ls1);
	unlink(ls2);
	unlink(ls3);
	exit(status);
}
