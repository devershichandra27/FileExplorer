#include <unistd.h>
#include <iostream>
#include <vector>
#include <fcntl.h>
#include <sys/stat.h>

using namespace std;

void copyutil(string destination, vector<string> filelist){
    
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



void copy(int n, char const * argv[]){
    string destinationfolder = argv[n-1];
    vector <string> filelist;
    for(int i=1; i<(n-1); i++)
        filelist.push_back(argv[i]);

    copyutil(destinationfolder, filelist);
}

int main(int argc, char const *argv[])
{
    copy(argc, argv);
    return 0;
}
