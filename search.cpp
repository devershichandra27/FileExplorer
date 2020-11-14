#include <unistd.h>
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <cstring>

using namespace std;

string searchutil(string directoryPath, string filename){
    DIR * directorypointer;
    struct dirent *d;
    struct stat s;
    if((directorypointer=opendir(directoryPath.c_str()))!= NULL){
        chdir(directoryPath.c_str());
        while((d = readdir(directorypointer))!=NULL) {
            if(strcmp(filename.c_str(), d->d_name)==0){
                return "True";
            }
            stat(d->d_name, &s);
            if(S_ISDIR(s.st_mode)){
                if((strcmp(".", d->d_name)!=0)&&(strcmp("..", d->d_name)!=0)){
                    if(strcmp(searchutil(d->d_name, filename).c_str(), "True")==0)
                        return "True";
                }
            }
        }
        chdir("..");
        closedir(directorypointer);
    }
    return "False";
}

string start_search(string filename){
    char buff[256];
    getcwd(buff, sizeof(buff));
    return searchutil(buff, filename);
}
int main(int argc, char const *argv[])
{
    string filename = argv[1];
    cout << start_search(filename)<<endl;
    return 0;
}
