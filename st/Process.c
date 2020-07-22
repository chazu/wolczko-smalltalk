/*
  	Smalltalk-80 Virtual Machine:
	Multi-processing module

	Copyright (c) 1984,1985,1986,1987,1988,1989 Mario Wolczko
	All rights reserved.

This module contains all the routines involved in multi-tasking
within the Smalltalk VM.  It can be called either by invoking
the ST multi-tasking primitives, or by input events.  Process
switches are checked for after a set number of bytecodes have
been executed, and after execution of process primitives.

$Log: Process.c,v $
 * Revision 5.0  1994/08/16  00:59:58  mario
 * Sync with others
 *
 * Revision 4.1  89/06/21  19:58:28  mario
 * Added new function to signal input semaphore.
 * Added declarations and casts to keep lint quiet.
 * 
 * Revision 3.3  89/05/03  15:54:40  mario
 * Added tracing of process switches.
 * 
 * Revision 3.2  87/03/21  14:46:20  miw
 * Added special version of firstContext() for simulation.
 * 
 * Revision 3.1  87/03/11  16:43:36  miw
 * Removed D-B.
 * 
 * Revision 2.2  85/12/12  10:31:31  miw
 * Due to addition of sp as a "register" of the virtual machine, had to
 * change an identifier from sp to schp.
 * 
 * Revision 2.1  85/11/19  17:41:57  miw
 * Stabilised with all other modules at Rev 2.1
 * 
 * Revision 1.3  85/07/05  13:10:10  miw
 * Changed push, storePtr to NR variants where possible.
 * 
 * Revision 1.2  85/06/27  19:06:32  miw
 * Altered order of include files.
 * 
 * Revision 1.1  85/01/10  12:55:49  miw
 * Initial revision
 * 

*/

static char *rcsid= "$Header: /home/mario/st/RCS/Process.c,v 5.0 1994/08/16 00:59:58 mario Exp mario $";

#include <stdio.h>

#include "BCFreq.h"

#include "Prim.h"

extern void trace_process_switch();

					/*ST!Interpreter!initializeSchedulerIndices!*/
#define PROC_LISTS           	0
#define ACTIVE_PROCESS          1

#define FIRST_LINK              0
#define LAST_LINK               1

#define EXCESS_SIGNALS          2

#define NEXT_LINK               0

#define SUSPENDED_CTX           1
#define PRIORITY                2
#define MY_LIST                 3




/* max. no. of semaphores that can be signalled between checks for
   process switches */
#define MAX_SEMAS       PROCESS_BC
		/* one entry per bytecode executed (for primitiveSignal) */



static OOP semaList[MAX_SEMAS];  /* the semaphores to be signalled at
				    the next opportunity */
static int semaIndex= 0;   /* the number of semaphores in the list */
/* The Blue Book maintains a single queue of semaphores to be signalled.
   However, most signals take place on the input semaphore, and can come
   in a rapid bursts due to mouse motion (especially when snapshotting).
   Hence, we maintain a separate count for the input semaphore to avoid
   filling the primary queue. */
static int inputSemaSignalCount= 0;  /* the no of pending input semaphore
					signals */


/* info. about a semaphore to be signalled at a specific time */
OOP     timedSema;             /* the semaphore itself */
BOOL    isTimedSema= FALSE;    /* whether the time should be checked */
TIME    semaWakeUpTime;        /* the time at which the semaphore
				  is to be signalled */


/* info. about whether a process switch might take place */
static OOP newProcess;          /* the new process */
static BOOL isNewProcess= FALSE;


					/*ST!Interpreter!schedulerPointer!*/
#define schedulerPtr            fetchPtr(VALUE, SCHED_ASS_PTR)

					/*ST!Interpreter!isEmptyList:!*/
#define isEmptyList(l)          (fetchPtr(FIRST_LINK, l)==NIL_PTR)

					/*ST!Interpreter!transferTo:!*/
#define xferTo(p)           { process_count= 0;    \
			      isNewProcess= TRUE;  \
			      newProcess= (p); }

					/*ST!Interpreter!sleep:!*/
static void addLastLink();
#define sleep(p, priority)      addLastLink(p, \
					    fetchPtr((priority)-1, \
						     fetchPtr(PROC_LISTS, \
							      schedulerPtr)))


					/*ST!Interpreter!activeProcess!*/
#define nextProcess             (isNewProcess ? \
				 newProcess : \
				 fetchPtr(ACTIVE_PROCESS, schedulerPtr))

					/*ST!Interpreter!suspendActive!*/
#define suspendActive           { xferTo(wakeHighestPriority()); }


extern int process_count;       /* defined in st80.c */


OOP ActiveProcess()	/* for external use */
{
	return fetchPtr(ACTIVE_PROCESS, fetchPtr(VALUE, SCHED_ASS_PTR));
}



					/*ST!Interpreter!checkProcessSwitch!*/
