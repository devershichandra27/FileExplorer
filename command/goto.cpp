#include <unistd.h>
#include <iostream>

using namespace std;

void gotoloc(string destinationpath){
    chdir(destinationpath.c_str());
}

int main(int argc, char const *argv[])
{
    string dest = argv[1];
    gotoloc(dest);
    char buf[256];
    getcwd(buf, sizeof(buf));
    cout << buf << endl;
    return 0;
}
