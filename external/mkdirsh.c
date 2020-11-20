#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <sys/stat.h>
#include <limits.h>

#define MAXX 1000

int is_symlink(char *name) {
    struct stat p_statbuf;
    if(lstat(name, &p_statbuf) < 0)
        return 0;
    return (S_ISLNK(p_statbuf.st_mode) == 1);
}

int main(int argc, char *argv[]) {

    int assertError = 1;
    int beVerbose = 0;
    int foundFiles = 0;

    for(int i = 1; i < argc; ++ i) {

        assert(argv[i] != NULL);

        if(argv[i][0] == '-') { // check for options
            if((int) strlen(argv[i]) > 1 && argv[i][1] == 'p') { // don't show error if already existing
                assertError = 0;
                continue;
            }
            else if((int) strlen(argv[i]) > 1 && argv[i][1] == 'v') { // be verbose
                beVerbose = 1;
                continue;
            }
            else {
                puts("INVALID OPTIONS");
                exit(EXIT_FAILURE);
            }
        }

        foundFiles = 1;

        int status = mkdir(argv[i], 0777); // 0777 is dir permission
        // printf("%d\n", status);

        if(status == -1) {
            if(errno == EEXIST) {
                if(assertError) {
                    printf("DIRECTORY [%s] ALREADY EXISTS\n", argv[i]);
                    perror("Error");
                }
            }
            else if (!assertError && errno == ENOENT) { // prefix directories don't exist
                
                char *dirName = (char *) calloc(MAXX, sizeof(char));
                int ptrDirName = 0;

                int ptrArg = 0;
                char *arg = argv[i];
                
                // create individual nested directories
                while(ptrArg < (int) strlen(arg)) {
                    while(ptrArg < (int) strlen(arg) && arg[ptrArg] != '/' && arg[ptrArg] != '\n' && arg[ptrArg] != '\0') {
                        dirName[ptrDirName ++] = arg[ptrArg ++];
                    }

                    if(is_symlink(dirName) && beVerbose) {
                        printf("Resolving symlink at %s\n", dirName);
                    }

                    char *bufRealPath = (char *) calloc(MAXX, sizeof(char));
                    if (realpath(dirName, bufRealPath) == NULL) {
                        strcpy(bufRealPath, dirName);
                    }

                    int subStatus = mkdir(bufRealPath, 0777); // create directory
                    assert((subStatus != -1) || (subStatus == -1 && errno == EEXIST));
                    if(subStatus != -1 && beVerbose) {
                        printf("mkdir: Directory [%s] created\n", bufRealPath);
                    }

                    dirName[ptrDirName ++] = '/';
                    free(bufRealPath);
                    ptrArg ++; 
                }

                free(dirName);
            }
            else {
                printf("ERROR OCCURED [errno : %d]\n", errno);
                perror("Error");
                exit(EXIT_FAILURE);
            }
        }
        else {
            if(beVerbose) {
                printf("mkdir: Directory [%s] created\n", argv[i]);
            }
        }
    }

    if(!foundFiles) {
        puts("MISSING ARGUMENTS");
        exit(EXIT_FAILURE);
    }

    return 0;
}