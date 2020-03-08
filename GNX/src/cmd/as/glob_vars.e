
! @(#)glob_vars.e	3.31    9/20/83
! @(#)Copyright (C) 1983 by National Semiconductor Corp.

!	global_vars.x

  var
    error_flag		: boolean;
    source_errors	: boolean;
    warn_flag           : boolean;
    pass2		: boolean;
    currentlinkpage	: integer;
    locationcounter	: integer;
    sbuffer		: sourcefile;
    linenumber		: integer;
    symtable		: symptr;
    symroot		: symtreeptr;
    symnames		: names_table;
    d_symnames		: names_table;
    g_symnames		: names_table;
    total_sdi_count	: integer;
    total_loc_dirs	: integer;	(* current loc dir count *)
    first_pc_symbol	: symptr;
    last_pc_symbol	: symptr;
    global_interm_rec	: intermcode;
    g_intermrec         : girec;
    gen_intermrec       : genirec;
    sdi_flag		: boolean;
    equ_flag		: boolean;
    upperlower		: boolean;
    big_list		: boolean;	(* true when -l assembly option*)
    tree_display        : boolean;      (* by sunil, true when -t  *)
    vm_flag             : boolean;      (* true when -v *)
    line_trace          : boolean;
    simple_list		: boolean;	(* true when at least one .LIST *)
    printsymtable	: boolean;
    objsymtable		: boolean;	(* output symbols to object file *)
    ext_to_sbrelative   : boolean;
    BACK_COMPATIBLE     : boolean;
    make_global		: boolean;
    process_cxp		: boolean;
    process_d_cxp	: boolean;
    eof_sbuffer		: boolean;      (* dummy eof for source. gives one *)
    mod_name		: boolean;
    do_list    		: boolean;
    done_list  		: boolean;
					(* extra line at the end of the source*)
    last_symbol,
    freelabel,
    this_symbol,
    above_symbol	: symptr;
    stabfilename	: nameptr;   
    global_type_info    : type_info_ptr;

    err_phase0		: boolean;
    err_phase1		: boolean;
    err_phase15		: boolean;
    err_phase2		: boolean;

  routine global_vars; forward;

