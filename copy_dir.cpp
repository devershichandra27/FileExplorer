#include <unistd.h>
#include <dirent.h>
#include <cstring>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

void removedir(char * dir){
    DIR *dp;
    struct dirent * entry;
    struct stat statbuff;

    if((dp = opendir(dir))!=NULL){
        chdir(dir);
        while((entry = readdir(dp))!=NULL){
            lstat(entry->d_name, &statbuff);
            if(S_ISDIR(statbuff.st_mode)){
                if((strcmp(".", entry->d_name)==0) || strcmp("..", entry->d_name)==0){
                    continue;
                }
                removedir(entry->d_name);
            }
            remove(entry->d_name);
        }
        chdir("..");
        rmdir(dir);
    }
}

int main(int argc, char const *argv[])
{
    char * folderlocation;
    strcpy(folderlocation, argv[1]);
    cout << "Cleaning directory " << argv[1] << endl;
    removedir(folderlocation);
    return 0;
}
