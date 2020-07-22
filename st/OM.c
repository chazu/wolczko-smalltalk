/*
  	Smalltalk-80 Virtual Machine:
	Routines of the Object Memory

	Copyright (c) 1984,1985,1986,1987,1988,1989 Mario Wolczko
	All rights reserved.

The reference counting scheme is NOT the "space efficient" one presented
in G/R.  Reference counts are stored in the efficient cyclic form 
(the overflow bit is in the lsb).

$Log: OM.c,v $
 * Revision 5.0  1994/08/16  00:59:11  mario
 * More tracing
 *
 * Revision 4.2  91/06/20  21:51:02  mario
 * Fixed bug in tracing (no output being produced!)
 * 
 * Revision 4.1  89/06/21  19:55:06  mario
 * Added loctnb field to OT union.
 * Tidied up interface to IO modules to make it machine-inpdependent.
 * Added declarations and casts to keep lint quiet.
 * 
 * Revision 3.6  89/05/03  15:53:30  mario
 * Changed trace() to do conditional tracing.
 * 
 * Revision 3.5  87/05/19  17:41:09  miw
 * Added selective tracing of method contexts.
 * 
 * Revision 3.4  87/03/24  21:05:27  miw
 * Made trivial alteration to low space notifier.
 * 
 * Revision 3.3  87/03/21  14:44:39  miw
 * Added simulation code.
 * Turned off low space notification.
 * 
 * Revision 3.2  87/03/11  16:30:12  miw
 * Added ActiveProcess() call, saturated counts of guaranteed oops after a gc. 
 * 
 * Revision 3.1  87/03/11  16:13:28  miw
 * Removed D-B.  Removed support for Perq and Orion. Simplified cursor
 * changes.  Removed context pools.
 * 
 * Revision 2.11  87/02/13  20:15:01  miw
 * Converted to 32-bit images:
 * - Altered the size field to count bytes rather than words, and all
 * 	appropriate calculations.
 * - Cursors are now arrays of WORD16
 * - Only 1 free chunk is now required after a garbage collection.
 * - Removed all mention of the odd field.
 * - Changed ConvBitMap to allocate bitmaps with widths which are multiples of
 * 	32.
 * 
 * Revision 2.10  87/02/07  17:42:28  miw
 * Added cursor-switching code (for compaction and gc) for SUN.
 * 
 * Revision 2.9  86/09/16  11:46:55  miw
 * Modified ConvBitMap for SUN.
 * 
 * Revision 2.8  86/01/18  19:50:28  miw
 * Expanded PERQ halftones to 64x32 in ConvBitMap; seemed sensible to
 * make use of already allocated memory.
 * 
 * Revision 2.7  86/01/17  21:09:04  miw
 * Altered ConvBitMap to convert halftones into 32x32 bitmaps.
 * Made fetchBMword and storeBMword more portable (at least to the Orion).
 * 
 * Revision 2.6  86/01/17  18:44:30  miw
 * Added Deutsch-Bobrow garbage collection.
 * Altered lastPointer() so that oops above the top-of-stack in contexts
 * aren't ref.counted.
 * Fixed the low space notifier---used to repeatedly notify when space
 * was low.
 * Made swapPtrs more efficient.
 * 
 * Revision 2.5  85/12/12  10:37:30  miw
 * Added extra caching in the compactor, because of direct pointers held to 
 * current instruction, top-of-stack, receiver and method.
 * 
 * Revision 2.4  85/12/07  15:53:33  miw
 * Split the otRest[] array into otRest[] and otCount[], due to the move
 * in position of oop tag bit from lsb to msb.
 * 
 * Revision 2.3  85/12/04  20:21:30  miw
 * Fixed a bug in the compacter (when sweeping).
 * Now cache bitmaps, using ConvBitMap() to convert, and fetchBMWord()
 * and storeBMWord() to access.
 * Removed the HDR_SIZE constant.
 * 
 * Revision 2.2  85/11/23  17:05:50  miw
 * Added separate arrays for sizes and classes. This led to extensive changes
 * in the use of the lastPointer() call, in the compacter, and the mark/sweep
 * collector.  Also had to modify swapPtrs().
 * 
 * Revision 2.1  85/11/19  17:40:50  miw
 * Stabilised with all other modules at Rev 2.1
 * 
 * Revision 1.14  85/11/18  20:00:09  miw
 * A the type of a ref.count variable from WORD to BYTE.
 * 
 * Revision 1.13  85/09/13  10:48:03  miw
 * Cache pointers to active and home contexts after a compaction has
 * taken place.
 * /.
 * 
 * Revision 1.12  85/08/07  08:35:37  miw
 * Changed access to O.T. free chain so that loctni() is used.
 * 
 * Revision 1.11  85/07/23  20:59:10  miw
 * Changed ref. counting so that only zero trap on count down requires
 * a function call.
 * Made the loctn field of the object table be a direct pointer into
 * the object memory.
 * 
 * Revision 1.10  85/07/05  13:04:55  miw
 * Altered the ref.counting scheme to use l.s.b. as overflow bit.
 * Forced TRUE and FALSE to have saturated counts.
 * 
 * Revision 1.9  85/06/27  19:02:46  miw
 * Split the object table into two arrays, one for object memory indices,
 * one for the rest.  Replaced the assignments to bit fields of the
 * object table with new macros.
 * 
 * Revision 1.8  85/04/23  16:34:53  miw
 * Added printout of number of words recovered by garbage collector.
 * 
 * Revision 1.7  85/04/23  11:51:52  miw
 * Corrected garbage collector: marks from activeContext, and increases
 * ref. count of activeContext.
 * 
 * Revision 1.6  85/04/15  12:57:17  miw
 * Fixed (again) sign extension in allocate() for Apollo by coercing
 * to (int).
 * 
 * Revision 1.5  85/04/03  11:21:08  miw
 * Deleted space_occupied_by(). 
 * 
 * Revision 1.4  85/04/02  18:33:08  miw
 * Slightly altered "roots" of mark/sweep collector
 * 
 * Revision 1.3  85/04/02  11:26:13  miw
 * Fixed compacter (by removing the ``extra word'' from big objects).
 * Added mark/sweep collector.
 * 
 * Revision 1.2  85/01/14  17:55:27  miw
 * 
 * Revision 1.1  85/01/10  12:54:33  miw
 * Initial revision
 * 
*/

