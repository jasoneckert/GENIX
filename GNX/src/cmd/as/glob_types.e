
! @(#)glob_types.e	3.31    9/20/83
! @(#)Copyright (C) 1983 by National Semiconductor Corp.

!	global_types.x

  type

    sourcefile = text;
    objectfile = text;   
    (*intermfile = file of char;*)

    input_text_line=packed array[1..128]of char;

    regtype = (gregs,fregs);
    regset = set of reglist;

    setcfgoperands = (cdsp,mmu,fpu,ic);
    cfmi_set = set of setcfgoperands;
  
    attribute = record
      attrib      : pchar {bwd};
      attribstate : pchar {boolean};
    end; 

    str_ptr = ^string_rec;
    string_rec = record
      strglen  : integer;
      strchars : packed array[1..129] of char;
    end;

    scaled = record
      case scalestate : boolean of
        true  : (sreg : pchar {reglist};
                 satt : pchar {bwd});
        false : ();
      end;

    numtypes = (inti,floati);
    float_format = (short_float,long_float);

    stackptr = ^stackrecord;

    doub_wrd=integer;
    quad_wrd=packed record
                d_lo:doub_wrd;d_hi:doub_wrd
             end;

    disp_types = (const_value,string_value);
    displacement = record
      case disp_type : disp_types of
        const_value  :  ( varying    : boolean;
    			  disp_expr  : stackptr;
                          case disp_const_type:numtypes of
                           inti  :(disp_value :integer);
                           floati:(disp_float_format:float_format;
                                   short_float_value:integer;
                                   long_float_value :quad_wrd)
                         );

        string_value :  ( str_rec	   : str_ptr);
    end;
   
    (* types for floating point *)
    t_string=array[1..max_float_length] of char;
    tp_string=^t_string;

    gentypes = (register,regrelative,memoryrelative,tos,externalg,absolute,
	        immediate,unresolved,stringg,mod_def);
    genptr = ^gen;
    gen = record
      start_of_stack : stackptr;
      gen_scale : scaled;
      sdi_gnum:integer;
      case gtype : gentypes of
        register       : ( case rtype : regtype of
		            gregs : (reg : pchar {reglist});
		            fregs : (freg: pchar {freglist}));
        regrelative    : (regreloffset : integer;
		          regreloffatt : attribute;
                          case rltype : preglist of
		            rn          : (rnum : pchar {reglist});
		            fp,sp,sb,pc,rpc : ()); 	(* rpc relocatable pc *)
        memoryrelative : (memoffset2 : integer;
			  memoffset1 : integer;
			  memoffatt2 : attribute;
			  memoffatt1 : attribute;
			  memreg : pchar {preglist});
        externalg      :  (extoffset : integer;
			   extindex  : integer;
			   extoffatt : attribute;
			   extinxatt : attribute;
			   ext_proc  : pchar {short}); (* 0=data reference
							  1=global proc ref
							  2=common (data)
							  3=local proc ref *)
        absolute       :  (absnum	   : integer;
			   absatt	   : attribute);
        immediate      :  ( immatt   : attribute
			    case numtype : numtypes of
			      inti    : (imint    : integer);
			      floati  : (imfloatsh:integer;imfloat:quad_wrd));
        unresolved     :  (unratt : attribute);
        stringg	       :  ( string_rec	  : str_ptr);
      end;

    type_info_ptr = ^ type_info_rec;
    symptr = ^symentry; 
    scopekinds=(progscope,codescope,typescope,varscope,procscope,returnscope,
	        procvarscope,beginscope);
    symtreeptr = ^symtreeentry; 
    symtreeentry =  record 
      leftlink  : symtreeptr;
      rightlink : symtreeptr;
      papalink  : symtreeptr;
      sonlink   : symtreeptr;
      syment    : symptr;
      startloc  : integer;
      endloc    : integer;
      unique2   : integer;
      headersym : symptr;
      topofscope: gen;
      kindofscope:scopekinds;
    end;

    scopedef = record
      parent : symtreeptr;
      domain : integer {see N_ty in stab.x};
    end;

    symrefptr = ^symrefhis;
    symrefhis = record
      lineref    : array[1..10] of integer;
      linerefinx : 0..10;
      nextrefrec : symrefptr
    end;

    termptr = stackptr;
    expptr  = stackptr;
    termtypes =(factor,subexp);
    factorptr = ^factorrec;
    factortypes = (fsym,fgen,fres,fnill);
    factorrec = record
	sdi_fnum:integer;
      case factortype : factortypes of
        fsym : (fsymbol : symptr);
        fgen : (fgenref : gen);
        fres : (fresindex : 1 .. maxtokens);
        fnill: ();
      end;
  
    expression = record
      op 	       : optokenlist;
      opd1,opd2  : termptr;
      end;
  
    trm = record
      unop : optokenlist;
      termlength : attribute;
      case termtype : termtypes of
        factor : (fact : factorrec);
        subexp : (sub  : expptr);
      end;
       
    stackrectypes = (exprec,termrec);
    stackrecord = record
      next_stack_rec : stackptr;
      case stackrectype : stackrectypes of
        exprec : (erec : expression);
        termrec: (trec : trm);
      end;

    nameptr = ^name;
    name = packed array[1..33] of char; 
    stab_rec_ptr=^stab_rec;
    stab_rec=record
      next_stab_rec:stab_rec_ptr;
      case whichstab:stabdirs of	
  	STABSd:( symbol_ptr:symptr);
        STABd :(type_info_doub:integer;
		stabnum1,stabnum2:displacement;
		is_1_present:boolean);
       SOURCEd:(sourceitem:integer;
		ispcrel   :boolean;
		islineorchar:boolean;	(* true for SLINE, false for SCHAR *)
		whichfile:nameptr);
       end;

    symentry = record
      symbolname  	: nameptr;
      linedefined 	: integer;
      leftptr     	: symptr;
      rightptr    	: symptr;
      nextsym	  	: symptr;	(* used in SDI updates and .PROC stuff*)
      sdi_count	  	: integer;
      locdir_number  	: integer;	(* current loc dir count *)
      references  	: symrefptr;
      balance	  	: integer;
      symbol_definition : gen;
      symdefined  	: pchar {boolean};
      to_be_externed	: pchar {boolean};
      scope       	: scopedef;
      unique1     	: integer;
      type_of_symbol	: type_info_ptr;
      !first_of_chain	: boolean;
      !chain_to_copy	: symptr;
      scopeptr    	: symtreeptr;
      first_stab  	: stab_rec_ptr;
      equ_sdi		: pchar {boolean};
      proc_name		: pchar {boolean};
     end;

    namechars = ^packed array [1..1024] of char;
    names_block = record
      base_addr   : namechars;
      last_used   : integer;
     end;
     names_table = array[1..maxblocks] of names_block;

    field_rec_ptr = ^field_rec;
    field_rec=record
       		next_field_rec:field_rec_ptr;
       		field_value:displacement
		field_size:integer;
	      end;

    irec = ^interm_loc_rec;
    intermcode = record
      linenumber : integer;
      irec_ptr : irec;
      case ftype : formats of
        format0  : (bcond : pchar {cond};
  		    bmode : pchar {modetypes};
		    blabel : gen);
  
        format1  : ( f1op : pchar {f1opcodes};
		     f1operand  : gen;
	             f1regs     : pchar {regset});

        format2  : ( f2operand  : gen;
		     f2short    : gen;
		     f2x	: pchar {bwd};
		     case f2op : f2opcodes of
		       ADDQx,CMPQx,MOVQx  : ();
		       Scondx	: (q1cond  : pchar {cond});
		       ACBx	: (f2label : genptr);
		       SPRx,LPRx  : (preg    : pchar {preglist}));

        format3  : ( f3operand  : gen;
		     false_cxpd : pchar; {boolean}
		     f3op       : pchar {f3opcodes};
		     f3x	: pchar {bwd});
  
        format4  : ( f4operand1 : gen;
		     f4operand2 : gen;
		     f4op	: pchar {f4opcodes};
		     f4x	: pchar {bwd});

        format5  : ( 
		     f5bck      : pchar {boolean};
		     f5t        : pchar {boolean};
		     f5uw       : pchar {uwfield};
		     f5bwd      : pchar {bwd};
		     case fop5  : f5opcodes of
		       MOVSz,CMPSz,SKPSz : ();
		       SETCFG        : (stcfgoperand : pchar {cfmi_set}));
  
        format6  : ( f6operand1 : gen;
		     f6operand2 : gen;
		     f6op	: pchar {f6opcodes};
		     f6x	: pchar {bwd});
  
        format7  : ( f7operand1 : gen;
		     f7operand2 : gen;
		     f7x	: pchar {bwd};
		     case fop7  : f7opcodes of
		       MOVMx,CMPMx  : (f7constant : genptr);
		       INSSx,EXTSx  : (f7constant1: genptr;
				             f7constant2: genptr));

        format8  : ( f8operand1 : gen;
		     f8operand2 : gen;
		     f8reg      : pchar {reglist};
		     f8x	: pchar {bwd};
		     case fop8  : f8opcodes of
		       EXTs,INSx          : ( f8disp : genptr);
		       CHECKx,INDEXx,
		       FFSx,MOVUSx,MOVSUx : ());
  
        format9  : ( f9operand1 : gen;
		     f9operand2 : gen;
		     case fop9  : f9opcodes of
		       MOVxf,TRUNCfx,
		       MOVLF,ROUNDfx : ( f9x   : pchar {bwd};
				         f9f   : pchar {fl}));
  
        format10 : ( fop10       : pchar {f10opcodes};
		     f10operand  : pchar {freglist});
  
        format11 : ( 
		     f11operand1 : gen;
		     f11operand2 : gen;
		     fop11       : pchar {f11opcodes};
		     f11f	 : pchar {fl});
  
        format12 : ( 
		     f12operand1 : gen;
		     f12operand2 : gen;
		     fop12       : pchar {f12opcodes};
		     f12reg      : pchar {freglist};
		     f12f	 : pchar {fl});
  
        format14 : ( 
		     f14operand  : gen;
		     fop14       : pchar {f14opcodes};
		     f14mreg     : pchar {mreglist});
  
        strgconst: ( strcr_count : integer;
		     str_value   : str_ptr); 
  
        byteconst: ( bcr_count   : displacement;
		     sign_b_flag : pchar {boolean};
		     bc_value    : displacement);
  
        wordconst: ( wcr_count   : displacement;
		     sign_w_flag : pchar {boolean};
		     wc_value    : displacement);
  
        doubconst: ( dcr_count   : displacement;
		     sign_d_flag : pchar {boolean};
		     dc_value    : displacement);
  
        quadconst: ( qcr_count   : displacement;
		     qc_value    : displacement);
  
        floatconst:( fcr_count   : displacement;
		     fc_value    : integer);
  
        longconst: ( lcr_count   : displacement;
		     lc_value    : integer);

	fieldconst:( first_field_rec : field_rec_ptr;
		     field_size  : integer);

	intermlistdir:( list_type   : listdirs;
		        nextcharpos : integer);

	last_interm:  ();
  
    end;

    genirec = ^gen_interm;
    gen_interm = record
		gen_rec  : gen;
		gen_next : genirec;
                 end;

    girec = ^pcsb_interm;
    pcsb_interm = record
		interm_rec  : intermcode;
                interm_next : girec;
                  end;

  interm_loc_rec = record
    next_irec : irec;
    location : integer;
    locdir_number : integer;
    unique   :integer;
  end;

  type_info_rec = record
		    t_name_ptr:nameptr;
                    t_doub_word:integer
                  end;
    mnem_def   = packed array[1..maxinsts] of instentry;
    rtmn_def	= packed array[1..maxtokens] of rtentry;
    dirmn_def = packed array[1..maxdirs] of direntry;
  routine global_types; forward;
