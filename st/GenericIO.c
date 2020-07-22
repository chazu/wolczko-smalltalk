static char *rcsid= "$Header: /home/mario/st/RCS/GenericIO.c,v 5.0 1994/08/16 00:58:34 mario Exp mario $";

/*
  	Smalltalk-80 Virtual Machine:
	Input/Output Module

	Copyright (c) 1984,1985,1986,1987,1988,1989 Mario Wolczko
	All rights reserved.

The input portion of this module is responsible for:
	- Polling the tablet to read the co-ordinates of the puck
	  (hereinafter referred to as the mouse), and determine
	  whether they have changed since the last poll.
	- Polling the buttons on the mouse to see if any have
	  changed state.
	- Polling the keyboard for any key depressions.
If any of the above events have occurred, entries are added to
an event buffer, giving the time of the event (either in
milliseconds since the last event, if the number can be
represented in 12 bits, or as an absolute time) and describing
the nature of the event. (G/R p.649)

This is the generic I/O module, that does not interface to any devices, 
but can read events from a saved event trace.

$Log: GenericIO.c,v $
 * Revision 5.0  1994/08/16  00:58:34  mario
 * Sync with others
 *
 * Revision 4.2  91/06/24  19:00:50  mario
 * Made change to new snapshot format (X11) for Generic I/O.
 * 
 * Revision 4.1  89/06/21  19:50:07  mario
 * *** empty log message ***
 * 
*/

#include <stdio.h>
#include <signal.h>
#include <ctype.h>
#include "IO.h"
#include "OM.h"
#include "OMMacros.h"

BITMAP theScreen;	/* the display bitmap */




/*
	IO_init() should be called once at the start of
	interpretation to place all the I/O devices in the correct
	modes.
*/

void IO_init()        /* must call this immediately before doing any input */
{
	extern void IO_exit(), moveCursor(), read_first_script_event();

	/* The VM will terminate cleanly, flushing all buffers, if it
	   receives a HUP signal */
	(void)signal(SIGHUP, IO_exit);

	if (io_mode != script_in) {
		fatal("And just *how* do you propose do get \
input into this thing, hmm?");
	}

	read_first_script_event();

	/* set a fake screen size */
	dispW= 1152 * 3/4;   /* maintain compatibility with SUN */
	dispH= 900 * 3/4;

	moveCursor(0, 0);
}



void IO_exit()            /* place input in sensible state and terminate */
{
	if (io_mode == script_out)
	  (void)fflush(script_file);
	log("Exiting");
	exit(0);
}


void input_enabled(en)		/* call this to enabled/disable input */
BOOL en;
{
}

void doInput()			/* call this to get more events */
{
	extern void read_script_event();
	read_script_event();
}



void moveCursor(x, y)        /* place cursor at given position */
int x, y;               /* The "linked" status is not affected */
{
	if (linked) {           /* update the mouse position to 
				   reflect the cursor position */
		MouseX= x;  MouseY= y;
	}
}




void newCursor(cursor)
WORD16 cursor[]; /* should be a 16-element array */
{
}


void setLinked(l)		/* link/unlink cursor and mouse */
BOOL l;
{
	linked= l;
}




/* Call this to load bitmaps (possibly in a machine-dependent format)
   from the snapshot.  The loctni() field of each bitmap oop indicates
   the offset in the image at which the bitmap begins.  There is no
   need to save the display bitmap.
*/
void load_bitmaps(image)
FILE *image;
{
	extern int fseek();
	extern char *malloc();

	for_every_oop(oop) {
		if (!isFree(oop) && bitmap(oop) && loctni(oop) != 0) {
			unsigned short w= bm_w(oop), h= bm_h(oop);
			int sz= h*((w+15)&~15)/8;
			(void)fseek(image, (long)(loctni(oop)), 0);
			loctn(oop)= (WORD *)malloc((unsigned)sz);
			if (loctn(oop) == NULL
			    || fread((char *)loctn(oop), sz, 1, image) != 1) {
				fatal("Cannot load bitmap!");
			}
		}
	}}
}


/* This routine should save all the bitmaps (except possibly the display)
   in the snapshot file, and update object_table_locs to reflect the
   offsets of the bitmaps in the image.  It should return TRUE if it
   cannot save the bitmaps in the image for some reason, FALSE otherwise.
*/
BOOL save_bitmaps(image, object_table_locs)
FILE *image;
unsigned *object_table_locs;
{
	for_every_oop(oop) {
		if (isFree(oop))
		  continue;
		if (bitmap(oop)) {
			if (loctnb(oop) != NULL) {
				unsigned short w= bm_w(oop);
				unsigned short h= bm_h(oop);
				int sz= h*((w+15)&~15)/8;
				/* BEWARE if you change the oop format */
				object_table_locs[oop]= (unsigned)ftell(image);
				if (fwrite((char *)loctn(oop), sz, 1, image) != 1)
				  return TRUE;
			} else {
				object_table_locs[oop]= 0;
			}
		} else {
			object_table_locs[oop]= loctn(oop) - om;
		}
	}}
	return FALSE;
}


WORD16 CuCompact[]= {     /* cursor during compaction */
  0x0000, 0x0000, 0x003f, 0x07c2,
  0x0102, 0x013e, 0x0142, 0x01b9,
  0x7f45, 0x4083, 0x7101, 0x4911,
  0x8501, 0x8482, 0x4844, 0x3038
};
WORD16 CuGarbage[]= {     /* cursor during garbage collection */
  0x0300, 0x1FF0, 0x2008, 0x2008,
  0x3FF8, 0x1010, 0x1550, 0x1550,
  0x1550, 0x1550, 0x1550, 0x1550, 
  0x1550, 0x1010, 0x1010, 0x3FF8
};



void re_start_window_input()
{
	fatal("Out of scripted events!");
}


/* Call this function to convert a Smalltalk Bitmap (instance of WordArray
   into a suitable representation for your bitblt primitive.
*/
BITMAP convert_bm(bm, w, h)
OOP bm;
WORD w, h;
{
	extern char *malloc();
	char *bm_bits;

	if (bm_bits= malloc(size(bm)), bm_bits == NULL) {
		/* in big trouble if malloc() fails */
		fatal("Can't convert bitmap!");
	}
	bcopy((char *)loctn(bm), bm_bits, size(bm));
	bm_w(bm)= w;
	bm_h(bm)= h;
	return (BITMAP)bm_bits;
}


