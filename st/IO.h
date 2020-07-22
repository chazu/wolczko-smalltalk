/*
  	Smalltalk-80 Virtual Machine:
	Parameterisation of Input and Output

	Copyright (c) 1984,1985,1986,1987,1988,1989 Mario Wolczko
	All rights reserved.

$Log: IO.h,v $
 * Revision 5.0  1994/08/16  00:58:47  mario
 * Sync with others
 *
 * Revision 4.1  89/06/21  19:52:06  mario
 * Moved into this module definitions required by both IO.c and MachineIO.c.
 * 
 * Revision 3.1  87/03/11  15:16:55  miw
 * Removed support for Perq.
 * 
 * Revision 2.1  85/11/19  17:39:51  miw
 * Stabilised with all other modules at Rev 2.1
 * 
 * Revision 1.3  85/11/14  16:34:47  miw
 * Added LANDSCAPE definition for PERQs.
 * 
 * Revision 1.2  85/10/28  18:41:20  miw
 * Added $Log: IO.h,v $
 * Revision 5.0  1994/08/16  00:58:47  mario
 * Sync with others
 *
 * Revision 4.1  89/06/21  19:52:06  mario
 * Moved into this module definitions required by both IO.c and MachineIO.c.
 * 
 * Revision 3.1  87/03/11  15:16:55  miw
 * Removed support for Perq.
 * 
 * Revision 2.1  85/11/19  17:39:51  miw
 * Stabilised with all other modules at Rev 2.1
 * 
 * Revision 1.3  85/11/14  16:34:47  miw
 * Added LANDSCAPE definition for PERQs.
 * 
 * 
*/

#include <sys/types.h>
#include <sys/timeb.h>
#include "ST.h"
#include "StdPtrs.h"

			/* whether input/output is real, or script-based */
enum IO_mode { real_time, script_in, script_out };

	/* logical key numbers of mouse buttons, G/R p.650 */
#define LEFTSW          130
#define MIDDLESW        129
#define RIGHTSW         128

	/* and key codes */
#define LSHIFTKEY	136
#define RSHIFTKEY	137
#define CTRLKEY		138
#define ALPHAKEY	139

	/* types of event, G/R p.649 */
#define DELTA_TIME      (0<<12)   /* not used */
#define X_LOCTN         (1<<12)
#define Y_LOCTN         (2<<12)
#define ON_BISTATE      (3<<12)
#define OFF_BISTATE     (4<<12)
#define ABSOLUTE_TIME   (5<<12)


#define LEFT		4
#define MIDDLE		2
#define RIGHT		1

#define BUTTONS         (LEFT | MIDDLE | RIGHT)


	/* screen bitmap parameters */
extern unsigned dispW, dispH;          /* height and width of display */

extern OOP inputSema;        /* input semaphore */

	/* stuff for recording/playing back sessions */
extern enum IO_mode io_mode;
extern FILE *script_file;
extern char *script_file_name;
extern BOOL display_enabled;

extern short MouseX, MouseY;
extern TIME SampleInterval;

extern BOOL linked;

