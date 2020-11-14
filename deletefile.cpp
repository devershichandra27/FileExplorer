#include <unistd.h>
#include <dirent.h>
#include <cstring>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
using namespace std;

void removefile(char * dir){
    remove(dir);
}

int main(int argc, char const *argv[])
{
    char * file=new char[512];
    strcpy(file, argv[1]);
    removefile(file);
    return 0;
}
