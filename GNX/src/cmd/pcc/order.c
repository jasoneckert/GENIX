/*
 * Copyright (C) 1982 by National Semiconductor Corporation
 *
 *	National Semiconductor Corporation
 *	2900 Semiconductor Drive
 *	Santa Clara, California 95051
 *
 *	All Rights Reserved
 *
 * This software is furnished under a license and may be used and copied only
 * in accordance with the terms of such license and with the inclusion of the
 * above copyright notice.  This software or any other copies thereof may not
 * be provided or otherwise made available to any other person.  No title to
 * or ownership of the software is hereby transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by National Semiconductor
 * Corporation.
 *
 * National Semiconductor Corporation assumes no responsibility for the use
 * or reliability of its software on equipment configurations that are not
 * supported by National Semiconductor Corporation.
 */

/*
 * order.c: version 5.1 of 9/8/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)order.c	5.1 (NSC) 9/8/83";
# endif

/* jima 6/17/82: Reversed offstar() handling of PTR|CHAR */
/* jima 7/ 5/82: Revised setasg & setgin for rewrites of = as op=;	*/
/*               Added makeaddr, mightalias, isalias, canasgop, tregcook */
/* jima 7/16/82: Fixed the fix to offstar() to delay index evaluation   */
/* jima 7/17/82: rallo() changed to FORCE info f0 for float/double */
/* jima 7/19/82: OREG bug fixed in isalias. More repair of offstar() */
/* jima 7/29/82: setexpr fix to do asgops Rt,then Lft; sucomp swaps disabled */
/* jima 8/9/82: deleted mycanon (contents moved to optim2) */
/* jima 8/11/82: offstar revisions */
/* JZ  9/21/82: Changed UNSIGNED LONG to UNSIGNED|LONG which is what
		is meant I believe */
/* JZ 9/22/82: Changed sucomp to require extra register for Oregs which might
		be needed for read modify write auto increment or decrement
		operations */
/* JZ 9/30/82: Changed rallo to MUSTDO large structure assignments in r1 + r2
		so that it will be in the right registers for the movs
		instruction. */
/*JZ 10/4/82:  Changed rallo to force stargs left side into r1 also for the
		same reason as above.  Also changed gencall to call offstar
		up to 15 times attempting to make offstar of whatever since
		we were getting stuck starg on passing structures to arrays
		of functions otherwise. These changes are from Jima.*/
/*JZ 10/12/82: Added check for FLD of right side to check for putting right
		side in a register in setasg*/
/*JZ 10/12/82: Deleted line in mkadrs() that was if (0) since that is a noop.*/
/*JZ 10/13/82: Changed rallo to mustdo the left into R0 and  the right into
		R2 for unsigned div and mod with 32 bits 
		so we can use the dei instruction.*/
/*JZ 10/14/82: Changed setexpr to put the left side of UNSIGNED or ULONG in a register also */
/*JZ 10/18/82: Changed sucomp to require freregs (all temporary registers)
		for unsigned mod and div operations */
/*JZ 10/19/82: Changed rallo to MUSTDO all unsigned mod or div into various registers*/
/*JZ 10/19/82: Changed setexpr to rallo the right into R1 if we have a mod or
		div so the registers are correct, dei needs r1*/
/*jima 10/21/82: Undid the MUSTDO hacks; the incorrect table.c entries for
		 non-leafs are removed, preserving bottom-up evaluation. */
/*JZ 10/26/82:   Changed setasg to not allow setasgoping for unsigned div
		 or mod, unless the left is allready in a register.*/
/*JZ 10/27/82: Changed sucomp to increase the number of registers needed
		for unsigned div of mod to be the maximum of the right+1 and
		the number of free register in order to force a store when
		necessary such as across a function call.  It may be allright
		to allow it up to the right instead of the right+1 however.*/
/*JZ 11/1/82:  Changed the register reserved for the left side  of dei
		to r0 from r1 since the low order half of the dei op
		is in the low order register.*/
/*JZ 11/1/82:  Changed the parentetheses in the check to see if we can do
		asg'oping in setasg in order to match with the previous
		version.  In adding a check for unsigned div I had
		accidentally reversed the meaning to allo aliasing which
		is very dangerous*/
/*JZ 11/4/82:   Changed sucomp to do similar things for unsigned div/mod
		for asg ops and for non asg ops.  Also added a check for
		unsigned so it only requires all the registers for unsigned*/
/*JZ 11/16/82:	Changed sucomp so that the left side in an asg div/mod is
		considered in the su number in addition to the right.  */
/*JZ 11/10/82:  Changed sucomp to require 0 registers for STARNM, since
		by definition that can be done with 0 registers, as in
		0(-4(fp)) ordinarily UNARY MUL takes at least 1 register
		however.*/
/*JZ 12/27/82:  Changed genargs to handle structure valued arguments using
		the new method, it now just calls genscall to do it*/
/*JZ 1/3/83:    Changed rallo to not reserve registers for  STASG
		when there is a STCALL or UNARY STCALL.  The called function
		does the actual assignment in this case so the registers are
		unnecessary. */
/*JZ 1/4/83:    Did above change for STARG also,for the same reasons as above*/
/*JZ 1/7/83:    Changed setstr() to recognize the case of a FORCE and pass
		its own pointer to the called function.  This is where we do
		a return fn();  This means that if recursion is done this way
		there will be only 1 copy of the structure from the top to
		the bottom layer of recursion.  This is not done for a STASG
		as we will then want to use the stack. */
/*JZ 1/19/83    Changed setexpr to check for OREG's on the left in binary ops
		and try	to force it into a register if that appears necessary
		from the su numbers*/
/*JZ 1/27/83:   Changed setasg to not allow the optimisation of a = b / or % c
		int a=b; b /or% = c if the types of the = and the / are 
		different. Changed mightalias to catch the case of *(int *)12
		and disallow it.  This has been rewritten to a NAME node
		with the name being @12.  I also rewrote the check in setasg
		to check the left side for this problem as well. */
/*JZ 1/28/83:	Changed setasg to not allow the above optimisations if the
		left side is not a register.  There is a minimum 10% time
		penalty for the extra memory reference that is created.  The
		time penalty is worse if the operands are external.
		When we bring back register char and short variables we
		must check for them in this situation and do a move
		extending into the register at the end.*/
/*JZ 1/31/83:   Changed the ord_assign() to check for UNARY MUL's when it
		does its copying and copy their kids as well.  We were having
		troubles with pointers being screwed up because that was not
		happening.*/
/*JZ 2/9/83:    Changed setexpr to check for a signed /= or %= and change the
		right side to INT or SHORT if it is unsigned something.  This
		will allow the tables to deal correctly with it instead of
		using the kluge code I was using.*/
/* JZ 3/8/83:   Changed setexpr to check for FLD before doing a makeaddr(l);
		makeaddr automatically will force a FLD into a register, since
		that is the only way to make it addressable,  however if it
  		is done then  instead of later we lose the address and only
   		have a register left.  This change causes assignment ops to
		work correctly on bitfields. */
/* JZ 3/23/83:  Changed setexpr to do above only when the op is really an
		asg-op.	If it really isn't an asg-op then there is no need to
		keep the address, since it will not be needed and trying to
		will cause you to use an extra register.*/
/*  JZ 4/8/83:  Changed offstar to not attempt to put the index of a pointer
		index pair under a plus in a register when there is no way of
		making it into an OREG.  This prevents the waste of registers.
		 */
