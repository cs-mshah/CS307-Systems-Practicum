#include<iostream>
#include<sstream>
#include <functional>
#include<map>
#include<string>
#include<signal.h>
#include<unistd.h>
#include<fstream>
#include<dirent.h>
#include<sys/types.h>
#include <sys/wait.h>
#include<vector>

using namespace std;

extern char **environ; 
// store environment variables
map<string, string> localEnvironmentVariables;
// store help text for each supported command
map<string, function<void()>> commandExecutor;
string userInput;
vector<string> userArgs;

string parseCommand(string userInput){
    // returns the first string as the command and stores the args in a vector
    string command, arg;
    userArgs.clear();
    stringstream argStream(userInput);
    argStream >> command;
    while(argStream >> arg){
        userArgs.push_back(arg);
    }
    return command;
}

void ignoreAllSignals(void){
  // ignore all signals received
    for(int i = 1; i < 65; i++){
      signal(i, SIG_IGN);
    }
}

void resumeAllSignals(void){
  // ignore all signals received
    for(int i = 1; i < 65; i++){
      signal(i, SIG_DFL);
    }
}

// 1. clr
void clearScreen(){
    cout << "\033[2J\033[1;1H";
}

// 2. pause
void executePause(){
    cout << "Press enter to continue\n";
    ignoreAllSignals();
    while(cin.get() != '\n');
    resumeAllSignals();
}

// 3. help
void displayHelp(){
    cout << "help" << " - " << "display user manual\n";
    cout << "clr" << " - " << "clear the screen\n";
    cout << "pause" << " - " << "Pause operations of the shell until ENTER is pressed\n";
    cout << "quit/CTRL-D" << " - " << "Quit the shell\n";
    cout << "history" << " - " << "Display the list of previously executed commands, even on shell restart\n";
    cout << "cd <directory>" << " - " << "Change the current default directory to <directory>. If the <directory> argument is not present, report the current directory\n";
    cout << "dir <directory>" << " - " << "List all the contents of the directory <directory>\n";
    cout << "environ" << " - " << "List all the environment strings of the current shell and the bash shell\n";
    cout << "echo <comment>" << " - " << "Display <comment> on the display followed by a new line\n";
    cout << "myshell <batchfile>" << " - " << "Execute commands listed in <batchfile>\n";
}

// 4. quit
void quitShell(){
    exit(0);
}

// 5. history
void updateHistory(string command){
    if(command.length() == 0)
        return;
    const string historyFile = localEnvironmentVariables["history_dump"];
    ofstream historyStream(historyFile, ios::app);
    historyStream << command + "\n";
    historyStream.close();
}

void displayHistory(){
    const string historyFile = localEnvironmentVariables["history_dump"];
    ifstream historyStream(historyFile);
    string command;
    while(getline(historyStream, command)){
        cout << command << "\n";
    }
}


// 10. 
void displayPWD(){
    cout << getenv("PWD") << "\n";
}


// 6. chdir
void changeDirectory(){
    if(userArgs.empty()){
        cout << "dir: ";
        displayPWD();
        return;
    }

    if(userArgs.size() > 1){
        cout << "Too many args\n";
        return;
    }

    char* newDirPath = realpath(userArgs[0].c_str(), NULL);
    
    if(newDirPath)
    {
        if(chdir(newDirPath) == -1){
            // failed
            cout << "cd failed\n";
        }
        else{
            // success
            setenv("PWD", newDirPath, 1);
        }
    }
    else
    {
        cout << "invalid path\n";
    }
}

// 7. dir
void listDirectory(){
    if(userArgs.size() > 1){
        cout << "Too many args\n";
        return;
    }

    char* dirPath = realpath(userArgs[0].c_str(), NULL);
    if(userArgs.empty()){
        dirPath = getenv("PWD");
    }
    
    if(dirPath){
        DIR* directory = opendir(dirPath);
        struct dirent* currentFile;
        if(directory){
            while(currentFile = readdir(directory)){
                cout << currentFile->d_name << "\n";
            }
        }
        closedir(directory);
    }
    else{
        cout << "invalid directory\n";
    }
}

// 8. environ
void printEnviron(){

    char** var = environ;
    while (*var != NULL)
    {
        cout << *var << "\n";
        var++;
    }

    for(auto &var : localEnvironmentVariables){
        cout << var.first << "=" << var.second << "\n";
    }
}

// 9. echo
void executeEcho(){
    string arg;
    for(auto &arg : userArgs){
        cout << arg << " ";
    }
    cout << "\n";
}

// 12. Execute programs
void executeProg(const char* filename){
    pid_t pid;

    pid = fork();
    if(pid < 0){
        cout << "Fork failed. Cannot execute the program.\n";
    }
    else if(pid == 0){
        // child process
        // prepare args for execvp
        const char** args = new const char* [userArgs.size() + 2];
        args[0] = filename;
        for(int i = 0; i < userArgs.size(); i++){
            args[i + 1] = userArgs[i].c_str();
        }
        args[userArgs.size() + 1] = NULL;

        if(execvp(args[0], (char **)args) < 0){
            cout << "invalid executable\n";
        }
        return;
    }
    else{
        wait(NULL);
    }
    return;
}


// 11. myshell

void executeBatchFile(const char* filename){
    const char* filepath = realpath(filename, NULL);
    if(filepath){
        ifstream batchStream(filepath);
        string commandLine;
        while(getline(batchStream, commandLine)){
            string command = parseCommand(commandLine);
            if(command.empty())
                continue;
            updateHistory(commandLine);
            if(commandExecutor.find(command) != commandExecutor.end()){
                commandExecutor[command]();
            }
            else{
                executeProg(command.c_str());
            }
        }

    }
    else{
        cout << "batch file does not exist\n";
    }
}

void executeMyshell(){
    if(userArgs.size() > 1){
        cout << "Too many args\n";
        return;
    }

    executeBatchFile(userArgs[0].c_str());
}

void initInternalCommands(){
    commandExecutor["clr"] = clearScreen;
    commandExecutor["help"] = displayHelp;
    commandExecutor["pause"] = executePause;
    commandExecutor["quit"] = quitShell;
    commandExecutor["history"] = displayHistory;
    commandExecutor["environ"] = printEnviron;
    commandExecutor["echo"] = executeEcho;
    commandExecutor["pwd"] = displayPWD;
    commandExecutor["cd"] = changeDirectory;
    commandExecutor["dir"] = listDirectory;
    commandExecutor["myshell"] = executeMyshell;
}

void initEnvironVariables(string shellPath){
    localEnvironmentVariables["history_dump"] = "/home/harnaman24/workarea/courses/cs307_sysprac/assgn1/shell_history.txt";
    localEnvironmentVariables["shell_prompt"] = "myshell>> ";
    localEnvironmentVariables["shell"] = shellPath;    
}

void displayPrompt(){
    cout << localEnvironmentVariables["shell_prompt"];
}

int main(int argc, char **argv){

    initInternalCommands();
    initEnvironVariables(realpath(argv[0], NULL));

    if(argc > 2){
        cout << "Too many args supplied\n";
        quitShell();
    }

    if(argc == 2){
        //batchfile supplied
        executeBatchFile(argv[1]);
        quitShell();
    }

    while(true){
        displayPrompt();

        getline(cin, userInput);
        if(cin.eof()){
            quitShell();
        }
        string command = parseCommand(userInput);
        if(command.empty())
            continue;
        updateHistory(userInput);
        if(commandExecutor.find(command) != commandExecutor.end()){
            commandExecutor[command]();
        }
        else{
            executeProg(command.c_str());
        }
    }

    return 0;
}