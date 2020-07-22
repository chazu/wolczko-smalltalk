static char *rcsid= "$Header: /home/mario/st/RCS/SunIO.c,v 5.0 1994/08/16 01:00:30 mario Exp mario $";

/*
  	Smalltalk-80 Virtual Machine:
	Input/Output Module

	Copyright (c) 1984,1985,1986,1987,1988,1989
	Mario Wolczko and Michael Fisher
	All rights reserved.

SUN:
	Only the standard Sun mono frame buffer (1152x900) has been
	tested .. caveat porter.

The input devices are set up in the following modes:

Keyboard:
	The keyboard should provide ASCII key up and down events,
	together with a few extra events for the shift, control and
	lock keys.

Pointing device:
	Smalltalk requires three buttons on
	the mouse.  To avoid any confusion between the real button
	colours and the colours assumed in the Smalltalk books,
	we will call the buttons LEFT, MIDDLE and RIGHT from now on.

$Log: SunIO.c,v $
 * Revision 5.0  1994/08/16  01:00:30  mario
 * Sync with others
 *
 * Revision 4.1  89/06/22  11:43:56  mario
 * Initial version taken from old IO.c.
 * 
*/

#include <stdio.h>
#include <signal.h>
#include <ctype.h>
#include <suntool/sunview.h>
#include <suntool/canvas.h>
#include "IO.h"
#include "OM.h"
#include "OMMacros.h"


#define isshifted(c)	(isupper(c))
#define iscntl(c)	(c < 32 && c != '\n' && c != '\r' \
			 && c != '\t' && c != '\033' \
			 && c != '\b' && c != '\177')
#define MAXX		1152
#define MAXY		900
#define PXL_INCR	1

				/* maps shifted keys to unshifted ones */
char  ShiftMap[128]= {
	' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* ^@ - ^O */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   /* ^P - ^_ */
	' ', '1', '\'', '3', '4', '5', '7', 0, 		/* SP - ' */
	'9', '0', '8', '=', 0, 0, 0, 0,			/* ( - / */
	0, 0, 0, 0, 0, 0, 0, 0,				/* 0 - 7 */
	0, 0, ';', 0, ',', 0, '.', '/',			/* 8 - ? */
	'2', 'a', 'b', 'c', 'd', 'e', 'f', 'g',		/* @ - G */
	'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',		/* H - O */
	'p', 'q', 'r', 's', 't', 'u', 'v', 'w',		/* P - W */
	'x', 'y', 'z', 0, 0, 0, '6', '-',		/* X - _ */
	0, 0, 0, 0, 0, 0, 0, 0,				/* ` - g */
	0, 0, 0, 0, 0, 0, 0, 0,				/* h - o */
	0, 0, 0, 0, 0, 0, 0, 0,				/* p - w */
	0, 0, 0, '[', '\\', ']', '`', '\177'		/* x - DEL */
  };
/* note that ^ _ and ~ are mapped to 6 - and `, and not to themselves */

Pixrect *theScreen;
static Frame    st_frame;
static Canvas   st_canvas;
static Cursor   screen_cursor;
struct pixwin   *screen_pixwin;
static short    icon_image[]= {
#include "smalltalk.icon"
};
DEFINE_ICON_FROM_IMAGE(icon, icon_image); /* A very silly icon */

#define NO_EVENT -1

/*
	IO_init() should be called once at the start of
	interpretation to place all the I/O devices in the correct
	modes.
*/

