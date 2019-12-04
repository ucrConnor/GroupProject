#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* compares username -- called after every new line */
bool checkUsername(FILE * f) {
    /* IF THE USERNAME CHANGES, WE MUST ALSO CHANGE LENGTH */
    char username[] = "test";
    int length = 4;

    /* doesn't need to change if username changes */
    bool equal = true;

    for (int i = 0; i < length; i++) {
        /* 
            technically, this would return that test and test2 as the same
            not gonna worry about this case for now
        */
        char c = fgetc(f);
        if (!(username[i] == c)) {
            equal = false;
            break;
        }
    }   
    return equal;
}

void genEntry(FILE * f, char * myEntry) {
    /*
        Expected Format
        osboxes:x:1000:1000::/home/osboxes:/bin/bash
    */
    // char generatedEntry[100];
    // 1 = eof uname, 2 = eof pw, 3 = eof uid, 4 = eof gid
    int colonCount = 0;
    int index = 0;
    bool goNext = false;

    char c = fgetc(f);

    while(c != '\n') {
        // printf("%c", c);
        if ((colonCount == 2 || colonCount == 3) && goNext != true) {
            myEntry[index] = '0';
            index++;
            goNext = true;
        }

        if (c == ':') {
            colonCount++;
            goNext = false;
        }

        if (!goNext) {
            myEntry[index] = c;
            index++;
        }

        c = fgetc(f);
    }
    // printf("\n");
    myEntry[index] = '\n';
    // int k = 0;
    // while(generatedEntry[k] != '\n') {
    //     printf("%c", generatedEntry[k]);
    //     k++;
    // }
    // printf("\n");
    // return &myEntry[0];
}

void nextLine(FILE * f) {
    char c = fgetc(f);
    while (c != EOF && c != '\n') {
        c = fgetc(f);
    }
}

void writeMyLine(FILE * f, char * line) {
    // printf("start of writeMyLine\n");
    int index = 0;
    while (line[index] != '\n') {
        // printf("%c", *iterator);
        fputc(line[index], f);
        index++;
    }
    // printf("\n");
}

int main() {
    FILE *ogFile, *newFile;
    ogFile = fopen("/etc/passwd", "r");
    newFile = fopen("./tmp/passwd", "w");

    char c = fgetc(ogFile);
    bool useMyEntry = false;
    char * myEntry = malloc(sizeof(char) * 100);
    fpos_t pos;
    // copy the files from /etc to ./tmp/passwd
    while(c != EOF) {
        fgetpos(ogFile, &pos);

        if (c == '\n') {
            if (checkUsername(ogFile)) {
                fsetpos(ogFile, &pos);
                // printf("username worked\n");
                genEntry(ogFile, myEntry);
                int k = 0;
                // while(myEntry[k] != '\n') {
                //     printf("%c", myEntry[k]);
                //     k++;
                // }
                // printf("\n");
                useMyEntry = true;
            } else {
                fsetpos(ogFile, &pos);
            }
        } 

        if (useMyEntry) {

            fputc('\n', newFile);
            // printf("before writemyline\n");
            writeMyLine(newFile, myEntry);
            // printf("writeMyLine worked\n");
            useMyEntry = false;
        } else {
            fputc(c, newFile);
        }
        c = fgetc(ogFile);
    }
}