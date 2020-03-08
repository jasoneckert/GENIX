#! /bin/csh -f
set infile = "/vmunix"
set outfile = "/vmsymbols"
if ($1 != '') set infile = $1
if ($2 != '') set outfile = $2
csym -f $infile $outfile _u _proc _nproc _bootime _spt _nspt _inode _ninode _file _nfile _ntext _nswap _swapbittable _userpages _bitmap _rs_tty _nttys _sio _forkstat _sum _rate _total _deficit _avenrun _avenscale _hz _cp_time _dc_xfer _msgbuf
