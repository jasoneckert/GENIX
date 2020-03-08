/*
 * @(#)file.h	3.4	7/14/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

/*
 * One file structure is allocated
 * for each open/creat/pipe call.
 * Main use is to hold the read/write
 * pointer associated with each open
 * file.
 */
struct	file
{
	short	f_flag;
	short	f_count;	/* reference count */
	struct inode *f_inode;	/* pointer to inode structure */
	union {
		off_t	f_offset;	/* read/write character pointer */
		struct chan *f_chan;	/* mpx channel pointer */
	} f_un;
};

#ifdef	KERNEL
extern struct file *file;	/* The file table itself */
extern struct file *fileNFILE;	/* pointer to end of table */
extern int nfile;

struct	file *getf();
struct	file *falloc();
#endif

/* flags */
#define	FREAD	01
#define	FWRITE	02
#define	FPIPE	04
#define	FKERNEL	040
