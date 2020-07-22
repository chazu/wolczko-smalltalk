/*
  	Smalltalk-80 Virtual Machine:
	Frequencies (in bytecodes) of checking for process switches
	and inputs.

	Copyright (c) 1984,1985,1986,1987,1988,1989 Mario Wolczko
	All rights reserved.

$Header: /home/mario/st/RCS/BCFreq.h,v 5.0 1994/08/16 00:57:48 mario Exp mario $
$Log: BCFreq.h,v $
 * Revision 5.0  1994/08/16  00:57:48  mario
 * Sync with others
 *
 * Revision 4.1  89/06/21  19:45:16  mario
 * Increased INPUT_BC from 100 to 1000.
 * 
 * Revision 3.1  87/03/11  14:30:39  miw
 * Stabilised with other modules.
 * 
 * Revision 2.1  85/11/19  17:37:33  miw
 * Stabilised with all other modules at Rev 2.1
 * 
 * Revision 1.2  85/07/23  20:57:50  miw
 * Removed REFRESH_BC.
 * 
 * Revision 1.1  85/01/10  12:51:31  miw
 * Initial revision
 * 

*/

#define PROCESS_BC      50
#define INPUT_BC        1000
