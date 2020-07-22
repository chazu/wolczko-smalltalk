/*
  	Smalltalk-80 Virtual Machine:
	I/O Primitives
	
	Copyright (c) 1984,1985,1986,1987,1988,1989 Mario Wolczko
	All rights reserved.

This module interfaces the I/O primitives to the I/O module.

$Log: IOPrims.c,v $
 * Revision 5.0  1994/08/16  00:58:52  mario
 * Sync with others
 *
 * Revision 4.2  91/06/24  19:00:59  mario
 * Made change to new snapshot format (X11) for Generic I/O.
 * 
 * Revision 4.1  89/06/21  19:53:06  mario
 * Made interface to IO.c and MachineIO.c machine-independent.
 * 
 * Revision 3.2  87/03/21  14:39:24  miw
 * Tidied prBoot() to use addrOfOop().
 * 
 * Revision 3.1  87/03/11  15:19:06  miw
 * Removed support for Perq and Orion.
 * Changed bitmap-related routines to use BM_WORD rather than WORD.
 * Removed fourth parameter from BitMapOf().
 * Added the prBoot() primitive to read an entire file.
 * 
 * Revision 2.7  87/02/13  19:55:04  miw
 * Converted to 32-bit: added the EVENT type, changed pos16 calls to pos32,
 * made cursor patterns arrays of WORD16 rather than WORD, altered calculation
 * of size field of a bitmap.
 * 
 * Revision 2.6  87/02/07  17:40:08  miw
 * Changed call to free() to free_bitmap() (for SUN).
 * 
 * Revision 2.5  87/02/06  12:20:39  miw
 * Added support for SunView.
 * 
 * Revision 2.4  86/01/17  21:00:00  miw
 * Added support for Orion, and altered BitMapOf to take a fourth parameter.
 * 
 * Revision 2.3  86/01/17  18:42:45  miw
 * Made stack access more efficient.
 * 
 * Revision 2.2  85/12/04  20:19:43  miw
 * Reworked prCopyBits(), prCursor() and prBeDisplay() for cached bitmaps,
 * and a display separate from object memory.
 * 
 * Revision 2.1  85/11/19  17:39:57  miw
 * Stabilised with all other modules at Rev 2.1
 * 
 * Revision 1.9  85/07/05  13:02:11  miw
 * Removed MSG_TRACE code.
 * Changed push() to NRpush() where appropriate.
 * 
 * Revision 1.8  85/04/23  16:41:13  miw
 * Tidied refce. to isEmptyEventQ().
 * 
 * Revision 1.7  85/04/15  12:58:30  miw
 * Removed duplicate definition of dispW and dispH.
 * 
 * Revision 1.6  85/04/02  18:32:11  miw
 * Removed VAX/PERQ communications.
 * 
 * Revision 1.5  85/04/02  11:24:11  miw
 * Fixed use of combRuleP identifier (too long for PERQ).
 * Added selective screen refresh.
 * 
 * Revision 1.4  85/03/14  19:49:37  miw
 * Corrected screen update on Apollo to clip to screen boundaries
 * 
 * Revision 1.3  85/03/06  15:22:51  miw
 * Added support for Apollo
 * 
 * Revision 1.2  85/01/14  17:53:38  miw
 * Added facilities for VAX-PERQ communication
 * 
 * Revision 1.1  85/01/10  12:54:07  miw
 * Initial revision
 * 

*/

static char *rcsid= "$Header: /home/mario/st/RCS/IOPrims.c,v 5.0 1994/08/16 00:58:52 mario Exp mario $";

#include "BitBlt.h"

#include <stdio.h>
#include <string.h>
#include "Prim.h"
#include "pos32int.h"


BOOL prMousePt()                     /* primitiveMousePoint */
{
	extern short MouseX, MouseY;    /* defined in IO.c */
	OOP newPt= instPtrs(CLASS_POINT, PT_SIZE);
	
	pop(1);
	NRstorePtr(XINDEX, newPt, intObj(MouseX));
	NRstorePtr(YINDEX, newPt, intObj(MouseY));
	push(newPt);
	return FALSE;
}


