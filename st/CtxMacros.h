/*
  	Smalltalk-80 Virtual Machine:
	Macros for access to Contexts

	Copyright (c) 1984,1985,1986,1987,1988,1989 Mario Wolczko
	All rights reserved.

$Header: /home/mario/st/RCS/CtxMacros.h,v 5.0 1994/08/16 00:58:16 mario Exp mario $
$Log: CtxMacros.h,v $
 * Revision 5.0  1994/08/16  00:58:16  mario
 * Sync with others
 *
 * Revision 4.1  89/06/21  19:48:19  mario
 * Stabilised with other files at 4.1.
 * 
 * Revision 3.3  87/03/21  14:37:45  miw
 * Added simulation code.
 * 
 * Revision 3.2  87/03/13  10:27:15  miw
 * Replaced asIsMethCtx macro.
 * 
 * Revision 3.1  87/03/11  15:09:20  miw
 * 32-bit versio: no D-B.
 * 
 * Revision 2.7  87/02/13  20:08:29  miw
 * Converted to 32-bit: Changed calc of init IP of meth.
 * Fixed a bug: now check whether rcvr is an int before caching a pointer to
 * its body.
 * 
 * Revision 2.6  86/05/07  19:11:40  miw
 * Added nilOutTemps macro.
 * 
 * Revision 2.3  85/12/12  10:34:02  miw
 * Added direct pointers to the current method, receiver, instruction and
 * top of stack.  Created specialised versions of some macros (header,
 * litMeth,...) to use these pointers, and improvement stack access macros
 * and bytecode fetch as well.
 * 
 * Revision 2.2  85/12/09  20:15:39  miw
 * Changed macros to accomodate the move of the SmallInteger tag bit.
 * 
 * Revision 2.1  85/11/19  17:38:26  miw
 * Stabilised with all other modules at Rev 2.1
 * 
 * Revision 1.3  85/09/13  10:52:35  miw
 * Added new macros to extract fields from method headers.
 * Added structure casts for contexts.
 * Added pointers to the home and active contexts, and macros to access
 * their fields more efficiently.  Changed some existing macros to
 * use the pointers.
 * 
 * Revision 1.2  85/07/05  12:56:54  miw
 * Added the NRpush() macro.
 * 
 * Revision 1.1  85/01/10  12:52:44  miw
 * Initial revision
 * 
*/
					/*ST!Interpreter!initializeMethodIndices!*/
#define HEADER                  0
#define LITERAL_START           1

					/*ST!Interpreter!headerOf:!*/
#define header(m)		fetchPtr(HEADER, m)
#ifndef SIM
#define currHeader              (*ABSmethod)
#else SIM
#define currHeader              fetchPtr(HEADER, method)
#endif SIM

					/*ST!Interpreter!literal:ofMethod:!*/
#define litMeth(offset, m)	fetchPtr((offset) + LITERAL_START, (m))
#ifndef SIM
	/* fetch the literal with given offset from the CURRENT method */
#define currLitMeth(offset)     (ABSmethod[LITERAL_START + (offset)])
#else SIM
#define currLitMeth(offset)	litMeth(offset, method)
#endif SIM

					/*ST!Interpreter!temporaryCountOf:!*/
#define HdrTempCntOf(h)		(((h) & 0x0F80) >> 7)
#define tempCntOf(m)            HdrTempCntOf(header(m))

					/*ST!Interpreter!largeContextFlagOf:!*/
/* returns 0 or 0x80 */
#define HdrLargeCtx(h)		((h) & 0x40)
#define largeCtx(m)             HdrLargeCtx(header(m))

					/*ST!Interpreter!literalCountOfHeader:!*/
#define litCnHdr(hdr)           (hdr & 0x3F)

					/*ST!Interpreter!literalCountOf!*/
#define litCount(m)             (litCnHdr(header(m)))
#define currLitCount		(litCnHdr(currHeader))

					/*ST!Interpreter!flagValueOf:!*/