/*  JZ 4/14/83: Changed setstr FORCE to set skip_return_code and to set the
		value of struc_skip_lab to the apropriate value.  This sends
		a signal to cgram.y RETURN to branch to struc_skip_lab and
		to efcode to generate a label to branch to. */
# include "mfile2"

/* Call offstar() if appropriate - used by makeaddr() and setexpr().
   Note: This is called with the possible U*, NOT its descendent, ala offstar.
   1 is returned if offstar was called, 0 if nothing happened.        (jima)*/
calloffstar(p) register NODE *p;  {   
	if(p->in.op == UNARY MUL) {
		offstar( p->in.left );
		return(1);
		}
	if(p->in.op==FLD  &&  p->in.left->in.op==UNARY MUL) {
		offstar( p->in.left->in.left );
		return(1);
		}
	return(0);
	}

/* Make a node addressable.
   If appropriate, offstar() is called to emit some address calculations
   for deeper parts and 1 is returned (a subsequent canon() will form
   an OREG).  Otherwise, order() is called to make the node addressable.
   If this is not possible without assignment to a temp register, a rewrite 
   rule will be invoked (usually setexpr()).  Usually, however, 
   addressability can be achieved without assigning the value to a temp.
   
   A 0 return means nothing happened, and the node is already addressable.
   
   (New 6/30/82 - jima)  */

makeaddr(p) register NODE *p; {
	register p_op = p->in.op;

	if (canaddr(p)) return(0);
	if (!calloffstar(p)) {
		order(p,FORADDR);  /* Hits rewrite (usually setexpr())	*/
			    	   /* if temp register needed.		*/
		}
	return(1);
	}

/* Construct an ASSIGNment and evaluate it, returning the LHS in p (which 
   may be the same as rhs or lhs, or unique).  HOWEVER, if the LHS is
   a temp register, a different temp register may be returned if dictated
   by MUSTDO in lhs->in.rall (see reclaim), which is used for the assignment.
   The other nodes are freed.  Order is called recursively. 
   If any of the nodes are UNARY MUL their offspring are copied in order to
   prevent possible tangles caused by multiple pointers to their kids.
   This may not be necessary but it seems to eliminate some problems that
   were resulting in bus errors.  At worst this is a slight inefficiency at
   best it may get rid of some problems.  Eventually we should get rid of
   STARNM entirely and simply replace it with OREG nodes with zero offsets.
   Used by setasg and zzzcode('W').  (jima+ JZ)*/

ord_assign(lhs,p,rhs) NODE *lhs, *p, *rhs; {
	register NODE *temp;
	register NODE *old_lhs = talloc();

	ncopy(old_lhs,lhs);
	if (lhs->in.op== UNARY MUL)
	{
		old_lhs->in.left= talloc();
		ncopy(old_lhs->in.left,lhs->in.left);
	}

	if(p==lhs)
	{
		temp = talloc();
		ncopy(temp,lhs); lhs = temp;
		if (lhs->in.op == UNARY MUL)
		{
			lhs->in.left= talloc();
			ncopy( lhs->in.left,p->in.left);
			p->in.left->in.op=FREE;
		}
	}
	else
	if(p==rhs) 
	{
		ncopy(temp=talloc(),rhs); rhs = temp;
		if (p->in.op == UNARY MUL)
		{
			rhs->in.left= talloc();
			ncopy( rhs->in.left,p->in.left);
			p->in.left->in.op=FREE;
		}
				
		ncopy(p,lhs); /* set fields, including type */

	}
	else    { /* unique destination node */
		ncopy(p,lhs); /* set fields, including type */
		}

	p->in.op = ASSIGN;
	p->in.left = lhs;
	p->in.right = rhs;

	if (istnode(lhs)) {
		/* "register overwrite" checks in reclaim() prevent keeping */
		/* a copy of a temporary register, so we must get it back   */
		/* as the result; respref[] forces temp regs to always be   */
		/* returned, and ASSIGNs between temp regs never occur, so  */
		/* the lhs is the only possible result. 		    */
		order(p,FORADDR);
		ASSERT(istnode(p), "ord_assign:lost treg");
 	}
	else {
 		order(p, FOREFF);
		ncopy(p,old_lhs); /* return the lhs */
		old_lhs->in.op = FREE;
		return;
	}
	if (old_lhs->in.op== UNARY MUL)
		old_lhs->in.left->in.op == FREE;
	old_lhs->in.op = FREE;
}

	
/* Set aliasing=1 if the node could secretly alias another.  Called     
 *  via walkf to determine if aliasing might occur.  See setasg. (jima) */
int aliasing;

mightalias( p ) NODE *p; {

	if (ISPTR(p->in.type) ||
    	    ISFTN(p->in.type) ||
    	    p->in.op == OREG  ||
	    (p->in.op == NAME && /* An absolute address isn't safe */
	     p->in.name[0] == '\0')||
    	    ( p->in.op==UNARY MUL &&
      	      p->in.left->in.op==ICON)) aliasing=1;
}

/* isalias: set aliasing=1 if the node is a leaf and refers to the same location
 * as aliastarget (which must also be a leaf).  This assumes that references to
 * to the same location always have the same representation.  Note that this
 * is not always true if U* subtrees are considered, but all such are treated
 * with suspicion in setasg().   
 *
 * Implicitly used registers in OREGs are recognized in p, but not in
 * aliastarget.  (jima) */
NODE *aliastarget;
isalias( p ) register NODE *p; {
	register p_op = p->in.op;
	register p_rval = p->tn.rval;
	register target_rval = aliastarget->tn.rval;
	if(  (p_op==aliastarget->in.op && p_rval==target_rval)
            ||
	     (p_op==OREG && aliastarget->in.op==REG &&
	        ( target_rval==p_rval ||
	          (R2TEST(p_rval) &&
                   (target_rval==R2UPK1(p_rval)||target_rval==R2UPK2(p_rval))) 
	        )
	     ) 
	  )  aliasing=1;
}

/* Determine if there is a corresponding ASGop form for op.  These operators */
/* are rewritten to their ASG op form for code generation.	(jima)       */
canasgop( op ) {

	/* Someday maybe a new flag bit should be put into dope[] for this */
	switch (op) {
		case MUL: case DIV: case MOD: 
		case PLUS: case MINUS:
		case LS: case RS: 
		case AND: case OR: case ER:
			return(1);
		default:
			return(0);
		}
	}


int maxargs = { -1 };

stoasg( p, o ) register NODE *p; {
	/* should the assignment op p be stored,
	   given that it lies as the right operand of o
	   (or the left, if o==UNARY MUL) */
	}

deltest( p ) register NODE *p; {
	/* should we delay the INCR or DECR operation p */
	p = p->in.left;
/*
	return( p->in.op == REG || p->in.op == NAME || p->in.op == OREG );
*/
	return(canaddr(p));
	}

autoincr( p ) NODE *p; {
	register NODE *q = p->in.left, *r;

	return 0; /* not bloodly likely */
}

mkadrs(p) register NODE *p; {
	register o;

	o = p->in.op;

	if(asgop(o)) {
		if(p->in.left->in.su >= p->in.right->in.su) {
			if(p->in.left->in.op == UNARY MUL) {
				SETSTO( p->in.left->in.left, INTEMP );
				}
			/*else if( 0 && p->in.left->in.op == FLD && p->in.left->in.left->in.op == UNARY MUL){
				SETSTO( p->in.left->in.left->in.left, INTEMP );
				}removed since it does nothing JZ*/
			else { /* should be only structure assignment */
				SETSTO( p->in.left, INTEMP );
				}
			}
		else SETSTO( p->in.right, INTEMP );
		}
	else {
		if(p->in.left->in.su > p->in.right->in.su) {
			SETSTO( p->in.left, INTEMP );
			}
		else {
			SETSTO( p->in.right, INTEMP );
			}
		}
	}