BOOL prCuLocPut()                   /* primitiveCursorLocPut */
{
	extern void moveCursor();    /* defined in IO.c */
	OOP newPt= popStack;
	
	if (fetchClass(newPt) == CLASS_POINT) {
		moveCursor(ffetchInt(XINDEX, newPt), ffetchInt(YINDEX, newPt));
		return FALSE;
	}
	unPop(1);
	return TRUE;
}


BOOL prCuLink()                     /* primitiveCursorLink */
{
	extern void setLinked();     /* defined in IO.c */
	OOP linkoop= popStack;
	
	if (linkoop == TRUE_PTR || linkoop == FALSE_PTR) {
		setLinked(linkoop==TRUE_PTR);
		return FALSE;
	}
	unPop(1);
	return TRUE;
}


BOOL prInpSema()                     /* primitiveInputSemaphore */
{
	extern OOP inputSema;   /* defined in IO.c */
	extern void input_enabled(); /* also in IO.c */
	OOP sema= popStack;
	if (sema == NIL_PTR || fetchClass(sema) == CLASS_SEMA) {
		inputSema= sema;
		input_enabled(sema != NIL_PTR);
		return FALSE;
	}
	unPop(1);
	return TRUE;
}


BOOL prSampleIntvl()                 /* primitiveSampleInterval */
{
	extern TIME SampleInterval;     /* defined in IO.c */
	OOP newIntvl= popStack;
	
	if (isInt(newIntvl)) {
		SampleInterval= intVal(newIntvl);
		return FALSE;
	}
	unPop(1);
	return TRUE;
}


BOOL prInpWord()                     /* primitiveInputWord */
{
	extern EVENT nextEvent();
	extern BOOL isEmptyEventQ();    /* defined in IO.c */
	OOP event;

	if (isEmptyEventQ())
		return TRUE;
	pop(1);
	pos32Int(nextEvent(), event);
	push(event);
	return FALSE;
}

