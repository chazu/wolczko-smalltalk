/*
  	Smalltalk-80 Virtual Machine:
	Bytecode routines

	Copyright (c) 1984,1985,1986,1987,1988,1989 Mario Wolczko
	All rights reserved.

This module contains the code for the detailed execution of
bytecodes.  A particular bytecode is executed by a call to an
entry in the dispatch table, which contains the address of the
function required to execute that bytecode.

$Log: BC.c,v $
 * Revision 5.0  1994/08/16  00:57:38  mario
 * More tracing stuff
 *
 * Revision 4.2  91/06/24  19:00:24  mario
 * Made change to new snapshot format (X11) for Generic I/O.
 * Added extra trace field to message sends (current rcvr).
 * 
 * Revision 4.1  89/06/21  19:44:50  mario
 * Added declarations and casts to keep lint quiet.
 * 
 * Revision 3.5  89/05/03  15:48:35  mario
 * Added more tacing features (jump, sends, returns).
 * 
 * Revision 3.4  87/05/19  17:41:35  miw
 * Added selective tracing of method contexts.
 * 
 * Revision 3.3  87/03/21  14:36:16  miw
 * Added simulation mode.
 * 
 * Revision 3.2  87/03/11  17:11:37  miw
 * Fixed trivial typing error; stable with 3.1
 * 
 * Revision 3.1  87/03/11  14:28:38  miw
 * 32-bit version without D-B.
 * 
 * Revision 2.10  87/02/13  19:40:27  miw
 * Converted to 32-bit---changed a size() call to a Wlen() call.
 * 
 * Revision 2.7  86/01/23  22:04:03  miw
 * Fixed a bug in message send reference counting.
 * 
 * Revision 2.6  86/01/17  19:03:37  miw
 * Added casts to void where necessary.
 * 
 * Revision 2.4  85/12/12  10:36:49  miw
 * Added direct pointers to current instruction and top of stack, and code
 * to make use of them.
 * 
 * Revision 2.3  85/12/07  15:54:28  miw
 * Altered the hash() macro because the oop tag bit moved.
 * 
 * Revision 2.2  85/12/04  20:13:53  miw
 * Increased method cache size and improved cache access code.
 * 
 * Revision 2.1  85/11/19  17:36:33  miw
 * Stabilised with all other modules at Rev 2.1
 * 
 * Revision 1.4  85/09/13  10:49:35  miw
 * Change calls so that pointers to active and home contexts are used
 * more effectively.
 * 
 * Revision 1.3  85/07/05  12:51:20  miw
 * Removed MSG_, BC_ and CALL_TRACE stuff.
 * Removed redundant ref.counting from pushes and stores of NIL,etc.
 * 
 * Revision 1.2  85/01/14  17:58:05  miw
 * 
 * Revision 1.1  85/01/10  12:50:56  miw
 * Initial revision
 * 
*/

static char *rcsid= "$Header: /home/mario/st/RCS/BC.c,v 5.0 1994/08/16 00:57:38 mario Exp mario $";

#include <stdio.h>
#include <string.h>
#include "ST.h"

#include "OM.h"
#include "OMMacros.h"
#include "CtxMacros.h"

#include "Regs.h"
#include "StdPtrs.h"
#include "ClassPtrs.h"
#include "Selectors.h"



/* common abbreviations:
	p       push
	R       Receiver
	Var     Variable
	T       Temporary
	L       Literal
	pps     pop and store
*/


					/*ST!Interpreter!sendSelector:argumentCount:!*/
/* this is required in a number of places, but logically it goes in the
   Send Bytecode section */

#define sendSelector(sel, argc)                                         \
{       	extern void sndSelToClass();				\
		OOP newRcvr;                                            \
		msgSelector= (sel);                                     \
		argCount= (argc);                                       \
		newRcvr= stackVal(argCount);                            \
		sndSelToClass(fetchClass(newRcvr));                     \
	}


/***************** STACK BYTECODES ****************/

					/*ST!Interpreter!stackBytecode!*/
					/*ST!Interpreter!pushReceiverVariableBytecode!*/
					/*ST!Interpreter!pushReceiverVariable:!*/
/* push Receiver Variable */
void pRVar()
	{ push(fetchPtr(currentBC & 15, receiver)); }

					/*ST!Interpreter!pushTemporaryVariableBytecode!*/
					/*ST!Interpreter!pushTemporaryVariable:!*/
