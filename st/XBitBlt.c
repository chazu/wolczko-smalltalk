/*
  	Smalltalk-80 Virtual Machine:
	XBitBlt (also known as RasterOp)

	Copyright (c) 1984,1985,1986,1987,1988,1989 Mario Wolczko
	All rights reserved.

This module implements the copyBits primitive (G/R p.651).

This is an X11-specific implementation.

It is assumed that the fields of the instance of BitBlt have
been fetched from object memory, and that the Forms have been converted 
to X11 pixmaps.

General philosophy: there are some things the MIT X server does fast, 
and we assume that they are fast in all X implementations.
For example, if we change the combination rule in a GC, X only transmits
the new one if it is really different.  Hence, we have not bothered to 
optimise this case.
However, some things are slow in the sample server (such as setting a 
clipping region for each BitBlt), so we pre-clip here.  Also, we detect
the case when an all-black halftone is used, and special-case that.
This code assumes that the first all-black 16x16 bitmap used is the
value of Form black, and that this will not change.  If it is used as the
target of a BitBlt, things will get slow...
$Log: XBitBlt.c,v $
 * Revision 5.0  1994/08/16  01:00:53  mario
 * Fix for colour display
 *
 * Revision 4.1  89/06/22  11:45:26  mario
 * Initial version.
 * 
*/

static char *rcsid= "$Header: /home/mario/st/RCS/XBitBlt.c,v 5.0 1994/08/16 01:00:53 mario Exp mario $";

#include "BitBlt.h"

#include <stdio.h>

#include "ST.h"
#include <X11/Xutil.h>

/* the following abbreviations are used:
	s,src   source
	d,dest  destination
	ht      halftone
	h,H     height
	w,W     width
*/

extern Display *display;
extern Pixmap theScreen, pix1;
extern int screen;
extern Window win;
extern unsigned dispW, dispH;

static GC general_gc, clip_gc, general_ht_gc, clip_ht_gc;
static GC screen_gc, tmp1_gc, tmp2_gc;
static Region region;

Pixmap black_ht;

static short all_black[4]= {
  GXclear, GXcopy, GXinvert, GXset,
};
  
void BitBlt(destBits, srcBits, htBits, combRule, dx, dy, w, h,
	clipX, clipY, clipW, clipH, sx, sy, sw, sh, dw, dh)
int dw, dh;	/* these are not used */
Pixmap destBits, srcBits, htBits;
{
	BOOL clip;
	XRectangle rect;
	GC gc_to_use;

	/* First we check for strange heights and widths.
	   One would expect that these should cause the primitive
	   to fail, but the 2.0 image actually generates these when
	   drawing SwitchViews!  Guess how long it took me to find
	   that out... */
	if (w <= 0 || h <= 0)
	  return;

	if (dx >= clipX && dy >= clipY
	    && dx + w <= clipX + clipW
	    && dy + h <= clipY + clipH) {
		/* no clipping required */
		clip= FALSE;
	} else {
		rect.x= clipX;  rect.y= clipY;
		rect.width= clipW; rect.height= clipH;
		clip= TRUE;
	}

	if (htBits == NO_FORM
	    || (htBits == black_ht && srcBits != NO_FORM)) { /* src only */
		if (clip) {
			XSetClipRectangles(display, clip_gc, 0, 0,
					   &rect, 1, YXBanded);
			gc_to_use= clip_gc;
		} else {
			gc_to_use= general_gc;
		}
		if (srcBits == NO_FORM) { /* use all-black as source */
			combRule= all_black[combRule&3];
			srcBits= destBits;
		}
		XSetFunction(display, gc_to_use, combRule);
		XCopyArea(display, srcBits, destBits,
			  gc_to_use, sx, sy, (unsigned)w, (unsigned)h, dx, dy);
	} else if (srcBits == NO_FORM) { /* halftone only */
		if (clip) {
			XSetClipRectangles(display, clip_ht_gc, 0, 0,
					   &rect, 1, YXBanded);
			gc_to_use= clip_ht_gc;
		} else {
			gc_to_use= general_ht_gc;
		}
		XSetFunction(display, gc_to_use, combRule);
		XSetStipple(display, gc_to_use, htBits);
		XFillRectangle(display, destBits, gc_to_use, dx, dy, w, h);
	} else {		/* general case */

		/* create a temporary pixmap the size of the source */
		extern Pixmap create_pixmap();
		Pixmap tmp_p;
		/* Yet again, we have to check for a strange case.
		   Smalltalk actually uses zero-width forms: X doesn't
		   like them a bit! */
		if (sw == 0 || sh == 0)
		  return;
		tmp_p= create_pixmap((unsigned)sw, (unsigned)sh);
		if (!tmp_p) {
			fatal("Couldn't create pixmap!");
		}
		/* fill it with halftone */
		XSetStipple(display, tmp1_gc, htBits);
		XFillRectangle(display, tmp_p, tmp1_gc, 0, 0, sw, sh);
		/* then AND in the source */
		XCopyArea(display, srcBits, tmp_p, tmp2_gc, sx, sy,
			  (unsigned)sw, (unsigned)sh, 0, 0);
		/* and apply to the dest, using combRule and clipping */
		rect.x= clipX;  rect.y= clipY;
		rect.width= clipW; rect.height= clipH;
		XSetClipRectangles(display, clip_gc, 0, 0, &rect, 1, YXBanded);
		XSetFunction(display, clip_gc, combRule);
		XCopyArea(display, tmp_p, destBits, clip_gc,
			  0, 0, (unsigned)w, (unsigned)h, dx, dy);
		XFreePixmap(display, tmp_p);
	}

	if (destBits == black_ht) {
		black_ht= NULL;
	}
}

