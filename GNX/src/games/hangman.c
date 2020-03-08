# ifndef NOSCCS
static char *sccsid = "@(#)hangman.c	1.5	8/1/83";
static char *cpyrid = "@(#)Copyright (C) 1983 by National Semiconductor Corp.";
# endif

/* The game of hangman adapted to video terminals - David I. Bell */

#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#include <curses.h>

#define DICT "/usr/dict/words"
#define EDICT "/usr/lib/w2006"
#define	STDIN 0
#define MAXERR 7		/* maximum number of wrong guesses */
#define	PENALTY 2
#define	MAXSAVE 10		/* number of previous games to remember */
#define MINLEN 7
#define	MAXLEN 20
#define	DATACOL 38		/* column to begin status lines on */
#define	GUESSLINE1 2		/* lines for various outputs */
#define	GUESSLINE2 4
#define	WORDLINE 8
#define	RESULTLINE 10
#define	INPUTLINE 12
#define	SCORELINE 16
#define	GAMESLINE 18
#define	HISTLINE 20
#define	REDRAWCHAR '\014'
#define	MAGIC	17171717
#define	words	save.s_words
#define	errors	save.s_errors
#define	scores	save.s_scores
#define	moves	save.s_moves
#define	alive	save.s_alive
#define	realword save.s_realword
#define	word	save.s_word
#define	letters	save.s_letters
#define	counts	save.s_counts

char	**theperson;	/* pointer to man or woman */
char	*dictfile;	/* name of dictionary file */
char	*savefile;	/* file to save games in */
char	*result;	/* result of the game */
char	*prompt;	/* prompt for the user */
int	savefd;		/* file descriptor for save file */
int	quit;		/* nonzero if wants to quit */
FILE	*dict;		/* dictionary file */
long	dictlen;	/* length of the dictionary file */
int	lastline;	/* last line of the screen */
float	womanchance;	/* chances of getting man or woman */
double	frand();	/* random number generator */
int	gotint();	/* signal routine */


struct	save	{		/* structure which holds saved games */
	int	s_magic;	/* magic number */
	int	s_words;	/* total words played */
	int	s_errors;	/* total number of errors */
	char	s_scores[MAXSAVE];	/* previous game scores */
	int	s_moves;	/* number of moves taken (errors) */
	int	s_alive;	/* wrong guesses left in game */
	char	s_realword[26];	/* the word to be guessed */
	char	s_word[26];	/* the word guessed so far */
	char	s_letters[26];	/* the letters available to guess with */
	short	s_counts[MAXERR+PENALTY+1];	/* histogram of games */
} save;


char	*theman[] = {		/* the characters which make the man */
"     _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _",
"     |   - - - - - - - - - - - - - - - | - - - - -",
"     |   |     /   /                   |",
"     |   |   /   /                  1~1~1~",
"     |   | /   /                1~1~1~1~1~1~1~",
"     |   |   /                  1~  1o  1o  1~",
"     |   | /                    1|    1^    1|",
"     |   |                        1\\  1=  1/",
"     |   |                    2_2_2_1|  1|2_2_2_",
"     |   |                  4/2|              2|5\\",
"     |   |                4/  2|              2|  5\\",
"     |   |              4/    2|              2|    5\\",
"     |   |            4/    4/2|              2|5\\    5\\",
"     |   |          4/    4/  2|2=2=2=2=2=2=2=2|  5\\    5\\",
"     |   |          4\\4_4/    3|              3|    5\\5_5/",
"     |   |                    3|      3_      3|",
"     |   |                    3|    3|  3|    3|",
"     |   |                    3|    3|  3|    3|",
"     |   |                    3|    3|  3|    3|",
"     |   |                6_6_3|    3|  3|    3|7_7_  ",
"     |   |              6|6_6_6_6_6_6|  7|7_7_7_7_7_7|",
"     |   |",
" _ _ |   | _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _",
" - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -",
0
};