#define flagValue(m)            ((header(m) & 0x7000) >> 12)

					/*ST!Interpreter!fieldIndexOf:!*/
#define fieldIndex(m)           ((header(m) & 0x0F80) >> 7)

					/*ST!Interpreter!headerExtensionOf:!*/
	/* BEWARE of side effects */
#define hdrExt(m)		litMeth(litCount(m) - 2, (m))
#define currHdrExt         	currLitMeth(currLitCount - 2)

					/*ST!Interpreter!primitiveIndexOf:!*/
/* BEWARE of side effects when using this macro */
#define primitiveIndex(m)       (((header(m)&0x7000) == 0x7000) ? (hdrExt(m) & 0xFF) : 0)

					/*ST!Interpreter!methodClassOf:!*/
/* BEWARE of side effects with this macro */
/* only works on the current method */
#define methodClass             fetchPtr(VALUE, currLitMeth(currLitCount-1))

					/*ST!Interpreter!initializeContextIndices!*/
#define SENDER                  0
#define INSTR_PTR               1
#define STACK_PTR               2
#define METHOD                  3
#define RECEIVER                5
#define TEMP_FR_START           6

#define CALLER                  0
#define BLOCK_ARGC              3
#define INIT_IP                 4
#define HOME                    5

typedef struct {
	OOP FSENDER_CALLER;
	OOP FINSTR_PTR;
	OOP FSTACK_PTR;
	OOP FMETHOD_BLOCK_ARGC;
	OOP FINIT_IP;
	OOP FRECEIVER_HOME;
	OOP FTEMP_FRAME[32];
} CONTEXT;


#define cacheActiveContext	{ ac= (CONTEXT *)addrOfOop(activeContext); }
#define cacheHomeContext	{ hc= (CONTEXT *)addrOfOop(homeContext); }
/* BEWARE of side effects on arg */
#define cacheMethod		{ ABSmethod= addrOfOop(method); }

#ifdef SIM
#define CtxStorePtr(a,b,c) 	storePtr(a,b,c)
#define CtxFetchPtr(a,b)	fetchPtr(a,b)
#define NRCtxStorePtr(a,b,c)	NRstorePtr(a,b,c)
#define NR2CtxStorePtr(a,b,c)	NR2storePtr(a,b,c)
#endif SIM

#ifndef SIM
					/* special fast routine for contexts */
					/* BEWARE of side effects with 2nd arg */
#define CtxStorePtr(FIELD, ctx, val)							\
				{ OOP val1= (val);					\
				  countUp(val1);					\
				  countDown((ctx)->FIELD);				\
				  (ctx)->FIELD= val1;					\
				}

					/* and a version which doesn't ref.cnt the src */
					/* BEWARE of side effects with 2nd arg */
#define NRCtxStorePtr(FIELD, ctx, val)							\
				{ OOP val1= (val);					\
				  countDown((ctx)->FIELD);				\
				  (ctx)->FIELD= val1;					\
				}

					/* and a version which doesn't ref.cnt both */
#define NR2CtxStorePtr(FIELD, ctx, val)	{ (ctx)->FIELD= (val);	}


#define CtxFetchPtr(FIELD, ctx)	((ctx)->FIELD)


#endif SIM
					/* shorthand versions of the above for the active context */
#ifndef SIM
#define sac(FIELD, val)		CtxStorePtr(FIELD, ac, val)
#define NRsac(FIELD, val)	NRCtxStorePtr(FIELD, ac, val)
#define NR2sac(FIELD, val)	NR2CtxStorePtr(FIELD, ac, val)
#define fac(FIELD)		CtxFetchPtr(FIELD, ac)
#else SIM
#define NRsac(FIELD, val)	NRCtxStorePtr(FIELD, activeContext, val)
#define NR2sac(FIELD, val)	NR2CtxStorePtr(FIELD, activeContext, val)
#define fac(FIELD)		CtxFetchPtr(FIELD, activeContext)
#endif SIM