static char *rcsid= "$Header: /home/mario/st/RCS/OM.c,v 5.0 1994/08/16 00:59:11 mario Exp mario $";

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "ST.h"

#include "OM.h"
#include "OMMacros.h"
#include "StdPtrs.h"
#include "ClassPtrs.h"

#include "CtxMacros.h"
#include "Regs.h"

#define HEAP_SPACE_STOP         (OM_SIZE - 1)     /* G/R p.658 */

WORD om[OM_SIZE];



LOC otLoc[OT_SIZE];
BYTE otRest[OT_SIZE];   	/* bitmap, free, ptr and odd bits */
BYTE otCount[OT_SIZE];		/* reference counts */
Size_fld Sizes[OT_SIZE];        /* sizes of the object bodies */
OOP Classes[OT_SIZE];           /* classes of the objects */


OOP     FCLhd[BIG_SIZE + 1];        /* initialise to NON_PTR */

				 /*ST!ObjectMemory!headOfFreePointerList!*/
				 /*ST!ObjectMemory!headOfFreePointerListPut:!*/
OOP     free_ptr;

BOOL lowOnSpace;		 /* have we given a low space notification? */

unsigned oops_left, words_left, ptr_limit, word_limit;
extern OOP limitSema;

#ifdef SIM
extern void trace();
#endif


