#include <bits/stdc++.h>

using namespace std;

void print(vector<vector<int> > &A, char c)
{
	int n = A.size();
	cout<<c<<"\n";
	for(int i=0;i<n;i++)
	{
		for(int j=0;j<n;j++)
		{
			cout<<A[i][j]<<" ";
		}
		cout<<"\n";
	}
}

void multiply(vector<vector<int> > &A, vector<vector<int> > &B, vector<vector<int> > &C)
{
	int n = A.size();
	for(int i=0;i<n;i++)
	{
		for(int j=0;j<n;j++)
		{
			for(int k=0;k<n;k++)
			{
				C[i][j] += A[i][k]*B[k][j];
			}
		}
	}
}

int main(int argc, char *argv[])
{
	assert(argc==2); // check for exactly 2 arguments

	int n = stoi(argv[1]);
	//cout<<n<<"\n";

	vector<vector<int> > A(n, vector<int> (n)), B(n, vector<int> (n)),C(n, vector<int> (n)) ;

    srand(time(NULL));
    for(int i=0;i<n;i++)
    {
    	for(int j=0;j<n;j++)
	    {
	    	A[i][j] = rand() % 50;
	    	B[i][j] = rand() % 50;
	    }
    }
    multiply(A, B, C);
    /*print(A, 'A');
    print(B, 'B');
    print(C, 'C');*/

	return 0;
}