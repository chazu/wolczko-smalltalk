/*
  	Smalltalk-80 Virtual Machine:
	System primitives

	Copyright (c) 1984,1985,1986,1987,1988,1989 Mario Wolczko
	All rights reserved.

$Log: SystemPrims.c,v $
 * Revision 5.0  1994/08/16  01:00:43  mario
 * Fix side-effect bug
 *
 * Revision 4.1  89/06/22  11:45:06  mario
 * Stabilised with other modules at 4.1.
 * 
 * Revision 3.1  87/03/11  16:51:46  miw
 * Removed D-B; stabilised with other modules.
 * 
 * Revision 2.4  87/02/13  19:59:58  miw
 * Avoided a minor problem when calling pos32val: should be given an int rathe
 * than an unsigned.
 * 
 * Revision 2.3  86/01/17  18:51:53  miw
 * Made stack access more efficient.
 * 
 * Revision 2.2  85/12/04  20:27:20  miw
 * Removed the declaration of free_ptr.
 * 
 * Revision 2.1  85/11/19  17:42:52  miw
 * Stabilised with all other modules at Rev 2.1
 * 
 * Revision 1.7  85/07/23  21:00:36  miw
 * Added more detail in prQuit.
 * 
 * Revision 1.6  85/07/05  13:24:43  miw
 * Replaced push() with NRpush() where appropriate
 * 
 * Revision 1.5  85/06/27  19:11:36  miw
 * Added call of pos32val.
 * 
 * Revision 1.4  85/04/15  13:02:40  miw
 * Added push of NIL in prSnapshot().
 * 
 * Revision 1.3  85/04/02  11:32:22  miw
 * Added code in prSnapshot() to invoke save_snapshot().
 * 
 * Revision 1.2  85/01/14  17:56:20  miw
 * 
 * Revision 1.1  85/01/10  12:57:51  miw
 * Initial revision
 * 
*/

static char *rcsid= "$Header: /home/mario/st/RCS/SystemPrims.c,v 5.0 1994/08/16 01:00:43 mario Exp mario $";

#include <stdio.h>
#include "Prim.h"
#include "pos32val.h"

					/*ST!Interpreter!primitiveEquivalent!*/
BOOL prEquiv()
{
  	OOP top= popStack;
  	OOP next= popStack;
	push(top == next ? TRUE_PTR : FALSE_PTR);
		/* How about that, sports fans ? */
	return FALSE;
}


					/*ST!Interpreter!primitiveClass!*/
BOOL prClass()
{
	OOP rcvr= popStack;
	push(fetchClass(rcvr));
	return FALSE;
}

BOOL prCoreLeft()                       /* primitiveCoreLeft */
{
	pop(1);
	if (isIntVal(words_left)) {
		NRpush(intObj(words_left));
	} else {
		WORD Size= words_left < 256*256 ? 2 : 3;
			/* BEWARE if you have > 16Mb of Object memory!!! */
		OOP w= instBytes(CLASS_LG_POS_INT, Size);
		storeByte(0, w, words_left & 0xFF);
		storeByte(1, w, (words_left >> 8) & 0xFF);
		if (Size == 3)
			storeByte(2, w, words_left >> 16);
		push(w);
	}
	return FALSE;
}



BOOL prQuit()                   /* primitiveQuit */
{
	extern void IO_exit();

	loge("Yet another Smalltalk session comes to a successful conclusion ...");
	IO_exit();
}


BOOL prOopsLeft()                       /*  primitiveOopsLeft */
{
	pop(1);
	if (isIntVal(oops_left)) {
		NRpush(intObj(oops_left));
	} else {
		OOP w= instBytes(CLASS_LG_POS_INT, 2);
			/* BEWARE if you have >64K oops! */
		storeByte(0, w, oops_left & 0xFF);
		storeByte(1, w, oops_left >> 8);
		push(w);
	}
	return FALSE;
}

OOP limitSema= NIL_PTR;

BOOL prSAtOopsLeft()                    /* primitiveSignalAtOopsLeftWordsLeft */
{
	int wds;
	unsigned oops;
	OOP sema;
	OOP intPtr= popStack;
	
	pos32val(intPtr, wds, 1);
	intPtr= popStack;
	if (isInt(intPtr)) {
		oops= intVal(intPtr);
		sema= popStack;
		if (sema==NIL_PTR || fetchClass(sema) == CLASS_SEMA) {
			limitSema= sema;
			word_limit= wds;
			ptr_limit= oops;
			return FALSE;
		} else
			unPop(3);
	} else
		unPop(2);
	return TRUE;
}

BOOL prSnapshot()
{
	extern BOOL save_snapshot();
	BOOL saved= save_snapshot();

	pop(1);
	NRpush(NIL_PTR);
	return !saved;
}
