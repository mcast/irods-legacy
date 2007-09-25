/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* 
 * ienv - The irods print environment utility
 */

#include "rodsClient.h"
#include "parseCommandLine.h"

void usage ();

int
main(int argc, char **argv) {
	int status;
	rodsEnv myEnv;
	rodsArguments_t myRodsArgs;
	char *optStr;
	
	
	optStr = "h";
	
	status = parseCmdLineOpt (argc, argv, optStr, 0, &myRodsArgs);
	
	if (status < 0) {
		printf("Use -h for help\n");
		exit (1);
	}

	if (myRodsArgs.help==True) {
		usage();
		exit(0);
	}

	rodsLogLevel(LOG_NOTICE);

	status = getRodsEnv (&myEnv);
	
	if (status < 0) {
		rodsLogError (LOG_ERROR, status, "main: getRodsEnv error. ");
		exit (1);
	}
	
	exit(0);
}

void
usage () {
   char *msgs[]={
"Usage : ienv [-h]",
"Display current irods environment. Equivalent to iinit -l.",
"Options are:",
" -h  this help",
""};
   int i;
   for (i=0;;i++) {
      if (strlen(msgs[i])==0) return;
      printf("%s\n",msgs[i]);
   }
}


