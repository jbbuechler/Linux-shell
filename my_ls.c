#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

void ls(const char *dir){

    // open directory and print contents 
    struct dirent *d;
    DIR *dh = opendir(dir);
    if (!dh) {
        if(errno == ENOENT){
            perror("Directory does not exist");
        }
        else{
        perror("Unable to read directory");
        }
        exit(EXIT_FAILURE);
    }
    while ((d = readdir(dh)) != NULL){
        if (d->d_name[0] == '.') continue;
        printf("%s  ", d->d_name);
    }
    printf("\n");
}

int main(int argc, const char *argv[]){
    if (argc == 1){
        ls(".");
    }
    else{
        printf("ls: too many arguments\n");
    }
}
