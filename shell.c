#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <limits.h>

#define MAXX 100000
#define CLIM 100
#define SLIM 1000
#define HLIM 16

// struct for maintaining history
struct keepHistory {
	char *pathHistory;
	char *history[HLIM + 1]; // history, maintain a static circular queue
	int startHistory;
	int endHistory;
};

struct keepHistory hist;

void insertHistory(char *lineCommand) { // insert into history list
	if((hist.endHistory + 1) % (HLIM) == hist.startHistory) {
		hist.history[hist.startHistory] = NULL;
		hist.startHistory = (hist.startHistory + 1) % (HLIM);
	}
	hist.history[hist.endHistory] = lineCommand;
	hist.endHistory = (hist.endHistory + 1) % (HLIM);
}

void writeToHistoryFile(char *lineCommand) { // insert it history file
	char *lineCommandToAppend = (char *) calloc(MAXX, sizeof(char));
	strcpy(lineCommandToAppend, lineCommand);
	strcat(lineCommandToAppend, "\n");
	// puts(lineCommandToAppend);
	int fd = open(hist.pathHistory, O_WRONLY | O_APPEND);
	assert(fd >= 0);
	write(fd, lineCommandToAppend, strlen(lineCommandToAppend));
	close(fd);
	free(lineCommandToAppend);
}

void readHistory() { // read from history file
	int fd, sz;
	fd = open(hist.pathHistory, O_RDONLY);
	assert(fd >= 0);
	char data[MAXX];
	sz = read(fd, data, sizeof(data));
	data[sz] = '\0';
	close(fd);

	int ptrData = 0;
	int okToRead = 1;

	while(okToRead) {
		// read a whole line
		char *lineRead = (char *) malloc(SLIM * CLIM * sizeof(char));
		int ptrLine = 0;
		while(ptrData < (int) strlen(data) && data[ptrData] != '\n' && data[ptrData] != 0x0) {
			lineRead[ptrLine ++] = data[ptrData ++];
		}
		lineRead[ptrLine] = '\0';
		if((int) strlen(lineRead) != 0)
			insertHistory(lineRead);
		ptrData ++;
		okToRead = (ptrData < (int) strlen(data));
	}
}

char *absolutePath; // absolute path for binaries

void linesplit(char *commands[], char *lineCommand, int *numCommands) {
	int ptrLineCommand = 0;
	while(ptrLineCommand < (int) strlen(lineCommand)) {
		char *newText = (char *) malloc(SLIM * sizeof(char));
		int ptrNewText = 0;
		while(ptrLineCommand < (int) strlen(lineCommand) && lineCommand[ptrLineCommand] != 32 && lineCommand[ptrLineCommand] != '\n' && lineCommand[ptrLineCommand] != 0x0) {
			newText[ptrNewText ++] = lineCommand[ptrLineCommand ++];
		} 
		newText[ptrNewText] = '\0';
		commands[(*numCommands)] = (char *) calloc(SLIM, sizeof(char));
		strcpy(commands[(*numCommands) ++], newText);
		while(ptrLineCommand < (int) strlen(lineCommand) && lineCommand[ptrLineCommand] == 32)
			++ ptrLineCommand;
	}

	// treating '&' as space delimiter
	for(int i = 0; i < *numCommands; ++ i) {
		for(int j = 0; j < (int) strlen(commands[i]); ++ j) {
			if(commands[i][j] == '&')
				commands[i][j] = 32;
		}
		// puts(commands[i]);
	}
}

// current paths
char *curpath[PATH_MAX]; // PATH
int lenCurPath = 0;
void formPath(char *path) { // form the current path 
	lenCurPath = 0;
	int ptrPath = 1; // as the 0th char would be /
	while(ptrPath < (int) strlen(path)) {
		char *here = (char *) calloc(SLIM, sizeof(char));
		int ptrHere = 0;
		while(ptrPath < (int) strlen(path) && path[ptrPath] != '/' && path[ptrPath] != '\0') {
			here[ptrHere ++] = path[ptrPath ++];
		}
		here[ptrHere] = '\0';
		if((int) strlen(here) == 0) //corner case
			break;
		curpath[lenCurPath ++] = here;
		ptrPath ++;
	}
}

