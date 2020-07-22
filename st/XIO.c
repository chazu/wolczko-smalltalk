static char *rcsid= "$Header: /home/mario/st/RCS/XIO.c,v 5.0 1994/08/16 01:01:05 mario Exp mario $";

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

$Log: XIO.c,v $
 * Revision 5.0  1994/08/16  01:01:05  mario
 * Fix for colour display
 *
 * Revision 4.2  91/06/20  21:51:37  mario
 * Fixed bug in bitmap setup: was assuming that new bitmaps were initially
 * white.
 * 
 * Revision 4.1  89/06/22  11:45:51  mario
 * Initial version, loosely based on old IO.c.
 * 
*/

#include <stdio.h>
#include <signal.h>
#include <sgtty.h>
#include <ctype.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/keysym.h>
#include "IO.h"
#include "smalltalk.xicon"
#include "OM.h"
#include "OMMacros.h"



#define PXL_INCR	1

Display *display;
Pixmap theScreen,
       pix1; /* used only as a 1-bit deep parent */
int screen;
Window win;
XWindowAttributes win_attr;
static XSetWindowAttributes attr;
static XColor black, white;

/*
	IO_init() should be called once at the start of
	interpretation to place all the I/O devices in the correct
	modes.
*/

void IO_init()        /* must call this immediately before doing any input */
{
	extern void IO_exit(), moveCursor(), input_enabled();
	extern void read_first_script_event(), open_script_file();
	extern Pixmap create_pixmap();
	extern BOOL mapped;
	XEvent report;
	XSizeHints size_hints;
	Pixmap icon_pixmap;

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

	moveCursor(0, 0);

	if (!display_enabled) {
		fatal("Can't disable the X11 display!");
	}

	/* In X we have to create a window (suggesting a default size
	   to the window manager), and then determine what size the
	   user/window manager chose. */

	/* We'll use the display selected by the DISPLAY environment
	   variable.  For completeness, we should also allow the user
	   to pass a display on the command line, but life's too
	   short for such things... */
	display= XOpenDisplay(NULL);
	if (display == NULL) {
		(void)fprintf(stderr, "cannot open display %s\n",
			      XDisplayName(NULL));
		exit(1);
	}
	screen= DefaultScreen(display);

	/* Make the default window size smaller than the display,
	   because full-size windows can cause trouble for some
	   window managers.
	   Again, completeness suggests we accept a geometry
	   argument, but...*/
	dispW= DisplayWidth(display, screen) * 3/4;
	dispH= DisplayHeight(display, screen) * 3/4;

	/* don't want mouse clicks passed to the window
	   manager ... */
	attr.do_not_propagate_mask= KeyPressMask|KeyReleaseMask
	  |ButtonPressMask|ButtonReleaseMask|PointerMotionMask;
	attr.background_pixel= WhitePixel(display, screen);
	win= XCreateWindow(display, RootWindow(display, screen),
			   0, 0, dispW, dispH, 0, 0, /*same depth as display*/
			   InputOutput, DefaultVisual(display, screen),
			   CWBackPixel|CWDontPropagate, &attr);
	/* Get the window attributes, principally to find out the depth */
	if (XGetWindowAttributes(display, win, &win_attr) == 0) {
		(void)fprintf(stderr, "cannot get window attributes\n");
		exit(1);
	}

	icon_pixmap= XCreateBitmapFromData(display, win,
					   smalltalk_bits,
					   smalltalk_width,
					   smalltalk_height);
	size_hints.flags= PSize | PMinSize;
	size_hints.width= dispW;
	size_hints.height= dispH;
	size_hints.min_width= 640;
	size_hints.min_height= 480;
	XSetStandardProperties(display, win, "st80", "st80", icon_pixmap,
			       NULL, 0, &size_hints);

	/* let's try to map the window ... */
	XSelectInput(display, win, StructureNotifyMask);
	XMapWindow(display,win);
	mapped= TRUE;
	do {		/* wait for window to be mapped */
		XNextEvent(display, &report);
	} while (report.type != ConfigureNotify && report.type != Expose);
	input_enabled(FALSE);

	/* At last we have a window.  Let's grab the size before the
	   user changes her mind. */
	if (report.type == ConfigureNotify) {
		dispW= report.xconfigure.width;
		dispH= report.xconfigure.height;
	} else {
		dispW= report.xexpose.width;
		dispH= report.xexpose.height;
	}

	/* The Smalltalk display object will be an X Pixmap.  We
	   could have requested backing store for the window instead,
	   but some servers don't provide it.  Maybe I'll put in
	   an option to do that sometime...*/
	theScreen= create_pixmap(dispW, dispH);

	/* While we're here, we may as will set up random other X
	   thingies...*/
	{ static void setup_X_image_format();
	  setup_X_image_format(); }

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
	/* what about destroy events? */
	/* enter, leave window?  */
	if (en && io_mode != script_in) {
		XSelectInput(display, win,
			     ExposureMask|StructureNotifyMask			     			    |KeyPressMask|KeyReleaseMask
			     |ButtonPressMask|ButtonReleaseMask
			     |PointerMotionMask);
	} else {
		XSelectInput(display, win,
			     ExposureMask|StructureNotifyMask);
	}
}

