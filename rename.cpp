#include <unistd.h>
#include <iostream>
#include <vector>
#include <fcntl.h>
#include <sys/stat.h>

using namespace std;

vector <string> filelist;

void renameutil(string o, string n){
    struct stat s;
    int sourcedescriptor = open(o.c_str(), O_RDONLY);
    string destinationlocation = n;
    stat(o.c_str(), &s);
    int destinationdescriptor = open(destinationlocation.c_str(), O_CREAT|O_TRUNC |O_WRONLY, S_IRUSR|S_IWUSR);
    int buffer[1024];
    int countread;
    while(countread=read(sourcedescriptor, buffer, sizeof(buffer))){
        write(destinationdescriptor, buffer, countread);
    }
    chmod(destinationlocation.c_str(), s.st_mode);
    remove(o.c_str());
}

void rename(int n, char const * argv[]){
    string newname = argv[2];
    string oldname = argv[1];
    
    renameutil(oldname, newname);
}

int main(int argc, char const *argv[])
{
    rename(argc, argv);
    return 0;
}
