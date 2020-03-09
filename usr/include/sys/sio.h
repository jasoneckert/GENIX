/*
 * @(#)sio.h	3.10	7/22/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

/*
 * Definitions for the Mesa 8 port serial i/o board.
 */

# ifdef KERNEL
# include "../h/types.h"
# else
# include <sys/types.h>
# endif

# define SIO_LINES	8

/*
 * SIO board command layout:
 *	The union is needed to facilitate byte accesses to the SIO board.
 */
typedef struct sio_cmd {
	union {
	    struct sio_cmd_byte0 {
		u_char	c_cmd  : 4,	/* command code -- see below	*/
			c_chan : 3,	/* affected line		*/
			c_int  : 1;	/* interrupt when done?		*/
	    } C_bits;
	    u_char	C_byte;
	} C_un;
	u_char		c_fill;
	u_char		c_data0,	/* used for C_CONFIG		*/
			c_data1;	/*   "   "     "		*/
} sio_cmd;

/*
 * Command code definitions:
 */
# define C_DONE		0x00 /* command chain end			*/
# define C_DMA_NO	0x01 /* stop dma activity			*/
# define C_DMA_GO	0x02 /* (re)start dma activity			*/
# define C_MDM_CLR	0x03 /* clear modem/line control bits		*/
# define C_MDM_SET	0x04 /* set modem/line control bits		*/
# define C_LINE_OFF	0x05 /* disable line				*/
# define C_LINE_ON	0x06 /* enable line				*/
# define C_INT_SINGLE	0x07 /* interrupt on every char			*/
# define C_INT_MANY	0x08 /* interrupt on 3/4 full			*/
# define C_ZAP_FIFO	0x09 /* clear fifo				*/
# define C_CONFIG	0x0a /* reconfigure a line			*/
# define C_SUBR		0x0b /* call subroutine (for diagnostics)	*/
# define C_NOP		0x0f /* null command				*/

/*
 * Usart configuration register bit assignments:
 *	Only values that are actually used are defined.
 *	They go into the two data bytes of the C_CONFIG command.
 */
# define UART_MODE	0x0003	/* usart mode & baud factor	*/
# define	U_MD_ASYNC1X	0x0001
# define UART_CH_LEN	0x000c	/* character length		*/
# define	U_LEN7		0x0008
# define	U_LEN8		0x000c
# define UART_PAR_ON	0x0010	/* parity enabled?		*/
# define UART_PAR_EVEN	0x0020	/* even parity?			*/
# define UART_STOP_BITS	0x00c0	/* number of stop bits		*/
# define	U_STOP1		0x0040
# define UART_BAUD	0x0f00	/* baud rate			*/
# define UART_CLOCK	0x3000	/* xmtr & rcvr clock sources	*/
# define	U_IN_CLK	0x3000

/*
 * Line status bit definitions (modem control information):
 *	These bits are used in the data byte of the C_MODEM command
 *	(to be defined shortly) and in the m_modem[i] fields defined
 *	below in sio_mem.
 *
 *	They are partitioned into command and status bits.
 */
# define M_DTR	0x02	/* Data Terminal Ready	- command	*/
# define M_BRK	0x08	/* Break		- command	*/
# define M_RTS	0x20	/* Ready To Send	- command	*/
# define M_DCD	0x40	/* Data Carrier Detect	- status	*/
# define M_DSR	0x80	/* Data Set Ready	- status	*/

/*
 * SIO board DMA parameter layout:
 */
typedef struct sio_dma {
	caddr_t	dma_addr;	/* Start addr (only 1st 3 bytes used)	*/
	u_short	dma_count;
	u_short	dma_resid;
} sio_dma;

/*
 * Overall SIO board memory layout:
 *	This structure is used in pointer form only.
 *	The memory itself resides on (one of) the SIO board(s).
 */
typedef struct sio_mem {
	char	m_prom  [0x800];
	u_char	m_fmode,	/* fifo interrupt mode (don't touch)	*/
		m_done,		/* bit mask of lines with dma complete	*/
		m_done_pend,	/* bit mask of lines completing since	*/
				/* m_done was last set by SIO board	*/
		m_mdm_lines,	/* bit mask of lines with changed modem	*/
				/* status				*/
		m_mdm_pend,	/* bit mask of lines with modem status	*/
				/* changed since m_mdm_lines was last	*/
				/* set by SIO board			*/
		m_fill0 [3],
		m_modem [8];	/* modem state info for each line	*/
	sio_dma	m_dma   [8];	/* a dma parameter area for each line	*/
	sio_cmd	m_cmd   [0x6c];	/* the command area			*/
	char	m_fill1 [0xe00];/* spacing to start of hw regs		*/
	char	m_usart [0x40];
	u_char	m_mask,		/* interrupt mask register		*/
		m_status,	/* interrupt status register		*/
		m_fill2,
		m_go,		/* initiate command by writing here	*/
		m_done_int,	/* force done interrupt			*/
		m_modem_int,	/* force modem interrupt		*/
		m_diag_int;	/* force diagnostic interrupt		*/
	char	m_fill3 [0x3b9];
	short	m_fifo;		/* fifo read location			*/
	char	m_fill4 [0xe3fe];
} sio_mem;

/*
 * Interrupt mask and status register bit layouts:
 */
# define I_DONE		0x01	/* command and/or dma complete; must be	*/
				/* distinguished by checking m_dma_cmd	*/
# define I_MODEM	0x02	/* modem interrupt			*/
# define I_DIAG		0x04	/* interrupt from board diagnostics	*/
# define I_FIFO		0x08	/* char received or 3/4 full		*/
# define I_ERROR	0x10	/* MESA <-> SIO board parity error	*/

/*
 * Fifo bit definitions:
 */
# define FI_CHAR	0x00ff	/* the character	*/
# define FI_PAR		0x0100	/* parity error?	*/
# define FI_OVER	0x0200	/* overrun error?	*/
# define FI_FRAME	0x0400	/* framing error?	*/
# define FI_LINE	0x3800	/* line index		*/
# define FI_VALID	0x8000	/* fifo nonempty	*/

/*
 * Extract the line index from a word obtained from the fifo.
 */
# define sio_getline(c) (((c) >> 11) & 07)

/*
 * Macro to initiate a command to the SIO board.
 *	This macro has become simple enough that its existence isn't really
 *	justified any more, but we retain it for historical reasons.
 */
# define sio_go(smp)	smp->m_go = 1

/*
 * Generalized macro for doing transactions with the SIO board.
 * All transfers must go a byte at a time (except for reading the fifo).
 * We probably could profit by defining more specialized macros
 * to obtain more efficient copying.
 *	void bytecpy (dst, src, len)
 *		<anything>	*dst, *src;
 *		unsigned	len;
 */
# define bytecpy(dst,src,len)	\
	{	register int	l = (len);		\
		register char	*dp = (char *)(dst);	\
		register char	*sp = (char *)(src);	\
		while (l--) *dp++ = *sp++;		\
	}
