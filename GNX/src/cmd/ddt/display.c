
/*
 * display.c: version 1.22 of 8/30/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)display.c	1.22 (NSC) 8/30/83";
# endif

/* display routines, does all fancy output */

#include <ctype.h>
#include <a.out.h>
#include <sys/param.h>
#include <sys/fpu.h>
#include "main.h"
#include "symbol.h"
#include "parse.h"
#include "display.h"

#define IBUFFERSIZE 4 /* the instruction buffer size */

struct itab {
	char *inames;	/* instruction name   */
	char *iproto;	/* instruction format */
} format[] = {
"B/",
"cccc1010",
"Bsr/Ret/Cxp/Rxp/Rett/reti/save/restore/Enter/exit/nop/wait/dia/flag/svc/bpt",
"xxxx0010",
"s/",
"22222cccc01111bb",
"spr/lpr",
"22222PPPPx1011bb",
"addq/cmpq///Acb/movq",
"22222ssssxxx11bb",
"cxpd//bicpsr//jump//bispsr////adjsp//jsr//case",
"22222xxxx11111bb",
"add/cmp/bic//addc/mov/or//sub/addr/and//subc/tbit/xor",
"mmmmm22222xxxxbb",
"setcfg",
"00000CCGG00010--00001110",
"movs/cmps//skps",
"00000SSSS0xxxxbb00001110",
"rot/ash/cbit/cbiti//lsh/sbit/sbiti/neg/not//subp/abs/com/ibit/addp",
"mmmmm22222xxxxbb01001110",
"Movm/Cmpm/inss/exts/movxbw/movzbw/movz/movx/mul/mei//dei/quo/rem/mod/div",
"mmmmm22222xxxxbb11001110",
"Ext/cvtp/Ins/check",
"mmmmm22222RRR0bbxx101110",
"index/ffs",
"mmmmm22222RRR1bb0x101110",
"movsu/movus",
"mmmmm222220x11bb10101110",
"mov/lfsr/movlf/movfl/round/trunc/sfsr/floor",
"mmmmm22222xxxfbb00111110",
"add/mov/cmp//sub/neg///div////mul/abs",
"mmmmm22222xxxx0f10111110",
"rdval/wrval/lmr/smr",
"22222MMMM0xxxx1100011110",
"\0","\0"
};

char  *fieldtab[] = {
"eq/ne/cs/cc/hi/ls/gt/le/fs/fc/lo/hs/lt/ge/r",
"b/w//d",
"/t/b/tb/w/tw/bw/tbw/////u/tu/bu/tbu",
"/m/c/cm",
"/i/f/fi",
"r0/r1/r2/r3/r4/r5/r6/r7",
"us////////fp/sp/sb/pc//psr/intbase/mod",
"bpr0/bpr1///pf0/pf1///sc//msr/bcnt/ptb0/ptb1//eia",
"0/1/2/3/4/5/6/7/-8/-7/-6/-5/-4/-3/-2/-1",
"l/f",
"\0"
};

char fieldtype[] = "cbSCGRPMs";
char indexm[] = "bwdq";

int	acontext = 0;	/* last size(b,w,d) of a displayed object */
int	firsthex, opcxpd;
int	context,instr;
int	istart,ilen;
char	*ibuf = 0;



nprint(num,radix)
unsigned int num,radix;
{
	unsigned int i,unum;
	char cout;
	unum = num;
	if ((i = num/radix) != 0) nprint(i,radix);
	num %= radix;
	if (num >= 10) {
	    cout = 'a' + (num - 10);
	    if (firsthex) printf("0");
	}
	else cout = num + '0';
	printf("%c",cout);
	firsthex = FALSE;
}

snprint(num)
int	num;
{
	firsthex = TRUE;
	if ((num < 0) && ((tempmodes.outradix == 10) || (num > -100))) {
		printf("-");
		num =  -num;
	}
	nprint(num,tempmodes.outradix);

}