notoff( t, r, off, cp) CONSZ off; char *cp; {
	/* is it legal to make an OREG or NAME entry which has an
	/* offset of off, (from a register of r), if the
	/* resulting thing had type t */

/*	if( r == R0 ) return( 1 );  /* NO */
	return(0);  /* YES */
	}

# define max(x,y) ((x)<(y)?(y):(x))
# define min(x,y) ((x)>(y)?(y):(x))
/* sucomp is called by canon to do bottom up traversal of the expression tree*/
sucomp(p)
register NODE *p; {

	/* set the su field in the node to the sethi-ullman
	   number, or local equivalent */

	register o, ty, sul, sur, r;

	o = p->in.op;
	ty = optype( o );
	p->in.su = szty( p->in.type );

	if(ty == LTYPE) {
		if(o == OREG) {
			r = p->tn.rval;
			/*
			 * oreg cost is (worst case) 1
			 * + number of temp registers used
			 */
		        /* CHANGED: The extra register should not be needed
			 * if a temp register is used at all, because reg
			 * sharing will occur in any final transfer of
			 * the value to a temp register.*/
			  p->in.su = 0;       /* jima */ 
			if(R2TEST(r)) {
				if(R2UPK1(r)!=100 && istreg(R2UPK1(r)))
					++p->in.su;
				if(istreg(R2UPK2(r)))
					++p->in.su;
			} else {
				if(istreg(r))
					++p->in.su;
			}
		}
		/* ICON test added - reloc. ICONs now only for cxp (jima) */
		/* LONG & UNSIGNED LONG added (jima 8/24/82) */
		if(p->in.su == szty(p->in.type)
		&& (p->in.op!=REG || !istreg(p->tn.rval))
		&& ( p->in.op==ICON  /* relocatable ICONs for cxp (only) */
# ifdef MEMRMWOK
		    || p->in.type==INT   /* includes all absolute ICONs */ 
		    || p->in.type==LONG  /* jima */
		    || p->in.type==ULONG /* jima added | JZ changed to ULONG*/
		    || p->in.type==UNSIGNED
		    || p->in.type==DOUBLE
# endif
		   ))
			p->in.su = 0;
		return;
	} else if(ty == UTYPE) {
		switch( o ) {
		case UNARY CALL:
		case UNARY STCALL:
			p->in.su = fregs;  /* all regs needed */
			return;
	/*If we can make a STARNM we do not require any registers
	  at all,  since we have something of the form 0(-4(fp)).
	  This may be pulled out when OREG is rewritten.
	  Otherwise we may need as many register as there are on
	  the left side since we may have a double OREG or something there*/
		case UNARY MUL:
			if (shumul(p->in.left)==STARNM)
			{
				p->in.su=0;
				return;
			}
			else p->in.su=max(p->in.left->in.su,1);
			return;

		default:
			p->in.su =  p->in.left->in.su
				  + (szty(p->in.type) > 1 ? 2 : 0) ;
			return;
			}
		}


	/* If rhs needs n, lhs needs m, regular su computation */

	sul = p->in.left->in.su;
	sur = p->in.right->in.su;

	if(o == ASSIGN) {
		/* computed by doing right, then left (if not in mem), then doing it */
		/* Assigns work with any mixed types, so do not force 
		   anything into extra temp registers.  The Right thus takes
		   none if if it needs none for mere addressability (jima) */
		p->in.su = max(sur,sul+(min(sul,1)));
/*        was:	p->in.su = max(sur,sul+1); (jima) */
		return;
		}
	/* Unsigned div and mod take all registers */
	/* However they can take more such as when there
	/*  is a function call. This is evaluated left to right so
	    we may need one register to make the left addressable
	    in addition to the registers needed to make the right
	    adressable.  This is so that we do a store for function
	    calls instead of thinking we have enough registers.*/
	if ((o==DIV|| o == MOD)&&ISUNSIGNED(p->in.type)){
		p->in.su = max(fregs,sur+1);
		return;
		}

	if(o == CALL || o == STCALL) {
		/* in effect, takes all free registers */
		p->in.su = fregs;
		return;
		}

	if(o == STASG) {
		/* right, then left */
		p->in.su = max( max( 1+sul, sur), fregs );
		return;
		}

 	if(asgop(o)) {
		/* computed by doing Rt, Lft address, Lft, op= */
		if ((p->in.op==ASG DIV||p->in.op==ASG MOD)
		    &&(ISUNSIGNED(p->in.type)))
		{ 
		 /* unsigned operations use all registers */
		 /* Since the op requires all the registers,
		    if the left requires registers for its address,
		    the address must be stored, since the operation
		    itself must have all 3 registers clear. Since the
		    right side may be an expression that might end up in
		    any register we want to force the right side into
		    1 register as well.  For instance the right side
		    might be another unsigned divide.  This allows
		    the right side to have r2 free to put it into.
		    Which it will do since rallo will mustdo the
		    right side int r2.  The su numbers are changed
		    for leaf nodes since they require no registers
		    to be addressable.  If they require registers it
		    is because they are smaller than INT size and must
		    be forced into temp registers which are reserved
		    for them in this case anyway.The reason for
		    having sur+1 is to force a store if the right is a
		    function call, since otherwise it will be in the
		    wrong register, r0 instead of r2.  Eventually
		    one of the rewrite rules should handle this
		    and move the right side into the correct 
		    register.*/
		    if (p->in.right->in.op==NAME||
		        (p->in.right->in.op ==REG&&!istnode(p->in.right)))
			     sur=p->in.right->in.su=0;
		    if (p->in.left->in.op==NAME||
		        (p->in.left->in.op ==REG&&!istnode(p->in.left)))
			     sul=p->in.left->in.su=0;
			p->in.su=max(sul+fregs,sur+1);
		}/* if unsigned ASG DIV/MOD */
	 	else p->in.su = max(sur,sul+szty(p->in.type));

/*              was: "computed by doing Rt, Lft address, Lft, op, store" */
/*		was:  p->in.su = max(sur,sul+2); 	          (jima) */
 
/*		if(o==ASG MUL||o==ASG DIV||o==ASG MOD)
/*		       p->in.su=max(p->in.su,fregs);
 (old irrelevant stuff?) */
		return;
		}

	switch( o ){

	case ANDAND:
	case OROR:
	case QUEST:
	case COLON:
	case COMOP:
		p->in.su = max( max(sul,sur), 1);
		return;

/* These swaps were moved to optim2, where they could be coordinated with
   swaps for other purposes (jima ) */
/*	case PLUS:
/*	case OR:
/*	case ER:
/*		/* commutative ops; put harder on left */
/*
/*		if(p->in.right->in.su > p->in.left->in.su && !istnode(p->in.left)) {
/*			register NODE *temp;
/*			temp = p->in.left;
/*			p->in.left = p->in.right;
/*			p->in.right = temp;
/*			}
/*		break;
 */
		}

	/* Binary op: Computed by left, then right, then do op */
	p->in.su = max(sul,szty(p->in.right->in.type)+sur);
}

int radebug = 0;

