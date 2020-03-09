/*
 * @(#)meld3.h	1.2	7/13/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

#define meldmagic 0xccf0

struct imagetype {
	int	magic;		/* magic number: ccf0(16) 	*/
	int	modcount;	/* number of modules	  	*/
	int	reloc;		/* load address			*/
	int	text_size;	/* bytes of text (mod,prog,link)*/
	int	data_size;	/* bytes of data (statics)	*/
	int	mod_begin;	/* mod table starts here 	*/
};				/* ``text_size'' includes padding
				   to sync the static area with
				   the next page */

union header {
	struct	imagetype subheader;
	char 	imagetotal[1024];
};

#define sysize 6		/* use this [sizeof(syentry)==8, too bad] */

struct syentry {
	int	value;		/* value of symbol */
	short	module;		/* module number symbol is exported from */
};				/* bytes of name follow, null-terminated */
