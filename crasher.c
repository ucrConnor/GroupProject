#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(void){
    fork();
    fork();
    fork();
    fork();
    while(1)
        execl("./crash", "crash", 0);
}