void OM_init()
{
	extern void load_snapshot();

	load_snapshot();
}




					/*ST!ObjectMemory!lastPointerOf:!*/

/* return the index of the first non-pointer in an object (not including
   the size field of course).  For plain objects this is the first word
   after the end of the object;  for word and byte objects this is the
   word after the class pointer;  for CompiledMethods this is the word
   containing the first bytecode.
*/

static unsigned lastPointer(oop)
OOP oop;
{
	if (ptrs(oop)) {
		return size(oop)/sizeof(WORD);
	} else if (class(oop) == CLASS_COMPILED_METHOD)
		return litCount(oop) + 1;
	else
		return 0;
}



					/*ST!ObjectMemory!countDown:!*/
					/*ST!ObjectMemory!forAllObjectsAccessibleFrom:!*/
					/*ST!ObjectMemory!forAllOtherObjectsAccessibleFrom:!*/

/* c_down_inner(): inner loop of the c_down() macro. */
void c_down_inner(oop)
OOP oop;
{
	unsigned offset, lp= lastPointer(oop);
	OOP next= class(oop);

	countDown(next);
	for (offset= 0;  offset < lp;  ++offset) {
#ifdef SIM	/* would throw off stats otherwise */
		next= loctn(oop)[offset];
#else
		next= HCword(oop, offset);
#endif
		countDown(next);
#ifdef SIM	/* would throw off stats otherwise */
		loctn(oop)[offset]= NIL_PTR;
#else
		HCwordPut(oop, offset, NIL_PTR);
#endif
	}
#ifdef SIM
	trace('d',oop,size(oop));
#endif SIM
	deallocate(oop);
}

					/*ST!ObjectMemory!obtainPointer:location:!*/
					/*ST!ObjectMemory!removeFromFreePointerList!*/

static OOP obtainPtr(Size, loc)	/* Size is in bytes */
WORD Size;
ADDR loc;
{
	OOP oop= free_ptr;

	if (oop == NON_PTR)
		return NON_PTR;         /* no free pointers */

	free_ptr= loctni(oop);
	count(oop)= 0;
	freePut(oop, FALSE);
	ptrsPut(oop, FALSE);
	bitmapPut(oop, FALSE);
		/* probably not very efficient */
	loctn(oop)= loc;
	size(oop)= Size;

	return oop;
}


					/*ST!ObjectMemory!attemptToAllocateChunk:!*/
					/*ST!ObjectMemory!attemptToAllocateChunkInCurrentSegment:!*/

static OOP attemptToAlloc(Size)	/* Size is in BYTEs */
WORD Size;
{
	OOP oop, *pred; /* predecessor in chain */

	for (pred= &FCLhead(BIG_SIZE); (oop= *pred) != NON_PTR;  pred= &class(oop)) {

		WORD avSize= size(oop);

		if (toWords(avSize) == toWords(Size)) {           /* exact fit */
			*pred= class(oop);
			size(oop)= Size;
			return oop;
		} else {
			int excessWords= (int)(toWords(avSize)) - toWords(Size);

			if (excessWords >= 0) {       /* split this object */
				OOP newPtr= obtainPtr((WORD)excessWords * sizeof(WORD),
						       loctn(oop) + toWords(Size));

				/* This section differs from G/R in that the first half
				   of the chunk is used rather than the second.
				   This is an attempt to allocate objects at one end
				   of the object memory. */
				
				if (newPtr == NON_PTR)
					return NON_PTR; /* no free object table entries */
				size(oop)= Size;

				if (excessWords < BIG_SIZE) {
					/* move to appropriate free chunk list */
					*pred= class(oop);
					class(newPtr)= FCLhead(excessWords);
					FCLhead(excessWords)= newPtr;
				} else {
					*pred= newPtr;
					class(newPtr)= class(oop);
				}

				return oop;
			}
		}
	}
	return NON_PTR; /* no suitable entries in chain */
}


