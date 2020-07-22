static char *rcsid= "$Header: /home/mario/st/RCS/SunRawIO.c,v 5.0 1994/08/16 01:00:34 mario Exp mario $";

/*
  	Smalltalk-80 Virtual Machine:
	Input/Output Module

	Copyright (c) 1984,1985,1986,1987,1988,1989 Mario Wolczko
	All rights reserved.

SUN:
	In this implementation we avoid the SUN window manager and do 
	everything via the raw devices.  Only the standard Sun mono frame
	buffer (1152x900) has been tested .. caveat porter.

The input devices are set up in the following modes:

Keyboard:
	The keyboard is the standard tty interface (we would use
	/dev/kbd if we could understand the manual page), so only
	provides ASCII key up events.  We simulate the down events, 
	together with a few extra events for the shift, control and
	lock keys.  Major problem: there is no difference between c-c
	and s-c-c.

Pointing device:
	The sun mouse, /dev/mouse, provides an extremely sensible
	protocol which is very easy to decode.

	Smalltalk requires three buttons on the mouse.  To avoid any
	confusion between the real button colours and the colours
	assumed in the Smalltalk books, we will call the buttons LEFT,
	MIDDLE and RIGHT from now on.

$Log: SunRawIO.c,v $
 * Revision 5.0  1994/08/16  01:00:34  mario
 * Sync with others
 *
 * Revision 4.1  89/06/22  11:44:34  mario
 * Initial version taken from old IO.c.
 * 
*/

#include <stdio.h>
#include <signal.h>
#include <ctype.h>
#include <sgtty.h>
#include <fcntl.h>
#include "IO.h"
#include "OM.h"
#include "OMMacros.h"

#define STDIN   0

#define isshifted(c)	(isupper(c))
#define iscntl(c)	(c < 32 && c != '\n' && c != '\r' \
			 && c != '\t' && c != '\033' \
			 && c != '\b' && c != '\177')
#define MAXX		1152
#define MAXY		900
#define PXL_INCR	1
#define INPUT_BUTTONS	(mbuf[0])

Pixrect *theScreen;
static int mouse;		/* the mouse device */
short cursorx, cursory;
static Pixrect *current_cursor, *saved_area;
	/* unfortunately, the SUN doesn't have a h/w cursor, so we
	   must simulate one */

static struct sgttyb InitKbd, repair;   /* mode of the controlling tty */


/*
	IO_init() should be called once at the start of
	interpretation to place all the I/O devices in the correct
	modes.
*/

void IO_init()        /* must call this immediately before doing any input */
{
	extern void IO_exit(), moveCursor(), read_first_script_event();
	extern void open_script_file();

	/* The VM will terminate cleanly, flushing all buffers, if it
	   receives a HUP signal */
	(void)signal(SIGHUP, IO_exit);
	(void)signal(SIGINT, IO_exit);
	(void)signal(SIGTERM, IO_exit);

	switch (io_mode) {
	case script_in:
		read_first_script_event();
		break;
	case script_out:
		open_script_file();
		break;
	}

	if ((mouse= open("/dev/mouse", 0)) < 0) {
		fatal("Cannot access mouse!");
	}
	if (fcntl(mouse, F_SETFL, FNDELAY) == -1) {
		fatal("Can't set non-blocking I/O on mouse!");
	}
	{			/* allocate space for cursor stuff */
		current_cursor= mem_create(16, 16, 1);
		saved_area= mem_create(16, 16, 1);
	}
	/* set up keyboard */
	{
		(void)ioctl(STDIN, TIOCGETP, &InitKbd);
		repair= InitKbd;
  		InitKbd.sg_flags= RAW;  
		(void)ioctl(STDIN, TIOCSETP, &InitKbd);
	}

	/* Grab hold of a pixrect backing up the frame buffer.
	   This code has not been tested on a colour display..
	   caveat emptor. */
	{
		extern char *getenv();
		char *fb= getenv("STFB");
		if (fb == NULL) fb= "/dev/fb";
		theScreen= pr_open(fb);
		if (theScreen == NULL) {
			fatal("Cannot open screen!");
		}
		dispW= theScreen->pr_size.x;
		dispH= theScreen->pr_size.y;
	}

	moveCursor(0, 0);
}