void checkProcessSwitch()
{
	extern TIME getTime();
	extern void asynchSignal();
	static void synchSignal();
	extern OOP inputSema;

	if (isTimedSema) {
		TIME now= getTime();
		if (now >= semaWakeUpTime) {
			asynchSignal(timedSema);
			isTimedSema= FALSE;
		}
	}

	while (semaIndex > 0)
		synchSignal(semaList[--semaIndex]);

	while (inputSemaSignalCount > 0) {
	  	synchSignal(inputSema);
		--inputSemaSignalCount;
	}

	if (isNewProcess) {
		OOP schp= schedulerPtr;
#ifdef SIM
		trace_process_switch(newProcess, activeContext);
#endif
		isNewProcess= FALSE;
		storePtr(SUSPENDED_CTX, fetchPtr(ACTIVE_PROCESS, schp),
			 activeContext);
		storePtr(ACTIVE_PROCESS, schp, newProcess);
		newActiveCtx(fetchPtr(SUSPENDED_CTX, newProcess));
	}
}


					/*ST!Interpreter!asynchronousSignal:!*/
void asynchSignal(s)
OOP s;
{
	if (semaIndex < MAX_SEMAS)
	  semaList[semaIndex++]= s;
	else
	  log("discarding excess signal");
	process_count= 0;
}

void signalInputSema()
{
	++inputSemaSignalCount;
	process_count= 0;
}


					/*ST!Interpreter!removeFirstLinkOfList:!*/
static OOP removeFirstLink(ll)
OOP ll;
{
	OOP     firstLink= fetchPtr(FIRST_LINK, ll),
		lastLink = fetchPtr(LAST_LINK, ll);

	/* the next line has been added so that Processes don't vanish
	   due to their ref. counts falling to zero */
	c_up(firstLink);

	if (firstLink == lastLink) {
		NRstorePtr(FIRST_LINK, ll, NIL_PTR);
		NRstorePtr(LAST_LINK, ll, NIL_PTR);
	} else
		storePtr(FIRST_LINK, ll, fetchPtr(NEXT_LINK, firstLink));
	storePtr(NEXT_LINK, firstLink, NIL_PTR);
	return firstLink;
}


					/*ST!Interpreter!synchronousSignal:!*/
static void synchSignal(s)
OOP s;
{
	static void resume();
	if (isEmptyList(s)) {
		fstoreInt(EXCESS_SIGNALS, s, 1+ffetchInt(EXCESS_SIGNALS, s));
	} else
		resume(removeFirstLink(s));
}


					/*ST!Interpreter!resume:!*/
static void resume(aProcess)
OOP aProcess;
{
	OOP active_process= nextProcess;
	WORD activePriority= ffetchInt(PRIORITY, active_process);
	WORD newPriority= ffetchInt(PRIORITY, aProcess);

	if (newPriority > activePriority) {
		sleep(active_process, activePriority);
		xferTo(aProcess);
	} else
		sleep(aProcess, newPriority);
}


					/*ST!Interpreter!addLastLink:toList:!*/
static void addLastLink(l, ll)
OOP l, ll;
{
	if (isEmptyList(ll)) {
		storePtr(FIRST_LINK, ll, l);
	} else
		storePtr(NEXT_LINK, fetchPtr(LAST_LINK, ll), l);
	storePtr(LAST_LINK, ll, l);
	storePtr(MY_LIST, l, ll);

	/* the next line balances the ref.count increment in removeFirstLink */
	c_down(l);

}



					/*ST!Interpreter!firstContext!*/
/* this routine should be called once at system startup */
OOP firstContext()              /* first active context */
{
	return fetchPtr(SUSPENDED_CTX, fetchPtr(ACTIVE_PROCESS, schedulerPtr));
}

#ifdef SIM
/* special version that doesn't use fetchPtr() */
OOP special_firstContext()
{
	/* output OOP of initial process */
	OOP process= loctn(loctn(SCHED_ASS_PTR)[VALUE])[ACTIVE_PROCESS];
	OOP ctx= loctn(process)[SUSPENDED_CTX];

	trace_process_switch(process, ctx);

	return ctx;
}
#endif SIM

void saveContext()                   /* restore pointers prior to snapshot */
{
	storeCtxRegs;
	storePtr(SUSPENDED_CTX, fetchPtr(ACTIVE_PROCESS, schedulerPtr), activeContext);
}



					/*ST!Interpreter!wakeHighestPriority!*/
static OOP wakeHighestPriority()
{
	OOP processList;
	OOP procLists= fetchPtr(PROC_LISTS, schedulerPtr);
	WORD priority= Wlen(procLists) - 1;

	while (isEmptyList(processList= fetchPtr(priority, procLists)))
		--priority;
	return removeFirstLink(processList);
}



					/*ST!Interpreter!primitiveSignal!*/
BOOL prSignal()
{
	extern void synchSignal();
	synchSignal(stackTop);
	return FALSE;
}




					/*ST!Interpreter!primitiveWait!*/
BOOL prWait()
{
	OOP rcvr= stackTop;
	SIGNED xsSignals= ffetchInt(EXCESS_SIGNALS, rcvr);

	if (xsSignals > 0) {
		fstoreInt(EXCESS_SIGNALS, rcvr, xsSignals - 1);
	} else {
		extern void addLastLink();
		addLastLink(nextProcess, rcvr);
		suspendActive;
	}
	return FALSE;
}



					/*ST!Interpreter!primitiveResume!*/
BOOL prResume()
{
	extern void resume();
	resume(stackTop);
	return FALSE;
}



					/*ST!Interpreter!primitiveSuspend!*/
BOOL prSuspend()
{
	if (stackTop == nextProcess) {
		pop(1);
		NRpush(NIL_PTR);
		suspendActive;
		return FALSE;
	} else
		return TRUE;
}
