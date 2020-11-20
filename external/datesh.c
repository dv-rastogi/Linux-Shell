#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {

	// calc time
	time_t rawtime;
	struct tm * ptm;
	time (&rawtime); // Get local time in time_t
	ptm = gmtime (&rawtime); // Find out UTC time
	time_t utctime = mktime(ptm); // Get UTC time as time_t

	char buf[256];
	memset(buf, 0, sizeof(buf));
	ptm = localtime(&rawtime);
	strftime(buf, 256, "Date in ISO format is %F", ptm); // Get ISO Format

	if(argc > 2) {
		puts("EXTRA OPTIONS");
		exit(EXIT_FAILURE);
	}

	if(argc == 1) { // local format
		printf("Local time %s", ctime(&rawtime));
		exit(EXIT_SUCCESS);
	}
	
	if((int) strlen(argv[1]) != 2 || argv[1][0] != '-') {
		puts("INVALID OPTIONS");
		exit(EXIT_FAILURE);
	}

	if(argv[1][1] == 'u') { // UTC format
		printf("UTC time %s", ctime(&utctime));
		exit(EXIT_SUCCESS);	
	}

	if(argv[1][1] == 'I') { // iso format
		puts(buf);
		exit(EXIT_SUCCESS);	
	}
	
	puts("INVALID OPTIONS");
    exit(EXIT_FAILURE);
}