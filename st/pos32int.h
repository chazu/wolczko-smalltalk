/*
  	Smalltalk-80 Virtual Machine:
	routine for converting a 32 bit integer into either a SmallInteger
	or a LargePositiveInteger.

	Copyright (c) 1984,1985,1986,1987,1988,1989 Mario Wolczko
	All rights reserved.

$Header: /home/mario/st/RCS/pos32int.h,v 5.0 1994/08/16 01:01:13 mario Exp mario $
$Log: pos32int.h,v $
 * Revision 5.0  1994/08/16  01:01:13  mario
 * Fix indexing.
 *
 * Revision 4.1  89/06/22  11:46:21  mario
 * Initial version taken from old IO.c.
 * Stabilised with other modules at 4.1.
 * 
 * Revision 3.1  87/03/11  16:53:58  miw
 * Removed ZCT stuff; stabilised with other modules.
 * 
 * Revision 1.1  87/02/13  19:35:49  miw
 * Initial revision
 * 
 * Revision 2.1  85/11/19  17:42:58  miw
 * Stabilised with all other modules at Rev 2.1
 * 
 * Revision 1.1  85/01/14  17:51:21  miw
 * Initial revision
 * 

*/

					/*ST!Interpreter!positive16BitIntegerFor:!*/
/* BEWARE of side effects involving arg 2 */
#define pos32Int(val, result)                                                   \
	{                                                                       \
		WORD value= (val);                                             \
		if (value <= MAX_INT)                                           \
			result= intObj(value);                                 \
		else {                                                          \
			result= instBytes(CLASS_LG_POS_INT, 4);                \
			storeByte(0, result, value & 0xFF);                     \
			storeByte(1, result, value >> 8);                       \
			storeByte(2, result, value >> 16);                      \
			storeByte(3, result, value >> 24);                      \
		}                                                               \
	}

