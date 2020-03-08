/*
 * cnlist.c: version 1.1 of 2/7/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)cnlist.c	1.1 (NSC) 2/7/83";
# endif

/* Like nlist, but reads compressed symbols (ie, a smaller file containing
 * only the needed symbols).   David I. Bell
 */

#include <sys/types.h>
#include <nlist.h>
#include <cnlist.h>
#define	MAXSYM	1000		/* sanity check */


struct	cnlist	cn;		/* header of file */
char	*data;			/* pointer to data */
struct	nlist	*symbols;	/* pointer to array of symbols */
unsigned long	symcount;	/* number of symbols in file */


cnlist(filename, nl)
	char	*filename;
	register struct	nlist	nl[];
{
	register struct	nlist	*sym;
	register int	io;
	register int	size;

	sym = nl;
	while (sym->n_name) {
		sym->n_type = 0;
		sym->n_value = 0;
		sym++;
	}
	io = open(filename, 0);
	if (io < 0) {
		return;
	}
	read(io, &cn, sizeof(cn));
	symcount = cn.cn_count;
	if ((cn.cn_magic != CN_MAGIC) || (symcount > MAXSYM)) {
		close(io);
		return;
	}
	size = lseek(io, 0, 2);
	if (size <= 0) {
		close(io);
		return;
	}
	data = (char *) malloc(size);
	lseek(io, 0, 0);
	read(io, data, size);
	symbols = (struct nlist *) (data + sizeof(cn));
	for (sym = symbols; sym < &symbols[symcount]; sym++) {
		sym->n_name += (int)data;	/* relocate symbols */
	}
	while (nl->n_name) {
		for (sym = symbols;  sym < &symbols[symcount]; sym++) {
			if (strcmp(nl->n_name, sym->n_name) == 0) {
				nl->n_type = sym->n_type;
				nl->n_other = sym->n_other;
				nl->n_desc = sym->n_desc;
				nl->n_value = sym->n_value;
				break;
			}
		}
		nl++;
	}
	free(data);
	close(io);
}
