/*
 * @(#)nlist.h	1.3	7/13/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

/*
 * Simplified, translated format of a symbol table entry:
 *
 * Note:
 *	Complete symbol table definitions reside in <stab.h> and simplified
 *	definitions in <nlist.h>.  These sets of definitions are not fully
 *	compatible with each other; symbol table information as defined in
 *	<nlist.h> does not correspond directly to a.out file contents, but
 *	instead is obtained by translating such information with nlist(3).
 *	The <nlist.h> definitions do not provide access to all symbol table
 *	information, but are more convenient than the <stab.h> definitions
 *	for what they do cover.
 *
 *	<Stab.h> and <nlist.h> are mutually incompatible;
 *	at most one may legally be included in a compilation.
 *
 * The n_type and n_value fields are not contained directly in
 * an a.out file, but rather are pieced together by nlist(3) from
 * addressing mode, link table, and module table information in
 * the file.
 */
struct	nlist {
	char		*n_name;	/* symbol name		*/
	unsigned char	n_type;		/* type flag; see below	*/
	char		n_other;	/* see nlist(3)		*/
	short		n_desc;		/* unused		*/
	unsigned long 	n_value;	/* symbol value		*/
};

/*
 * Simple values for n_type.
 *
 * Note:
 *	Nlist(3) never returns a type of N_COMM.
 *
 *	Nor does it ever return a type of N_FN.
 *	However, this may change in the future.
 */
# define N_UNDF	0x00	/* undefined			*/
# define N_ABS	0x02	/* absolute			*/
# define N_TEXT	0x04	/* text				*/
# define N_DATA	0x06	/* data				*/
# define N_BSS	0x08	/* bss				*/
# define N_COMM	0x12	/* common (internal to ld)	*/
# define N_FN	0x1f	/* file name symbol		*/

# define N_EXT	0x01	/* external bit, or'ed in	*/
# define N_TYPE	0x1e	/* mask for all the type bits	*/

/*
 * Sdb entries have some of the N_STAB bits set.
 *	These are given in <stab.h>.
 *
 *	Note that nlist(3) does not return stab information.
 */
# define N_STAB	0xe0	/* if any of these bits set, a SDB entry */

/*
 * Format for namelist values.
 */
# define N_FORMAT	"%08x"
