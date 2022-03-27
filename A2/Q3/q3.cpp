#include <bits/stdc++.h>
#include "include/mythreads.h"

typedef unsigned char BYTE;

using namespace std;

int n; // size of array
int t; // number of threads
int h; // depth of recursions
int threadCount = 0; // number of threads made
pthread_mutex_t numThreadsLock = PTHREAD_MUTEX_INITIALIZER; // lock for counting number of threads
vector<pthread_t> th; // vector of threads
vector<BYTE> arr;

// UTILITY FUNCTIONS

// To print array
void printArray(int size)
{
    for (int i = 0; i < size; i++)
        cout << arr[i] << " ";
    cout << "\n\n";
}

void readFile(string filename, vector<BYTE> &vec)
{
    // open the file:
    ifstream file(filename, ios::binary);

    // Stop eating new lines in binary mode!!!
    file.unsetf(ios::skipws);

    // get its size:
    streampos fileSize;

    file.seekg(0, ios::end);
    fileSize = file.tellg();
    file.seekg(0, ios::beg);

    // reserve capacity
    vec.reserve(fileSize);

    // read the data:
    vec.insert(vec.begin(),
               istream_iterator<BYTE>(file),
               istream_iterator<BYTE>());
}

// To round of number of threads into nearest (least) power of 2
int roundNumberOfThreads(int m){
    return (1<<m)-1;
}

// Calculating depth of recursions
int heightOfFunc(int t){
    return floor(log(t)/log(2))-1;
}

// Structure to pass into threads
struct args {
    int threadID;
    // int parentID;
    int height;
    int begin;
    int end;
};

void merge(int const left, int const mid, int const right)
{
    auto const subArrayOne = mid - left + 1;
    auto const subArrayTwo = right - mid;
  
    // Create temp arrays
    auto *leftArray = new int[subArrayOne],
         *rightArray = new int[subArrayTwo];
  
    // Copy data to temp arrays leftArray[] and rightArray[]
    for (auto i = 0; i < subArrayOne; i++)
        leftArray[i] = arr[left + i];
    for (auto j = 0; j < subArrayTwo; j++)
        rightArray[j] = arr[mid + 1 + j];
  
    auto indexOfSubArrayOne = 0, // Initial index of first sub-array
        indexOfSubArrayTwo = 0; // Initial index of second sub-array
    int indexOfMergedArray = left; // Initial index of merged array
  
    // Merge the temp arrays back into array[left..right]
    while (indexOfSubArrayOne < subArrayOne && indexOfSubArrayTwo < subArrayTwo) {
        if (leftArray[indexOfSubArrayOne] <= rightArray[indexOfSubArrayTwo]) {
            arr[indexOfMergedArray] = leftArray[indexOfSubArrayOne];
            indexOfSubArrayOne++;
        }
        else {
            arr[indexOfMergedArray] = rightArray[indexOfSubArrayTwo];
            indexOfSubArrayTwo++;
        }
        indexOfMergedArray++;
    }
    // Copy the remaining elements of
    // left[], if there are any
    while (indexOfSubArrayOne < subArrayOne) {
        arr[indexOfMergedArray] = leftArray[indexOfSubArrayOne];
        indexOfSubArrayOne++;
        indexOfMergedArray++;
    }
    // Copy the remaining elements of
    // right[], if there are any
    while (indexOfSubArrayTwo < subArrayTwo) {
        arr[indexOfMergedArray] = rightArray[indexOfSubArrayTwo];
        indexOfSubArrayTwo++;
        indexOfMergedArray++;
    }
}

void mergeSort(int const begin, int const end)
{
    if (begin >= end)
        return; // Returns recursively
  
    auto mid = begin + (end - begin) / 2;
    mergeSort(begin, mid);
    mergeSort(mid + 1, end);
    merge(begin, mid, end);
}

// Mergsort function (threads)
void* func(void* arg)
{

    struct args *arrArg = (struct args *)(arg);
    int begin = arrArg->begin;
    int end = arrArg->end;
    int height = arrArg->height;
    //cout << "ThreadID: " << arrArg->threadID << " Begin: "<< arrArg->begin << " End: " << arrArg->end << " height: " << arrArg->height << endl; 

    if (height < h){
        
        auto mid = begin + (end - begin) / 2;
        
        Pthread_mutex_lock(&numThreadsLock);
        int tempThreadCount = threadCount;
        threadCount+=2;
        Pthread_mutex_unlock(&numThreadsLock);


        struct args *newArg = (struct args *)malloc(sizeof(struct args));
        newArg->threadID = tempThreadCount;
        newArg->height = height+1;
        newArg->begin = begin;
        newArg->end = mid;

        Pthread_create(&th[newArg->threadID], NULL, func, newArg);

        struct args *newArg2 = (struct args *)malloc(sizeof(struct args));
        newArg2->threadID = tempThreadCount+1;
        newArg2->height = height+1;
        newArg2->begin = mid+1;
        newArg2->end = end;
        Pthread_create(&th[newArg2->threadID], NULL, func, newArg2);

        Pthread_join(th[newArg->threadID], NULL);
        Pthread_join(th[newArg2->threadID], NULL);
        
        merge(begin, mid, end);
        //cout << arrArg->threadID << " doing merging here of threads "  << newArg->threadID << " and " << newArg2->threadID << endl;


        return NULL;
    }
    else{
        mergeSort(begin, end);
        //cout << "Doing normal mergesort here with ThreadID: " << arrArg->threadID << "\n";
        return NULL;
    }
}

int main(int argc, char *argv[])
{
    assert(argc==4); // check for exactly 4 arguments

    // Assigning n and t
    t = stoi(argv[1]);
    string in_path = argv[2], out_file = argv[3];
    
    // read file into memory
    readFile(in_path, arr);
    n = arr.size();
    cout<<n<<"\n";
    h = heightOfFunc(t); // Calculating depth of recursion of threads
    t = roundNumberOfThreads(h+1); // Number of threads made (2^m -1)
    th.resize(t); // Creating t threads

    threadCount += 1;
    struct args *start = (struct args *)malloc(sizeof(struct args));
    // start->parentID = -1;
    start->threadID = 0;
    start->height = 0;
    start->begin = 0;
    start->end = n-1;

    auto start_time = chrono::steady_clock::now();
    pthread_t parent; // Creating parent thread
    Pthread_create(&parent, NULL, func, start);
    Pthread_join(parent, NULL);
    auto end_time = chrono::steady_clock::now();

    cout << "Elapsed time in milliseconds: "
         << chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count()
         << " sec";

    return 0;
}