/*
 * tcerr.c: version 1.4 of 11/13/83
 * 
 */

# ifdef SCCS
static char *sccsid = "@(#)tcerr.c	1.4 (NSC) 11/13/83";
# endif


/* tcerr - program to explain tape errors */

#include <sys/tcu.h>

main(argc, argv)
	char	**argv;
{
	long	immsts;			/* immediate status */
	long	errsts;			/* controller error status */
	long	drvsts;			/* drive status */

	if (argc != 4) {
		printf("Usage:  tcerr immediate-status error-status drive-status\n");
		exit(1);
	}
	immsts = readnum(argv[1]);
	errsts = readnum(argv[2]);
	drvsts = readnum(argv[3]);
	dumpimm(immsts);
	dumperr(errsts);
	dumpdrv(drvsts);
}




/* Dump immediate status */
dumpimm(data)
	register long	data;
{
	register char	*str;

	switch (STATUS(data)) {
		case TCUSTS_BUSY:
			str = "busy";
			break;
		case TCUSTS_ERROR:
			str = "done with error";
			break;
		case TCUSTS_DONE:
			str = "done without error";
			break;
		case TCUSTS_IDLE:
			str = "idle";
			break;
	}
	printf("Channel status: %d (%s)\n", STATUS(data), str);
	bitout("Sequence error", data & TCUIS_SEQERR);
	bitout("Drive status available", data & TCUIS_STSAVAIL);
	bitout("Controller check", data & TCUIS_CTRLCHK);
	bitout("Mesa interface check", data & TCUIS_MIFCHK);
	bitout("Non-maskable interrupt", data & TCUIS_ERRINT);
	bitout("Executing self-test", data & TCUIS_SELFTEST);
}


/* Dump controller error status */
dumperr(data)
	register long	data;
{
	register char	*str;

	switch (CTLFAULTCODE(data)) {
		case TCUCF_NOERROR:
			str = "No error";
			break;
		case TCUCF_NOCMMD:
			str = "Command not found";
			break;
		case TCUCF_SEQERR:
			str = "Improper command sequence";
			break;
		case TCUCF_PGIOCB:
			str = "Page violation in iocb";
			break;
		case TCUCF_PGTABLE:
			str = "Page violation in buffer table";
			break;
		case TCUCF_PGBLOCK:
			str = "Page violation in sense/dump block";
			break;
		case TCUCF_EXCCMMD:
			str = "Command not allowed in exception";
			break;
		case TCUCF_CTRLFLT:
			str = "Fault in controller";
			break;
		case TCUCF_FIFO:
			str = "Fifo has too many bytes";
			break;
		case TCUCF_MIFERR:
			str = "Data compare during mif read test";
			break;
		case TCUCF_CMDINHIB:
			str = "Tape action is inhibited";
			break;
		case TCUCF_STSHUNG:
			str = "Getsts timeout";
			break;
		case TCUCF_DATAINHIB:
			str = "Tape inhibited in data transfer";
			break;
		case TCUCF_EXCREAD:
			str = "Exception during status read";
			break;
		case TCUCF_CMDPAR:
			str = "Parity error in command to drive";
			break;
		case TCUCF_STSPAR:
			str = "Parity error in status read";
			break;
		case TCUCF_DATAPAR:
			str = "Parity error in data transfer";
			break;
		case TCUCF_CMDTIME:
			str = "Timeout issuing command to drive";
			break;
		case TCUCF_POSTIME:
			str = "Timeout during rewind, retension, or erase";
			break;
		case TCUCF_SRCTIME:
			str = "Timeout during search";
			break;
		case TCUCF_BLKTIME:
			str = "Timeout waiting for next block";
			break;
		case TCUCF_DATATIME:
			str = "Timeout during block transfer";
			break;
		default:
			str = "Unknown error";
			break;
	}
	printf("Controller fault: %d (%s)\n", CTLFAULTCODE(data), str);
	switch (MIFFAULTCODE(data)) {
		case TCUMF_NOERROR:
			str = "No error";
			break;
		case TCUMF_MIFIOCB:
			str = "IOCB parameter parity error";
			break;
		case TCUMF_MIFTABLE:
			str = "Page table entry parity error";
			break;
		case TCUMF_MIFCHAIN:
			str = "Next chain address parity error";
			break;
		case TCUMF_MIFREAD:
			str = "Data read parity error";
			break;
		case TCUMF_MIFDATA:
			str = "Data timeout";
			break;
		case TCUMF_MIFCMMD:
			str = "Command timeout";
			break;
		case TCUMF_CLOSE:
			str = "IOCB close-up timeout";
			break;
		default:
			str = "Unknown error";
			break;
	}
	printf("MIF error code: %d (%s)\n", MIFFAULTCODE(data), str);
}


/* Dump drive status */
dumpdrv(data)
	register long	data;
{
	bitout("File mark detected", data & TCUSB_FILEMARK);
	bitout("Bad block detected", data & TCUSB_BADBLOCK);
	bitout("Unrecoverable data error", data & TCUSB_DATAERROR);
	bitout("End of tape", data & TCUSB_ENDOFTAPE);
	bitout("Write locked tape", data & TCUSB_WRITELOCK);
	bitout("Drive offline", data & TCUSB_OFFLINE);
	bitout("Cartridge not in place", data & TCUSB_NOTAPE);
	bitout("Some of above errors are set", data & TCUSB_ANY0);
	bitout("Drive reset occurred", data & TCUSB_RESET);
	bitout("Unused", data & TCUSB_RES1);
	bitout("Start of tape", data & TCUSB_STARTTAPE);
	bitout("Eight or more retries", data & TCUSB_RETRYS);
	bitout("No data detected", data & TCUSB_NODATA);
	bitout("Illegal drive command", data & TCUSB_ILLCMMD);
	bitout("Some of above errors are set", data & TCUSB_ANY1);
}


/* Read a hex number from a string */
readnum(str)
	register char	*str;
{
	register long	data;
	register char	ch;

	data = 0;
	while (ch = *str++) {
		data *= 16;
		if ((ch >= '0') && (ch <= '9'))
			data += (ch - '0');
		else if ((ch >= 'a') && (ch <= 'f'))
			data += (ch - 'a' + 10);
		else {
			printf("Bad hex number typed\n");
			exit(1);
		}
	}
	return(data);
}



/* print a string depending if data is nonzero */
bitout(name, data)
	char	*name;
{
	if (data) printf("%s\n", name);
}