extern void newCursor();
extern OOP cursorBits;

					/*ST!ObjectMemory!compactCurrentSegment!*/
void compact()
{
	extern BITMAP CuCompact;
	register ADDR si, di, lowWaterMark= om + HEAP_SPACE_STOP;
	int Size;
	unsigned free_space;
	register OOP oop, next;

	log("compacting");

	newCursor(CuCompact);     /* visual feedback */

	storeCtxRegs;
					/*ST!ObjectMemory!abandonFreeChunksInSegment:!*/
		/* must also reverse the pointers */
	for (Size= 0;  Size <= BIG_SIZE; ++Size) {
		for (oop= FCLhead(Size);  oop != NON_PTR;  oop= next) {
			{ ADDR t= loctn(oop);
			  if (lowWaterMark > t) lowWaterMark= t; };
			if (Size > 0)
				HCwordPut(oop, 0, oop);
			next= class(oop);
					/*ST!ObjectMemory!releasePointer:!*/
			class(oop)= NON_PTR;
			freePut(oop, TRUE);
			toFreeAdd(oop);
		}
					/*ST!ObjectMemory!resetFreeChunkList:inSegment:!*/
		FCLhead(Size)= NON_PTR;
	}

	if (lowWaterMark == om + HEAP_SPACE_STOP)            /* no compaction possible */
		goto done;

	log("Phase 1 complete");


					/*ST!ObjectMemory!reverseHeapPointersAbove:!*/

	/* reverse heap ptrs above low water mark */
	for_every_oop(p) {
		if (!isFree(p) && !bitmap(p) && loctn(p) >= lowWaterMark
		    && size(p) > 0) {
			OOP firstword= HCword(p, 0);
			HCwordPut(p, 0, p);
			loctni(p)= firstword;
		}
	}}

	log("Phase 2 complete");


					/*ST!ObjectMemory!sweepCurrentSegmentFrom:!*/
	/* sweep from low water mark */
	for (si= di= lowWaterMark; si < om + HEAP_SPACE_STOP; ) {
		oop= *si++;
		if (class(oop) == NON_PTR) {   /* this is a free chunk */
			si += (toWords(size(oop)) - 1);
		} else {
			WORD ASize, firstword;
			register int i;

			firstword= loctni(oop);
			loctn(oop)= di;
			*di++= firstword;
			
			ASize= toWords(size(oop));
			for (i= 1;  i < ASize; ++i)
				*di++= *si++;
		 }
	}

	free_space= HEAP_SPACE_STOP + 1 - (di - om);

	if (free_space != words_left)
		(void)fprintf(stderr,
			      "Word accounting error?: words_left=%d, free_space=%d\n",
			      words_left, free_space);

	words_left= 0;

	deallocate(obtainPtr(free_space*sizeof(WORD), di));
	free_space= 0;
	
    done:
	cacheActiveContext;  fetchCtxRegs;  /* because objects may have moved */

	/* restore previous cursor */
	newCursor(loctnb(cursorBits));

	(void)fprintf(stderr, "compaction complete, %d words left\n",
		      words_left);
}


#define unmark(oop)     { count(oop)= 0; }
#define mark(oop)       { count(oop)= 2; }
#define unmarked(oop)   (count(oop) == 0)

					/*ST!ObjectMemory!zeroReferenceCounts!*/
static void zeroRefCounts()
{
	for_every_oop(oop) {
		unmark(oop);
	}}
}


					/*ST!ObjectMemory!forAllOtherObjectsAccessibleFromSuchThat:do:!*/
static void markRest(root)
OOP root;
{
	WORD offset;
	WORD lp= lastPointer(root);
	OOP next= class(root);

	if (unmarked(next)) {
		mark(next); markRest(next);
	}

	for (offset= 0;  offset < lp;  ++offset) {
		next= HCword(root, offset);
		if (!isInt(next) && unmarked(next)) {
			mark(next);
			markRest(next);
		}
	}
}



					/*ST!ObjectMemory!markObjectsAccessibleFrom:!*/
					/*ST!ObjectMemory!forAllObjectsAccessibleFromsuchThat:do:!*/