/* push Temporary Variable */
void pTVar()
	{ push(temp(currentBC & 15)); }

					/*ST!Interpreter!pushLiteralConstantBytecode!*/
					/*ST!Interpreter!pushLiteralConstant:!*/
/* push Literal Constant */
void pLConst()
	{ push(literal(currentBC & 31)); }

					/*ST!Interpreter!pushLiteralVariableBytecode!*/
					/*ST!Interpreter!pushLiteralVariable:!*/
/* push Literal Variable */
void pLVar()
	{ push(fetchPtr(VALUE, literal(currentBC & 31))); }

					/*ST!Interpreter!storeAndPopReceiverVariableBytecode!*/
/* pop and store Receiver Variable */
void ppsRVar()
	{ storePtr(currentBC & 7, receiver, popStack); }

					/*ST!Interpreter!storeAndPopTemporaryVariableBytecode!*/
/* pop and store Temporary Variable */
void ppsTVar()
	{ storePtr((currentBC & 7) + TEMP_FR_START, homeContext, popStack); }

/* push Receiver, True, False, Nil, -1, 0, 1, 2 */

					/*ST!Interpreter!pushReceiverBytecode!*/
void pRcvr()         { push(receiver); }

					/*ST!Interpreter!pushConstantBytecode!*/
void pTrue()         { NRpush(TRUE_PTR); }
void pFalse()        { NRpush(FALSE_PTR); }
void pNil()          { NRpush(NIL_PTR); }
void pM1()           { NRpush(MINUS_ONE_PTR); }
void p0()            { NRpush(ZERO_PTR); }
void p1()            { NRpush(ONE_PTR); }
void p2()            { NRpush(TWO_PTR); }


					/*ST!Interpreter!extendedPushBytecode!*/
void ePush()
{
	BYTE desc= nextByte;

	switch (desc & 0xC0) {
	case 0:
		push(fetchPtr((desc & 0x3F), receiver));
		break;
	case 0x40:
		push(temp(desc & 0x3F));
		break;
	case 0x80:
		push(literal(desc & 0x3F));
		break;
	case 0xC0:
		push(fetchPtr(VALUE, literal(desc & 0x3F)));
		break;
	}
}


					/*ST!Interpreter!extendedStoreBytecode!*/
void eStore()
{
	extern void BCFault();
	BYTE desc= nextByte;

	switch (desc & 0xC0) {
	case 0:
		storePtr(desc & 0x3F, receiver, stackTop);
		break;
	case 0x40:
		storePtr((desc & 0x3F) + TEMP_FR_START, homeContext, stackTop);
		break;
	case 0x80:
		BCFault();
		break;
	case 0xC0:
		storePtr(VALUE, literal(desc & 0x3F), stackTop);
		break;
	}
}


					/*ST!Interpreter!extendedStoreAndPopBytecode!*/
void ePopStore()
	{ eStore(); pop(1); }


					/*ST!Interpreter!popStackBytecode!*/
void popST()
	{ pop(1); }

					/*ST!Interpreter!duplicateTopBytecode!*/
/* duplicate Stack Top - BEWARE of side effects if you alter the stack macros */
void dupST()
	{ push(stackTop); }

					/*ST!Interpreter!pushActiveContextBytecode!*/
void pushActCtx()
	{ push(activeContext); }



/**************** JUMP BYTECODES ****************/


					/*ST!Interpreter!jumpBytecode!*/
					/*ST!Interpreter!jump:!*/
#ifndef SIM
#define jump(offset)            { ip += (offset); }
#else SIM
static void jump(offset)
int offset;
{
	extern void trace_jump();

	trace_jump(currentBC, TRUE, method, ip - (BYTE *)ABSmethod, offset);
	ip += (offset);
}
#endif
					/*ST!Interpreter!shortUnconditionalJump!*/
void SUJump()
	{ jump((currentBC & 7) + 1); }


					/*ST!Interpreter!jumplf:by:!*/
					/*ST!Interpreter!sendMustBeBoolean!*/
       /* assume that cond is one of TRUE_PTR, FALSE_PTR, and notcond is its inverse */
