/* 
  	Smalltalk-80 Virtual Machine:
	standard pointers of the system 

	Copyright (c) 1984,1985,1986,1987,1988,1989 Mario Wolczko
	All rights reserved.

$Header: /home/mario/st/RCS/StdPtrs.h,v 5.0 1994/08/16 01:00:20 mario Exp mario $
$Log: StdPtrs.h,v $
 * Revision 5.0  1994/08/16  01:00:20  mario
 * Sync with others
 *
 * Revision 4.1  89/06/22  11:42:59  mario
 * Stabilised with othermodules at 4.1.
 * 
 * Revision 3.1  87/03/11  16:50:40  miw
 * Stabilised with other modules.
 * 
 * Revision 2.4  87/02/13  19:45:34  miw
 * Converted values to 32-bit (change in position of tag bit).
 * 
 * Revision 2.3  85/12/07  15:38:36  miw
 * Halved the values of the oops to take into account the change in position of
 * the tag bit, and altered the values of the SmallInteger pointers as well.
 * Moved SMALLTALK from OM.c to here.
 * 
 * Revision 2.2  85/12/04  20:26:58  miw
 * Added UNUSED_PTR.
 * 
 * Revision 2.1  85/11/19  17:42:46  miw
 * Stabilised with all other modules at Rev 2.1
 * 
 * Revision 1.1  85/01/10  12:57:40  miw
 * Initial revision
 * 

*/
					/*ST!Interpreter!initializeSmallIntegers!*/
#define MINUS_ONE_PTR   0xFFFFFFFF
#define ZERO_PTR        0x80000000
#define ONE_PTR         0x80000001
#define TWO_PTR         0x80000002

					/*ST!Interpreter!initializeGuaranteedPointers!*/
#define UNUSED_PTR	0
#define NIL_PTR         1
#define FALSE_PTR       2
#define TRUE_PTR        3

#define SCHED_ASS_PTR           4

#define SPECIAL_SELECTORS       24
#define CHAR_TABLE              25
#define SMALLTALK       	9      /* the system dictionary, not defd in G&R */
