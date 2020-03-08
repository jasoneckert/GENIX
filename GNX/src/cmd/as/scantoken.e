
! @(#)scantoken.e	3.31    9/20/83
! @(#)Copyright (C) 1983 by National Semiconductor Corp.

!	scantoken.x

  type
    
    rtmnst_def	= packed array[61..122] of integer;

    tokentypes = (symbol,specchar,gentoken,restoken);
    token = packed record
      case tokentype : tokentypes of
	symbol	: (sym : name;
		   symlength : integer);
	specchar: (spch : char);
	gentoken: (genref : gen);
	restoken: (rtokenindex : 1 .. maxtokens);
    end;

  var
    
    currtoken 	: token;
    rtstart	: rtmnst_def;
    global_string : str_ptr;

  procedure nexttoken(var temptoken : token); forward;

  procedure scannumber(var temptoken:token);forward;

  procedure check_for_comma; forward;

  procedure scan_comma; forward;

  routine scantoken; forward;