BOOL prCopyBits()                    /* primitiveCopyBits */
{
	extern void update_if_screen();
	extern void BitBlt();        /* defined in BitBlt.c */
	SIGNED combRule;
	OOP bitblt= stackTop;
	/* can't really check these as they might be subclasses
	   (e.g. DisplayScreen) */
	OOP destForm= fetchPtr(DEST_FORM, bitblt);
	OOP srcForm= fetchPtr(SRC_FORM, bitblt);
	OOP htForm= fetchPtr(HT_FORM, bitblt);
	OOP combRP= fetchPtr(COMB_RULE, bitblt);
	/* NB: Smalltalk source says that must check that combRule
	   is in range 0-15 */
	OOP destXp= fetchPtr(DEST_X, bitblt);
	OOP destYp= fetchPtr(DEST_Y, bitblt);
	OOP widthp= fetchPtr(WIDTH, bitblt);
	OOP heightp= fetchPtr(HEIGHT, bitblt);
	OOP clipXp= fetchPtr(CLIP_X, bitblt);
	OOP clipYp= fetchPtr(CLIP_Y, bitblt);
	OOP clipWp= fetchPtr(CLIP_W, bitblt);
	OOP clipHp= fetchPtr(CLIP_H, bitblt);
	OOP srcXp= fetchPtr(SRC_X, bitblt);
	OOP srcYp= fetchPtr(SRC_Y, bitblt);
	OOP destWp= fetchPtr(FORM_WIDTH, destForm);
	OOP destHp= fetchPtr(FORM_HEIGHT, destForm);
	OOP srcWp= (srcForm==NIL_PTR)
	  ? ZERO_PTR
	    : fetchPtr(FORM_WIDTH, srcForm);
	OOP srcHp= (srcForm==NIL_PTR)
	  ? ZERO_PTR
	    : fetchPtr(FORM_HEIGHT, srcForm);
	
	if (isInt(combRP)) {
		combRule= intVal(combRP);
		if (combRule < 0 || combRule > 15)
			goto failed;
	} else
		goto failed;
	
	if (isInt(destXp)
	   && isInt(destYp)
	   && isInt(widthp)
	   && isInt(heightp)
	   && isInt(clipXp)
	   && isInt(clipYp)
	   && isInt(clipWp)
	   && isInt(clipHp)
	   && isInt(srcXp)
	   && isInt(srcYp)
	   && isInt(destWp)
	   && isInt(destHp)
	   && isInt(srcWp)
	   && isInt(srcHp)) {           
		SIGNED destX= intVal(destXp);
		SIGNED destY= intVal(destYp);
		SIGNED width= intVal(widthp);
		SIGNED height= intVal(heightp);
		SIGNED clipX= intVal(clipXp);
		SIGNED clipY= intVal(clipYp);
		SIGNED clipW= intVal(clipWp);
		SIGNED clipH= intVal(clipHp);
		SIGNED srcX= intVal(srcXp);
		SIGNED srcY= intVal(srcYp);
		SIGNED destW= intVal(destWp);
		SIGNED destH= intVal(destHp);
		SIGNED srcW= intVal(srcWp);
		SIGNED srcH= intVal(srcHp);
		OOP destBits= fetchPtr(FORM_BITS, destForm);
		BITMAP dstBitsP= BitMapOf(destBits, (WORD)destW, (WORD)destH);
		OOP srcBits, htBits;
		BITMAP srcBitsP= srcForm == NIL_PTR 
				? NO_FORM
				: (srcBits= fetchPtr(FORM_BITS, srcForm),
				   BitMapOf(srcBits, (WORD)srcW, (WORD)srcH));
		BITMAP htBitsP= htForm == NIL_PTR
				? NO_FORM
				: (htBits= fetchPtr(FORM_BITS, htForm),
				   BitMapOf(htBits, 16, 16));
		


		BitBlt(dstBitsP, srcBitsP, htBitsP, combRule,
		       destX, destY, width, height,
		       clipX, clipY, clipW, clipH,
		       srcX, srcY, srcW, srcH, destW, destH);

		update_if_screen(dstBitsP, clipX, clipY, clipW, clipH,
				 destX, destY, width, height);
		return FALSE;
	}
failed:
	log("BitBlt failed");
	return TRUE;
}

BOOL prTime()                        /* primitiveTimeWordsInto */
{
	extern TIME absTime();  /* defined in IO.c */
	OOP time_obj= popStack;
	TIME t;
	
	if (ptrs(time_obj) || Blen(time_obj) < 4) {
		unPop(1);
		return TRUE;
	}
	t= absTime();
	storeByte(0, time_obj, t & 0xFF);
	storeByte(1, time_obj, (t >> 8) & 0xFF);
	storeByte(2, time_obj, (t >> 16) & 0xFF);
	storeByte(3, time_obj, t >> 24);
	return FALSE;
}


BOOL prTick()                        /* primitiveTickWordsInto */
{
	extern TIME getTime();  /* defined in IO.c */
	OOP ms_obj= popStack;
	TIME t;
	
	if (ptrs(ms_obj) || Blen(ms_obj) < 4) {
		unPop(1);
		return TRUE;
	}
	t= getTime();
	storeByte(0, ms_obj, t & 0xFF);
	storeByte(1, ms_obj, (t >> 8) & 0xFF);
	storeByte(2, ms_obj, (t >> 16) & 0xFF);
	storeByte(3, ms_obj, t >> 24);
	return FALSE;
}


