
! @(#)prsymtable.e	3.31    9/20/83
! @(#)Copyright (C) 1983 by National Semiconductor Corp.

!	prsymtable.x

  bytez = 0..255;
  var
    largest_sym_size : integer;
    curr_symname : integer;
    currsymnamed : integer;
    currsymname2 : integer;
  function findsym(sptr:symptr;var name1:name):symptr;forward;

  function processsymbol(curtoken : token) sptr : symptr; forward;

  function process_typename(t_sym:name):nameptr;forward;

  procedure new_name (var nam : nameptr; var str : name;
		    var name_index : integer; 
		    var name_table : names_table); forward;

  function get_namelength(var s_name : name) name_length : bytez; forward;

  procedure search( var sptr : symptr;
		    var name1 : name;
		    var h : boolean;
		    var newsym : symptr); forward;

  procedure assignlineref(s: symptr); forward;

  routine prsymtable; forward;

