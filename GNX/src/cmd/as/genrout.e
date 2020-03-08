
! @(#)genrout.e	3.31    9/20/83
! @(#)Copyright (C) 1983 by National Semiconductor Corp.

!	genrout.x

  var
    last_alloc_sb, last_alloc_pc : integer;
    first_irec, last_irec : irec;
    first_sbirec, last_sbirec : irec;

    first_genirec, last_genirec : genirec;
    first_sbgenirec, last_sbgenirec : genirec;

    first_girec, last_girec : girec;
    first_sbgirec, last_sbgirec : girec;

  procedure increment_locationcounter(temp:integer); forward;
  
  function new_g_index : irec; forward;

  function new_g_gen : genirec; forward;

  function new_g_intermrec : girec; forward;
  
  routine genrout; forward;
