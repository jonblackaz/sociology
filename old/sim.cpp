#include <iostream>
#include <vector>
#include <string>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;


int main()
{
	int n = 0;
	cout << "Select a size: " << endl;
	cin >> n;
	srand ( time(NULL) );
	int k = rand() % (n-1);
	cout << endl << "k: " << k << endl;
	vector<int> d(n,0);
        // Vector of length k containing two dimensional vectors
	vector< vector< vector<int> > > vI3Matrix(2, vector< vector<int> > (n, vector<int>(k,0)) );
	cout << "Success" << endl;
	int i;
	int j = 0;
	float m = 0;
	while ( j < n ){
		i = 0;
		cout << j;
		while ( i < k )
			{
			m = rand() % 100;
			cout << " " << m << " ";
			vI3Matrix[0][n][k];
			i++;
			}
		i = 0;
		while ( i < k )
			{
			m = rand() % 100;
			i++;
			}
		j++;
		}
	return 0;
}