rallo( p, down ) NODE *p; {
	/* do register allocation */
	register o, type, down1, down2, ty;

	if(radebug ) printf( "rallo( %o, %d )\n", p, down );

	down2 = NOPREF;
	p->in.rall = down;
	down1 = ( down &= ~MUSTDO ); /*Prefer the MUSTDO register below */

	ty = optype( o = p->in.op );
	type = p->in.type;


	if(type == DOUBLE || type == FLOAT) {
		if(o == FORCE)
			down1 = F0|MUSTDO;
			/* was: down1 = R0|MUSTDO;  (jima) */
	} else switch(o) {
	case ASSIGN:	
		down1 = NOPREF;
		down2 = down;
		break;
/* This code is also commented out in the vax version******************************
	case MUL:
	case DIV:
	case MOD:
		down1 = R3|MUSTDO;
		down2 = R5|MUSTDO;
		break;

	case ASG MUL:
	case ASG DIV:
	case ASG MOD:
		p->in.left->in.rall = down1 = R3|MUSTDO;
		if(p->in.left->in.op == UNARY MUL) {
			rallo( p->in.left->in.left, R4|MUSTDO );
			}
		else if( p->in.left->in.op == FLD  && p->in.left->in.left->in.op == UNARY MUL ){
			rallo( p->in.left->in.left->in.left, R4|MUSTDO );
			}
		else rallo( p->in.left, R3|MUSTDO );
		rallo( p->in.right, R5|MUSTDO );
		return;
 */
# define MINMOVSLEN 17  /* movs instruction needed for structures this big */

	case DIV:
	case MOD:
	case ASG DIV:
	case ASG MOD:
		if  ((ISUNSIGNED(p->in.type)) 
		  /*==ULONG || p->in.type ==UNSIGNED) &&
		     (p->in.left->in.type==ULONG || 
		     p->in.left->in.type==UNSIGNED||
		     p->in.right->in.type==ULONG || 
		     p->in.right->in.type==UNSIGNED)*/
		    )
		     {/* try always using this instruction dei doesn't cost much and saves time*/
			down1= R0|MUSTDO;
			down2= R2|MUSTDO;/*r0 is needed (with r1) with dei*/}
		break;
	case STASG:
		if (p->stn.stsize >= MINMOVSLEN &&
		    p->in.right->in.op != UNARY STCALL &&
		    p->in.right->in.op != STCALL) {
			/* A movs instruction will be needed - force the
			   addresses of the source and destination into
			   registers r1 & r2, respectively.  Sucomp()
			   forces all other regs to be available, so
			   r0 will be free for the length.  Code is
			   generated in zzzcode.   (jima)
			   However in the case of a STCALL or 
			   UNARY STCALL the function will do
			   the work so the registers should not
			   be reserved*/
			down1 = R2|MUSTDO;
			down2 = R1|MUSTDO;
		}
		break;
	case STARG:/*See above for reasons for check for STCALL */
		if (p->stn.stsize >= MINMOVSLEN && 
		    p->in.left->in.op != UNARY STCALL &&
		    p->in.left->in.op != STCALL) {
			p->stn.rall = R1|MUSTDO; /* is this needed? */
			/* left branch of STARG is the arg */
			down1 = R1|MUSTDO;
		}
		break;
	case CALL:
	case EQ:
	case NE:
	case GT:
	case GE:
	case LT:
	case LE:
	case NOT:
	case ANDAND:
	case OROR:
		down1 = NOPREF;
		break;

	case FORCE:	
		down1 = R0|MUSTDO;
		break;

		}

	if(ty != LTYPE ) rallo( p->in.left, down1 );
	if(ty == BITYPE ) rallo( p->in.right, down2 );

	}

/* Simplify a subtree under UNARY MUL so that oreg2 will form an OREG later */

/* optim2() has swapped branches under PLUS so that strict Lft, then Rt
   order of evaluation will generate reasonable things (jima) */
offstar( p ) register NODE *p; {
	register NODE *l = p->in.left;
	register NODE *r = p->in.right;
	if (xdebug) {
		printf("--offstar(%d) on:\n",p);
		fwalk(p,eprint,1);
		}


	if(p->in.op == PLUS) {
		/* Put "shifted" index into a reg for index mode [REG:bwd] */
		/* Note: optim2 converts MINUS constant to PLUS (negated con) */
		if( l->in.op==LS ) {
			register NODE *lr;
			if (l->in.left->in.op!=REG ) {
				order( l->in.left, INTAREG);
				return;
				}
			lr = l->in.right;
			if(lr->in.op==ICON && lr->tn.lval>=0 && lr->tn.lval<=3){
				/* should be able to form 0(RHS)[Reg:bwdq] */
				if (makeaddr(r)) return;
				if (r->in.op==OREG && R2TEST(r->tn.rval))
					{ order(r,INTAREG); return; }
				}
			} /* processing for << on left */

		 /* Help formation of constant(Reg) or (Reg/Name)[Reg:b] */
		if((r->in.op==ICON || r->in.op==REG || ISPTR(r->in.type)) 
		   && l->in.op!=REG) {
			order(l, INTAREG|INAREG);
			return;
			}
		/* The following probably is unnecessary because optim2 puts 
		   index expressions on the left to be processed first (jima) */
		/* If the left side requires a register and the right side 
		   is put in a register that will mean 2 registers are needed
		   since the left side will probably be unable to share. */  
		if((l->in.op==ICON || (l->in.op==REG &&
		    (!istnode(l)|| p->in.su >= 2)) || (ISPTR(l->in.type)
		    && (p->in.su >=(l->in.su+1))) )
		   && r->in.op!=REG) {
			/* okay to do rt because lft uses no more reg than
			   allowed by operation*/
			order(r, INTAREG|INAREG);
			return;
			}
		} /* PLUS */

	/* If p is cannot be collapsed into STARNM, try and collapse child*/
	if(p->in.op == UNARY MUL && !shumul(l)) {
		offstar( l );
		return;
	}
	order( p, INTAREG|INAREG );
}
/*The old offstar body:
/*	if(p->in.op == PLUS) {
/* /* I think this was here as a klude to avoid inconsistencies in order of
/* /* evaluation between offstar and sucomp/store.  This should no longer be
/* /* a problem, so this is removed.			(jima)
/*		if(p->in.left->in.su == fregs) {
/*			order( p->in.left, INTAREG|INAREG );
/*			return;
/*		} else if( p->in.right->in.su == fregs ) {
/*			order( p->in.right, INTAREG|INAREG );
/*			return;
/*		}
/*		if(p->in.left->in.op==LS && 
/*		  (p->in.left->in.left->in.op!=REG || tlen(p->in.left->in.left)!=sizeof(int) ) ) {
/*			order( p->in.left->in.left, INTAREG|INAREG );
/*			return;
/*		}
/*		if(p->in.right->in.op==LS &&
/*		  (p->in.right->in.left->in.op!=REG || tlen(p->in.right->in.left)!=sizeof(int) ) ) {
/*			order( p->in.right->in.left, INTAREG|INAREG );
/*			return;
/*		}
/*		if(p->in.type == (PTR|CHAR) || p->in.type == (PTR|UCHAR)) {
/*			if(p->in.left->in.op!=REG || tlen(p->in.left)!=sizeof(int)) {
/*				order( p->in.left, INTAREG|INAREG );
/*				return;
/*			}
/*			else if( p->in.right->in.op!=REG || tlen(p->in.right)!=sizeof(int) ) {
/*				order(p->in.right, INTAREG|INAREG);
/*				return;
/*			}
/*		}
/*	}
/*	if(p->in.op == PLUS || p->in.op == MINUS) {
/*		if(p->in.right->in.op == ICON) {
/*			p = p->in.left;
/*			order( p , INTAREG|INAREG);
/*			return;
/*			}
/*		}
/*
/*	if(p->in.op == UNARY MUL && !canaddr(p)) {
/*		offstar( p->in.left );
/*		return;
/*	}
/*	order( p, INTAREG|INAREG );
/*(end old offstar body) */

