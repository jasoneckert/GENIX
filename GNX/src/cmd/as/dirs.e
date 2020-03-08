
! @(#)dirs.e	3.31    9/20/83
! @(#)Copyright (C) 1983 by National Semiconductor Corp.

  !	dirs.x
  
  type
    

    asm_modes = (standard,inverted,link_inverted);
    modetype  = ( progrel,stbsrel,frrel,stckrel,ab,extg);
    mode_record = record
      old_loc : integer;
      old_link: integer;
      old_mode: modetype;
      old_asm : asm_modes;
    end;

    exportptr = ^exportrec;
    exportrec = record
      exportsptr      : symptr;
      nextexportrec   : exportptr;
    end;
  
    importptr = ^importrec;
    importrec = record
      importsptr      : symptr;
      comm_size       : integer;
      nextimportrec   : importptr;
    end;

  var 

    assembly_mode : asm_modes;
    currentmode   : modetype;
    export_count  : integer;
    import_count  : integer;

    exportchain : exportptr;
    exportstart : exportptr;
    importchain : importptr;
    importstart : importptr;

    modeindex : integer;


  procedure makepublic( s : symptr); forward;

  procedure make_external(var sptr : symptr); forward;

  procedure processdirective(var temptoken : token); forward;

  function getdirindex(var temptoken : token) : integer; forward;

  procedure storage_init (var g_index : irec); forward;

  procedure processrelocdir(dirindex : integer); forward;

  procedure processlistdir(dirindex : integer); forward;

  procedure processstordir(dirindex : integer); forward;

  procedure processprocdir(dirindex: integer; var next_proc: procdirs); forward;

  procedure pop_current_mode; forward;

  routine dirs; forward;
