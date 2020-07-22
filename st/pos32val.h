/*
  	Smalltalk-80 Virtual Machine:
	routine for converting a SmallInteger or a LargePositiveInteger
	into a 32-bit int.

	Copyright (c) 1984,1985,1986,1987,1988,1989 Mario Wolczko
	All rights reserved.

$Header: /home/mario/st/RCS/pos32val.h,v 5.0 1994/08/16 01:01:19 mario Exp mario $
$Log: pos32val.h,v $
 * Revision 5.0  1994/08/16  01:01:19  mario
 * Sync with others
 *
 * Revision 4.1  89/06/22  11:46:39  mario
 * Stabilised with other modules at 4.1.
 * 
 * Revision 3.1  87/03/11  16:55:18  miw
 * Stabilised with other modules.
 * 
 * Revision 2.2  87/02/13  20:03:59  miw
 * Added a check to see if the input arg is indeed positive.
 * 
 * Revision 2.1  85/11/19  17:43:04  miw
 * Stabilised with all other modules at Rev 2.1
 * 
 * Revision 1.2  85/06/27  19:43:41  miw
 * Corrected syntax error
 * 
 * Revision 1.1  85/06/27  19:38:50  miw
 * Initial revision
 * 

*/

/* assumes res is a 32-bit quantity */
/* BEWARE of side effects involving arg 2 */
#define pos32val(intPtr, res, stackC)	{				\
	OOP oop1= (intPtr);						\
									\
	if (isInt(oop1)) {						\
		if ((SIGNED)(res= intVal(oop1)) < 0) {            	\
			unPop(stackC);                                  \
			return TRUE;                                    \
		}                                                       \
	} else if (fetchClass(oop1)==CLASS_LG_POS_INT && Blen(oop1)<=4) {\
		res= fetchByte(0, oop1) + fetchByte(1, oop1)*256;	\
		if (Blen(oop1) >= 3)					\
			res += 256*256*fetchByte(2, oop1);		\
		if (Blen(oop1) == 4)					\
			res += 256*256*256*fetchByte(3, oop1);		\
	} else {							\
		unPop(stackC);						\
		return TRUE;						\
	}								\
}
