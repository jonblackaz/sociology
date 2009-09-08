#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <cstdlib>

using namespace std;

void binary ( int, string &);
int maskTest ( string &, string &, int n);

class Multicounter
	{
	public:
		Multicounter() { itsTotalPeaks = 0; itsTotalRuns = 0; };
		~Multicounter();
		void Add( int n );
		double Average();
	private:
		int itsTotalPeaks;
		int itsTotalRuns; 
};

void Multicounter::Add ( int n )
{
	itsTotalRuns++;
	itsTotalPeaks = itsTotalPeaks + n;
}

Multicounter::~Multicounter ()
{
	cout << itsTotalPeaks << " " << itsTotalRuns << endl;
}

double Multicounter::Average ()
{
	double average = ((double) itsTotalPeaks / (double) itsTotalRuns);
	return average ;
}

class Payoff
	{
	public:
		Payoff (string configuration, double fitness);
		~Payoff () {};
		string GetConfig() { return itsConfiguration; }
		double GetFitness() { return itsFitness; }
		void SetConfig(string configuration) { itsConfiguration = configuration; }
		void SetFitness(double fitness) { itsFitness = fitness; }
		int CompareNeighbors ( vector<Payoff> & , int n, int i);
	private:
		string itsConfiguration;
		double itsFitness;
};

Payoff::Payoff (string configuration, double fitness)
{
	itsConfiguration = configuration;
	itsFitness = fitness;
}

int Payoff::CompareNeighbors ( vector<Payoff> & d, int n, int i )
/* this is all screwed up, a lot of extra loops. It does take the configuration, switch one digit, find the payoff for that config, then compare*/
{
	int k = 0;
	int L = 0;
	int M = 0;
	while ( L < pow(2,n) ){
		k=0;
		while ( k < n ){
			string configuration = d[i].GetConfig();
			if ( configuration[k] == '0' ) { configuration[k] = '1'; }
			else { configuration[k] = '0'; }
			if ( configuration == d[L].GetConfig() )
				{
				if ( d[i].GetFitness() < d[L].GetFitness() ){
					return 1;
					}
				}
			k++;
			}
		L++;
		}
	return 0;
}