#ifndef SIM
#define jumplf(cond, notcond, offset)    {                                      \
				  OOP bool= popStack;                           \
				  if (bool == (cond)) {                         \
					jump(offset);                           \
				  } else if (bool != (notcond)) {		\
					unPop(1);                               \
					sendSelector(MUST_BE_BOOLEAN, 0);       \
				  }                                             \
				}
#else SIM
static void jumplf(cond, notcond, offset)
OOP cond, notcond;
int offset;
{
	extern void trace_jump();
	OOP bool= popStack;
	if (bool == cond) {
		jump(offset);
	} else if (bool != notcond) {
		unPop(1);
		sendSelector(MUST_BE_BOOLEAN, 0);
	} else {
		trace_jump(currentBC, FALSE, method,
			   ip - (BYTE *)ABSmethod, offset);
	}
}
#endif SIM

					/*ST!Interpreter!shortConditionalJump!*/
void pJumpFalse()
	{  jumplf(FALSE_PTR, TRUE_PTR, (currentBC & 7) + 1); }

					/*ST!Interpreter!longUnconditionalJump!*/
/* extended Unconditional Jump */
void eJump()
	{  int offset= 256*((currentBC & 7) - 4) + nextByte;
	   jump(offset); }

					/*ST!Interpreter!longConditionalJump!*/

/* extended Jump on True */
void eJumpTrue()
{
	WORD nb= nextByte;     /* must be here because of side effect */

	jumplf(TRUE_PTR, FALSE_PTR, 256*(currentBC & 3) + nb);
}

/* extended Jump on False */
void eJumpFalse()
{
	WORD nb= nextByte;     /* must be here because of side effect */

	jumplf(FALSE_PTR, TRUE_PTR, 256*(currentBC & 3) + nb);
}


/**************** SEND BYTECODES ****************/

					/*ST!Interpreter!sendBytecode!*/
					/*ST!Interpreter!extendedSendBytecode!*/
					/*ST!Interpreter!singleExtendedSendBytecode!*/
void esLit()                 /* extended send of literal*/
{
	BYTE desc= nextByte;
	sendSelector(literal(desc & 31), desc>>5);
}

					/*ST!Interpreter!doubleExtendedSendBytecode!*/
void eseLit()                /* extended send of extended literal */
{
	BYTE Count= nextByte;
	sendSelector(literal(nextByte), Count);
}

					/*ST!Interpreter!singleExtendedSuperBytecode!*/
void eSsLit()                /* extended superclass send of literal */
{
	extern void sndSelToClass();
	BYTE desc= nextByte;
	argCount= desc >> 5;
	msgSelector= literal(desc & 31);
	sndSelToClass(superclass(methodClass));
}

					/*ST!Interpreter!doubleExtendedSuperBytecode!*/
void eSseLit()               /* extended superclass send of extended literal */
{
	extern void sndSelToClass();
	argCount= nextByte;
	msgSelector= literal(nextByte);
	sndSelToClass(superclass(methodClass));
}


/* table which indexes into arithmetic primitives */
extern BOOL (*ArithMsg[])();


					/*ST!Interpreter!sendSpecialSelectorBytecode!*/

					/*ST!Interpreter!specialSelectorPrimitiveResponse!*/

					/*ST!Interpreter!arithmeticSelectorPrimitive!*/
void sArithMsg()             /* send an arithmetic message */
{
	WORD selIndex= (currentBC - 176);

	if ((*ArithMsg[selIndex])()) {          /* primitives do their own checks */
		selIndex += selIndex;
		sendSelector(fetchPtr(selIndex, SPECIAL_SELECTORS),
			ffetchInt(selIndex + 1, SPECIAL_SELECTORS));
	}
}



					/*ST!Interpreter!commonSelectorPrimitive!*/

extern BOOL prEquiv(), prClass(), prBlockCopy(), prValue();

void sCommonMsg()            /* send a special message */
{
	OOP rcvr, rcvrClass;
	WORD selIndex= (currentBC - 176)*2;

	/* special selector primitive response */
	argCount= ffetchInt(selIndex + 1, SPECIAL_SELECTORS);
	switch (currentBC) {
	case 198:
		(void)prEquiv();              /* never fails */
		return;
	case 199:
		(void)prClass();              /* never fails */
		return;
	case 200:
		rcvr= stackVal(argCount);
		rcvrClass= fetchClass(rcvr);
		if (rcvrClass == CLASS_METH_CTX
		    ||  rcvrClass == CLASS_BLOCK_CTX) {
			(void)prBlockCopy();          /* never fails */
			return;
		}
		break;
	case 201:
	case 202:
		rcvr= stackVal(argCount);
		if (fetchClass(rcvr) == CLASS_BLOCK_CTX && !prValue())
			return;
		break;
	default:
		/* not implemented */
		break;
	}

	/* primitive failed */

	sendSelector(fetchPtr(selIndex, SPECIAL_SELECTORS),
		     ffetchInt(selIndex + 1, SPECIAL_SELECTORS));
}

					/*ST!Interpreter!sendLiteralSelectorBytecode!*/

