/*
  	Smalltalk-80 Virtual Machine:
	Class and Method Lookup (for debugging)

	Copyright (c) 1984,1985,1986,1987,1988,1989 Mario Wolczko
	All rights reserved.

This module is used to translate from the oop of a method/class to its
name.  It works in conjunction with the files method_index and
method_table, class_index and class_table.  It only works for methods
and classes which are in these tables.

$Header: /home/mario/st/RCS/MethodLookup.c,v 5.0 1994/08/16 00:58:59 mario Exp mario $
$Log: MethodLookup.c,v $
 * Revision 5.0  1994/08/16  00:58:59  mario
 * Sync with others
 *
 * Revision 4.1  89/06/21  19:54:08  mario
 * Stabilised with other files at 4.1.
 * 
 * 
 * Revision 3.1  87/03/11  15:24:44  miw
 * Stabilised with other modules.
 * 
 * Revision 2.2  86/01/23  22:01:24  miw
 * Added lookup for classes as well as methods.
 * 
 * Revision 2.1  85/11/19  17:40:46  miw
 * Stabilised with all other modules at Rev 2.1
 * 
 * Revision 1.3  85/06/27  19:01:38  miw
 * Made the module into an include file.
 * 
 * Revision 1.2  85/04/23  16:46:57  miw
 * Corrected setup of offsets[]...didn't work on Apollo due to use of
 * (int *) where (long *) was required.  Removed calss to fputc().
 * 
 * Revision 1.1  85/04/21  20:09:55  miw
 * Initial revision
 * 
*/


#define NO_METHS 4500
#define NO_CLASSES 450

static OOP meths[NO_METHS];
static long meth_offsets[NO_METHS];
static OOP classes[NO_CLASSES];
static long class_offsets[NO_CLASSES];

static FILE *mi,*mt, *ci, *ct;

static int no_meths, no_classes;

static void print_str();
static long find_str();

int method_lookup_init()      /* initialise the internal state */
{
	int i, j;
	long k;
	
	if ((mi= fopen("method_index", "r")) == NULL)
		fatal("Can't open method_index!");
	if ((mt= fopen("method_table","r")) == NULL)
		fatal("Can't open method_table!");
	if ((ci= fopen("class_index", "r")) == NULL)
		fatal("Can't open class_index!");
	if ((ct= fopen("class_table","r")) == NULL)
		fatal("Can't open class_table!");
		
	for (i= 0; fscanf(ci, "%x%d", &j, &k)==2; ++i) {
		classes[i]= j;
		class_offsets[i]= k;
	}
	no_classes= i;  (void)fclose(ci);
	for (i= 0; fscanf(mi, "%x%d", &j, &k)==2; ++i) {
		meths[i]= j;
		meth_offsets[i]= k;
	}
	(void)fclose(mi);  no_meths= i;
	return no_meths;
}

long find_meth(m)       /* return the offset of the name of method m */
OOP m;                  /* return -1 if the method cannot be found */
{
	return find_str(m, meths, meth_offsets, no_meths);
}

long find_class(c)      /* return the offset of the name of class c */
OOP c;                  /* return -1 if the class cannot be found */
{
	return find_str(c, classes, class_offsets, no_classes);
}

void print_meth_name(m, f)   /* print the name of method m on file f */
OOP m;
FILE *f;
{
	print_str(m, f, find_meth, mt);
}

void print_class_name(c, f)   /* print the name of class c on file f */
OOP c;
FILE *f;
{
	print_str(c, f, find_class, ct);
}

static long find_str(oop, keys, values, table_size)
				 /* return values[p] such that keys[p]==oop */
OOP oop;	                 /* return -1 if the method cannot be found */
OOP keys[];
long values[];
int table_size;
{
	int lo= 0, hi= table_size;
	int mid= (lo + hi)/2;
	
	while (keys[mid] != oop) {
		if (lo == hi) return -1;        /* not found */
		if (keys[mid] < oop)
			lo= mid+1;
		else
			hi= mid;
		mid= (lo + hi)/2;
	}
	return values[mid];
}

static void print_str(oop, f, search_f, table)   /* print the name of oop on file f */
				/* using search function search_f and table */
OOP oop;
FILE *f, *table;
long (*search_f)();
{
	long offset= (*search_f)(oop);
	
	if (offset == -1)
		(void)fprintf(f, "No such pre-defined object");
	else {
		int c;
		(void)fseek(table, offset, 0);
		while ((c= fgetc(table)) != '\n')
			(void)putc(c, f);
	}
}
