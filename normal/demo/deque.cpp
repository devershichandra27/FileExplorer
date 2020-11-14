#include <iostream>
#include <deque>

using namespace std;

int main(int argc, char const *argv[])
{
    deque <int> d;
    d.push_back(1);
    d.push_back(2);
    cout << d[1] << endl;

    return 0;
}