static void markFrom(root)
OOP root;
{
	(void)fprintf(stderr, "marking from %04x\n", root);

	if (unmarked(root)) {
		mark(root);
		markRest(root);
	}
}


					/*ST!ObjectMemory!markAccessibleObjects!*/
static void markObjects()
{
		/* ENSURE that you also alter rectify() if you alter this */
	markFrom(SMALLTALK);
	markFrom(activeContext);
}


					/*ST!ObjectMemory!rectifyCountsAndDeallocateGarbage!*/
static void rectify()
{
	log("rectifying counts"); 

	{       /* reset heads of free chunk lists */
		WORD Size;

		for (Size= 0;  Size <= BIG_SIZE; ++Size)
			FCLhead(Size)= NON_PTR;
	}

	words_left= 0;

	count(UNUSED_PTR)= 1;	/* claim oop 0 is used */

	for_every_oop(oop) {
		if (!isFree(oop)) {
			BYTE Count;

			if ((Count= count(oop)) == 0) {        /* unmarked */
				deallocate(oop);
			} else {
				register WORD offset;
				register WORD lp= lastPointer(oop);

				count(oop)= Count - 2;
				countUp(class(oop));
				for (offset= 0;  offset < lp; ++offset)
					countUp(HCword(oop, offset));
			}
		}
	}}

	{       /* make sure roots don't disappear */
		OOP i;
		extern OOP ActiveProcess();

		countUp(SMALLTALK);
		countUp(activeContext);

		for (i= NIL_PTR; i <= LAST_REF_COUNTED_OOP; ++i) {
			/* these are guaranteed to stay */
			count(i)= SATURATED;
		}
		countUp(ActiveProcess());
	}
}

					/*ST!ObjectMemory!reclaimInaccessibleObjects!*/
void reclaim()
{
	extern BITMAP CuGarbage;
	(void)fprintf(stderr, "collecting garbage, %d words free\n",
		      words_left);

	newCursor(CuGarbage);     /* visual feedback */

	zeroRefCounts();
	markObjects();
	rectify();

	/* restore previous cursor */
	newCursor(loctnb(cursorBits));

	(void)fprintf(stderr, "collection complete, %d words left\n",
		      words_left);
}



					/*ST!ObjectMemory!allocate:odd:pointer:extra:class:!*/
					/*ST!ObjectMemory!allocateChunk:!*/
OOP allocate(Size, Ptrs, Class)	/* Size is in BYTEs */
WORD Size;
BOOL Ptrs;
OOP Class;
{
	OOP oop;
	WORD WSize= toWords(Size);

	c_up(Class);

					/*ST!ObjectMemory!removeFromFreeChunkList:!*/
	if (WSize < BIG_SIZE) {
		oop= FCLhead(WSize);
		if (oop != NON_PTR) {
			FCLhead(WSize)= class(oop);
			goto init;
		}
	}

	/* look at "big" objects' free list */
	oop= attemptToAlloc(Size);

	if (oop == NON_PTR) {           /* no success - try compaction */
		compact();
		oop= attemptToAlloc(Size);
		if (oop == NON_PTR) {   /* try garbage collection */
			reclaim();
			oop= attemptToAlloc(Size);
			if (oop == NON_PTR) {   /* and another compaction */
				compact();
				oop= attemptToAlloc(Size);
				if (oop == NON_PTR) {
					/* yes, we have no bananas */
					static void errorNoMem();
					errorNoMem();
				}
			}
		}
	}

    init: /* initialise the object */
	otAttribsPut(oop, Ptrs ? PTRS : 0);
	class(oop)= Class;
	size(oop)= Size;

#ifdef SIM
	trace('c',oop,Size);
	trace('k',oop,Class);
#endif SIM
	/* place nil/0 in newly allocated object */
	{
		register WORD defltEntry= Ptrs ? NIL_PTR : 0;
		register WORD i;

		for (i= 0;  i < WSize;  ++i)
			HCwordPut(oop, i, defltEntry);
	}

	if (((words_left -= WSize) <= word_limit  ||  --oops_left < ptr_limit)
	    &&  !lowOnSpace  &&  limitSema != NIL_PTR) {

		log("Low space notifier would have been activated");
		(void)fprintf(stderr, "bc=%d, words_left=%d, oops_left=%d\n",
			bc_count, words_left, oops_left);
		(void)fprintf(stderr, "word_limit=%d, ptr_limit=%d\n",
			word_limit, ptr_limit);
		word_limit /= 4;
		ptr_limit /= 4;
/*		asynchSignal(limitSema);        /* running low */

		lowOnSpace= TRUE;
	}

	return oop;
}



					/*ST!ObjectMemory!initialInstanceOf:!*/

