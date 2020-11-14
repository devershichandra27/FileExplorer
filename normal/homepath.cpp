#include <unistd.h>
#include<iostream>
#include <cstring>
using namespace std;


int main(int argc, char const *argv[])
{
    char buf[256];
    getcwd(buf, sizeof(buf));
    string homepath = buf;
    cout << homepath << endl;
    return 0;
}
