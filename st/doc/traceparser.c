#include <stdio.h>
#include <assert.h>

char get_c(opt)
int opt;
{
  char c = getc(stdin);
  if (opt == 'a') (void) putc(c,stderr);    /* trace */
  return(c);
}

long get_w(opt)
int opt;
{
  /* note coercion of long onto int */
  int w = getw(stdin);
  if (opt == 'a') (void) putw(w,stderr);    /* trace */
  return(w);
}

void abort_with(s)
char *s;
{
	(void) fprintf(stderr, "%s\n", s);
	exit(1);
}


main(argc, argv)
     int argc;
     char **argv;
{
    extern int optind;	       /* used by getopt() */
    extern char *optarg;	/* used by getopt() */

    unsigned char bytecode;
    unsigned char taken;
    unsigned char arg_count;
    unsigned char tmp_count;


    int operation;
    int i;
    int c;
    int opt;

    long process;
    long ip;
    long destination;
    long class;
    long selector;
    long method;
    long oop;
    long offset;
    long context;
    long receiver;
    long o_receiver;


    opt=' '; /* init opt to empty */

    /* get command line options */
    while ((c= getopt(argc, argv, "a")) != EOF) 
      {
	switch (c) 
	  {
	  case 'a':
	    /* switch tracing on */
	    opt = c;
	    break;
	  default: 
	    abort_with("Simulation:error parsing command line\n");
	  }
      }

    if (opt == 'a')
      {
	/* if tracing option sepcified */
	/* separate crap from simulator stderr from this op */

	for(i=0;i<100;i++) (void) fprintf(stderr," ");
      }

    while ((operation=get_c(opt)) != EOF)
      {
	switch(operation)
	  {
	  case 'i':
	  case 'I':
	  case 'f':
	  case 'c':
	  case 'C':
	  case 'd':
	  case 'D':
	  case 'r':
	  case 'R':
	  case 'w':
	  case 'W':
	  case 'x':
	  case 'X':
	  case 'k':
	  case 'l':
	  case 'L':
	  case 'g':
	    oop=get_w(opt);
	    offset=get_w(opt);
	    (void) printf("%c %d %d\n",operation,oop,offset);
	    break;
	  case 'j':
	    bytecode=get_c(opt);
	    taken=get_c(opt);
	    ip=get_w(opt);
	    offset=get_w(opt);
	    destination=get_w(opt);
	    (void) printf("%c\t%d\t%d\t%d\t%d\t%d\n",operation,256+bytecode,taken,ip,offset,destination);
	    break;    
	  case 'm':
	    bytecode = get_c(opt);
	    class = get_w(opt);
	    selector = get_w(opt);
	    method = get_w(opt);
	    context = get_w(opt);
	    offset = get_w(opt);
	    receiver = get_w(opt);
	    o_receiver = get_w(opt);
	    arg_count = get_c(opt);
	    tmp_count = get_c(opt);
	    
	    (void) printf("%c\t%d\t%d\t%d\t",operation,bytecode,class,selector);
	    (void) printf("%d\t%d\t%d\t%d\t%d\t",method,context,offset,receiver,o_receiver);
	    (void) printf("%d\t%d\n",arg_count,tmp_count);
	    break;
	  case 'a':
	    arg_count = get_c(opt);
	    o_receiver = get_w(opt); /* class */
	    class = get_w(opt);
	    (void) printf("%c\t%d\t%d\t%d\t",operation,arg_count,o_receiver,class);
    	    { int n=arg_count;
	      while (n--) 
		(void) printf("%d\t",get_w(opt));
      	    }
	    putchar('\n');
	    break;
	  case 'b':
	    method = get_w(opt);		
	    context = get_w(opt);		
	    offset = get_w(opt);				
	    receiver = get_w(opt);		
	    arg_count = get_c(opt);		
	    
	    (void) printf("%c\t%d\t%d\t%d\t",operation,method,context,offset);
	    (void) printf("%d\t%d\n",receiver,arg_count);
	    break;
	  case 'M':
	    bytecode=get_c(opt);
	    method=get_w(opt);
	    context=get_w(opt);
	    offset=get_w(opt);
	    (void) printf("%c\t%d\t%d\t%d\t\n",operation,bytecode,method,context,offset);
	    break;
	  case 'p': 
	    process=get_w(opt);
	    context=get_w(opt);
	    (void) printf("%c\t%d\t%d\n",operation,process,context);
	    break;
	  case 'B': 
	    receiver=get_w(opt);
	    class = get_w(opt);
	    (void) printf("%c\t%d\t%d\n",operation,receiver,class);
	    break;
	  default:  
	    (void) fprintf(stdout,"Traceparser:error parsing input file\n");
	    if (opt == 'a')
	      {
		/* require error tracing */
		
		/* separate by blanks at point of error */
		for(i=0;i<100;i++) (void) fprintf(stderr," ");

		/* and copy rest of input to stderrr */
		while ((operation=get_c(opt)) != EOF);
		{
		  fprintf(stderr,"%c",operation);
		}
	      }
	    exit(1);
	    break;
	  }
  }
}

  
  
  
  
