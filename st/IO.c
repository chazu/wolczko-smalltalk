static char *rcsid= "$Header";
/*
  	Smalltalk-80 Virtual Machine:
	UNIX I/O module

	Copyright (c) 1984,1985,1986,1987,1988,1989
	Mario Wolczko and Michael Fisher
	All rights reserved.

This is the UNIX-dependent, but machine-independent part of the I/O 
system.

$Log: IO.c,v $
 * Revision 5.0  1994/08/16  00:58:40  mario
 * Added an fflush()
 *
 * Revision 4.1  89/06/21  19:51:13  mario
 * Moved all machine-dependent stuff into MachineIO.c.
 * Tidied up the interfaces to other modules.
 * 
*/

#include <stdio.h>
#include "IO.h"

unsigned dispW, dispH;          /* height and width of display */

OOP inputSema= NIL_PTR;        /* input semaphore */

	/* stuff for recording/playing back sessions */
#ifndef SIM
  enum IO_mode io_mode= real_time;       /* script or real time */
#else SIM
  enum IO_mode io_mode= script_in;       /* script or real time */
#endif SIM

FILE *script_file= NULL;
char *script_file_name= "st80.script";

BOOL display_enabled= TRUE;
static struct {
	unsigned bc_count;  /* bytecode count at this event*/
	short dummy;	/* to accomodate the vax...and for
			   backward compatibility with old script files*/
	EVENT event;
} event_record;

short MouseX= 0, MouseY= 0;   /* Mouse coordinates as reported to VM;
				   start at top left */

TIME SampleInterval= 0;

BOOL linked= TRUE;             /* Are cursor and mouse linked? */

	/* the event buffer - local to this module */
#define EVENT_BUFFER_SIZE       256
static EVENT eb[EVENT_BUFFER_SIZE];

int head= 0, tail= 0;
					     
/* open the script file and read the first event */
void read_first_script_event()
{
	script_file= fopen(script_file_name, "r");
	if (script_file == NULL) {
		fatal("Cannot access script file!");
	}
	/* read first event */
	if (fread((char *)&event_record, sizeof(event_record),
		  1, script_file) != 1) {
		fatal("Cannot read event!");
	}
	(void)fprintf(stderr,
		      "First event to occur at bc=%u\n",
		      event_record.bc_count);
}	

/* open the script file for writing */
void open_script_file()
{
	script_file= fopen(script_file_name, "w");
	if (script_file == NULL) {
		fatal("Cannot open script file!");
	}
}
	

/* Process the next scripted event and read another.
   Switch to real-time input if there aren't any more. */
void read_script_event()
{
	extern void addEvent(), re_start_window_input();
	static int events_to_ignore= 0;

	while (bc_count >= event_record.bc_count) {
		addEvent(event_record.event);
		
		if (events_to_ignore == 0)
		  switch (event_record.event&0xF000) {
		  case X_LOCTN:
			  MouseX= event_record.event & 0xFFF;
			  if (MouseX & 0x800)     /* sign extend */
			    MouseX |= ((-1)&~0xFFF);
			  break;
		  case Y_LOCTN:
			  MouseY= event_record.event & 0xFFF;
			  if (MouseY & 0x800)     /* sign extend */
			    MouseY |= ((-1)&~0xFFF);
			  break;
		  case DELTA_TIME:
			  break;
		  case ON_BISTATE: 
			  break;
		  case OFF_BISTATE:
			  break;
		  case ABSOLUTE_TIME:
			  /* mustn't confuse absolute time with
			     mouse events */
			  events_to_ignore= 2;
			  break;
		  }
		else
		  --events_to_ignore;
		if (fread((char *)&event_record, sizeof(event_record),
			  1, script_file) != 1) {
			/* there ain't no more */
			(void)fclose(script_file);    
			event_record.bc_count= 1000000000; /* i.e., forever */
			if (display_enabled) {
				io_mode= script_out;
				script_file= fopen(script_file_name, "a");
				if (script_file == NULL) {
					io_mode= real_time;
					log("script file closed");
				} else {
					log("script file finished, appending");
				}
			}
			re_start_window_input();
			break;
		}
	}
}

void addEvent(e)              /* add an event to the event buffer */
EVENT e;
{
	extern void signalInputSema();

	if (inputSema == NIL_PTR)
		return;
	if (tail >= head && tail - head < EVENT_BUFFER_SIZE - 1
	    || head - tail > 1) {
		/* room exists for event  */
		eb[tail++]= e;
		if (tail >= EVENT_BUFFER_SIZE)
			tail= 0;
		signalInputSema();
		if (io_mode == script_out) {
			event_record.bc_count= bc_count;
			event_record.event= e;
			(void)fwrite((char *)&event_record,
				     sizeof(event_record), 1, script_file);
			(void)fflush(script_file);
		}
	}
}



EVENT nextEvent()                /* return event at head of buffer */
{
	/* NB no checks for empty queue */
	EVENT event= eb[head];
	if (++head >= EVENT_BUFFER_SIZE)
		head= 0;
	return event;
}

BOOL isEmptyEventQ()
{
	return head==tail;
}


TIME getTime()          /* return the value of the millisecond clock */
{
	struct timeb Time_now;      /* new time */

	ftime(&Time_now);
	return 1000*Time_now.time + Time_now.millitm;   /* ignore overflow */
}

	/* #secs. from Jan 1 1901 00:00 to Jan 1 1970, 00:00 */
#define EPOCH   ((unsigned long)(1970-1901)*365 + 17 /*leap years*/)*24*60*60

TIME absTime()
{
	long time();

	return (TIME)(time((time_t *)0)) + EPOCH;
			/* time since Jan 1 1970, 00:00 GMT */
}

void do_time(t)
TIME t;
{
	static TIME last_ms;
	unsigned abs_clock;
	unsigned ms_intvl= t - last_ms;

	last_ms= t;

	/* put in time event */
	if (ms_intvl > 4095) {
		/* absolute time required - takes 3 buffer entries */
		/* calculate value of absolute time millisecond clock */
		if (tail >= head
		    && tail - head < EVENT_BUFFER_SIZE - 3
		    || head - tail > 3) {
			struct timeb Time_now;              /* new time */

			ftime(&Time_now);   /* read the clock */
			/* i.e. buffer space left */
			abs_clock= 1000*Time_now.time + Time_now.millitm;
			/* ignore overflow */
			addEvent(ABSOLUTE_TIME);
			addEvent(abs_clock&0xFFFF);
			addEvent(abs_clock>>16);
		}
	} else
		addEvent(/* DELTA_TIME+ */ ms_intvl);
}