OOP initialInstance(Class)
OOP Class;
{
	for_every_oop(p) {
		if (!isFree(p) && class(p) == Class)
			return p;
	}}
	return NIL_PTR;
}


					/*ST!ObjectMemory!instanceAfter:!*/
OOP instanceAfter(oop)
OOP oop;
{
	OOP p;
	OOP Class= class(oop);

	/* BEWARE if you change the oop representation */
	for (p= oop+1;  p <= LAST_OOP;  ++p)
		if (!isFree(p) && class(p) == Class)
			return p;
	return NIL_PTR;
}


					/*ST!ObjectMemory!swapPointersOf:and:!*/

void swapPtrs(a, b)
OOP a, b;
{
	ADDR loc= loctn(a);
	BOOL Ptrs= ptrs(a),
	     BitMap= bitmap(a);
	WORD Size= size(a);
	OOP  Class= class(a);
	loctn(a)= loctn(b);
	size(a)= size(b);
	class(a)= class(b);
	ptrsPut(a, ptrs(b));
	bitmapPut(a, bitmap(b));
	loctn(b)= loc;
	size(b)= Size;
	class(b)= Class;
	ptrsPut(b, Ptrs);
	bitmapPut(b, BitMap);
}


static void errorNoMem()
{
	fatal("Out of object or bitmap memory or pointers ... SPLAT!!\n");
}

BITMAP ConvBitMap(bm, w, h)	/* cache bm as a bitmap */
OOP bm;
WORD w, h;
{
	extern void move_to_FCL();
	extern BITMAP convert_bm();
	BITMAP bm_area;

	move_to_FCL(bm);
	bm_area= convert_bm(bm, w, h);
	loctnb(bm)= bm_area;
	bitmapPut(bm, TRUE);
	return bm_area;
}

void move_to_FCL(oop)	/* move the object body of oop onto a free chunk list */
OOP oop;
{
	extern void errorNoMem();
		/* grab a free pointer */
	OOP new_oop= obtainPtr(size(oop), loctn(oop));
		/* we're in big trouble here if there are no oops free... */
	if (new_oop == NON_PTR)
		errorNoMem();
	deallocate(new_oop);	/* move new oop and old area onto free chunk list */
}

/* Routines for looking at bitmaps.
   These never seem to be invoked ... good job, really, as they aren't
   properly implemented for SunView and X.
   What should happen is that they should call the appropriate routine to
   inspect a collection of pixels in the bitmap. */
WORD fetchBMWord(i, oop)
WORD i;
OOP oop;
{
	(void)fprintf(stderr, "fetchBMword(%d,%04x) at %d\n",
		      i, oop, bc_count);
	
	if (bitmap(oop)) {
		return HCword(oop, i);
	} else
		return fetchWord(i, oop);
}


