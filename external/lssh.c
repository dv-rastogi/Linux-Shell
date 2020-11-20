#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include <assert.h>

#define SLIM 1000
#define FLIM 1000

void alphaSort(char **names, int n) { // sort according to ascii

    char temp1[SLIM];
    char temp2[SLIM];

    for(int i = 0; i < n - 1; ++ i) {
        for(int j = i + 1; j < n; ++ j) {
            strcpy(temp1, names[i]);
            strcpy(temp2, names[j]);

            // as comparator checks only alphabetically, convert all uppercase into lowercase
            for(int k = 0; k < (int) strlen(temp1); ++ k) {
                if((int) temp1[k] >= 65 && (int) temp1[k] <= 90) {
                    temp1[k] += 32;
                }
            }
            for(int k = 0; k < (int) strlen(temp2); ++ k) {
                if((int) temp2[k] >= 65 && (int) temp2[k] <= 90) {
                    temp2[k] += 32;
                }
            }

            if(strcmp(temp2, temp1) < 0) {
                // swap
                char *temp = (char *) malloc(SLIM * sizeof(char));
                strcpy(temp, names[i]);
                strcpy(names[i], names[j]);
                strcpy(names[j], temp);
            }
        }
    }
}

void fillExt(char *temp, char *s) { // extract extension
    int ptr = 0;
    while(ptr < (int) strlen(s) && s[ptr]!= '.') {
        ptr ++;
    }   
    ptr ++;
    int ptrTemp = 0;
    temp[ptrTemp ++] = '.';
    while(ptr < (int) strlen(s)) {
        temp[ptrTemp ++] = s[ptr ++];
    }
    temp[ptrTemp ++] = '\0';
}

void extSort(char **names, int n) { // sort according to extensions

    for(int i = 0; i < n - 1; ++ i) {
        for(int j = i + 1; j < n; ++ j) {
            
            char *temp1 = (char *) calloc(SLIM, sizeof(char));
            char *temp2 = (char *) calloc(SLIM, sizeof(char));

            fillExt(temp1, names[i]);
            fillExt(temp2, names[j]);

            if(strcmp(temp2, temp1) < 0) {
                // swap
                char *temp = (char *) malloc(SLIM * sizeof(char));
                strcpy(temp, names[i]);
                strcpy(names[i], names[j]);
                strcpy(names[j], temp);
            }
        }
    }
}

int main(int argc, char* argv[])
{
    struct dirent *curFile;

    int showHiddenFiles = 0;
    int sortFilesExt = 0;
    int foundFiles = 0;

    for(int i = 1; i < argc; ++ i) {

        assert(argv[i] != NULL);

        if(!foundFiles && argv[i][0] == '-') { // check for options
            if((int) strlen(argv[i]) > 1 && argv[i][1] == 'a') { // show hidden files
                showHiddenFiles = 1;
                continue;
            }
            else if((int) strlen(argv[i]) > 1 && argv[i][1] == 'X') { // sort extensions
                sortFilesExt = 1;
                continue;
            }
            else {
                puts("INVALID OPTIONS");
                exit(EXIT_FAILURE);
            }
        }

        if(strcmp(argv[i],"~") == 0) // system shortcuts
            argv[i] = "/home";

        foundFiles = 1; // dirs found
        DIR *curDir = opendir(argv[i]);
        if(curDir == NULL) {
            perror("Error");
            exit(EXIT_FAILURE);
        }
        
        char *names[FLIM];
        int numFiles = 0;
        while((curFile = readdir(curDir)) != NULL) {
            // printf("%s\n", curFile -> d_name);
            if((curFile -> d_name)[0] == '.' && !showHiddenFiles)
                continue;

            names[numFiles] = (char *) malloc(SLIM * sizeof(char));
            strcpy(names[numFiles ++], curFile -> d_name); 
        }

        if(sortFilesExt) {
            extSort(names, numFiles);
        }
        else {
            alphaSort(names, numFiles);
        } 

        printf("%s:\n", argv[i]);
        for(int i = 0; i < numFiles; ++ i) {
            printf("%s   ", names[i]);
        }
        puts("");

        closedir(curDir);
    }

    if(!foundFiles) {
        // no dirs found, print default directory
        DIR *curDir = opendir(".");
        assert(curDir != NULL);
        char *names[FLIM];
        int numFiles = 0;
        while((curFile = readdir(curDir)) != NULL) {
            // printf("%s\n", curFile -> d_name);
            if((curFile -> d_name)[0] == '.' && !showHiddenFiles)
                continue;

            names[numFiles] = (char *) malloc(SLIM * sizeof(char));
            assert(names[numFiles] != NULL);
            strcpy(names[numFiles ++], curFile -> d_name); 
        }

        if(sortFilesExt) {
            extSort(names, numFiles);
        }
        else {
            alphaSort(names, numFiles);
        } 

        printf(".:\n");
        for(int i = 0; i < numFiles; ++ i) {
            printf("%s   ", names[i]);
        }
        puts("");

        closedir(curDir);
    }

    return 0;
}