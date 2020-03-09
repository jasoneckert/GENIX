/*
 * @(#)varargs.h	1.2	7/13/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

typedef char *va_list;
# define va_dcl int va_alist;
# define va_start(list) list = (char *) &va_alist
# define va_end(list)
# define va_arg(list,mode) ((mode *)(list += sizeof(mode)))[-1]
