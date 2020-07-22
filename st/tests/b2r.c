
#include <stdio.h>
#include <string.h>
main(argc,argv)
int argc;
char *argv[];
{
FILE *fi,*fo,*fopen(); /* fv points to vector */
int i,count;
int type,oop,obSize,limit,ot[64*1024],offset,data;
/*================================*/
count=0;
  fi=stdin; 
  fo=stdout;
 while(	(type=getc(fi)) != EOF){
	++count;
	switch(type){
		case 'f': fprintf(fo," The end of Initialization \n");
		case 'i':
		case 'I':
		case 'r':
		case 'R':
			getw(fi);
			getw(fi);
			break;
		case 'c':
		case 'n':
		case 'C':
		case 'd':
		case 'D':
			
			oop=getw(fi);
			obSize=getw(fi);
			break;
		case 'W':
		case 'w':
			oop=getw(fi);
			offset=getw(fi);
			data=getw(fi);  
			break;

		default:  fprintf(stderr,"illegle character %c (0x%x)!!\n",type,type);
			  for(i=0;i<10;++i) {
			  fprintf(stderr,"more character %c(0x%x)!!\n",type,type);
                          }
			printf("count=%d\n",count);
			exit(); 
		break;
               }
}
printf("count=%d\n",count);
	fclose(fo);
}/*main */