char	*thewoman[] = {		/* the characters which make the woman */
"     _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _",
"     |   - - - - - - - - - - - - - - - | - - - - -",
"     |   |     /   /                   |",
"     |   |   /   /                  1~1~1~",
"     |   | /   /                1~1~1~1~1~1~1~",
"     |   |   /                  1~  1o  1o  1~",
"     |   | /                    1~    1^    1~",
"     |   |                      1~1\\  1=  1/1~",
"     |   |                    2_2_2_1|  1|2_2_2_",
"     |   |                  4/2|              2|5\\",
"     |   |                4/  2|              2|  5\\",
"     |   |              4/  4/2|              2|5\\  5\\",
"     |   |            4/  4/  2|              2|  5\\  5\\",
"     |   |          4/  4/    3/2=2=2=2=2=2=2=3\\    5\\  5\\",
"     |   |          4\\4/    3/                  3\\    5\\5/",
"     |   |                3/                      3\\",
"     |   |              3/                          3\\",
"     |   |            3/3_3_3_3_3_3_3_3_3_3_3_3_3_3_3_3\\",
"     |   |                      6|  6|  7|  7|",
"     |   |                  6_6_6|  6|  7|  7|7_7_",
"     |   |                6|6_6_6_6_6|  7|7_7_7_7_7|",
"     |   |",
" _ _ |   | _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _",
" - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -",
0
};

main(argc,argv)
	char	**argv;
{
	register char	ch;

	argv++;
	dictfile = DICT;
	womanchance = 0.5;
	while ((argc > 1) && (**argv == '-')) {
		switch (argv[0][1]) {
			case 'd':
				dictfile = EDICT;
				if (argv[0][2]) dictfile = &argv[0][2];
				break;
			case 'm':
				womanchance = 0.0;
				break;
			case 'w':
				womanchance = 1.0;
				break;
			default:
				fatal("bad option");
		}
		argc--;
		argv++;
	}
	if (argc > 1) savefile = *argv;
	setup();
	while (1) {
		startnew();
		while (alive > 0) getguess();
		prompt = "Another game? ";
		while (1) {
			ch = readchar();
			if (ch == 'n') savegame();
			if (ch == 'y') break;
		}
	}
}


/* Initialize dictionary file, saved game status, and terminal screen */
setup()
{
	int tvec[2];
	struct stat statb;

	time(tvec);
	srand(tvec[0]+tvec[1]);
	if((dict = fopen(dictfile,"r")) == NULL) fatal("no dictionary");
	if(stat(dictfile,&statb) < 0) fatal("can't stat");
	dictlen = statb.st_size;
	if (savefile) {
		save.s_magic = MAGIC;
		if (access(savefile, 0) == 0) {
			savefd = open(savefile, 2);	/* file exists */
			if (savefd < 0) {
				perror("cannot open data file");
				exit(1);
			}
			if (read(savefd, &save, sizeof(save)) < 0) {
				perror("cannot read data file");
				exit(1);
			}
			lseek(savefd, 0, 0);
			if (save.s_magic != MAGIC) 
				fatal("bad magic number in data file");
			scrambleword();		/* unscramble the word */
		} else {
			savefd = creat(savefile, 0666);		/* create it */
			if (savefd < 0) {
				perror("cannot create data file");
				exit(1);
			}
		}
	}
	initscr();
	lastline = stdscr->_maxy - 1;
	crmode();
	noecho();
	signal(SIGINT, gotint);
}



/* Start a new game */
startnew()
{	int i;
	long int pos;
	char buf[128];

	prompt = "Guess: ";
	theperson = (frand() >= womanchance) ? thewoman : theman;
	if (alive > 0)
		return;		/* true if continuing a saved game */
	for (i = 0; i < 26; i++) {
		word[i] = 0;
		realword[i] = 0;
		letters[i] = 'a' + i;
	}
	pos = frand()*dictlen;
	fseek(dict, pos, 0);
	fscanf(dict, "%s\n", buf);
	getword();
	alive = MAXERR;
	moves = 0;
}


/* Show the current status of the game */
showstate()
{
	register char	**sp;
	register char	*s;
	register int	i;

	move(0, 0);			/* display the picture */
	for (sp = theperson; *sp; sp++) {
		for (s = *sp; *s; s++) {
			if (*s++ <= moves + '0') addch(*s); else addch(' ');
		}
		addch('\n');
	}

	move(WORDLINE, DATACOL);	/* show the word guessed so far */
	printw("Word:   ");
	for (i = 0; i < 26; i++) {
		if (word[i] == 0) break;
		addch(word[i]);
		addch(' ');
	}
	clrtoeol();

	move(GUESSLINE1, DATACOL);	/* show the list of guesses */
	for (i = 0; i < 13; i++) {
		addch(letters[i]);
		addch(' ');
	}
	clrtoeol();
	move(GUESSLINE2, DATACOL);
	for (i = 13; i < 26; i++) {
		addch(letters[i]);
		addch(' ');
	}
	clrtoeol();

	if (alive <= 0) {		/* show the game result */
		move(RESULTLINE, DATACOL);
		printw("%s %s", result, realword);
		clrtoeol();
	}

	if (words) {			/* show the current score */
		move(SCORELINE, DATACOL);
		printw("Games: %d    Average score: %4.2f",
			words, (float)errors/words);
		clrtoeol();
	}

	if (words > 1) {		/* show previous games */
		move(GAMESLINE, DATACOL);
		printw("Last %d scores:", (words <= MAXSAVE) ? words : MAXSAVE);
		for (i = 0; ((i < words) && (i < MAXSAVE)); i++)
			printw(" %d", scores[i]);
		clrtoeol();
	}

	if (words) {			/* show histogram of scores */
		move(HISTLINE, DATACOL);
		printw("Histogram:");
		for (i = 0; i < MAXERR; i++) printw(" %d", counts[i]);
		printw(" - %d", counts[MAXERR+PENALTY]);
		clrtoeol();
	}

	move(INPUTLINE, DATACOL);	/* show the appropriate prompt */
	printw(prompt);
	clrtoeol();

	refresh();			/* update the screen!! */
}