void doInput()
{
	extern void add_to_updates(), addEvent(), do_time();
	extern void do_time(), update_screen(), read_script_event();
	static int OldMouseX, OldMouseY;
	extern BOOL mapped;
	unsigned Mms_intvl;
	static Time lastM_ms;
	Time now;
	int dx,dy;
	short but, state;
	KeySym key;
	XEvent report;
	int num_to_do;

	update_screen();

	if (inputSema == NIL_PTR)
		return;
	if (io_mode == script_in) {
		read_script_event();
	}

	num_to_do= XPending(display);
	while (num_to_do-- > 0) {
		XNextEvent(display, &report);
		switch (report.type) {
		case Expose:
			add_to_updates(report.xexpose.x, report.xexpose.y,
				       report.xexpose.width,
				       report.xexpose.height);
			break;
		case MotionNotify:
			now= report.xmotion.time;
			do_time((TIME)now);
			Mms_intvl= (TIME)now - lastM_ms;
			MouseX= report.xmotion.x;
			MouseY= report.xmotion.y;
			dx= MouseX - OldMouseX;
			dy= MouseY - OldMouseY;
			if ((dx <= -PXL_INCR || dx >= PXL_INCR)
			    && Mms_intvl >= SampleInterval) {
				/* then detect x-axis movement: */
				addEvent(X_LOCTN
					 | ((short)MouseX) & 0xFFF);
				OldMouseX= MouseX;
				/* update mouse sampling time */
				lastM_ms= now;
			}
			
			if ((dy <= -PXL_INCR || dy >= PXL_INCR)
			    && Mms_intvl >= SampleInterval) {
				/* then detect x-axis movement: */
				addEvent(Y_LOCTN
					 | ((short)MouseY) & 0xFFF);
				OldMouseY= MouseY;
				/* update mouse sampling time */
				lastM_ms= now;
			}
			break;			
		case ButtonPress:
		case ButtonRelease:
			do_time((TIME)report.xbutton.time);
			switch (report.xbutton.button) {
			case Button1:
				but= LEFTSW;
				break;
			case Button2:
				but= MIDDLESW;
				break;
			case Button3:
				but= RIGHTSW;
				break;
			default:
				log("unknown button!");
				goto no_but;
			}
			state= report.type==ButtonPress
			  ? ON_BISTATE : OFF_BISTATE;
			addEvent(state | but);
		no_but: break;
		case KeyPress:
		case KeyRelease:
			do_time((TIME)report.xkey.time);
			state= report.type==KeyPress
			  ? ON_BISTATE : OFF_BISTATE;
			key= XKeycodeToKeysym(display,
					      report.xkey.keycode, 0);
			switch (key) {
			case XK_Shift_L: key= LSHIFTKEY; break;
			case XK_Shift_R: key= RSHIFTKEY; break;
			case XK_Control_L: key= CTRLKEY; break;
			case XK_Caps_Lock: key= ALPHAKEY; break;
			case XK_BackSpace:
			case XK_Tab: 
			case XK_Linefeed: 
			case XK_Return: 
			case XK_Escape:
			case XK_Delete:
				key &= 0x7F;
				break;
			case NoSymbol:
				log("unknown key!");
				goto no_key;
			}
			if (key > 255) {
				log("unknown keysym ... ignored");
				goto no_key;
			}
			addEvent(state | key);
		no_key: break;
		case ConfigureNotify:
			if (report.xconfigure.width != dispW
			    || report.xconfigure.height != dispH) {
				/* grab a new display */
				extern Pixmap create_pixmap();
				extern void set_display_form();
				XFreePixmap(display, theScreen);
				dispW= report.xconfigure.width;
				dispH= report.xconfigure.height;
				theScreen= create_pixmap(dispW, dispH);
				set_display_form(theScreen);
			}
			break;
		case MapNotify:
			mapped= TRUE;
			break;
		case UnmapNotify:
			mapped= FALSE;
			break;
		}
	}
}

