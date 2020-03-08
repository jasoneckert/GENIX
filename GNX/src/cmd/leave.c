
/*
 * leave.c: version 1.4 of 11/1/82
 * Unix System Command Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)leave.c	1.4 (NSC) 11/1/82";
# endif

#include <stdio.h>
/*
 * leave [hhmm]
 *
 * Reminds you when you have to leave.
 * Leave prompts for input and goes away if you hit return.
 * It nags you like a mother hen.
 */
char origlogin[20], thislogin[20];
char *getlogin();
char *whenleave;
char *ctime();
char buff[100];
char usermsg[100];

main(argc,argv)
char **argv;
{
	long when, tod, now, diff, hours, minutes;
	int *nv;
	int atoi();
	int *localtime();

	if (argc > 2) strcpy(usermsg,argv[2]);
	if (argc == 2) strcpy(usermsg,"You have to leave");
	if (argc < 2) {
		printf("When do you have to leave? ");
		fflush(stdout);
		buff[read(0,buff,sizeof buff)] = 0;
		printf("Message? ");
		fflush(stdout);
		usermsg[read(0,usermsg,80) - 1 ] = '\0';
					/*Replace CR with nul */
		if (usermsg[0] == '\0') strcpy(usermsg,"You have to leave");
	} else {
		strcpy(buff,argv[1]);
	}
	if (buff[0] == '\n')
		exit(0);
	strcpy(origlogin,getlogin());	/* will need to know in bother */
	if (buff[0] == '+') {
		diff = atoi(buff+1);
		doalarm(diff,usermsg);
	}
	if (buff[0] < '0' || buff[0] > '9') {
		printf("usage: %s [hhmm] [message]\n",argv[0]);
		exit(1);
	}

	tod = atoi(buff);
	hours = tod / 100;
	if (hours > 12)
		hours -= 12;
	if (hours == 12)
		hours = 0;
	minutes = tod % 100;

	if (hours < 0 || hours > 12 || minutes < 0 || minutes > 59) {
		printf("usage: %s [hhmm] [message]\n",argv[0]);
		exit(1);
	}

	time(&now);
	nv = localtime(&now);
	when = 60*hours+minutes;
	if (nv[2] > 12) nv[2] -= 12;	/* do am/pm bit */
	now = 60*nv[2] + nv[1];
	diff = when - now;
	while (diff < 0)
		diff += 12*60;
	if (diff > 11*60)
	   {printf("That time has already passed!\n");
	    exit(0);}
	doalarm(diff,usermsg);
	exit(0);
}


doalarm(nmins,msg)
long nmins;
char msg[];
{
	char msg1[100], msg2[100], msg3[100];
	char *msg4;
	register int i;
	int slp1, slp2, slp3, slp4;
	int seconds, gseconds;
	long daytime;

	seconds = 60 * nmins;
	if (seconds <= 0)
		seconds = 1;
	gseconds = seconds;

	strcpy(msg1,msg); strcpy(msg2,msg); strcpy(msg3,msg);
	strcat(msg1," in 5 minutes");
	
	if (seconds <= 60*5) {
		slp1 = 0;
	} else {
		slp1 = seconds - 60*5;
		seconds = 60*5;
	}

	strcat(msg2," in one minute!");
	if (seconds <= 60) {
		slp2 = 0;
	} else {
		slp2 = seconds - 60;
		seconds = 60;
	}

	strcat(msg3," right now!");
	slp3 = seconds;

	msg4 = "You're going to be late!";
	slp4 = 60;

	time(&daytime);
	daytime += gseconds;
	whenleave = ctime(&daytime);
	printf("Alarm set for %s\n",whenleave);
	if (fork())
		exit(0);
	signal(2,1);
	signal(3,1);
	signal(15,1);

	if (slp1)
		bother(slp1,msg1);
	if (slp2)
		bother(slp2,msg2);
	bother(slp3,msg3);
	for (i=1;i<=2;i++) bother(slp4,msg4);
	exit(0);
}

bother(slp,msg)
int slp;
char *msg;
{

	delay(slp);
	printf("\7\7\7");
	printf("%s\n",msg);
}

/*
 * delay is like sleep but does it in 100 sec pieces and
 * knows what zero means.
 */
delay(secs) int secs; {
	int n;

	while(secs>0) {
		n = 100;
		secs = secs - 100;
		if (secs < 0) {
			n = n + secs;
		}
		if (n > 0)
			sleep(n);
		strcpy(thislogin,getlogin());
		if (strcmp(origlogin, thislogin))
			exit(0);
	}
}

#ifdef V6
char *getlogin() {
#include <utmp.h>

	static struct utmp ubuf;
	int ufd;

	ufd = open("/etc/utmp",0);
	seek(ufd, ttyn(0)*sizeof(ubuf), 0);
	read(ufd, &ubuf, sizeof(ubuf));
	ubuf.ut_name[sizeof(ubuf.ut_name)] = 0;
	return(&ubuf.ut_name);
}
#endif
