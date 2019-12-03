#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* compares username -- called after every new line */
bool checkUsername(char *p) {
    /* IF THE USERNAME CHANGES, WE MUST ALSO CHANGE LENGTH */
    char username[] = "test";
    int length = 4;

    /* doesn't need to change if username changes */
    char * copy = p; // copy so that we don't modify the pointer in main
    bool equal = true;

    for (int i = 0; i < length; i++) {
        /* 
            technically, this would return that test and test2 as the same
            not gonna worry about this case for now
        */
        if (!(username[i] == *(copy + i))) {
            equal = false;
            break;
        }
    }   
    return equal;
}

char * genEntry(char *p) {
    /*
        Expected Format
        osboxes:x:1000:1000::/home/osboxes:/bin/bash
    */
    char * copy = p;
    char generatedEntry[100];
    // 1 = eof uname, 2 = eof pw, 3 = eof uid, 4 = eof gid
    int colonCount = 0;
    int index = 0;
    bool goNext = false;


    while(*(copy) != '\n') {
        char currentChar = *copy;

        if (colonCount == 2 || colonCount == 3) {
            currentChar = '0';
            goNext = true;
        }

        if (currentChar == ':') {
            colonCount++;
            goNext = false;
        }

        if (!goNext) {
            generatedEntry[index] = currentChar;
        }

        index++;
        copy++;
    }
    generatedEntry[index] = '\n';
    return generatedEntry;
}

void nextLine(FILE * f) {
    char c = fgetc(f);
    while (c != EOF && c != '\n') {
        c = fgetc(f);
    }
}

void writeMyLine(FILE * f, char * line) {
    char * iterator = line;
    while (*iterator != '\n') {
        fputc(*iterator, f);
        iterator++;
    }
}

int main() {
    FILE *ogFile, *newFile;
    ogFile = fopen("/etc/passwd", "r");
    newFile = fopen("./tmp/passwd", "w");

    char c = fgetc(ogFile);
    bool useMyEntry = false;
    char * myEntry;
    // copy the files from /etc to ./tmp/passwd
    while(c != EOF) {

        // short circuit evaluation
        if (c == '\n' && checkUsername(ogFile + 1)) {
           myEntry = genEntry(ogFile + 1);
           useMyEntry = true;
        } 

        if (useMyEntry) {
            fputc(newFile, '\n');
            writeMyLine(newFile, myEntry);
            nextLine(ogFile);
            useMyEntry = false;
        } else {
            fputc(c, newFile);
        }
        c = fgetc(ogFile);
    }
}