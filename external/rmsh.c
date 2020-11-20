#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <assert.h>

int main(int argc, char* argv[]) {

	int foundFiles = 0;
	int confirmation = 0;
	int isDirectory = 0;

	for(int i = 1; i < argc; ++ i) {

        assert(argv[i] != NULL);

		if(argv[i][0] == '-') { // check for options

            if((int) strlen(argv[i]) > 1 && argv[i][1] == 'i') { // ask for confirmation
                confirmation = 1;
                continue;
            }
            else if((int) strlen(argv[i]) > 1 && argv[i][1] == 'd') { // remove directory
            	isDirectory = 1;
                continue;
            }
            else {
                puts("INVALID ARGUMENTS");
                exit(EXIT_FAILURE);
            }
        }

        foundFiles = 1;
        if(confirmation) {
        	printf("Delete [%s] ? [1/0]: ", argv[i]);
        	int response;
        	scanf("%d", &response);
        	if(response != 1) {
        		continue;
        	}	
        }

        int status = -1;
        int foundDir = 1;

        if(opendir(argv[i]) == NULL) { // check if not directory
        	foundDir = 0;
        	status = remove(argv[i]);
        }

        if(status == -1) {
        	if(foundDir && isDirectory) {
        		int status = rmdir(argv[i]);
        		if(status == -1) {
        			printf("ERROR OCCURED IN DIRECTORY REMOVAL [errno : %d]\n", errno);
                    perror("Error");
        			exit(EXIT_FAILURE);
        		}
        	}	
        	else {	
        		errno = (foundDir ? EISDIR : errno);
        		printf("ERROR OCCURED IN RM [errno : %d]\n", errno);
                perror("Error");
        		exit(EXIT_FAILURE);
        	}
        }
	}

	if(!foundFiles) {
		puts("INSUFFICIENT ARGUMENTS");
		exit(EXIT_FAILURE);
	}

	return 0;
}