#define CtxSInt(FIELD, actx, val)	NR2CtxStorePtr(FIELD, actx, intObj(val))
#define CtxFInt(FIELD, actx)	intVal(CtxFetchPtr(FIELD, actx))


					/*ST!Interpreter!storeInstructionPointerValue:inContext:!*/
#define storeIPval(ip, ctx)     fstoreInt(INSTR_PTR, ctx, ip)
#ifndef SIM
#define AstrIPval(ip, actx)	CtxSInt(FINSTR_PTR, actx, ip)
#else SIM
#define AstrIPval(ip, actx)	CtxSInt(INSTR_PTR, actx, ip)
#endif SIM

					/*ST!Interpreter!storeStackPointerValue:inContext:!*/
#define storeSPval(sp, ctx)     fstoreInt(STACK_PTR, ctx, sp)
#ifndef SIM
#define AstrSPval(sp, actx)	CtxSInt(FSTACK_PTR, actx, sp)
#else SIM
#define AstrSPval(sp, actx)	CtxSInt(STACK_PTR, actx, sp)
#endif SIM

					/*ST!Interpreter!isBlockContext:!*/
#define isBlockCtx(c)           (isInt(fetchPtr(METHOD, c)))
#ifndef SIM
#define ABSisBlockCtx(c)	(isInt(CtxFetchPtr(FMETHOD_BLOCK_ARGC, c)))
#else SIM
#define ABSisBlockCtx(c)	(isInt(CtxFetchPtr(METHOD, c)))
#endif SIM
#define acIsMethCtx		(ac == hc)	/* is the active context a MethodContext? */


					/*ST!Interpreter!fetchContextRegisters!*/
					/*ST!Interpreter!instructionPointerOfContext:!*/
					/*ST!Interpreter!stackPointerOfContext:!*/

#ifndef SIM

#define fetchCtxRegs            {                                                       \
				  homeContext= ABSisBlockCtx(ac)               		\
						? fac(FRECEIVER_HOME)         		\
						: activeContext;                        \
				  cacheHomeContext;					\
				  receiver= CtxFetchPtr(FRECEIVER_HOME, hc);            \
				  method= CtxFetchPtr(FMETHOD_BLOCK_ARGC, hc);          \
				  cacheMethod;						\
				  ip= (BYTE*)ABSmethod + CtxFInt(FINSTR_PTR, ac) - 1;   \
				  sp= (WORD *)ac + CtxFInt(FSTACK_PTR, ac) + TEMP_FR_START - 1;\
				}

#else SIM

#define fetchCtxRegs            {                                                       \
				  homeContext= ABSisBlockCtx(activeContext)             \
						? fac(HOME)         		        \
						: activeContext;                        \
				  cacheHomeContext;					\
				  receiver= CtxFetchPtr(RECEIVER, homeContext);         \
				  method= CtxFetchPtr(METHOD, homeContext);             \
				  cacheMethod;						\
				  ip= (BYTE*)ABSmethod + CtxFInt(INSTR_PTR, activeContext) - 1;\
				  sp= (WORD *)ac + CtxFInt(STACK_PTR, activeContext) + TEMP_FR_START - 1;\
				}

#endif SIM

					/*ST!Interpreter!storeContextRegisters!*/
#ifndef SIM
#define storeCtxRegs            {                                                       	\
				  AstrIPval((WORD)(ip - (BYTE*)ABSmethod + 1), ac);          	\
				  AstrSPval((WORD)(sp - (WORD *)ac - TEMP_FR_START + 1), ac);	\
				}

#else SIM
#define storeCtxRegs            {                                                       	\
				  AstrIPval((WORD)(ip - (BYTE*)ABSmethod + 1), activeContext);	\
				  AstrSPval((WORD)(sp - (WORD *)ac - TEMP_FR_START + 1), activeContext);\
				}

#endif SIM


