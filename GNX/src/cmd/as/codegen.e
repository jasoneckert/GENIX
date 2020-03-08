
! @(#)codegen.e	3.31    9/20/83
! @(#)Copyright (C) 1983 by National Semiconductor Corp.

!	codegen.x
  type byte_block=packed array[1..512]of char;
  var
    byte_buffer: byte_block;
    byte_count : integer;
    pc_byte_count : integer;

  procedure flush_bytes;forward;

  procedure gencode(g_index : irec); forward;

  routine output_sym(i : symptr); forward;

  routine output_sym_ents(i : symptr); forward;

  routine codegen; forward;

