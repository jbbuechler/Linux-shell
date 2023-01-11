#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]){

    // open file and print contents
    FILE* fptr;
    fptr = fopen(argv[1], "r");
    if (!fptr || argc!=2){
        printf("cat: read error\n");
    }
    char ch;
    while((ch=fgetc(fptr))!=EOF){
        printf("%c", ch);
    }
    fclose(fptr);
}
