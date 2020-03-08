
! @(#)formatsmod.e	3.31    9/20/83
! @(#)Copyright (C) 1983 by National Semiconductor Corp.


  !	formatsmod.x
  
  type
  
  formats = (format0,format1,format2,format3,format4,format5,format6,
    	     format7,format8,format9,format10,format11,format12, format14,
	     byteconst,strgconst,wordconst,doubconst,quadconst,floatconst,
	     longconst,fieldconst,intermlistdir,last_interm);
  f0opcode  = (Bcond,Dummy);
  f1opcodes = (BSR,CXP,RET,RXP,RETT,SAVE,RESTORE,ENTER,EXITo,RETI,NOP,
    	       WAIT, DIA,FLAG,SVC,BPT);
  f2opcodes = (ADDQx,CMPQx,MOVQx,Scondx,ACBx,SPRx,LPRx);
  f3opcodes = (CXPD,BICPSRx,JMP,BISPSRx,ADJSPx,JSR,CASEx);
  f4opcodes = (ADDx,CMPx,BICx,ADDCx,MOVx,ORx,SUBx,ADDRo,ADDRx,ANDx,SUBCx,TBITx,
	       XORx,LXPD);
  f5opcodes = (MOVSz,CMPSz,SKPSz,SETCFG);
  f6opcodes = (ROTx,ASHx,CBITx,LSHx,SBITx,SBITIx,NEGx,NOTx,SUBPx,ABSx,
	       COMx, IBITx,ADDPx,CBITIx);
  f7opcodes = (MOVMx,CMPMx,INSSx,EXTSx,MULx,MEIx,DEIx,
               DIVx, REMx,MODx,VALx,SEDx,SEWx,ZEWx,ZEDx);
  f8opcodes = (EXTs,INSx,CVTP,CHECKx,INDEXx,FFSx,MOVSUx,MOVUSx);
  f9opcodes = (MOVxf,TRUNCfx,ROUNDfx,FLOORfx,LFSR,SFSR,MOVLF,MOVFL);
  f10opcodes= (SFREG,RFREG);
  f11opcodes= (ADDf,MOVf,CMPf,ABSf,SUBf,NEGf,DIVf,FRACf,MULf);
  f12opcodes= (POLYf,DOTf,POLYFL,DOTFL);
  f14opcodes= (LMR,SMR,RDVAL,WRVAL);

  directives= (relocscope,proc,storage,stabs,lexscope,listing);
  relocdirs = (IMPORTd,EXPORTd,IMPORTPd,EXPORTPd,COMMd,ROUTINESd,LOC,
		DSECT,STATIC,ENDSEG,PROGRAMd);
  procdirs  = (PROCd,RETURNS,VARd,BEGINd,ENDPROC,MODULEd);
  stordirs  = (BYTE,WORD,DOUBLE,SBYTE,SWORD,SDOUBLE,FLOATd,LONG,BLKB,BLKW,
		BLKD,BLKQ,BLKF,BLKL,ALIGN,FIELD,EQU);
  stabdirs  = (STABd,STABSd,SOURCEd);
  lexdirs   = (BEGINTYPd,ENDTYPd,BEGINVARd,ENDVARd,BEGINBLKd,ENDBLKd);
  listdirs  = (TITLE,SUBTITLE,EJECT,NOLIST,LIST,WIDTH);
  bwd = (b,w,d,q,cx,lx,cx_d);
  uwfield = (untilmatch,whilematch,nouw);
  fl = (f,l);
  cond= (r,eq,ne,lt,ge,lo,hs,le,gt,ls,hi,cs,cc,fs,fc);
  reglist = (r0,r1,r2,r3,r4,r5,r6,r7);
  freglist = (f0,f1,f2,f3,f4,f5,f6,f7);
  preglist = (sp,sb,intbase,psr,fp,us,upsr,pc,tosk,rn,rpc,modreg);
  mreglist = (bpr0,bpr1,pf0,pf1,sca,bc,ptb0,ptb1,msr,eia);

  pchar = packed array[0..0] of char;

  instentry = record
    instchars: packed array[1..maxinstlen] of char;
    case instformat : formats of 
      format0        : (instf0 : pchar {f0opcode};
		        f0cond : pchar {cond});
      format1        : (instf1 : pchar {f1opcodes});
      format2        : (instf2 : pchar {f2opcodes};
		        f2cond : pchar {cond};
		        f2bwd  : pchar {bwd});
      format3        : (instf3 : pchar {f3opcodes};
		        f3bwd  : pchar {bwd});
      format4        : (instf4 : pchar {f4opcodes};
		        f4bwd  : pchar {bwd});
      format5        : (instf5 : pchar {f5opcodes};
		        f5bwd  : pchar {bwd};
		        f5bck  : pchar {boolean};
		        f5t    : pchar {boolean};
		        f5uw   : pchar {uwfield});
      format6        : (instf6 : pchar {f6opcodes};
		        f6bwd  : pchar {bwd});
      format7        : (instf7 : pchar {f7opcodes};
		        f7bwd  : pchar {bwd});
      format8        : (instf8 : pchar {f8opcodes};
		        f8bwd  : pchar {bwd});
      format9        : (instf9 : pchar {f9opcodes};
		        f9fl   : pchar {fl};
		        f9bwd  : pchar {bwd});
      format10       : (instf10: pchar {f10opcodes});
      format11       : (instf11: pchar {f11opcodes};
		        f11fl  : pchar {fl});
      format12       : (instf12: pchar {f12opcodes};
		        f12fl  : pchar {fl});
      format14       : (instf14: pchar {f14opcodes};
		        f14bwd : pchar {bwd});
    end;

  direntry = record
    dirchars : packed array[1..maxdirlen] of char;
    is_allowed:  pchar; { one bit for each scopekind }
    case dirtype : directives of
      relocscope : ( dirrelo : pchar {relocdirs} );
      proc	 : ( dirproc : pchar {procdirs} );
      storage	 : ( dirstor : pchar {stordirs} );
      stabs      : ( dirstab : pchar {stabdirs} );
      lexscope   : ( dirlex  : pchar {lexdirs} );
      listing    : ( dirlist : pchar {listdirs} );
    end;
  
  optokenlist = (comop,notop,extop,modop,andop,shlop,shrop,orop,xorop,
		 addop,subop,mulop,divop,relop,absop);
  rtokentypes = (regtokens,fregtokens,pregtokens,mregtokens,optokens);
  rtentry = record
    rtchars : packed array[1 .. maxrtlen] of char;
    case rtokentype : rtokentypes of
      regtokens  : (rtreg  : pchar {reglist});
      fregtokens : (rtfreg : pchar {freglist});
      pregtokens : (rtpreg : pchar {preglist});
      mregtokens : (rtmreg : pchar {mreglist});
      optokens   : (rtops  : pchar {optokenlist});
    end;

  optypes = (instruction, directive, reservedtoken);
  opentry = record
    case optype : optypes of
      instruction  : ( opinst : instentry );
      directive    : ( opdir  : direntry  );
      reservedtoken: ( oprt   : rtentry   );
    end;

 opfile = file of char;

 routine formatsmod; forward;

