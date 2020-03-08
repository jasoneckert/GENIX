main (argc, argv) 
int argc; char *argv[]; {
register char *cp;
int acc;
register short n;
short base;
	cp = argv[1];
	if (*cp == '0') {
		cp++;
		if (*cp == 'x' || *cp == 'X') {
			cp++;
			base = 16;
		}
		else base = 8;
	}
	else if (*cp == 'x' || *cp == 'X') {
		cp ++;
		base = 16;
	}
	else base = 10;
	while (n = *cp++) {
		if (n >= 'a') n -= 0x60;
		else if (n >= 'A') n -= 0x40;
		else n -= 0x30;
		acc = acc * base + n; 
	}
	printf ("decimal %d octal %o hex %x\n",acc, acc, acc);
}
