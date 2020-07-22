/*
  	Smalltalk-80 Virtual Machine:
	Array and Storage Management Primitives

	Copyright (c) 1984,1985,1986,1987,1988,1989 Mario Wolczko
	All rights reserved.
	
(Stream primitives have not been implemented.)

$Log: ArrStorePrim.c,v $
 * Revision 5.0  1994/08/16  00:56:45  mario
 * Extra parens in macros.
 *
 * Revision 4.2  91/06/24  18:59:53  mario
 * Made change to new snapshot format (X11) for Generic I/O.
 * 
 * Revision 4.1  89/06/21  19:42:44  mario
 * Added casts to keep lint quiet.
 * 
 * Revision 3.1  87/03/11  13:56:33  miw
 * 32-bit version with no D-B.
 * 
 * Revision 2.5  87/02/13  20:02:44  miw
 * Converted to 32-bit image:
 * - Changed calls to pos16{int,val} to call 32-bit versions.
 * - Moved pos16val (now 32) to a separate file.
 * - Altered size calculations now that size() is in bytes.
 * 
 * Revision 2.4  86/01/17  18:29:08  miw
 * Made all stack manipulation more efficient in light of D-B g.c.
 * 
 * Revision 2.3  85/12/09  20:20:28  miw
 * Altered positions of bit fields due to move in SmallInteger tag bit.
 * 
 * Revision 2.2  85/12/04  20:13:18  miw
 * Added tests for bitmaps in sub() and subStore() macros.
 * 
 * Revision 2.1  85/11/19  17:36:03  miw
 * Stabilised with all other modules at Rev 2.1
 * 
 * Revision 1.5  85/06/27  18:55:53  miw
 * Spaces to tabs.
 * 
 * Revision 1.3  85/03/06  14:53:52  miw
 * Added version of machine-dependent struct for Apollo
 * 
 * Revision 1.2  85/01/14  17:53:54  miw
 * Moved pos16int into a header file
 * 
 * Revision 1.1  85/01/10  12:50:24  miw
 * Initial revision
 * 
*/

static char *rcsid= "$Header: /home/mario/st/RCS/ArrStorePrim.c,v 5.0 1994/08/16 00:56:45 mario Exp mario $";

#include "Prim.h"
#include "pos32int.h"
#include "pos32val.h"

/**************** ARRAY PRIMITIVES ****************/


					/*ST!Interpreter!isIndexable:!*/
#define IS_INDEXABLE	0x1000
#define isIndexable(i)	((i) & IS_INDEXABLE)
					/*ST!Interpreter!isPointers:!*/
#define IS_POINTERS	0x4000
#define isPointers(i)	((i) & IS_POINTERS)
					/*ST!Interpreter!isWords:!*/
#define IS_WORDS	0x2000
#define isWords(i)	((i) & IS_WORDS)
					/*ST!Interpreter!fixedFieldsOf:!*/
#define fixedFieldsOf(i)		((i)&0x07FF)





					/*ST!Interpreter!instanceSpecificationOf:!*/
/* returns a WORD */
#define insSpec(Class)          fetchPtr(INSTANCE_SPEC, Class)

					/*ST!Interpreter!lengthOf:!*/
/* BEWARE of side effects on the 1st arg */
#ifdef SUN
#define length(array, insSp)    (isWords(insSp) ? Wlen(array) : Blen(array))
#else
#define length(array, insSp)    (bitmap(array) ? bm_w(array)*bm_h(array)/8 \
				 : (isWords(insSp) ? Wlen(array) : Blen(array)))
#endif

					/*ST!Interpreter!checkIndexableBoundsOf:in:!*/
/* BEWARE of side effects when using this macro */
/* Assume index is a WORD */
#define checkIndex(index, array, insSp, stackc)                                 \
	{                                                                       \
		if ((index) == 0 || (index) + fixedFieldsOf(insSp) > length(array, insSp)) {\
			unPop(stackc);                                          \
			return TRUE;                                            \
		}                                                               \
	}




extern WORD fetchBMWord();
extern void storeBMWord();

					/*ST!Interpreter!subscript:with:!*/
/* BEWARE of side effects involving args */
#define sub(array, index, result, insSp)                                        \
	{                                                                       \
		if (isWords(insSp))                                             \
			if (isPointers(insSp))                                  \
				result= fetchPtr((index) - 1, array);             \
			else if (bitmap(array)) {                               \
				pos32Int(fetchBMWord((index) - 1, array), result);\
			} else {                                                \
				pos32Int(fetchWord((index) - 1, array), result);  \
			}                                                       \
		else                                                            \
			result= intObj(fetchByte((index) - 1, array));            \
	}




					/*ST!Interpreter!subscript:with:storing:!*/