/* Emit code for unary operator on <src>, result into <dest> 		*/
/* Evaluation order is <src>, then <dest>.			        */
/* Returns 1 for intermediate results (dest or src is being modified) 	*/
/* and zero when completed.  dest and/or src may be changed.  The 	*/
/* caller [e.g., setasg()] must arrange for rewriting or reclaimation.	*/

emit_uop(dest,op,src) NODE *dest, *src; {
	char *opsuf;
	register i;
	extern char *opsuffix();
	ASSERT(src->in.op!=ICON,"uop icon?");

	if (makeaddr(src)) return(1);
	if (!canaddr(dest) && src->in.su>szty(src->in.type)) {
		/* Must complete src to avoid evaluation order error */	
		order(src,INTAREG|INTBREG);
		return(1);
		}
	if (makeaddr(dest)) return(1);

	opsuf = opsuffix(dest,src);
	if (*opsuf!='b' && *opsuf!='w' && *opsuf!='d' && 
	    *opsuf!='f' && *opsuf!='l') {
		/* sizes (or types) are incompatible */
		/* put <src> into a register to resolve incompatibility */
		order(src,INTAREG|INTBREG);
		return(emit_uop(dest,op,src));
		}

	if (op==COMPL) printf("	com%c	", *opsuf );
	else
	if (op==UNARY MINUS) printf("	neg%c	", *opsuf );
	else
		cerror("emit_uop");
	adrput(src);
	printf(",");
	adrput(dest);
	printf("	;emit_uop\n");
	return(0);
	}


/* The only time this rewrite is invoked is FOREFF, and order will convert
   the INCR or DECR into +=.  Otherwise, setasg() will assign the operand
   to the LHS and then rewrite as +=.  (jima) */
setincr( p ) register NODE *p; {
	return( 0 );
	}

/* Evaluate an operation with INCR/DECR as one or both operands.  Called 
   from setasg.  The operation is done with the INCR/DECR nodes removed.
   Then, saved copies are used to do the increment(s)/decrement(s).  
   
   The operands need not be addressable.  (jima) */

do_op_with_incr(p,cookie) register NODE *p; {
	register NODE *l_incr = NIL;
	register NODE *r_incr = NIL;
	register NODE *q, *temp;
	register NODE *second;

	/* determine order of evaluation */
	if (asgop(p->in.op)) {
		q = p->in.right;
		second = p->in.left;
		}
	else {
		q = p->in.left;
		second = (optype(p->in.op)==BITYPE? p->in.right : q);
		}

	for(;;) {
		if (q->in.op==INCR || q->in.op==DECR) {
			if (!canaddr(q->in.left)) order(q->in.left,FORADDR);
			/* save the INCR/DECR for later */
			if(l_incr==NIL) l_incr = tcopy(q);
			else            r_incr = tcopy(q);
			/* replace INCR/DECR with the plain operand */
			tfree(q->in.right);
			ncopy(q,temp=q->in.left);
			temp->in.op = FREE;
			}
		else
		if (!canaddr(q)) order(q,FORADDR);

		if (q==second) break;
		q=second;
		}

	order(p,cookie);  /* do the op */

	for(q=l_incr;;) {
		if (q!=NIL) {
			if (q->in.op==INCR) q->in.op = ASG PLUS;
			else
			if (q->in.op==DECR) q->in.op = ASG MINUS;
			else
				cerror("do_op_with_incr");
			order(q,FOREFF);
			}
		if (q==r_incr) break;
		q=r_incr;
		}
	}

