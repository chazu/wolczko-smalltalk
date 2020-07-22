/*
  	Smalltalk-80 Virtual Machine:
	Main dispatch loop

	Copyright (c) 1984,1985,1986,1987,1988,1989 Mario Wolczko
	All rights reserved.

This module contains the bytecode fetch and dispatch loop.

$Log: st80.c,v $
 * Revision 5.0  1994/08/16  01:01:44  mario
 * Added 'm' option
 *
 * Revision 4.1  89/06/22  11:47:10  mario
 * Cleaned up for lint.
 * Moved much machine-dependent code into the I/O modules.
 * Added more rational logging facilities.
 * 
 * Revision 3.4  87/03/24  21:06:09  miw
 * Fixed minor bug in simulation version.
 * 
 * Revision 3.3  87/03/21  14:35:32  miw
 * Added simulation, static grouping, parsing of command line switches.
 * 
 * Revision 3.2  87/03/13  10:27:42  miw
 * Replaced code that used asIsMethCtx macro.
 * 
 * Revision 3.1  87/03/11  16:56:45  miw
 * Removed support for Orion and Perq.
 * Removed context pools and D-B.
 * 
 * Revision 2.10  87/02/13  19:58:10  miw
 * Converted to 32-bits: removed all mention of the odd field, changed size of
 * printing field for oops to 8 bytes.
 * 
 * Revision 2.7  86/01/23  22:01:46  miw
 * Improved single step facilities, especially display of the
 * active context.  Also, class names now appear in place of oops.
 * 
 * Revision 2.6  86/01/17  21:25:37  miw
 * Added ORION support.  Added void where necessary.  Tidied single_step().
 * 
 * Revision 2.5  86/01/17  18:58:08  miw
 * Removed support for the APOLLO.
 * Altered single step: removed snapshotting, added g.c. and dump of ZCT.
 * 
 * Revision 2.4  85/12/12  10:35:36  miw
 * Removed instrPtr, stackPtr, and added ip, sp, ABSmethod and ABSrcvr.
 * Made necessary corresponding alterations.
 * 
 * Revision 2.3  85/12/07  15:55:05  miw
 * Moved the SmallInteger tag bit in oops from lsb to msb.
 * 
 * Revision 2.2  85/12/04  20:27:53  miw
 * Added the call to BBlt_setup(), and altered the display of objects to
 * deal with cached bitmaps.
 * 
 * Revision 2.1  85/11/19  17:43:09  miw
 * Stabilised with all other modules at Rev 2.1
 * 
 * Revision 1.17  85/11/18  20:03:23  miw
 * Added event loop for all machines (to enable script-driven input).
 * Fixed single step code so that large objects could be inspected.
 * 
 * Revision 1.16  85/11/14  16:36:34  miw
 * Added call to get_window_statistics().
 * 
 * Revision 1.15  85/09/18  19:41:05  miw
 * Added script facility.
 * 
 * Revision 1.14  85/09/13  11:12:09  miw
 * Added direct pointers to active and home contexts, and debugging
 * info to print them out.  Removed the primitive activation trace,
 * 
 * Revision 1.13  85/08/07  08:33:56  miw
 * Added loctni() macro.
 * 
 * Revision 1.12  85/07/23  21:01:50  miw
 * Changed the trace printout to give 32bits for loctn() in the object
 * table.
 * 
 * Revision 1.11  85/07/05  13:28:14  miw
 * Removed BC_TRACE and MSG_TRACE stuff.
 * 
 * Revision 1.10  85/06/27  19:12:15  miw
 * Made the method lookup module a conditionally-included file.
 * Added the Quit function to single step mode.
 * Reordered the include files (to avoid the "redefined" warning from
 * cc).
 * 
 * Revision 1.9  85/04/23  16:52:37  miw
 * Removed potential infinite recursion in stack dump.
 * 
 * Revision 1.8  85/04/23  11:50:34  miw
 * Improved the printing of objects in single step mode.
 * Added a check on refce. counts before and after snapshots (ss mode).
 * 
 * Revision 1.7  85/04/22  10:10:13  miw
 * Improved printing of stack dump.
 * .,
 * 
 * Revision 1.6  85/04/21  20:40:35  miw
 * Added stack dump.
 * 
 * Revision 1.5  85/04/02  18:36:10  miw
 * Enabled snapshots to be written in single step mode.
 * 
 * Revision 1.4  85/04/02  11:33:41  miw
 * Removed regular refresh from PERQ version.
 * 
 * Revision 1.3  85/03/06  15:32:09  miw
 * Added support for Apollo.
 * Fixed initialisation bug (writing to act[256]).
 * Turned extened trace of prim.95 off.
 * 
 * Revision 1.2  85/01/14  17:54:21  miw
 * Tracing turned off for VAX-PERQ tests
 * 
 * Revision 1.1  85/01/10  12:57:57  miw
 * Initial revision
 * 
*/