void IO_exit()            /* place input in sensible state and terminate */
{
	/* repair keyboard */
	(void)ioctl(STDIN, TIOCSETP, &repair);
	if (io_mode == script_out)
	  (void)fflush(script_file);
	log("Exiting");
	exit(0);
}


void input_enabled(en)		/* call this to enabled/disable input */
BOOL en;
{
}


void doInput()
{
	extern void moveCursor(), do_time(), addEvent();
	struct timeb Time_now; 
	TIME now;
	static unsigned short lastM_ms= 0; /* time of last mouse event */
	TIME  Mms_intvl; /* interval in ms since last event and mouse event */
	register int dx= 0, dy= 0;      /* change in mouse location */
	static short Buttons= 0;	/* old button state */
	register short b_down, b_up;    /* button transitions */
	long n_chars=0;			/* number of chars to read from kbd */
	char next_ch;			/* next key depression */
	char mbuf[3];			/* buffer for mouse data */
	int n_read;			/* how many bytes read */
	BOOL got_some= FALSE;		/* any mouse input? */

	if (inputSema == NIL_PTR)
		return;

	if (io_mode == script_in) {     /* reading from a script */
		extern void read_script_event();
		read_script_event();
		return;
	}


	/* get data about mouse, buttons and keyboard */
	while (n_read= read(mouse, mbuf, sizeof(mbuf)),
	       n_read == sizeof(mbuf)) {
		dx += mbuf[1];	/* assume sign extension */
		dy -= mbuf[2];  /* mouse y goes in opposite dir. */
		got_some= TRUE;
	}
	INPUT_BUTTONS= ~INPUT_BUTTONS;	/* the polarity is inverted */

	/* see if any events have occurred */
	if ((!got_some ||
	     (dx < PXL_INCR && dx > -PXL_INCR 
	      && dy < PXL_INCR && dy > -PXL_INCR
	      && (INPUT_BUTTONS &= BUTTONS)==Buttons))
	    && ((void)ioctl(STDIN, FIONREAD, &n_chars), n_chars==0))
	  return;

	/* see how much time has passed since the last event */
	ftime(&Time_now);   /* read the clock */
	now= 1000*Time_now.time + Time_now.millitm;  /* ignore overflow */
	do_time(now);
	
	if ((dx != 0 || dy != 0)) {     /* mouse has moved */
		Mms_intvl= now - lastM_ms;
		if (Mms_intvl >= SampleInterval) {
			lastM_ms= now;
			
			if (dx != 0) {
				MouseX += dx;
				addEvent(X_LOCTN | (MouseX & 0xFFF));
			}
			
			if (dy != 0) {
				MouseY += dy;
				addEvent(Y_LOCTN | (MouseY & 0xFFF));
			}
			
			/* update cursor position */
			if (linked)
			  moveCursor(cursorx+dx, cursory+dy);
		}
	}

	while (n_chars-- > 0) {
		BOOL ctrlChar, shiftKey;

		/* key has been pressed */
		(void)read(STDIN, &next_ch, 1);
		if (iscntl(next_ch)) {	/* control char */
			ctrlChar= TRUE;
			next_ch += '@';
			addEvent(ON_BISTATE + 138);
		}
		if (isshifted(next_ch))	{ /* shifted key */
			shiftKey= TRUE;
			next_ch= tolower(next_ch);
			addEvent(ON_BISTATE + 136);
		}

		addEvent(ON_BISTATE + (next_ch & 0x7f));
		/* NB: down and up transition */
		addEvent(OFF_BISTATE + (next_ch & 0x7f));

		if (ctrlChar)	/* control char */
			addEvent(OFF_BISTATE + 138);
		if (shiftKey)	/* shifted key */
			addEvent(OFF_BISTATE + 136);
	}
	if (INPUT_BUTTONS != Buttons) {
		/* button pressed or released */
		b_down= INPUT_BUTTONS & ~Buttons;
			/* buttons that have gone down */
		b_up= Buttons & ~INPUT_BUTTONS;
			/* buttons that have gone up */
		Buttons= INPUT_BUTTONS;
		if (b_down) {
			if (b_down & LEFT)
				addEvent(ON_BISTATE + LEFTSW);
			if (b_down & MIDDLE)
				addEvent(ON_BISTATE + MIDDLESW);
			if (b_down & RIGHT)
				addEvent(ON_BISTATE + RIGHTSW);
		}
		if (b_up) {
			if (b_up & LEFT)
				addEvent(OFF_BISTATE + LEFTSW);
			if (b_up & MIDDLE)
				addEvent(OFF_BISTATE + MIDDLESW);
			if (b_up & RIGHT)
				addEvent(OFF_BISTATE + RIGHTSW);
		}
	}
}


