/*test program to detect arrow key press, backspace, k, i and : (colon)*/
#include <unistd.h>
#include <iostream>
#include <termio.h>

using namespace std;

int inputchar(){
    int ch;
    struct termios oldsettings;
    struct termios newsettings;
    tcgetattr(STDIN_FILENO, &oldsettings);
    newsettings = oldsettings;
    newsettings.c_lflag = newsettings.c_lflag & ~ICANON;
    newsettings.c_lflag = newsettings.c_lflag & ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newsettings);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldsettings);
    return ch;
}

void detectArrowKey(){
    int ch = inputchar();
    if(ch==27){
        cout << "here 27"<<endl;
        ch=inputchar();
        if(ch==91){
            cout << "here 91"<<endl;
            ch=inputchar();
            switch (ch)
            {
            case 65:
                cout << "UP" <<endl;
                break;
            case 66:
                cout << "DOWN" <<endl;
                break;
            case 67:
                cout << "RIGHT" <<endl;
                break;
            case 68:
                cout << "LEFT" <<endl;
                break;
            default:
                break;
            }
        }
    } else if (ch==58){
        cout << "COLON Pressed" <<endl;
    } else if (ch==72 || ch==104){
        cout << "H Pressed"<<endl;
    } else if (ch==107 || ch==75){
        cout << "k Pressed"<<endl;
    } else if (ch==73 || ch==105){
        cout << "i Pressed"<<endl;
    } else if (ch==127){
        cout << "Backspace Pressed"<<endl;
    }else if (ch=='\n'){
        cout << "Enter"<<endl;
    } 
}

int main(int argc, char const *argv[])
{
   detectArrowKey();
    // int c = '^';
    // // char c = getchar();
    // int d = '[';
    // cout<< c << endl;
    // cout <<  d << endl;
    return 0;
}
