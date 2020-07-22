/*
  	Smalltalk-80 Virtual Machine:
	BitBlt (also known as RasterOp)

	Copyright (c) 1984,1985,1986,1987,1988,1989 Mario Wolczko
	All rights reserved.

This module implements the copyBits primitive (G/R p.651).

This is a SUN-specific implementation using pixrect().

It is assumed that the fields of the instance of BitBlt have
been fetched from object memory, and that the Forms have been converted 
to pixrect format.

$Log: SunBitBlt.c,v $
 * Revision 5.0  1994/08/16  01:00:25  mario
 * *** empty log message ***
 *
 * Revision 4.1  89/06/22  11:43:25  mario
 * Initial version (taken from old BitBlt.c).
 * 
 * Revision 2.1.4.3  87/03/21  14:34:17  miw
 * Added option to disable display update.
 * 
 * Revision 2.1.4.2  87/03/11  14:37:22  miw
 * Added test so that zero/negative width/height bitmap is not allocated.
 * Added extra tests and error messages if allocation fails.
 * 
 * Revision 2.1.4.1  87/02/06  12:23:59  miw
 * Specialised version of BitBlt for the SUN3 under SunView.
 * 
 * Revision 2.1.3.2  86/12/04  19:52:44  miw
 * Added code to avoid trashing cursor.
 * 
 * Revision 2.1.3.1  86/09/16  11:40:35  miw
 * Specialised version for the SUN3 workstation.
 * 
 * Revision 2.1  85/11/19  17:37:40  miw
 * Stabilised with all other modules at Rev 2.1
 * 
 * Revision 1.2  85/07/23  20:58:13  miw
 * Added special optimisation for combRule=3, srcBits=NULL.
 * 
 * Revision 1.1  85/01/10  12:51:48  miw
 * Initial revision
 * 
*/

static char *rcsid= "$Header: /home/mario/st/RCS/SunBitBlt.c,v 5.0 1994/08/16 01:00:25 mario Exp mario $";

#include "BitBlt.h"

#include <stdio.h>
#ifdef SUNTOOLS
  extern struct pixwin *screen_pixwin;
# include <sunwindow/window_hs.h>
#endif
#include <sunwindow/rect.h>

#include "ST.h"

static void SetRect(r, x, y, w, h)
Rect *r;
short x, y, w, h;
{
	r->r_left= x;
	r->r_top = y;
	r->r_width= w;
	r->r_height= h;
}

static void SectRect(r1, r2, r)	/* r= intersection of r1 and r2 */
Rect r1, r2, *r;
{
	r->r_left= max(r1.r_left, r2.r_left);
	r->r_top= max(r1.r_top, r2.r_top);
	r->r_width=
	  min(r1.r_left + r1.r_width, r2.r_left + r2.r_width) - r->r_left;
	r->r_height=
	  min(r1.r_top + r1.r_height, r2.r_top + r2.r_height) - r->r_top;
}


/* the following abbreviations are used:
	s,src   source
	d,dest  destination
	ht      halftone
	h,H     height
	w,W     width
*/



/* Table of SUN equivalents of Smalltalk combination rules. */
int	ruleTable[16]= {PIX_COLOR(0),
			PIX_SRC & PIX_DST | PIX_DONTCLIP,
			PIX_SRC & PIX_NOT(PIX_DST) | PIX_DONTCLIP,
			PIX_SRC | PIX_DONTCLIP,
			PIX_DST & PIX_NOT(PIX_SRC) | PIX_DONTCLIP, 
			PIX_DST | PIX_DONTCLIP, 
			PIX_SRC^PIX_DST | PIX_DONTCLIP, 
			PIX_SRC|PIX_DST | PIX_DONTCLIP,
			PIX_NOT(PIX_SRC) & PIX_NOT(PIX_DST) | PIX_DONTCLIP, 
			PIX_NOT(PIX_SRC ^ PIX_DST) | PIX_DONTCLIP, 
			PIX_NOT(PIX_DST) | PIX_DONTCLIP, 
			PIX_SRC | PIX_NOT(PIX_DST) | PIX_DONTCLIP, 
			PIX_NOT(PIX_SRC) | PIX_DONTCLIP, 
			PIX_NOT(PIX_SRC) | PIX_DST | PIX_DONTCLIP, 
			PIX_NOT(PIX_SRC) | PIX_NOT(PIX_DST) | PIX_DONTCLIP, 
			PIX_COLOR(1)}; /* black */


