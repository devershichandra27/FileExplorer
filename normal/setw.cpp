/*checking the functioning of setw*/
#include <iostream>
#include <iomanip>
using namespace std;

int main(int argc, char const *argv[])
{
    string str = "Devershi";
    string s = "Chandra";
    string c = "Srivastava";
    cout  << str<< setw(2); cout << s << setw(11);cout << c <<endl;
    return 0;
}