void sLit0Args()             /* send literal selector, 0 arguments */
	{ sendSelector(literal(currentBC & 15), 0); }

void sLit1Arg()              /* send literal selector, 1 argument */
	{ sendSelector(literal(currentBC & 15), 1); }

void sLit2Args()             /* send literal selector, 2 arguments */
	{ sendSelector(literal(currentBC & 15), 2); }


/* here is the method cache, functionally identical to that in G/R */

#define CACHE_SIZE 1024

static struct CacheEntry{
	OOP     Selector;
	OOP     Class;
	OOP     Method;
	BYTE    primIndex;
} MCache[CACHE_SIZE];


					/*ST!Interpreter!initializeMethodCache!*/
					/*ST!Interpreter!primitiveFlushCache!*/

BOOL prFlushCache()
{
	WORD i;

	for (i= 0;  i < CACHE_SIZE; )
		MCache[i++].Selector= NIL_PTR;
	return FALSE;
}



#ifdef SIM
OOP currClass;			/* the class of the new receiver */
#endif SIM

extern BOOL (*dispPrim[])();            /* the  primitive dispatch table */

					/*ST!Interpreter!sendSelectorToClass:!*/
void sndSelToClass(Class)
OOP Class;
{
	extern void executeNewMethod();
	{
					/*ST!Interpreter!findNewMethodInClass:!*/
		/* see Bits of History, p.244 for an explanation of this hash function */
		WORD Hash= (msgSelector ^ Class) & (CACHE_SIZE - 1);
		struct CacheEntry *ce= &MCache[Hash];

#ifdef SIM
		currClass= Class;
#endif SIM

		if (ce->Selector == msgSelector  &&  ce->Class == Class) { /* hit */
			newMethod= ce->Method;
			primIndex= ce->primIndex;
		} else {
			extern void lookupMethodInClass();
			lookupMethodInClass(Class);
			ce->Selector  = msgSelector;
			ce->Class     = Class;
			ce->Method    = newMethod;
			ce->primIndex = primIndex;
		}
	}

	executeNewMethod();
}



#ifdef SIM

		/* estimated no. of temps for each prim */
static BYTE prim_temp_count[256]= {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2,	/* 48-63 */
	2, 6, 6, 4, 1, 1, 3, 4, 2, 3, 3, 0, 0, 1, 1, 1,	/* 64-79 */
	4, 3, 3, 4, 6, 2, 3, 4, 1 /* rest are 0 */	/* 80-95 */
};

static BOOL isPrimFail= FALSE;	/* to tell activeNewMethod not to emit
				   send trace */

#endif


					/*ST!Interpreter!executeNewMethod!*/
					/*ST!Interpreter!primitiveResponse!*/
					/*ST!Interpreter!quickReturnSelf!*/
					/*ST!Interpreter!quickInstanceLoad!*/
					/*ST!Interpreter!initPrimitive!*/
					/*ST!Interpreter!dispatchPrimitives!*/
					/*ST!Interpreter!success!*/
					/*ST!Interpreter!success:!*/