/* setexpr is called from order() to implement rewriting of expressions, 
   including ASG ops and OPLOGs.  Plain expressions can get here when they 
   are dangling, as with the comma operator or when doing address calculations,
   or when asgop() need something in a temp registers.

   This is the last stop for makeaddr() calls which can't be satisfied
   with the tables;  such items are forced into temporary registers here.  
   (makeaddr itself usually evaluates for cookie=FORADDR, which does not 
   include INTAREG|INTBREG).

   Recursion occurs on Binary and some unary ops to make their descendents
   addressable.

   1 is returned if re-evaluation is appropriate, 0 if nothing happened.

   Implementation note: Store assumes certain order of evaluation, and only
     eliminates function calls which would create a register conflict with
     that order of evaluation.  This must be kept in mind here.    (jima)
*/
setexpr( p, cook ) register NODE *p; {  /* (jima) */
	register int  p_op = p->in.op;
	int opty = optype(p_op);
	extern NODE * icon_alloc();

	if (odebug) {
		printf("setexpr(%d):\n",p);
		fwalk(p,eprint,1);
		}
	if (opty == UTYPE) {
		/* Future: Have to handle SCONV nodes somehow */
		if (calloffstar(p)) {} 
		else
		if ( !makeaddr(p->in.left) ) {
			/* No possible recursion - have to put in temp reg */
			ASSERT(p_op==UNARY AND || 
			       p_op==UNARY MINUS ||
			       p_op==COMPL ||
			       p_op==SCONV, "setexpr: unexp. UTYPE");
			order(p,INTAREG|INTBREG);
			}
		}
	else
	if (opty == BITYPE) {
		register NODE *l = p->in.left;
		register NODE *r = p->in.right;
		register int  r_op = r->in.op;
		register int  l_op = l->in.op;

		/* store() and sucomp() assume that the left will be evaluated
	   	   first (so calls are left there, not stored); calls on 
		   the right were previosly stored into temp mem.  Except
		   for assignment ops, which are assumed to be done Right,
		   then Left.  See sucomp() & store().  	(jiam) */

		if (r_op==INCR || r_op==DECR || l_op==INCR || l_op==DECR) {
			do_op_with_incr(p,cook);
			return(1);
			}
		/*  The following code is for the case of a real
		/*  int /=or %=  unsigned; expression this is guaranteed by C
		/*  to be done as a signed expression valid to 32 bits after
		/*  sign and zero extending its operands.  Since we change
		/*  regular expressions that are int / unsigned into /= and
		/*  an unsigned template correctly matches that case we
		/*  must change the right side of this to be signed so
		/*  it will match the signed divide/mod templates.
		/*  This must be done 32 bits unless the size of the right side
		/*  is char in which case it can be done 16 bits since that
		/*  will avoid sign extending the right side in the mod/div .*/
		/*  We are assuming that all bits are valid in register
		/*  variables no matter what their apparent size*/
		if (canaddr(r)&&canaddr(l)&&(p_op== ASG MOD ||p_op== ASG DIV)&&
		   !ISUNSIGNED(p->in.type)&&ISUNSIGNED(r->in.type))
		{
			
			if (tlen(r)!= SZINT/SZCHAR && ( r->in.op!=REG))
					order(r,INTAREG);
			if (tlen(r)== SZCHAR/SZCHAR)
				r->in.type=SHORT;
			else r->in.type = INT;
			return(1);
		}

		if (asgop(p_op) && makeaddr(r)) return(1);

		if (asgop(p_op) && (r->in.su+l->in.su > p->in.su)) {
			/* Order of evaluation is critical */
			/* Must put RHS into a register */
			ASSERT(r->in.op!=REG,"bad asg su");
			order(r,INTAREG|INTBREG);
			}
		else
/*		if (logop(p_op) && (l_op==INCR || l_op==DECR))  {
/*			/* Do the top operation direct with datum under
/*			   the ++ or --, then do += or =- */
/*			if (!makeaddr(l->in.left)) { /* side-effects 1st */
/*				p->in.left = tcopy(l->in.left);
/*				order(p,cook); /* do the top operation */
/*				l->in.op = (l_op==INCR ? ASG PLUS : ASG MINUS);
/*				order(l,FOREFF); /* do the incr/decr */
/*				}
/*			}
/*		else
/*		if (logop(p_op) && (r_op==INCR || r_op==DECR))  {
/*			if (!makeaddr(r->in.left)) {
/*				p->in.right = tcopy(r->in.left);
/*				order(p,cook);
/*				r->in.op = (r_op==INCR ? ASG PLUS : ASG MINUS);
/*				order(r,FOREFF);
/*				}
/*			}
/*		else
 */
		if ( asgop(p_op) && makeaddr(r) ) {}
		else		/* prevent us from losing address of bitfield */
		if ((l->in.op !=FLD|| !asgop(p_op)) && makeaddr(l) ) {}
		else
		if (l->in.op==OREG && r->in.su + l->in.su >3)
		/* OREG's take multiple registers to be addressable so we
		/* must force them into one register if there will not be
		/* enough registers available.  Supposedly if we still need
		/* the right to be addressable we will recurse again and make
		/* it addressable.  If that is not so I will need to do
		/* something strange here to combine this and the next one*/
		{
			order(l,INTAREG);
		}
		else
		if (makeaddr(r) ) {}
		else
		if (canasgop(p_op)) { /* Rewrite to ASGop form */
			/* both sides are now addressable... */
			if (!istnode(l)) order(l,INTAREG|INTBREG);
			p->in.op = ASG p_op;
			/* Changing the op nominally alters the order of 
			   evaluation (asgops are done Rt,then Lft).  It 
			   would seem that additional stores might be needed, 
			   but this should not happen since the LHS is 
			   already a simple temp reg, so the RHS is all that 
			   is left for possible further reduction. */
			}
		else
		if (p_op==INCR || p_op==DECR) { 
			/* None of the optomized handling applied, so do
			   away with the ++ or -- by assigning to a temp */
			order(p,INTAREG|INTBREG);
			}
		else
		/* At this point, the branches are both addressable but not 
		   necessarily in registers.  Probably the op is not yet
		   possible because of incompatible types - put the operands
		   in registers to resolve this (all operations always
		   work with both operands in regs - and unsigned / and %
		   using dei always work with the lhs in reg and rhs in mem). */

		if ( (l->in.op != REG  
# ifdef MEMRMWOK
		      	&& l->in.type != p->in.type
# endif
		     ) || 
		     r->in.op==REG ||
		     ((p->in.op== ASG DIV || p->in.op== ASG MOD)&&
		      ISUNSIGNED(p->in.type)&&
		      !istnode(l)) ) {
			if (asgop(p_op)) {
				/* Mixed sizes must have prevented direct 
				   assignment - put LHS in a temp reg, do
				   it, and return ASSIGN of treg back */
				/* If order of evaluation is critical, the
				   check above will have put the RHS in a reg */
				l = tcopy(l);  /* save for final ASSIGN */
				order( p->in.left, INTAREG|INTBREG );
				if ((p->in.op== ASG DIV || p->in.op== ASG MOD)
					&& ISUNSIGNED(p->in.type) )
				{
					ASSERT((p->in.left->in.rall&~MUSTDO)== R0
						&&p->in.left->tn.lval==R0,
					  	"Wrong register U/%% setexpr");
					l->in.rall= NOPREF;
						
				}
				order( p, INAREG|INBREG ); /* do the op= */
				p->in.rall= NOPREF;

				/* ord_assign is not called here so that
				   the temp reg can be be the final result */
				ncopy( r=talloc(), p );
				ncopy( p, l ); /* to get type, etc. of LHS */
				p->in.right = r;
				p->in.left = l;
				p->in.op = ASSIGN;
				}
			else
				order(l,INTAREG|INTBREG);
			}
		else
		if ( r->in.op!=REG ) order(r,INTAREG|INTBREG);
		else {
			/* Both operands are now in registers, but the 
			   operation doesn't work yet.  */
			/* Since all ops should work with registers, this
			   is strange... (jima) */
			cerror("setexpr: BINOP fails with regs");
			order(p,INTAREG|INTBREG);
			}
		}
	else {
		/* leaf */
		ASSERT(p_op==CCODES,"setexpr: unxpected leaf");
		order(p,INTAREG|INTBREG);
		}
	return(1);
	}
int skip_return_code = 0;
extern struc_skip_lab;

