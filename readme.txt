***********************************************************************

	SIMPLE SHELL IN C [MADE BY DIVYANSH RASTOGI] (2019464)					

			OS Assignment 1			

***********************************************************************

LOCATIONS:

	1. "descriptions" contains description of each command executed in shell
	2. "external" contains source codes for external commands, "x" command would be present in "$xsh.c"
	3. "out" contains all binaries of external commands
	4. Makefile for compilation
	5. History.txt for maintaining command history of shell	

COMPILATION & EXECUTION:

	The shell binary relies on compilation of binaries of external commands.
	First, compilation of external binaries are performed.	
	
	Command to compile externals:
		make binaries
	
	Command to compile shell and run shell:
		make run

SHELL CONTROL & ERROR HANDLING:
	
	At start of execution:
		Shell by default opens & is loaded in current working directory.
		The history file is read and history list is formed.
		PATH variable of the shell is formed.
	
	To get started:
		Type 'help'. It will show the user the following available commands.
		To gain further insight on each command, type help [command].
		Error handling in case of mismatched inputs is taken care of appropriately.
	
	Available Commands: (Refer descriptions for the same)
		Internal:
			1. echo
			2. pwd
			3. cd
			4. history
			5. exit
		External:
			1. mkdir
			2. rm
			3. date
			4. ls
			5. cat
			
	Each of the following descriptions contains exclusive test-cases for every command. 
	For test cases for the shell, refer "test_cases.txt".
	
	The current working dir. is shown in the shell following user input:
		
		$OSAssignment_1_2019464> 
	
	'&' acts as a space delimiter in the input.
	The input automatically is terminated with a new line escape sequence.
	The input is parsed and a 2D array of commands is created which stores and space seperates the input.
	
	The commands are checked and flags for externalCommand and internalCommand is set accordingly.
	If both of the flags fail, an appropriate message of "Command not recognized" is displayed.
	
	For each internal and external commands, appropriate errors and cases are explicity handled. 
	(Refer descriptions for the same)
	
	The internal commands execute within our current binary while for an external command, a child process is forked and their respective binary is loaded.
	In case if, errors arise during forking or if the child processes is exited abnormally, appropriate error messages are shown accordingly.
	
	In case if the user input doesn't match any of the available commands, appropriate error messages are displayed.
	
