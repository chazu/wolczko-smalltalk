/*
  	Smalltalk-80 Virtual Machine:
	Snapshot        (Manchester format)

	Copyright (c) 1984,1985,1986,1987,1988,1989 Mario Wolczko
	All rights reserved.

$Log: Snapshot.c,v $
 * Revision 5.0  1994/08/16  01:00:16  mario
 * Sync with others
 *
 * Revision 4.1  89/06/22  11:42:19  mario
 * Moved machine-dependent code (mainly to do with bitmaps) to IO modules.
 * Added lots of casts and declarations to keep lint quiet.
 * 
 * Revision 3.3  89/05/03  15:56:24  mario
 * Added work-around for curious display RC bug.
 * 
 * Revision 3.2  87/03/21  14:47:26  miw
 * Added simulation code.
 * Added ability to read different images, trace files and scripts.
 * 
 * Revision 3.1  87/03/11  16:48:47  miw
 * Removed support for Orion and Perq.  Changed to rectangle() to malloc().
 * Changed the link/unlink stuff to use rename().
 * 
 * Revision 2.9  87/02/13  20:11:03  miw
 * Changed to 32-bit:
 * - Defined a new format, MANCHESTER2, type code 43.
 * - An image can now have an object table less than OT_SIZE in length.
 * - Because the size field is now 32 bits, no longer need to create a lot of
 * 	free chunks at startup; 1 will do.
 * 
 * Revision 2.8  87/02/07  17:41:48  miw
 * Added code for SUN snapshot (using pixrect routines).
 * 
 * Revision 2.7  86/01/18  16:27:52  miw
 * Fixed test of fseek: VAX now returns non-zero values when it succeeds.
 * 
 * Revision 2.6  86/01/17  21:19:45  miw
 * Changed 0 to SCREEN.
 * 
 * Revision 2.5  86/01/17  18:51:04  miw
 * Added an fflush(stderr).
 * 
 * Revision 2.4  85/12/07  15:33:28  miw
 * Modified the oop format so the tag bit is the most significant.
 * Consequently had to alter indexing of arrays.  Also separated the ref.
 * count field away from the otRest[] array.
 * 
 * Revision 2.3  85/12/04  20:26:01  miw
 * Removed all references to the display object.
 * Added the bitmap space to the snapshot format, and extended load_snapshot()
 * and save_snapshot() to deal with cached bitmaps.
 * 
 * Revision 2.2  85/11/23  17:03:09  miw
 * Now reads and writes a new format known as "Manchester format".
 * Should be much faster to read and write.
 * The primary difference is that the size and class fields of objects
 * are held in the object table.
 * 
 * Revision 2.1  85/11/19  17:42:26  miw
 * Stabilised with all other modules at Rev 2.1
 * 
 * Revision 1.12  85/11/18  20:02:17  miw
 * Added more trace info o/p.
 * 
 * Revision 1.11  85/08/07  08:29:07  miw
 * Added loctni() macro for O.T. free chain
 * 
 * Revision 1.10  85/08/05  11:33:58  miw
 * Added temporary variable in initialisation of free chain to get round
 * PERQ bug.
 * 
 * Revision 1.9  85/07/23  21:27:24  miw
 * Made loctn field of object table point directly into object memory.
 * 
 * Revision 1.8  85/07/05  13:11:43  miw
 * Changed the ref.counting scheme to put the overflow bit in the l.s.b.
 * Now initialise the display object here (as opposed to in OM.c).
 * 
 * Revision 1.7  85/06/27  19:07:52  miw
 * Altered macro calls to take into account new split in object table.
 * Removed the system() call to /bin/mv, and replaced it with link/unlink
 * calls.
 * 
 * Revision 1.6  85/04/23  16:43:59  miw
 * Removed all refces. to fputc().  Altered code of save_snapshot() (when
 * writing OT)--suspect a bug in the Apollo C compiler.
 * 
 * Revision 1.5  85/04/03  11:22:26  miw
 * Added stricter checking on image to be loaded.
 * 
 * Revision 1.4  85/04/02  18:34:42  miw
 * Fixed writing out of object table (previously too short, and didn't
 * write free entries correctly).
 * 
 * Revision 1.3  85/04/02  11:31:16  miw
 * Added save_snapshot().
 * 
 * Revision 1.2  85/01/14  17:57:58  miw
 * 
 * Revision 1.1  85/01/10  12:57:08  miw
 * Initial revision
 * 

*/

static char *rcsid= "$Header: /home/mario/st/RCS/Snapshot.c,v 5.0 1994/08/16 01:00:16 mario Exp mario $";