/* BEWARE of side effects involving args 3 & 4 */
#define subStore(array, index, value, insSp, stackc)                            \
	{                                                                       \
		if (isWords(insSp))                                             \
			if (isPointers(insSp)) {                                \
				storePtr((index) - 1, array, value);              \
			} else {                                                \
				WORD result;                                    \
				pos32val(value, result, stackc);                \
				if (bitmap(array))				\
					storeBMWord((index) - 1, array, result);	\
				else						\
					storeWord((index) - 1, array, result);    \
			}                                                       \
		else if (isInt(value)) {                                        \
			SIGNED result= intVal(value);                           \
			if (result < 0 || result > 255) {                       \
				unPop(stackc);                                  \
				return TRUE;                                    \
			}                                                       \
			storeByte((index) - 1, array, (BYTE)result);              \
		} else {                                                        \
			unPop(stackc);                                          \
			return TRUE;                                            \
		}                                                               \
	}



					/*ST!Interpreter!primitiveAt!*/
BOOL prAt()
{
	OOP array, res, i_s;
	int index;

	pos32val(popStack, index, 1);
	array= popStack;
	i_s= insSpec(fetchClass(array));
	checkIndex(index, array, i_s, 2);
	index += fixedFieldsOf(i_s);
	sub(array, (WORD)index, res, i_s);
	push(res);
	return FALSE;
}


					/*ST!Interpreter!primitiveAtPut!*/
BOOL prAtPut()
{
	OOP array, value, i_s;
	int index;

	value= popStack;
	pos32val(popStack, index, 2);
	array= popStack;
	i_s= insSpec(fetchClass(array));
	checkIndex(index, array, i_s, 3);
	index += fixedFieldsOf(i_s);
	subStore(array, (WORD)index, value, i_s, 3);
	push(value);
	return FALSE;
}


					/*ST!Interpreter!primitiveSize!*/
BOOL prSize()
{
	WORD len;
	OOP array= popStack, i_s;

	if (isInt(array))
	  return TRUE;
	i_s= insSpec(fetchClass(array));
	pos32Int(length(array, i_s) - fixedFieldsOf(i_s), len);
	push(len);
	return FALSE;
}



					/*ST!Interpreter!primitiveStringAt!*/
BOOL prSAt()
{
	OOP array, i_s;
	int index, ascii;

	pos32val(popStack, index, 1);
	array= popStack;
	i_s= insSpec(fetchClass(array));
	checkIndex(index, array, i_s, 2);
	if (isWords(i_s)) {
		unPop(2);
		return TRUE;
	}
	ascii= fetchByte((WORD)(index - 1), array);
	push(fetchPtr((WORD)ascii, CHAR_TABLE));
	return FALSE;
}


					/*ST!Interpreter!initializeCharacterIndex!*/
#define CHAR_VALUE              0


					/*ST!Interpreter!primitiveStringAtPut!*/
BOOL prSAtPut()
{
	OOP array, ascii, i_s;
	int index;
	OOP character= popStack;

	if (fetchClass(character) != CLASS_CHARACTER) {
		unPop(1);
		return TRUE;
	}
	pos32val(popStack, index, 2);
	array= popStack;
	i_s= insSpec(fetchClass(array));
	checkIndex(index, array, i_s, 3);
	ascii= fetchPtr(CHAR_VALUE, character);
	subStore(array, (WORD)index, ascii, i_s, 3);
	push(character);
	return FALSE;
}


/**************** STORAGE MANAGEMENT PRIMITIVES ****************/


					/*ST!Interpreter!primitiveObjectAt!*/
BOOL prOAt()
{
	SIGNED index;
	OOP intPtr= popStack;
	OOP rcvr= popStack;

	if (isInt(intPtr)
	   &&  (index= intVal(intPtr)) > 0
	   &&  index <= oopCount(rcvr)) {
		push(fetchPtr((WORD)(index - 1), rcvr));
		return FALSE;
	}
	unPop(2);
	return TRUE;
}


					/*ST!Interpreter!primitiveObjectAtPut!*/
BOOL prOAtPut()
{
	SIGNED index;
	OOP newValue= popStack;
	OOP intPtr= popStack;
	OOP rcvr= popStack;

	if (isInt(intPtr)
	   &&  (index= intVal(intPtr)) > 0
	   &&  index <= oopCount(rcvr)) {
		storePtr(index - 1, rcvr, newValue);
		push(newValue);
		return FALSE;
	}
	unPop(3);
	return TRUE;
}



					/*ST!Interpreter!primitiveNew!*/
BOOL prNew()
{
	WORD Size;
	OOP Class= popStack, i_s;

	i_s= insSpec(Class);
	if (isIndexable(i_s)) {
		unPop(1);
		return TRUE;
	}
	Size= fixedFieldsOf(i_s);
	push(isPointers(i_s) ? instPtrs(Class, Size) : instWords(Class, Size));
	return FALSE;
}



					/*ST!Interpreter!primitiveNewWithArg!*/
