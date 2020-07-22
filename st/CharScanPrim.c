/*
  	Smalltalk-80 Virtual Machine:
	Character Scanning Primitive

	Copyright (c) 1984,1985,1986,1987,1988,1989 Mario Wolczko
	All rights reserved.

This primitive seems to make a big difference to the `feel' of the 
interactive display,  especially when views are refreshed.
  
It now uses BitBlt(), which should be fast.

$Log: CharScanPrim.c,v $
 * Revision 5.0  1994/08/16  00:58:04  mario
 * Sync with others
 *
 * Revision 4.1  89/06/21  19:46:34  mario
 * Made interface to display machine-independent.
 * 
 * Revision 3.1  87/03/11  14:40:21  miw
 * 32-bit version: no D-B.
 * Changed bitmap routines to use BM_WORD.
 *  
 * 
 * Revision 2.5  87/02/06  12:21:57  miw
 * Added support for SunView.
 * 
 * Revision 2.4  86/01/17  19:16:43  miw
 * Added fourth param to BitMapOf for halftones.
 * 
 * Revision 2.2  85/12/04  20:15:52  miw
 * Removed fastBitBlt() on the assumption that BitBlt() would have adequate
 * speed.  Added a test of the "display" instance variable; if not true
 * several tests of other variables can be omitted.
 * 
 * Revision 2.1  85/11/19  17:37:56  miw
 * Stabilised with all other modules at Rev 2.1
 * 
 * Revision 1.2  85/11/14  16:28:09  miw
 * Fixed incorrect replacement of destX when CrossedX stop condition
 * occurs.
 * 
 * Revision 1.1  85/07/05  12:54:27  miw
 * Initial revision
 * 

*/

static char *rcsid= "$Header: /home/mario/st/RCS/CharScanPrim.c,v 5.0 1994/08/16 00:58:04 mario Exp mario $";

#include "BitBlt.h"

#include <stdio.h>
#include "Prim.h"

/* fields of CharacterScanner (subclass of BitBlt) */
#define LAST_INDEX      14      /* an <Integer> */
#define X_TABLE         15      /* an <Array> */
#define STOP_CONDITIONS 16      /* an <Array> */
#define TEXT            17
#define TEXT_STYLE      18
#define LEFT_MARGIN     19
#define RIGHT_MARGIN    20
#define FONT            21
#define LINE            22
#define RUN_STOP_INDEX  23
#define SPACE_COUNT     24
#define SPACE_WIDTH     25
#define OUTPUT_MEDIUM   26

/* values in the pool TextConstants */
#define CROSSED_X       258
#define END_OF_RUN      257


