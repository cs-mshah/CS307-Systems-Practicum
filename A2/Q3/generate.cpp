#include <bits/stdc++.h>

using namespace std;

const long long N = 2 * 1024 * 1024;

int random_number(int a, int b) {

    return a + (b - a)*((float)rand())/((float)RAND_MAX);

}

int main(int argc, char* argv[]) {

    srand(time(NULL));

    cout << "size_t is limted to " << numeric_limits<size_t>::max() << " bytes (" << numeric_limits<size_t>::max() /1024/1024<<" MB)"<< endl;
    cout << "streamsize is limited to " << numeric_limits<streamsize>::max() << " bytes (" << numeric_limits<streamsize>::max() / 1024 / 1024/1024/104 << " TB)" << endl;

    static char mbloc[1024*1024]; // 1MB 
    
    const size_t blocs = 2*1024; // 2GB
    ofstream os("binary.dat"); 
    for(int i = 0; i < blocs && os; i++) {
        for(int j=0; j< 1024*1024; j++)
        {
            char r = random_number(0, 127);
            mbloc[j] = r;
        }
        os.write(mbloc, sizeof(mbloc));
        if(i % 512)
            cout << "tellp() -> " << os.tellp() << "\r";
    }
    cout << endl;
    os.seekp(0, ios::end); 
    cout << os.tellp() << " end"<<endl;  
    os.close();
    cout << "File generation finished"; 

    return 0;

}