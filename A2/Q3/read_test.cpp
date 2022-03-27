#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>

typedef unsigned char BYTE;

using namespace std;

vector<BYTE> bytes;

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

int main()
{
	readFile("binary.dat", bytes);

    cout<<bytes.size()<<"\n";
    for(auto c:bytes)
    {
    	cout<<c<<"\n";
    }

	return 0;
}