
	.section	".text",#alloc,#execinstr
	.align	8
	.skip	16

	! block 0

	.global	read_first_script_event
	.type	read_first_script_event,2
read_first_script_event:
	save	%sp,-96,%sp

	! block 1
.L455:
.L456:
.L457:

! File IO.c:
!    1	static char *rcsid= "$Header";
!    2	/*
!    3	  	Smalltalk-80 Virtual Machine:
!    4		UNIX I/O module
!    5	
!    6		Copyright (c) 1984,1985,1986,1987,1988,1989
!    7		Mario Wolczko and Michael Fisher
!    8		All rights reserved.
!    9	
!   10	This is the UNIX-dependent, but machine-independent part of the I/O 
!   11	system.
!   12	
!   13	$Log: IO.c,v $
!   14	 * Revision 5.0  1994/08/16  00:58:40  mario
!   15	 * Added an fflush()
!   16	 *
!   17	 * Revision 4.1  89/06/21  19:51:13  mario
!   18	 * Moved all machine-dependent stuff into MachineIO.c.
!   19	 * Tidied up the interfaces to other modules.
!   20	 * 
!   21	*/
!   22	
!   23	#include <stdio.h>
!   24	#include "IO.h"
!   25	
!   26	unsigned dispW, dispH;          /* height and width of display */
!   27	
!   28	OOP inputSema= NIL_PTR;        /* input semaphore */
!   29	
!   30		/* stuff for recording/playing back sessions */
!   31	#ifndef SIM
!   32	  enum IO_mode io_mode= real_time;       /* script or real time */
!   33	#else SIM
!   34	  enum IO_mode io_mode= script_in;       /* script or real time */
!   35	#endif SIM
!   36	
!   37	FILE *script_file= NULL;
!   38	char *script_file_name= "st80.script";
!   39	
!   40	BOOL display_enabled= TRUE;
!   41	static struct {
!   42		unsigned bc_count;  /* bytecode count at this event*/
!   43		short dummy;	/* to accomodate the vax...*/
!   44		EVENT event;
!   45	} event_record;
!   46	
!   47	short MouseX= 0, MouseY= 0;   /* Mouse coordinates as reported to VM;
!   48					   start at top left */
!   49	
!   50	TIME SampleInterval= 0;
!   51	
!   52	BOOL linked= TRUE;             /* Are cursor and mouse linked? */
!   53	
!   54		/* the event buffer - local to this module */
!   55	#define EVENT_BUFFER_SIZE       256
!   56	static EVENT eb[EVENT_BUFFER_SIZE];
!   57	
!   58	int head= 0, tail= 0;
!   59						     
!   60	/* open the script file and read the first event */
!   61	void read_first_script_event()
!   62	{
!   63		script_file= fopen(script_file_name, "r");

	sethi	%hi(script_file_name),%l0
	or	%l0,%lo(script_file_name),%l0
	ld	[%l0+0],%l0
	sethi	%hi(.L458),%l1
	or	%l1,%lo(.L458),%l1
	mov	%l0,%o0
	mov	%l1,%o1
	call	fopen
	nop
	mov	%o0,%l1
	sethi	%hi(script_file),%l0
	or	%l0,%lo(script_file),%l0
	st	%l1,[%l0+0]

	! block 2
.L460:

!   64		if (script_file == NULL) {

	sethi	%hi(script_file),%l0
	or	%l0,%lo(script_file),%l0
	ld	[%l0+0],%l1
	mov	0,%l0
	cmp	%l1,%l0
	bne	.L459
	nop

	! block 3
.L461:
.L462:
.L463:

!   65			fatal("Cannot access script file!");

	sethi	%hi(.L464),%l0
	or	%l0,%lo(.L464),%l0
	mov	%l0,%o0
	call	fatal
	nop

	! block 4
.L465:
.L459:
.L467:

!   66		}
!   67		/* read first event */
!   68		if (fread((char *)&event_record, sizeof(event_record),
!   69			  1, script_file) != 1) {

	sethi	%hi($XARBNkBwjBUuCkH.event_record),%l1
	or	%l1,%lo($XARBNkBwjBUuCkH.event_record),%l1
	mov	8,%l2
	mov	1,%l3
	sethi	%hi(script_file),%l0
	or	%l0,%lo(script_file),%l0
	ld	[%l0+0],%l0
	mov	%l1,%o0
	mov	%l2,%o1
	mov	%l3,%o2
	mov	%l0,%o3
	call	fread
	nop
	mov	%o0,%l0
	cmp	%l0,1
	be	.L466
	nop

	! block 5
.L468:
.L469:
.L470:

!   70			fatal("Cannot read event!");

	sethi	%hi(.L471),%l0
	or	%l0,%lo(.L471),%l0
	mov	%l0,%o0
	call	fatal
	nop

	! block 6
.L472:
.L466:
.L473:

!   71		}
!   72		(void)fprintf(stderr,
!   73			      "First event to occur at bc=%u\n",
!   74			      event_record.bc_count);

	sethi	%hi(__iob+32),%l1
	or	%l1,%lo(__iob+32),%l1
	sethi	%hi(.L474),%l2
	or	%l2,%lo(.L474),%l2
	sethi	%hi($XARBNkBwjBUuCkH.event_record),%l0
	or	%l0,%lo($XARBNkBwjBUuCkH.event_record),%l0
	ld	[%l0+0],%l0
	mov	%l1,%o0
	mov	%l2,%o1
	mov	%l0,%o2
	call	fprintf
	nop
	ba	.L454
	nop

	! block 7
.L475:
.L476:
.L454:
	jmp	%i7+8
	restore
	.size	read_first_script_event,(.-read_first_script_event)
	.align	8
	.align	8
	.skip	16

	! block 0

	.global	open_script_file
	.type	open_script_file,2
open_script_file:
	save	%sp,-96,%sp

	! block 1
.L480:
.L481:
.L482:

! File IO.c:
!   75	}	
!   76	
!   77	/* open the script file for writing */
!   78	void open_script_file()
!   79	{
!   80		script_file= fopen(script_file_name, "w");

	sethi	%hi(script_file_name),%l0
	or	%l0,%lo(script_file_name),%l0
	ld	[%l0+0],%l0
	sethi	%hi(.L483),%l1
	or	%l1,%lo(.L483),%l1
	mov	%l0,%o0
	mov	%l1,%o1
	call	fopen
	nop
	mov	%o0,%l1
	sethi	%hi(script_file),%l0
	or	%l0,%lo(script_file),%l0
	st	%l1,[%l0+0]

	! block 2
.L485:

!   81		if (script_file == NULL) {

	sethi	%hi(script_file),%l0
	or	%l0,%lo(script_file),%l0
	ld	[%l0+0],%l1
	mov	0,%l0
	cmp	%l1,%l0
	bne	.L484
	nop

	! block 3
.L486:
.L487:
.L488:

!   82			fatal("Cannot open script file!");

	sethi	%hi(.L489),%l0
	or	%l0,%lo(.L489),%l0
	mov	%l0,%o0
	call	fatal
	nop

	! block 4
.L490:
.L484:
	ba	.L479
	nop

	! block 5
.L491:
.L492:
.L479:
	jmp	%i7+8
	restore
	.size	open_script_file,(.-open_script_file)
	.align	8
	.align	8
	.skip	16

	! block 0

	.global	read_script_event
	.type	read_script_event,2
read_script_event:
	save	%sp,-96,%sp

	! block 1
.L496:
.L497:
.L504:

! File IO.c:
!   83		}
!   84	}
!   85		
!   86	
!   87	/* Process the next scripted event and read another.
!   88	   Switch to real-time input if there aren't any more. */
!   89	void read_script_event()
!   90	{
!   91		extern void addEvent(), re_start_window_input();
!   92		static int events_to_ignore= 0;
!   93	
!   94		while (bc_count >= event_record.bc_count) {

	sethi	%hi(bc_count),%l0
	or	%l0,%lo(bc_count),%l0
	ld	[%l0+0],%l1
	sethi	%hi($XARBNkBwjBUuCkH.event_record),%l0
	or	%l0,%lo($XARBNkBwjBUuCkH.event_record),%l0
	ld	[%l0+0],%l0
	cmp	%l1,%l0
	blu	.L503
	nop

	! block 2
.L505:
.L501:
.L506:
.L507:

!   95			addEvent(event_record.event);

	sethi	%hi($XARBNkBwjBUuCkH.event_record),%l0
	or	%l0,%lo($XARBNkBwjBUuCkH.event_record),%l0
	lduh	[%l0+6],%l0
	sll	%l0,16,%l0
	srl	%l0,16,%l0
	mov	%l0,%o0
	call	addEvent
	nop

	! block 3
.L509:

!   96			
!   97			if (events_to_ignore == 0)

	sethi	%hi($XBRBNkBwjBUuCkH.read_script_event.events_to_ignore),%l0
	or	%l0,%lo($XBRBNkBwjBUuCkH.read_script_event.events_to_ignore),%l0
	ld	[%l0+0],%l0
	cmp	%l0,0
	bne	.L508
	nop

	! block 4
.L510:
.L513:

!   98			  switch (event_record.event&0xF000) {

	ba	.L512
	nop

	! block 5
.L514:
.L515:
.L516:

!   99			  case X_LOCTN:
!  100				  MouseX= event_record.event & 0xFFF;

	sethi	%hi($XARBNkBwjBUuCkH.event_record),%l0
	or	%l0,%lo($XARBNkBwjBUuCkH.event_record),%l0
	lduh	[%l0+6],%l0
	sll	%l0,16,%l0
	srl	%l0,16,%l0
	mov	4095,%l1
	and	%l0,%l1,%l1
	sethi	%hi(MouseX),%l0
	or	%l0,%lo(MouseX),%l0
	sth	%l1,[%l0+0]

	! block 6
.L518:

!  101				  if (MouseX & 0x800)     /* sign extend */

	sethi	%hi(MouseX),%l0
	or	%l0,%lo(MouseX),%l0
	ldsh	[%l0+0],%l0
	sll	%l0,16,%l0
	sra	%l0,16,%l0
	and	%l0,2048,%l0
	cmp	%l0,%g0
	be	.L517
	nop

	! block 7
.L519:
.L520:

!  102				    MouseX |= ((-1)&~0xFFF);

	sethi	%hi(MouseX),%l0
	or	%l0,%lo(MouseX),%l0
	ldsh	[%l0+0],%l0
	mov	-4096,%l1
	or	%l0,%l1,%l1
	sethi	%hi(MouseX),%l0
	or	%l0,%lo(MouseX),%l0
	sth	%l1,[%l0+0]

	! block 8
.L517:
.L521:

!  103				  break;

	ba	.L511
	nop

	! block 9
.L522:
.L523:

!  104			  case Y_LOCTN:
!  105				  MouseY= event_record.event & 0xFFF;

	sethi	%hi($XARBNkBwjBUuCkH.event_record),%l0
	or	%l0,%lo($XARBNkBwjBUuCkH.event_record),%l0
	lduh	[%l0+6],%l0
	sll	%l0,16,%l0
	srl	%l0,16,%l0
	mov	4095,%l1
	and	%l0,%l1,%l1
	sethi	%hi(MouseY),%l0
	or	%l0,%lo(MouseY),%l0
	sth	%l1,[%l0+0]

	! block 10
.L525:

!  106				  if (MouseY & 0x800)     /* sign extend */

	sethi	%hi(MouseY),%l0
	or	%l0,%lo(MouseY),%l0
	ldsh	[%l0+0],%l0
	sll	%l0,16,%l0
	sra	%l0,16,%l0
	and	%l0,2048,%l0
	cmp	%l0,%g0
	be	.L524
	nop

	! block 11
.L526:
.L527:

!  107				    MouseY |= ((-1)&~0xFFF);

	sethi	%hi(MouseY),%l0
	or	%l0,%lo(MouseY),%l0
	ldsh	[%l0+0],%l0
	mov	-4096,%l1
	or	%l0,%l1,%l1
	sethi	%hi(MouseY),%l0
	or	%l0,%lo(MouseY),%l0
	sth	%l1,[%l0+0]

	! block 12
.L524:
.L528:

!  108				  break;

	ba	.L511
	nop

	! block 13
.L529:
.L530:

!  109			  case DELTA_TIME:
!  110				  break;

	ba	.L511
	nop

	! block 14
.L531:
.L532:

!  111			  case ON_BISTATE: 
!  112				  break;

	ba	.L511
	nop

	! block 15
.L533:
.L534:

!  113			  case OFF_BISTATE:
!  114				  break;

	ba	.L511
	nop

	! block 16
.L535:
.L536:

!  115			  case ABSOLUTE_TIME:
!  116				  /* mustn't confuse absolute time with
!  117				     mouse events */
!  118				  events_to_ignore= 2;

	mov	2,%l1
	sethi	%hi($XBRBNkBwjBUuCkH.read_script_event.events_to_ignore),%l0
	or	%l0,%lo($XBRBNkBwjBUuCkH.read_script_event.events_to_ignore),%l0
	st	%l1,[%l0+0]

	! block 17
.L537:

!  119				  break;

	ba	.L511
	nop

	! block 18
.L538:

!  120			  }

.L512:
	sethi	%hi($XARBNkBwjBUuCkH.event_record),%l0
	or	%l0,%lo($XARBNkBwjBUuCkH.event_record),%l0
	lduh	[%l0+6],%l0
	sll	%l0,16,%l0
	srl	%l0,16,%l0
	sethi	%hi(61440),%l1
	and	%l0,%l1,%l1
	sethi	%hi(12288),%l0
	cmp	%l1,%l0
	bg	.L_y0
	nop

	! block 19
	be	.L531
	nop

	! block 20
	sethi	%hi(4096),%l0
	cmp	%l1,%l0
	bg	.L_y1
	nop

	! block 21
	be	.L515
	nop

	! block 22
	cmp	%l1,0
	be	.L529
	nop

	! block 23
	ba	.L539
	nop

	! block 24
.L_y1:
	sethi	%hi(8192),%l0
	cmp	%l1,%l0
	be	.L522
	nop

	! block 25
	ba	.L539
	nop

	! block 26
.L_y0:
	sethi	%hi(16384),%l0
	cmp	%l1,%l0
	be	.L533
	nop

	! block 27
	sethi	%hi(20480),%l0
	cmp	%l1,%l0
	be	.L535
	nop

	! block 28
.L539:
.L511:
	ba	.L540
	nop

	! block 29
.L508:
.L541:

!  121			else
!  122			  --events_to_ignore;

	sethi	%hi($XBRBNkBwjBUuCkH.read_script_event.events_to_ignore),%l0
	or	%l0,%lo($XBRBNkBwjBUuCkH.read_script_event.events_to_ignore),%l0
	ld	[%l0+0],%l0
	sub	%l0,1,%l1
	sethi	%hi($XBRBNkBwjBUuCkH.read_script_event.events_to_ignore),%l0
	or	%l0,%lo($XBRBNkBwjBUuCkH.read_script_event.events_to_ignore),%l0
	st	%l1,[%l0+0]

	! block 30
.L540:
.L543:

!  123			if (fread((char *)&event_record, sizeof(event_record),
!  124				  1, script_file) != 1) {

	sethi	%hi($XARBNkBwjBUuCkH.event_record),%l1
	or	%l1,%lo($XARBNkBwjBUuCkH.event_record),%l1
	mov	8,%l2
	mov	1,%l3
	sethi	%hi(script_file),%l0
	or	%l0,%lo(script_file),%l0
	ld	[%l0+0],%l0
	mov	%l1,%o0
	mov	%l2,%o1
	mov	%l3,%o2
	mov	%l0,%o3
	call	fread
	nop
	mov	%o0,%l0
	cmp	%l0,1
	be	.L542
	nop

	! block 31
.L544:
.L545:
.L546:

!  125				/* there ain't no more */
!  126				(void)fclose(script_file);    

	sethi	%hi(script_file),%l0
	or	%l0,%lo(script_file),%l0
	ld	[%l0+0],%l0
	mov	%l0,%o0
	call	fclose
	nop

	! block 32
.L547:

!  127				event_record.bc_count= 1000000000; /* i.e., forever */

	sethi	%hi(1000000000),%l1
	or	%l1,%lo(1000000000),%l1
	sethi	%hi($XARBNkBwjBUuCkH.event_record),%l0
	or	%l0,%lo($XARBNkBwjBUuCkH.event_record),%l0
	st	%l1,[%l0+0]

	! block 33
.L549:

!  128				if (display_enabled) {

	sethi	%hi(display_enabled),%l0
	or	%l0,%lo(display_enabled),%l0
	ld	[%l0+0],%l0
	cmp	%l0,%g0
	be	.L548
	nop

	! block 34
.L550:
.L551:
.L552:

!  129					io_mode= script_out;

	mov	2,%l1
	sethi	%hi(io_mode),%l0
	or	%l0,%lo(io_mode),%l0
	st	%l1,[%l0+0]

	! block 35
.L553:

!  130					script_file= fopen(script_file_name, "a");

	sethi	%hi(script_file_name),%l0
	or	%l0,%lo(script_file_name),%l0
	ld	[%l0+0],%l0
	sethi	%hi(.L554),%l1
	or	%l1,%lo(.L554),%l1
	mov	%l0,%o0
	mov	%l1,%o1
	call	fopen
	nop
	mov	%o0,%l1
	sethi	%hi(script_file),%l0
	or	%l0,%lo(script_file),%l0
	st	%l1,[%l0+0]

	! block 36
.L556:

!  131					if (script_file == NULL) {

	sethi	%hi(script_file),%l0
	or	%l0,%lo(script_file),%l0
	ld	[%l0+0],%l1
	mov	0,%l0
	cmp	%l1,%l0
	bne	.L555
	nop

	! block 37
.L557:
.L558:
.L559:

!  132						io_mode= real_time;

	mov	0,%l1
	sethi	%hi(io_mode),%l0
	or	%l0,%lo(io_mode),%l0
	st	%l1,[%l0+0]

	! block 38
.L560:

!  133						log("script file closed");

	sethi	%hi(.L561),%l0
	or	%l0,%lo(.L561),%l0
	mov	%l0,%o0
	call	log
	nop

	! block 39
.L562:
	ba	.L563
	nop

	! block 40
.L555:
.L564:
.L565:

!  134					} else {
!  135						log("script file finished, appending");

	sethi	%hi(.L566),%l0
	or	%l0,%lo(.L566),%l0
	mov	%l0,%o0
	call	log
	nop

	! block 41
.L567:
.L563:
.L568:
.L548:
.L569:

!  136					}
!  137				}
!  138				re_start_window_input();

	call	re_start_window_input
	nop

	! block 42
.L570:

!  139				break;

	ba	.L503
	nop

	! block 43
.L571:
.L542:
.L572:

!  140			}
!  141		}

	sethi	%hi(bc_count),%l0
	or	%l0,%lo(bc_count),%l0
	ld	[%l0+0],%l1
	sethi	%hi($XARBNkBwjBUuCkH.event_record),%l0
	or	%l0,%lo($XARBNkBwjBUuCkH.event_record),%l0
	ld	[%l0+0],%l0
	cmp	%l1,%l0
	bgeu	.L501
	nop

	! block 44
.L573:
.L503:
	ba	.L495
	nop

	! block 45
.L574:
.L575:
.L495:
	jmp	%i7+8
	restore
	.size	read_script_event,(.-read_script_event)
	.align	8
	.align	8
	.skip	16

	! block 0

	.global	addEvent
	.type	addEvent,2
addEvent:
	save	%sp,-104,%sp
	st	%i0,[%fp+68]

	! block 1
.L578:
.L579:
.L582:

! File IO.c:
!  142	}
!  143	
!  144	void addEvent(e)              /* add an event to the event buffer */
!  145	EVENT e;
!  146	{
!  147		extern void signalInputSema();
!  148	
!  149		if (inputSema == NIL_PTR)

	sethi	%hi(inputSema),%l0
	or	%l0,%lo(inputSema),%l0
	ld	[%l0+0],%l1
	mov	1,%l0
	cmp	%l1,%l0
	bne	.L581
	nop

	! block 2
.L583:
.L584:

!  150			return;

	ba	.L577
	nop

	! block 3
.L581:
.L586:

!  151		if (tail >= head && tail - head < EVENT_BUFFER_SIZE - 1
!  152		    || head - tail > 1) {

	sethi	%hi(tail),%l0
	or	%l0,%lo(tail),%l0
	ld	[%l0+0],%l1
	sethi	%hi(head),%l0
	or	%l0,%lo(head),%l0
	ld	[%l0+0],%l0
	cmp	%l1,%l0
	bl	.L588
	nop

	! block 4
.L589:
	sethi	%hi(tail),%l0
	or	%l0,%lo(tail),%l0
	ld	[%l0+0],%l2
	sethi	%hi(head),%l0
	or	%l0,%lo(head),%l0
	ld	[%l0+0],%l1
	sub	%l2,%l1,%l0
	cmp	%l0,255
	bl	.L587
	nop

	! block 5
.L590:
.L588:
	sethi	%hi(head),%l0
	or	%l0,%lo(head),%l0
	ld	[%l0+0],%l2
	sethi	%hi(tail),%l0
	or	%l0,%lo(tail),%l0
	ld	[%l0+0],%l1
	sub	%l2,%l1,%l0
	cmp	%l0,1
	ble	.L585
	nop

	! block 6
.L591:
.L587:
.L592:
.L593:

!  153			/* room exists for event  */
!  154			eb[tail++]= e;

	sethi	%hi(tail),%l0
	or	%l0,%lo(tail),%l0
	ld	[%l0+0],%l0
	st	%l0,[%fp-4]
	ld	[%fp-4],%l0
	add	%l0,1,%l1
	sethi	%hi(tail),%l0
	or	%l0,%lo(tail),%l0
	st	%l1,[%l0+0]
	lduh	[%fp+70],%l2
	ld	[%fp-4],%l0
	sll	%l0,1,%l0
	sethi	%hi($XARBNkBwjBUuCkH.eb),%l1
	or	%l1,%lo($XARBNkBwjBUuCkH.eb),%l1
	add	%l0,%l1,%l0
	sth	%l2,[%l0+0]

	! block 7
.L595:

!  155			if (tail >= EVENT_BUFFER_SIZE)

	sethi	%hi(tail),%l0
	or	%l0,%lo(tail),%l0
	ld	[%l0+0],%l0
	cmp	%l0,256
	bl	.L594
	nop

	! block 8
.L596:
.L597:

!  156				tail= 0;

	mov	0,%l1
	sethi	%hi(tail),%l0
	or	%l0,%lo(tail),%l0
	st	%l1,[%l0+0]

	! block 9
.L594:
.L598:

!  157			signalInputSema();

	call	signalInputSema
	nop

	! block 10
.L600:

!  158			if (io_mode == script_out) {

	sethi	%hi(io_mode),%l0
	or	%l0,%lo(io_mode),%l0
	ld	[%l0+0],%l0
	cmp	%l0,2
	bne	.L599
	nop

	! block 11
.L601:
.L602:
.L603:

!  159				event_record.bc_count= bc_count;

	sethi	%hi(bc_count),%l0
	or	%l0,%lo(bc_count),%l0
	ld	[%l0+0],%l1
	sethi	%hi($XARBNkBwjBUuCkH.event_record),%l0
	or	%l0,%lo($XARBNkBwjBUuCkH.event_record),%l0
	st	%l1,[%l0+0]

	! block 12
.L604:

!  160				event_record.event= e;

	lduh	[%fp+70],%l1
	sethi	%hi($XARBNkBwjBUuCkH.event_record),%l0
	or	%l0,%lo($XARBNkBwjBUuCkH.event_record),%l0
	sth	%l1,[%l0+6]

	! block 13
.L605:

!  161				(void)fwrite((char *)&event_record,
!  162					     sizeof(event_record), 1, script_file);

	sethi	%hi($XARBNkBwjBUuCkH.event_record),%l1
	or	%l1,%lo($XARBNkBwjBUuCkH.event_record),%l1
	mov	8,%l2
	mov	1,%l3
	sethi	%hi(script_file),%l0
	or	%l0,%lo(script_file),%l0
	ld	[%l0+0],%l0
	mov	%l1,%o0
	mov	%l2,%o1
	mov	%l3,%o2
	mov	%l0,%o3
	call	fwrite
	nop

	! block 14
.L606:

!  163				(void)fflush(script_file);

	sethi	%hi(script_file),%l0
	or	%l0,%lo(script_file),%l0
	ld	[%l0+0],%l0
	mov	%l0,%o0
	call	fflush
	nop

	! block 15
.L607:
.L599:
.L608:
.L585:
	ba	.L577
	nop

	! block 16
.L609:
.L610:
.L577:
	jmp	%i7+8
	restore
	.size	addEvent,(.-addEvent)
	.align	8
	.align	8
	.skip	16

	! block 0

	.global	nextEvent
	.type	nextEvent,2
nextEvent:
	save	%sp,-112,%sp

	! block 1
.L614:
.L615:
.L616:

! File IO.c:
!  164			}
!  165		}
!  166	}
!  167	
!  170	EVENT nextEvent()                /* return event at head of buffer */
!  171	{
!  172		/* NB no checks for empty queue */
!  173		EVENT event= eb[head];

	sethi	%hi(head),%l0
	or	%l0,%lo(head),%l0
	ld	[%l0+0],%l0
	sll	%l0,1,%l0
	sethi	%hi($XARBNkBwjBUuCkH.eb),%l1
	or	%l1,%lo($XARBNkBwjBUuCkH.eb),%l1
	add	%l0,%l1,%l0
	lduh	[%l0+0],%l0
	sth	%l0,[%fp-6]

	! block 2
.L618:

!  174		if (++head >= EVENT_BUFFER_SIZE)

	sethi	%hi(head),%l0
	or	%l0,%lo(head),%l0
	ld	[%l0+0],%l0
	add	%l0,1,%l0
	st	%l0,[%fp-12]
	ld	[%fp-12],%l1
	sethi	%hi(head),%l0
	or	%l0,%lo(head),%l0
	st	%l1,[%l0+0]
	ld	[%fp-12],%l0
	cmp	%l0,256
	bl	.L617
	nop

	! block 3
.L619:
.L620:

!  175			head= 0;

	mov	0,%l1
	sethi	%hi(head),%l0
	or	%l0,%lo(head),%l0
	st	%l1,[%l0+0]

	! block 4
.L617:
.L621:

!  176		return event;

	lduh	[%fp-6],%l0
	sll	%l0,16,%l0
	srl	%l0,16,%l0
	st	%l0,[%fp-4]
	ba	.L613
	nop

	! block 5
	ba	.L613
	nop

	! block 6
.L622:
.L623:
.L613:
	ld	[%fp-4],%l0
	mov	%l0,%i0
	jmp	%i7+8
	restore
	.size	nextEvent,(.-nextEvent)
	.align	8
	.align	8
	.skip	16

	! block 0

	.global	isEmptyEventQ
	.type	isEmptyEventQ,2
isEmptyEventQ:
	save	%sp,-104,%sp

	! block 1
.L627:
.L628:
.L629:

! File IO.c:
!  177	}
!  178	
!  179	BOOL isEmptyEventQ()
!  180	{
!  181		return head==tail;

	sethi	%hi(head),%l0
	or	%l0,%lo(head),%l0
	ld	[%l0+0],%l2
	sethi	%hi(tail),%l0
	or	%l0,%lo(tail),%l0
	ld	[%l0+0],%l1
	cmp	%l2,%l1
	mov	0,%l0
	be,a	1f
	mov	1,%l0
1:
	st	%l0,[%fp-4]
	ba	.L626
	nop

	! block 2
	ba	.L626
	nop

	! block 3
.L630:
.L631:
.L626:
	ld	[%fp-4],%l0
	mov	%l0,%i0
	jmp	%i7+8
	restore
	.size	isEmptyEventQ,(.-isEmptyEventQ)
	.align	8
	.align	8
	.skip	16

	! block 0

	.global	getTime
	.type	getTime,2
getTime:
	save	%sp,-112,%sp

	! block 1
.L635:
.L636:
.L638:

! File IO.c:
!  182	}
!  183	
!  185	TIME getTime()          /* return the value of the millisecond clock */
!  186	{
!  187		struct timeb Time_now;      /* new time */
!  188	
!  189		ftime(&Time_now);

	add	%fp,-14,%l0
	mov	%l0,%o0
	call	ftime
	nop

	! block 2
.L639:

!  190		return 1000*Time_now.time + Time_now.millitm;   /* ignore overflow */

	ld	[%fp-14],%l0
	sll	%l0,10,%l2
	sll	%l0,3,%l1
	sub	%l2,%l1,%l2
	sll	%l0,4,%l1
	sub	%l2,%l1,%l2
	lduh	[%fp-10],%l0
	sll	%l0,16,%l0
	srl	%l0,16,%l1
	add	%l2,%l1,%l0
	st	%l0,[%fp-4]
	ba	.L634
	nop

	! block 3
	ba	.L634
	nop

	! block 4
.L640:
.L641:
.L634:
	ld	[%fp-4],%l0
	mov	%l0,%i0
	jmp	%i7+8
	restore
	.size	getTime,(.-getTime)
	.align	8
	.align	8
	.skip	16

	! block 0

	.global	absTime
	.type	absTime,2
absTime:
	save	%sp,-104,%sp

	! block 1
.L645:
.L646:
.L648:

! File IO.c:
!  191	}
!  192	
!  193		/* #secs. from Jan 1 1901 00:00 to Jan 1 1970, 00:00 */
!  194	#define EPOCH   ((unsigned long)(1970-1901)*365 + 17 /*leap years*/)*24*60*60
!  195	
!  196	TIME absTime()
!  197	{
!  198		long time();
!  199	
!  200		return (TIME)(time((time_t *)0)) + EPOCH;

	mov	0,%l0
	mov	%l0,%o0
	call	time
	nop
	mov	%o0,%l0
	sethi	%hi(-2117514496),%l1
	or	%l1,%lo(-2117514496),%l1
	add	%l0,%l1,%l0
	st	%l0,[%fp-4]
	ba	.L644
	nop

	! block 2
	ba	.L644
	nop

	! block 3
.L649:
.L650:
.L644:
	ld	[%fp-4],%l0
	mov	%l0,%i0
	jmp	%i7+8
	restore
	.size	absTime,(.-absTime)
	.align	8
	.align	8
	.skip	16

	! block 0

	.global	do_time
	.type	do_time,2
do_time:
	save	%sp,-120,%sp
	st	%i0,[%fp+68]

	! block 1
.L654:
.L655:
.L657:

! File IO.c:
!  201				/* time since Jan 1 1970, 00:00 GMT */
!  202	}
!  203	
!  204	void do_time(t)
!  205	TIME t;
!  206	{
!  207		static TIME last_ms;
!  208		unsigned abs_clock;
!  209		unsigned ms_intvl= t - last_ms;

	ld	[%fp+68],%l2
	sethi	%hi($XBRBNkBwjBUuCkH.do_time.last_ms),%l0
	or	%l0,%lo($XBRBNkBwjBUuCkH.do_time.last_ms),%l0
	ld	[%l0+0],%l1
	sub	%l2,%l1,%l0
	st	%l0,[%fp-8]

	! block 2
.L658:

!  211		last_ms= t;

	ld	[%fp+68],%l1
	sethi	%hi($XBRBNkBwjBUuCkH.do_time.last_ms),%l0
	or	%l0,%lo($XBRBNkBwjBUuCkH.do_time.last_ms),%l0
	st	%l1,[%l0+0]

	! block 3
.L660:

!  213		/* put in time event */
!  214		if (ms_intvl > 4095) {

	ld	[%fp-8],%l1
	mov	4095,%l0
	cmp	%l1,%l0
	bleu	.L659
	nop

	! block 4
.L661:
.L662:
.L664:

!  215			/* absolute time required - takes 3 buffer entries */
!  216			/* calculate value of absolute time millisecond clock */
!  217			if (tail >= head
!  218			    && tail - head < EVENT_BUFFER_SIZE - 3
!  219			    || head - tail > 3) {

	sethi	%hi(tail),%l0
	or	%l0,%lo(tail),%l0
	ld	[%l0+0],%l1
	sethi	%hi(head),%l0
	or	%l0,%lo(head),%l0
	ld	[%l0+0],%l0
	cmp	%l1,%l0
	bl	.L666
	nop

	! block 5
.L667:
	sethi	%hi(tail),%l0
	or	%l0,%lo(tail),%l0
	ld	[%l0+0],%l2
	sethi	%hi(head),%l0
	or	%l0,%lo(head),%l0
	ld	[%l0+0],%l1
	sub	%l2,%l1,%l0
	cmp	%l0,253
	bl	.L665
	nop

	! block 6
.L668:
.L666:
	sethi	%hi(head),%l0
	or	%l0,%lo(head),%l0
	ld	[%l0+0],%l2
	sethi	%hi(tail),%l0
	or	%l0,%lo(tail),%l0
	ld	[%l0+0],%l1
	sub	%l2,%l1,%l0
	cmp	%l0,3
	ble	.L663
	nop

	! block 7
.L669:
.L665:
.L670:
.L671:

!  220				struct timeb Time_now;              /* new time */
!  221	
!  222				ftime(&Time_now);   /* read the clock */

	add	%fp,-18,%l0
	mov	%l0,%o0
	call	ftime
	nop

	! block 8
.L672:

!  223				/* i.e. buffer space left */
!  224				abs_clock= 1000*Time_now.time + Time_now.millitm;

	ld	[%fp-18],%l0
	sll	%l0,10,%l2
	sll	%l0,3,%l1
	sub	%l2,%l1,%l2
	sll	%l0,4,%l1
	sub	%l2,%l1,%l2
	lduh	[%fp-14],%l0
	sll	%l0,16,%l0
	srl	%l0,16,%l1
	add	%l2,%l1,%l0
	st	%l0,[%fp-4]

	! block 9
.L673:

!  225				/* ignore overflow */
!  226				addEvent(ABSOLUTE_TIME);

	sethi	%hi(20480),%l0
	mov	%l0,%o0
	call	addEvent
	nop

	! block 10
.L674:

!  227				addEvent(abs_clock&0xFFFF);

	ld	[%fp-4],%l0
	sethi	%hi(65535),%l1
	or	%l1,%lo(65535),%l1
	and	%l0,%l1,%l0
	mov	%l0,%o0
	call	addEvent
	nop

	! block 11
.L675:

!  228				addEvent(abs_clock>>16);

	ld	[%fp-4],%l0
	srl	%l0,16,%l0
	mov	%l0,%o0
	call	addEvent
	nop

	! block 12
.L676:
.L663:
.L677:
	ba	.L678
	nop

	! block 13
.L659:
.L679:

!  229			}
!  230		} else
!  231			addEvent(/* DELTA_TIME+ */ ms_intvl);

	ld	[%fp-8],%l0
	mov	%l0,%o0
	call	addEvent
	nop

	! block 14
.L678:
	ba	.L653
	nop

	! block 15
.L680:
.L681:
.L653:
	jmp	%i7+8
	restore
	.size	do_time,(.-do_time)
	.align	8

	.section	".data",#alloc,#write
	.align	4
	.global	script_file
script_file:
	.skip	4
	.type	script_file,#object
	.size	script_file,4
	.align	4
	.global	script_file_name
script_file_name:
	.word	.L447
	.type	script_file_name,#object
	.size	script_file_name,4

	.section	".rodata1",#alloc
	.align	4
.L458:
	.ascii	"r\0"
	.skip	2
	.type	.L458,#object
	.size	.L458,4

	.section	".data1",#alloc,#write
	.align	4
.L464:
	.ascii	"Cannot access script file!\0"
	.skip	1
	.type	.L464,#object
	.size	.L464,28

	.section	".bss",#alloc,#write
	.align	4
	.global	$XARBNkBwjBUuCkH.event_record
$XARBNkBwjBUuCkH.event_record:
	.skip	8
	.type	$XARBNkBwjBUuCkH.event_record,#object
	.size	$XARBNkBwjBUuCkH.event_record,8

	.section	".data1",#alloc,#write
	.align	4
.L471:
	.ascii	"Cannot read event!\0"
	.skip	1
	.type	.L471,#object
	.size	.L471,20

	.section	".rodata1",#alloc
	.align	4
.L474:
	.ascii	"First event to occur at bc=%u\n\0"
	.skip	1
	.type	.L474,#object
	.size	.L474,32
	.align	4
.L483:
	.ascii	"w\0"
	.skip	2
	.type	.L483,#object
	.size	.L483,4

	.section	".data1",#alloc,#write
	.align	4
.L489:
	.ascii	"Cannot open script file!\0"
	.skip	3
	.type	.L489,#object
	.size	.L489,28

	.section	".data",#alloc,#write
	.align	4
	.global	$XBRBNkBwjBUuCkH.read_script_event.events_to_ignore
$XBRBNkBwjBUuCkH.read_script_event.events_to_ignore:
	.skip	4
	.type	$XBRBNkBwjBUuCkH.read_script_event.events_to_ignore,#object
	.size	$XBRBNkBwjBUuCkH.read_script_event.events_to_ignore,4
	.align	2
	.global	MouseX
MouseX:
	.skip	2
	.type	MouseX,#object
	.size	MouseX,2
	.align	2
	.global	MouseY
MouseY:
	.skip	2
	.type	MouseY,#object
	.size	MouseY,2
	.align	4
	.global	display_enabled
display_enabled:
	.word	0x1
	.type	display_enabled,#object
	.size	display_enabled,4
	.align	4
	.global	io_mode
io_mode:
	.skip	4
	.type	io_mode,#object
	.size	io_mode,4

	.section	".rodata1",#alloc
	.align	4
.L554:
	.ascii	"a\0"
	.type	.L554,#object
	.size	.L554,2

	.section	".data1",#alloc,#write
	.align	4
.L561:
	.ascii	"script file closed\0"
	.skip	1
	.type	.L561,#object
	.size	.L561,20
	.align	4
.L566:
	.ascii	"script file finished, appending\0"
	.type	.L566,#object
	.size	.L566,32

	.section	".data",#alloc,#write
	.align	4
	.global	inputSema
inputSema:
	.word	0x1
	.type	inputSema,#object
	.size	inputSema,4
	.align	4
	.global	tail
tail:
	.skip	4
	.type	tail,#object
	.size	tail,4
	.align	4
	.global	head
head:
	.skip	4
	.type	head,#object
	.size	head,4

	.section	".bss",#alloc,#write
	.align	2
	.global	$XARBNkBwjBUuCkH.eb
$XARBNkBwjBUuCkH.eb:
	.skip	512
	.type	$XARBNkBwjBUuCkH.eb,#object
	.size	$XARBNkBwjBUuCkH.eb,512
	.align	4
	.global	$XBRBNkBwjBUuCkH.do_time.last_ms
$XBRBNkBwjBUuCkH.do_time.last_ms:
	.skip	4
	.type	$XBRBNkBwjBUuCkH.do_time.last_ms,#object
	.size	$XBRBNkBwjBUuCkH.do_time.last_ms,4
Bbss.bss:
	.skip	0
	.size	Bbss.bss,0

	.section	".data",#alloc,#write
Ddata.data:
	.skip	0
	.size	Ddata.data,0

	.section	".rodata",#alloc
Drodata.rodata:
	.skip	0
	.size	Drodata.rodata,0

	.section	".data",#alloc,#write
	.align	4
	.global	$XARBNkBwjBUuCkH.rcsid
$XARBNkBwjBUuCkH.rcsid:
	.word	.L12
	.type	$XARBNkBwjBUuCkH.rcsid,#object
	.size	$XARBNkBwjBUuCkH.rcsid,4

	.section	".data1",#alloc,#write
	.align	4
.L12:
	.ascii	"$Header\0"
	.type	.L12,#object
	.size	.L12,8
	.align	4
.L447:
	.ascii	"st80.script\0"
	.type	.L447,#object
	.size	.L447,12

	.section	".data",#alloc,#write
	.align	4
	.global	SampleInterval
SampleInterval:
	.skip	4
	.type	SampleInterval,#object
	.size	SampleInterval,4
	.align	4
	.global	linked
linked:
	.word	0x1
	.type	linked,#object
	.size	linked,4
	.common	dispH,4,4
	.common	dispW,4,4

	.file	"IO.c"
	.xstabs	".stab.index","/home/mario/st/",100,0,0,0
	.xstabs	".stab.index","IO.c",100,0,3,0
	.stabs	"/home/mario/st/",100,0,0,0
	.stabs	"IO.c",100,0,3,0
	.xstabs	".stab.index","",56,0,0,0
	.xstabs	".stab.index","",56,0,0,0
	.stabs	"",56,0,0,0
	.stabs	"",56,0,0,0
	.xstabs	".stab.index","Xt ; g ; V=3.0 ; R=3.0 ; G=$XARBNkBwjBUuCkH.",60,0,0,777001200
	.stabs	"Xt ; g ; V=3.0 ; R=3.0 ; G=$XARBNkBwjBUuCkH.",60,0,0,777001200
	.xstabs	".stab.index","/home/mario/st; /usr/dist/local/pkgs/sunpro,v3.0.1/5.x-sparc/bin/../SC3.0.1/bin/cc -DX11 -g -I/usr/ucbinclude -c -S  IO.c -W0,-xp\\$XARBNkBwjBUuCkH.",52,0,0,0
	.stabs	"char:t(0,1)=bsc1;0;8;",128,0,0,0
	.stabs	"short:t(0,2)=bs2;0;16;",128,0,0,0
	.stabs	"int:t(0,3)=bs4;0;32;",128,0,0,0
	.stabs	"long:t(0,4)=bs4;0;32;",128,0,0,0
	.stabs	"long long:t(0,5)=bs8;0;64;",128,0,0,0
	.stabs	"signed char:t(0,6)=bsc1;0;8;",128,0,0,0
	.stabs	"signed short:t(0,7)=bs2;0;16;",128,0,0,0
	.stabs	"signed int:t(0,8)=bs4;0;32;",128,0,0,0
	.stabs	"signed long:t(0,9)=bs4;0;32;",128,0,0,0
	.stabs	"signed long long:t(0,10)=bs8;0;64;",128,0,0,0
	.stabs	"unsigned char:t(0,11)=buc1;0;8;",128,0,0,0
	.stabs	"unsigned short:t(0,12)=bu2;0;16;",128,0,0,0
	.stabs	"unsigned:t(0,13)=bu4;0;32;",128,0,0,0
	.stabs	"unsigned int:t(0,14)=bu4;0;32;",128,0,0,0
	.stabs	"unsigned long:t(0,15)=bu4;0;32;",128,0,0,0
	.stabs	"unsigned long long:t(0,16)=bu8;0;64;",128,0,0,0
	.stabs	"float:t(0,17)=R1;4;",128,0,0,0
	.stabs	"double:t(0,18)=R2;8;",128,0,0,0
	.stabs	"long double:t(0,19)=R6;16;",128,0,0,0
	.stabs	"void:t(0,20)=bs0;0;0",128,0,0,0
	.xstabs	".stab.index","$XARBNkBwjBUuCkH.rcsid",32,0,0,0
	.stabs	"$XARBNkBwjBUuCkH.rcsid:S(0,21)=*(0,1)",38,0,4,0
	.stabs	"/usr/ucbinclude/stdio.h",130,0,0,0
	.stabs	"size_t:t(1,1)=(0,14)",128,0,4,8192
	.stabs	"fpos_t:t(1,2)=(0,4)",128,0,4,128
	.stabs	":T(1,3)=s16_cnt:(0,3),0,32;_ptr:(1,4)=*(0,11),32,32;_base:(1,4),64,32;_flag:(0,11),96,8;_file:(0,11),104,8;;",128,0,16,1
	.stabs	"FILE:t(1,5)=(1,3)",128,0,16,2048
	.stabn	162,0,0,0
	.stabs	"./IO.h",130,0,0,0
	.stabs	"/usr/ucbinclude/sys/types.h",130,0,0,0
	.stabs	"_physadr:T(3,1)=s4r:(3,2)=ar(0,3);0;0;(0,3),0,32;;",128,0,4,1
	.stabs	"physadr:t(3,3)=*(3,1)",128,0,4,2048
	.stabs	"_label:T(3,4)=s8val:(3,5)=ar(0,3);0;1;(0,3),0,64;;",128,0,8,1
	.stabs	"label_t:t(3,6)=(3,4)",128,0,8,2048
	.stabs	"uchar_t:t(3,7)=(0,11)",128,0,1,8194
	.stabs	"ushort_t:t(3,8)=(0,12)",128,0,2,8704
	.stabs	"uint_t:t(3,9)=(0,14)",128,0,4,8192
	.stabs	"ulong_t:t(3,10)=(0,15)",128,0,4,8320
	.stabs	"addr_t:t(3,11)=*(0,1)",128,0,4,2
	.stabs	"caddr_t:t(3,12)=*(0,1)",128,0,4,2
	.stabs	"daddr_t:t(3,13)=(0,4)",128,0,4,128
	.stabs	"off_t:t(3,14)=(0,4)",128,0,4,128
	.stabs	"cnt_t:t(3,15)=(0,2)",128,0,2,512
	.stabs	"paddr_t:t(3,16)=(0,15)",128,0,4,73856
	.stabs	"use_t:t(3,17)=(0,11)",128,0,1,73730
	.stabs	"sysid_t:t(3,18)=(0,2)",128,0,2,512
	.stabs	"index_t:t(3,19)=(0,2)",128,0,2,512
	.stabs	"swblk_t:t(3,20)=(0,4)",128,0,4,128
	.stabs	"lock_t:t(3,21)=(0,2)",128,0,2,512
	.stabs	"boolean:T(3,22)=eB_FALSE:0,B_TRUE:1,;",128,0,4,1
	.stabs	"boolean_t:t(3,23)=(3,22)",128,0,4,16
	.stabs	"l_dev_t:t(3,24)=(0,15)",128,0,4,73856
	.stabs	"longlong_t:t(3,25)=(0,5)",128,0,8,131072
	.stabs	"u_longlong_t:t(3,26)=(0,16)",128,0,8,139264
	.stabs	"offset_t:t(3,27)=(0,5)",128,0,8,196608
	.stabs	"diskaddr_t:t(3,28)=(0,5)",128,0,8,196608
	.stabs	":T(3,29)=s8_u:(0,4),0,32;_l:(3,14),32,32;;",128,0,8,1
	.stabs	"lloff:T(3,30)=u8_f:(3,27),0,64;_p:(3,29),0,64;;",128,0,8,1
	.stabs	"lloff_t:t(3,31)=(3,30)",128,0,8,4096
	.stabs	":T(3,32)=s8_u:(0,4),0,32;_l:(3,13),32,32;;",128,0,8,1
	.stabs	"lldaddr:T(3,33)=u8_f:(3,28),0,64;_p:(3,32),0,64;;",128,0,8,1
	.stabs	"lldaddr_t:t(3,34)=(3,33)",128,0,8,4096
	.stabs	"k_fltset_t:t(3,35)=(0,15)",128,0,4,73856
	.stabs	"id_t:t(3,36)=(0,4)",128,0,4,128
	.stabs	"major_t:t(3,37)=(0,15)",128,0,4,73856
	.stabs	"minor_t:t(3,38)=(0,15)",128,0,4,73856
	.stabs	"pri_t:t(3,39)=(0,2)",128,0,2,512
	.stabs	"o_mode_t:t(3,40)=(0,12)",128,0,2,74240
	.stabs	"o_dev_t:t(3,41)=(0,2)",128,0,2,512
	.stabs	"o_uid_t:t(3,42)=(0,12)",128,0,2,74240
	.stabs	"o_gid_t:t(3,43)=(0,12)",128,0,2,74240
	.stabs	"o_nlink_t:t(3,44)=(0,2)",128,0,2,512
	.stabs	"o_pid_t:t(3,45)=(0,2)",128,0,2,512
	.stabs	"o_ino_t:t(3,46)=(0,12)",128,0,2,74240
	.stabs	"key_t:t(3,47)=(0,3)",128,0,4,64
	.stabs	"mode_t:t(3,48)=(0,15)",128,0,4,73856
	.stabs	"uid_t:t(3,49)=(0,4)",128,0,4,128
	.stabs	"gid_t:t(3,50)=(0,4)",128,0,4,65664
	.stabs	"nlink_t:t(3,51)=(0,15)",128,0,4,73856
	.stabs	"dev_t:t(3,52)=(0,15)",128,0,4,73856
	.stabs	"ino_t:t(3,53)=(0,15)",128,0,4,73856
	.stabs	"pid_t:t(3,54)=(0,4)",128,0,4,128
	.stabs	"ssize_t:t(3,55)=(0,3)",128,0,4,64
	.stabs	"time_t:t(3,56)=(0,4)",128,0,4,128
	.stabs	"clock_t:t(3,57)=(0,4)",128,0,4,128
	.stabs	"unchar:t(3,58)=(0,11)",128,0,1,8194
	.stabs	"uint:t(3,59)=(0,14)",128,0,4,8192
	.stabs	"ulong:t(3,60)=(0,15)",128,0,4,8320
	.stabs	"hostid_t:t(3,61)=(0,4)",128,0,4,128
	.stabs	"u_char:t(3,62)=(0,11)",128,0,1,8194
	.stabs	"u_short:t(3,63)=(0,12)",128,0,2,8704
	.stabs	"u_int:t(3,64)=(0,14)",128,0,4,8192
	.stabs	"u_long:t(3,65)=(0,15)",128,0,4,8320
	.stabs	"ushort:t(3,66)=(0,12)",128,0,2,8704
	.stabs	"_quad:T(3,67)=s8val:(3,68)=ar(0,3);0;1;(0,4),0,64;;",128,0,8,1
	.stabs	"quad:t(3,69)=(3,67)",128,0,8,2048
	.stabs	"/usr/include/sys/select.h",130,0,0,0
	.stabs	"/usr/include/sys/time.h",130,0,0,0
	.stabs	"/usr/include/sys/feature_tests.h",130,0,0,0
	.stabn	162,0,0,0
	.stabs	"timeval:T(5,1)=s8tv_sec:(0,4),0,32;tv_usec:(0,4),32,32;;",128,0,8,1
	.stabs	"timezone:T(5,2)=s8tz_minuteswest:(0,3),0,32;tz_dsttime:(0,3),32,32;;",128,0,8,1
	.stabs	"/usr/ucbinclude/sys/types.h",130,0,0,0
	.stabn	162,0,0,0
	.stabs	"itimerval:T(5,3)=s16it_interval:(5,1),0,64;it_value:(5,1),64,64;;",128,0,16,1
	.stabs	"timespec:T(5,4)=s8tv_sec:(3,56),0,32;tv_nsec:(0,4),32,32;;",128,0,8,1
	.stabs	"timespec_t:t(5,5)=(5,4)",128,0,8,2048
	.stabs	"timestruc_t:t(5,6)=(5,4)",128,0,8,2048
	.stabs	"itimerspec:T(5,7)=s16it_interval:(5,4),0,64;it_value:(5,4),64,64;;",128,0,16,1
	.stabs	"itimerspec_t:t(5,8)=(5,7)",128,0,16,2048
	.stabs	"hrtime_t:t(5,9)=(0,5)",128,0,8,196608
	.stabs	"/usr/include/time.h",130,0,0,0
	.stabs	"/usr/include/sys/feature_tests.h",130,0,0,0
	.stabn	162,0,0,0
	.stabs	"clockid_t:t(8,1)=(0,3)",128,0,4,64
	.stabs	"timer_t:t(8,2)=(0,3)",128,0,4,64
	.stabs	"tm:T(8,3)=s36tm_sec:(0,3),0,32;tm_min:(0,3),32,32;tm_hour:(0,3),64,32;tm_mday:(0,3),96,32;tm_mon:(0,3),128,32;tm_year:(0,3),160,32;tm_wday:(0,3),192,32;tm_yday:(0,3),224,32;\\",128,0,36,1
	.stabs	"tm_isdst:(0,3),256,32;;",128,0,36,1
	.stabs	"/usr/include/sys/time.h",130,0,0,0
	.stabn	162,0,0,0
	.stabs	"/usr/include/sys/siginfo.h",130,0,0,0
	.stabs	"/usr/include/sys/feature_tests.h",130,0,0,0
	.stabn	162,0,0,0
	.stabs	"sigval:T(11,1)=u4sival_int:(0,3),0,32;sival_ptr:(11,2)=*(0,20),0,32;;",128,0,4,1
	.stabs	"sigevent:T(11,3)=s12sigev_notify:(0,3),0,32;sigev_signo:(0,3),32,32;sigev_value:(11,1),64,32;;",128,0,12,1
	.stabs	"/usr/include/sys/machsig.h",130,0,0,0
	.stabn	162,0,0,0
	.stabs	"/usr/include/sys/time.h",130,0,0,0
	.stabn	162,0,0,0
	.stabs	":T(11,4)=s8_uid:(3,49),0,32;_value:(11,1),32,32;;",128,0,8,1
	.stabs	":T(11,5)=s12_utime:(3,57),0,32;_status:(0,3),32,32;_stime:(3,57),64,32;;",128,0,12,1
	.stabs	":T(11,6)=u12_kill:(11,4),0,64;_cld:(11,5),0,96;;",128,0,12,1
	.stabs	":T(11,7)=s16_pid:(3,54),0,32;_pdata:(11,6),32,96;;",128,0,16,1
	.stabs	":T(11,8)=s8_addr:(3,12),0,32;_trapno:(0,3),32,32;;",128,0,8,1
	.stabs	":T(11,9)=s8_fd:(0,3),0,32;_band:(0,4),32,32;;",128,0,8,1
	.stabs	":T(11,10)=s116_faddr:(3,12),0,32;_tstamp:(5,6),32,64;_syscall:(0,2),96,16;_nsysarg:(0,1),112,8;_fault:(0,1),120,8;_sysarg:(11,11)=ar(0,3);0;7;(0,4),128,256;_mstate:(11,12)=ar(0,3);0;16;(0,4),384,544;;",128,0,116,1
	.stabs	":T(11,13)=u116_pad:(11,14)=ar(0,3);0;28;(0,3),0,928;_proc:(11,7),0,128;_fault:(11,8),0,64;_file:(11,9),0,64;_prof:(11,10),0,928;;",128,0,116,1
	.stabs	"siginfo:T(11,15)=s128si_signo:(0,3),0,32;si_code:(0,3),32,32;si_errno:(0,3),64,32;_data:(11,13),96,928;;",128,0,128,1
	.stabs	"siginfo_t:t(11,16)=(11,15)",128,0,128,2048
	.stabs	":T(11,17)=s8_uid:(3,49),0,32;_value:(11,1),32,32;;",128,0,8,1
	.stabs	":T(11,18)=s12_utime:(3,57),0,32;_status:(0,3),32,32;_stime:(3,57),64,32;;",128,0,12,1
	.stabs	":T(11,19)=u12_kill:(11,17),0,64;_cld:(11,18),0,96;;",128,0,12,1
	.stabs	":T(11,20)=s16_pid:(3,54),0,32;_pdata:(11,19),32,96;;",128,0,16,1
	.stabs	":T(11,21)=s8_addr:(3,12),0,32;_trapno:(0,3),32,32;;",128,0,8,1
	.stabs	":T(11,22)=s8_fd:(0,3),0,32;_band:(0,4),32,32;;",128,0,8,1
	.stabs	":T(11,23)=s16_faddr:(3,12),0,32;_tstamp:(5,6),32,64;_syscall:(0,2),96,16;_nsysarg:(0,1),112,8;_fault:(0,1),120,8;;",128,0,16,1
	.stabs	":T(11,24)=u16_proc:(11,20),0,128;_fault:(11,21),0,64;_file:(11,22),0,64;_prof:(11,23),0,128;;",128,0,16,1
	.stabs	"k_siginfo:T(11,25)=s28si_signo:(0,3),0,32;si_code:(0,3),32,32;si_errno:(0,3),64,32;_data:(11,24),96,128;;",128,0,28,1
	.stabs	"k_siginfo_t:t(11,26)=(11,25)",128,0,28,2048
	.stabs	"sigqueue:T(11,27)=s40sq_next:(11,28)=*(11,27),0,32;sq_info:(11,26),32,224;sq_func:(11,29)=*(11,30)=f(0,20),256,32;sq_backptr:(11,2),288,32;;",128,0,40,1
	.stabs	"sigqueue_t:t(11,31)=(11,27)",128,0,40,2048
	.stabn	162,0,0,0
	.stabn	162,0,0,0
	.stabn	162,0,0,0
	.stabs	"fd_mask:t(4,1)=(0,4)",128,0,4,128
	.stabs	"fd_set:T(4,2)=s128fds_bits:(4,3)=ar(0,3);0;31;(4,1),0,1024;;",128,0,128,1
	.stabs	"fd_set:t(4,4)=(4,2)",128,0,128,2048
	.stabn	162,0,0,0
	.stabs	"/usr/ucbinclude/sys/sysmacros.h",130,0,0,0
	.stabn	162,0,0,0
	.stabn	162,0,0,0
	.stabs	"/usr/include/sys/timeb.h",130,0,0,0
	.stabs	"timeb:T(16,1)=s10time:(3,56),0,32;millitm:(0,12),32,16;timezone:(0,2),48,16;dstflag:(0,2),64,16;;",128,0,10,1
	.stabn	162,0,0,0
	.stabs	"./ST.h",130,0,0,0
	.stabs	"BOOL:t(17,1)=(0,3)",128,0,4,64
	.stabs	"WORD:t(17,2)=(0,14)",128,0,4,8192
	.stabs	"OOP:t(17,3)=(0,14)",128,0,4,73728
	.stabs	"WORD16:t(17,4)=(0,12)",128,0,2,8704
	.stabs	"EVENT:t(17,5)=(0,12)",128,0,2,74240
	.stabs	"SIGNED:t(17,6)=(0,3)",128,0,4,64
	.stabs	"BYTE:t(17,7)=(0,11)",128,0,1,8194
	.stabs	"TIME:t(17,8)=(0,14)",128,0,4,8192
	.stabs	"ADDR:t(17,9)=*(0,14)",128,0,4,73728
	.stabs	"/usr/include/X11/Xlib.h",130,0,0,0
	.stabs	"/usr/ucbinclude/sys/types.h",130,0,0,0
	.stabn	162,0,0,0
	.stabs	"/usr/include/X11/X.h",130,0,0,0
	.stabs	"XID:t(20,1)=(0,15)",128,0,4,8320
	.stabs	"Window:t(20,2)=(0,15)",128,0,4,73856
	.stabs	"Drawable:t(20,3)=(0,15)",128,0,4,73856
	.stabs	"Font:t(20,4)=(0,15)",128,0,4,73856
	.stabs	"Pixmap:t(20,5)=(0,15)",128,0,4,73856
	.stabs	"Cursor:t(20,6)=(0,15)",128,0,4,73856
	.stabs	"Colormap:t(20,7)=(0,15)",128,0,4,73856
	.stabs	"GContext:t(20,8)=(0,15)",128,0,4,73856
	.stabs	"KeySym:t(20,9)=(0,15)",128,0,4,73856
	.stabs	"Mask:t(20,10)=(0,15)",128,0,4,8320
	.stabs	"Atom:t(20,11)=(0,15)",128,0,4,8320
	.stabs	"VisualID:t(20,12)=(0,15)",128,0,4,8320
	.stabs	"Time:t(20,13)=(0,15)",128,0,4,8320
	.stabs	"KeyCode:t(20,14)=(0,11)",128,0,1,8194
	.stabn	162,0,0,0
	.stabs	"_XExtData:T(18,1)=s16number:(0,3),0,32;next:(18,2)=*(18,1),32,32;free_private:(18,3)=*(18,4)=f(0,3),64,32;private_data:(0,21),96,32;;",128,0,16,1
	.stabs	"XExtData:t(18,5)=(18,1)",128,0,16,2048
	.stabs	":T(18,6)=s16extension:(0,3),0,32;major_opcode:(0,3),32,32;first_event:(0,3),64,32;first_error:(0,3),96,32;;",128,0,16,1
	.stabs	"XExtCodes:t(18,7)=(18,6)",128,0,16,2048
	.stabs	"_XExten:T(18,8)=s60next:(18,9)=*(18,8),0,32;codes:(18,7),32,128;create_GC:(18,10)=*(18,11)=f(0,3),160,32;copy_GC:(18,12)=*(18,13)=f(0,3),192,32;flush_GC:(18,14)=*(18,15)=f(0,3),224,32;free_GC:(18,16)=*(18,17)=f(0,3),256,32;create_Font:(18,18)=*(18,19)=f(0,3),288,32;free_Font:(18,20)=*(18,21)=f(0,3),320,32;\\",128,0,60,1
	.stabs	"close_display:(18,22)=*(18,23)=f(0,3),352,32;error:(18,24)=*(18,25)=f(0,3),384,32;error_string:(18,26)=*(18,27)=f(0,21),416,32;name:(0,21),448,32;;",128,0,60,1
	.stabs	"_XExtension:t(18,28)=(18,8)",128,0,60,2048
	.stabs	":T(18,29)=s12depth:(0,3),0,32;bits_per_pixel:(0,3),32,32;scanline_pad:(0,3),64,32;;",128,0,12,1
	.stabs	"XPixmapFormatValues:t(18,30)=(18,29)",128,0,12,2048
	.stabs	":T(18,31)=s92function:(0,3),0,32;plane_mask:(0,15),32,32;foreground:(0,15),64,32;background:(0,15),96,32;line_width:(0,3),128,32;line_style:(0,3),160,32;cap_style:(0,3),192,32;join_style:(0,3),224,32;\\",128,0,92,1
	.stabs	"fill_style:(0,3),256,32;fill_rule:(0,3),288,32;arc_mode:(0,3),320,32;tile:(20,5),352,32;stipple:(20,5),384,32;ts_x_origin:(0,3),416,32;ts_y_origin:(0,3),448,32;font:(20,4),480,32;\\",128,0,92,1
	.stabs	"subwindow_mode:(0,3),512,32;graphics_exposures:(0,3),544,32;clip_x_origin:(0,3),576,32;clip_y_origin:(0,3),608,32;clip_mask:(20,5),640,32;dash_offset:(0,3),672,32;dashes:(0,1),704,8;;",128,0,92,1
	.stabs	"XGCValues:t(18,32)=(18,31)",128,0,92,2048
	.stabs	"_XGC:T(18,33)=s112ext_data:(18,34)=*(18,5),0,32;gid:(20,8),32,32;rects:(0,3),64,32;dashes:(0,3),96,32;dirty:(0,15),128,32;values:(18,32),160,736;;",128,0,112,1
	.stabs	"GC:t(18,35)=*(18,33)",128,0,4,2048
	.stabs	":T(18,36)=s32ext_data:(18,34),0,32;visualid:(20,12),32,32;class:(0,3),64,32;red_mask:(0,15),96,32;green_mask:(0,15),128,32;blue_mask:(0,15),160,32;bits_per_rgb:(0,3),192,32;map_entries:(0,3),224,32;;",128,0,32,1
	.stabs	"Visual:t(18,37)=(18,36)",128,0,32,2048
	.stabs	":T(18,38)=s12depth:(0,3),0,32;nvisuals:(0,3),32,32;visuals:(18,39)=*(18,37),64,32;;",128,0,12,1
	.stabs	"Depth:t(18,40)=(18,38)",128,0,12,2048
	.stabs	":T(18,41)=s80ext_data:(18,34),0,32;display:(18,42)=*(18,43)=xs_XDisplay:,32,32;root:(20,2),64,32;width:(0,3),96,32;height:(0,3),128,32;mwidth:(0,3),160,32;mheight:(0,3),192,32;ndepths:(0,3),224,32;\\",128,0,80,1
	.stabs	"depths:(18,44)=*(18,40),256,32;root_depth:(0,3),288,32;root_visual:(18,39),320,32;default_gc:(18,35),352,32;cmap:(20,7),384,32;white_pixel:(0,15),416,32;black_pixel:(0,15),448,32;max_maps:(0,3),480,32;\\",128,0,80,1
	.stabs	"min_maps:(0,3),512,32;backing_store:(0,3),544,32;save_unders:(0,3),576,32;root_input_mask:(0,4),608,32;;",128,0,80,1
	.stabs	"Screen:t(18,45)=(18,41)",128,0,80,2048
	.stabs	":T(18,46)=s16ext_data:(18,34),0,32;depth:(0,3),32,32;bits_per_pixel:(0,3),64,32;scanline_pad:(0,3),96,32;;",128,0,16,1
	.stabs	"ScreenFormat:t(18,47)=(18,46)",128,0,16,2048
	.stabs	":T(18,48)=s60background_pixmap:(20,5),0,32;background_pixel:(0,15),32,32;border_pixmap:(20,5),64,32;border_pixel:(0,15),96,32;bit_gravity:(0,3),128,32;win_gravity:(0,3),160,32;backing_store:(0,3),192,32;backing_planes:(0,15),224,32;\\",128,0,60,1
	.stabs	"backing_pixel:(0,15),256,32;save_under:(0,3),288,32;event_mask:(0,4),320,32;do_not_propagate_mask:(0,4),352,32;override_redirect:(0,3),384,32;colormap:(20,7),416,32;cursor:(20,6),448,32;;",128,0,60,1
	.stabs	"XSetWindowAttributes:t(18,49)=(18,48)",128,0,60,2048
	.stabs	":T(18,50)=s92x:(0,3),0,32;y:(0,3),32,32;width:(0,3),64,32;height:(0,3),96,32;border_width:(0,3),128,32;depth:(0,3),160,32;visual:(18,39),192,32;root:(20,2),224,32;\\",128,0,92,1
	.stabs	"class:(0,3),256,32;bit_gravity:(0,3),288,32;win_gravity:(0,3),320,32;backing_store:(0,3),352,32;backing_planes:(0,15),384,32;backing_pixel:(0,15),416,32;save_under:(0,3),448,32;colormap:(20,7),480,32;\\",128,0,92,1
	.stabs	"map_installed:(0,3),512,32;map_state:(0,3),544,32;all_event_masks:(0,4),576,32;your_event_mask:(0,4),608,32;do_not_propagate_mask:(0,4),640,32;override_redirect:(0,3),672,32;screen:(18,51)=*(18,45),704,32;;",128,0,92,1
	.stabs	"XWindowAttributes:t(18,52)=(18,50)",128,0,92,2048
	.stabs	":T(18,53)=s12family:(0,3),0,32;length:(0,3),32,32;address:(0,21),64,32;;",128,0,12,1
	.stabs	"XHostAddress:t(18,54)=(18,53)",128,0,12,2048
	.stabs	"funcs:T(18,55)=s24create_image:(18,56)=*(18,57)=f(18,58)=*(18,59)=xs_XImage:,0,32;destroy_image:(18,60)=*(18,61)=f(0,3),32,32;get_pixel:(18,62)=*(18,63)=f(0,15),64,32;put_pixel:(18,64)=*(18,65)=f(0,3),96,32;sub_image:(18,66)=*(18,67)=f(18,58),128,32;add_pixel:(18,68)=*(18,69)=f(0,3),160,32;;",128,0,24,1
	.stabs	"_XImage:T(18,59)=s88width:(0,3),0,32;height:(0,3),32,32;xoffset:(0,3),64,32;format:(0,3),96,32;data:(0,21),128,32;byte_order:(0,3),160,32;bitmap_unit:(0,3),192,32;bitmap_bit_order:(0,3),224,32;\\",128,0,88,1
	.stabs	"bitmap_pad:(0,3),256,32;depth:(0,3),288,32;bytes_per_line:(0,3),320,32;bits_per_pixel:(0,3),352,32;red_mask:(0,15),384,32;green_mask:(0,15),416,32;blue_mask:(0,15),448,32;obdata:(0,21),480,32;\\",128,0,88,1
	.stabs	"f:(18,55),512,192;;",128,0,88,1
	.stabs	"XImage:t(18,70)=(18,59)",128,0,88,2048
	.stabs	":T(18,71)=s28x:(0,3),0,32;y:(0,3),32,32;width:(0,3),64,32;height:(0,3),96,32;border_width:(0,3),128,32;sibling:(20,2),160,32;stack_mode:(0,3),192,32;;",128,0,28,1
	.stabs	"XWindowChanges:t(18,72)=(18,71)",128,0,28,2048
	.stabs	":T(18,73)=s12pixel:(0,15),0,32;red:(0,12),32,16;green:(0,12),48,16;blue:(0,12),64,16;flags:(0,1),80,8;pad:(0,1),88,8;;",128,0,12,1
	.stabs	"XColor:t(18,74)=(18,73)",128,0,12,2048
	.stabs	":T(18,75)=s8x1:(0,2),0,16;y1:(0,2),16,16;x2:(0,2),32,16;y2:(0,2),48,16;;",128,0,8,1
	.stabs	"XSegment:t(18,76)=(18,75)",128,0,8,2048
	.stabs	":T(18,77)=s4x:(0,2),0,16;y:(0,2),16,16;;",128,0,4,1
	.stabs	"XPoint:t(18,78)=(18,77)",128,0,4,2048
	.stabs	":T(18,79)=s8x:(0,2),0,16;y:(0,2),16,16;width:(0,12),32,16;height:(0,12),48,16;;",128,0,8,1
	.stabs	"XRectangle:t(18,80)=(18,79)",128,0,8,2048
	.stabs	":T(18,81)=s12x:(0,2),0,16;y:(0,2),16,16;width:(0,12),32,16;height:(0,12),48,16;angle1:(0,2),64,16;angle2:(0,2),80,16;;",128,0,12,1
	.stabs	"XArc:t(18,82)=(18,81)",128,0,12,2048
	.stabs	":T(18,83)=s32key_click_percent:(0,3),0,32;bell_percent:(0,3),32,32;bell_pitch:(0,3),64,32;bell_duration:(0,3),96,32;led:(0,3),128,32;led_mode:(0,3),160,32;key:(0,3),192,32;auto_repeat_mode:(0,3),224,32;;",128,0,32,1
	.stabs	"XKeyboardControl:t(18,84)=(18,83)",128,0,32,2048
	.stabs	":T(18,85)=s56key_click_percent:(0,3),0,32;bell_percent:(0,3),32,32;bell_pitch:(0,14),64,32;bell_duration:(0,14),96,32;led_mask:(0,15),128,32;global_auto_repeat:(0,3),160,32;auto_repeats:(18,86)=ar(0,3);0;31;(0,1),192,256;;",128,0,56,1
	.stabs	"XKeyboardState:t(18,87)=(18,85)",128,0,56,2048
	.stabs	":T(18,88)=s8time:(20,13),0,32;x:(0,2),32,16;y:(0,2),48,16;;",128,0,8,1
	.stabs	"XTimeCoord:t(18,89)=(18,88)",128,0,8,2048
	.stabs	":T(18,90)=s8max_keypermod:(0,3),0,32;modifiermap:(18,91)=*(20,14),32,32;;",128,0,8,1
	.stabs	"XModifierKeymap:t(18,92)=(18,90)",128,0,8,2048
	.stabs	"_DisplayAtoms:T(18,93)=s24text:(20,11),0,32;wm_state:(20,11),32,32;wm_protocols:(20,11),64,32;wm_save_yourself:(20,11),96,32;wm_change_state:(20,11),128,32;wm_colormap_windows:(20,11),160,32;;",128,0,24,1
	.stabs	":T(18,94)=s12sequence_number:(0,4),0,32;old_handler:(18,95)=*(18,96)=f(0,3),32,32;succeeded:(0,3),64,32;;",128,0,12,1
	.stabs	"_XDisplay:T(18,43)=s1252ext_data:(18,34),0,32;next:(18,42),32,32;fd:(0,3),64,32;lock:(0,3),96,32;proto_major_version:(0,3),128,32;proto_minor_version:(0,3),160,32;vendor:(0,21),192,32;resource_base:(0,4),224,32;\\",128,0,1252,0
	.stabs	"resource_mask:(0,4),256,32;resource_id:(0,4),288,32;resource_shift:(0,3),320,32;resource_alloc:(18,97)=*(18,98)=f(0,15),352,32;byte_order:(0,3),384,32;bitmap_unit:(0,3),416,32;bitmap_pad:(0,3),448,32;bitmap_bit_order:(0,3),480,32;\\",128,0,1252,0
	.stabs	"nformats:(0,3),512,32;pixmap_format:(18,99)=*(18,47),544,32;vnumber:(0,3),576,32;release:(0,3),608,32;head:(18,100)=*(18,101)=xs_XSQEvent:,640,32;tail:(18,100),672,32;qlen:(0,3),704,32;last_request_read:(0,15),736,32;\\",128,0,1252,0
	.stabs	"request:(0,15),768,32;last_req:(0,21),800,32;buffer:(0,21),832,32;bufptr:(0,21),864,32;bufmax:(0,21),896,32;max_request_size:(0,14),928,32;db:(18,102)=*(18,103)=xs_XrmHashBucketRec:,960,32;synchandler:(18,104)=*(18,105)=f(0,3),992,32;\\",128,0,1252,0
	.stabs	"display_name:(0,21),1024,32;default_screen:(0,3),1056,32;nscreens:(0,3),1088,32;screens:(18,51),1120,32;motion_buffer:(0,15),1152,32;current:(20,2),1184,32;min_keycode:(0,3),1216,32;max_keycode:(0,3),1248,32;\\",128,0,1252,0
	.stabs	"keysyms:(18,106)=*(20,9),1280,32;modifiermap:(18,107)=*(18,92),1312,32;keysyms_per_keycode:(0,3),1344,32;xdefaults:(0,21),1376,32;scratch_buffer:(0,21),1408,32;scratch_length:(0,15),1440,32;ext_number:(0,3),1472,32;ext_procs:(18,108)=*(18,28),1504,32;\\",128,0,1252,0
	.stabs	"event_vec:(18,109)=ar(0,3);0;127;(18,110)=*(18,111)=f(0,3),1536,4096;wire_vec:(18,112)=ar(0,3);0;127;(18,113)=*(18,114)=f(0,3),5632,4096;lock_meaning:(20,9),9728,32;key_bindings:(18,115)=*(18,116)=xsXKeytrans:,9760,32;cursor_font:(20,4),9792,32;atoms:(18,117)=*(18,93),9824,32;reconfigure_wm_window:(18,94),9856,96;flags:(0,15),9952,32;\\",128,0,1252,0
	.stabs	"mode_switch:(0,14),9984,32;;",128,0,1252,0
	.stabs	"Display:t(18,118)=(18,43)",128,0,1252,2048
	.stabs	":T(18,119)=s60type:(0,3),0,32;serial:(0,15),32,32;send_event:(0,3),64,32;display:(18,120)=*(18,118),96,32;window:(20,2),128,32;root:(20,2),160,32;subwindow:(20,2),192,32;time:(20,13),224,32;\\",128,0,60,1
	.stabs	"x:(0,3),256,32;y:(0,3),288,32;x_root:(0,3),320,32;y_root:(0,3),352,32;state:(0,14),384,32;keycode:(0,14),416,32;same_screen:(0,3),448,32;;",128,0,60,1
	.stabs	"XKeyEvent:t(18,121)=(18,119)",128,0,60,2048
	.stabs	"XKeyPressedEvent:t(18,122)=(18,119)",128,0,60,67584
	.stabs	"XKeyReleasedEvent:t(18,123)=(18,119)",128,0,60,67584
	.stabs	":T(18,124)=s60type:(0,3),0,32;serial:(0,15),32,32;send_event:(0,3),64,32;display:(18,120),96,32;window:(20,2),128,32;root:(20,2),160,32;subwindow:(20,2),192,32;time:(20,13),224,32;\\",128,0,60,1
	.stabs	"x:(0,3),256,32;y:(0,3),288,32;x_root:(0,3),320,32;y_root:(0,3),352,32;state:(0,14),384,32;button:(0,14),416,32;same_screen:(0,3),448,32;;",128,0,60,1
	.stabs	"XButtonEvent:t(18,125)=(18,124)",128,0,60,2048
	.stabs	"XButtonPressedEvent:t(18,126)=(18,124)",128,0,60,67584
	.stabs	"XButtonReleasedEvent:t(18,127)=(18,124)",128,0,60,67584
	.stabs	":T(18,128)=s60type:(0,3),0,32;serial:(0,15),32,32;send_event:(0,3),64,32;display:(18,120),96,32;window:(20,2),128,32;root:(20,2),160,32;subwindow:(20,2),192,32;time:(20,13),224,32;\\",128,0,60,1
	.stabs	"x:(0,3),256,32;y:(0,3),288,32;x_root:(0,3),320,32;y_root:(0,3),352,32;state:(0,14),384,32;is_hint:(0,1),416,8;same_screen:(0,3),448,32;;",128,0,60,1
	.stabs	"XMotionEvent:t(18,129)=(18,128)",128,0,60,2048
	.stabs	"XPointerMovedEvent:t(18,130)=(18,128)",128,0,60,67584
	.stabs	":T(18,131)=s68type:(0,3),0,32;serial:(0,15),32,32;send_event:(0,3),64,32;display:(18,120),96,32;window:(20,2),128,32;root:(20,2),160,32;subwindow:(20,2),192,32;time:(20,13),224,32;\\",128,0,68,1
	.stabs	"x:(0,3),256,32;y:(0,3),288,32;x_root:(0,3),320,32;y_root:(0,3),352,32;mode:(0,3),384,32;detail:(0,3),416,32;same_screen:(0,3),448,32;focus:(0,3),480,32;\\",128,0,68,1
	.stabs	"state:(0,14),512,32;;",128,0,68,1
	.stabs	"XCrossingEvent:t(18,132)=(18,131)",128,0,68,2048
	.stabs	"XEnterWindowEvent:t(18,133)=(18,131)",128,0,68,67584
	.stabs	"XLeaveWindowEvent:t(18,134)=(18,131)",128,0,68,67584
	.stabs	":T(18,135)=s28type:(0,3),0,32;serial:(0,15),32,32;send_event:(0,3),64,32;display:(18,120),96,32;window:(20,2),128,32;mode:(0,3),160,32;detail:(0,3),192,32;;",128,0,28,1
	.stabs	"XFocusChangeEvent:t(18,136)=(18,135)",128,0,28,2048
	.stabs	"XFocusInEvent:t(18,137)=(18,135)",128,0,28,67584
	.stabs	"XFocusOutEvent:t(18,138)=(18,135)",128,0,28,67584
	.stabs	":T(18,139)=s52type:(0,3),0,32;serial:(0,15),32,32;send_event:(0,3),64,32;display:(18,120),96,32;window:(20,2),128,32;key_vector:(18,140)=ar(0,3);0;31;(0,1),160,256;;",128,0,52,1
	.stabs	"XKeymapEvent:t(18,141)=(18,139)",128,0,52,2048
	.stabs	":T(18,142)=s40type:(0,3),0,32;serial:(0,15),32,32;send_event:(0,3),64,32;display:(18,120),96,32;window:(20,2),128,32;x:(0,3),160,32;y:(0,3),192,32;width:(0,3),224,32;\\",128,0,40,1
	.stabs	"height:(0,3),256,32;count:(0,3),288,32;;",128,0,40,1
	.stabs	"XExposeEvent:t(18,143)=(18,142)",128,0,40,2048
	.stabs	":T(18,144)=s48type:(0,3),0,32;serial:(0,15),32,32;send_event:(0,3),64,32;display:(18,120),96,32;drawable:(20,3),128,32;x:(0,3),160,32;y:(0,3),192,32;width:(0,3),224,32;\\",128,0,48,1
	.stabs	"height:(0,3),256,32;count:(0,3),288,32;major_code:(0,3),320,32;minor_code:(0,3),352,32;;",128,0,48,1
	.stabs	"XGraphicsExposeEvent:t(18,145)=(18,144)",128,0,48,2048
	.stabs	":T(18,146)=s28type:(0,3),0,32;serial:(0,15),32,32;send_event:(0,3),64,32;display:(18,120),96,32;drawable:(20,3),128,32;major_code:(0,3),160,32;minor_code:(0,3),192,32;;",128,0,28,1
	.stabs	"XNoExposeEvent:t(18,147)=(18,146)",128,0,28,2048
	.stabs	":T(18,148)=s24type:(0,3),0,32;serial:(0,15),32,32;send_event:(0,3),64,32;display:(18,120),96,32;window:(20,2),128,32;state:(0,3),160,32;;",128,0,24,1
	.stabs	"XVisibilityEvent:t(18,149)=(18,148)",128,0,24,2048
	.stabs	":T(18,150)=s48type:(0,3),0,32;serial:(0,15),32,32;send_event:(0,3),64,32;display:(18,120),96,32;parent:(20,2),128,32;window:(20,2),160,32;x:(0,3),192,32;y:(0,3),224,32;\\",128,0,48,1
	.stabs	"width:(0,3),256,32;height:(0,3),288,32;border_width:(0,3),320,32;override_redirect:(0,3),352,32;;",128,0,48,1
	.stabs	"XCreateWindowEvent:t(18,151)=(18,150)",128,0,48,2048
	.stabs	":T(18,152)=s24type:(0,3),0,32;serial:(0,15),32,32;send_event:(0,3),64,32;display:(18,120),96,32;event:(20,2),128,32;window:(20,2),160,32;;",128,0,24,1
	.stabs	"XDestroyWindowEvent:t(18,153)=(18,152)",128,0,24,2048
	.stabs	":T(18,154)=s28type:(0,3),0,32;serial:(0,15),32,32;send_event:(0,3),64,32;display:(18,120),96,32;event:(20,2),128,32;window:(20,2),160,32;from_configure:(0,3),192,32;;",128,0,28,1
	.stabs	"XUnmapEvent:t(18,155)=(18,154)",128,0,28,2048
	.stabs	":T(18,156)=s28type:(0,3),0,32;serial:(0,15),32,32;send_event:(0,3),64,32;display:(18,120),96,32;event:(20,2),128,32;window:(20,2),160,32;override_redirect:(0,3),192,32;;",128,0,28,1
	.stabs	"XMapEvent:t(18,157)=(18,156)",128,0,28,2048
	.stabs	":T(18,158)=s24type:(0,3),0,32;serial:(0,15),32,32;send_event:(0,3),64,32;display:(18,120),96,32;parent:(20,2),128,32;window:(20,2),160,32;;",128,0,24,1
	.stabs	"XMapRequestEvent:t(18,159)=(18,158)",128,0,24,2048
	.stabs	":T(18,160)=s40type:(0,3),0,32;serial:(0,15),32,32;send_event:(0,3),64,32;display:(18,120),96,32;event:(20,2),128,32;window:(20,2),160,32;parent:(20,2),192,32;x:(0,3),224,32;\\",128,0,40,1
	.stabs	"y:(0,3),256,32;override_redirect:(0,3),288,32;;",128,0,40,1
	.stabs	"XReparentEvent:t(18,161)=(18,160)",128,0,40,2048
	.stabs	":T(18,162)=s52type:(0,3),0,32;serial:(0,15),32,32;send_event:(0,3),64,32;display:(18,120),96,32;event:(20,2),128,32;window:(20,2),160,32;x:(0,3),192,32;y:(0,3),224,32;\\",128,0,52,1
	.stabs	"width:(0,3),256,32;height:(0,3),288,32;border_width:(0,3),320,32;above:(20,2),352,32;override_redirect:(0,3),384,32;;",128,0,52,1
	.stabs	"XConfigureEvent:t(18,163)=(18,162)",128,0,52,2048
	.stabs	":T(18,164)=s32type:(0,3),0,32;serial:(0,15),32,32;send_event:(0,3),64,32;display:(18,120),96,32;event:(20,2),128,32;window:(20,2),160,32;x:(0,3),192,32;y:(0,3),224,32;;",128,0,32,1
	.stabs	"XGravityEvent:t(18,165)=(18,164)",128,0,32,2048
	.stabs	":T(18,166)=s28type:(0,3),0,32;serial:(0,15),32,32;send_event:(0,3),64,32;display:(18,120),96,32;window:(20,2),128,32;width:(0,3),160,32;height:(0,3),192,32;;",128,0,28,1
	.stabs	"XResizeRequestEvent:t(18,167)=(18,166)",128,0,28,2048
	.stabs	":T(18,168)=s56type:(0,3),0,32;serial:(0,15),32,32;send_event:(0,3),64,32;display:(18,120),96,32;parent:(20,2),128,32;window:(20,2),160,32;x:(0,3),192,32;y:(0,3),224,32;\\",128,0,56,1
	.stabs	"width:(0,3),256,32;height:(0,3),288,32;border_width:(0,3),320,32;above:(20,2),352,32;detail:(0,3),384,32;value_mask:(0,15),416,32;;",128,0,56,1
	.stabs	"XConfigureRequestEvent:t(18,169)=(18,168)",128,0,56,2048
	.stabs	":T(18,170)=s28type:(0,3),0,32;serial:(0,15),32,32;send_event:(0,3),64,32;display:(18,120),96,32;event:(20,2),128,32;window:(20,2),160,32;place:(0,3),192,32;;",128,0,28,1
	.stabs	"XCirculateEvent:t(18,171)=(18,170)",128,0,28,2048
	.stabs	":T(18,172)=s28type:(0,3),0,32;serial:(0,15),32,32;send_event:(0,3),64,32;display:(18,120),96,32;parent:(20,2),128,32;window:(20,2),160,32;place:(0,3),192,32;;",128,0,28,1
	.stabs	"XCirculateRequestEvent:t(18,173)=(18,172)",128,0,28,2048
	.stabs	":T(18,174)=s32type:(0,3),0,32;serial:(0,15),32,32;send_event:(0,3),64,32;display:(18,120),96,32;window:(20,2),128,32;atom:(20,11),160,32;time:(20,13),192,32;state:(0,3),224,32;;",128,0,32,1
	.stabs	"XPropertyEvent:t(18,175)=(18,174)",128,0,32,2048
	.stabs	":T(18,176)=s28type:(0,3),0,32;serial:(0,15),32,32;send_event:(0,3),64,32;display:(18,120),96,32;window:(20,2),128,32;selection:(20,11),160,32;time:(20,13),192,32;;",128,0,28,1
	.stabs	"XSelectionClearEvent:t(18,177)=(18,176)",128,0,28,2048
	.stabs	":T(18,178)=s40type:(0,3),0,32;serial:(0,15),32,32;send_event:(0,3),64,32;display:(18,120),96,32;owner:(20,2),128,32;requestor:(20,2),160,32;selection:(20,11),192,32;target:(20,11),224,32;\\",128,0,40,1
	.stabs	"property:(20,11),256,32;time:(20,13),288,32;;",128,0,40,1
	.stabs	"XSelectionRequestEvent:t(18,179)=(18,178)",128,0,40,2048
	.stabs	":T(18,180)=s36type:(0,3),0,32;serial:(0,15),32,32;send_event:(0,3),64,32;display:(18,120),96,32;requestor:(20,2),128,32;selection:(20,11),160,32;target:(20,11),192,32;property:(20,11),224,32;\\",128,0,36,1
	.stabs	"time:(20,13),256,32;;",128,0,36,1
	.stabs	"XSelectionEvent:t(18,181)=(18,180)",128,0,36,2048
	.stabs	":T(18,182)=s32type:(0,3),0,32;serial:(0,15),32,32;send_event:(0,3),64,32;display:(18,120),96,32;window:(20,2),128,32;colormap:(20,7),160,32;new:(0,3),192,32;state:(0,3),224,32;;",128,0,32,1
	.stabs	"XColormapEvent:t(18,183)=(18,182)",128,0,32,2048
	.stabs	":T(18,184)=u20b:(18,185)=ar(0,3);0;19;(0,1),0,160;s:(18,186)=ar(0,3);0;9;(0,2),0,160;l:(18,187)=ar(0,3);0;4;(0,4),0,160;;",128,0,20,1
	.stabs	":T(18,188)=s48type:(0,3),0,32;serial:(0,15),32,32;send_event:(0,3),64,32;display:(18,120),96,32;window:(20,2),128,32;message_type:(20,11),160,32;format:(0,3),192,32;data:(18,184),224,160;;",128,0,48,1
	.stabs	"XClientMessageEvent:t(18,189)=(18,188)",128,0,48,2048
	.stabs	":T(18,190)=s32type:(0,3),0,32;serial:(0,15),32,32;send_event:(0,3),64,32;display:(18,120),96,32;window:(20,2),128,32;request:(0,3),160,32;first_keycode:(0,3),192,32;count:(0,3),224,32;;",128,0,32,1
	.stabs	"XMappingEvent:t(18,191)=(18,190)",128,0,32,2048
	.stabs	":T(18,192)=s20type:(0,3),0,32;display:(18,120),32,32;resourceid:(20,1),64,32;serial:(0,15),96,32;error_code:(0,11),128,8;request_code:(0,11),136,8;minor_code:(0,11),144,8;;",128,0,20,1
	.stabs	"XErrorEvent:t(18,193)=(18,192)",128,0,20,2048
	.stabs	":T(18,194)=s20type:(0,3),0,32;serial:(0,15),32,32;send_event:(0,3),64,32;display:(18,120),96,32;window:(20,2),128,32;;",128,0,20,1
	.stabs	"XAnyEvent:t(18,195)=(18,194)",128,0,20,2048
	.stabs	"_XEvent:T(18,196)=u96type:(0,3),0,32;xany:(18,195),0,160;xkey:(18,121),0,480;xbutton:(18,125),0,480;xmotion:(18,129),0,480;xcrossing:(18,132),0,544;xfocus:(18,136),0,224;xexpose:(18,143),0,320;\\",128,0,96,1
	.stabs	"xgraphicsexpose:(18,145),0,384;xnoexpose:(18,147),0,224;xvisibility:(18,149),0,192;xcreatewindow:(18,151),0,384;xdestroywindow:(18,153),0,192;xunmap:(18,155),0,224;xmap:(18,157),0,224;xmaprequest:(18,159),0,192;\\",128,0,96,1
	.stabs	"xreparent:(18,161),0,320;xconfigure:(18,163),0,416;xgravity:(18,165),0,256;xresizerequest:(18,167),0,224;xconfigurerequest:(18,169),0,448;xcirculate:(18,171),0,224;xcirculaterequest:(18,173),0,224;xproperty:(18,175),0,256;\\",128,0,96,1
	.stabs	"xselectionclear:(18,177),0,224;xselectionrequest:(18,179),0,320;xselection:(18,181),0,288;xcolormap:(18,183),0,256;xclient:(18,189),0,384;xmapping:(18,191),0,256;xerror:(18,193),0,160;xkeymap:(18,141),0,416;\\",128,0,96,1
	.stabs	"pad:(18,197)=ar(0,3);0;23;(0,4),0,768;;",128,0,96,1
	.stabs	"XEvent:t(18,198)=(18,196)",128,0,96,4096
	.stabs	"_XSQEvent:T(18,101)=s100next:(18,100),0,32;event:(18,198),32,768;;",128,0,100,0
	.stabs	"_XQEvent:t(18,199)=(18,101)",128,0,100,2048
	.stabs	":T(18,200)=s12lbearing:(0,2),0,16;rbearing:(0,2),16,16;width:(0,2),32,16;ascent:(0,2),48,16;descent:(0,2),64,16;attributes:(0,12),80,16;;",128,0,12,1
	.stabs	"XCharStruct:t(18,201)=(18,200)",128,0,12,2048
	.stabs	":T(18,202)=s8name:(20,11),0,32;card32:(0,15),32,32;;",128,0,8,1
	.stabs	"XFontProp:t(18,203)=(18,202)",128,0,8,2048
	.stabs	":T(18,204)=s80ext_data:(18,34),0,32;fid:(20,4),32,32;direction:(0,14),64,32;min_char_or_byte2:(0,14),96,32;max_char_or_byte2:(0,14),128,32;min_byte1:(0,14),160,32;max_byte1:(0,14),192,32;all_chars_exist:(0,3),224,32;\\",128,0,80,1
	.stabs	"default_char:(0,14),256,32;n_properties:(0,3),288,32;properties:(18,205)=*(18,203),320,32;min_bounds:(18,201),352,96;max_bounds:(18,201),448,96;per_char:(18,206)=*(18,201),544,32;ascent:(0,3),576,32;descent:(0,3),608,32;;",128,0,80,1
	.stabs	"XFontStruct:t(18,207)=(18,204)",128,0,80,2048
	.stabs	":T(18,208)=s16chars:(0,21),0,32;nchars:(0,3),32,32;delta:(0,3),64,32;font:(20,4),96,32;;",128,0,16,1
	.stabs	"XTextItem:t(18,209)=(18,208)",128,0,16,2048
	.stabs	":T(18,210)=s2byte1:(0,11),0,8;byte2:(0,11),8,8;;",128,0,2,1
	.stabs	"XChar2b:t(18,211)=(18,210)",128,0,2,2048
	.stabs	":T(18,212)=s16chars:(18,213)=*(18,211),0,32;nchars:(0,3),32,32;delta:(0,3),64,32;font:(20,4),96,32;;",128,0,16,1
	.stabs	"XTextItem16:t(18,214)=(18,212)",128,0,16,2048
	.stabs	":T(18,215)=u4display:(18,120),0,32;gc:(18,35),0,32;visual:(18,39),0,32;screen:(18,51),0,32;pixmap_format:(18,99),0,32;font:(18,216)=*(18,207),0,32;;",128,0,4,1
	.stabs	"XEDataObject:t(18,217)=(18,215)",128,0,4,4096
	.stabs	"XErrorHandler:t(18,218)=*(18,219)=f(0,3)",128,0,4,64
	.stabs	"XIOErrorHandler:t(18,220)=*(18,221)=f(0,3)",128,0,4,64
	.stabn	162,0,0,0
	.stabs	"BITMAP:t(17,10)=(0,15)",128,0,4,73856
	.stabn	162,0,0,0
	.stabs	"./StdPtrs.h",130,0,0,0
	.stabn	162,0,0,0
	.stabs	"IO_mode:T(2,1)=ereal_time:0,script_in:1,script_out:2,;",128,0,4,1
	.stabn	162,0,0,0
	.xstabs	".stab.index","inputSema",32,0,0,0
	.stabs	"inputSema:G(17,3)",32,0,4,0
	.xstabs	".stab.index","io_mode",32,0,0,0
	.stabs	"io_mode:G(2,1)",32,0,4,0
	.xstabs	".stab.index","script_file",32,0,0,0
	.stabs	"script_file:G(0,22)=*(1,5)",32,0,4,0
	.xstabs	".stab.index","script_file_name",32,0,0,0
	.stabs	"script_file_name:G(0,21)",32,0,4,0
	.xstabs	".stab.index","display_enabled",32,0,0,0
	.stabs	"display_enabled:G(17,1)",32,0,4,0
	.stabs	":T(0,23)=s8bc_count:(0,14),0,32;dummy:(0,2),32,16;event:(17,5),48,16;;",128,0,8,1
	.xstabs	".stab.index","MouseX",32,0,0,0
	.stabs	"MouseX:G(0,2)",32,0,2,0
	.xstabs	".stab.index","MouseY",32,0,0,0
	.stabs	"MouseY:G(0,2)",32,0,2,0
	.xstabs	".stab.index","SampleInterval",32,0,0,0
	.stabs	"SampleInterval:G(17,8)",32,0,4,0
	.xstabs	".stab.index","linked",32,0,0,0
	.stabs	"linked:G(17,1)",32,0,4,0
	.xstabs	".stab.index","head",32,0,0,0
	.stabs	"head:G(0,3)",32,0,4,0
	.xstabs	".stab.index","tail",32,0,0,0
	.stabs	"tail:G(0,3)",32,0,4,0
	.xstabs	".stab.index","read_first_script_event",36,0,0,0
	.stabs	"read_first_script_event:F(0,20)",36,0,0,0
	.stabn	192,0,1,.L456-read_first_script_event+0
	.stabs	"IO.c",132,0,0,0
	.stabn	68,0,63,.L457-read_first_script_event+0
	.stabn	68,0,64,.L460-read_first_script_event+0
	.stabn	192,0,2,.L462-read_first_script_event+0
	.stabn	68,0,65,.L463-read_first_script_event+0
	.stabn	224,0,2,.L465-read_first_script_event+0
	.stabn	68,0,69,.L467-read_first_script_event+0
	.stabn	192,0,2,.L469-read_first_script_event+0
	.stabn	68,0,70,.L470-read_first_script_event+0
	.stabn	224,0,2,.L472-read_first_script_event+0
	.stabn	68,0,74,.L473-read_first_script_event+0
	.stabn	68,0,75,.L475-read_first_script_event+0
	.stabn	224,0,1,.L476-read_first_script_event+0
	.xstabs	".stab.index","open_script_file",36,0,0,0
	.stabs	"open_script_file:F(0,20)",36,0,0,0
	.stabn	192,0,1,.L481-open_script_file+0
	.stabn	68,0,80,.L482-open_script_file+0
	.stabn	68,0,81,.L485-open_script_file+0
	.stabn	192,0,2,.L487-open_script_file+0
	.stabn	68,0,82,.L488-open_script_file+0
	.stabn	224,0,2,.L490-open_script_file+0
	.stabn	68,0,84,.L491-open_script_file+0
	.stabn	224,0,1,.L492-open_script_file+0
	.xstabs	".stab.index","read_script_event",36,0,0,0
	.stabs	"read_script_event:F(0,20)",36,0,0,0
	.stabn	192,0,1,.L497-read_script_event+0
	.xstabs	".stab.index","$XBRBNkBwjBUuCkH.read_script_event.events_to_ignore",32,0,0,0
	.stabs	"$XBRBNkBwjBUuCkH.read_script_event.events_to_ignore:V(0,3)",38,0,4,$XBRBNkBwjBUuCkH.read_script_event.events_to_ignore-Ddata.data+0
	.stabn	68,0,94,.L504-read_script_event+0
	.stabn	192,0,2,.L506-read_script_event+0
	.stabn	68,0,95,.L507-read_script_event+0
	.stabn	68,0,97,.L509-read_script_event+0
	.stabn	68,0,98,.L513-read_script_event+0
	.stabn	192,0,3,.L514-read_script_event+0
	.stabn	68,0,100,.L516-read_script_event+0
	.stabn	68,0,101,.L518-read_script_event+0
	.stabn	68,0,102,.L520-read_script_event+0
	.stabn	68,0,103,.L521-read_script_event+0
	.stabn	68,0,105,.L523-read_script_event+0
	.stabn	68,0,106,.L525-read_script_event+0
	.stabn	68,0,107,.L527-read_script_event+0
	.stabn	68,0,108,.L528-read_script_event+0
	.stabn	68,0,110,.L530-read_script_event+0
	.stabn	68,0,112,.L532-read_script_event+0
	.stabn	68,0,114,.L534-read_script_event+0
	.stabn	68,0,118,.L536-read_script_event+0
	.stabn	68,0,119,.L537-read_script_event+0
	.stabn	224,0,3,.L538-read_script_event+0
	.stabn	68,0,122,.L541-read_script_event+0
	.stabn	68,0,124,.L543-read_script_event+0
	.stabn	192,0,3,.L545-read_script_event+0
	.stabn	68,0,126,.L546-read_script_event+0
	.stabn	68,0,127,.L547-read_script_event+0
	.stabn	68,0,128,.L549-read_script_event+0
	.stabn	192,0,4,.L551-read_script_event+0
	.stabn	68,0,129,.L552-read_script_event+0
	.stabn	68,0,130,.L553-read_script_event+0
	.stabn	68,0,131,.L556-read_script_event+0
	.stabn	192,0,5,.L558-read_script_event+0
	.stabn	68,0,132,.L559-read_script_event+0
	.stabn	68,0,133,.L560-read_script_event+0
	.stabn	224,0,5,.L562-read_script_event+0
	.stabn	192,0,5,.L564-read_script_event+0
	.stabn	68,0,135,.L565-read_script_event+0
	.stabn	224,0,5,.L567-read_script_event+0
	.stabn	224,0,4,.L568-read_script_event+0
	.stabn	68,0,138,.L569-read_script_event+0
	.stabn	68,0,139,.L570-read_script_event+0
	.stabn	224,0,3,.L571-read_script_event+0
	.stabn	224,0,2,.L572-read_script_event+0
	.stabs	"re_start_window_input:P(0,20)",36,0,0,0
	.stabs	"addEvent:P(0,20)",36,0,0,0
	.stabn	68,0,142,.L574-read_script_event+0
	.stabn	224,0,1,.L575-read_script_event+0
	.xstabs	".stab.index","addEvent",36,0,0,0
	.stabs	"addEvent:F(0,20);(0,24)=(0,24)",36,0,0,0
	.stabs	"e:p(17,5)",160,0,2,70
	.stabn	192,0,1,.L579-addEvent+0
	.stabn	68,0,149,.L582-addEvent+0
	.stabn	68,0,150,.L584-addEvent+0
	.stabn	68,0,152,.L586-addEvent+0
	.stabn	192,0,2,.L592-addEvent+0
	.stabn	68,0,154,.L593-addEvent+0
	.stabn	68,0,155,.L595-addEvent+0
	.stabn	68,0,156,.L597-addEvent+0
	.stabn	68,0,157,.L598-addEvent+0
	.stabn	68,0,158,.L600-addEvent+0
	.stabn	192,0,3,.L602-addEvent+0
	.stabn	68,0,159,.L603-addEvent+0
	.stabn	68,0,160,.L604-addEvent+0
	.stabn	68,0,162,.L605-addEvent+0
	.stabn	68,0,163,.L606-addEvent+0
	.stabn	224,0,3,.L607-addEvent+0
	.stabn	224,0,2,.L608-addEvent+0
	.stabs	"signalInputSema:P(0,20)",36,0,0,0
	.stabn	68,0,166,.L609-addEvent+0
	.stabn	224,0,1,.L610-addEvent+0
	.xstabs	".stab.index","nextEvent",36,0,0,0
	.stabs	"nextEvent:F(17,5)",36,0,0,0
	.stabn	192,0,1,.L615-nextEvent+0
	.stabs	"event:(17,5)",128,0,2,-6
	.stabn	68,0,173,.L616-nextEvent+0
	.stabn	68,0,174,.L618-nextEvent+0
	.stabn	68,0,175,.L620-nextEvent+0
	.stabn	68,0,176,.L621-nextEvent+0
	.stabn	68,0,177,.L622-nextEvent+0
	.stabn	224,0,1,.L623-nextEvent+0
	.xstabs	".stab.index","isEmptyEventQ",36,0,0,0
	.stabs	"isEmptyEventQ:F(17,1)",36,0,0,0
	.stabn	192,0,1,.L628-isEmptyEventQ+0
	.stabn	68,0,181,.L629-isEmptyEventQ+0
	.stabn	68,0,182,.L630-isEmptyEventQ+0
	.stabn	224,0,1,.L631-isEmptyEventQ+0
	.xstabs	".stab.index","getTime",36,0,0,0
	.stabs	"getTime:F(17,8)",36,0,0,0
	.stabn	192,0,1,.L636-getTime+0
	.stabs	"Time_now:(16,1)",128,0,10,-14
	.stabn	68,0,189,.L638-getTime+0
	.stabn	68,0,190,.L639-getTime+0
	.stabs	"ftime:P(0,3)",36,0,0,0
	.stabn	68,0,191,.L640-getTime+0
	.stabn	224,0,1,.L641-getTime+0
	.xstabs	".stab.index","absTime",36,0,0,0
	.stabs	"absTime:F(17,8)",36,0,0,0
	.stabn	192,0,1,.L646-absTime+0
	.stabn	68,0,200,.L648-absTime+0
	.stabs	"time:P(0,4);(0,25)=*(0,4)",36,0,0,0
	.stabn	68,0,202,.L649-absTime+0
	.stabn	224,0,1,.L650-absTime+0
	.xstabs	".stab.index","do_time",36,0,0,0
	.stabs	"do_time:F(0,20);(0,14)",36,0,0,0
	.stabs	"t:p(17,8)",160,0,4,68
	.stabn	192,0,1,.L655-do_time+0
	.xstabs	".stab.index","$XBRBNkBwjBUuCkH.do_time.last_ms",32,0,0,0
	.stabs	"$XBRBNkBwjBUuCkH.do_time.last_ms:V(17,8)",40,0,4,$XBRBNkBwjBUuCkH.do_time.last_ms-Bbss.bss+0
	.stabs	"abs_clock:(0,14)",128,0,4,-4
	.stabs	"ms_intvl:(0,14)",128,0,4,-8
	.stabn	68,0,209,.L657-do_time+0
	.stabn	68,0,211,.L658-do_time+0
	.stabn	68,0,214,.L660-do_time+0
	.stabn	192,0,2,.L662-do_time+0
	.stabn	68,0,219,.L664-do_time+0
	.stabn	192,0,3,.L670-do_time+0
	.stabs	"Time_now:(16,1)",128,0,10,-18
	.stabn	68,0,222,.L671-do_time+0
	.stabn	68,0,224,.L672-do_time+0
	.stabn	68,0,226,.L673-do_time+0
	.stabn	68,0,227,.L674-do_time+0
	.stabn	68,0,228,.L675-do_time+0
	.stabn	224,0,3,.L676-do_time+0
	.stabn	224,0,2,.L677-do_time+0
	.stabn	68,0,231,.L679-do_time+0
	.stabn	68,0,232,.L680-do_time+0
	.stabn	224,0,1,.L681-do_time+0
	.xstabs	".stab.index","$XARBNkBwjBUuCkH.eb",32,0,0,0
	.stabs	"$XARBNkBwjBUuCkH.eb:S(0,26)=ar(0,3);0;255;(17,5)",40,0,512,$XARBNkBwjBUuCkH.eb+0
	.xstabs	".stab.index","$XARBNkBwjBUuCkH.event_record",32,0,0,0
	.stabs	"$XARBNkBwjBUuCkH.event_record:S(0,23)",40,0,8,$XARBNkBwjBUuCkH.event_record+0
	.xstabs	".stab.index","dispH",32,0,0,0
	.stabs	"dispH:G(0,14)",32,0,4,0
	.xstabs	".stab.index","dispW",32,0,0,0
	.stabs	"dispW:G(0,14)",32,0,4,0
	.stabs	"fatal:P(0,20)",36,0,0,0
	.stabs	"log:P(0,20)",36,0,0,0
	.stabs	"time:P(3,56);(0,25)",36,0,0,0
	.stabs	"fwrite:P(1,1);(0,27)=*(0,28)=k(0,20);(0,14);(0,14);(0,29)=*(1,3)",36,0,0,0
	.stabs	"fread:P(1,1);(11,2);(0,14);(0,14);(0,29)",36,0,0,0
	.stabs	"fprintf:P(0,3);(0,29);(0,30)=*(0,31)=k(0,1);0",36,0,0,0
	.stabs	"fopen:P(0,22);(0,32)=*(0,33)=k(0,1);(0,34)=*(0,35)=k(0,1)",36,0,0,0
	.stabs	"fflush:P(0,3);(0,29)",36,0,0,0
	.stabs	"fclose:P(0,3);(0,29)",36,0,0,0
	.stabs	"",98,0,0,0
	.ident	"@(#)stdio.h	1.1	90/04/27 SMI"
	.ident	"@(#)types.h	1.10	93/07/21 SMI"
	.ident	"@(#)select.h	1.10	92/07/14 SMI"
	.ident	"@(#)time.h	2.36	93/06/09 SMI"
	.ident	"@(#)feature_tests.h	1.5	93/04/26 SMI"
	.ident	"@(#)types.h	1.10	93/07/21 SMI"
	.ident	"@(#)time.h	1.15	93/04/13 SMI"
	.ident	"@(#)siginfo.h	1.33	93/07/09 SMI"
	.ident	"@(#)machsig.h	1.9	92/07/14 SMI"
	.ident	"@(#)sysmacros.h	1.1	90/04/27 SMI"
	.ident	"@(#)timeb.h	1.8	92/07/14 SMI"
	.ident	"@(#)types.h	1.10	93/07/21 SMI"
	.ident	"acomp: SC3.0.1 15 Jun 1994 Sun C 3.0.1"
