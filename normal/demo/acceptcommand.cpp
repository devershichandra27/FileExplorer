/*Improved formattedoutput.cpp. Accepting commands using colon key*/
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
#include <map>
#include <sys/wait.h>
#include <cstring>
#include <deque>
#include <iomanip>

using namespace std;

void pageup();
void pagedown();
void enterkeypressed(int);
void display(string);
string formatpermission(mode_t);
string getusername(uid_t);
void formatsize(off_t);
string getgroupname(gid_t);
void executecommand(string);

vector <string> directorylist;
int nrows = 10;
int currenttopindex = 0;
int currentbottomindex = 0;
bool scroll = true;
int cursorindex  =-1;
map <int, string> indexmapper;
string homedirectorypath;
string currentdirectorypath;
deque <string> pathstack(1000);
int pathstackcounter, maxstacksize;
string prevpath;
string nextpath;


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
                    if(((cursorindex-1)>=currenttopindex)&&((cursorindex-1)<=currentbottomindex)){
                        printf("\033[1A");
                        cursorindex--;
                    }
                    break;
                case 66:
                    if(((cursorindex+1)>=currenttopindex)&&((cursorindex+1)<=currentbottomindex)){
                        printf("\033[1B");
                        cursorindex++;
                    }
                    break;
                case 67:
                    //right
                    //pathstackcounter++;
                    nextpath = pathstack[pathstackcounter];
                    display(nextpath);
                    break;
                case 68:
                    //left
                    if(pathstackcounter>1){
                        pathstackcounter-=2;
                        prevpath = pathstack[pathstackcounter];
                    }
                    else
                        prevpath = homedirectorypath;
                    display(prevpath);
                    break;
                default:
                    break;
                }
            }
        } else if (ch==58){
            //Colon pressed
            printf("\033[%d;1H",nrows);
            string command;
            getline(cin, command);
            printf("\033[%d;1H",nrows);
            executecommand(command);
        } else if (ch==72 || ch==104){
            display(homedirectorypath);
        } else if (ch==107 || ch==75){
            pagedown();
        } else if (ch==73 || ch==105){
            pageup();
        } else if (ch==127){
            //cout << "Backspace Pressed"<<endl;
            display("..");
        } else if(ch=='\n'){
            int key = cursorindex;
            printf("\033[1;1H");
            cursorindex=currenttopindex;
            enterkeypressed(key);
        } else if(ch==81 || ch==113){
            //q Q pressed.
            cout << "\033[2J\033[1;1H";
            cout << "Explorer exited." << endl;
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
    indexmapper.clear();
    for(int i=currenttopindex; i<=currentbottomindex; i++){
        cout<<setw(30) << directorylist[i];
        struct stat info;
        stat(directorylist[i].c_str(), &info);
        char * modifiedtime = ctime(&info.st_mtim.tv_nsec);
        string permission = formatpermission(info.st_mode);
        string finalPermission = S_ISDIR(info.st_mode)==true?"d":"-";
        finalPermission.append(permission);
        formatsize(info.st_size);
        cout<<setw(10)<< getusername(info.st_uid);
        cout<<setw(10)<< getgroupname(info.st_gid);
        cout<< setw(30) << modifiedtime ;
        indexmapper[i] = directorylist[i];
    }
    printf("\033[1;1H");
    cursorindex=currenttopindex;
}

void pagedown(){
    if((currentbottomindex==(directorylist.size()-1)) && scroll)
        return;
    currentbottomindex = currentbottomindex + (nrows-1);
    currentbottomindex = currentbottomindex>(directorylist.size()-1)?(directorylist.size()-1):currentbottomindex;
    currenttopindex = currentbottomindex - (nrows -1);
    currenttopindex = currenttopindex>0? currenttopindex: 0;
    cout << "\033[2J\033[1;1H";
    indexmapper.clear();
    for(int i=currenttopindex; i<=currentbottomindex; i++){
        cout<<setw(30) << directorylist[i];
        struct stat info;
        stat(directorylist[i].c_str(), &info);
        char * modifiedtime = ctime(&info.st_mtim.tv_nsec);
        string permission = formatpermission(info.st_mode);
        string finalPermission = S_ISDIR(info.st_mode)==true?"d":"-";
        finalPermission.append(permission);
        formatsize(info.st_size);
        cout<<setw(10)<< getusername(info.st_uid);
        cout<<setw(10)<< getgroupname(info.st_gid);
        cout<< setw(30) << modifiedtime ;
        indexmapper[i] = directorylist[i];
    }
    printf("\033[1;1H");
    cursorindex=currenttopindex;
}

void executecommand(string command){
    //TODO split commands in tokens.
    //execute that function
}

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

void enterkeypressed(int k){
    string name = indexmapper[k];
    struct stat s;
    lstat(name.c_str(), &s);
    if(S_ISDIR(s.st_mode)){
        display(name);
    } else {
        displayfile(name);
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
        cout <<setw(8) << s; cout << setw(2)<< " B";
        return;
    }
    float q = s/1024;
    if(q<1024){
        q = (int) (q*100 + 0.5);
        q = (float) q/100;
        cout<<setw(7) <<q ; cout << setw(3)<<  " KB";
        return;
    }
    q= q/1024;
    if(q<1024){
        q = (int) (q*100 + 0.5);
        q = (float) q/100;
        cout<<setw(7) <<q ; cout << setw(3)<<  " MB";
        return;
    }
    q= q/1024;
    if(q<1024){
        q = (int) (q*100 + 0.5);
        q = (float) q/100;
        cout<<setw(7) <<q ; cout << setw(3)<<  " GB";
        return;
    }
    q= q/1024;
    q = (int) (q*100 + 0.5);
    q = (float) q/100;
    cout<<setw(7) <<q ; cout << setw(3)<<  " TB";
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
}

void display(string pathname){
    if(pathname.empty()){
        cout << "\033[2J\033[1;1H";
        cout << "You are lost."<<endl << "Press left arrow key to go back or press q/Q to exit." <<endl;
        return;
    }
    filldirectorylistvector(pathname);
    cout << "\033[2J\033[1;1H";
    indexmapper.clear();
    char buf[256];
    getcwd(buf, sizeof(buf));
    currentdirectorypath = buf;
    for (int i = 0; (i < nrows)&&(i<directorylist.size()); i++){
        cout<<setw(30) << directorylist[i];
        struct stat info;
        stat(directorylist[i].c_str(), &info);
        char * modifiedtime = ctime(&info.st_mtim.tv_nsec);
        string permission = formatpermission(info.st_mode);
        string finalPermission = S_ISDIR(info.st_mode)==true?"d":"-";
        finalPermission.append(permission);
        formatsize(info.st_size);
        cout<<setw(10)<< getusername(info.st_uid);
        cout<<setw(10)<< getgroupname(info.st_gid);
        cout<< setw(30) << modifiedtime ;
        // cout <<endl;
        indexmapper[i] = directorylist[i];
    }
    currenttopindex = 0;
    currentbottomindex = nrows-1;
    currentbottomindex = currentbottomindex<directorylist.size()?currentbottomindex:directorylist.size();
    printf("\033[1;1H");
    cursorindex=currenttopindex;
    pathstack[pathstackcounter] = currentdirectorypath;
    pathstackcounter++;
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
    char buf[256];
    getcwd(buf, sizeof(buf));
    homedirectorypath = buf;
    pathstackcounter = 0;
    display(argv[1]);
    detectArrowKey();
    return 0;
}