void executeNewMethod()
{ 
	static void trace_send(), trace_ret(), save_send(), trace_saved_send();
	extern void activateNewMethod();

	/* primitive Response ? */
	if (primIndex == 0) { 
		OOP result;
		WORD flag= flagValue(newMethod); 
		if (flag != 5) {
			if (flag == 6) { /* quick instance load */ 
#ifdef SIM
				trace_send(currentBC, currClass, msgSelector,
					   method, activeContext,
					   ip - (BYTE *)ABSmethod,
					   stackVal(argCount), argCount, 0);
#endif
				result= fetchPtr(fieldIndex(newMethod), popStack);
				push(result); 
#ifdef SIM
				trace_ret(124, /* 124= return stack top BC */
					  newMethod, 0, /* dummy context oop */
					  0, /* ip of 0 (what else?) */
					  result);
#endif
			} else {
				activateNewMethod();
			}
		}
#ifdef SIM
		  else {	/* quick return self */
			  result= stackVal(argCount);
			  trace_send(currentBC, currClass, msgSelector,
				     method, activeContext,
				     ip - (BYTE *)ABSmethod,
				     result, argCount, 0);
			  trace_ret(120, /* 120= return receiver BC */
				    newMethod, 0, 0, result);
		  }
#endif SIM			
	} else {
#ifdef SIM
		BOOL performOrVal= primIndex >= 81 && primIndex <= 84;

		if (!performOrVal) {
			/* don't trace value: or perform: primitives */
			trace_send(currentBC, currClass, msgSelector,
				   method, activeContext,
				   ip - (BYTE *)ABSmethod,
				   stackVal(argCount), argCount,
				   prim_temp_count[primIndex]);
		}

#endif SIM
		if ((*dispPrim[primIndex])()) { /* dispatchPrimitives */ 
			/* primitive failure */ 
#ifdef SIM
			isPrimFail= TRUE;
#endif SIM
			activateNewMethod();
#ifdef SIM
			isPrimFail= FALSE;
#endif SIM
		} 
#ifdef SIM
		else if (!performOrVal) {
			/* primitive succeeded---simulate a return
			 * if not a value or perform prim */
			trace_ret(120, /* 120= return receiver BC */
				  newMethod, 0, 0, stackTop);
		}
#endif SIM
	}
}


					/*ST!Interpreter!activateNewMethod!*/
void activateNewMethod()
{
	extern void trace_send();
	OOP hdr= header(newMethod);
	WORD ctxSize= HdrLargeCtx(hdr) ?
	  		(TEMP_FR_START + 32)
			  : (TEMP_FR_START + 12);
	OOP newCtx= instPtrs(CLASS_METH_CTX, ctxSize);
#ifndef SIM
        CONTEXT *ABSnewCtx= (CONTEXT *)addrOfOop(newCtx);
        CtxStorePtr(FSENDER_CALLER, ABSnewCtx, activeContext);
        AstrIPval(HdrInitIPofMeth(hdr), ABSnewCtx);
        AstrSPval(HdrTempCntOf(hdr), ABSnewCtx);
        CtxStorePtr(FMETHOD_BLOCK_ARGC, ABSnewCtx, newMethod);
#else SIM
        CtxStorePtr(SENDER, newCtx, activeContext);
        AstrIPval(HdrInitIPofMeth(hdr), newCtx);
        AstrSPval(HdrTempCntOf(hdr), newCtx);
	if (!isPrimFail)
	  trace_send(currentBC, currClass, msgSelector, method,
		     activeContext, ip - (BYTE *)ABSmethod,
		     stackVal(argCount), argCount, HdrTempCntOf(hdr));
        CtxStorePtr(METHOD, newCtx, newMethod);
#endif SIM
	transfer(argCount + 1, sp - (WORD*)ac - argCount,
		 activeContext, RECEIVER, newCtx);
	pop(argCount + 1);

	newActiveCtx(newCtx);
}

					/*ST!Interpreter!hash:!*/
#define hash(oop)       (oop)



static void errorDoesNotUnderstand()
{
	fatal("Recursive not understood error ... SPLAT!");
}


					/*ST!Interpreter!lookupMethodInClass:!*/