void changePath(char *to) { // change the current path
	if(strcmp(to, ".") == 0)
		return;
	if(to[0] == '/') {
		// absolute path
		for(int i = 0; i < lenCurPath; ++ i)
			curpath[i] = NULL;
		formPath(to);
	}
	else if(strcmp(to, "..") == 0) {
		if(lenCurPath != 0) {
			lenCurPath --;
			curpath[lenCurPath] = NULL;
		}
	}
	else {
		int ptrTo = 0;
		while(ptrTo < (int) strlen(to)) {
			char *here = (char *) calloc(SLIM, sizeof(char));
			int ptrHere = 0;
			while(ptrTo < (int) strlen(to) && to[ptrTo] != '/' && to[ptrTo] != '\0') {
				here[ptrHere ++] = to[ptrTo ++];
			}	
			here[ptrHere] = '\0';
			if((int) strlen(here) == 0) //corner case
				break;
			curpath[lenCurPath] = (char *) calloc(strlen(here), sizeof(char));
			strcpy(curpath[lenCurPath ++], here);
			ptrTo ++;
		}
	}
}

void getPath(char *pathf) { // get net path
	assert(pathf != NULL);
	for(int i = 0; i < lenCurPath; ++ i) {
		char *here = (char *) calloc(SLIM, sizeof(char));
		here[0] = '/';
		strcat(here, curpath[i]);
		strcat(pathf, here);
	}
	if((int) strlen(pathf) == 0) {
		strcat(pathf, "/\0");
	}
}

void help(char *commands[], int *numCommands) { // help / command descriptions using cat

	if(*numCommands == 2) {
		char *args[3];
		args[2] = NULL;
		args[0] = (char *) calloc(FILENAME_MAX, sizeof(char));
		args[1] = (char *) calloc(FILENAME_MAX, sizeof(char));
		strcpy(args[0], absolutePath); // absolute path to location of binaries
		strcpy(args[1], absolutePath); // absolute path to location of binaries
		strcat(args[0], "/out/catsh");
		// internal
		if(strcmp(commands[1], "echo") == 0) {
			strcat(args[1], "/descriptions/echo.txt");
		}
		else if(strcmp(commands[1], "pwd") == 0) {
			strcat(args[1], "/descriptions/pwd.txt");
		}
		else if(strcmp(commands[1], "history") == 0) {
			strcat(args[1], "/descriptions/history.txt");
		}
		else if(strcmp(commands[1], "cd") == 0) {
			strcat(args[1], "/descriptions/cd.txt");
		}
		else if(strcmp(commands[1], "exit") == 0) {
			strcat(args[1], "/descriptions/exit.txt");
		}
		// external
		else if(strcmp(commands[1], "mkdir") == 0) {
			strcat(args[1], "/descriptions/mkdir.txt");
		}
		else if(strcmp(commands[1], "rm") == 0) {
			strcat(args[1], "/descriptions/rm.txt");
		}
		else if(strcmp(commands[1], "ls") == 0) {
			strcat(args[1], "/descriptions/ls.txt");
		}
		else if(strcmp(commands[1], "cat") == 0) {
			strcat(args[1], "/descriptions/cat.txt");
		}
		else if(strcmp(commands[1], "date") == 0) {
			strcat(args[1], "/descriptions/date.txt");
		}
		else {
			puts("INVALID OPTION");
			return;
		}	

		// fork a child for calling "cat" binary
		pid_t pid;
		pid = fork();
		int status;

		if(pid < 0) {
			puts("Forking Error");
			exit(EXIT_FAILURE);
		}
		else if(pid == 0) {
			execvp(args[0], args); //control passed 
			exit(EXIT_FAILURE); // if binary not found
		}
		else {
			waitpid(pid, &status, WUNTRACED);
			assert(WIFEXITED(status)); // child executed normally
			// printf("Exit status of child process for external command %d\n", (int) WEXITSTATUS(status));
		}

		free(args[0]);
		free(args[1]);
	}
	else if(*numCommands == 1) {
		puts("\nxxx SIMPLE SHELL IN C [MADE BY Divyansh Rastogi] xxx\n");
		puts("Available commands:");
		int idx = 1;
		printf("%d. echo\n", idx ++);
		printf("%d. pwd\n", idx ++);
		printf("%d. cd\n", idx ++);
		printf("%d. history\n", idx ++);
		printf("%d. exit\n", idx ++);
		printf("%d. mkdir\n", idx ++);
		printf("%d. rm\n", idx ++);
		printf("%d. cat\n", idx ++);
		printf("%d. ls\n", idx ++);
		printf("%d. date\n", idx ++);
		puts("\nType help [command], to view description of individual commands.");
	}
	else {
		puts("INVALID NUMBER OF ARGUMENTS");
		return;
	}
}