typeout(num,format)
int num;
char *format;
{
	int modf,off,symatch;
	char symfound[MAXSTRLEN];
	lasttypeout = num;		/* save value for tab */
	if ((*format != '*') && (*format != '\0')) printf("%c",*format);
	format++;
	if ((tempmodes.relative) && (num >= matchbase)) {
		symatch = lookbyval(num,&off,&modf,symfound);
		if ((symatch) && (off <= matchequal)) {
			printf("%s",symfound);
			if (off) {
				printf("+");
				snprint(off);
			}
		} else snprint(num);
	} else snprint(num);
	if ((*format != '*') && (*format != '\0')) printf("%c",*format);
	format++;
	if ((*format != '*') && (*format != '\0')) printf("%c",*format);

}

autoi(addrat)
int addrat;
{
register struct exec *hpr = &ddtheader;
	if (tempmodes.outputmode == INSTRUCT) return(TRUE);
	if (tempmodes.outputmode != NUMERIC) return(FALSE);
	if (tempmodes.automat) {
		if ((((hpr->a_text_addr+hpr->a_mod) <= addrat) &&
		(addrat < (hpr->a_text_addr + hpr->a_text))) ||
		((hpr->a_mod_addr <= addrat) && (addrat <= hpr->a_mod_addr+12)))
		    return(TRUE);
	}
	return(FALSE);
}

display(address)
int address;
{
char	achar;
int	contentat, delta, instat, instlen;
	acontext = tempmodes.context;
	if ((address >= 0) && autoi(address)) {
		instat = address;
		if (disasm(instat,&instlen,TRUE)) {
			contentat = getmem(address);
			acontext = instlen;
			return(contentat);
		}
	}
	if (address < 0) {
		contentat = getreg(address);
	} else {
		contentat = getmem(address);
	}
	if (tempmodes.outputmode == ABSNUMERIC) {
		snprint(contentat);
	} else if (tempmodes.outputmode == STRING) {
		delta = 1;
		while (TRUE) {
			achar = getbyte(address+delta-1);
			if (achar == '\0') break;
			if ((achar >= ' ') && (achar <='~')) printf("%c",achar);
			else printf("/%x",achar);
			delta++;
		}
		acontext = delta;
		printf("\r\n");
	} else if (tempmodes.outputmode == CHARACTER) {
		acontext = 1;
		achar = contentat;
		if ((achar >= ' ') && (achar <= '~')) printf("%c",achar);
		else printf("/%x",achar);
	} else if (tempmodes.outputmode == FLOATING) {
		printf("%g",contentat);
	} else typeout(contentat,"  ");
	return(contentat);
}

unsigned ext(x,p,n)
unsigned x,p,n;
{
	return((x >> (p+1-n)) & ~(~0 << n));
}

unsigned ins(x,y,n)
unsigned x,y,n;
{
	return((y << n) | (x & ~(~0 << n)));
}

unsigned getfield(pattern,matchc,value)
char *pattern, matchc;
unsigned value;
{
char *sp;
int firstbit,len;
	sp = (char *) index(pattern,matchc);
	if (sp == 0) return(-1);
	for (firstbit=len=0; *sp != '\0'; firstbit++)
		if (*sp++ == matchc) len++;
	return(ext(value,firstbit-1,len));
}

getname(value,names,name)
int value;
char *names,*name;
{
int i;
	*name = '\0';
	if (value < 0) return;
	for (i=0; i < value; i++) {
		while (*names != '/') {
			if (*names == '\0') return;
			names++;
		}
		names++;
	}
	if ((*names == '/') || (*names == '\0')) return;
	do {
		*name++ = *names++;
	} while ((*names != '/') && (*names != '\0'));
	*name = '\0';
}

unsigned instget()
{
	if (ibuf == 0) ibuf = (char *) malloc(IBUFFERSIZE);
	if ((ilen < 0) || (ilen >= IBUFFERSIZE)) {
		if (ilen < 0) ilen = 0;
		getmemb(istart+ilen,IBUFFERSIZE,ibuf);
		istart += ilen;
		ilen = 0;
	}
	return((0xff & (*(ibuf + ilen++))));
}