void lookupMethodInClass(Class)
OOP Class;
{
	OOP current;

#ifdef SIM
	extern void trace();

	trace('l',Class,fetchClass(stackVal(argCount)));
#endif

 start:

	for (current= Class;  current != NIL_PTR;  current= superclass(current)) {
		OOP dict= fetchPtr(MSG_DICT, current);

					/*ST!Interpreter!lookupMethodInDictionary:!*/

		/* lookup method in dictionary */
		WORD length= Wlen(dict);
		WORD index= ((length - SEL_START - 1) & hash(msgSelector))
		  		+ SEL_START;
		BOOL wrap= FALSE;

		for (;;) {
			OOP nextSel= fetchPtr(index, dict);
			if (nextSel == NIL_PTR)         /* not found */
				break;
			if (nextSel == msgSelector) {   /* found */
				newMethod= fetchPtr(index - SEL_START,
						    fetchPtr(METH_ARRAY, dict));
				primIndex= primitiveIndex(newMethod);
#ifdef SIM
				trace('L',newMethod,stackVal(argCount));
#endif
				return;
			}
			if (++index == length) {
				if (wrap) break;        /* not found */
				wrap= TRUE;
				index= SEL_START;
			}
		}
	}

	/* message not found in current dictionary */
	if (msgSelector == DOES_NOT_UNDERSTAND) {
		extern void errorDoesNotUnderstand();
		errorDoesNotUnderstand();
	}

	log("Message not understood");

	/* create message */
					/*ST!Interpreter!createActualMessage!*/
	{
		OOP argArray= instPtrs(CLASS_ARRAY, argCount);
		OOP msg= instPtrs(CLASS_MSG, MSG_SIZE);
		storePtr(MSG_SELECTOR, msg, msgSelector);
		storePtr(MSG_ARGS, msg, argArray);
		transfer(argCount, sp - (WORD *)ac - (argCount - 1),
			 activeContext, 0, argArray);
		pop(argCount);
		push(msg);
		argCount= 1;
	}

	msgSelector= DOES_NOT_UNDERSTAND;
	/* lookupMethodInClass(Class);  */
	goto start;
}


/**************** RETURN BYTECODES ****************/

#define null(x)

					/*ST!Interpreter!returnValue:to:!*/

#ifndef SIM
#define retVal(res, ctx, pushFN, C_UpFN, C_DownFN)                      \
	{                                                               \
		OOP res1= (res),  ctx1= (ctx);                          \
		if (ctx1 == NIL_PTR || fetchPtr(INSTR_PTR, ctx1) == NIL_PTR) { \
			push(activeContext);                            \
			pushFN(res1);                                   \
			sendSelector(CANNOT_RETURN, 1);                 \
		}                                                       \
		C_UpFN(res1);                                         	\
				/*ST!Interpreter!returnToActiveContext:!*/\
		c_up(ctx1);                                             \
				/*ST!Interpreter!nilContextFields!*/    \
		NRsac(FSENDER_CALLER, NIL_PTR);             		\
		NR2sac(FINSTR_PTR, NIL_PTR);          			\
									\
		c_down(activeContext);                                  \
		activeContext= ctx1;                                    \
		cacheActiveContext;					\
		fetchCtxRegs;                                           \
									\
		pushFN(res1);                                           \
		C_DownFN(res1);                                         \
	}
#else SIM
#define retVal(res, ctx, pushFN, C_UpFN, C_DownFN)                      \
	{                                                               \
		static void trace_ret(), trace_ret2();			\
		OOP res1= (res),  ctx1= (ctx);                          \
		if (ctx1 == NIL_PTR || fetchPtr(INSTR_PTR, ctx1) == NIL_PTR) { \
			push(activeContext);                            \
			pushFN(res1);                                   \
			sendSelector(CANNOT_RETURN, 1);                 \
		}                                                       \
		trace_ret(currentBC, method, activeContext,		\
			  ip - (BYTE *)ABSmethod, res1);		\
		C_UpFN(res1);                                         	\
				/*ST!Interpreter!returnToActiveContext:!*/\
		c_up(ctx1);                                             \
				/*ST!Interpreter!nilContextFields!*/    \
		NRsac(SENDER, NIL_PTR);             		        \
		NR2sac(INSTR_PTR, NIL_PTR);          			\
									\
		c_down(activeContext);                                  \
		activeContext= ctx1;                                    \
		cacheActiveContext;					\
		fetchCtxRegs;                                           \
									\
		pushFN(res1);                                           \
		C_DownFN(res1);                                         \
		trace_ret2();						\
	}
#endif SIM


					/*ST!Interpreter!returnBytecode!*/

/* return Receiver, True, False, Nil */

void rRcvr()         { retVal(receiver, sender, push, countUp, countDown); }
void rTrue()         { retVal(TRUE_PTR, sender, NRpush, null, null); }
void rFalse()        { retVal(FALSE_PTR, sender, NRpush, null, null); }
void rNil()          { retVal(NIL_PTR, sender, NRpush, null, null); }

/* return Stack Top from Message, Block */

void rSTMessage()    { retVal(popStack, sender, push, countUp, countDown); }
void rSTBlock()      { retVal(popStack, caller, push, countUp, countDown); }



