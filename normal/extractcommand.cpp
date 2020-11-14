#include <unistd.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

using namespace std;

void copyutil(string destination, vector<string> filelist){
    
    for(string a : filelist){
        struct stat s;
        int sourcedescriptor = open(a.c_str(), O_RDONLY);
        string destinationlocation;
        stat(a.c_str(), &s);
        if(destination[destination.length()-1]=='/')
            destinationlocation = destination + a;
        else
            destinationlocation = destination + "/" + a;
        
        int destinationdescriptor = open(destinationlocation.c_str(), O_CREAT|O_TRUNC |O_WRONLY, S_IRUSR|S_IWUSR);
        int buffer[1024];
        int countread;
        while(countread=read(sourcedescriptor, buffer, sizeof(buffer))){
            write(destinationdescriptor, buffer, countread);
        }
        chmod(destinationlocation.c_str(), s.st_mode);
    }
    cout << "Copy Done";
}

void moveutil(string destination, vector <string> filelist){
    
    for(string a : filelist){
        struct stat s;
        int sourcedescriptor = open(a.c_str(), O_RDONLY);
        string destinationlocation;
        stat(a.c_str(), &s);
        if(destination[destination.length()-1]=='/')
            destinationlocation = destination + a;
        else
            destinationlocation = destination + "/" + a;
        
        int destinationdescriptor = open(destinationlocation.c_str(), O_CREAT|O_TRUNC |O_WRONLY, S_IRUSR|S_IWUSR);
        int buffer[1024];
        int countread;
        while(countread=read(sourcedescriptor, buffer, sizeof(buffer))){
            write(destinationdescriptor, buffer, countread);
        }
        chmod(destinationlocation.c_str(), s.st_mode);
        remove(a.c_str());
    }
    cout << "Move Done";
}


void renameutil(string o, string n){
    struct stat s;
    int sourcedescriptor = open(o.c_str(), O_RDONLY);
    string destinationlocation = n;
    stat(o.c_str(), &s);
    int destinationdescriptor = open(destinationlocation.c_str(), O_CREAT|O_TRUNC |O_WRONLY, S_IRUSR|S_IWUSR);
    int buffer[1024];
    int countread;
    while(countread=read(sourcedescriptor, buffer, sizeof(buffer))){
        write(destinationdescriptor, buffer, countread);
    }
    chmod(destinationlocation.c_str(), s.st_mode);
    remove(o.c_str());
    cout <<"Rename Done";
}

void create_file(string filename, string destinationfolder){
    string filelocation;
    if(destinationfolder[destinationfolder.length()-1] =='/'){
        filelocation = destinationfolder+filename;
    } else
        filelocation = destinationfolder + "/" + filename;
    
    int fdis = open(filelocation.c_str(), O_CREAT|O_TRUNC|O_WRONLY, S_IWUSR|S_IRUSR|S_IRGRP);
    if(fdis!=-1){
        cout << filename << ": File successfully created."<<endl;
    } else {
        cout << filename << ": File cannot be created" << endl;
    }
}
void create_dir(string foldername, string folderlocation){
    string filelocation;
    if(folderlocation[folderlocation.length()-1] =='/'){
        filelocation = folderlocation+foldername;
    } else
        filelocation = folderlocation + "/" + foldername;
    int k = mkdir(filelocation.c_str(), S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IXOTH);
    if(k==0){
        cout << "Folder created succesfully." << endl;
    } else {
        cout << "Folder could not be created." << endl;
    }
}

void gotoloc(string destinationpath){
    chdir(destinationpath.c_str());
}

string searchutil(string directoryPath, string filename){
    DIR * directorypointer;
    struct dirent *d;
    struct stat s;
    if((directorypointer=opendir(directoryPath.c_str()))!= NULL){
        chdir(directoryPath.c_str());
        while((d = readdir(directorypointer))!=NULL) {
            if(strcmp(filename.c_str(), d->d_name)==0){
                return "True";
            }
            stat(d->d_name, &s);
            if(S_ISDIR(s.st_mode)){
                if((strcmp(".", d->d_name)!=0)&&(strcmp("..", d->d_name)!=0)){
                    if(strcmp(searchutil(d->d_name, filename).c_str(), "True")==0)
                        return "True";
                }
            }
        }
        chdir("..");
        closedir(directorypointer);
    }
    return "False";
}

string start_search(string filename){
    char buff[256];
    getcwd(buff, sizeof(buff));
    return searchutil(buff, filename);
}

void splitcommand(string s){
    istringstream ss(s);
    string token;
    vector<string> vi;
    while(getline(ss, token, ' ')){
        vi.push_back(token);
    }
    vector<string>filelist (vi.begin()+1, vi.end()-1);
    string command = vi.front();
    string destination = vi.back();
    if(strcmp(command.c_str(), "copy")==0){
        copyutil(destination, filelist);
    } else if(strcmp(command.c_str(), "move")==0){
        moveutil(destination, filelist);
    } else if(strcmp(command.c_str(), "rename")==0){
        string oldname = vi[1];
        string newname = vi[2];
        renameutil(oldname, newname);
    } else if(strcmp(command.c_str(), "create_file")==0){
        string filename = vi[1];
        string destinationfolder = vi[2];
        create_file(filename, destinationfolder);
    } else if(strcmp(command.c_str(), "create_dir")==0){
        string foldername = vi[1];
        string destinationfolder = vi[2];
        create_dir(foldername, destinationfolder);
    } else if(strcmp(command.c_str(), "goto")==0){
        string destinationfolder = vi[1];
        gotoloc(destinationfolder);
    } else if(strcmp(command.c_str(), "search")==0){
        string file = vi[1];
        cout << start_search(file);
    }
}

int main(int argc, char const *argv[]){

    string str = "search getsize.cpp";
    splitcommand(str);
    return 0;
}