setstr( p ) register NODE *p; { /* structure assignment */
	if (p->in.op ==FORCE)	/* returning a function that returns a
				   structure.  Pass the pointer to the
				   function and then skip the structure
				   return code, since the function we are
				   calling will do the actual moving of the
				   structure.  Tell cgram.y about this by
				   setting skip_return_code to 1;
				   That tells it to use struc_skip_lab to
				   branch to. */
	{
		 NODE *newnode = talloc();
		 NODE *parentnode=talloc();

		 parentnode->in.left = newnode;
		 parentnode->in.op=UNARY MUL;
		 parentnode->in.type= STRTY;/*  pointer to struct */
		 parentnode->in.su = 0;
		 parentnode->tn.rval=0;
		 newnode->in.name = "12(fp)";
		 newnode->in.op = NAME;
		 newnode->in.su = 0;
		 newnode ->in.type =INCREF(STRTY);
		 newnode ->in.left = newnode->in.right = NIL;
		 if (p->in.left->in.op == STCALL || p->in.left->in.op == UNARY STCALL)
		 {
		 	addstrucarg(p->in.left,p->in.left,parentnode);
			if (!genscall(p->in.left,FORSTRUCT))/* succeeded */
			{
			    if (!struc_skip_lab) struc_skip_lab =getlab();
			    skip_return_code = 1;
			    return(1);
			}
			return 0;/*  call failed */
		 }
		 else cerror("bad op to setstr");
	 }
	
	if(p->in.right->in.op != REG) {
	        if (p->in.right->in.op== STCALL||p->in.right->in.op==UNARY STCALL)
		{/*  If this is a call the actual assignment is done by the
		 /*  called procedure through the pointer that is passed.
		 /*  Therefore addstrucarg puts a u& above the left argument
		 /*  adds it as the first argument to p->in.right and then
		 /*  rewrites p as p->in.right.*/
			addstrucarg(p,p->in.right,p->in.left); 
		        return(!genscall(p,FORSTRUCT));
		}
		order( p->in.right, INTAREG );
		return(1);
		}
	p = p->in.left;
	if(p->in.op != NAME && p->in.op != OREG) {
		if(p->in.op != UNARY MUL ) cerror( "bad setstr" );
		order( p->in.left, INTAREG );
		return( 1 );
		}
	return( 0 );
}
/* rewrite for ASSIGN,any goal:

   A number of operations can't be handled well just with templates in 
   table[] because they require simultaneous use of the LHS of an assignment
   and the expression on the RHS.  This used be handled by forcing the RHS
   into a temp reg using the INTAREG cookie, but this did not allow direct 
   evaluation into the LHS.  These cases (which are not handled by the 
   templates) are implemented ad-hoc here.

	.  ASSIGNs of relocatable constants (U& -> NAME) are implemented 
	   using the addr or lxpd instruction.  

	.  Code is generated for COMPL, UNARY MINUS, and subtraction from zero.

	.  INCR and DECR are converted to assignment and +=.

	.  If the RHS is a string of binary ops which have assignment forms, 
	   convert to use assignment ops (ASG ops) and use the LHS of the
	   ASSIGN as an accumulator if appropriate.  

	.  If none of the above apply, first the RHS, then the LHS, are
	   made addressable (recursion).  setexpr() may eventually force
	   the RHS into a temp register.
*/
setasg( p, cook ) NODE *p; {  /* (jima) */
	register NODE *q;
	register NODE *r = p->in.right;
	register NODE *l = p->in.left;
	register int  r_op = r->in.op;
	register int  l_op = l->in.op;
	int truelab, falselab;
	extern int odebug;

	/* Through all of this, call makeaddr() [which calls offstar() for
	   UNARY MULs, etc.) to do additional processing to make nodes
	   addressable when needed.  offstar() modifies whatever
	   is under the UNARY MUL so that oreg2 will recognize and collapse 
	   it into an OREG in next canon().  

	   sucomp() and store() assume left,then right order of evaluation
	   except for asgops, which are assumed to be done right, then
	   left. (jima) */


	if (optype(r_op)==UTYPE && !callop(r_op)) {
		/* get stuff under unary ops addressable, then LHS */
		if (calloffstar(r)  /* UNARY MUL */
		    || makeaddr(r->in.left)) return(1);
		 if ( r->in.op== FLD) {/* JZ if the right is a field
				        put it in a register*/
			   order(r,INTAREG);
			   return(1);
			   }
		if (r->in.su+l->in.su > p->in.su &&
		    r->in.left->in.su>szty(r->in.type)){
			/* must put right into a register */
			order(r->in.left,INTAREG|INTBREG);
			return(1);
			}
		if (makeaddr(l)) return(1);
		}

/* FLD patch ---maybe unneeded now?--- */
/* 	if(l_op == FLD && !(r->in.type==INT || r->in.type==UNSIGNED)) {
/* 		order( r, INAREG);
/* 		return(1);
/* 		}
/* end of FLD patch */

	switch( r_op ) {
	case UNARY AND:
		/* RHS is an "address" - implement assignment with addr/lxpd.
	
	   	Note: The first pass does not collapse U& expressions into 
		ICONs to avoid matching ordinary templates including ICONs 
		(since the Mesa linker doesn't support relocable constants, 
		addr/lxpd initcode instructions must be used).  optim() & 
		andable() make the 1st pass decision on this.  But they
	   	ARE collapsed into ICONs by optim2 when the object of 
		callops - for use with cxp - but that it the only time 
	   	this occurs. [end note] */
	
	        if (tlen(l)!=4 &&!istnode(l)) 
			cerror("LHS can't hold an address (setasg)");
	
		printf("	");
		addrlxpd(r->in.left);  /*  emit "addr/lxpd\t" */
		adrput(r->in.left); printf(","); adrput(l);
		printf("	;setasg\n");
		rtyflg++; /* prevent changing of reg types (see reclaim)*/
		reclaim(p,RLEFT,INAREG);  /* return LHS in case it is needed */
		return(1);

	case COMPL:
	case UNARY MINUS:
		/* put out COMi or NEGifl direct into LHS */
		if (!emit_uop(l,r_op,r->in.left))
			rtyflg++; /* prevent changing of reg types in reclaim)*/
			reclaim(p,RLEFT,INAREG|SOREG); /* return LHS */
		return(1);

	case MINUS:
		if (!tshape(r->in.left,SZERO)) break;
		/* Subtraction from zero - emit NEGifl instruction */
		if (!emit_uop(l,UNARY MINUS,r->in.right))
			rtyflg++; 
			reclaim(p,RLEFT,INAREG|SOREG);  /* done LHS */
		return(1);

	case INCR:
	case DECR:
		/* post-increment/decrement after assignment */
		do_op_with_incr(p,cook);
		return(1);

	case SCONV:
		if (makeaddr(r->in.left) || makeaddr(l)) return(1);
		/* If the conversion doesn't need run-time code, or
		   if a temporary is necessary (e.g., float = (double) int),
		   the conversion is evaluated via order... */
		{ register rl_type = r->in.left->in.type;
		if (l->in.type!=r->in.type ||
		    (l->in.type!=FLOAT && l->in.type!=DOUBLE &&
		     rl_type!=FLOAT && rl_type!=DOUBLE)) {
			order(r,FORADDR);
			return(1);
		} }
		/* Conversion direct to LHS */
		p->in.right = r->in.left; /* remove the SCONV node */
		r->in.op = FREE;
		/*   Change floating constants to smaller size if possible */
		if (p->in.right->in.op == FCON)
		{		/* ( */
		    if ((p->in.left->in.type == FLOAT)||
# if ns16000
			p->in.right->fpn.dval == (float) p->in.right->fpn.dval)
#else
			float_ceq(p->in.right->fpn.dval,
				  float_double(
				  	double_float(p->in.right->fpn.dval))))
#endif
		    	p->in.right->in.type = FLOAT;
		    }


		zzzcode(p,'A',cook); /* do the converting move */
		printf("	;setasg\n");
		reclaim(p,RLEFT,cook); /* return LHS */
		return(1);

	} /* end switch r_op */

	if (logop(r_op)) {
		order(r,FORCC);
		return(1); /* An ASSIGN template should now work */
		}

	if (canasgop(r_op)) /* an assign-op form exists */ {
		/* Only the ASGop forms exist in the table.
		 * The LHS and operator types are either both float/double or
		 * both scalar (SCONV nodes always appear at mixtures).
		 *
		 * Use the LHS as an accumulator with assignment-ops
		 * if that is the best thing to do.  Otherwise use a temp
		 * register.
		 *
		 * (LHS = A op B) becomes (LHS=A,A op=B) if possible, and
		 * side-effects or aliasing can't occur.
		 *  Checks:
		 *    1. Types of LHS and operator are compatible.
		 *    2. LHS  REG.  I/o registers, which have side-
		 *       effects, are assumed to be indirect, never NAMES.
		 *    3. LHS not explicitly referenced in RHS.
		 *    4. RHS not unsigned DIV/MOD.  Since this is always
		         done into r1 r0 pair using the dei instruction
			 there is no saving in first using an asg op.
			 However we must check first to see if it is
			 allready in a temporary register.   */

		aliasing = 0;
		if (l_op!=REG ||(!istreg(l->tn.lval) &&( l->in.type == CHAR||

				 l->in.type ==UCHAR ||l->in.type ==SHORT ||
				 l->in.type == USHORT))||
				 ((r->in.op==DIV||r->in.op==MOD)&&
				  ISUNSIGNED(r->in.type)&&!istreg(l->tn.lval))|| /* && 
/* The following code used to allow the above described optimisation for
/* NAME nodes if it was safe.  It has been removed since there is a
/* considerable time penalty for that optimisation and we wish to optimise
/* speed most of all. The following comment is for that withdrawn optimisation*/
			/* Conversion for non-regs are not free, and there   */
			/* might be hidden aliasing.  It is also inefficient */
			/* to use a non-reg as an accumulator > one op.      */
			/* Previously checked for tos also*/
			/* We should not make an assignment op out of unsigned*/
			/* div or mod unless it is already in a register*/
			/* However we may change this to allow this to be done*/
			/* when the left is twice the size of the right.*/
			/* since the dei instruction will allow the operands*/
			/* to be in memory then*/
/*			( l_op!=NAME ||
/*			  (l_op == NAME && l->in.name[0] == '\0')|| 
/*			  (tlen(l) != tlen(r)||
/*			  ((ISUNSIGNED(r->in.type) && (r->in.op == DIV ||r->in.op ==MOD))&& !istnode(r->in.right))) ||
/*		          (walkf(r,mightalias), aliasing) || 
/*
/*			  canasgop(r->in.left->in.op))||*/
		   ( 
		    /* check for explicit aliasing on RHS */
		     (aliastarget=l, walkf(r,isalias), aliasing) ) 
		     /* we lose information on types so they better be the
		        the same.  Watch out when we get char/short register
			variables. They may present a problem here. */
		   ||ISUNSIGNED(l->in.type)!= ISUNSIGNED(r->in.left->in.type)){
			/* Use temporary which can be safely assigned    */
			/* On recursion the temp will be an accumulator. */
			if (odebug) printf("setasg: using temp.\n");
			order(r,INTAREG|INTBREG);
			}
		else {
			register TWORD lhs_type;
			if (odebug) printf("setasg(%d): ASGop'ing\n",p);
			/* Do an assignment of the left branch below the op
			   into the original LHS, recursively evaluating the
			   r->in.left sub-tree.  Then rewrite the op as
			   an ASGop, replacing the original ASSIGN, and do it.
			   The original lhs is used as an accumulator.
			   Left-to-right evaluation order is preserved.

			   If <expr> starts with an operator which can't be
			   rewritten as an ASGop, then the effect of
			      "A=((((  <expr>) +B) +C) +D) +E" is 
			      "  ((((A=<expr>)+=B)+=C)+=D)+=E".  */

			lhs_type = l->tn.type;
			ASSERT(!mixtypes(l,r),"setasg: mixtypes");

			/* ASSIGN the LHS to the deeper context, leaving
			   the result in r->in.left. */
			l->in.rall = r->in.rall;  /* probably irrelevant */
			ord_assign(l, r->in.left, r->in.left);

			ASSERT(l_op==REG || tlen(p)==tlen(r), "setasg: conv");
			/* Move up the RHS to replace the old ASSIGN node */
			r->in.rall = p->in.rall;  /* preserve MUSTDOs */
			ncopy(p,r);
		  	r->in.op = FREE;
		        p->in.op = ASG p->in.op; /* change to assignment form */
			order(p,FORADDR);        /* do it, get back lhs */
			/* The ASSIGN may imply intra-scalar conversion: */
			p->in.type = lhs_type;  /* nop on 16032 */
			}
	       	return(1);
		} /* canasgop */

	if (r_op==CCODES || !makeaddr(r)) {
		/* right is already addressable or CCODES */
		if (l->in.su + r->in.su > p->in.su) {
			/* must do strictly in Rt, then Lft order */
			ASSERT(r->in.op!=REG && r->in.op!=CCODES,
			       "setasg: bad su");
			order(r,INTAREG|INTBREG);
			}
		else {
			if (!makeaddr(l)) { 
			  	/* both are addressable (or CCODES) */
				if (r->in.op!=REG && r->in.op!=CCODES)
					order(r, INTAREG|INTBREG);
				else
					cerror("setasg: can't do it");
				}
			}
		}
	return(1);
	}

