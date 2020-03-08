
! @(#)scan.e	3.31    9/20/83
! @(#)Copyright (C) 1983 by National Semiconductor Corp.

!	scan.x

  var

    current_char : char;
    charpos : integer;
    max_char_pos:integer;
    lastpos : integer;
    line    : input_text_line;

  procedure skipblanks; forward;
  
  function getlchar lchar : char; forward;

  function getschar schar : char; forward;
  
  procedure processeoln; forward;

  routine read_line; forward;
  
  procedure reporterror(errornum : integer); forward;

  procedure  insert_align_expr(n1s,n1e,n2s,n2e:integer);forward;

  routine scan; forward;

