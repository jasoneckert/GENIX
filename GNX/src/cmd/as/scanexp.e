
! @(#)scanexp.e	3.31    9/20/83
! @(#)Copyright (C) 1983 by National Semiconductor Corp.

!	scanexp.x

  var

    resetesp : boolean;
    stack_start, current_stack_rec : stackptr;
  

  function newexp  nexp : stackptr; forward;

  function newterm ntrm : stackptr; forward;
  
  procedure produce_external_entry (var sptr: symptr); forward;

  procedure construct_temp_symbol(var tempfactor : factorrec); forward;

  procedure check_range(var att : attribute; temp : integer); forward;

  function convert_string(temp_string : str_ptr) : integer; forward;

  function simpleexp : termptr; forward;
 
  procedure fixgen(var g:gen);forward;

  procedure resolve_expression(t : termptr; var result :factorrec); forward;

  procedure resolve_1_expression(t : termptr; var result :factorrec); forward;

  routine scanexp; forward;
