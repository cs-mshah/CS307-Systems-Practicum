#include<iostream>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<fstream>

using namespace std;

const string OUTPUT_FILE = "StateChangeLog.txt";
const string WAITING_STATE = "Waiting for Spoons";
const string ONE_SPOON_ACQUIRED_STATE = "One spoon acquired";
const string EATING_STATE = "Eating";
const string THINKING_STATE = "Thinking";
const int MAX_CYCLES_ALLOWED = 100;

const int MAX_THINKING_TIME = 2;
const int EATING_TIME = 20;

//const int EATING_TIME = 1;
//const int MAX_THINKING_TIME = 1;

const int NUM_SPOONS = 5;
const int NUM_STUDENTS = 5;
mutex spoonOperationsLock;
mutex studentLock;
mutex printLock;
condition_variable conditionVariable;

bool spoonStatus[NUM_SPOONS] = {false}; //Initializes all entries to false
struct Student {
    string state;
    int cyclesCompleted;

    Student() {
        state = WAITING_STATE;
        cyclesCompleted = 0;
    }
} studentStatus[NUM_STUDENTS];

int getRandomThinkingTime() {
    return rand()%(MAX_THINKING_TIME*1000); //get a random amount of time between 0 - 2000 (milliseconds)
}

void printStudentStatus() {
    ofstream logfile;
    logfile.open(OUTPUT_FILE, ios::app);
    logfile<<"\n\n";
    for(int i=0;i<NUM_STUDENTS;i++) {
        logfile<<"S"<<i+1<<" ("<<studentStatus[i].cyclesCompleted<<"): "<<studentStatus[i].state<<'\n';
    }
    logfile<<"\n-----------------";
    logfile.close();
}

bool canTakeSpoons(int threadID) {
    lock_guard<mutex> guard(spoonOperationsLock);

    int leftSpoon = threadID, rightSpoon = (threadID+1)%NUM_STUDENTS;
    if(spoonStatus[leftSpoon]==false && spoonStatus[rightSpoon]==false) {
        //If both left and right spoons are free, take both spoons and continue to eat
        spoonStatus[leftSpoon] = true;
        spoonStatus[rightSpoon] = true;
        cout<<"Student "<<threadID<<" has taken spoons: "<<leftSpoon<<" and "<<rightSpoon<<endl;
        return true;
    }
    return false;
}

void giveUpSpoons(int threadID) {
    int leftSpoon = threadID, rightSpoon = (threadID+1)%NUM_STUDENTS;
    spoonStatus[leftSpoon] = false;
    spoonStatus[rightSpoon] = false;
}

void student(int threadID) {
    int cyclesDone = 0;

    while(cyclesDone<MAX_CYCLES_ALLOWED) {
        unique_lock<mutex> uniqueLock(studentLock);
        conditionVariable.wait(uniqueLock, [threadID]{return canTakeSpoons(threadID);});

        studentStatus[threadID].state = EATING_STATE;
        printStudentStatus();

        uniqueLock.unlock();
        this_thread::sleep_for(chrono::seconds(EATING_TIME)); //EATING

        uniqueLock.lock();
        giveUpSpoons(threadID);
        cout<<"student "<<threadID<<" is done eating"<<endl;
        studentStatus[threadID].state = THINKING_STATE;
        printStudentStatus();
        uniqueLock.unlock();

        conditionVariable.notify_one();

        this_thread::sleep_for(chrono::milliseconds(getRandomThinkingTime()));

        cyclesDone++;

        uniqueLock.lock();
        studentStatus[threadID].cyclesCompleted = cyclesDone;
        studentStatus[threadID].state = WAITING_STATE;
        printStudentStatus();
        uniqueLock.unlock();
    }
}

int main() {
    cout<<"Starting the Dining Students program\n";

    //clearing LOG FILE
    ofstream logfile;
    logfile.open(OUTPUT_FILE, ofstream::out | ofstream::trunc);
    logfile.close();

    //creating one thread for each student
    thread threads[NUM_STUDENTS];
    int threadIDs[NUM_STUDENTS];
    for(int i=0;i<NUM_STUDENTS;i++) {
        threadIDs[i] = i;
        threads[i] = thread(student, threadIDs[i]);
    }
    for(int i=0;i<NUM_STUDENTS;i++) {
        threads[i].join();
    }
    return 0;
}