BOOL prNewWithArg()
{
	int Size;
	OOP Class, i_s;

	pos32val(popStack, Size, 1);
	Class= popStack;
	i_s= insSpec(Class);
	if (isIndexable(i_s)) {
		Size += fixedFieldsOf(i_s);
		push(isPointers(i_s) ? instPtrs(Class, (WORD)Size)
		    : isWords(i_s)   ? instWords(Class, (WORD)Size)
				    : instBytes(Class, (WORD)Size));
		return FALSE;
	}
	unPop(2);
	return TRUE;
}



					/*ST!Interpreter!primitiveBecome!*/
BOOL prBecome()
{
	extern void swapPtrs();
	OOP that= popStack;
	OOP this= popStack;

	if (isInt(this) || isInt(that)) {
		unPop(2);
		return TRUE;
	}
	swapPtrs(this, that);
	push(this);
	return FALSE;
}

/* UNSURE as to whether this should enforce the restriction that one can only
   access the fixed fields, i.e.
	  (index) > (insSp).FixedFields
   is illegal.  Certainly G/R use the length as below.
*/
					/*ST!Interpreter!checkInstanceVariableBoundsOf:in:!*/
/* assume index is SIGNED */
/* BEWARE of side effects involving args 1 & 2 */
#define checkInstance(index, oop, insSp, stackc)                                \
	{                                                                       \
		if ((index) <= 0 || (index) > length(oop, insSp)) {             \
			unPop(stackc);                                          \
			return TRUE;                                            \
		}                                                               \
	}



					/*ST!Interpreter!primitiveInstVarAt!*/
BOOL prIVAt()
{
	OOP val;
	OOP intPtr= popStack;
	OOP rcvr= popStack;

	if (isInt(intPtr)) {
		SIGNED index= intVal(intPtr);
		OOP i_s;

		i_s= insSpec(fetchClass(rcvr));
		checkInstance(index, rcvr, i_s, 2);
		sub(rcvr, (WORD)index, val, i_s);
		push(val);
		return FALSE;
	}
	unPop(2);
	return TRUE;
}


					/*ST!Interpreter!primitiveInstVarAtPut!*/
BOOL prIVAPut()
{
	OOP newValue= popStack;
	OOP intPtr= popStack;
	OOP rcvr= popStack;

	if (isInt(intPtr)) {
		SIGNED index= intVal(intPtr);
		OOP i_s;

		i_s= insSpec(fetchClass(rcvr));
		checkInstance(index, rcvr, i_s, 3);
		subStore(rcvr, (WORD)index, newValue, i_s, 3);
		push(newValue);
		return FALSE;
	}
	unPop(3);
	return TRUE;
}



					/*ST!Interpreter!primitiveAsOop!*/
BOOL prAsOop()
{
	OOP rcvr= popStack;
	if (isInt(rcvr)) {
		unPop(1);
		return TRUE;
	} else {
		push(objToInt(rcvr));
		return FALSE;
	}
}


					/*ST!Interpreter!primitiveAsObject!*/
BOOL prAsObject()
{
	OOP rcvr= popStack;

	if (isInt(rcvr)) {
		OOP obj= intToObj(rcvr);
		if (hasObject(obj)) {
			push(obj);
			return FALSE;
		}
	}
	unPop(1);
	return TRUE;
}


					/*ST!Interpreter!primitiveSomeInstance!*/
BOOL prSomeInstance()
{
	OOP inst= initialInstance(popStack);

	if (inst == NIL_PTR) {
		unPop(1);
		return TRUE;
	} else {
		push(inst);
		return FALSE;
	}
}


					/*ST!Interpreter!primitiveNextInstance!*/
BOOL prNextInstance()
{
	OOP inst= instanceAfter(popStack);

	if (inst == NIL_PTR) {
		unPop(1);
		return TRUE;
	} else {
		push(inst);
		return FALSE;
	}
}


					/*ST!Interpreter!primitiveNewMethod!*/
BOOL prNewMethod()
{
	WORD hdr= popStack;
	OOP intPtr= popStack;

	if (isInt(intPtr)) {
		SIGNED no_bytes= intVal(intPtr);
		if (no_bytes >= 0) {
			OOP Class= popStack;
			int lcn= litCnHdr(hdr);
			WORD Size= (lcn + 1)*(sizeof(WORD)) + no_bytes;
			OOP newMeth= instBytes(Class, Size);
			/* initialise literal frame to NIL */
			while (lcn >= 0)
				storeWord((WORD)lcn--, newMeth, NIL_PTR);
					/* use storeWord to avoid ref.counting */
			storePtr(0, newMeth, hdr);
			push(newMeth);
			return FALSE;
		}
	} 
	unPop(2);
	return TRUE;
}
