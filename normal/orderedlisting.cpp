/*Ordered listing of directories. And selecting using cursor movement*/
#include <unistd.h>
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <vector>
#include <algorithm>
#include <ctime>
#include <bitset>
#include <pwd.h>
#include <grp.h>
#include <termio.h>
#include <signal.h>
#include <sys/ioctl.h>

using namespace std;

void pageup();
void pagedown();

vector <string> directorylist;
int nrows = 10;
int currenttopindex = 0;
int currentbottomindex = 0;
bool scroll = true;

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
    if((currenttopindex==0)&& scroll )
        return;
    currenttopindex = currenttopindex-(nrows-1);
    currenttopindex = currenttopindex>0?currenttopindex:0;
    currentbottomindex = currenttopindex +(nrows-1);
    currentbottomindex = currentbottomindex<(directorylist.size()-1)?currentbottomindex: (directorylist.size()-1);
    cout << "\033[2J\033[1;1H";
    for(int i=currenttopindex; i<=currentbottomindex; i++){
        cout << directorylist[i] << endl;
    }
}

void pagedown(){
    if((currentbottomindex==(directorylist.size()-1)) && scroll)
        return;
    currentbottomindex = currentbottomindex + (nrows-1);
    currentbottomindex = currentbottomindex>(directorylist.size()-1)?(directorylist.size()-1):currentbottomindex;
    currenttopindex = currentbottomindex - (nrows -1);
    currenttopindex = currenttopindex>0? currenttopindex: 0;
    cout << "\033[2J\033[1;1H";
    for(int i=currenttopindex; i<=currentbottomindex; i++){
        cout << directorylist[i] << endl;
    }
}

string formatpermission(mode_t p){
    bitset<9> x(p);
    string str = x.to_string();
    string result = "";
    int counter = 0;
    while(counter<3){
        if(str[0+(3*counter)]=='1')
            result.append(string(1, 'r'));
        else
            result.append(string(1, '-'));
        if(str[1+(3*counter)]=='1')
            result.append(string(1, 'w'));
        else
            result.append(string(1, '-'));
        if(str[2+(3*counter)]=='1')
            result.append(string(1, 'x'));
        else
            result.append(string(1, '-'));
        counter++;
    }
    return result;
}

string getusername (uid_t u){
    struct passwd * p;
    p = getpwuid(u);
    return p->pw_name;
}

string getgroupname (gid_t g){
    struct group *p;
    p = getgrgid(g);
    return p->gr_name;
}

void formatsize(off_t s){
    string result="";
    if(s<1024){
        cout << s << " B ";
        return;
    }
    float q = s/1024;
    if(q<1024){
        q = (int) (q*100 + 0.5);
        q = (float) q/100;
        cout << q << " KB ";
        return;
    }
    q= q/1024;
    if(q<1024){
        q = (int) (q*100 + 0.5);
        q = (float) q/100;
        cout << q << " MB ";
        return;
    }
    q= q/1024;
    if(q<1024){
        q = (int) (q*100 + 0.5);
        q = (float) q/100;
        cout << q << " GB ";
        return;
    }
    q= q/1024;
    q = (int) (q*100 + 0.5);
    q = (float) q/100;
    cout << q << " TB ";
}

void filldirectorylistvector(string path){
    DIR * directorypointer;
    struct dirent * entry;
    directorylist.clear();
    if((directorypointer = opendir(path.c_str())) == NULL){
        write(2, "Cannot open directory.\n", 23);
        return;
    }
    chdir(path.c_str());
    while((entry = readdir(directorypointer))!= NULL){
        directorylist.push_back(entry->d_name);
    }
    sort(directorylist.begin(), directorylist.end());
    // for(string s : directorylist){
        // struct stat info;
        // stat(s.c_str(), &info);
        // char * modifiedTime = ctime(&info.st_mtim.tv_sec);
        // string permission = formatpermission(info.st_mode);
        // string finalPermission = S_ISDIR(info.st_mode)==true?"d":"-";
        // finalPermission.append(permission);
        // cout << s<< "\t"; formatsize(info.st_size); cout<<"\t"<< getusername(info.st_uid) << "\t"<< getgroupname(info.st_gid)  <<"\t"<<finalPermission << "\t" << modifiedTime;
    // }
    //chdir("..");
}

void display(string pathname){
    filldirectorylistvector(pathname);
    cout << "\033[2J\033[1;1H";
    for (int i = 0; (i < nrows)&&(i<directorylist.size()); i++){
        cout << directorylist[i]<<endl;
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
        scroll=false;
    }
}

int main(int argc, char const *argv[])
{
    signal(28, resizehandler);
    struct winsize w;
    ioctl (0, TIOCGWINSZ, &w);
    nrows = (w.ws_row)-1;
    display(argv[1]);
    detectArrowKey();
    return 0;
}