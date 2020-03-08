/* dcerr - program to explain disk errors */

# ifdef SCCS
static char *sccsid = "@(#)dcerr.c	2.4	(NSC) 1/28/83";
# endif

#include <sys/dcusaio.h>

main(argc, argv)
	char	**argv;
{
	register long	data;
	register char	*arg;
	register char	ch;
	int	more;

	if (argc < 2) {
		printf("Usage:  dcerr sense-data\n");
		exit(1);
	}
	argv++;
	argc--;
	more = 0;
	while (argc > 0) {
		arg = *argv;
		data = 0;
		while (ch = *arg) {
			data *= 16;
			if ((ch >= '0') && (ch <= '9'))
				data += (ch - '0');
			else if ((ch >= 'a') && (ch <= 'f'))
				data += (ch - 'a' + 10);
			else {
				printf("Bad hex number typed\n");
				exit(1);
			}
			arg++;
		}
		if (more) printf("\n");
		dumpsense(data);
		argv++;
		argc--;
		more = 1;
	}
}



/* Subroutine to dump out the sense bytes */
dumpsense(data)
	register long	data;
{
	register long	code;

	bitout("Sequence error", data & DCUSB_SEQERR);
	code = BUSFAULTCODE(data);
	if (code != DCUBF_NOERROR) {
		printf("Bus fault code: %D (", code);
 		switch (code) {
		case DCUBF_DRIVETIME: 	printf("Drive acknowledge timeout");
					break;
		case DCUBF_MESAERROR: 	printf("Mesa interface error");
					break;
		case DCUBF_PARITY: 	printf("CBUS input parity error");
					break;
		case DCUBF_INDEXTIME: 	printf("Index/sector pulse timeout");
					break;
		case DCUBF_WORDTIME: 	printf("Word flag timeout");
					break;
		case DCUBF_DMATIME: 	printf("DMA transfer timeout");
					break;
		case DCUBF_CBUSBUSY: 	printf("CBUS busy");
					break;
		}
		printf(")\n");
	}
	code = CTLFAULTCODE(data);
	if (code != DCUCF_NOERROR) {
		printf("Control fault code: %D (", code);
		switch (code) {
		case DCUCF_OCCUPIED: 	printf("Drive occupied");
					break;
		case DCUCF_NOSECTOR: 	printf("Sector not found");
					break;
		case DCUCF_ECCHEADER: 	printf("ECC error in header field");
					break;
		case DCUCF_ECCDATA: 	printf("ECC error in data field");
					break;
		case DCUCF_ILLPARAM: 	printf("Illegal DCU parameter");
					break;
		case DCUCF_ILLCMMD: 	printf("Illegal DCU command");
					break;
		case DCUCF_REJECT: 	printf("Rejected DCU command");
					break;
		case DCUCF_WRITELOCK: 	printf("Write protection error");
					break;
		case DCUCF_MEMERROR: 	printf("Memory read error");
					break;
		case DCUCF_SYNCERROR: 	printf("Sync error in data field");
					break;
		case DCUCF_DRIVEERR: 	printf("Drive fault");
					break;
		case DCUCF_NODRIVES:	printf("No available drive");
					break;
		case DCUCF_DRIVEADDR:	printf("Incorrect drive address");
					break;
		default:		printf("Undefined error code");
					break;
		}
		printf(")\n");
	}
	bitout("Device not ready", data & DCUSB_NOTREADY);
	bitout("Control bus error", data & DCUSB_BUSERROR);
	bitout("Illegal command", data & DCUSB_ILLCMMD);
	bitout("Illegal parameter", data & DCUSB_ILLPARAM);
	bitout("Sense 2 nonzero", data & DCUSB_SENSE2);
	bitout("Sense 3 nonzero", data & DCUSB_SENSE3);
	bitout("Busy executing", data & DCUSB_BUSY);
	bitout("Normal completion", data & DCUSB_NORMAL);
	bitout("Seek error", data & DCUSB_SEEKERR);
	bitout("Read/write fault", data & DCUSB_RWFAULT);
	bitout("Power fault", data & DCUSB_POWER);
	bitout("Rd/wrt violation",data & DCUSB_VIOLATION);
	bitout("Speed error", data & DCUSB_SPEEDERR);
	bitout("Command reject", data & DCUSB_REJECT);
	bitout("Other errors", data & DCUSB_OTHERERR);
	bitout("Vendor errors", data & DCUSB_VENDORERR);
	bitout("Initial state", data & DCUSB_INITSTATE);
	bitout("Ready transition", data & DCUSB_RDYTRANS);
	bitout("Reserved to this port", data & DCUSB_THISPORT);
	bitout("Forced release", data & DCUSB_RELEASED);
	bitout("Reserved to alternate port", data & DCUSB_ALTPORT);
	bitout("Attribute table modified", data & DCUSB_ATTMOD);
	bitout("In write protected area", data & DCUSB_WRITELOCK);
	bitout("Vendor unique attentions", data & DCUSB_VENDATTN);
}


/* print a string depending if data is nonzero */
bitout(name, data)
	char	*name;
{
	if (data) printf("%s\n", name);
}
