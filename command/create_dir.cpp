#include <unistd.h>
#include <iostream>
#include <sys/stat.h>

using namespace std;

void create_dir(string foldername, string folderlocation){
    string filelocation;
    if(folderlocation[folderlocation.length()-1] =='/'){
        filelocation = folderlocation+foldername;
    } else
        filelocation = folderlocation + "/" + foldername;
    int k = mkdir(filelocation.c_str(), S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IXOTH);
    if(k==0){
        cout << "Folder created succesfully." << endl;
    } else {
        cout << "Folder could not be created." << endl;
    }
}

int main(int argc, char const *argv[])
{
    string foldername = argv[1];
    string folderlocation = argv[2];
    create_dir(foldername, folderlocation);
    return 0;
}
