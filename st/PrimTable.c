/*
  	Smalltalk-80 Virtual Machine:
	Primitive dispatch table

	Copyright (c) 1984,1985,1986,1987,1988,1989 Mario Wolczko
	All rights reserved.

The primitives return TRUE if they FAIL, FALSE if they succeed.

$Log: PrimTable.c,v $
 * Revision 5.0  1994/08/16  00:59:51  mario
 * Sync with others
 *
 * Revision 4.1  89/06/21  19:57:44  mario
 * Stabilised with other files at 4.1.
 * 
 * Revision 3.1  87/03/11  16:32:07  miw
 * Added prBoot() primitive.
 * 
 * Revision 2.1  85/11/19  17:41:48  miw
 * Stabilised with all other modules at Rev 2.1
 * 
 * Revision 1.2  85/07/05  13:08:39  miw
 * Added the prScanChars() primitive.
 * 
 * Revision 1.1  85/01/10  12:55:38  miw
 * Initial revision
 * 

*/

static char *rcsid= "$Header: /home/mario/st/RCS/PrimTable.c,v 5.0 1994/08/16 00:59:51 mario Exp mario $";

#include "ST.h"

static BOOL primFail();

extern BOOL 
	prAddSI(), prSubSI(),
  	prLTSI(), prGTSI(), prLESI(), prGESI(), prEQSI(), prNESI(),
  	prMultSI(), prDivideSI(), prModSI(), prDivSI(), prQuoSI(), 
	prAndSI(), prOrSI(), prXorSI(),
  	prBitShSI(), prMkPtSI(),
	prAsFloat(), prAddFL(), prSubFL(),
  	prLTFL(), prGTFL(), prLEFL(), prGEFL(), prEQFL(), prNEFL(),
  	prMulFL(), prDivFL(), prTruncFL(), prFracPart(),
  	prAt(), prAtPut(), prSize(), prSAt(), prSAtPut(),
	prOAt(), prOAtPut(), prNew(), prNewWithArg(),
	prBecome(), prIVAt(), prIVAPut(), prAsOop(), prAsObject(), 
	prSomeInstance(), prNextInstance(), prNewMethod(),
	prBlockCopy(), prValue(), prValWithArgs(),
	prPerform(), prPerWithArgs(), prSignal(), prWait(),
	prResume(), prSuspend(), prFlushCache(),
	prMousePt(), prCuLocPut(), prCuLink(), prInpSema(),
	prSampleIntvl(), prInpWord(), prCopyBits(),
	prSnapshot(), prTime(), prTick(), prSigAt(), prBeCursor(),
	prBeDisplay(), prScanChars(),
	prEquiv(), prClass(), prCoreLeft(), prQuit(),
	prOopsLeft(), prSAtOopsLeft(),
  	prBoot();


BOOL (*dispPrim[])()= {
	primFail, 
	prAddSI, prSubSI, prLTSI, prGTSI, prLESI, prGESI, prEQSI, prNESI,
	prMultSI, prDivideSI, prModSI, prDivSI, prQuoSI,
	prAndSI, prOrSI, prXorSI, prBitShSI, prMkPtSI,
	primFail, primFail,
	primFail, primFail, primFail, primFail, primFail, primFail, primFail,
	primFail, primFail, primFail, primFail, primFail, primFail, primFail,
	primFail, primFail, primFail,
	primFail, primFail, 
	prAsFloat, prAddFL, prSubFL, prLTFL, prGTFL, prLEFL, prGEFL, 
	prEQFL, prNEFL, prMulFL, prDivFL, 
	prTruncFL, prFracPart, primFail, primFail,
	primFail, primFail, primFail, primFail, primFail, 
	prAt, prAtPut, prSize, prSAt, prSAtPut,
	primFail, primFail, primFail, 
	prOAt, prOAtPut, prNew, prNewWithArg,
	prBecome, prIVAt, prIVAPut, prAsOop, prAsObject, 
	prSomeInstance, prNextInstance, prNewMethod,
	prBlockCopy, prValue, prValWithArgs,
	prPerform, prPerWithArgs, prSignal, prWait,
	prResume, prSuspend, prFlushCache,
	prMousePt, prCuLocPut, prCuLink, prInpSema,
	prSampleIntvl, prInpWord, prCopyBits,
	prSnapshot, prTime, prTick, prSigAt, prBeCursor,
	prBeDisplay, prScanChars, 
	primFail, primFail, 
	primFail, primFail, primFail, primFail, 
	prEquiv, prClass, prCoreLeft, prQuit, primFail, 
	prOopsLeft, prSAtOopsLeft,
	primFail, primFail, primFail, primFail, 
	primFail, primFail, primFail, primFail, 
	primFail, primFail, primFail, 
	primFail, primFail, primFail, primFail,
	primFail, primFail, primFail, primFail,
	primFail, primFail, primFail, primFail,
	primFail, primFail, primFail, primFail,
	primFail, primFail, primFail, primFail,
	primFail, primFail, primFail, primFail,
	primFail, primFail, primFail, primFail,
	primFail, primFail, primFail, primFail,
	primFail, primFail, primFail, primFail,
	primFail, primFail, primFail, primFail,
	primFail, primFail, primFail, primFail,
	primFail, primFail, primFail, primFail,
	primFail, primFail, primFail, primFail,
	primFail, primFail, primFail, primFail,
	primFail, primFail, primFail, primFail,
	primFail, primFail, primFail, primFail,
	primFail, primFail, primFail, primFail,
	primFail, primFail, primFail, primFail,
	primFail, primFail, primFail, primFail,
	primFail, primFail, primFail, primFail,
	primFail, primFail, primFail, primFail,
	primFail, primFail, primFail, primFail,
	primFail, primFail, primFail, primFail,
	primFail, primFail, primFail, primFail,
	primFail, primFail, primFail, primFail,
	primFail, primFail, primFail, primFail,
	primFail, primFail, primFail, primFail,
	primFail, primFail, primFail, primFail,
	primFail, primFail, primFail, primFail,
	primFail, primFail, primFail, primFail,
	primFail, primFail, primFail, primFail,
	primFail, primFail, primFail, prBoot
};



/*
	Arithmetic selectors dispatch table
*/

BOOL (*ArithMsg[])()= {
	prAddSI, prSubSI, prLTSI, prGTSI, prLESI, prGESI, prEQSI, prNESI,
	prMultSI, prDivideSI, prModSI, prMkPtSI, prBitShSI, prDivSI,
	prAndSI, prOrSI
};



					/*ST!Interpreter!primitiveFail!*/
static BOOL primFail()
{
	return TRUE;
}
