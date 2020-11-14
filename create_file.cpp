#include <unistd.h>
#include <iostream>
#include <fcntl.h>
using namespace std;

void create_file(string filename, string destinationfolder){
    string filelocation;
    if(destinationfolder[destinationfolder.length()-1] =='/'){
        filelocation = destinationfolder+filename;
    } else
        filelocation = destinationfolder + "/" + filename;
    
    int fdis = open(filelocation.c_str(), O_CREAT|O_TRUNC|O_WRONLY, S_IWUSR|S_IRUSR|S_IRGRP);
    if(fdis!=-1){
        cout << filename << ": File successfully created."<<endl;
    } else {
        cout << filename << ": File cannot be created" << endl;
    }
}


int main(int argc, char const *argv[])
{
    string filename = argv[1];
    string destinationfolder = argv[2];
    create_file(filename, destinationfolder);
    return 0;
}
