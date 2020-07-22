/*
  	Smalltalk-80 Virtual Machine:
	Pre-defined message selectors

	Copyright (c) 1984,1985,1986,1987,1988,1989 Mario Wolczko
	All rights reserved.

$Header: /home/mario/st/RCS/Selectors.h,v 5.0 1994/08/16 01:00:07 mario Exp mario $
$Log: Selectors.h,v $
 * Revision 5.0  1994/08/16  01:00:07  mario
 * Sync with others
 *
 * Revision 4.1  89/06/21  19:59:46  mario
 * Stabilised with other files at 4.1.
 * 
 * Revision 3.1  87/03/11  16:47:07  miw
 * Stabilised with other modules.
 * 
 * Revision 2.2  85/12/07  15:37:28  miw
 * Halved the values of all the oops, as the tag bit has been moved from
 * the lsb to the msb.
 * 
 * Revision 2.1  85/11/19  17:42:22  miw
 * Stabilised with all other modules at Rev 2.1
 * 
 * Revision 1.1  85/01/10  12:56:48  miw
 * Initial revision
 * 

*/

					/*ST!Interpreter!initializeGuaranteedPointers!*/
#define DOES_NOT_UNDERSTAND     21
#define CANNOT_RETURN           22
#define MUST_BE_BOOLEAN         26