static char *rcsid= "$Header: /home/mario/st/RCS/st80.c,v 5.0 1994/08/16 01:01:44 mario Exp mario $";

#include <stdio.h>
#include "ST.h"
#include "BCFreq.h"
#include "OM.h"
#include "OMMacros.h"
#include "CtxMacros.h"
#include "ClassPtrs.h"
#include "StdPtrs.h"
#include "IO.h"

/* define the Virtual Machine's "Registers" */
OOP
  activeContext,
  homeContext,
  receiver,
  method,
  msgSelector,
  newMethod;
WORD
  argCount;
BYTE
  currentBC,
  primIndex;
BYTE
  *ip;
WORD
  *sp,
  *ABSmethod;
CONTEXT
  *ac,
  *hc;

unsigned bc_count= 0;       /* bytecode count */
int process_count= PROCESS_BC; /* set to zero to invoke process switch */
char *trace_file_name= "st80.trace";

extern void
  IO_init(),
  OM_init(),
  BitBlt_init(),
  Float_init(),
  doInput(),
  checkProcessSwitch(),
  (*dispTable[])();
extern OOP
  firstContext();
extern BOOL
  prFlushCache();


#ifdef SINGLE_STEP
  void single_step();
  extern int meth_lookup_init();
  FILE *ssif=stdin, *ssof=stderr;
  extern OOP free_ptr, inputSema, limitSema;
  extern unsigned words_left, oops_left, word_limit, ptr_limit;
  static int no_to_exec= 0;
#endif


#ifdef SIM
  BOOL s_group= FALSE;		/* static grouping */
  extern void do_static_grouping();
  extern OOP special_firstContext();
  char *trace_opts= "";		/* these are the things we want to monitor */
  extern void trace();
#endif SIM


					/*ST!Interpreter!interpret!*/
					/*ST!Interpreter!cycle!*/
void main(argc, argv)
int argc;
char *argv[];
{
	extern void parse_opts();
	int input= INPUT_BC;

	parse_opts(argc, argv);

#ifdef SINGLE_STEP
	(void)fprintf(ssof, "Loading method index...");
	(void)fprintf(ssof, "%d methods in index.\n", method_lookup_init());
#else
	(void)freopen(trace_file_name, "w", stderr);
#endif

	/* initialise other modules */
	IO_init();
	OM_init();
	Float_init();
	BitBlt_init();

	(void)prFlushCache();
#ifndef SIM
	activeContext= firstContext();
#else SIM
	activeContext= special_firstContext();
	if (s_group) {
		do_static_grouping();
	}
#endif SIM
	cacheActiveContext;
	fetchCtxRegs;

	for (;;) {
		if (--input <= 0) {
			doInput();
			input= INPUT_BC;
		}

		if (--process_count < 0) {
			checkProcessSwitch();
			process_count= PROCESS_BC;
		}

		bc_count++;
#ifdef SINGLE_STEP
		currentBC= nextByte;
		single_step();
		(*dispTable[currentBC])();
#else
		(*dispTable[currentBC= nextByte])();
#endif
#ifdef SIM
		trace('X', bc_count, 0);
#endif
	}
}


void parse_opts(argc, argv)
int argc;
char *argv[];
{
	extern char *optarg;
	extern char *image_file_name;
	int c;

	while (c= getopt(argc,argv,"grwdi:t:s:T:"), c != EOF) {
		switch (c) {
		case 'r':
		  	io_mode= script_in;
			loge("Script mode: r");
			break;
#ifndef SIM
		case 'w':
		  	io_mode= script_out;
			loge("Script mode: w");
			break;
#endif SIM
#ifdef SIM
		case 'g':  /* static grouping */
		  	s_group= TRUE;
			loge("Statically grouping at initialisation");
			break;
#endif SIM
		case 'd':  /* disable display */
		  	display_enabled= !display_enabled;
			loge("Display disabled");
			break;
		case 'i': /* where the image is */
		  	image_file_name= optarg;
			break;
		case 't':  /* trace file */
		  	trace_file_name= optarg;
			break;
#ifdef SIM
		case 'T': /* simulation tracing options */
			trace_opts= optarg;
			break;
#endif SIM
		case 's':  /* script file */
		  	script_file_name= optarg;
			break;
		default:
			(void)fprintf(stderr,
				"usage: %s [-g] [-d] [-r] [-w] [-i image] \
[-t tracefile] [-s scriptfile] [-T tracing options]\n",
				argv[0]);
			exit(1);
		}
	}
}


void fatal(s)			/* print an error message, clean up and die */
char *s;
{
	extern void IO_exit();
	(void)fprintf(stderr, "Fatal error: %s\n", s);
	IO_exit();
}


