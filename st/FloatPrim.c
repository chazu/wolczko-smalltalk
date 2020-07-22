/*
  	Smalltalk-80 Virtual Machine:
	Floating Point Primitives

	Copyright (c) 1984,1985,1986,1987,1988,1989 Mario Wolczko
	All rights reserved.

This module assumes that the format of Float objects is compatible with
the host machine's float type.

$Log: FloatPrim.c,v $
 * Revision 5.0  1994/08/16  00:58:25  mario
 * Sync with others
 *
 * Revision 4.1  89/06/21  19:48:53  mario
 * Removed references to VAX.
 * Stabilised with other files at 4.1.
 * 
 * Revision 3.1  87/03/11  15:13:13  miw
 * Stabilised with other modules; no D-B.
 * 
 * Revision 2.7  87/02/13  19:37:30  miw
 * Converted to 32-bit image.  Removed machine dependent cases (these are dealt
 * with by the image conversion program).
 * 
 * Revision 2.6  86/09/16  11:43:25  miw
 * Added cases for SUN.
 * 
 * Revision 2.5  86/01/17  19:22:10  miw
 * Added casts to void where necessary.
 * 
 * Revision 2.2  86/01/17  18:41:01  miw
 * Removed support for APOLLO.
 * Made stack access more efficient due to D-B.
 * Fixed a bug in prFracPart(): wasn't returning the result!
 * 
 * Revision 2.1  85/11/19  17:38:50  miw
 * Stabilised with all other modules at Rev 2.1
 * 
 * Revision 1.4  85/07/05  12:59:42  miw
 * Changed push() to NRpush() where appropriate.
 * 
 * Revision 1.3  85/03/06  15:10:49  miw
 * Added code for Apollo.
 * Removed trace printing.
 * 
 * Revision 1.2  85/02/22  17:07:33  miw
 * Fixed VAX floating point format
 * 
 * Revision 1.1  85/01/10  12:53:29  miw
 * Initial revision
 * 
*/

static char *rcsid= "$Header: /home/mario/st/RCS/FloatPrim.c,v 5.0 1994/08/16 00:58:25 mario Exp mario $";

#include <stdio.h>
#include "Prim.h"

typedef union {
	float   F;
	WORD    W[sizeof(float) / sizeof(WORD)];
} FLOAT;

#define FLOAT_SIZE                      1

#define makeFloat                       instWords(CLASS_FLOAT, FLOAT_SIZE);

#define fetchFloat(oop, f)              { f.W[0]= fetchWord(0, oop); }


/* BEWARE of side-effects when using this macro */
#define storeFloat(oop, fval)           {                               \
					  storeWord(0, oop, fval.W[0]); \
					}


/* BEWARE of side effects because of fetchClass() */
#define isFloat(oop)                    (fetchClass(oop) == CLASS_FLOAT)



BOOL prAsFloat()                     /* primitiveAsFloat */
{
	FLOAT f;
	OOP fptr, intPtr= popStack;

	if (isInt(intPtr)) {
		f.F= intVal(intPtr);
		fptr= makeFloat;
		storeFloat(fptr, f);
		push(fptr);
		return FALSE;
	}
	unPop(1);
	return TRUE;
}


#define FLERR           1               /* any non-zero value will do */


#define FLAP(OP)                        /* Float Arithmetic Primitive */\
	{                                                               \
		FLOAT farg, frcvr, fres;                                \
		OOP fptr= popStack;                                     \
		if (isFloat(fptr)) {                                    \
			fetchFloat(fptr, farg);                         \
			fptr= popStack;                                 \
			if (isFloat(fptr) && setjmp(env) != FLERR) {    \
				fetchFloat(fptr, frcvr);                \
				fres.F= frcvr.F OP farg.F;              \
				fptr= makeFloat;                        \
				storeFloat(fptr, fres);                 \
				push(fptr);                             \
				return FALSE;                           \
			}                                               \
			unPop(2);                                       \
		} else                                                  \
			unPop(1);                                       \
		return TRUE;                                            \
	}

#include <signal.h>
#include <setjmp.h>


static jmp_buf env;             /* environment when floating point trap occurs */


void Float_init()                     /* initialise Floating Point under/overflow trap */
{
	static void FloatError();

	(void)signal(SIGILL, FloatError);
	(void)signal(SIGFPE, FloatError);
}

static void FloatError()
{
	longjmp(env, FLERR);
}


BOOL prAddFL()          FLAP(+)                 /* primitiveFloatAdd */
BOOL prSubFL()          FLAP(-)                 /* primitiveFloatSubtract */
BOOL prMulFL()          FLAP(*)                 /* primitiveFloatMultiply */
BOOL prDivFL()          FLAP(/)                 /* primitiveFloatDivide */


#define FLBP(OP)                        /* Float Boolean Primitive */           \
	{                                                                       \
		FLOAT farg, frcvr;                                              \
		OOP fptr= popStack;                                             \
		if (isFloat(fptr)) {                                            \
			fetchFloat(fptr, farg);                                 \
			fptr= popStack;                                         \
			if (isFloat(fptr)) {                                    \
				fetchFloat(fptr, frcvr);                        \
				NRpush((frcvr.F OP farg.F) ? TRUE_PTR : FALSE_PTR);\
				return FALSE;                                   \
			}                                                       \
			unPop(2);                                               \
		} else                                                          \
			unPop(1);                                               \
		return TRUE;                                                    \
	}


BOOL prLTFL()        FLBP(<)                 /* primitiveFloatLessThan */
BOOL prGTFL()        FLBP(>)                 /* primitiveFloatGreaterThan */
BOOL prLEFL()        FLBP(<=)                /* primitiveFloatLessOrEqual */
BOOL prGEFL()        FLBP(>=)                /* primitiveFloatGreterOrEqual */
BOOL prEQFL()        FLBP(==)                /* primitiveFloatEqual */
BOOL prNEFL()        FLBP(!=)                /* primitiveFloatNotEqual */


BOOL prTruncFL()                             /* primitiveTruncated */
{
	FLOAT f;
	SIGNED i;
	OOP fptr= popStack;

	if (isFloat(fptr)) {
		fetchFloat(fptr, f);
		if (f.F > MIN_INT - 1 && f.F < MAX_INT + 1 && (i= f.F, isIntVal(i))) {
			/* NON-PORTABLE: assumes (int)-1.5==-1 */
			NRpush(intObj(i));
			return FALSE;
		}
	}
	unPop(1);
	return TRUE;
}



BOOL prFracPart()                    /* primitiveFractionalPart */
{
	extern double modf();
	FLOAT f, frac;
	double junk;
	OOP fptr= popStack;

	if (isFloat(fptr)) {
		fetchFloat(fptr, f);
		frac.F= modf(f.F, &junk);
		fptr= makeFloat;
		storeFloat(fptr, frac);
		push(fptr);
		return FALSE;
	}
	unPop(1);
	return TRUE;
}

