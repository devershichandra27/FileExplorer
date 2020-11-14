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

using namespace std;

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

void printdir(string path){
    DIR * directorypointer;
    struct dirent * entry;
    
    if((directorypointer = opendir(path.c_str())) == NULL){
        write(2, "Cannot open directory.\n", 23);
        return;
    }
    chdir(path.c_str());
    vector <string> directorylist;
    while((entry = readdir(directorypointer))!= NULL){
        directorylist.push_back(entry->d_name);
    }
    sort(directorylist.begin(), directorylist.end());
    for(string s : directorylist){
        struct stat info;
        stat(s.c_str(), &info);
        char * modifiedTime = ctime(&info.st_mtim.tv_sec);
        string permission = formatpermission(info.st_mode);
        string finalPermission = S_ISDIR(info.st_mode)==true?"d":"-";
        finalPermission.append(permission);
        cout << s<< "\t"; formatsize(info.st_size); cout<<"\t"<< getusername(info.st_uid) << "\t"<< getgroupname(info.st_gid)  <<"\t"<<finalPermission << "\t" << modifiedTime;
    }
    //chdir("..");
}

int main(int argc, char const *argv[])
{
    if(argc!=2){
        cout << "Please provide valid url" << endl;
        return -1;
    }
    string p = argv[1];
    printdir(p);
    return 0;
}