void log(s)
char *s;
{
	(void)fprintf(stderr, "st80: %s @ bytecode %u\n", s, bc_count);
	(void)fflush(stderr);
}

void logs(s)
char *s;
{
	(void)fprintf(stderr, "st80: %s", s);
	(void)fflush(stderr);
}

void loge(s)
char *s;
{
	(void)fprintf(stderr, "%s\n", s);
	(void)fflush(stderr);
}

#ifdef SINGLE_STEP

#include "MethodLookup.c"

/* print a readable form of object Oop to file f, listing the contents of
   the first n words (if appropriate) */
static void print_obj(Oop, f, n)
OOP Oop;
FILE *f;
{
	WORD Size= size(Oop), i;
	
	(void)fprintf(f, "%08x\n", Oop);
	if (isInt(Oop)) {
		(void)fprintf(f, "\tInteger %d\n", intVal(Oop));
		return;
	}
	if (isFree(Oop)) {
		(void)fprintf(f, "\tF%05x\n\n", loctn(Oop));
		return;
	}
	(void)fprintf(f, "\t%3d%c%c %08x\n",
		count(Oop), ptrs(Oop)? 'P' : 'W', 
		bitmap(Oop) ? 'B' : ' ', loctn(Oop));
	(void)fprintf(f, "\t%5d %08x\t", Size, class(Oop));
	print_class_name(class(Oop), f);
	(void)putc('\n', f);
	if (bitmap(Oop)) {
		if (loctnb(Oop)==theScreen) {
			(void)fprintf(f, "Display object\n");
		} else {
			(void)fprintf(f, "Bits...\n");
		}
	} else {
		for (i= 0; i < toWords(Size) && i < n; ++i)
		  (void)fprintf(f, "%08x\t\t%08x\n", i, HCword(Oop, i));
	}
}

static void print_expanded_class(c, f)
OOP c;
FILE *f;
{
	if (isInt(c))
		(void)fprintf(f, "Integer: %d", intVal(c));
	else {
		(void)putc('a', f); print_class_name(class(c), f);
	}
}

static void print_active_context(f)
FILE *f;
{
	WORD Size= size(activeContext), i;
	
	(void)fprintf(f, "active context %08x\n", activeContext);
	if (isFree(activeContext)) {
		(void)fprintf(f, "Free!\n");
		return;
	}
	if (!ptrs(activeContext)) {
		(void)fprintf(f, "Not a pointer object!\n");
		return;
	}
	if (bitmap(activeContext)) {
		(void)fprintf(f, "A bitmap!\n");
		return;
	}
	(void)fprintf(f, "\trc=%d %08x\n",
		count(activeContext), loctn(activeContext));
	(void)fprintf(f, "\t%5d ", Size);
	print_class_name(class(activeContext), f);
	(void)putc('\n', f);
	for (i= 0; i < toWords(Size); ++i) {
		OOP oop= HCword(activeContext, i);
		(void)fprintf(f, "%08x%s%08x\t", i,
			      i==sp-(WORD*)ac ? "--> " : "\t", oop);
		if (i <= sp-(WORD *)ac)
			switch (i) {
			case SENDER:
				if (class(oop)==CLASS_METH_CTX)
					print_meth_name(fetchPtr(METHOD, oop), f);
				else
					(void)fprintf(f, "Block context");
				break;
			case METHOD:
				if (acIsMethCtx)
					print_meth_name(oop, f);
				else
					print_expanded_class(oop, f);
				break;
			case HOME:
				if (!acIsMethCtx)
					print_meth_name(fetchPtr(METHOD, homeContext), f);
				else
					print_expanded_class(oop, f);
				break;
			default:
				print_expanded_class(oop, f);
				break;
			}
		(void)putc('\n', f);
	}
}

/* This is a cheap and cheerful UI for a STVM front panel.
   You can inspect the internal state of the VM, execute bytecodes,
   and various other random things.  The user if prompted with a "menu"
   of single-letter commands, from which one may be chosen.  Some commands
   take a following argument.  The only error message is '?' ... maybe
   someone will sue me for stealing the "look and feel" of the ed(1)
   user interface! */