void BCFault()
{
	fatal("Illegal bytecode encountered ... SPLAT!");
}


/* the dispatch table */

void (*dispTable[])()= {
	pRVar, pRVar, pRVar, pRVar, pRVar, pRVar, pRVar, pRVar,
	pRVar, pRVar, pRVar, pRVar, pRVar, pRVar, pRVar, pRVar,
	pTVar, pTVar, pTVar, pTVar, pTVar, pTVar, pTVar, pTVar,
	pTVar, pTVar, pTVar, pTVar, pTVar, pTVar, pTVar, pTVar,
	pLConst, pLConst, pLConst, pLConst, pLConst, pLConst, pLConst, pLConst,
	pLConst, pLConst, pLConst, pLConst, pLConst, pLConst, pLConst, pLConst,
	pLConst, pLConst, pLConst, pLConst, pLConst, pLConst, pLConst, pLConst,
	pLConst, pLConst, pLConst, pLConst, pLConst, pLConst, pLConst, pLConst,
	pLVar, pLVar, pLVar, pLVar, pLVar, pLVar, pLVar, pLVar,
	pLVar, pLVar, pLVar, pLVar, pLVar, pLVar, pLVar, pLVar,
	pLVar, pLVar, pLVar, pLVar, pLVar, pLVar, pLVar, pLVar,
	pLVar, pLVar, pLVar, pLVar, pLVar, pLVar, pLVar, pLVar,
	ppsRVar, ppsRVar, ppsRVar, ppsRVar, ppsRVar, ppsRVar, ppsRVar, ppsRVar,
	ppsTVar, ppsTVar, ppsTVar, ppsTVar, ppsTVar, ppsTVar, ppsTVar, ppsTVar,
	pRcvr, pTrue, pFalse, pNil, pM1, p0, p1, p2,
	rRcvr, rTrue, rFalse, rNil,
	rSTMessage, rSTBlock,
	BCFault, BCFault,
	ePush,
	eStore,
	ePopStore,
	esLit, eseLit, eSsLit, eSseLit,
	popST, dupST,
	pushActCtx,
	BCFault, BCFault, BCFault, BCFault, BCFault, BCFault,
	SUJump, SUJump, SUJump, SUJump, SUJump, SUJump, SUJump, SUJump, 
	pJumpFalse, pJumpFalse, pJumpFalse, pJumpFalse, 
	pJumpFalse, pJumpFalse, pJumpFalse, pJumpFalse,
	eJump, eJump, eJump, eJump, eJump, eJump, eJump, eJump, 
	eJumpTrue, eJumpTrue, eJumpTrue, eJumpTrue, 
	eJumpFalse, eJumpFalse, eJumpFalse, eJumpFalse, 
	sArithMsg, sArithMsg, sArithMsg, sArithMsg, 
	sArithMsg, sArithMsg, sArithMsg, sArithMsg, 
	sArithMsg, sArithMsg, sArithMsg, sArithMsg, 
	sArithMsg, sArithMsg, sArithMsg, sArithMsg, 
	sCommonMsg, sCommonMsg, sCommonMsg, sCommonMsg,
	sCommonMsg, sCommonMsg, sCommonMsg, sCommonMsg,
	sCommonMsg, sCommonMsg, sCommonMsg, sCommonMsg,
	sCommonMsg, sCommonMsg, sCommonMsg, sCommonMsg,
	sLit0Args, sLit0Args, sLit0Args, sLit0Args,
	sLit0Args, sLit0Args, sLit0Args, sLit0Args,
	sLit0Args, sLit0Args, sLit0Args, sLit0Args,
	sLit0Args, sLit0Args, sLit0Args, sLit0Args,
	sLit1Arg, sLit1Arg, sLit1Arg, sLit1Arg, 
	sLit1Arg, sLit1Arg, sLit1Arg, sLit1Arg, 
	sLit1Arg, sLit1Arg, sLit1Arg, sLit1Arg, 
	sLit1Arg, sLit1Arg, sLit1Arg, sLit1Arg, 
	sLit2Args, sLit2Args, sLit2Args, sLit2Args, 
	sLit2Args, sLit2Args, sLit2Args, sLit2Args, 
	sLit2Args, sLit2Args, sLit2Args, sLit2Args, 
	sLit2Args, sLit2Args, sLit2Args, sLit2Args
};