static void errorNoBits()
{
	fatal("Cannot allocate pixrect!\n");
}

void BitBlt(destBits, srcBits, htBits, combRule, dx, dy, w, h,
	clipX, clipY, clipW, clipH, sx, sy, sw, sh, dw, dh)
unsigned sw, sh, dw, dh;
Pixrect *destBits, *srcBits, *htBits;
{
	extern void errorNoBits(), SetRect(), SectRect();
	Pixrect *temp, *clippedDest;
	/* SUN number for combination rule. */
	int	op= ruleTable[combRule];
	Rect r1, r2, r3;
	int	xx, yy, ww, hh;
#ifndef SUNTOOLS
	BOOL intersect;
	extern struct pixrect *theScreen;
	extern short cursorx, cursory;
	extern void remove_cursor_pattern(), replace_cursor_pattern();

	if (destBits == theScreen) {
		/* must see if we have to avoid the cursor */
		intersect = dx < cursorx+16 && dy < cursory+16
		  && dx+w > cursorx && dy+h > cursory;
	} else if (srcBits == theScreen) {
		/* must see if we have to avoid the cursor */
		intersect = sx < cursorx+16 && sy < cursory+16
		  && sx+w > cursorx && sy+h > cursory;
	}

	if (intersect) {
		remove_cursor_pattern();
	}
#endif

	clippedDest= pr_region(destBits, clipX, clipY, clipW, clipH);
	if (clippedDest == NULL) {
		errorNoBits();
	}
	dx -= clipX;
	dy -= clipY;

	if (htBits == NO_FORM) { 	/* No halftone masking. */
		pr_rop(clippedDest, dx, dy, w, h, op, srcBits, sx, sy);
	} else if (srcBits == NO_FORM) {  /* Filling an area with halftone. */
		pr_replrop(clippedDest, dx, dy, w, h, op, htBits, dx&15, dy&15);
	} else {
		/* Halftone masking. 'and' source with halftone before main
		 * operation. This is not provided by the SUN rasterop
		 * so we must use an auxiliary raster for the 'and'.
		 */

		/* Clipping rectangle on source bitmap. */
		SetRect(&r1, sx-dx, sy-dy, clipW, clipH);

		/* Extent on source bitmap. */
		SetRect(&r2, sx, sy, w, h);

		/* Intersect clip rectangle and extent. */
		SectRect(r1, r2, &r3);

		/* Boundary rectangle for bits on the source. */
		SetRect(&r1, 0, 0, sw, sh);

		/* Set r2 to intersection of clip, extent, and bounds. */
		SectRect(r3, r1, &r2);

		xx= r2.r_left;
		yy= r2.r_top;
		ww= r2.r_width;
		hh= r2.r_height;

		if (ww > 0 && hh > 0) {	/* otherwise a no-op */
			temp= mem_create(ww, hh, 1);
			
			if (temp == NULL) {
				errorNoBits();
			}

			/* Fill buffer with halftone. */
			pr_replrop(temp, 0, 0, ww, hh, PIX_SRC, htBits, dx&15, dy&15);

			/* 'and' source into buffer. */
			pr_rop(temp, 0, 0, ww, hh, PIX_SRC & PIX_DST, srcBits, xx, yy);

			/* combine contents of buffer with dest. */
			pr_rop(clippedDest, dx+xx-sx, dy+yy-sy, ww, hh, op, temp, 0, 0);

			/* Free the buffer. */
			pr_close(temp);
		}
	}
	pr_close(clippedDest);
#ifndef SUNTOOLS
	if (intersect) {
		replace_cursor_pattern();
	}
#endif
}

extern unsigned dispW, dispH;

void BitBlt_init()
{
}

void update_if_screen(pix, cx, cy, cw, ch, x, y, w, h)
Pixrect *pix;
int cx, cy, cw, ch, x, y, w, h;
{
#ifdef SUNTOOLS
	extern void SetRect(), SectRect();
	Rect r1, r2, r;
	extern Pixrect *theScreen;
	extern BOOL display_enabled;

	if (pix == theScreen && display_enabled) {
		SetRect(&r1, cx, cy, cw, ch);
		SetRect(&r2, x, y, w, h);
		SectRect(r1, r2, &r);
		if (r.r_width > 0 && r.r_height > 0)
		  pw_rop(screen_pixwin,
			 r.r_left, r.r_top, r.r_width, r.r_height,
			 PIX_SRC, pix, r.r_left, r.r_top);
	}
#endif
}
