/*program to display the content of the file in vi editor*/
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <sys/wait.h>

using namespace std;

void displayfile(string filename){
    int pid = fork();
    if(pid){
        //parent
        int restat;
        waitpid(pid, &restat, 0);
    } else {
        char * fname = (char *)malloc(filename.length()+1);
        strcpy(fname, filename.c_str());
        char * const a[] = {"vi", fname, NULL};
        execvp("vi", a);
    }
}

int main(int argc, char const *argv[])
{
    string filename = "getsize.cpp";
    displayfile(filename);
    return 0;
}
