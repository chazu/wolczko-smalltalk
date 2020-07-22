/*
  	Smalltalk-80 Virtual Machine:
	Fields of an instance of BitBlt

	Copyright (c) 1984,1985,1986,1987,1988,1989 Mario Wolczko
	All rights reserved.

$Header: /home/mario/st/RCS/BitBlt.h,v 5.0 1994/08/16 00:57:56 mario Exp mario $
$Log: BitBlt.h,v $
 * Revision 5.0  1994/08/16  00:57:56  mario
 * Sync with others
 *
 * Revision 4.1  89/06/21  19:45:39  mario
 * Removed references to suntools.
 * 
 * Revision 3.1  87/03/11  14:38:45  miw
 * Removed support for PERQ and ORION.
 * 
 * Revision 2.5  87/02/06  12:17:19  miw
 * Added support for Suntools
 * 
 * Revision 2.4  86/09/16  11:42:28  miw
 * Added case for SUN
 * 
 * Revision 2.3  86/01/17  19:14:47  miw
 * Added NO_FORM for VAX and ORION versions.
 * 
 * Revision 2.2  85/12/04  20:14:47  miw
 * Added NO_FORM constant.
 * 
 * Revision 2.1  85/11/19  17:37:49  miw
 * Stabilised with all other modules at Rev 2.1
 * 
 * Revision 1.1  85/07/05  12:53:20  miw
 * Initial revision
 * 

*/

#define DEST_FORM       0
#define SRC_FORM        1
#define HT_FORM         2
#define COMB_RULE       3
#define DEST_X          4
#define DEST_Y          5
#define WIDTH           6
#define HEIGHT          7
#define SRC_X           8
#define SRC_Y           9
#define CLIP_X          10
#define CLIP_Y          11
#define CLIP_W          12
#define CLIP_H          13

#define FORM_BITS       0
#define FORM_WIDTH      1
#define FORM_HEIGHT     2

#define NO_FORM		NULL
