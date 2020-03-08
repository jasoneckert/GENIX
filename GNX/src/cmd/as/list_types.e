
! @(#)list_types.e	3.31    9/20/83
! @(#)Copyright (C) 1983 by National Semiconductor Corp.

! 	list_types.x

  type
    
    relmodes = (coderel, datarel);
    listrectypes = (code_rec, list_rec, err_rec);

    listrec = record
      linenum :	integer;
      case listrectype : listrectypes of
	code_rec: (reltype : relmodes ;
		   location: integer;
		   num_bytes:integer);
	list_rec: (nextpos : integer;
		   dirtype : listdirs);
	err_rec : (errnum  : integer;
		   errpos  : integer);
   end;


