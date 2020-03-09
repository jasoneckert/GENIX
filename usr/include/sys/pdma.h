/*
 * @(#)pdma.h	3.2	7/14/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */


struct pdma
{
	caddr_t p_addr;
	char	*p_mem;
	char	*p_end;
	int	p_arg;
	int	(*p_fcn)();
};
