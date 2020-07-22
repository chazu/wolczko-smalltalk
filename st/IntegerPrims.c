/*
  	Smalltalk-80 Virtual Machine:
	Primitive methods

	Copyright (c) 1984,1985,1986,1987,1988,1989 Mario Wolczko
	All rights reserved.

This module contains the integer primitive methods, which are
called via a dispatch table.

$Log: IntegerPrims.c,v $
 * Revision 5.1  1994/08/31  00:28:47  mario
 * Fixed bug in shift prim (shift right by 32 or more was undefined)
 *
 * Revision 5.0  1994/08/16  00:58:55  mario
 * Sync with others
 *
 * Revision 4.1  89/06/21  19:53:48  mario
 * Stabilised with other files at 4.1.
 * 
 * Revision 3.1  87/03/11  15:20:26  miw
 * 32-bit version; no D-B; stabilised with other modules
 * 
 * Revision 2.3  87/02/13  20:06:52  miw
 * Converted to 32-bit:  Made prMultSI a separate routine because of the extra
 * checking required to detect potential overflows.
 * 
 * Revision 2.2  86/01/17  18:43:11  miw
 * Made stack access more efficient.
 * 
 * Revision 2.1  85/11/19  17:40:16  miw
 * Stabilised with all other modules at Rev 2.1
 * 
 * Revision 1.2  85/07/05  13:03:32  miw
 * Removed redundant ref.counts where appropriate.
 * 
 * Revision 1.1  85/01/10  12:54:19  miw
 * Initial revision
 * 
*/

static char *rcsid= "$Header: /home/mario/st/RCS/IntegerPrims.c,v 5.1 1994/08/31 00:28:47 mario Exp mario $";

#include "Prim.h"



/************ ARITHMETIC PRIMITIVES **************/

#define SIAP(OP)        /* SmallInteger Arithmetic Primitive */         \
	{                                                               \
		SIGNED intArg, intRcvr;                                 \
		int intRes;     /* preserve precision */                \
		OOP intPtr= popStack;                                  \
									\
		if (isInt(intPtr)) {                                    \
			intArg= intVal(intPtr);                        \
			intPtr= popStack;                              \
			if (isInt(intPtr)) {                            \
				intRcvr= intVal(intPtr);               \
				intRes= intRcvr OP intArg;             \
				if (isIntVal(intRes)) {                 \
					NRpush(intObj(intRes));         \
					return FALSE;                   \
				}                                       \
			}                                               \
			unPop(2);                                       \
		} else                                                  \
			unPop(1);                                       \
		return TRUE;                                            \
	}


					/*ST!Interpreter!primitiveAdd!*/
					/*ST!Interpreter!popInteger!*/
					/*ST!Interpreter!pushInteger:!*/

BOOL prAddSI()          SIAP(+)         /* primitiveAdd */
BOOL prSubSI()          SIAP(-)         /* primitiveSubtract */

BOOL prMultSI()                  /* primitiveMultiply */
{                                                               
	SIGNED intArg, intRcvr;
	int intRes;     /* preserve precision */
	OOP intPtr= popStack;
	
	if (isInt(intPtr)) {
		intArg= intVal(intPtr);
		intPtr= stackTop;
		if (isInt(intPtr)) {
			intRcvr= intVal(intPtr);
			/* At this point we have a pair of integers but
			   we don't know yet whether the product will overflow.
			   First, we deal with the case where both numbers are
			   15-bit integers; this cannot possibly overflow
			   (note that -32768*-32768 is *not* acceptable) */
			if (-32768 < intRcvr && intRcvr < 32768
			    && -32768 < intArg && intArg < 32768) {
				intRes= intRcvr * intArg;
				stackTop= intObj(intRes);
				return FALSE;
			} else {
				/* Hopefully the previous test caught the
				   majority of cases.  Now we see if the
				   product *might* overflow.  Consider the
				   absolute values of the multiplicand and
				   the multiplier, m and n.  Then let
				   M and N be the smallest integers such that
				   	2^M >= m   and  2^N >= n.
				   Then 2^(M+N) >= m*n; all we do is calculate
				   M and N and determine whether M+N < 31.
				   Of course, what's really required here
				   is a bit of assembler to test the machine's
				   overflow condition code, but that would
				   hardly be portable...*/
				int M= 1, N= 1;
				int absRcvr= abs(intRcvr), absArg= abs(intArg);
				while (absRcvr) {
					++M;  absRcvr >>= 1;
				}
				while (absArg) {
					++N;  absArg >>= 1;
				}
				if (M + N < BPW) {
					intRes= intRcvr * intArg;
					stackTop= intObj(intRes);
					return FALSE;
				}
			}
		}
	}
	unPop(1);
	return TRUE;
}

					/*ST!Interpreter!primitiveDivide!*/
BOOL prDivideSI()
{
	SIGNED intArg, intRcvr, intRes;
	OOP intPtr= popStack;

	if (isInt(intPtr)) {
		intArg= intVal(intPtr);
		intPtr= popStack;
		if (intArg != 0 && isInt(intPtr)) {
			intRcvr= intVal(intPtr);
			intRes= intRcvr / intArg;
			if ((intRcvr % intArg == 0) && isIntVal(intRes)) {
				NRpush(intObj(intRes));
				return FALSE;
			}
		}
		unPop(2);
	} else
		unPop(1);
	return TRUE;
}

					/*ST!Interpreter!primitiveMod!*/
