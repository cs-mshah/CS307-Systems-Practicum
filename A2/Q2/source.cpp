#include<bits/stdc++.h>
#include <unistd.h>      // for fork()
#include <string.h>      // for strerror()
#include <sys/wait.h>
#include <sys/types.h>
#include<functional>
#include <stdlib.h>

using namespace std;

int READ_END = 0, WRITE_END = 1;
map<char, char> charMap;

int n_children = 4;
vector<int[2]> child_fds; // each fd as fd_read, fd_write

string encrypt(string txt){
    string result = txt;
    for(int i = 0; i < txt.length(); i++){
        if(charMap.find(txt[i]) != charMap.end()){
            result[i] = charMap[txt[i]];
        }
    }
    return result;
}

void child_one(void){
    close(child_fds[0][0]); // Close reading end of pipe

    string txt;
    while(!cin.eof()){
        getline(cin, txt);
        write(child_fds[0][1], encrypt(txt).c_str(), txt.length());
        cout << "child: " << encrypt(txt) << "\n";
        //close(child_fds[0][1]); // Close writing end of pipe
    }

    cout << "exit child 1\n";
    exit(0);
}

void child_two(void){
    const char* filepath = realpath("random.txt", NULL);
    if(filepath){
        ifstream fileStream(filepath);
        string line;
        while(getline(fileStream, line)){
            const char** args = new const char*[3];
            string cmd = "espeak \"" + line + "\"";
            // cout << "child2 : " << cmd;
            int x = system(cmd.c_str());
        }
        cout << "child 2: all done\n";
    }
    else{
        cout << "invalid file\n";
    }

    exit(0);
}

void child_three(void){
    string copyFile = "copythis.txt", newFile = "new_" + copyFile;
    const char* filepath = realpath(copyFile.c_str(), NULL);
    if(filepath){
        ifstream fileStream(filepath);
        ofstream fileCopyStream(newFile);
        string line;
        while(getline(fileStream, line)){
            // cout << "child3: " << line << "\n";
            fileCopyStream << line << "\n";
            // write(child_fds[2][1], line.c_str(), line.length());
            // close(child_fds[2][1]); // Close writing end of pipe
        }
        cout << "child 3: all done\n";
    }
    else{
        cout << "invalid file\n";
    }
    exit(0);
}

void child_four(void){
    static unsigned long long lastTotalUser, lastTotalUserLow, lastTotalSys, lastTotalIdle;
    unsigned long long totalUser, totalUserLow, totalSys, totalIdle, total;
    double percent;
    FILE* file = fopen("/proc/stat", "r");
    fscanf(file, "cpu %llu %llu %llu %llu", &lastTotalUser, &lastTotalUserLow,
        &lastTotalSys, &lastTotalIdle);
    fclose(file);
    sleep(1);
    while(wait(NULL)<0){
        file = fopen("/proc/stat", "r");
        fscanf(file, "cpu %llu %llu %llu %llu", &totalUser, &totalUserLow,
        &totalSys, &totalIdle);
        fclose(file);
        if (totalUser < lastTotalUser || totalUserLow < lastTotalUserLow ||
        totalSys < lastTotalSys || totalIdle < lastTotalIdle){
        //Overflow detection. 
        percent = -1.0;
    }
    else{
        total = (totalUser - lastTotalUser) + (totalUserLow - lastTotalUserLow) +
            (totalSys - lastTotalSys);
        percent = total;
        total += (totalIdle - lastTotalIdle);
        percent /= total;
        percent *= 100;
    }

    lastTotalUser = totalUser;
    lastTotalUserLow = totalUserLow;
    lastTotalSys = totalSys;
    lastTotalIdle = totalIdle;

    // cout << "Child Four:" << percent << endl;
    string txt =  to_string(percent);
    write(child_fds[0][1], txt.c_str(), txt.length());
    close(child_fds[0][1]); // Close writing end of pipe
    sleep(2);
    }
    cout << "Child 4 done :)"<<endl;

}

int init_child(function<void()> func, int index){
    pid_t pid = fork();
    if(pid == 0){
        // child
        func();
        cout << "exit child\n";
        exit(0);
    }
    else{
        close(child_fds[index][WRITE_END]);
        return pid;
    }
}

void init(){

    // read mapping conf
    const char* filepath = realpath("mapping.txt", NULL);
    if(filepath){
        ifstream fileStream(filepath);
        char ch, ch_map;
        while(fileStream >> ch){
            fileStream >> ch_map;
            charMap[ch] = ch_map;
        }        
    }
    else{
        cout << "invalid file\n";
    }

    // children
    child_fds = vector<int[2]>(n_children);  
    for(int i = 0; i < n_children; i++){
        pipe(child_fds[i]);
    }
    int pid4 = init_child(child_four, 3);
    int pid3 = init_child(child_three, 2);
    int pid2 = init_child(child_two, 1);
    int pid1 = init_child(child_one, 0);

    // parent
    pid_t wpid;
    int status = 0;
    fd_set fdset;
    int max_fd;
    char buffer[1024];

    while (true){
        cout << "---\n";
        FD_ZERO(&fdset);

        //select(max_fd + 1, &fdset, NULL, NULL, NULL);

        // if(FD_ISSET(child_fds[0][0], &fdset))
        {
            int status = read(child_fds[0][READ_END], buffer, 1024);
            if(status > 0)
                cout << "c1: " <<  buffer << "\n";
        }

        // status = read(child_fds[1][READ_END], buffer, 1024);
        // if(status > 0)
        //     cout << "c2: " <<  buffer << "\n";

        // status = read(child_fds[2][READ_END], buffer, 1024);
        // if(status > 0)
        //     cout << "c3: " <<  buffer << "\n";

        // status = read(child_fds[3][READ_END], buffer, 1024);
        // if(status > 0)
        //     cout << "c4: " <<  buffer << "\n";

        sleep(5);
    }

    cout << "all parent\n";
    wait(NULL);
}

int main(){
    init();

    return 0;
}