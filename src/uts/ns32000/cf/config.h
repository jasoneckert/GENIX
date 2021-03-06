/* Configuration Definition */
#define HOSTMEM 1

#define	SXT_0 20
#define	FP_0 1
#define	CON_0 1
#define	MEMORY_0 1
#define	TTY_0 1
#define	ERRLOG_0 1
#define	NBUF	200
#define	NINODE	175
#define	NFILE	175
#define	NMOUNT	8
#define	NCALL	50
#define	NPROC	65
#define	NCLIST	150
#define	NSABUF	0
#define	POWER	0
#define	MAXUP	25
#define	NHBUF	64
#define	NPBUF	20
#define	CSIBNUM	20
#define	VPMBSZ	8192
#define	VPMNEXUS	0
#define	X25LINKS	1
#define	X25BUFS	256
#define	X25NEXUS	0
#define	X25BYTES	(16*1024)
#define	BX25LINKS	2
#define	BX25BUFS	80
#define	BX25NEXUS	0
#define	BX25BYTES	(16*1024)
#define	BX25HLPROT	2
#define	SESBUFS	32
#define	SESBYTES	(8*1024)
#define	MESG	1
#define	MSGMAP	100
#define	MSGMAX	8192
#define	MSGMNB	16384
#define	MSGMNI	50
#define	MSGSSZ	8
#define	MSGTQL	40
#define	MSGSEG	1024
#define	SEMA	1
#define	SEMMAP	10
#define	SEMMNI	10
#define	SEMMNS	60
#define	SEMMNU	30
#define	SEMMSL	25
#define	SEMOPM	10
#define	SEMUME	10
#define	SEMVMX	32767
#define	SEMAEM	16384
#define	SHMEM	1
#define	SHMMAX	(128*1024)
#define	SHMMIN	1
#define	SHMMNI	100
#define	SHMSEG	6
#define	SHMBRK	16
#define	SHMALL	512
#define	STIBSZ	8192
#define	STOBSZ	8192
#define	STIHBUF	(ST_0*4)
#define	STOHBUF	(ST_0*4)
#define	STNPRNT	(ST_0>>2)
#define	STNEXUS	0
#define	EMTBSZ	8192
#define	EMRBSZ	8192
#define	EMRCVSZ	2048
#define	EMBHDR	(EM_0*6)
#define	EMNEXUS	0
#define	FLCKREC	200
#define	FLCKFIL	50
#define	REGIONS	163
#define	GETPGSLO	25
#define	GETPGSHI	50
#define	VHANDR	4
#define	VHANDL	999999
#define	VHNDFRAC	8
#define	AGERATE	5
#define	PREPAGE	4
#define	MAXSC	32
#define	SPTMAP	200
#define	MAXPMEM	0

#define NVBLKD	32	/* number of disk devices */
#define NREQPD	1	/* number of i/o requests outstanding per disk */
#define NVCHRD	8	/* number of tty devices */
#define NVTAPE	2	/* number of magtape devices */
#define NVFILE	16	/* number of host file devices */
#define	NVHMEM	4	/* number of host memory devices */
