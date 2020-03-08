
! @(#)init1.e	3.31    9/20/83
! @(#)Copyright (C) 1983 by National Semiconductor Corp.

!	init1.x

  type string32=packed array[1..32]of char;

  var

    objfilename : string32;
    sourcename	: name;
    ibuffer1    : cfile {intermfile for code};
    ibuffer3    : cfile {intermfile for sb(data)};
    lstbuff1	: cfile {listing stuff for code section, listing directives
			  and errors};
    lstbuff2	: cfile { listing intermed stuff for data section};
    lstbuff3	: cfile { listing intermed stuff for data section};
    obuffer	: cfile ;
    last_loc    : symptr;
    out_sym_count : integer;

  procedure init2; forward;

  procedure remove_tmps; forward;

  procedure list_t4; forward;

  procedure remove_t5; forward;

  procedure closelistfiles; forward;

  procedure closeonelist; forward;

  procedure output_export_list; forward;

  procedure output_import_list; forward;

  routine output_strings; forward;

  procedure clean_up; forward;

  routine sb_init; forward;

  routine trans_name_ptr(nameptr : integer) value : integer; forward;

  routine init1; forward;
