/*Improved formattedoutput.cpp. Accepting commands using colon key*/
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <dirent.h>
#include <sys/stat.h>
#include <iostream>
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

vector<string> directorylist;
int nrows = 10;
int currenttopindex = 0;
int currentbottomindex = 0;
bool scroll = true;
int cursorindex = -1;
map<int, string> indexmapper;
string homedirectorypath;
string currentdirectorypath;
deque<string> pathstack(1000);
int pathstackcounter, maxstacksize;
string prevpath;
string nextpath;

void copyutil(string destination, vector<string> filelist)
{

    for (string a : filelist)
    {
        struct stat s;
        int sourcedescriptor = open(a.c_str(), O_RDONLY);
        string destinationlocation;
        stat(a.c_str(), &s);
        if (destination[destination.length() - 1] == '/')
            destinationlocation = destination + a;
        else
            destinationlocation = destination + "/" + a;

        int destinationdescriptor = open(destinationlocation.c_str(), O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
        int buffer[1024];
        int countread;
        while (countread = read(sourcedescriptor, buffer, sizeof(buffer)))
        {
            write(destinationdescriptor, buffer, countread);
        }
        chmod(destinationlocation.c_str(), s.st_mode);
    }
    cout << "Copy Done";
}

void moveutil(string destination, vector<string> filelist)
{

    for (string a : filelist)
    {
        struct stat s;
        int sourcedescriptor = open(a.c_str(), O_RDONLY);
        string destinationlocation;
        stat(a.c_str(), &s);
        if (destination[destination.length() - 1] == '/')
            destinationlocation = destination + a;
        else
            destinationlocation = destination + "/" + a;

        int destinationdescriptor = open(destinationlocation.c_str(), O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
        int buffer[1024];
        int countread;
        while (countread = read(sourcedescriptor, buffer, sizeof(buffer)))
        {
            write(destinationdescriptor, buffer, countread);
        }
        chmod(destinationlocation.c_str(), s.st_mode);
        remove(a.c_str());
    }
    cout << "Move Done";
}

void renameutil(string o, string n)
{
    struct stat s;
    int sourcedescriptor = open(o.c_str(), O_RDONLY);
    string destinationlocation = n;
    stat(o.c_str(), &s);
    int destinationdescriptor = open(destinationlocation.c_str(), O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
    int buffer[1024];
    int countread;
    while (countread = read(sourcedescriptor, buffer, sizeof(buffer)))
    {
        write(destinationdescriptor, buffer, countread);
    }
    chmod(destinationlocation.c_str(), s.st_mode);
    remove(o.c_str());
    cout << "Rename Done";
}

void create_file(string filename, string destinationfolder)
{
    string filelocation;
    if (destinationfolder[destinationfolder.length() - 1] == '/')
    {
        filelocation = destinationfolder + filename;
    }
    else
        filelocation = destinationfolder + "/" + filename;

    int fdis = open(filelocation.c_str(), O_CREAT | O_TRUNC | O_WRONLY, S_IWUSR | S_IRUSR | S_IRGRP);
    if (fdis != -1)
    {
        cout << filename << ": File successfully created." << endl;
    }
    else
    {
        cout << filename << ": File cannot be created" << endl;
    }
}
void create_dir(string foldername, string folderlocation)
{
    string filelocation;
    if (folderlocation[folderlocation.length() - 1] == '/')
    {
        filelocation = folderlocation + foldername;
    }
    else
        filelocation = folderlocation + "/" + foldername;
    int k = mkdir(filelocation.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH);
    if (k == 0)
    {
        cout << "Folder created succesfully." << endl;
    }
    else
    {
        cout << "Folder could not be created." << endl;
    }
}

void removedir(char *dir)
{
    DIR *dp;
    struct dirent *entry;
    struct stat statbuff;

    if ((dp = opendir(dir)) != NULL)
    {
        chdir(dir);
        while ((entry = readdir(dp)) != NULL)
        {
            lstat(entry->d_name, &statbuff);
            if (S_ISDIR(statbuff.st_mode))
            {
                if ((strcmp(".", entry->d_name) == 0) || strcmp("..", entry->d_name) == 0)
                {
                    continue;
                }
                removedir(entry->d_name);
            }
            remove(entry->d_name);
        }
        chdir("..");
        rmdir(dir);
    }
}

void gotoloc(string destinationpath)
{
    chdir(destinationpath.c_str());
}

string searchutil(string directoryPath, string filename)
{
    DIR *directorypointer;
    struct dirent *d;
    struct stat s;
    if ((directorypointer = opendir(directoryPath.c_str())) != NULL)
    {
        chdir(directoryPath.c_str());
        while ((d = readdir(directorypointer)) != NULL)
        {
            if (strcmp(filename.c_str(), d->d_name) == 0)
            {
                return "True";
            }
            stat(d->d_name, &s);
            if (S_ISDIR(s.st_mode))
            {
                if ((strcmp(".", d->d_name) != 0) && (strcmp("..", d->d_name) != 0))
                {
                    if (strcmp(searchutil(d->d_name, filename).c_str(), "True") == 0)
                        return "True";
                }
            }
        }
        chdir("..");
        closedir(directorypointer);
    }
    return "False";
}

string start_search(string filename)
{
    char buff[256];
    getcwd(buff, sizeof(buff));
    return searchutil(buff, filename);
}

void splitcommand(string s)
{
    istringstream ss(s);
    string token;
    vector<string> vi;
    while (getline(ss, token, ' '))
    {
        vi.push_back(token);
    }
    vector<string> filelist(vi.begin() + 1, vi.end() - 1);
    string command = vi.front();
    string destination = vi.back();
    if (strcmp(command.c_str(), "copy") == 0)
    {
        copyutil(destination, filelist);
    }
    else if (strcmp(command.c_str(), "move") == 0)
    {
        moveutil(destination, filelist);
    }
    else if (strcmp(command.c_str(), "rename") == 0)
    {
        string oldname = vi[1];
        string newname = vi[2];
        renameutil(oldname, newname);
    }
    else if (strcmp(command.c_str(), "create_file") == 0)
    {
        string filename = vi[1];
        string destinationfolder = vi[2];
        create_file(filename, destinationfolder);
    }
    else if (strcmp(command.c_str(), "create_dir") == 0)
    {
        string foldername = vi[1];
        string destinationfolder = vi[2];
        create_dir(foldername, destinationfolder);
    }
    else if (strcmp(command.c_str(), "delete_dir"))
    {
        string foldername = vi[1];
        char *folderlocation = new char[512];
        strcpy(folderlocation, foldername.c_str());
        removedir(folderlocation);
    } else if (strcmp(command.c_str(), "delete_file")){
        string filename = vi[1];
        char * file = new char[512];
        strcpy(file, filename.c_str());
        remove(file);
    }
    else if (strcmp(command.c_str(), "goto") == 0)
    {
        string destinationfolder = vi[1];
        gotoloc(destinationfolder);
    }
    else if (strcmp(command.c_str(), "search") == 0)
    {
        string file = vi[1];
        cout << start_search(file);
    }
}

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

void detectArrowKey()
{
    while (true)
    {
        int ch = inputchar();
        if (ch == 27)
        {
            ch = inputchar();
            if (ch == 91)
            {
                ch = inputchar();
                switch (ch)
                {
                case 65:
                    if (((cursorindex - 1) >= currenttopindex) && ((cursorindex - 1) <= currentbottomindex))
                    {
                        printf("\033[1A");
                        cursorindex--;
                    }
                    break;
                case 66:
                    if (((cursorindex + 1) >= currenttopindex) && ((cursorindex + 1) <= currentbottomindex))
                    {
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
                    if (pathstackcounter > 1)
                    {
                        pathstackcounter -= 2;
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
        }
        else if (ch == 58)
        {
            //Colon pressed
            printf("\033[%d;1H", nrows);
            string command;
            getline(cin, command);
            printf("\033[%d;1H", nrows);
            splitcommand(command);
        }
        else if (ch == 72 || ch == 104)
        {
            display(homedirectorypath);
        }
        else if (ch == 107 || ch == 75)
        {
            pagedown();
        }
        else if (ch == 73 || ch == 105)
        {
            pageup();
        }
        else if (ch == 127)
        {
            //cout << "Backspace Pressed"<<endl;
            display("..");
        }
        else if (ch == '\n')
        {
            int key = cursorindex;
            printf("\033[1;1H");
            cursorindex = currenttopindex;
            enterkeypressed(key);
        }
        else if (ch == 81 || ch == 113)
        {
            //q Q pressed.
            cout << "\033[2J\033[1;1H";
            cout << "Explorer exited." << endl;
            return;
        }
    }
}

void pageup()
{
    if ((currenttopindex == 0) && scroll)
        return;
    currenttopindex = currenttopindex - (nrows - 1);
    currenttopindex = currenttopindex > 0 ? currenttopindex : 0;
    currentbottomindex = currenttopindex + (nrows - 1);
    currentbottomindex = currentbottomindex < (directorylist.size() - 1) ? currentbottomindex : (directorylist.size() - 1);
    cout << "\033[2J\033[1;1H";
    indexmapper.clear();
    for (int i = currenttopindex; i <= currentbottomindex; i++)
    {
        cout << setw(30) << directorylist[i];
        struct stat info;
        stat(directorylist[i].c_str(), &info);
        string modifiedtime = ctime(&info.st_mtime);
        string permission = formatpermission(info.st_mode);
        string finalPermission = S_ISDIR(info.st_mode) == true ? "d" : "-";
        finalPermission.append(permission);
        formatsize(info.st_size);
        cout << setw(15) << finalPermission;
        cout << setw(10) << getusername(info.st_uid);
        cout << setw(10) << getgroupname(info.st_gid);
        cout << setw(30) << modifiedtime;
        indexmapper[i] = directorylist[i];
    }
    printf("\033[1;1H");
    cursorindex = currenttopindex;
}

void pagedown()
{
    if ((currentbottomindex == (directorylist.size() - 1)) && scroll)
        return;
    currentbottomindex = currentbottomindex + (nrows - 1);
    currentbottomindex = currentbottomindex > (directorylist.size() - 1) ? (directorylist.size() - 1) : currentbottomindex;
    currenttopindex = currentbottomindex - (nrows - 1);
    currenttopindex = currenttopindex > 0 ? currenttopindex : 0;
    cout << "\033[2J\033[1;1H";
    indexmapper.clear();
    for (int i = currenttopindex; i <= currentbottomindex; i++)
    {
        cout << setw(30) << directorylist[i];
        struct stat info;
        stat(directorylist[i].c_str(), &info);
        string modifiedtime = ctime(&info.st_mtime);
        string permission = formatpermission(info.st_mode);
        string finalPermission = S_ISDIR(info.st_mode) == true ? "d" : "-";
        finalPermission.append(permission);
        formatsize(info.st_size);
        cout << setw(15) << finalPermission;
        cout << setw(10) << getusername(info.st_uid);
        cout << setw(10) << getgroupname(info.st_gid);
        cout << setw(30) << modifiedtime;
        indexmapper[i] = directorylist[i];
    }
    printf("\033[1;1H");
    cursorindex = currenttopindex;
}

void displayfile(string filename)
{
    int pid = fork();
    if (pid)
    {
        //parent
        int restat;
        waitpid(pid, &restat, 0);
    }
    else
    {
        char *fname = (char *)malloc(filename.length() + 1);
        strcpy(fname, filename.c_str());
        char *const a[] = {"vi", fname, NULL};
        execvp("vi", a);
    }
}

void enterkeypressed(int k)
{
    string name = indexmapper[k];
    struct stat s;
    lstat(name.c_str(), &s);
    if (S_ISDIR(s.st_mode))
    {
        display(name);
    }
    else
    {
        displayfile(name);
    }
}

string formatpermission(mode_t p)
{
    bitset<9> x(p);
    string str = x.to_string();
    string result = "";
    int counter = 0;
    while (counter < 3)
    {
        if (str[0 + (3 * counter)] == '1')
            result.append(string(1, 'r'));
        else
            result.append(string(1, '-'));
        if (str[1 + (3 * counter)] == '1')
            result.append(string(1, 'w'));
        else
            result.append(string(1, '-'));
        if (str[2 + (3 * counter)] == '1')
            result.append(string(1, 'x'));
        else
            result.append(string(1, '-'));
        counter++;
    }
    return result;
}

string getusername(uid_t u)
{
    struct passwd *p;
    p = getpwuid(u);
    return p->pw_name;
}

string getgroupname(gid_t g)
{
    struct group *p;
    p = getgrgid(g);
    return p->gr_name;
}

void formatsize(off_t s)
{
    string result = "";
    if (s < 1024)
    {
        cout << setw(8) << s;
        cout << setw(2) << " B";
        return;
    }
    float q = s / 1024;
    if (q < 1024)
    {
        q = (int)(q * 100 + 0.5);
        q = (float)q / 100;
        cout << setw(7) << q;
        cout << setw(3) << " KB";
        return;
    }
    q = q / 1024;
    if (q < 1024)
    {
        q = (int)(q * 100 + 0.5);
        q = (float)q / 100;
        cout << setw(7) << q;
        cout << setw(3) << " MB";
        return;
    }
    q = q / 1024;
    if (q < 1024)
    {
        q = (int)(q * 100 + 0.5);
        q = (float)q / 100;
        cout << setw(7) << q;
        cout << setw(3) << " GB";
        return;
    }
    q = q / 1024;
    q = (int)(q * 100 + 0.5);
    q = (float)q / 100;
    cout << setw(7) << q;
    cout << setw(3) << " TB";
}

void filldirectorylistvector(string path)
{
    DIR *directorypointer;
    struct dirent *entry;
    directorylist.clear();
    if ((directorypointer = opendir(path.c_str())) == NULL)
    {
        write(2, "Cannot open directory.\n", 23);
        return;
    }
    chdir(path.c_str());
    while ((entry = readdir(directorypointer)) != NULL)
    {
        directorylist.push_back(entry->d_name);
    }
    sort(directorylist.begin(), directorylist.end());
}

void display(string pathname)
{
    if (pathname.empty())
    {
        cout << "\033[2J\033[1;1H";
        cout << "You are lost." << endl
             << "Press left arrow key to go back or press q/Q to exit." << endl;
        return;
    }
    filldirectorylistvector(pathname);
    cout << "\033[2J\033[1;1H";
    indexmapper.clear();
    char buf[256];
    getcwd(buf, sizeof(buf));
    currentdirectorypath = buf;
    for (int i = 0; (i < nrows) && (i < directorylist.size()); i++)
    {
        cout << setw(30) << directorylist[i];
        struct stat info;
        stat(directorylist[i].c_str(), &info);
        string modifiedtime = ctime(&info.st_mtime);
        string permission = formatpermission(info.st_mode);
        string finalPermission = S_ISDIR(info.st_mode) == true ? "d" : "-";
        finalPermission.append(permission);
        formatsize(info.st_size);
        cout << setw(15) << finalPermission;
        cout << setw(10) << getusername(info.st_uid);
        cout << setw(10) << getgroupname(info.st_gid);
        cout << setw(30) << modifiedtime;
        // cout <<endl;
        indexmapper[i] = directorylist[i];
    }
    currenttopindex = 0;
    currentbottomindex = nrows - 1;
    currentbottomindex = currentbottomindex < directorylist.size() ? currentbottomindex : directorylist.size();
    printf("\033[1;1H");
    cursorindex = currenttopindex;
    pathstack[pathstackcounter] = currentdirectorypath;
    pathstackcounter++;
}
void resizehandler(int k)
{
    if (k == 28)
    {
        struct winsize w;
        ioctl(0, TIOCGWINSZ, &w);
        nrows = w.ws_row - 1;
        scroll = false;
    }
}

int main(int argc, char const *argv[])
{
    signal(28, resizehandler);
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    nrows = (w.ws_row) - 1;
    char buf[256];
    getcwd(buf, sizeof(buf));
    homedirectorypath = buf;
    pathstackcounter = 0;
    display(argv[1]);
    detectArrowKey();
    return 0;
}