#include <stdio.h>
#include <string.h>
#include "ST.h"
#include "OM.h"
#include "OMMacros.h"
#include "ClassPtrs.h"
#include "StdPtrs.h"

#define MANCHESTER2      43

extern OOP free_ptr;

WORD16 image_type;

extern long ftell();
extern char *malloc();
extern BOOL s_group;
char *image_file_name= "st80.image";

#define EXT ".bak"


void load_snapshot()
{
	extern long ftell();
	extern long get32();
#ifdef SIM
	extern void trace();
#endif
	int obj_sp_len;            /* size of the object space */
	int obj_tab_len;           /* size of the object table */
	FILE *image;
	long ot_offset;

	image= fopen(image_file_name, "r");
	if (image == NULL)
		fatal("Cannot access image!");

	obj_sp_len= (int)get32(image);

	(void)fprintf(stderr, "%d words in the object space\n", obj_sp_len);

	if (obj_sp_len > OM_SIZE)
		fatal("Image is too big for memory!");

	obj_tab_len= (int)get32(image);

	(void)fread((char *)&image_type, sizeof(WORD16), 1, image);
	if (image_type != MANCHESTER2)
		fatal("Image not in Manchester2 format!");

	if (obj_tab_len > OT_SIZE)
		fatal("Saved object table is too big!");

	(void)fseek(image, 512L, 0);

	/* read object space */
	{
		logs("Loading object space..."); 
		(void)fread((char *)om, sizeof(WORD), obj_sp_len, image);
		loge("done."); 
	}

	ot_offset= (ftell(image) + 511) & ~511;
	(void)fprintf(stderr, "Object table begins at offset %d\n", ot_offset);
	if (fseek(image, ot_offset, 0) == -1)
		fatal("Can't seek to object table!");
	
	{       /* read object table */
		unsigned *offsets;

		for_every_oop(oop) {
			freePut(oop, TRUE);
		}}

		logs("Loading object table..."); 

		offsets= (unsigned *)malloc(OT_SIZE*sizeof(unsigned));
		if (offsets == NULL)
			fatal("Can't allocate temporary space!");
		(void)fread((char *)offsets, sizeof(unsigned),
			    obj_tab_len, image);
		(void)fread((char *)otCount, sizeof(BYTE),
			    obj_tab_len, image);
		(void)fread((char *)otRest, sizeof(BYTE),
			    obj_tab_len, image);
		/* BEWARE if you change the oop representation */
		for_every_oop(oop) {
			if (!isFree(oop)) {
				if (bitmap(oop)) {
					loctni(oop)= offsets[oop];
				} else {
					loctn(oop)= om + offsets[oop];
				}
			}
		}}
		free((char *)offsets);
		(void)fread((char *)Sizes, sizeof(WORD),
			    obj_tab_len, image);
		(void)fread((char *)Classes,sizeof(OOP),
			    obj_tab_len, image);

		loge("done."); 
	}

	{	/* load bitmaps */
  		extern void load_bitmaps();
		logs("Loading bitmaps: ");
		load_bitmaps(image);
		loge("done."); 
	}

	{       /* link free entries in object table */
		/* BEWARE if you change the oop representation */
		OOP oop= LAST_OOP+1;

		logs("Linking free chain..."); 

		free_ptr= NON_PTR;
		do {    /* link in reverse order */
			/* BEWARE if you change the representation of oops */
			--oop;
			if (isFree(oop)) {
				++oops_left;
				toFreeAdd(oop);
#ifdef SIM
			} else {
 				if (!s_group && oop != UNUSED_PTR) {
					trace('i',oop,size(oop));
					trace('k',oop,class(oop));
				}
#endif SIM
			}
		} while (oop > FIRST_OOP);
		loge("done."); 
		(void)fprintf(stderr, "%d free pointers\n",  oops_left);
	}
#ifdef SIM
	if (!s_group)
		trace('f',0,0);
#endif SIM

	{       /* initialise free chunk lists */

		int i;
		OOP fcp; /* free chunk pointer */
		ADDR loc= om + obj_sp_len;  /* location of free chunk */
		unsigned free_space= (words_left= OM_SIZE - (loc - om));

		for (i= 0;  i < BIG_SIZE;  ++i)
			FCLhead(i)= NON_PTR;

		/* find a free pointer */
		FCLhead(BIG_SIZE)= fcp= free_ptr;
		free_ptr= loctni(fcp);
		/* create a new free chunk */
		loctn(fcp)= loc;
		freePut(fcp, FALSE);
		ptrsPut(fcp, FALSE);
		size(fcp)= free_space*sizeof(WORD);
		class(fcp)= NON_PTR;
		free_space= 0;

	}
	(void)fclose(image);

	loge("Snapshot loaded."); 
}