static void single_step()
{
	extern void saveContext(), atoi(), IO_exit(), reclaim(),
		compact(), stack_dump();
	char s[2];
	int c;
	OOP oop;

	if (--no_to_exec > 0)
		return;

	storeCtxRegs;
	
	for (;;) {
		(void)fprintf(ssof, "<%d,%d>aAcdfghiIpQrx? ",
			      bc_count, currentBC);
		if (fscanf(ssif, "%1s", s) == EOF) {
			IO_exit(); /* never returns */
		}
		switch (s[0]) {
		case 'A':	/* dump of active context */
			print_active_context(ssof);
			break;
		case 'c':       /* print interesting context regs */
			(void)fprintf(ssof, "inputSema= %08x\n", inputSema);
			(void)fprintf(ssof, "oops_left= %d, oop_limit= %d\n",
				      oops_left, ptr_limit);
			(void)fprintf(ssof, "words_left= %d, word_limit= %d\n",
				      words_left, word_limit);
			(void)fprintf(ssof, "limitSema= %08x\n", limitSema);
			break;
		case 'd':       /* stack dump */
			stack_dump(activeContext, 0);
			break;
		case 'f':       /* print free lists */
			for (c= 0; c <= BIG_SIZE; ++c) {
				(void)fprintf(ssof, "%d:\t", c);
				for (oop= FCLhead(c); oop != NON_PTR; oop= class(oop))
					(void)fprintf(ssof, "(%08x,%08x) ",
						      oop, loctn(oop));
				(void)fputc('\n', ssof);
			}
			break;
		case 'g': 	/* collect garbage, compact */
			saveContext();
			reclaim();
			compact();
			(void)fprintf(ssof, "Mustn't continue now...\n");
			break;
		case 'h':       /* help */
			(void)fprintf(ssof, "a - active and home contexts\n");
			(void)fprintf(ssof, "c - interesting context regs\n");
			(void)fprintf(ssof, "d - stack dump\n");
			(void)fprintf(ssof, "f - free lists\n");
			(void)fprintf(ssof, "g - garbage collect\n");
			(void)fprintf(ssof, "ip- object p, i for first 100 oops, I for all\n");
			(void)fprintf(ssof, "pn- first n free oops\n");
			(void)fprintf(ssof, "Q - quit\n");
			(void)fprintf(ssof, "r - registers\n");
			(void)fprintf(ssof, "x[n] - execute 1[n] bytecodes\n");
			break;
		case 'p':       /* p n= print first free n oops */
			(void)fscanf(ssif, "%d", &c);
			for (oop= free_ptr; c-->0 && oop != NON_PTR; oop= loctni(oop))
				(void)fprintf(ssof, "%08x ", oop);
			(void)fputc('\n', ssof);
			break;
		case 'Q':
			IO_exit();                   
		case 'I':
		case 'i':       /* ip= inspect object p */
			(void)fscanf(ssif, "%x", &oop);
			print_obj(oop, ssof, s[0]=='I' ? 65536 : 100);
			break;
		case 'x':       /* xn= execute n instrs. x= execute 1 instr */
			if ((c= getc(ssif)) == '\n') {
				no_to_exec= 1;
				return;
			}
			(void)ungetc(c, ssif);
			(void)fscanf(ssif, "%d", &no_to_exec);
			return;
		case 'r': /* reg dump */
			(void)fprintf(ssof, "receiver= %08x\n", receiver);
			(void)fprintf(ssof, "method= %08x\n", method);
			(void)fprintf(ssof, "msgSelector= %08x\n", msgSelector);
			(void)fprintf(ssof, "newMethod= %08x\n", newMethod);
			(void)fprintf(ssof, "stackPtr= %4x\n", sp - (WORD *)ac);
			(void)fprintf(ssof, "instrPtr= %4x\n", ip - (BYTE *)ABSmethod);
			(void)fprintf(ssof, "argCount= %4x\n", argCount);
			(void)fprintf(ssof, "currentBC= %02x\n", currentBC);
			(void)fprintf(ssof, "primIndex= %02x\n", primIndex);
			/* drop thru */
		case 'a':
			(void)fprintf(ssof, "active context= %08x\n", activeContext);
			(void)fprintf(ssof, "abs. address of ac= %08x\n", (int)ac);
			(void)fprintf(ssof, "home context= %08x\n", homeContext);
			(void)fprintf(ssof, "abs. address of hc= %08x\n", (int)hc);
			break;
		default:
			(void)putc('?', ssof);
			/* yes, folks, the "look and feel" of the ed
			   user interface ... */
		}
	}
}

static void stack_dump(ctx, v)
OOP ctx;
{
	extern void context_dump();
	if (ctx != NIL_PTR) {
		if (fetchClass(ctx) == CLASS_BLOCK_CTX) {
			(void)fprintf(ssof, "%*x  Block context\n", v+v+4, ctx);
			(void)fprintf(ssof, "%*s", v+v+13, "from sender:\n");
			stack_dump(fetchPtr(SENDER, ctx), v+1);
			(void)fprintf(ssof, "%*s", v+v+11, "from home:\n");
			context_dump(fetchPtr(HOME, ctx), v+1);
		} else {
			context_dump(ctx, v);
			stack_dump(fetchPtr(SENDER, ctx), v);
		}
	}
}

static void context_dump(ctx, v)
OOP ctx;
{
	(void)fprintf(ssof, "%*x  ", v+v+4, ctx);
	print_meth_name(fetchPtr(METHOD, ctx), ssof);
	(void)fputc('\n', ssof);
}

#endif