#ifdef SIM

/*****************tracing routines for simulation ******************/


BYTE _nextByte()
{
	extern void trace();
	trace('x',method,(WORD)(ip - (BYTE *)ABSmethod)/sizeof(WORD));
	return *ip++;
}

extern char *trace_opts, *index();

static void trace_jump(bytecode, taken, method, ip, offset)
BYTE bytecode;
BOOL taken;
OOP method;
int ip;
int offset;
{
	if (index(trace_opts, 'j') == NULL)
		return;

	(void)putchar('j');
	(void)putchar((char)bytecode);
	(void)putchar((char)taken);
	(void)putw((int)method, stdout);
	(void)putw(ip, stdout);
	(void)putw(offset, stdout);
}

static void trace_send(bytecode, Class, sel, meth, ctx, offset,
		       newRcvr, argc, tempc)
BYTE bytecode;
OOP Class,sel,meth,newRcvr,ctx;
int offset;
WORD argc,tempc;
{
	if (index(trace_opts, 'm') != NULL) {
		(void)putchar('m');
		(void)putchar((char)bytecode);
		(void)putw((int)Class, stdout);
		(void)putw((int)sel, stdout);
		(void)putw((int)meth, stdout);
		(void)putw((int)ctx, stdout);
		(void)putw(offset, stdout);
		(void)putw((int)newRcvr, stdout);
		(void)putw((int)receiver, stdout);
		(void)putchar((char)argc);
		(void)putchar((char)tempc);
	}
	if (index(trace_opts, 'a') != NULL) {
		int n=argc;
		(void)putchar('a');
		(void)putchar((char)argc);
		(void)putw((int)fetchClass(receiver), stdout);
		(void)putw((int)Class, stdout);
		while (n--)
		  (void)putw((int)fetchClass(stackVal(n)),stdout);
	}

}

static BYTE s_bytecode;
static OOP s_Class, s_sel, s_meth, s_newRcvr, s_ctx;
static int s_offset;
static WORD s_argc, s_tempc;

static void save_send(bytecode, Class, sel, meth, ctx, offset,
		       newRcvr, argc, tempc)
BYTE bytecode;
OOP Class,sel,meth,newRcvr,ctx;
int offset;
WORD argc,tempc;
{
	s_bytecode= bytecode;
	s_Class= Class;
	s_sel= sel;
	s_meth= meth;
	s_newRcvr= newRcvr;
	s_ctx= ctx;
	s_offset= offset;
	s_argc= argc;
	s_tempc= tempc;
}

static void trace_saved_send()
{
	trace_send(s_bytecode, s_Class, s_sel, s_meth, s_ctx, s_offset,
		   s_newRcvr, s_argc, s_tempc);
}

void trace_new_blk_ctx(method, actctx, offset, blkctx, arg_count)
OOP method, actctx, blkctx;
int offset;
WORD arg_count;
{
	if (index(trace_opts, 'b') != NULL) {
		(void)putchar('b');
		(void)putw((int)method, stdout);
		(void)putw((int)actctx, stdout);
		(void)putw(offset, stdout);
		(void)putw((int)blkctx, stdout);
		(void)putchar((char)arg_count);
	}
}

static void trace_ret(bytecode, method, actctx, offset, result)
BYTE bytecode;
OOP method, actctx, result;
int offset;
{
	if (index(trace_opts, 'M') != NULL) {
		(void)putchar('M');
		(void)putchar((char)bytecode);
		(void)putw((int)method, stdout);
		(void)putw((int)actctx, stdout);
		(void)putw(offset, stdout);
	}
	if (index(trace_opts, 'n') != NULL) {
		(void)putchar('n');
		(void)putw((int)fetchClass(result), stdout);
		(void)putw((int)result==receiver, stdout);
	}
}

static void trace_ret2()
{
	if (index(trace_opts, 'B') != NULL) {
		(void)putchar('B');
		(void)putw((int)receiver, stdout);
		(void)putw((int)fetchClass(receiver), stdout);
	}
}

void trace_process_switch(proc, ctx)
OOP proc, ctx;
{
	if (index(trace_opts, 'p') != NULL) {
		(void)putchar('p');
		(void)putw((int)proc, stdout);
		(void)putw((int)ctx, stdout);
	}
}

#endif SIM