/* setasop is called from order() to do rewrites of assignment operators. */
setasop( p ) register NODE *p; {
	/* setup for =ops */

/*	register rt, ro;
/*	register NODE *r = p->in.right;
/*
/*	rt = r->in.type;
/*	ro = r->in.op;
/*
/*	if(ro == UNARY MUL && rt != CHAR) {
/*		offstar( r->in.left );
/*		return(1);
/*		}
/*
/*	if(( rt == CHAR || rt == SHORT || rt == UCHAR || rt == USHORT ||
/*			( ro != REG && ro != ICON && ro != NAME && ro != OREG ) ) ){
/*		order( r, INAREG|INBREG );
/*		return(1);
/*		}
/*	if((p->in.op == ASG LS || p->in.op == ASG RS) && ro != ICON && ro != REG) {
/*		order( r, INAREG );
/*		return(1);
/*		}
/*
/*	p = p->in.left;
/*	if(p->in.op == FLD ) p = p->in.left;
/*
/*	switch( p->in.op ){
/*
/*	case REG:
/*	case ICON:
/*	case NAME:
/*	case OREG:
/*		return(0);
/*
/*	case UNARY MUL:
/*		if(p->in.left->in.op==OREG )
/*			return(0);
/*		else
/*			offstar( p->in.left );
/*		return(1);
/*
/*		}
/* */
	/* 
	cerror( "setasop" );  /* setasop() NOT USED NOW (jima) */
	}

int crslab = 9999;  /* Honeywell */

getlab(){
	return( crslab-- );
	}

deflab( l ){
	printf( "L%d:\n", l );
	}
#line 1280
genargs( p, ptemp ) register NODE *p, *ptemp; {
	register NODE *pasg;
	register align;
	register size;
	register TWORD type;

	/* generate code for the arguments */

	/*  first, do the arguments on the right */
	while( p->in.op == CM ){
		genargs( p->in.right, ptemp );
		p->in.op = FREE;
		p = p->in.left;
		}

	if(p->in.op == STARG) { /* structure valued argument */

		size = p->stn.stsize;
		align = p->stn.stalign;
		if(p->in.left->in.op == ICON) { /* Should never occur on the 16k */
			p->in.op = FREE;
			p= p->in.left;
			}
		else if(p->in.left->in.op==STCALL||
		             p->in.left->in.op==UNARY STCALL)
		{
			p->in.op = FREE;
			p = p->in.left;
			genscall(p,FORARG);
			rfree(R0,INT);/*will never be freed otherwise */
			p->in.op=FREE; /*We are through with this node now*/
			return;
		}
		else
		{
			/* make it look beautiful... */
			register int i;
			p->in.op = UNARY MUL;
			canon( p );  /* turn it into an oreg */
			for (i=0; p->in.op!=OREG; i++) {
				offstar( p->in.left );
				canon( p );
				if (i>=15) cerror( "stuck starg" ); /* was 2 JZ*/
				}
			}


 		ptemp->tn.lval = 0;	/* all moves to (sp) */

		pasg = talloc();
		pasg->in.op = STASG;
		pasg->stn.stsize = size;
		pasg->stn.stalign = align;
		pasg->in.right = p;
		pasg->in.left = tcopy( ptemp );

		/* the following line is done only with the knowledge
		that it will be undone by the STASG node, with the
		offset (lval) field retained */

		if(p->in.op == OREG ) p->in.op = REG; /* only for temporaries*/

 		order( pasg, FORARG );
		ptemp->tn.lval += size;
		return;
		}

	/* ordinary case */

	order( p, FORARG );
	}

argsize( p ) register NODE *p; {
	register t;
	t = 0;
	if(p->in.op == CM) {
		t = argsize( p->in.left );
		p = p->in.right;
		}
	if(p->in.type == DOUBLE || p->in.type == FLOAT) {
		SETOFF( t, 4 );
		return( t+8 );
		}
	else if( p->in.op == STARG ){
 		SETOFF( t, 4 );  /* alignment */
 		return( t + ((p->stn.stsize+3)/4)*4 );  /* size */
		}
	else {
		SETOFF( t, 4 );
		return( t+4 );
		}
	}


