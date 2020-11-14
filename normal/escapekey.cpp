#include <unistd.h>
#include <iostream>
#include <termio.h>

using namespace std;

int inputchar()
{
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

int main(int argc, char const *argv[])
{
    char ch;
    string com = "";
    while (true)
    {
        char ch = (char)inputchar();
        if (ch == '\e')
        {
            printf("\r\033[K\r");
            
            break;
        }
        if ((int)ch == 127)
        {
            printf("\b \b");
            if (com.size())
            {
                // cout << "popping back " << command.size();
                com.pop_back();
            }
        }
        if (ch == '\n')
        {
            // splitcommand(command);
            printf("\r\033[K\r");
            cout << com;
            // printf("\033[%d;1H\033[K",nrows)
        }
        else
        {
            cout << ch;
            com.push_back(ch);
        }
    }
    printf("\r\033[K\r");

    cout << com << endl;
    com = "";
    return 0;
}
