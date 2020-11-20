#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#define MXN 100000
#define MVN 1000

int main(int argc, char *argv[]) {

	int numberAllLines = 0;
	int showEnds = 0;
	int foundFiles = 0;

	for(int argi = 1; argi < argc; ++ argi) {

		assert(argv[argi] != NULL);

		if(!foundFiles && argv[argi][0] == '-') { // is an option
			if((int) strlen(argv[argi]) > 1) {
				if(argv[argi][1] == 'E') {
					showEnds = 1;
					continue;
				}
				else if(argv[argi][1] == 'n') {
					numberAllLines = 1;
					continue;
				}
				else {
					puts("INVALID");
					exit(EXIT_FAILURE);
				}
			}
			else {
				puts("INVALID");
				exit(EXIT_FAILURE);
			}
		}

		int fd, sz;
		foundFiles = 1;
		fd = open(argv[argi], O_RDONLY);
			
		if(fd < 0) {
			perror("Error");
			exit(EXIT_FAILURE);
		}

		char data[MXN];
		sz = read(fd, data, sizeof(data));
		data[sz] = '\0';
		close(fd);

		int okToRead = 1;
		int ptrData = 0;
		int lineNumber = 1;

		while(okToRead) {

			char lineRead[MVN];
			int ptrLine = 0;

			// read a whole line
			while(ptrData < (int) strlen(data) && data[ptrData] != '\n' && data[ptrData] != 0x0) {
				lineRead[ptrLine ++] = data[ptrData ++];
			}

			if(showEnds) {
				lineRead[ptrLine ++] = '$';
			}

			lineRead[ptrLine] = '\0';
			
			// print the line
			if(numberAllLines) {
				printf("%d. %s\n", lineNumber, lineRead);
			}
			else {
				puts(lineRead);
			}

			ptrData ++;
			lineNumber ++;
			okToRead = (ptrData < (int) strlen(data));
		}
		puts("");
	}

	return 0;
}