#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(void){
    int i = fork();
    int j = fork();

    printf("%d - %d\n", i,j);
    execl("./exploit", "exploit", 0);
}