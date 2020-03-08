/*
 * nlist.c: version 1.4 of 4/12/83
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)nlist.c	1.4 (NSC) 4/12/83";
# endif

/*
 * nlist - retrieve attributes from name list
 *
 *	This version works on Mesa Unix style a.out files.
 *	Thus, it assumes the existence of a string table and
 *	of a symbol table containing extensive type and scope
 *	information.
 *
 *	Symbol information is translated from the Mesa a.out
 *	format to the simplified nlist format.
 */

# include <sys/types.h>
# include <pagsiz.h>	/* used to avoid pulling in all of stdio */
# ifdef DEBUG
# include <stdio.h>
# endif DEBUG
# include <a.out.h>
# include <stab.h>
# include <sys/modtable.h>

# ifndef Static
# ifdef DEBUG
# define Static
# else
# define Static	static
# endif
# endif Static

typedef struct nlist	f_nlist;

/*
 * The following structure definition is identical to the one
 * in <nlist.h> except for names, which have been changed
 * to prevent conflicts with the nlist definition in <stab.h>.
 * Note that both of <nlist.h> and <stab.h> cannot be simultaneously
 * included due to name clashes.  Presumably, this is the only site
 * where this conflict causes trouble, since nlist()'s purpose
 * is to translate the stab-style nlist entry to the simpler nlist-style
 * entry.
 *
 * WARNING:
 *	If this structure is ever changed, the one in <nlist.h>
 *	should change accordingly, and conversely!  Ditto for
 *	the defines that follow.
 */
typedef struct nl_nlist {
	char	*nl_name;
	u_char	nl_type;
	char	nl_other;
	short	nl_desc;
	u_long	nl_value;
} nl_nlist;
# define NL_UNDEF	0x00
# define NL_ABS		0x02
# define NL_TEXT	0x04
# define NL_DATA	0x06
# define NL_BSS		0x08
# define NL_COMM	0x12
# define NL_FN		0x1f	/* that's odd, it's odd!!!	*/
# define NL_EXT		0x01

static int			infd;
static struct exec		hdr;
static off_t			txt_off;
static char			*sym_base,	/* pointers to start of	*/
				*str_base;	/* buffers for syms &	*/
						/* strings		*/
static f_nlist			*nl_curp,	/* pointers to current	*/
				*nl_LIMIT;	/* & boundary STEs	*/
static struct modtable		curmod;		/* cur mod table entry	*/
static u_char			mt_init_data;	/* cur mod has data/bss	*/

extern f_nlist	*get_STE      (),
		*get_info     (),
		*eat_aux_STEs (),
		*do_mt_entry  ();
extern char	*sbrk         (),
		*brk          ();

