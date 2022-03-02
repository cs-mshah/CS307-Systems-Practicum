#include <bits/stdc++.h>
#include <pthread.h>
#include "include/mythreads.h"

using namespace std;

int n; // size of matrices
vector<vector<int> > A, B, C;

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

void *multiply(void *arg)
{
	long int num = (long int)(arg);
	
	for(int i=num;i<num+1;i++)
	{
		for(int j=0;j<n;j++)
		{
			for(int k=0;k<n;k++)
			{
				C[i][j] += A[i][k] * B[k][j];
			}
		}
	}
}

int main(int argc, char *argv[])
{
	assert(argc==2); // check for exactly 2 arguments

	n = stoi(argv[1]);
	//cout<<n<<"\n";

	A.assign(n, vector<int> (n,0));
	B.assign(n, vector<int> (n,0));
	C.assign(n, vector<int> (n,0));

    srand(time(NULL));
    for(int i=0;i<n;i++)
    {
    	for(int j=0;j<n;j++)
	    {
	    	A[i][j] = rand() % 50;
	    	B[i][j] = rand() % 50;
	    }
    }
    
	vector<pthread_t> th(n); // create n threads. each one computing C_i

	for(int i = 0; i < n; i++)
    {
        Pthread_create(&th[i], NULL, multiply, (void *)i);
    }
    for(int i = 0; i < n; i++)
    {
        Pthread_join(th[i], NULL);
    }

    /*print(A, 'A');
    print(B, 'B');
    print(C, 'C');*/

	return 0;
}