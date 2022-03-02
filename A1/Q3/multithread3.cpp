#include <bits/stdc++.h>
#include <pthread.h>
#include "include/mythreads.h"

using namespace std;

int n; // size of matrices
vector<vector<int> > A, B, C;

pthread_mutex_t Lock = PTHREAD_MUTEX_INITIALIZER;

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
    // printf("%ld\n",num);
    int ifrom = (num % 2) * (n / 2);
    // printf("%d\n",ifrom);
    int isteps = 0;
    int jfrom = (num / 2) * (n / 2);
    // printf("%d\n\n",jfrom);
    int jsteps = 0;
    //printf("from %d to %d \n", from, to);
    int iStepsLimit;
    int jStepsLimit;
    if (n % 2 == 0)
    {
        iStepsLimit = n / 2;
        jStepsLimit = n / 2;
    }
    else
    {
        if (ifrom == 0)
        {
            iStepsLimit = n / 2;
        }
        else
        {
            iStepsLimit = (n + 1) / 2;
        }
        if (jfrom == 0)
        {
            jStepsLimit = n / 2;
        }
        else
        {
            jStepsLimit = (n + 1) / 2;
        }
    }

    for (int i = ifrom; isteps < iStepsLimit; i++)
    {
        isteps++;
        for (int j = jfrom; jsteps < jStepsLimit; j++)
        {
            jsteps++;
            for (int k = 0; k < n; k++)
            {
                // start critical section

                Pthread_mutex_lock(&Lock);

                C[i][k] += (A[i][j] * B[j][k]);

                Pthread_mutex_unlock(&Lock);
                // end critical section
            }
        }
        jsteps = 0;
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
    
	vector<pthread_t> th(4); // create 4 threads, each performing block multiplication

	for(int i = 0; i < 4; i++)
    {
        Pthread_create(&th[i], NULL, multiply, (void *)i);
    }
    for(int i = 0; i < 4; i++)
    {
        Pthread_join(th[i], NULL);
    }

    /*print(A, 'A');
    print(B, 'B');
    print(C, 'C');*/

	return 0;
}