nlist (name, list)
	char		*name;
	nl_nlist	*list;
{
	register nl_nlist	*nnp;
	register f_nlist	*np;
	register int		i, tmp;
	register int		maxlen,
				nreq;
	register char		*namep;
	char			*old_brk;
	int			retcode;

	retcode = 0;

	/*
	 * Get number of symbols to process and
	 * the length of the longest name among them.
	 */
	maxlen = 0;
	for ( nnp = list, nreq = 0
	    ; nnp->nl_name && nnp->nl_name[0]
	    ; nnp++, nreq++
	    ) {
		nnp->nl_type  = 0;
		nnp->nl_value = 0;
		nnp->nl_desc  = 0;
		nnp->nl_other = 0;
		i             = strlen(nnp->nl_name);
		if (i > maxlen)
			maxlen = i;
	}

	/* Make sure the file of symbol defs is legitimate: */
	infd = open (name, 0);
	if (infd < 0)
		return (-1);
	(void) read (infd, (char *) &hdr, sizeof hdr);
	if (N_BADMAG (hdr)) {
		close  (infd);
		return (-1);
	}

	/*
	 * Allocate space for the symbol and string tables
	 * and read them in.
	 */
	tmp      = hdr.a_syms + hdr.a_strings;
	old_brk  = sbrk (0);
	sym_base = sbrk (tmp);
	str_base = sym_base + hdr.a_syms;
	(void) lseek (infd, (long) N_SYMOFF (hdr), 0);
	if (read (infd, sym_base, tmp) != tmp) {
		retcode = -1;
		goto alldone;
	}

	/* Set up pointers into symbol table: */
	nl_curp  = (f_nlist *) sym_base;
	nl_LIMIT = (f_nlist *) ((char *) nl_curp + hdr.a_syms);

	/*
	 * Search through the symbol table, looking for info
	 * pertinent to the symbols we're interested in.
	 */

	/* Bias txt_off to compensate for unmapped address space portions: */
	tmp        = N_TXTOFF (hdr);
	txt_off    = (hdr.a_magic == XMAGIC) ? tmp - 1024
					     : tmp;

	/*
	 * Invariant at loop top:
	 *	The previous MSTE and all its auxiliary entries
	 *	have been consumed.
	 */
	while ((np = get_STE ()) != (f_nlist *) 0) {

		/* Check for the start of a new module: */
		if (N_MODULE (*np)) {
			np = do_mt_entry (np);
			continue;
		}

		/* Get symbol name and check for a match: */
		namep = str_base + N_STRX (np->n_name);
		for (nnp = list; nnp->nl_name[0]; nnp++) {
			i = 0;
			while (nnp->nl_name[i]) {
				if (nnp->nl_name[i] != namep[i])
					goto cont;	/* no match */
				i++;
			}
			if (namep[i])
				goto cont;	/* too long */

#			ifdef notdef
#			ifdef DEBUG
			printf ("match: %s\n", nambuf);
#			endif
#			endif notdef

			/*
			 * We've found one of the symbols we're looking
			 * for; fill in info about it:
			 *
			 * Points to note:
			 *   -	Unless we can't find a defining occurrence
			 *	for the symbol, we don't really care about
			 *	mere references to the symbol.  We use the
			 *	nl_other to record whether we've yet seen
			 *	a definitive occurrence during processing.
			 *   -	We (for now, at least) ignore scope info in
			 *	the symbol table.  Thus, we use the first
			 *	defining info we see for each symbol.
			 */

			/*
			 * If we already have definitive info about the symbol,
			 * ignore the new info, but just gobble up its
			 * auxiliary STEs:
			 */
			if (nnp->nl_other) {
#				ifdef notdef
#				ifdef DEBUG
				printf ("skipping %s\n", nnp->nl_name);
#				endif
#				endif notdef
				break;
			}

			/* If the symbol is of suitable kind, grab its info: */
			if (np->n_etype <= N_METN)
				np = get_info (np, nnp);

			/*
			 * If this was a definitive occurrence, decrement
			 * the number of symbols we still have to check:
			 */
			if (nnp->nl_other && --nreq <= 0)
				goto alldone;
			break;

		cont:
			;
		}

		/*
		 * Gobble up auxiliary STEs remaining
		 * from the symbol just examined:
		 */
		np = eat_aux_STEs (np);
	}
alldone:
	/* Free symbol and string buffer space: */
	(void) brk (old_brk);

	(void) close (infd);
	return (retcode);
}

/*
 * Process a module table entry symbol.
 */
Static f_nlist *
do_mt_entry (np)
	register f_nlist	*np;
{
	/* Read in the associated mod table entry: */
	(void) lseek (infd, txt_off + np->n_value, 0);
	(void) read  (infd, (char *) &curmod, sizeof (struct modtable));
#	ifdef notdef
#	ifdef DEBUG
	printf	( "MOD: sb: %o, lb: %o, pb: %o\n"
		, curmod.mt_sb, curmod.mt_lb, curmod.mt_pb
		);
#	endif DEBUG
#	endif notdef

	/*
	 * Record whether the static base section for this module
	 * belongs in the data or bss segments:
	 */
	mt_init_data = np->n_initialized;

	/* Consume trailing auxiliary STEs: */
	return (eat_aux_STEs (np));
}

/*
 * Use addressing mode and type info from the symbol table entry
 * *np and its associated auxiliary entries to calculate the value
 * and type fields of *nnp.  Return a pointer to the last STE
 * examined during this calculation.
 */