long get32(f)
FILE *f;
{
	unsigned n;

	n= getc(f)&0xFF;
	n= (n<<8) + (getc(f)&0xFF);
	n= (n<<8) + (getc(f)&0xFF);
	return (n<<8) + (getc(f)&0xFF);
}


BOOL save_snapshot()  /* write out a snapshot, return TRUE if succeeded */
{
	extern void reclaim(), compact(), saveContext();
	static void put32();
	FILE *image;
	int obj_sp_len, obj_tab_len;
	long ot_offset;
	unsigned *object_table_locs;
	OOP oop;

	(void)fprintf(stderr,
		      "Snapshot invoked after %d bytecodes executed\n",
		      bc_count);

	saveContext();
	reclaim();      /* garbage collect... */
	compact();      /* ...and compact */

	logs("Writing snapshot...");

	image= fopen("st80.image.new", "w");
	if (image == NULL)
		return FALSE;

	obj_sp_len= OM_SIZE - words_left;
	(void)fprintf(stderr, "%d words in the object space\n", obj_sp_len);

	put32((unsigned)obj_sp_len, image);

	/* find the last non-free oop */
	for (oop= LAST_OOP; isFree(oop); --oop)
	  ;
	obj_tab_len= oop + 1;	/* BEWARE if you change the oop format */
	(void)fprintf(stderr, "Object table has %d entries\n", obj_tab_len);
	put32((unsigned)obj_tab_len, image);
	
	image_type= MANCHESTER2;
	(void)fwrite((char *)&image_type, sizeof(WORD16), 1, image);

	(void)fseek(image, 512L, 0);

	{       /* write out object space */
		(void)fwrite((char *)om, sizeof(WORD), obj_sp_len, image);
	}

	{	/* write out bitmaps */
		extern BOOL save_bitmaps();
		ot_offset= (ftell(image)+511) & ~511;
		(void)fseek(image, ot_offset, 0);
		
		(void)fseek(image, (long)obj_tab_len * (sizeof(LOC)
		      + sizeof(BYTE) + sizeof(BYTE) + sizeof(WORD)
		      + sizeof(OOP)), 1);	/* jump over object table */
		
		(void)fprintf(stderr, "bitmaps begin at offset %ld\n",
			      ftell(image));
		/* make copy of object table locations */
		object_table_locs=
		  (unsigned *)malloc((unsigned)obj_tab_len*sizeof(unsigned));
		if (object_table_locs == NULL) {
			log("Could not allocate temporary space");
			(void)fclose(image);
			return FALSE;
		}

		if (save_bitmaps(image,object_table_locs))
			return FALSE;
		(void)fprintf(stderr,
			      "bitmaps end at offset %ld\n", ftell(image));
	}
	
	{       /* write out object table */


		(void)fseek(image, ot_offset, 0);
		(void)fprintf(stderr,
			      "object table begins at offset %ld\n",
			      ot_offset);
		(void)fwrite((char *)object_table_locs, sizeof(unsigned),
			     obj_tab_len, image);

		(void)fwrite((char *)otCount, sizeof(BYTE), obj_tab_len,
			     image);

		/* mark the single remaining free chunk pointer as free... */
		freePut(FCLhead(BIG_SIZE), TRUE);
		(void)fwrite((char *)otRest, sizeof(BYTE), obj_tab_len, image);
		/* ...and clear the free bit */
		freePut(FCLhead(BIG_SIZE), FALSE);

		(void)fwrite((char *)Sizes, sizeof(WORD), obj_tab_len, image);
		(void)fwrite((char *)Classes, sizeof(OOP), obj_tab_len, image);
		
		free((char *)object_table_locs);
	}

	(void)fclose(image);

	{	/* rename the old image to something safe, rename
		   the new to be the old */
  		char *bak= malloc((unsigned)
				   (strlen(image_file_name)+strlen(EXT)+1));
		(void)strcpy(bak, image_file_name);
		(void)strcat(bak, EXT);
		if (rename(image_file_name, bak) == 0
		    && rename("st80.image.new", image_file_name) == 0) {
			(void)fprintf(stderr,
				      "%s renamed to %s, new %s created\n",
				      image_file_name, bak, image_file_name);
		} else {
			log("Rename failed!");
			return FALSE;
		}
		free(bak);
	}
	loge("Snapshot written.");

	return TRUE;
}



static void put32(w, f)
unsigned w;
FILE *f;
{
	(void)putc((char)(w>>24), f);
	(void)putc((char)(w>>16), f);
	(void)putc((char)(w>>8), f);
	(void)putc((char)(w&255), f);
}

