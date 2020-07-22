/* 
  	Smalltalk-80 Virtual Machine:
	types and constants of use in the Smalltalk Virtual Machine

	Copyright (c) 1984,1985,1986,1987,1988,1989 Mario Wolczko
	All rights reserved.

$Header: /home/mario/st/RCS/ST.h,v 5.0 1994/08/16 01:00:05 mario Exp mario $
$Log: ST.h,v $
 * Revision 5.0  1994/08/16  01:00:05  mario
 * Sync with others
 *
 * Revision 4.1  89/06/21  19:59:19  mario
 * Added interface to logging functions (log*, fatal).
 * Stabilised with other files at 4.1.
 * 
 * Revision 3.2  89/05/03  15:55:31  mario
 * undef'd min and max.
 * 
 * Revision 3.1  87/03/11  16:46:29  miw
 * Added BM_WORD typedef for bitmaps
 * 
 * Revision 2.2  87/02/13  20:05:30  miw
 * Converted to 32-bit image:
 * Changed WORD, SIGNED, MAX_INT and MIN_INT, L2BPW, ONES.
 * Added WORD16.
 * 
 * Revision 2.1  85/11/19  17:42:17  miw
 * Stabilised with all other modules at Rev 2.1
 * 
 * Revision 1.4  85/07/23  21:00:09  miw
 * Changed ADDR to be a direct pointer into object memory.
 * 
 * Revision 1.3  85/06/27  19:07:07  miw
 * Added definition of ADDR (previously in OMMacros.h).
 * 
 * Revision 1.2  85/01/14  17:58:27  miw
 * Added max and MaxBec macros
 * 
 * Revision 1.1  85/01/10  12:56:23  miw
 * Initial revision
 * 
*/

#ifndef _ST
#define _ST

#ifndef FALSE
#define FALSE   (0)
#endif
#ifndef TRUE
#define TRUE    (1)
#endif

#ifndef NULL
#define NULL    0
#endif

typedef int BOOL;

typedef unsigned WORD;

typedef WORD OOP;
typedef unsigned short WORD16;
typedef WORD16 EVENT;

typedef int SIGNED;

#define MAX_INT         (0x3FFFFFFF) /* 2^30 - 1 */
#define MIN_INT         (-(1<<30)) /* -2^30 */

typedef unsigned char BYTE;

typedef unsigned TIME;		/* 32 bit integer */

typedef WORD *ADDR;                  /* index into object memory */

/* Parameterisation for 16-bit (object memory) words. */

#define BPW     (sizeof(WORD)*8)        /* bits per word */
#define L2BPW   5                       /* log (base 2) BPW */
#define ONES    0xFFFFFFFF

/* this is a useful macro for assigning a minimum value.
   MinBec(x,y) is the same as if(x>y)x=y but more efficient
   as y is only evaluated once.
*/

#define MinBec(x, y)    { int t= (y); if (x > t) x= t; }

#undef min
#define min(a, b)       (a < b ? a : b)

#define MaxBec(x, y)    { int t= (y); if (x < t) x= t; }

#undef max
#define max(a, b)       (a > b ? a : b)

#ifdef X11
#  include <X11/Xlib.h>
   typedef Pixmap BITMAP;
#else
   typedef WORD16 BM_WORD;
#  ifdef SUN
#  include <pixrect/pixrect_hs.h>
   typedef Pixrect *BITMAP;
#  else
   typedef BM_WORD *BITMAP;
#  endif
#  define BITS    (sizeof(BM_WORD)*8)
#endif

/* internal bytecode count */
extern unsigned bc_count;
extern void log(), logs(), loge(), fatal();

#endif _ST