void storeBMWord(i, oop, value)
OOP oop;
WORD i, value;
{
	(void)fprintf(stderr, "storeBMword(%d,%04x,%d) at %d\n",
		i, oop, value, bc_count);

	if (bitmap(oop)) {
		HCwordPut(oop, i, value);
	} else {
		storeWord(i, oop, value);
	}
}

#ifdef SIM
/* definitions of memory-accessing routines for simulations */

WORD HCword(oop, offset)
OOP oop;
WORD offset;
{
	trace('r',oop,offset);
	return loctn(oop)[offset];
}

void _HCwordPut(oop, offset, val)
OOP oop;
WORD offset;
WORD val;
{
	trace('w',oop,offset);
	loctn(oop)[offset]= val;
}

BYTE HCbyte(oop, offset)
OOP oop;
WORD offset;
{
	trace('r',oop,offset/sizeof(WORD));
	return ((BYTE *)loctn(oop))[offset];
}

void _HCbytePut(oop, boffset, bval)
OOP oop;
WORD boffset;
BYTE bval;
{
	trace('w',oop,boffset/sizeof(WORD));
	((BYTE *)loctn(oop))[boffset]= bval;
}


/*
   #define HCword(oop, offset)             (loctn(oop)[offset])
   #define HCwordPut(oop, offset, val)     { HCword(oop, offset)= (val); }
   #define HCbyte(oop, boffset)            (((BYTE *)loctn(oop))[boffset])
   #define HCbytePut(oop, boffset, bval)   { HCbyte(oop, boffset)= (bval); }
*/

void trace(type,oop,offset)
char type;
OOP oop;
WORD offset;
{
	extern char *trace_opts, *index();

	if (type != 'k'
	    && type != 'f'
	    && type != 'X'
	    && type != 'g'
	    && type != 'i'
	    && class(oop)==CLASS_METH_CTX) {
		type= toupper(type);
	}

	if (index(trace_opts, type) == NULL)
		return;

	(void)putchar(type);
	(void)putw((int)oop, stdout);
	(void)putw((int)offset, stdout);
}

/* routines to do static sweep, for simulations */

#define SMARK	1
#define smarked(oop)		(otRest[(oop)]&SMARK)
#define smark(oop)		{ otRest[(oop)] |= SMARK; }
#define sunmark(oop)		{ otRest[(oop)] &= ~SMARK; }

static unsigned trace_depth;

static unsigned slastPointer(oop) /* special version that avoids HCword() */
OOP oop;
{
	if (ptrs(oop)) {
		return size(oop)/sizeof(WORD);
	} else if (class(oop) == CLASS_COMPILED_METHOD)
		return (loctn(oop)[0] & 0x3F) + 1;
	else
		return 0;
}

static void smarkRest(root)		/* breadth first marking */
OOP root;
{
	WORD offset;
	WORD lp= slastPointer(root);
	OOP next= class(root);
	trace('g',root,trace_depth++);
	if (isFree(root)) {
		(void)fprintf(stderr, "Dangling oop %d found\n", root);
		return;
	}

	if (!smarked(next)) {
		trace('i',next,size(next));
		trace('k',next,class(next));
		smark(next);
		smarkRest(next);
	}

	for (offset= 0;  offset < lp;  ++offset) {
		next= loctn(root)[offset]; /* mustn't invoke HCword() */
		if (!isInt(next) && !smarked(next)) {
			trace('i',next,size(next));
			trace('k',next,class(next));
			smark(next);
			smarkRest(next);
		}
	}
	--trace_depth;
}

static void smarkFrom(root)
OOP root;
{
	trace_depth=0;
	if (!smarked(root)) {
		trace('i',root,size(root));
		trace('k',root,class(root));
		smark(root);
		smarkRest(root);
	}
}

void do_static_grouping()
{
	for_every_oop(p) {
		sunmark(p);
	}}
	smarkFrom(activeContext);
	smarkFrom(SMALLTALK);
	trace('f',0,0);
}
#endif SIM