Pixmap create_pixmap(w, h)
unsigned w, h;
{
  	Pixmap p= XCreatePixmap(display, win, w, h, 1);
	if (p == None) {
		fatal("Cannot allocate X pixmap!");
	}
	return p;
}

void moveCursor(x, y)        /* place cursor at given position */
int x, y;               /* The "linked" status is not affected */
{
	if (x != MouseX || y != MouseY)
	  XWarpPointer(display, win, win, 0, 0, dispW, dispH, x, y);
	if (linked) {           /* update the mouse position to 
				   reflect the cursor position */
		MouseX= x;  MouseY= y;
	}
}




void newCursor(cursor)
Pixmap cursor;
{
	static Cursor x_cursor= None;
	x_cursor= XCreatePixmapCursor(display, cursor, cursor,
				      &black, &white, 0, 0); 
	XDefineCursor(display, win, x_cursor);
	XFreeCursor(display, x_cursor);
	XFlush(display);
}


void setLinked(l)		/* link/unlink cursor and mouse */
BOOL l;
{
	linked= l;
}



void load_bitmaps(image)
FILE *image;
{
	extern Pixmap create_bitmap(), convert_bm();
	extern int fseek();
	extern char *malloc();

	for_every_oop(oop) {
		if (!isFree(oop) && bitmap(oop)) {
			if (loctni(oop) != 0) {
				Pixmap p;
				unsigned short w= bm_w(oop), h= bm_h(oop);
				unsigned sz= h*((w+15)&~15)/8;
				(void)fseek(image, (long)(loctni(oop)), 0);
				loctn(oop)= (WORD *)malloc(sz);
				if (loctn(oop) == NULL
				    || fread((char *)loctn(oop), sz, 1,
					     image) != 1) {
					fatal("Error loading bitmap!");
				}
				p= create_bitmap(w, h, (WORD16 *)loctn(oop));
				free((char *)loctn(oop));
				loctnb(oop)= p;
			} else {
				loctnb(oop)= theScreen;
			}
		}
	}}
}


BOOL save_bitmaps(image,object_table_locs)
FILE *image;
unsigned *object_table_locs;
{
	extern char *malloc();

	for_every_oop(oop) {
		if (isFree(oop))
		  continue;
		if (bitmap(oop)) {
			if (loctnb(oop) != theScreen) {
				extern int screen;
				unsigned short w= bm_w(oop);
				unsigned short h= bm_h(oop);
				int sz= h*((w+15)&~15)/8;
				char *data= malloc((unsigned)sz);
				XImage *img;

				if (data == NULL)
				  return TRUE;
				/* see comments below about ZPixmap */
				img= XCreateImage(display,
						  DefaultVisual(display, screen),
						  1, ZPixmap, 0, data,
						  w, h, 16, 0);
				if (img == NULL) {
					free(data);
					return TRUE;
				}
				img->byte_order= MSBFirst;
				img->bitmap_bit_order= MSBFirst;
				(void)XGetSubImage(display, loctnb(oop),
						  0, 0, w, h, 1, ZPixmap,
						  img, 0, 0);
				
				/* BEWARE if you change the oop format */
				object_table_locs[oop]= (unsigned)ftell(image);
				if (fwrite(data, sz, 1, image) != 1)
				  return TRUE;
				free(data);
				XDestroyImage(img);
			} else {
				object_table_locs[oop]= 0;
			}
		} else {
			object_table_locs[oop]= loctn(oop) - om;
		}
	}}
	return FALSE;
}


Pixmap create_bitmap(w, h, data)
WORD w, h;
WORD16 *data;
{
	extern BITMAP black_ht;
	static Pixmap XCreateBitmapFromForm();
	Pixmap p= XCreateBitmapFromForm((char *)data, w, h);
	/* try to detect the all-black halftone in an attempt to
	   save work later... */
	if (w == 16 && h == 16) {
		WORD16 r= 0xFFFF;
		while (h--) r &= *data++;
		if (r == 0xFFFF) { /* found it! */
			black_ht= p;
		}
	}
	return p;
}


