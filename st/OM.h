/*
  	Smalltalk-80 Virtual Machine:
	Routines and data structures provided by the Object Memory Module 

	Copyright (c) 1984,1985,1986,1987,1988,1989 Mario Wolczko
	All rights reserved.

$Header: /home/mario/st/RCS/OM.h,v 5.0 1994/08/16 00:59:25 mario Exp mario $
$Log: OM.h,v $
 * Revision 5.0  1994/08/16  00:59:25  mario
 * Sync with others
 *
 * Revision 4.1  89/06/21  19:55:59  mario
 * Added X11 support, attempted to machine things more machine-independent.
 * 
 * Revision 3.1  87/03/11  16:15:35  miw
 * Removed D-B; stabilised with other modules.
 * 
 * Revision 2.3  85/12/07  15:38:03  miw
 * Split otRest[] into otCount[] and otRest[].
 * 
 * Revision 2.2  85/11/23  17:01:42  miw
 * Added new arrays for sizes and classes of objects.
 * 
 * Revision 2.1  85/11/19  17:41:22  miw
 * Stabilised with all other modules at Rev 2.1
 * 
 * Revision 1.3  85/08/07  08:30:45  miw
 * Added LOC type.
 * 
 * Revision 1.2  85/06/27  19:04:25  miw
 * Split the objert table into two.
 * 
 * Revision 1.1  85/01/10  12:54:51  miw
 * Initial revision
 * 
*/

extern WORD om[];

typedef union {
	ADDR	addr;
	WORD	index;	/* used for the free chain */
	BITMAP	bmap;	/* used to reference external bitmaps */
} LOC;

/* we need to keep the width and height of bitmaps in the size field
   when in X11 */
#ifdef SUN
  typedef WORD Size_fld;
#else
  typedef union {
	  WORD	nbytes;
	  struct {
		  unsigned short w, h;
	  } extent;
  } Size_fld;
#endif

extern LOC otLoc[];
extern BYTE otCount[], otRest[];
extern OOP FCLhd[];
extern Size_fld Sizes[];
extern OOP Classes[];

extern OOP allocate(), initialInstance(), instanceAfter();
extern void IO_exit(), swapPtrs(), c_down(), c_down_inner();

extern OOP free_ptr;
extern unsigned oops_left, words_left, ptr_limit, word_limit;
extern OOP limitSema;
extern BOOL lowOnSpace;

/* access to the display bitmap .. I/O system dependent */
#ifdef X11
  extern Display *display;
#endif

extern BITMAP theScreen;