disp()
{
int i,r;
	r = instget();
	if ((r & 0x80) == 0) {
	  if (r & 0x40) {
		r = r | 0xffffff80;
	  } else r = r & 0x7f;
	} else if ((r & 0xc0) == 0x80) {
		r = instget() | (r << 8);
	        if (r & 0x2000) {
		    r = r | 0xffffc000;
	        } else r = r & 0x3fff;
	} else {
		for (i=0; i < 3; i++) {
		    r <<= 8;
		    r |= instget();
		}
	        if (r & 0x20000000) {
		    r = r | 0xc0000000;
	        } else r = r & 0x3fffffff;
	}
	return(r);
}

reglist(value,f,i,l)
{
int	n = 0,t = FALSE;
	printf("[");
	while (f != l) {
		if (ext(value,f,1)) {
			if (t) printf(","); else t = TRUE;
			printf("r%d",n);
		}
		n++;
		f += i;
	}
	printf("]");
	return(TRUE);
}

findmod(pc)
{
	struct exec *hpr = &ddtheader;
	int modf,off,symatch;
	char symfound[MAXSTRLEN];
	int i;
	if (trustsyms)
	    symatch = lookbyval(pc,&off,&modf,symfound);
	else symatch = FALSE;
	if (symatch) {
		return(modf);
	} else if (pc < hpr->a_mod) return(0);
	else {
		i = hpr->a_mod_addr + (hpr->a_mod);
		do {
			i -= 16;
			if (getdouble(i+8) <= pc)
				return(i);
		} while (i >= hpr->a_mod_addr);
	}
	return(0);
	
}

etype(d1,d2)
{
    printf("ext<");
    typeout(getdouble(getdouble(findmod(istart)+4)+(d1*4))+d2,"*>");
}

newsbtype(d1)
{
    if (opcxpd) {
	    d1 = getdouble(getdouble(findmod(istart))+d1);
	    typeout(getdouble((d1&0xffff)+8) + ext(d1,31,16),"**");
    } else {
	    typeout(getdouble(getdouble(findmod(istart))+d1),"*<");
	    printf("xsb>");
    }
}

mtype(d1,regn,regname)
char *regname;
{
	if (regn <= 1) {
		snprint(d1);
		printf("(%s)",regname);
		return;
	}
	if (regn == 3) typeout(istart+d1,"*<");
	else
	typeout(getdouble(findmod(istart))+d1,"*<");
	printf("%s>",regname);
}

mmtype(d1,d2,regn,regname)
char *regname;
{
	snprint(d2);
	printf("(");
	mtype(d1,regn,regname);
	printf(")");
}

newsbmtype(d1,d2,regn,regname)
char *regname;
{
	if (d2 == 0) {
		newsbtype(d1);
	} else {
		snprint(d2);
		printf("(");
		newsbtype(d1);
		printf(")");
	}
}

domode(mode,ibyte,printit)
{
	int i,indexmode = 0,d1,d2;
	char fieldname[9];
	if (mode >= 0x1c) {
	    indexmode = mode;
	    mode = ibyte >> 3;
	}
	getname((8+(mode & 3)),fieldtab[6],fieldname);
	switch (mode) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
		if (printit) printf("r%d",mode & 7);
		break;
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
		d1 = disp();
		if (printit) {
		    snprint(d1);
		    printf("(r%d)",mode & 7);
		}
		break;
	case 16:
	case 17:
	case 18:
		d1 = disp();
		d2 = disp();
		if (printit) {
			if ((mode == 0x12) && (d1 < 0)) {
		    		newsbmtype(d1,d2,mode&3,fieldname);
			} else {
		    		mmtype(d1,d2,mode&3,fieldname);
			}
		}
		break;
	case 20:
		d1 = 0;
		for (i=1; i <= context; i++) d1 = instget() | (d1 << 8);
		switch (context) {
		case 1:
			if (d1 & 0x80) d1 |= 0xffffff00;
			break;
		case 2:
			if (d1 & 0x8000) d1 |= 0xffff0000;
			break;
		}
		if (printit) typeout(d1,"**");
		break;
	case 21:
		d1 = disp();
		if (printit) {
		    printf("@");
		    typeout(d1,"**");
		}
		break;
	case 22:
		d1 = disp();
		d2 = disp();
		if (printit) etype(d1,d2);
		break;
	case 23:
		if (printit) printf("tos");
		break;
	default:
		d1 = disp();
		if (printit) {
			if ((mode == 0x1a) && (d1 < 0)) {
				newsbtype(d1);
			} else {
				mtype(d1,mode&3,fieldname);
			}
		}
		break;
	}
	if ((indexmode) && (printit))
		printf("[r%d:%c]",ibyte&7,indexm[indexmode&3]);
}