void IO_init()        /* must call this immediately before doing any input */
{
	extern void IO_exit(), moveCursor(), read_first_script_event();
	extern void refresh_screen(), open_script_file();
	extern void start_window_input();

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

	screen_cursor= cursor_create(0);

	moveCursor(0, 0);

	if (display_enabled) {
		static struct rect frame_rect= {0, 0, MAXX, MAXY};
		/* Create the "window" ("frame", in SunView lingo).. */
		st_frame= window_create(NULL, FRAME,
				    FRAME_SHOW_LABEL, FALSE,
				    FRAME_ICON, &icon,
				    FRAME_CLOSED, TRUE,
				    WIN_RECT, &frame_rect,
				    WIN_SHOW, TRUE,
				    WIN_ERROR_MSG, "Can't create window", 0);
		/* ..and then a "canvas" (bitmap) to draw into.
		   The refresh_screen function will be called whenever
		   there is any window damage. */
		st_canvas= window_create(st_frame, CANVAS,
					  CANVAS_AUTO_SHRINK, TRUE,
					  CANVAS_AUTO_EXPAND, TRUE,
					  WIN_SHOW, TRUE,
					  CANVAS_RETAINED, FALSE,
					  CANVAS_REPAINT_PROC,
					  refresh_screen, 0);
		/* get the size of the window .. */
		dispW= (int) window_get(st_canvas, CANVAS_WIDTH);
		dispH= (int) window_get(st_canvas, CANVAS_HEIGHT);
		/* ...and the pixwin (bitmap) that lies within. */
		screen_pixwin= canvas_pixwin(st_canvas);
		pw_writebackground(screen_pixwin, 0, 0, MAXX, MAXY, PIX_CLR);
		start_window_input();
	} else {
		dispW= MAXX;
		dispH= MAXY;
	}
	theScreen= mem_create(dispW, dispH, 1);
	if (theScreen == NULL) {
		fatal("Can't create a screen bitmap!");
	}
}

/* st_event_handler() is called by the SunView library whenever there
   is a pending event of interest.  It decodes the event, and adds it
   into the Smalltalk event queue.  This is perhaps the most suitable
   of all input models, as no time is wasted polling the interface.
   Any ports of this module to other machines should try to emulate
   this version if at all possible. 
*/

static void st_event_handler(canvas, event, arg)
Canvas          canvas;
Event          *event;
caddr_t         arg;
{
	extern void do_time(), addEvent();
	int event_type;
	struct timeval  timev;
	int dx,dy;
	short code= NO_EVENT;
	TIME Mms_intvl; /* interval in ms since last event and mouse event */
	TIME now;
	static unsigned short lastM_ms= 0; /* time of last mouse event */

	static int OldMouseX, OldMouseY;

	if (io_mode == script_in)
	  return;

	timev= event_time(event);
	event_type= event_id(event);
	now= 1000*timev.tv_sec + timev.tv_usec/1000; /* ignore overflow */
	Mms_intvl= now - lastM_ms;


	/* First we must calculate millisecond interval since last 
	 * event. If this is less than sampleInterval, then we ignore
	 * any mouse motion.. If not, then we calculate time since last event,
	 * check whether it will fit in 12 bits and encode it into an
	 * event structure (and then send it!).
	 */

	do_time(now);

	/* So now we've put the time in, we'll add the actual event
	   into the buffer. */

	switch (event_type= event_id(event)) {
	case SHIFT_CTRL:
		code= CTRLKEY;
		break;
	case SHIFT_LEFT:
		code= LSHIFTKEY;
		break;
	case SHIFT_RIGHT:
		code= RSHIFTKEY;
		break;
	case SHIFT_CAPSLOCK:
		code= ALPHAKEY;
		break;
	case MS_LEFT:
		code= LEFTSW;
		break;
	case MS_MIDDLE:
		code= MIDDLESW;
		break;
	case MS_RIGHT:
		code= RIGHTSW;
		break;
	default:
		if (event_type < 128) {
		  	code= event_type;
			if (event_ctrl_is_down(event)) {
				code += 'a' - 1;
			} else if (event_shift_is_down(event)) {
				code= ShiftMap[code];
			}
		}
		break;
	}
	if (code != NO_EVENT)
	  	addEvent((event_is_up(event) ? OFF_BISTATE : ON_BISTATE)
			 + code);

	MouseX= (int) event_x(event);
	MouseY= (int) event_y(event);
	dx= MouseX - OldMouseX;
	dy= MouseY - OldMouseY;

	if ( ((dx <= -PXL_INCR) || (dx >= PXL_INCR)) &&
	    (Mms_intvl >= SampleInterval) ) {
		  /* then detect x-axis movement: */
		  addEvent(X_LOCTN | ((short)MouseX) & 0xFFF);
		  OldMouseX= MouseX;
		  /* update mouse sampling time */
		  lastM_ms= now;
	  }
	
	if ( ((dy <= -PXL_INCR) || (dy >= PXL_INCR)) &&
	    (Mms_intvl >= SampleInterval) ) {
		/* then detect x-axis movement: */
		addEvent(Y_LOCTN | ((short)MouseY) & 0xFFF);
		OldMouseY= MouseY;
		/* update mouse sampling time */
		lastM_ms= now;
	}
	return;
}

