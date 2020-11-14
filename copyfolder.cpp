#include<unistd.h>
#include <iostream>
#include <dirent.h>
#include <cstring>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>

using namespace std;

void copyfileutil(string destination, vector<string> filelist){
    
    for(string a : filelist){
        struct stat s;
        int sourcedescriptor = open(a.c_str(), O_RDONLY);
        string destinationlocation;
        stat(a.c_str(), &s);
        if(destination[destination.length()-1]=='/')
            destinationlocation = destination + a;
        else
            destinationlocation = destination + "/" + a;
        
        int destinationdescriptor = open(destinationlocation.c_str(), O_CREAT|O_TRUNC |O_WRONLY, S_IRUSR|S_IWUSR);
        int buffer[1024];
        int countread;
        while(countread=read(sourcedescriptor, buffer, sizeof(buffer))){
            write(destinationdescriptor, buffer, countread);
        }
        chmod(destinationlocation.c_str(), s.st_mode);
    }
}
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
void copydir(string source, string destination){
    cout << source<< endl;
    cout << destination<< endl;
    string s = source;
    string d=destination;
    if(destination[d.length()-1]!='/')
        d = destination + string("/") + s;
    else
        d = destination + s;

    string exists = start_search(d);
    if(strcmp(exists.c_str(), "False")==0){
        mkdir(d.c_str(), S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IXOTH);
    }
    struct stat statbuf;
    lstat(source.c_str(), &statbuf);
    chmod(d.c_str(), statbuf.st_mode);
    
    DIR * dp;
    struct dirent * entry;
    struct stat statbuffer;

    if ((dp=opendir(source.c_str()))!=NULL){
        chdir(source.c_str());
        while((entry=readdir(dp))!=NULL){
            lstat(entry->d_name, &statbuffer);
            vector<string>filelist;
            if(S_ISDIR(statbuffer.st_mode)){
                if((strcmp(".", entry->d_name)!=0) && (strcmp(".", entry->d_name)!=0)){
                    string temp = source + string("/") + (entry->d_name);
                    cout << "REACHED DIRECTORY" << endl;
                    copydir(temp, destination);
                }
            }else {
                cout << "REACHED FILE" << endl;
                filelist.push_back(entry->d_name);
                copyfileutil(destination, filelist);
                filelist.clear();
            }        
        }
        chdir("..");
        closedir(dp);
    }


}

int main(int argc, char const *argv[])
{
    string s = "dsa";
    string d = "temp";
    cout <<s << endl;
    cout <<d << endl;
    copydir(s, d);
    return 0;
}