int main()
{
	int totalruns = 0;
	int iii = 0;
	cout << "How many runs? ";
	Multicounter Counter;
	cin >> totalruns;
	// below this point is the creation of a landscape
	int n = 0;
	int k = 0;
	cout << "Select a size: ";
	cin >> n;
	srand ( time(NULL) );
	cout << "Select k (greater than 0): ";
	cin >> k;
	if ( n > 12 ) { cout << "Less than 13!" << endl; return 0; }
	string configuration;  // list of positions, 0 means not considered, 1 means not chosen, 2 means chosen
	bool choices[10] = {0}; // this means k has to be less than 10, because these represent whether each decision is chosen to be considered vs every other decision
	int i = 0;
	int j = 0;
	int ii = 0;
	int kk = 0;
	int L = 0;
	int choices_counter = 0;
	int random_choice = 0;
	int T = pow(2,(k+1));
	int decision = 0;
	int X = 0;
	int Z = 0;
	int cc = 0;
	int jj = 0;
	double fitnessvalue = 0;
	int success;
	string temp, temp2;
	string numbuf;
	string binaryConfig;
	int numbufLength = 0;
	vector < vector < string > > key; // this is a vector of vectors of strings where each vector has 2^(k+1) strings attached to it	
	vector< vector< Payoff > > payoffs;
	vector < Payoff > d;
	int MM = 0;
	float randomTemp;
while (iii < totalruns) // for some reason, it does the same payoff vector every time?
{
	ii = 0; kk = 0; L = 0; choices_counter = 0; random_choice = 0; decision = 0; X = 0; Z = 0; cc = 0; numbuf.clear(); X=0; binaryConfig.clear(); numbufLength = 0; configuration.clear(); payoffs.clear(); key.clear(); jj = 0; temp.clear(); temp2.clear(); fitnessvalue = 0; configuration.clear(); d.clear();
	while (decision < n){
		cc = 0;
		while (cc < 11)
			{
			choices[cc] = 0;
			cc++;
			}
		ii = 0; kk = 0; L = 0; choices_counter = 0; X = 0; Z = 0;  numbufLength = 0; binaryConfig.clear(); numbuf.clear();
		while (choices_counter < k) // this loop selects k random decisions
			{

			choices[decision] = 1;
			random_choice = rand() % (n); // random integer from 0 to n-1 as index
			if (choices[random_choice] != 1)
				{
				choices[random_choice] = 1;
				binaryConfig.push_back('0');
				choices_counter++;
				}
			}
		if ( k == 0 )
			{
			choices[decision] = 1;
			}
		// now that there is an array showing which choices matter, we need keys for each of the 2^(k+1) choices for those values
		key.resize(decision+1);
		key[decision].resize(1);
		T = pow(2, (k+1));
		while ( (ii < T) ) // loops across all T possible combinations C_i(d_i, d_!i) -- removed k > 0
			{
				numbuf.clear();
				configuration.clear();
				kk = 0;
				Z = 0;
				binary(X, numbuf);
				numbufLength = numbuf.length();
				binaryConfig.replace((k-numbufLength+1),numbufLength,numbuf);
				while (kk < n)
				{
				// this loop adds k chars to string key, 0 means not considered, 1 means not chosen, 2 means chosen
					if (choices[kk] == 1)
						{
							if (binaryConfig[Z] == '1'){ configuration.push_back('2'); Z++;}
							else { configuration.push_back('1'); Z++;}
						}
					else
						{
							configuration.push_back('0');
						}
					kk++;
				}
				key[decision].push_back(configuration);
				ii++;
				X++;
			}
		
		decision++;
	}
	// a vector of maps, where the vector is each decision (d_i), and where each key represents a configuration of choices, and the value is the payoff
	i = 0;  // counter that represents each decision (d_i)
	j = 0;  // counter that represents each other decision given (d_i)(itsTotalPeaks / itsTotalRuns)
	while ( (i < n) ){ // removed && k > 0
		j = 0;
		payoffs.push_back( vector < Payoff >() );
			while ( j < T )
				{
				configuration = key[i][j+1];
				randomTemp = (double)rand() / ((double)(RAND_MAX)+(double)(1));
				payoffs[i].push_back(Payoff(configuration, randomTemp));
// filling the map with keys from 0 to n^(k+1), and values [0,1]
				j++;
				}
		i++;
		}
	i=0;
	jj = 0, success = 0;
	fitnessvalue = 0;
	T = pow(2, n);
	temp.clear();
	temp2.clear();
	while (i < T)
		{
		configuration.clear();
		ii=0;
		while (ii < n)
		{
			configuration.push_back('0');
			ii++;
		}
		numbuf.clear();
		binary(i, numbuf);
		numbufLength = numbuf.length();
		configuration.replace((n-numbufLength),numbufLength,numbuf);
		d.push_back(Payoff(configuration, 0));
		jj=0;
		while (jj < n)
			{
			kk=0;
			while (kk < pow(2, k+1))  // this loop goes through each decision kk, goes to the payoff map, and adds up all the vectors with configs that match
				{
				temp = payoffs[jj][kk].GetConfig();
				temp2 = configuration;
				success = maskTest(temp, temp2, n);
				if ( success ) {
					d[i].SetFitness( d[i].GetFitness() + payoffs[jj][kk].GetFitness() );
					}
				kk++;
				}
			jj++;
			}
		// d[configuration]=payoffSum;
		d[i].SetFitness( d[i].GetFitness()/ (double) n ); // the number of vectors that matched should be equal to the total number (N)
		i++;
		} // this loop created a vector, d, filled with strings representing each possible combination (2^n)
	i=0;	
	MM=0;
	while (i < T)
		{
/* now that we have key/value pairs, and each decision d_i has a value, in order to get the total for the decision configuration, i need to add up all the values for each decision. CRAP, what is stored in the key is the configuration that tells what the relevant other decisions are, but i still would need a vector of the actual decision possibilities (EDIT: this is now the d vector), then i need to use each of those possibilities to get a number from the key vector
 how to determine peaks? 	
*/
		configuration = d[i].GetConfig();
		success = d[i].CompareNeighbors(d, n, i);
		if (!success) {
			cout << "Local peak! " << d[i].GetConfig() << endl;
			MM++;
			}
		i++;
		}
	Counter.Add( MM );
	iii++;
}
	cout << Counter.Average() << endl;
return 0;
}

void binary(int number, string & numbuf) {
	int remainder;
	if(number == 0) {
		numbuf.push_back('0');
		return;
	}
	if(number == 1) {
		numbuf.push_back('1');
		return;
	}
	remainder = number%2;
	binary(number >> 1, numbuf);
	if(remainder==0){numbuf.push_back('0');}
	if(remainder==1){numbuf.push_back('1');}
	return;
}

int maskTest(string & temp, string & temp2, int n) { // checks to see if the first string (i.e. 21012) matches the second (i.e. 10101)
	if ( n == 0) { return 1; }	
	n = n-1;
	if( temp[n] == '0' ) { // this needs to be changed so that if temp[n] is 2, temp2 must be 1, if temp is 1, temp2 == 0
		if(maskTest(temp, temp2, n)) { return 1; }
	}
	if( temp[n] == '2' && temp2[n] == '1' ) {
		if(maskTest(temp, temp2, n)) { return 1; }
	}
	if( temp[n] == '1' && temp2[n] == '0' ) {
		if(maskTest(temp, temp2, n)) { return 1; }
	}
	return 0;
}
