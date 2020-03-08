
! @(#)cmpc.e	3.31    9/20/83
! @(#)Copyright (C) 1983 by National Semiconductor Corp.

{ cmpc.x
}
{ Assembly language routine to compare strings:}
routine cmpc (
		    length	: integer;  !! length of strings for compare
		    s1, s2	: integer   !! addresses of the strings
		    ) test	: integer;	!! returns -, 0, or +
		forward;