BOOL prModSI()
{
	SIGNED intArg, intRcvr, intRes;
	OOP intPtr= popStack;

	if (isInt(intPtr)) {
		intArg= intVal(intPtr);
		intPtr= popStack;
		if (intArg != 0 && isInt(intPtr)) {
			intRcvr= intVal(intPtr);
			intRes= intRcvr % intArg;
			if (intRes < 0)
				intRes += intArg;       /* round division to -infinity */
			NRpush(intObj(intRes));
			return FALSE;
		}
		unPop(2);
	} else
		unPop(1);
	return TRUE;
}

					/*ST!Interpreter!primitiveDiv!*/
BOOL prDivSI()
{
	SIGNED intArg, intRcvr, intRes;
	OOP intPtr= popStack;

	if (isInt(intPtr)) {
		intArg= intVal(intPtr);
		intPtr= popStack;
		if (intArg != 0 && isInt(intPtr)) {
			intRcvr= intVal(intPtr);
			intRes= intRcvr / intArg;
			if (intRcvr % intArg < 0)       /* round toward -infinity */
				--intRes;
			if (isIntVal(intRes)) {
				NRpush(intObj(intRes));
				return FALSE;
			}
		}
		unPop(2);
	} else
		unPop(1);
	return TRUE;
}



					/*ST!Interpreter!primitiveQuo!*/
BOOL prQuoSI()
{
	SIGNED intArg, intRcvr, intRes;
	OOP intPtr= popStack;

	if (isInt(intPtr)) {
		intArg= intVal(intPtr);
		intPtr= popStack;
		if (intArg != 0 && isInt(intPtr)) {
			intRcvr= intVal(intPtr);
			intRes= intRcvr / intArg;
				/* NON-PORTABLE: assumes -7/2 == -3 */
			if (isIntVal(intRes)) {
				NRpush(intObj(intRes));
				return FALSE;
			}
		}
		unPop(2);
	} else
		unPop(1);
	return TRUE;
}


#define SIBP(OP)        /* SmallIntegerBoolean Primitive */             \
	{                                                               \
		SIGNED intArg;                                          \
		OOP intPtr= popStack;                                  \
									\
		if (isInt(intPtr)) {                                    \
			intArg= intVal(intPtr);                        \
			intPtr= popStack;                              \
			if (isInt(intPtr)) {                            \
				NRpush((intVal(intPtr) OP intArg)         \
					? TRUE_PTR : FALSE_PTR);        \
				return FALSE;                           \
			}                                               \
			unPop(2);                                       \
		} else                                                  \
			unPop(1);                                       \
		return TRUE;                                            \
	}


					/*ST!Interpreter!primitiveLessThan!*/

BOOL prLTSI()           SIBP(<)         /* primitiveLessThan */
BOOL prGTSI()           SIBP(>)         /* primitiveGreaterThan */
BOOL prLESI()           SIBP(<=)        /* primitiveLessOrEqual */
BOOL prGESI()           SIBP(>=)        /* primitiveGreaterOrEqual */
BOOL prEQSI()           SIBP(==)        /* primitiveEqual */
BOOL prNESI()           SIBP(!=)        /* primitiveNotEqual */



#define SIBitP(OP)      /* SmallIntegerBit Primitive */                 \
	{                                                               \
		SIGNED intArg;                                          \
		OOP intPtr= popStack;                                  \
									\
		if (isInt(intPtr)) {                                    \
			intArg= intVal(intPtr);                        \
			intPtr= popStack;                              \
			if (isInt(intPtr)) {                            \
				NRpush(intObj((intVal(intPtr)) OP intArg));\
				return FALSE;                           \
			}                                               \
			unPop(2);                                       \
		} else                                                  \
			unPop(1);                                       \
		return TRUE;                                            \
	}


					/*ST!Interpreter!primitiveBitAnd!*/

BOOL prAndSI()          SIBitP(&)       /* primitiveBitAnd */
BOOL prOrSI()           SIBitP(|)       /* primitiveBitOr */
BOOL prXorSI()          SIBitP(^)       /* primtiveBitXor */


					/*ST!Interpreter!primitiveBitShift!*/
BOOL prBitShSI()
{
	SIGNED intArg, intRcvr, intRes;
	OOP intPtr= popStack;

	if (isInt(intPtr)) {
		intArg= intVal(intPtr);
		intPtr= popStack;
		if (isInt(intPtr)) {
			intRcvr= intVal(intPtr);
			if (intArg < 0) {               /* right shift */
				intArg= -intArg;
				NRpush(intArg >= sizeof(SIGNED)*8
				       ? ZERO_PTR
				       : intObj(intRcvr >> intArg));
				return FALSE;
			} else {
				intRes= intRcvr << intArg;
				if ((intRes >> intArg) == intRcvr) {
					/* NON-PORTABLE: assumes -1>>1==-1 */
					NRpush(intObj(intRes));
					return FALSE;
				}
			}
		}
		unPop(2);
	} else
		unPop(1);
	return TRUE;
}



					/*ST!Interpreter!primitiveMakePoint!*/
BOOL prMkPtSI()
{
	OOP intArg, intRcvr, ptRes;

	if (isInt(intArg= popStack)) {
		if (isInt(intRcvr= popStack)) {
			ptRes= instPtrs(CLASS_POINT, PT_SIZE);
			NRstorePtr(XINDEX, ptRes, intRcvr);
			NRstorePtr(YINDEX, ptRes, intArg);
			push(ptRes);
			return FALSE;
		}
		unPop(2);
	} else
		unPop(1);
	return TRUE;
}