/* Read the current guess */
getguess()
{
	register char	c;
	register int	i;
	int	ok;

	while (1) {
		c = readchar();
		if ((c < 'a') || (c > 'z')) continue;
		if (letters[c-'a'] != '.') break;
	}
	letters[c-'a'] = '.';
	ok = 0;
	for (i = 0; realword[i] != 0; i++) {
		if (realword[i] == c) {
			word[i] = c;
			ok = 1;
		}
	}
	if (ok == 0) {
		alive--;
		moves++;
		if (alive <= 0) endgame(1);
		return;
	}
	for (i = 0; word[i] != 0; i++)
		if (word[i] == '.') return;
	endgame(0);
}



/* Terminate the current game */
endgame(lost)
{
	register int	i;

	result = "You win, the answer is";
	if (lost) {
		result = "You lose, the answer was";
		moves += PENALTY;
	}
	errors += moves;
	words++;
	for (i = MAXSAVE-1; i > 0; i--) scores[i] = scores[i-1];
	scores[0] = moves;	/* insert score into saved list */
	counts[moves]++;
	alive = 0;
}


/* Save the current game status and exit */
savegame()
{
	move(lastline, 0);
	clrtoeol();
	refresh();
	endwin();
	if (savefile) {
		scrambleword();		/* protect word from prying eyes */
		if (write(savefd, &save, sizeof(save)) != sizeof(save)) {
			perror("cannot write data file");
			exit(1);
		}
	}
	exit(0);
}


/* Read a character from the user */
readchar()
{
	extern	int	errno;
	char	ch;

	showstate();
	while (1) {
		if (quit) savegame();	/* wants out, save game and exit */
		errno = 0;
		if (read(STDIN, &ch, 1) > 0) break;
		if (errno != EINTR) quit = 1;
	}
	if ((ch >= 'A') && (ch <= 'Z')) ch += 'a' - 'A';
	if (ch == REDRAWCHAR) wrefresh(curscr);
	return(ch);
}


/* Scramble (or unscramble) the word to be guessed so it isn't obviously
 * visible in the saved game status.  Not meant to be infinitely secure,
 * but secure enough so that typing the file won't show the word.
 */
scrambleword()
{
	register int	i;
	register int	j;

	j = 111;
	for (i = 0; i < 26; i++) {
		realword[i] ^= j;
		j = j * 19 + 37;
	}
}


/* Select a word from the dictionary */
getword()
{	char wbuf[128],c;
	int i,j;
loop:
	if(fscanf(dict,"%s\n",wbuf)==EOF)
	{	fseek(dict,0L,0);
		goto loop;
	}
	if((c=wbuf[0])>'z' || c<'a') goto loop;
	for(i=j=0;wbuf[j]!=0;i++,j++)
	{	if(wbuf[j]=='-') j++;
		wbuf[i]=wbuf[j];
	}
	wbuf[i]=0;
	if ((i<MINLEN) || (i>MAXLEN)) goto loop;
	for(j=0;j<i;j++)
		if((c=wbuf[j])<'a' || c>'z') goto loop;
	strcpy(realword,wbuf);
	for(j=0;j<i;word[j++]='.');
}



/* Compute a random floating point number */
double frand()
{
	double	pow();

	return(rand() / pow(2.0, (double)(8 * sizeof(int)) - 1));
}


/* Here on an interrupt signal */
gotint()
{
	signal(SIGINT, gotint);		/* reenable it */
	quit = 1;
}


/* Here on a fatal error */
fatal(s) char *s;
{
	fprintf(stderr, "%s\n", s);
	exit(1);
}