void BitBlt_init()
{
	/* GC for general BitBlts */
	general_gc= XCreateGC(display, pix1, (unsigned long)0, (XGCValues *)0);
	XSetForeground(display, general_gc, BlackPixel(display, screen));
	XSetBackground(display, general_gc, WhitePixel(display, screen));
	XSetGraphicsExposures(display, general_gc, FALSE);

	/* GC for general BitBlts, when clipping */
	clip_gc= XCreateGC(display, pix1, (unsigned long)0, (XGCValues *)0);
	XSetForeground(display, clip_gc, BlackPixel(display, screen));
	XSetBackground(display, clip_gc, WhitePixel(display, screen));
	XSetGraphicsExposures(display, clip_gc, FALSE);

	/* GC for updating the display */
	screen_gc= XCreateGC(display, win, (unsigned long)0, (XGCValues *)0);
	XSetPlaneMask(display, screen_gc, AllPlanes);
	XSetForeground(display, screen_gc, BlackPixel(display, screen));
	XSetBackground(display, screen_gc, WhitePixel(display, screen));
	XSetFunction(display, screen_gc, GXcopy); 
	XSetGraphicsExposures(display, screen_gc, FALSE);

	/* GC for halftoning */
	general_ht_gc= XCreateGC(display, pix1, (unsigned long)0, (XGCValues *)0);
	XSetForeground(display, general_ht_gc, BlackPixel(display, screen));
	XSetBackground(display, general_ht_gc, WhitePixel(display, screen));
	XSetFillStyle(display, general_ht_gc, FillOpaqueStippled);
	XSetGraphicsExposures(display, general_ht_gc, FALSE);

	/* GC for halftoning, when clipping */
	clip_ht_gc= XCreateGC(display, pix1, (unsigned long)0, (XGCValues *)0);
	XSetForeground(display, clip_ht_gc, BlackPixel(display, screen));
	XSetBackground(display, clip_ht_gc, WhitePixel(display, screen));
	XSetFillStyle(display, clip_ht_gc, FillOpaqueStippled);
	XSetGraphicsExposures(display, clip_ht_gc, FALSE);

	/* GC for general-case BitBlts, intermediate halftoning */
	tmp1_gc= XCreateGC(display, pix1, (unsigned long)0, (XGCValues *)0);
	XSetForeground(display, tmp1_gc, BlackPixel(display, screen));
	XSetBackground(display, tmp1_gc, WhitePixel(display, screen));
	XSetGraphicsExposures(display, tmp1_gc, FALSE);
	XSetFillStyle(display, tmp1_gc, FillOpaqueStippled);
	XSetFunction(display, tmp1_gc, GXcopy); 
	tmp2_gc= XCreateGC(display, pix1, (unsigned long)0, (XGCValues *)0);
	XSetForeground(display, tmp2_gc, BlackPixel(display, screen));
	XSetBackground(display, tmp2_gc, WhitePixel(display, screen));
	XSetGraphicsExposures(display, tmp2_gc, FALSE);
	XSetFunction(display, tmp2_gc, GXand);

	/* initialise update region */
	region= XCreateRegion();
}

void update_if_screen(pix, cx, cy, cw, ch, x, y, w, h)
Pixmap pix;
int cx, cy, cw, ch, x, y, w, h;
{
	extern void add_to_updates();

	if (pix == theScreen && w > 0 && h > 0) {
		add_to_updates(x, y, w, h);
	}
}

BOOL mapped= FALSE;


/* the rectangle specified should be repainted */
void add_to_updates(x, y, w, h)
{
	XRectangle rect;

	rect.x= x;  rect.y= y; rect.width= w; rect.height= h;
	XUnionRectWithRegion(&rect, region, region);
}

void update_screen()
{
        extern XWindowAttributes win_attr;
	if (mapped && !XEmptyRegion(region)) {
		XSetRegion(display, screen_gc, region);
		if (win_attr.depth == 1)
			XCopyArea(display, theScreen, win, screen_gc,
				  0, 0, dispW, dispH, 0, 0);
		else
		  	XCopyPlane(display, theScreen, win, screen_gc,
				   0, 0, dispW, dispH, 0, 0, 0x1);
		XDestroyRegion(region);
		region= XCreateRegion();
	}
}

#ifdef FOO			/* this may be faster on some servers */
#define MAXRECTS 128
static XRectangle rects[MAXRECTS];
static n_rects= 0;

/* the rectangle specified should be repainted */
void add_to_updates(x, y, w, h)
{
	extern void update_screen();
	XRectangle *rect= &rects[n_rects++];

	rect->x= x;  rect->y= y; rect->width= w; rect->height= h;
	if (n_rects == MAXRECTS) {
		update_screen();
	}
}

void update_screen()
{
	if (n_rects > 0 && mapped) {
		XSetClipRectangles(display, screen_gc, 0, 0,
				   rects, n_rects, Unsorted);
		XCopyArea(display, theScreen, win, screen_gc,
			  0, 0, dispW, dispH, 0, 0);
		n_rects= 0;
	}
}

#endif FOO

