/*
  	Smalltalk-80 Virtual Machine:
	Control Primitives

	Copyright (c) 1984,1985,1986,1987,1988,1989 Mario Wolczko
	All rights reserved.
	
$Log: ControlPrims.c,v $
 * Revision 5.0  1994/08/16  00:58:12  mario
 * Sync with others
 *
 * Revision 4.1  89/06/21  19:47:37  mario
 * Added declarations to keep lint quiet.
 * Stabilised with other files at 4.1.
 * 
 * Revision 3.2  89/05/03  15:51:10  mario
 * Added tracing of block evaluation.
 * 
 * Revision 3.1  87/03/11  15:05:42  miw
 * 32-bit version--no D-B.
 * 
 * Revision 2.2  85/12/12  10:32:34  miw
 * Changed expressions involving stackPtr and instrPtr to ones involving
 * sp and ip.
 * 
 * Revision 2.1  85/11/19  17:38:19  miw
 * Stabilised with all other modules at Rev 2.1
 * 
 * Revision 1.2  85/07/05  12:55:47  miw
 * Removed MSG_TRACE code.
 * 
 * Revision 1.1  85/01/10  12:52:26  miw
 * Initial revision
 * 
*/

static char *rcsid= "$Header: /home/mario/st/RCS/ControlPrims.c,v 5.0 1994/08/16 00:58:12 mario Exp mario $";

#include <stdio.h>
#include "Prim.h"


extern BOOL (*dispPrim[])();

					/*ST!Interpreter!primitiveBlockCopy!*/
BOOL prBlockCopy()
{
	OOP blkArgc= popStack;
	OOP ctx= popStack;
	OOP methCtx= isBlockCtx(ctx) ? fetchPtr(HOME, ctx) : ctx;
	WORD len= Wlen(methCtx);
	OOP newCtx= instPtrs(CLASS_BLOCK_CTX, len);
	OOP initIP= intObj(ip + 3 - (BYTE *)ABSmethod);

	storePtr(INIT_IP, newCtx, initIP);
	storePtr(INSTR_PTR, newCtx, initIP);
	storeSPval(0, newCtx);
	storePtr(BLOCK_ARGC, newCtx, blkArgc);
	storePtr(HOME, newCtx, methCtx);
	push(newCtx);
	return FALSE;
}


					/*ST!Interpreter!primitiveValue!*/
BOOL prValue()
{
	OOP blkCtx= stackVal(argCount);

#ifdef SIM
	{ extern void trace_new_blk_ctx();
	  trace_new_blk_ctx(method, activeContext,
			  ip - (BYTE *)ABSmethod, blkCtx, argCount); }
#endif SIM

	if (argCount != argCOfBlock(blkCtx))
		return TRUE;
	transfer(argCount, sp - (WORD *)ac - argCount + 1,
		 activeContext, TEMP_FR_START, blkCtx);
	pop(argCount + 1);
	storePtr(INSTR_PTR, blkCtx, fetchPtr(INIT_IP, blkCtx));
	storeSPval(argCount, blkCtx);
	storePtr(CALLER, blkCtx, activeContext);
	newActiveCtx(blkCtx);
	return FALSE;
}


					/*ST!Interpreter!primitiveValueWithArgs!*/
BOOL prValWithArgs()
{
	OOP argArray= popStack;
	OOP blkCtx= popStack;
	WORD blkArgc= argCOfBlock(blkCtx);

#ifdef SIM
	{ extern void trace_new_blk_ctx();
	  trace_new_blk_ctx(method, activeContext,
			    ip - (BYTE *)ABSmethod, blkCtx, blkArgc); }
#endif SIM

	if (fetchClass(argArray) != CLASS_ARRAY || Wlen(argArray) != blkArgc) {
		unPop(2);
		return TRUE;
	}
	transfer(blkArgc, 0, argArray, TEMP_FR_START, blkCtx);
	storePtr(INSTR_PTR, blkCtx, fetchPtr(INIT_IP, blkCtx));
	storeSPval(blkArgc, blkCtx);
	storePtr(CALLER, blkCtx, activeContext);
	newActiveCtx(blkCtx);
	return FALSE;
}


					/*ST!Interpreter!primitivePerform!*/
BOOL prPerform()
{
	extern void lookupMethodInClass(), executeNewMethod();
	WORD argCOfNewMeth, selIndex;
	OOP perfSel= msgSelector;
	OOP newRcvr= stackVal(argCount);

	msgSelector= stackVal(argCount - 1);
	lookupMethodInClass(fetchClass(newRcvr));
	argCnt(newMethod, argCOfNewMeth);
	if (argCOfNewMeth != argCount - 1) {
		msgSelector= perfSel;
		return TRUE;
	}
	selIndex= sp - (WORD *)ac - --argCount;
	transfer(argCount, selIndex + 1, activeContext,
		 selIndex, activeContext);
	pop(1);
	executeNewMethod();
	return FALSE;
}



					/*ST!Interpreter!primitivePerformWithArgs!*/
BOOL prPerWithArgs()
{
	extern void lookupMethodInClass(), executeNewMethod();
	WORD index, argCOfNewMeth;
	OOP perfSel, rcvr;
	OOP argArray= popStack;
	WORD arraySize= Wlen(argArray);

	if (fetchClass(argArray) != CLASS_ARRAY
	   || sp - (WORD *)ac + arraySize >= Wlen(activeContext)) {
		unPop(1);
		return TRUE;
	}
	perfSel= msgSelector;
	msgSelector= popStack;
	rcvr= stackTop;
	argCount= arraySize;
	for (index= 0; index < argCount; ++index)
		push(fetchPtr(index, argArray));
	lookupMethodInClass(fetchClass(rcvr));
	argCnt(newMethod, argCOfNewMeth);
	if (argCOfNewMeth != argCount) {
		unPop(argCount);
		push(msgSelector);
		push(argArray);
		argCount= 2;
		msgSelector= perfSel;
		return TRUE;
	} else {
		executeNewMethod();
		return FALSE;
	}
}