disasm(instat,instlen,printit)
int instat,*instlen,printit;
{
char *ip,*in,*fp;
char opname[9],firstchar;
int f,f2,ib1,ib2,i,j,opcode,len,displace,operand = FALSE,bytes = 0;
unsigned instr, mask;
	istart = instat;
	ilen = -1;
	for (i = 0; *(format[i].iproto) != '\0'; i++) {
		ip = format[i].iproto;
		in = format[i].inames;
		len = strlen(ip);
		while ((len / 8) > bytes) {
			instr = ins(instr,instget(),bytes << 3);
			bytes++;
		}
		mask = 1 << (len - 1);
		for (j = 0; j < len; j++) {
			if (instr & mask) { if (*(ip+j) == '0') goto trynext; }
		 	else if (*(ip+j) == '1') goto trynext;
			mask >>= 1;
		}
		context = getfield(ip,'b',instr) + 1;
		if (context == 3) {
			if (i == 14) {
				opcode = getfield(ip,'x',instr);
				if (opcode == 2) context = 4;
				else goto trynext;
			} else goto trynext;
		} else if (context == 0) context = 4;
		opcode = getfield(ip,'x',instr);
		if (opcode == -1) opcode = 0;
		getname(opcode,in,opname);
		if (opname[0] == '\0') goto trynext;
		if (displace = isupper(opname[0])) opname[0] |= 0x20;
	 	if (printit) printf("%s",opname);	
		firstchar = opname[0];
		if (printit) {
			if (i == 14) {
			    if ((opcode != 6) && (opcode >= 4))
				printf("%c",getfield(ip,'f',instr) ? 'f' : 'l');
				
			} else if (i == 15)
				printf("%c",getfield(ip,'f',instr) ? 'f' : 'l');
			fp = fieldtype;
			for (j=0; *fp != '\0'; j++) {
			    if (*fp == 'M') if (opcode <= 1) {
				fp++;
			    	continue;
			    }
			    f = getfield(ip,*fp++,instr);
			    if (f >= 0) {
				if (j == 1) {
				    if (i == 5)
					if ((opcode == 0) || (opcode == 4) ||
					    (opcode == 12)) continue;
				    if ((i == 6) && (opcode == 9)) continue;
				    if (i == 10)
				    if ((opcode==4)||(opcode==5)) continue;
				    if (i == 14)
				    if ((opcode==1)||(opcode==6)||
					(opcode==2)||(opcode==3)) continue;
				}
				if ((opcode==1) && (j==5) && (i==12)) continue;
				getname(f,fieldtab[j],opname);
				if (j > 2) {
					if (operand) printf(",");
					else operand = TRUE;
				}
				printf("%s",opname);
				if (i == 10)
				    if ((opcode==6)||(opcode==7)) printf("d");
				if ((i == 14) && (opcode==0))
				printf("%c",getfield(ip,'f',instr) ? 'f' : 'l');
			    }

			    if (j == 2) printf("\t");
		    }
		}
		if (i == 1) {
			if ((opcode ==6) || (opcode ==8)) {
				f = instget();
				if (printit) operand = reglist(f,0,1,8);
			} else if ((opcode ==7) || (opcode ==9)) {
				f = instget();
				if (printit) operand = reglist(f,7,-1,-1);
			}
		}
		if (i == 9)
			if ((opcode ==0) || (opcode == 1) || (opcode ==5))
				context = 1;
		if ((i == 5) && (opcode == 0)) opcxpd = TRUE;
		else opcxpd = FALSE;
		f = getfield(ip,'m',instr);
		if (f >= 0x1c) ib1 = instget();
		f2 = getfield(ip,'2',instr);
		if (f2 >= 0x1c) ib2 = instget();
		if (f >= 0) {
		    if ((operand) && (printit)) printf(",");
		    else operand = TRUE;
		    domode(f,ib1,printit);
		}
		if (f2 >= 0) {
		    if ((operand) && (printit)) printf(",");
		    else operand = TRUE;
		    domode(f2,ib2,printit);
		}
		if (displace == TRUE) {
		    if (operand && printit) printf(",");
		    f = disp();
		    if (printit) {
			if (firstchar <= 'b') typeout(instat+f,"**");
			else if (firstchar == 'c') {
			    ib1 = getdouble(getdouble(findmod(instat)+4)+(f*4));
			    typeout(getdouble((ib1&0xffff)+8) +
				    ext(ib1,31,16),"**");
			}
			else snprint(f);
		    }
			
		}
		if (i == 10) 
			if ((opcode == 2) || (opcode == 3)) {
			    f = instget();
			    if (printit) 
			    printf(",%d,%d",(f >> 5),((f & 0x1f)+1));
			}
		*instlen = ilen + (istart - instat);
		return(TRUE);
		trynext: ;
	}
	return(FALSE);
}

