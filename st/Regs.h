/*
  	Smalltalk-80 Virtual Machine:
	Registers of the Virtual Machine

	Copyright (c) 1984,1985,1986,1987,1988,1989 Mario Wolczko
	All rights reserved.

$Header: /home/mario/st/RCS/Regs.h,v 5.0 1994/08/16 01:00:02 mario Exp mario $
$Log: Regs.h,v $
 * Revision 5.0  1994/08/16  01:00:02  mario
 * Sync with others
 *
 * Revision 4.1  89/06/21  19:59:03  mario
 * Stabilised with other files at 4.1.
 * 
 * 
 * Revision 3.1  87/03/11  16:45:22  miw
 * Removed ABSrcvr.
 * 
 * Revision 2.2  85/12/12  10:30:32  miw
 * Deleted instrPtr and stackPtr (offsets in method and activeContext), and
 * added ip, sp, ABSrcvr andd
 * ABSmethod as direct pointers.
 * 
 * Revision 2.1  85/11/19  17:42:09  miw
 * Stabilised with all other modules at Rev 2.1
 * 
 * Revision 1.2  85/09/13  10:46:57  miw
 * Added direct pointers to active and home contexts.
 * 
 * Revision 1.1  85/01/10  12:56:05  miw
 * Initial revision
 * 
*/

extern OOP activeContext, homeContext, receiver, method, msgSelector,
		newMethod;
extern WORD argCount;
extern BYTE currentBC, primIndex;
extern CONTEXT *ac, *hc;
extern BYTE *ip;
extern WORD *sp, *ABSmethod;

