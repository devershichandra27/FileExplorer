/*positioning cursor on random position in screen*/
#include <unistd.h>
#include <iostream>
#include <termio.h>
#include <vector>
#include <signal.h>
#include <sys/ioctl.h>

using namespace std;

void pageup();
void pagedown();


int nrows = 10;
int currenttopindex  = 0 ;
int currentbottomindex = 0;
vector<int> vi;

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
    while(true) {
        int ch = inputchar();
        if(ch==27){
            ch=inputchar();
            if(ch==91){
                ch=inputchar();
                switch (ch)
                {
                case 65:
                    printf("\033[1A");
                    break;
                case 66:
                    printf("\033[1B");
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
            pagedown();
        } else if (ch==73 || ch==105){
            pageup();
        } else if (ch==127){
            cout << "Backspace Pressed"<<endl;
        } else if(ch==81 || ch==113){
            return;
        }
    }
}
void pageup(){
    currenttopindex = currenttopindex-(nrows-1);
    currenttopindex = currenttopindex>0?currenttopindex:0;
    currentbottomindex = currenttopindex +(nrows-1);
    currentbottomindex = currentbottomindex<(vi.size()-1)?currentbottomindex: (vi.size()-1);
    cout << "\033[2J\033[1;1H";
    for(int i=currenttopindex; i<=currentbottomindex; i++){
        cout << vi[i] << endl;
    }
}

void pagedown(){
    currentbottomindex = currentbottomindex + (nrows-1);
    currentbottomindex = currentbottomindex>(vi.size()-1)?(vi.size()-1):currentbottomindex;
    currenttopindex = currentbottomindex - (nrows -1);
    currenttopindex = currenttopindex>0? currenttopindex: 0;
    cout << "\033[2J\033[1;1H";
    for(int i=currenttopindex; i<=currentbottomindex; i++){
        cout << vi[i] << endl;
    }
}

void display(){
    for(int i=0; i<500; i++){
        vi.push_back(i);
    }
    cout << "\033[2J\033[1;1H";
    for (int i = 0; (i < nrows)&&(i<vi.size()) ; i++){
        cout << vi[i] << endl;
        // if((((i+1)< nrows)&&((i+1)<vi.size())))
        //     cout << endl;
    }
    currenttopindex = 0;
    currentbottomindex = nrows-1;
    currentbottomindex = currentbottomindex<0?0:currentbottomindex;
}

void resizehandler(int k){
    if(k==28){
        struct winsize w;
        ioctl (0, TIOCGWINSZ, &w);
        nrows = w.ws_row-1;
    }
}

int main(int argc, char const *argv[])
{
    signal(28, resizehandler);
    struct winsize w;
    ioctl (0, TIOCGWINSZ, &w);
    nrows = (w.ws_row)-1;
    display();
    detectArrowKey();
    return 0;
}
