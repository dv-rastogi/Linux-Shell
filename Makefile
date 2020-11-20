run:
	@gcc -Wall -o shell shell.c
	@./shell
binaries:
	gcc -Wall -o out/catsh external/catsh.c
	gcc -Wall -o out/datesh external/datesh.c
	gcc -Wall -o out/lssh external/lssh.c
	gcc -Wall -o out/mkdirsh external/mkdirsh.c
	gcc -Wall -o out/rmsh external/rmsh.c