#ifndef SIM
					/*ST!Interpreter!fetchByte!*/
/* This can't be called fetchByte because of clashes with the other
   fetchByte macro
   BEWARE when using this as a parameter to a function/macro which uses the
   instructionPtr */
#define nextByte                (*ip++)
#else SIM
extern BYTE _nextByte();
#define nextByte		(_nextByte())
#endif SIM

					/*ST!Interpreter!initializeClassIndices!*/
#define SUPER_CLASS             0
#define MSG_DICT                1
#define INSTANCE_SPEC           2
#define METH_ARRAY              1
#define SEL_START               2

					/*ST!Interpreter!superclassOf:!*/
#define superclass(Class)       fetchPtr(SUPER_CLASS, Class)

					/*ST!Interpreter!initializeMessageIndices!*/
#define MSG_SELECTOR            0
#define MSG_ARGS                1
#define MSG_SIZE                2

					/*ST!Interpreter!initialInstructionPointerOfMethod:!*/
#define HdrInitIPofMeth(h)      ((litCnHdr(h) + LITERAL_START)*sizeof(WORD) + 1)
#define initIPofMeth(m)         HdrInitIPofMeth(header(m))

/*
	Macros for stack manipulation and access
*/

					/*ST!Interpreter!push:!*/
#define push(obj)               { OOP val1= (obj);					\
				  countUp(val1);					\
				  countDown(*++sp);					\
				  *sp= val1;						\
				}

					/* Non-ref counting version */
#define NRpush(obj)             { OOP val1= (obj);					\
				  countDown(*++sp);					\
				  *sp= val1;						\
				}

					/*ST!Interpreter!popStack!*/
#define popStack                (*sp--)

					/*ST!Interpreter!stackTop!*/
#define stackTop                (*sp)

					/*ST!Interpreter!unPop:!*/
#define unPop(n)                { sp += (n); }

					/*ST!Interpreter!pop:!*/
#define pop(n)                  { sp -= (n); }

					/*ST!Interpreter!stackValue:!*/
#define stackVal(n)             (*(sp - (n)))


					/*ST!Interpreter!newActiveContext:!*/
#define newActiveCtx(ctx)                                               \
	{                                                               \
		storeCtxRegs;                                           \
		c_down(activeContext);                                  \
		activeContext= (ctx);                                   \
		cacheActiveContext;					\
		c_up(activeContext);                                    \
		fetchCtxRegs;                                           \
	}

					/*ST!Interpreter!sender!*/
#ifndef SIM
#define sender                  CtxFetchPtr(FSENDER_CALLER, hc)
#else SIM
#define sender                  CtxFetchPtr(SENDER, homeContext)
#endif SIM

					/*ST!Interpreter!caller!*/
#ifndef SIM
#define caller                  fac(FSENDER_CALLER)
#else SIM
#define caller                  fac(SENDER)
#endif SIM

					/*ST!Interpreter!temporary:!*/
#ifndef SIM
#define temp(offset)            CtxFetchPtr(FTEMP_FRAME[(offset)], hc)
#else SIM
#define temp(offset)            CtxFetchPtr(TEMP_FR_START+(offset), homeContext)
#endif SIM

					/*ST!Interpreter!literal:!*/
#define literal(l)              currLitMeth(l)

					/*ST!Interpreter!objectPointerCountOf:!*/
#define oopCount(methodPtr)     (litCount(methodPtr) + LITERAL_START)

					/*ST!Interpreter!argumentCountOfBlock:!*/
#define argCOfBlock(blockPtr)   ffetchInt(BLOCK_ARGC, blockPtr)

					/*ST!Interpreter!argumentCountOf:!*/
#define argCnt(methPtr, argc)   { WORD flag= flagValue(methPtr);                \
				  (argc)= flag < 5 ? flag                      	\
					 : flag < 7 ? 0                         \
					 : (hdrExt(methPtr) & 0x1F00)>>8;       \
				}

