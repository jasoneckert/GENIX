
! @(#)e2bio.e	3.31    9/20/83
! @(#)Copyright (C) 1983 by National Semiconductor Corp.

{
    This bio.x is used for the 16k only. We call all these C procedures
    directly from EPascal.
}
import	errno: integer;

type	cfile    = integer;
type	seekmode = (m_abs, m_rel, m_end);

procedure fopen(ref name: any string;
	        ref mode: any string) : cfile; ccall forward;

procedure fdopen(fd: integer; ref mode: any string) : cfile;
                 ccall forward;

procedure fcreate(ref name: any string; mask: integer) : cfile;
                  ccall forward;

procedure fclose (fd: cfile); ccall forward;

procedure unlink(ref name: any string); ccall forward;

procedure fflush(fd: cfile); ccall forward;

procedure fwrite(var buffer: any universal; nbytes: integer; i: integer;
		  fd: cfile) actual: integer;
                  ccall forward;

procedure fread(var buffer: any universal; nbytes: integer; i: integer;
		  fd: cfile) actual: integer;
                  ccall forward;

procedure ftell(fd: cfile) pos: integer;
                              ccall forward;

procedure fseek(fd: cfile; where: integer;
		 mode: seekmode) pos: integer;
                ccall forward;

!fast procedure fseek$2 (fd: cfile; where: integer) pos: integer;
!begin
!    pos := fseek(fd,where,m_abs);
!end;

procedure mktemp(var name: any string); ccall forward;

{	The following will not work until the size bugs are fixed in e2	}

!procedure fseek$rec(fd: cfile;
		  !ref rec: any universal;
		  !num: integer) pos: integer;
!begin
    !pos := fseek(fd,size(type rec)*num,m_abs);
!end;

!fast procedure fwrite$2( fd: cfile; var buffer: any universal) actual: integer;
!begin
    !actual := fwrite(fd,buffer,size(type buffer));
!end;

!fast procedure fread$2(fd: cfile; var buffer: any universal) actual:integer;
!begin
    !actual := fread(fd,buffer,size(type buffer));
!end;

routine bio;forward;
