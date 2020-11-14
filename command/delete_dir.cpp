#include <unistd.h>
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
# include <cstring>
using namespace std;

void clean_dir(std::string foldername){
    DIR * directorypointer;
    struct dirent *d;
    if((directorypointer=opendir(foldername.c_str()))!= NULL){
        chdir(foldername.c_str());
        while((d = readdir(directorypointer))!=NULL) {
            struct stat s;
            stat(d->d_name, &s);
            if(S_ISDIR(s.st_mode)){
                if((strcmp(".", d->d_name)!=0)&&(strcmp("..", d->d_name)!=0)){
                    clean_dir(d->d_name);
                }
            } else {
                remove(d->d_name);
            }
        }
        chdir("..");
        closedir(directorypointer);
    }
}

void delete_dir(string folderpath){
    clean_dir(folderpath);
    DIR * directorypointer;
    struct dirent *d;
    if((directorypointer=opendir(folderpath.c_str()))!=NULL){
        chdir(folderpath.c_str());
        while((d=readdir(directorypointer))!=NULL){
            struct stat s;
            stat(d->d_name, &s);
            if(S_ISDIR(s.st_mode)){
                if((strcmp(d->d_name, ".")!=0)&&(strcmp(d->d_name, "..")!=0)){
                    if(folderpath[folderpath.length()-1]!='/'){
                        string str = folderpath+"/"+(d->d_name);
                        delete_dir(str);
                    } else {
                        string str = folderpath+(d->d_name);
                        delete_dir(str);
                    }
                }
            }
            cout << d->d_name << endl;
        }
        chdir("..");
    }
}

int main(int argc, char const *argv[])
{
    string deletefolderpath = argv[1];
    delete_dir(deletefolderpath);
    return 0;
}
