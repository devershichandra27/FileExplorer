#include <unistd.h>
#include <iostream>
#include <vector>
#include <fcntl.h>
#include <sys/stat.h>

using namespace std;

vector <string> filelist;

void moveutil(string destination){
    
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
        remove(a.c_str());
    }
}

void move(int n, char const * argv[]){
    string destinationfolder = argv[n-1];

    for(int i=1; i<(n-1); i++)
        filelist.push_back(argv[i]);

    moveutil(destinationfolder);
}

int main(int argc, char const *argv[])
{
    move(argc, argv);
    return 0;
}