ibackup(addrat)
{
	int newaddr,modf,off,symatch;
	int instlen = 0;
	char symfound[MAXSTRLEN];
	if (autoi(addrat)) {
		newaddr = addrat - 1;
		symatch = lookbyval(newaddr,&off,&modf,symfound);
		if (symatch) {
		    newaddr -= off;
		    while ((newaddr + instlen) < addrat) {
			newaddr += instlen;
			if (disasm(newaddr,&instlen,FALSE)) {
			} else return(addrat);
		    }
		    return(newaddr);
		}
		

	} 
	return(addrat);
}

char *regstr(regnum)
int regnum;
{
char *preg = '\0';
	if (regnum >= 0) {
		printf("can't print this register- bug\r\n");
	} else {
		if (regnum > SPSR) {
			preg = reg2tab[0 - (regnum - SR0)].regname;
		} else if (regnum > SBPR0) {
			preg = reg3tab[0 - (regnum - SPSR)].regname;
		} else if (regnum >= SINTB) {
			preg = reg4tab[0 - (regnum - SBPR0)].regname;
		} else if (regnum == FSR) {
			preg = reg3tab[7].regname;
		} else if ((regnum >= F7) && (regnum <= F0)) {
			preg = reg2tab[11 + (F0 - regnum)].regname;
		} else {
		    printf("can't print this register- bug\r\n");
		}
	}
	return(preg);
}

tracestack(fpat,pcfrom,modin,doargs)
int fpat,pcfrom,modin;
{
	int pcreturn,fpnext,modf,off,symatch;
	int instreturn,args,arg,i = 0;
	char symfound[MAXSTRLEN];
	symatch = lookbyval(pcfrom,&off,&modf,symfound);
	if ((symatch == FALSE) || (modf != modin)) return;
	printf("%s",symfound);
	if (off) {
		printf("+");
		snprint(off);
	}
	printf("(");
	pcreturn = getdouble(fpat+4);
	if (doargs) {
		instreturn = getdouble(pcreturn);
		if ((instreturn & 0xffff) == 0xa57c) {
			args = 0 - ((instreturn >> 16) | 0xffffff00);
			while (args > 0) {
				arg = getdouble(fpat+12+i);
				typeout(arg,"***");
				i += 4;
				args -= 4;
				if (args > 0) printf(",");

			}
		}
	}
	printf(")\r\n");
	fpnext = getdouble(fpat);
	if (fpcheck(fpnext))
		tracestack(fpnext,pcreturn,(getdouble(fpat+8) & 0xffff),doargs);

}

skipinst()
{
int instlen, thepc;
	thepc = getreg(SPC);
	if (disasm(thepc,&instlen,FALSE)) {
		setreg(SPC,(thepc + instlen));
	} else {
		printf("\r\ncan not skip this instruction\r\n");
	}
	showspot();
}