Static f_nlist *
get_info (np, nnp)
	register f_nlist	*np;
	register nl_nlist	*nnp;
{
	register int	disp1,
			disp2;
	long		link_entry;
	struct modtable	xmod;

	switch (np->n_addrmode) {
	case A_EX:
#		ifdef notdef
#		ifdef DEBUG
		if (nnp->nl_other)
			printf ("symbol already seen definitively\n");
#		endif DEBUG
#		endif notdef
		/*
		 * We don't consider an external addressing mode symbol
		 * to constitute definitive information.  If there is another
		 * definition elsewhere, it is easier to extract type info
		 * directly from it, and if there isn't, we can't make a
		 * data/bss determination anyhow.  We do what we can here,
		 * hoping to be superceded later.  Note that data/bss symbols
		 * are arbitrarily called data.
		 *
		 * The above comments aren't quite right.  We do have a chance
		 * of making a data/bss determination.  There may be an
		 * auxiliary STE following that specifies auxtype of common.
		 * In this case we can infer that the symbol lives in the bss.
		 * If such an auxiliary entry is absent, by default we take
		 * the symbol to be data.
		 */
		/*
		 * If the STE doesn't use the N_at field, there's nothing we
		 * can do with it here...
		 */
		if (np->n_etype != N_MAT)
			break;
		nnp->nl_type  = ((np->n_proc) ? NL_TEXT : NL_DATA) +
				((np->n_ext)  ? NL_EXT  : 0      );
		/* Get all the displacements we need: */
		if (np->n_disp1 == -32768) {
			/*
			 * Displacements too big for a doubleword;
			 * go off to alternate locations to get them:
			 */
			np = get_STE ();
			disp1 = np->n_ldisp1;
			disp2 = np->n_ldisp2;
		}
		else {
			/* Short displacements: */
			disp1 = np->n_disp1;
			disp2 = np->n_disp2;
		}
		/* Grab link table entry: */
		(void) lseek (infd, txt_off + curmod.mt_lb + 4*disp1, 0);
		(void) read  (infd, &link_entry, sizeof link_entry);
#		ifdef notdef
#		ifdef DEBUG
		printf ("link entry: %o\n", link_entry);
#		endif DEBUG
#		endif notdef
		/*
		 * If the symbol is a procedure, we have to decode a mod/offset
		 * pair; otherwise we have to complete the external addressing
		 * mode calculation.
		 */
		if ((nnp->nl_type & ~NL_EXT) == NL_TEXT) {
			/* Get appropriate mod table entry: */
			(void) lseek (infd, txt_off + (link_entry & 0xffff), 0);
			(void) read  (infd, &xmod, sizeof xmod);

			nnp->nl_value = xmod.mt_pb +
					((link_entry >> 16) & 0xffff);
		}
		else {
			nnp->nl_value = link_entry + disp2;
		}
		/*
		 * Check for auxiliary entries 
		 * with more type-determining info:
		 */
		if (np->n_more) {
			np = get_STE ();
			if (  (nnp->nl_type & NL_DATA)
			   && np->n_etype == N_AUX
			   && !np->n_typeinfo
			   && np->n_auxtype == N_COMMON
			   ) {
				nnp->nl_type = NL_BSS | (nnp->nl_type & NL_EXT);
			}
		}
		break;

	case A_SB:
		nnp->nl_value = curmod.mt_sb + np->n_value;
		nnp->nl_type  = ((mt_init_data) ? NL_DATA : NL_BSS) +
				((np->n_ext)    ? NL_EXT  : 0     );
		nnp->nl_other++;
		break;

	case A_PC:
		nnp->nl_value = curmod.mt_pb + np->n_value;
		nnp->nl_type  = NL_TEXT + ((np->n_ext) ? NL_EXT : 0);
		nnp->nl_other++;
		break;

	default:
#		ifdef DEBUG
		printf ("need case for addr mode %o\n", np->n_addrmode);
#		endif DEBUG
		break;
	}

#	ifdef notdef
#	ifdef DEBUG
	printf ("value: %o, type: %o\n", nnp->nl_value, nnp->nl_type);
#	endif DEBUG
#	endif notdef

	return (np);
}

/*
 * Obtain the next symbol table entry and return a pointer to it,
 * or 0 if there are no more.
 */
Static f_nlist *
get_STE ()
{
	if (nl_curp < nl_LIMIT)
		return (nl_curp++);
	return ((f_nlist *) 0);
}

/*
 * Discard auxiliary STEs up to the next main STE, i.e.,
 * so that the next STE read will be a main STE.
 */
Static f_nlist *
eat_aux_STEs (np)
	register f_nlist	*np;
{
	/* Gobble as long as the n_more bit is set: */
	while (np->n_more)
		np = get_STE ();
	return (np);
}

# ifdef DEBUG
static char *etype [] = {
	"MAT",
	"MTN",
	"MPTN",
	"METN",
	"SLINE",
	"SCHAR",
	"MAUX",
	"AUX"
};

# ifdef notdef
dump_STE (np)
	register f_nlist	*np;
{
	register char	*namep;

	namep = str_base + N_STRX (np->n_name);

	/* Blank line before MSTEs: */
	if (np->n_etype <= N_METN || np->n_etype == N_MAUX)
		putchar ('\n');

	/* Addr mode, if relevant: */
	if (np->n_etype <= N_SCHAR)
		printf ("%02o ", np->n_addrmode);
	else
		printf ("   ");

	printf	( "%5s %c %o\t%o\t%c[%s]\t%s\n"
		, etype[np->n_etype]
		, np->n_ext ? 'E' : ' '
		, np->n_type
		, np->n_value
		, np->n_more ? '+' : ' '
		, namep
		, N_MODULE (*np) ? "MOD" : "   "
		);
}
# endif notdef
# endif DEBUG