BOOL prSigAt()                       /* primitiveSignalAt */
{
	extern BOOL isTimedSema;        /* defined in Process.c */
	extern TIME semaWakeUpTime;     /* defined in Process.c */
	extern OOP timedSema;           /* defined in Process.c */
	OOP clockp= popStack;
	OOP sema= popStack;
	
	if (fetchClass(sema) != CLASS_SEMA) {   /* hold any previous semaphore */
		isTimedSema= FALSE;
		return FALSE;
	}
	if (isInt(clockp)) {
		semaWakeUpTime= intVal(clockp);
	} else if (ptrs(clockp) || Blen(clockp) < 4) {
		unPop(2);
		return TRUE;
	} else {
		semaWakeUpTime= (((((fetchByte(3, clockp) << 8)
				     + fetchByte(2, clockp)) << 8)
				   + fetchByte(1, clockp)) << 8)
		  + fetchByte(0, clockp);
	}
	timedSema= sema;
	isTimedSema= TRUE;
	return FALSE;
}


OOP cursorBits= NIL_PTR;       /* the current cursor */

BOOL prBeCursor()               /* primitiveBeCursor */
{
	extern void newCursor();             /* defined in IO.c */
	static BITMAP old_pattern;
	BITMAP cursor_pattern;
	cursorBits= fetchPtr(FORM_BITS, stackTop);
	cursor_pattern= BitMapOf(cursorBits, 16, 16);
	if (cursor_pattern != old_pattern) {
		newCursor(cursor_pattern);
		old_pattern= cursor_pattern;
	}
	return FALSE;
}

static OOP displayOop, displayBM;

void set_display_form(b)
BITMAP b;
{
	extern unsigned dispW, dispH;
	fstoreInt(FORM_WIDTH, displayOop, dispW);
	fstoreInt(FORM_HEIGHT, displayOop, dispH);
	loctnb(displayBM)= b;
}

BOOL prBeDisplay()	/* does not update the screen from the bitmap in the object */
{
	extern unsigned dispW, dispH;
	OOP dispForm= stackTop;
	unsigned width= ffetchInt(FORM_WIDTH, dispForm);
	unsigned height= ffetchInt(FORM_HEIGHT, dispForm);
	OOP bit_map= fetchPtr(FORM_BITS, dispForm);
		
	if (width > dispW  ||  height > dispH)	/* too big */
		return TRUE;

	if (bitmap(bit_map)) {
		if (loctnb(bit_map) != theScreen)
			free_bitmap(loctnb(bit_map));
	} else {
		extern void move_to_FCL();
		move_to_FCL(bit_map);
		bitmapPut(bit_map, TRUE);
	}
#ifdef SUN
	size(bit_map)= dispW*dispH/8;
#else
	bm_w(bit_map)= dispW;
	bm_h(bit_map)= dispH;
#endif
	loctnb(bit_map)= theScreen;
	/* reset the size of the Form */
	fstoreInt(FORM_WIDTH, dispForm, dispW);
	fstoreInt(FORM_HEIGHT, dispForm, dispH);
	displayOop= dispForm;
	displayBM= bit_map;
	return FALSE;
}

#include <sys/types.h>
#include <sys/stat.h>

BOOL prBoot()			/* input a String from a file */
{
	OOP filenameOop= popStack;
	extern char *malloc();
	FILE *f;
	struct stat Stat;

	if (fetchClass(filenameOop) == CLASS_STRING) {
		char *fn= malloc(size(filenameOop)+1);
		(void)strncpy(fn, (char *)addrOfOop(filenameOop),
			      (int)size(filenameOop));
		fn[size(filenameOop)]= '\0';
		if ((f= fopen(fn, "r")) != NULL
		    && stat(fn, &Stat)==0) {
			OOP resStr= instBytes(CLASS_STRING, (WORD)Stat.st_size);
			if (fread((char *)addrOfOop(resStr),
				  (int)Stat.st_size, 1, f) == 1) {
				(void)fclose(f);
				free(fn);
				push(resStr);
				return FALSE;
			}
			deallocate(resStr);
		}
		free(fn);
	}
	unPop(1);
	return TRUE;
}
