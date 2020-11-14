/*Shoud print terminal size on top line every time the terminal is resized*/
#include <iostream>
#include <sys/ioctl.h>
#include <signal.h>

using namespace std;

void printSize(){
    struct winsize w;
    ioctl (0, TIOCGWINSZ, &w);
    cout << "\033[2J\033[1;1H" << endl;
    printf("lines: %d\n", w.ws_row);
    printf("Cols: %d\n", w.ws_col);
}
void signal_handler(int k){
    if(k==28)
        printSize();
}

int main(int argc, char const *argv[])
{
    signal(28, signal_handler);
    getchar();
    return 0;
}

