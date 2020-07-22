/*
  	Smalltalk-80 Virtual Machine:
	Macro to invoke a new method

	Copyright (c) 1984,1985,1986,1987,1988,1989 Mario Wolczko
	All rights reserved.

$Header: /home/mario/st/RCS/Exec.h,v 5.0 1994/08/16 00:58:19 mario Exp mario $
$Log: Exec.h,v $
 * Revision 5.0  1994/08/16  00:58:19  mario
 * Sync with others
 *
 * Revision 4.1  89/06/21  19:48:29  mario
 * Stabilised with other files at 4.1.
 * 
 * Revision 3.1  87/03/11  15:10:54  miw
 * No D-B; stabilised with other modules.
 * 
 * Revision 2.1  85/11/19  17:38:39  miw
 * Stabilised with all other modules at Rev 2.1
 * 
 * Revision 1.4  85/07/23  20:58:43  miw
 * Removed primitive trace.
 * 
 * Revision 1.3  85/07/05  12:57:56  miw
 * Removed MSG_TRACE alternative.
 * 
 * Revision 1.2  85/03/06  14:56:58  miw
 * Added \\n in trace messages (important for Apollo version)
 * 
 * Revision 1.1  85/01/10  12:52:57  miw
 * Initial revision
 * 
*/

					/*ST!Interpreter!executeNewMethod!*/
					/*ST!Interpreter!primitiveResponse!*/
					/*ST!Interpreter!quickReturnSelf!*/
					/*ST!Interpreter!quickInstanceLoad!*/
					/*ST!Interpreter!initPrimitive!*/
					/*ST!Interpreter!dispatchPrimitives!*/
					/*ST!Interpreter!success!*/
					/*ST!Interpreter!success:!*/

#define executeNewMethod                                                                \
	{                                                                               \
		/* primitive Response ? */                                              \
		if (primIndex == 0) {                                                   \
			WORD flag= flagValue(newMethod);                                \
			if (flag != 5)                                                  \
				if (flag == 6) { /* quick instance load */              \
					push(fetchPtr(fieldIndex(newMethod), popStack));\
				} else                                                  \
					activateNewMethod();                            \
		} else if ((*dispPrim[primIndex])()) { /* dispatchPrimitives */         \
				/* primitive failure */                                 \
				activateNewMethod();                                    \
		}                                                                       \
	}