/* <CharacterScanner>scanCharactersFrom: startIndex
		to: stopIndex
		in: srcString
		rightX: rightX
		stopConditions: stops
		displaying: display
*/
BOOL prScanChars()
{
	extern void BitBlt();
	OOP display= popStack;
	OOP stops= popStack;
	SIGNED rightX= intVal(popStack);
	OOP srcString= popStack;
	SIGNED stopIndex= intVal(popStack);
	SIGNED startIndex= intVal(popStack);
	OOP charScanner= stackTop;

	OOP xTable= fetchPtr(X_TABLE, charScanner);
		/* xTable, stopConditions and lastIndex will be cached during the loop */
	OOP stopConditions= fetchPtr(STOP_CONDITIONS, charScanner);
	SIGNED lastIndex;

	OOP srcForm= fetchPtr(SRC_FORM, charScanner);
	OOP destForm= fetchPtr(DEST_FORM, charScanner);
	OOP htForm= fetchPtr(HT_FORM, charScanner);
	OOP destXp= fetchPtr(DEST_X, charScanner);
	OOP destYp= fetchPtr(DEST_Y, charScanner);
	OOP heightp= fetchPtr(HEIGHT, charScanner);
	OOP clipXp= fetchPtr(CLIP_X, charScanner);
	OOP clipYp= fetchPtr(CLIP_Y, charScanner);
	OOP clipWp= fetchPtr(CLIP_W, charScanner);
	OOP clipHp= fetchPtr(CLIP_H, charScanner);
	OOP srcYp= fetchPtr(SRC_Y, charScanner);
	OOP combRP= fetchPtr(COMB_RULE, charScanner);
	OOP destWp= fetchPtr(FORM_WIDTH, destForm);
	OOP destHp= fetchPtr(FORM_HEIGHT, destForm);
	OOP srcWp= (srcForm==NIL_PTR) ? ZERO_PTR : fetchPtr(FORM_WIDTH, srcForm);
	OOP srcHp= (srcForm==NIL_PTR) ? ZERO_PTR : fetchPtr(FORM_HEIGHT, srcForm);
	
	if (isInt(destXp)
	   && isInt(destWp)
	   && isInt(destHp)
	   && isInt(srcWp)
	   && isInt(srcHp)
	   && (display != TRUE_PTR
	      || (isInt(destYp)
		  && isInt(heightp)
		  && isInt(clipXp)
		  && isInt(clipYp)
		  && isInt(clipWp)
		  && isInt(clipHp)
		  && isInt(srcYp)
		  && isInt(combRP)))) {
		SIGNED destX= intVal(destXp), original_destX= destX;
		SIGNED destW= intVal(destWp);
		SIGNED destH= intVal(destHp);
		SIGNED srcW= intVal(srcWp);
		SIGNED srcH= intVal(srcHp);
		OOP dstBits= fetchPtr(FORM_BITS, destForm);
		OOP srcBits, htBits;
		BITMAP dstBitsP= BitMapOf(dstBits, (WORD)destW, (WORD)destH),
		       srcBitsP= srcForm == NIL_PTR
				? NO_FORM : (srcBits= fetchPtr(FORM_BITS, srcForm),
					     BitMapOf(srcBits, (WORD)srcW, (WORD)srcH)), 
		       htBitsP= htForm == NIL_PTR
				? NO_FORM : (htBits= fetchPtr(FORM_BITS, htForm),
					     BitMapOf(htBits, 16, 16));
		SIGNED width, srcX, destY, height, clipX, clipY, clipW, clipH, srcY, nextDestX, combRule;
		
		if (display == TRUE_PTR) {	
			destY= intVal(destYp);
			height= intVal(heightp);
			clipX= intVal(clipXp);
			clipY= intVal(clipYp);
			clipW= intVal(clipWp);
			clipH= intVal(clipHp);
			srcY= intVal(srcYp);
			combRule= intVal(combRP);
		}
		/* remember that indices are based from 0 in the VM but from 1 in st-80 */
		for (lastIndex= startIndex-1;  lastIndex < stopIndex;  ++lastIndex) {
			/* NB: no bound check on index */

			WORD ascii= fetchByte((WORD)lastIndex, srcString);

			if (fetchPtr(ascii, stopConditions) != NIL_PTR) {
				pop(1);         /* pop self */
				push(fetchPtr(ascii, stops));
				if (lastIndex == startIndex-1) /* don't restore srcX, destX and width */
					goto A_restore;
				goto restore_and_exit;
			}

			srcX= ffetchInt(ascii, xTable);
			nextDestX= destX + (width= ffetchInt(ascii+1, xTable) - srcX);
			if (nextDestX > rightX) {
				pop(1);         /* pop self */
				push(fetchPtr(CROSSED_X-1, stops));
				fstoreInt(DEST_X, charScanner, destX);
				goto B_restore;
			}

			if (display == TRUE_PTR)
				BitBlt(dstBitsP, srcBitsP, htBitsP, combRule, destX, destY,
					width, height,
					clipX, clipY, clipW, clipH,
					srcX, srcY, srcW, srcH, destW, destH);

			destX= nextDestX;
		}

		lastIndex= stopIndex-1;
		pop(1);         /* pop self */
		push(fetchPtr(END_OF_RUN-1, stops));    /* normal return value */

restore_and_exit:       /* replace cached values */
		fstoreInt(DEST_X, charScanner, nextDestX);
B_restore:
		fstoreInt(SRC_X, charScanner, srcX);
		fstoreInt(WIDTH, charScanner, width);
A_restore:
		fstoreInt(LAST_INDEX, charScanner, lastIndex+1);

		if (display == TRUE_PTR) {
			/* batch screen updates */
			extern void update_if_screen();
			update_if_screen(dstBitsP,
					 clipX, clipY, clipW, clipH,
					 original_destX, destY,
					 destX - original_destX,
					 height);
		}
		return FALSE;
	}
	/* failed */
	unPop(6);
	return TRUE;
}





