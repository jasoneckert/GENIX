/*
 * vinfo.h
 * Defintion for VIOS device information structure
 *	dgw 2/21/85
 */


/* Info structure for getting device information from vios */
struct info {
	int i[4];	/* for ints for return parameters */
	char str[4];	/* chars to hold device name string */
};