/*
 * XCreateBitmapFromForm: Routine to make a pixmap of depth 1 
 *	based on a Smalltalk Form.
 *      Data is a pointer to the bit data, and 
 *      width & height give the size in bits of the pixmap.
 *
 * The following format is assumed for data:
 *
 *    format=ZPixmap  (I have no idea why this works, but it does.  I copied
 *	it from the source of XCreateBitmapFromData)
 *    bit_order=MSBFirst
 *    byte_order=MSBFirst
 *    padding=0
 *    bitmap_unit=16
 *    xoffset=0
 *    no extra bytes per line
 * Modelled after an Xlib routine of a similar name.
 */  
static XImage ximage;
static GC gc, clear_gc;

static Pixmap XCreateBitmapFromForm(data, width, height)
char *data;
unsigned int width, height;
{
	Pixmap pix;
	BOOL is_white= TRUE;
	int n;
	char *p= data;

	ximage.height= height;
	ximage.width= width;
	ximage.data= data;
	ximage.bytes_per_line= ((width+15)/16)*2;
	pix= XCreatePixmap(display, win, width, height, 1);
	if (!pix) {
		fatal("Cannot allocate X pixmap!");
	}
	/* try to detect all-white forms and save down-loading them */
	for (n= height*ximage.bytes_per_line; n-- > 0;)
	  if (*p++) {
		  is_white= FALSE;
		  break;
	  }
	if (!is_white)
	  XPutImage(display, pix, gc, &ximage, 0, 0, 0, 0, width, height);
	else {
		XCopyArea(display, pix, pix, clear_gc, 0, 0,
			  width, height, 0, 0);
	}
	return pix;
}

static void setup_X_image_format() /* saves doing this for every Form */
{
	extern Pixmap CuCompact, CuGarbage;
	extern WORD16 CuCompactBits[], CuGarbageBits[];
	pix1= XCreatePixmap(display, win, 1, 1, 1);
	gc= XCreateGC(display, pix1, (unsigned long)0, (XGCValues *)0);
	XSetForeground(display, gc, BlackPixel(display, screen));
	XSetBackground(display, gc, WhitePixel(display, screen));
	XSetFunction(display, gc, GXcopy); 
	clear_gc= XCreateGC(display, pix1, (unsigned long)0, (XGCValues *)0);
	XSetForeground(display, clear_gc, BlackPixel(display, screen));
	XSetBackground(display, clear_gc, WhitePixel(display, screen));
	XSetFunction(display, clear_gc, GXclear); 
	ximage.depth= 1;
	ximage.xoffset= 0;
	ximage.format= ZPixmap;
	ximage.byte_order= MSBFirst;
	ximage.bitmap_unit= 16;
	ximage.bitmap_bit_order= MSBFirst;
	ximage.bitmap_pad= 16;
	black.pixel= BlackPixel(display, screen);
	white.pixel= WhitePixel(display, screen);
	XQueryColor(display, DefaultColormap(display, screen), &black);
	XQueryColor(display, DefaultColormap(display, screen), &white);

	/* initialise built-in cursors */
	CuCompact= create_bitmap(16, 16, CuCompactBits);
	CuGarbage= create_bitmap(16, 16, CuGarbageBits);
}

					       
BITMAP convert_bm(bm, w, h)
OOP bm;
WORD w, h;
{
	extern Pixmap create_bitmap();
	BITMAP new_bit_map;

	if (w == 0 || h == 0) {	/* see XBitBlt.c for explanation */
		(void)fprintf(stderr,
			      "strange BitBlt: oop=%08x w=%d, h=%d, bc_count=%d\n",
			      bm,w,h,bc_count);
		if (w==0) w= 1;
		if (h==0) h= 1;	/* what a kludge */
	}
	bm_w(bm)= w;
	bm_h(bm)= h;
	new_bit_map= create_bitmap(w, h, (WORD16 *)loctn(bm));
	return new_bit_map;
}

WORD16 CuCompactBits[]= {     /* cursor during compaction */
	0x0000, 0x0000, 0x003f, 0x07c2,
	0x0102, 0x013e, 0x0142, 0x01b9,
	0x7f45, 0x4083, 0x7101, 0x4911,
	0x8501, 0x8482, 0x4844, 0x3038
  };
WORD16 CuGarbageBits[]= {             /* cursor during garbage collection */
	0x0300, 0x1FF0, 0x2008, 0x2008,
	0x3FF8, 0x1010, 0x1550, 0x1550,
	0x1550, 0x1550, 0x1550, 0x1550, 
	0x1550, 0x1010, 0x1010, 0x3FF8
  };
Pixmap CuCompact, CuGarbage;




void re_start_window_input()
{
	extern void input_enabled();
	input_enabled(TRUE);
}