/* refresh_screen() is called from the SunView library whenever window
   damage has occurred to the Smalltalk window.
   Rather than try to be smart and update only the damaged areas, we
   simply redraw the whole window.
*/
static void refresh_screen(canvas, pixwin, ignored)
Canvas canvas;
Pixwin *pixwin;
Rectlist *ignored;
{
	pw_rop(pixwin, 0, 0, dispW, dispH, PIX_SRC, theScreen, 0, 0);
}


/* start_window_input() tells the SunView library to pass on the
   input events that the Smalltalk system is interested in.
   It also passes in the name of the event-handling function.
   See the SunView Programmer's Manual for more detail.
*/
static void start_window_input()
{
	extern void st_event_handler();

	window_set(st_canvas, WIN_CONSUME_KBD_EVENTS, WIN_ASCII_EVENTS,
		   SHIFT_LEFT, SHIFT_RIGHT, 
		   SHIFT_CAPSLOCK, SHIFT_CTRL,
		   WIN_UP_ASCII_EVENTS, WIN_UP_EVENTS, 0,
		   WIN_IGNORE_KBD_EVENTS,
		   KEY_LEFT(2), KEY_LEFT(3),
		   KEY_LEFT(4), KEY_LEFT(5),
		   KEY_LEFT(6), KEY_LEFT(7),
		   KEY_LEFT(8), KEY_LEFT(9), KEY_LEFT(10),
		   KEY_TOP(2), KEY_TOP(3),
		   KEY_TOP(4), KEY_TOP(5),
		   KEY_TOP(6), KEY_TOP(7),
		   KEY_TOP(8), KEY_TOP(9), KEY_TOP(1),
		   KEY_RIGHT(1),
		   KEY_RIGHT(2), KEY_RIGHT(3),
		   KEY_RIGHT(4), KEY_RIGHT(5),
		   KEY_RIGHT(6), KEY_RIGHT(7),
		   KEY_RIGHT(8), KEY_RIGHT(9), KEY_RIGHT(10),
		   KEY_RIGHT(11), KEY_RIGHT(12), KEY_RIGHT(13),
		   KEY_RIGHT(14), KEY_RIGHT(15), 0,
		   WIN_CONSUME_PICK_EVENTS, WIN_UP_EVENTS, 0,
		   WIN_IGNORE_PICK_EVENTS, LOC_RGNENTER,
		   LOC_RGNEXIT, 0,
		   WIN_EVENT_PROC, st_event_handler,
		   0);
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

void doInput()
{
	extern void read_script_event(), moveCursor();

	if (inputSema == NIL_PTR)
		return;

	(void)notify_dispatch();
	
	if (io_mode == script_in) {   /* reading from a script? */
		read_script_event();
	}
}

void moveCursor(x, y)        /* place cursor at given position */
int x, y;               /* The "linked" status is not affected */
{
	if (display_enabled) {
		window_set(st_canvas, WIN_MOUSE_XY, x, y, WIN_SHOW, TRUE, 0);
	}
	if (linked) {           /* update the mouse position to 
				   reflect the cursor position */
		MouseX= x;  MouseY= y;
	}
}




void newCursor(cursor)
Pixrect *cursor;
{
	if (display_enabled) {
		cursor_set(screen_cursor, CURSOR_IMAGE, cursor, 0);
		window_set(st_canvas, WIN_CURSOR, screen_cursor, 0);
	}
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
				loctnb(oop)= pr_load(image, NULL);
				if (loctnb(oop) == NULL) {
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
		fatal("Cannot convert bitmap!");
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
	extern void start_window_input();
	start_window_input();
}
