
! @(#)dirsx.e	3.31    9/20/83
! @(#)Copyright (C) 1983 by National Semiconductor Corp.

!	dirsx.x

  procedure appendtoroot(sptr : symptr); forward;

  procedure addtosymroot(sptr : symptr); forward;

  procedure pop_root_level; forward;

  procedure adjust_proc_labels(chain: symptr; offset:integer; neglen: integer);
			forward;

  procedure scan_storage_directives(var next_proc: procdirs; var chain: symptr);
			forward;

  procedure scan_proc_insts(var next_proc : procdirs); forward;

  procedure process_title(dirindex : integer); forward;

  procedure process_subtitile(dirindex : integer); forward;

  procedure process_eject(dirindex : integer); forward;

  procedure process_nolist(dirindex : integer); forward;

  procedure process_list(dirindex : integer); forward;

  procedure process_width(dirindex : integer); forward;

  routine dirsx; forward;