void moveCursor(x, y)        /* place cursor at given position */
int x, y;               /* The "linked" status is not affected */
{
	extern void remove_cursor_pattern(), replace_cursor_pattern();

	remove_cursor_pattern();

	/* save the background at the new location */
	cursorx= x;  cursory= y;
	replace_cursor_pattern();
	if (linked) {           /* update the mouse position to 
				   reflect the cursor position */
		MouseX= x;  MouseY= y;
	}
}

void remove_cursor_pattern()
{
	/* replace background */
	pr_rop(theScreen, cursorx, cursory, 16, 16, PIX_SRC, saved_area, 0, 0);
}

void replace_cursor_pattern()
{
	pr_rop(saved_area, 0, 0, 16, 16, PIX_SRC, theScreen, cursorx, cursory);
		
	/* then OR in new cursor */
	pr_rop(theScreen, cursorx, cursory, 16, 16, PIX_SRC|PIX_DST,
	       current_cursor, 0, 0);
}



void newCursor(cursor)
Pixrect *cursor;
{
	current_cursor= cursor;
	moveCursor(cursorx, cursory);
}


void setLinked(l)		/* link/unlink cursor and mouse */
BOOL l;
{
	linked= l;
}


void load_bitmaps(image)
FILE *image;
{
	extern int fseek();

	for_every_oop(oop) {
		if (!isFree(oop) && bitmap(oop)) {
			if (loctni(oop) != 0) {
				(void)fseek(image, (long)(loctni(oop)), 0);
				if (loctnb(oop)= pr_load(image, NULL),
				    loctnb(oop) == NULL) {
					fatal("Error loading bitmap!");
				}
			} else {
				loctnb(oop)= theScreen;
			}
		}
	}}
}


BOOL save_bitmaps(image, object_table_locs)
FILE *image;
unsigned *object_table_locs;
{
	for_every_oop(oop) {
		if (isFree(oop))
		  continue;
		if (bitmap(oop)) {
			if (loctnb(oop) != theScreen) {
				/* BEWARE if you change the oop format */
				object_table_locs[oop]= ftell(image);
				if (pr_dump((Pixrect *)loctn(oop),
					    image, NULL, RT_STANDARD, FALSE)) {
					log("Error while dumping bitmap");
					return TRUE;
				}
			} else {
				object_table_locs[oop]= 0;
			}
		} else {
			object_table_locs[oop]= loctn(oop) - om;
		}
	}}
	return FALSE;
}


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
	return mem_point(w, h, 1, bm_bits);
}


WORD16 cu_compact[]= {     /* cursor during compaction */
  0x0000, 0x0000, 0x003f, 0x07c2,
  0x0102, 0x013e, 0x0142, 0x01b9,
  0x7f45, 0x4083, 0x7101, 0x4911,
  0x8501, 0x8482, 0x4844, 0x3038
};
WORD16 cu_garbage[]= {             /* cursor during garbage collection */
  0x0300, 0x1FF0, 0x2008, 0x2008,
  0x3FF8, 0x1010, 0x1550, 0x1550,
  0x1550, 0x1550, 0x1550, 0x1550, 
  0x1550, 0x1010, 0x1010, 0x3FF8
};
mpr_static(CuGarbagePixRect, 16, 16, 1, cu_garbage);
Pixrect *CuGarbage= &CuGarbagePixRect;
mpr_static(CuCompactPixRect, 16, 16, 1, cu_compact);
Pixrect *CuCompact= &CuCompactPixRect;



void re_start_window_input()
{
}