int main() {

	puts("\n*** SIMPLE SHELL : [Made By Divyansh Rastogi] ***");

	int exitOk = 0;

	// form absolute path
	absolutePath = (char *) calloc(FILENAME_MAX, sizeof(char));
	getcwd(absolutePath, FILENAME_MAX);
	printf("Loaded at [%s]\n\n", absolutePath);

	puts("Type 'help' to get started.\n");

	// read history from a prexisting file
	// create path for history file
	hist.startHistory = 0;
	hist.endHistory = 0;
	hist.pathHistory = (char *) calloc(FILENAME_MAX, sizeof(char));
	strcpy(hist.pathHistory, absolutePath);
	strcat(hist.pathHistory, "/history.txt");
	readHistory();

	//form current path
	formPath(absolutePath);

	while(!exitOk) {
		if(lenCurPath != 0)
			printf("$%s> ", curpath[lenCurPath - 1]);
		else
			printf("$> ");

		char *lineCommand = (char *) calloc(SLIM * CLIM, sizeof(char));
		fgets(lineCommand, CLIM * SLIM, stdin);
		lineCommand = strtok(lineCommand, "\n"); // replacing new line character
		// puts(lineCommand);

		char *commands[CLIM];
		int *numCommands = (int *) calloc(1, sizeof(int));
		linesplit(commands, lineCommand, numCommands);

		int isInternalCommand = 0, isExternalCommand = 0;
		
		// add to history
		insertHistory(lineCommand);

		// 5 internal commands
		// cd, echo, history, pwd, exit
		{	
			// ECHO
			if(strcmp(commands[0], "echo") == 0) {
				isInternalCommand = 1;
				int newLine = 1;
				int lookForOptions = 1;
				int lookForEscapeSequences = 0;
				for(int i = 1; i < *numCommands; ++ i) {
					if(lookForOptions && commands[i][0] == '-') { // is an option
						if((int) strlen(commands[i]) > 1) {
							if(commands[i][1] == 'n') {
								newLine = 0;
								continue;
							}
							if(commands[i][1] == 'e') {
								lookForEscapeSequences = 1;
								continue;
							}
						}
					}
					lookForOptions = 0;
					if(lookForEscapeSequences) {
						if(commands[i][0] == '\\') {
							if((int) strlen(commands[i]) > 1 && commands[i][1] == 'n') {
								printf("\n");
								continue;
							}
							if((int) strlen(commands[i]) > 1 && commands[i][1] == 't') {
								printf("\t");
								continue;
							}
						}
					}	
					printf("%s", commands[i]);
					if(i != *numCommands - 1)
							printf(" ");
				}	
				if(newLine)
					printf("\n");
			}

			// EXIT
			else if(strcmp(commands[0], "exit") == 0) {
				isInternalCommand = 1;
				if(*numCommands > 2) {
					puts("INVALID NUMBER OF ARGUMENTS");
				}
				else {
					exitOk = 1;
					if(*numCommands == 2) {
						int exitStatus = atoi(commands[1]);
						printf("Exited with %d exit status.\n", exitStatus);
					}
					else {
						puts("Exited with default exit status.");
					}
				}
			}

			// HISTORY
			else if(strcmp(commands[0], "history") == 0) {
				isInternalCommand = 1;
				if(*numCommands > 1) {
					if(commands[1][0] == '-') { // is an option
						if((int) strlen(commands[1]) > 0) {
							if(commands[1][1] == 'c') { // ignore args
								// clear 
								for(int i = 0; i < HLIM; ++ i)
									hist.history[i] = NULL;
								hist.startHistory = 0;
								hist.endHistory = 0;
								fclose(fopen(hist.pathHistory, "w")); // clearing contents of a file
							}
							else if(commands[1][1] == 's') {
								// append commands[2 ... n] as a line at the end of history
								if(*numCommands > 2) {
									int lastHistory = (hist.endHistory - 1 + HLIM) % HLIM;
									hist.history[lastHistory] = (char *) calloc(SLIM * CLIM, sizeof(char));
									for(int i = 2; i < *numCommands; ++ i) {
										strcat(hist.history[lastHistory], commands[i]);
										if(i != *numCommands - 1)
											strcat(hist.history[lastHistory], " ");
									}
								}
							}
							else {
								puts("INVALID OPTIONS");
							}
						}
						else {
							puts("INVALID OPTIONS");
						}
					}
					else {
						puts("INVALID OPTIONS");
					}
				}
				else {
					// print history
					for(int idx = 1, ptr = hist.startHistory; ptr != hist.endHistory && hist.history[ptr] != NULL; ptr = (ptr + 1) % HLIM , ++ idx) {
						printf("%d. %s\n", idx, hist.history[ptr]);
					}
				}
			}

			// PWD
			else if(strcmp(commands[0], "pwd") == 0) {
				isInternalCommand = 1;
				if(*numCommands > 2) {
					puts("INVALID NUMBER OF ARGUMENTS");
				}
				else if(*numCommands == 2) { // path resolving symlinks
					if(strcmp(commands[1], "-P") == 0) {
						char currentDirectory[FILENAME_MAX];
						getcwd(currentDirectory, FILENAME_MAX);
						printf("%s\n", currentDirectory);
					}
					else if(strcmp(commands[1], "-L") == 0) { // default case
						// path with symlinks
						char *pathHere = (char *) calloc(PATH_MAX, sizeof(char));
						getPath(pathHere);
						puts(pathHere);
						free(pathHere);
					}
					else {
						puts("INVALID OPTIONS");
					}
				}
				else {	
					// path with symlinks
					char *pathHere = (char *) calloc(PATH_MAX, sizeof(char));
					getPath(pathHere);
					puts(pathHere);
					free(pathHere);
				}
			}

			// CD
			else if(strcmp(commands[0], "cd") == 0) {
				isInternalCommand = 1;	
				if(*numCommands == 3) {
					if(strcmp(commands[1], "-P") == 0) { // symbolic links
						if(strcmp(commands[2], "~") == 0) { // corner case for home
							commands[2] = "/home";
						}
						char bufRealPath[PATH_MAX];
						char *res = realpath(commands[2], bufRealPath); // resolve the path without symbolic links
						if(res) {
							assert(chdir(bufRealPath) == 0);
							char currentDirectory[FILENAME_MAX];
							getcwd(currentDirectory, FILENAME_MAX);
							changePath(currentDirectory); // resolve all symlinks
						}
						else {
							puts("PATH COULDN'T BE RESOLVED");
							perror("Error");
						}
						free(res);
					}
					else if(strcmp(commands[1], "-L") == 0) { // default
						if(strcmp(commands[2], "~") == 0) { // corner case for home
							commands[2] = "/home";
						}
						if(chdir(commands[2]) != 0) {
							printf("CD TO [%s] FAILED: NO SUCH DIRECTORY\n", commands[2]);
							perror("Error");
						}
						else {
							changePath(commands[2]);
						}
					}
					else {
						puts("INVALID OPTIONS");
					}
				}
				else if(*numCommands == 2) {
					if(strcmp(commands[1], "~") == 0) { // corner case for home
						commands[1] = "/home";
					}
					if(chdir(commands[1]) != 0) {
						printf("CD TO [%s] FAILED: NO SUCH DIRECTORY\n", commands[1]);
						perror("Error");
					}
					else {
						changePath(commands[1]);
					}
				}
				else {
					puts("INVALID NUMBER OF ARGUMENTS");
				}

				// maintain curpath and actual path
				char *pathf = (char *) calloc(PATH_MAX, sizeof(char));
				getPath(pathf);
				assert(chdir(pathf) == 0);
				free(pathf);
			}
		}

		if(isInternalCommand) // no need to check for external command
			goto terminateLoop;

		// 5 external commands
		// ls, cat, date, rm, mkdir
		{ 	
			char *args[*numCommands + 1];
			args[*numCommands] = NULL;
			args[0] = (char *) calloc(FILENAME_MAX, sizeof(char));
			strcpy(args[0], absolutePath); // absolute path to location of binaries

			for(int param = 1; param < *numCommands; ++ param) {
				args[param] = (char *) calloc(FILENAME_MAX, sizeof(char));
				strcpy(args[param], commands[param]);
				// puts(args[param]);
			}

			if(strcmp(commands[0], "cat") == 0) {
				isExternalCommand = 1;
				strcat(args[0], "/out/catsh");		
			} 

			else if(strcmp(commands[0], "date") == 0) {
				isExternalCommand = 1;
				strcat(args[0], "/out/datesh");	
			}

			else if(strcmp(commands[0], "ls") == 0) {
				isExternalCommand = 1;
				strcat(args[0], "/out/lssh");	
			}

			else if(strcmp(commands[0], "mkdir") == 0) {
				isExternalCommand = 1;
				strcat(args[0], "/out/mkdirsh");	
			}

			else if(strcmp(commands[0], "rm") == 0) {
				isExternalCommand = 1;
				strcat(args[0], "/out/rmsh");
			}

			if(!isExternalCommand) {
				goto terminateLoop; // jmp to loop termination, no need to fork
			}

			// fork a child
			pid_t pid;
			pid = fork();
			int status;

			if(pid < 0) {
				puts("Forking Error");
				exit(EXIT_FAILURE);
			}
			else if(pid == 0) {
				execvp(args[0], args); //control passed 
				exit(EXIT_FAILURE); // if binary not found
			}
			else {
				waitpid(pid, &status, WUNTRACED);
				assert(WIFEXITED(status)); // child executed normally
				// printf("Exit status of child process for external command %d\n", (int) WEXITSTATUS(status));
			}

			// free memory
			for(int i = 0; i < *numCommands + 1; ++ i)
				free(args[i]);
		}

		terminateLoop:
		// enter the last command in the history file
		if(hist.startHistory != hist.endHistory) // empty condition
			writeToHistoryFile(hist.history[(hist.endHistory - 1 + HLIM) % HLIM]);

		// puts("Flag reached");

		if(!isInternalCommand && !isExternalCommand) {
			if(strcmp(commands[0], "help") == 0) { // check for help
				help(commands, numCommands);
			}
			else {
				puts("COMMAND NOT RECOGNIZED");
			}
		}
	}

	return 0;
}