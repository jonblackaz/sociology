/* 
 * this version takes a pair of landscapes and creates another that takes the key vector of each
 * and adds a second n-length vector to the end of each key that represents the cross-landscape effects.
 * 
 * BIG PROBLEM - HOW AM I HANDLING D AND D_TWO?  
 * Right now, the configurations are set up so that d[config] and d_two[config] both refer to the same overall configuration... however
 * when i designed it I was thinking that they would refer to the 'inverse' of each other... d_1 and d_2 would switch... how does this affect landscapes?
 */

#include <iostream>
#include <fstream>
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
string flipConfigString( const string & configuration );

class Multicounter
	{
	public:
		Multicounter() { itsTotalPeaks = 0; itsTotalRuns = 0; };
		Multicounter( string name ) { itsTotalPeaks = 0; itsTotalRuns = 0; itsName = name; };
		~Multicounter();
		void Add( int n );
		void Add( double added );
		void Replace( double replaced );
		double Average( string which );
		void Show();
	private:
		string itsName;
		int itsTotalRuns;
		int itsTotalPeaks;
		double itsDouble;
};

void Multicounter::Add ( int n )
{
	itsTotalRuns++;
	itsTotalPeaks = itsTotalPeaks + n;
}

void Multicounter::Add ( double added )
{
	itsTotalRuns++;
	itsDouble = itsDouble + added;
}

void Multicounter::Replace ( double replaced )
{
	itsTotalRuns++;
	itsDouble = replaced;
}

void Multicounter::Show()
{
	cout << itsName << " - " << itsTotalRuns << " Total: " << itsDouble << "  Average: " << this->Average( "double" ) << endl;
}

Multicounter::~Multicounter ()
{
}

double Multicounter::Average ( string which )
{
	double average;
	if (which == "double") {
		average = ((double) itsDouble / (double) itsTotalRuns);
	}
	if (which == "int") {
		average = ((double) itsTotalPeaks / (double) itsTotalRuns);
	}
	return average ;
}

class Payoff
	{
	public:
		Payoff (string configuration, double fitness);
		Payoff () {};
		~Payoff () {};
		string GetConfig() { return itsConfiguration; }
		double GetFitness() { return itsFitness; }
		void SetConfig(string configuration) { itsConfiguration = configuration; }
		void SetFitness(double fitness) { itsFitness = fitness; }
		int CompareNeighbors ( map<string, Payoff> & , int n, string testConfig);
		int CompareNeighbors ( map<string, Payoff> & , int n, string testConfig, int first, int last);
	private:
		string itsConfiguration;
		double itsFitness;
		bool peak;
};

Payoff::Payoff (string configuration, double fitness)
{
	itsConfiguration = configuration;
	itsFitness = fitness;
}

int Payoff::CompareNeighbors ( map<string,Payoff> & d, int itsN, string testConfig )
/*  takes the configuration, switch one digit, find the payoff for that config, then compare*/
{
	int YY = 0;
	int M = 0;
	while ( YY < itsN )
		{
		string configuration = testConfig;
		if ( configuration[YY] == '0' ) { configuration[YY] = '1'; }
		else { configuration[YY] = '0'; }
		if ( d[configuration].GetFitness() > d[testConfig].GetFitness() )
			{
			return 1;
			}
		YY++;
		}
	return 0;
}

int Payoff::CompareNeighbors ( map<string, Payoff> & d, int itsN, string testConfig, int firstN, int lastN )
/* this is all screwed up, a lot of extra loops. It does take the configuration, switch one digit, find the payoff for that config, then compare*/
{
	int YY = 0;
	int M = 0;
	bool test;
	string partial, temp, configuration;
	int T = pow(2,itsN);
	map<string,Payoff>::iterator L;
	for ( L = d.begin() ; L != d.end() ; L++){
		YY=0;
		partial = testConfig.substr(firstN,(lastN-firstN));
		test = (*L).first.compare(firstN,lastN,partial);
		temp = (*L).first;
		if ( (*L).second.GetFitness() > d[testConfig].GetFitness() && test == 0)
			{
				return 1;
			}
		while ( YY < lastN )
			{
			configuration = partial;
			configuration[YY] == '0' ? configuration[YY] = '1' : configuration[YY] = '0';
			test = temp.compare(firstN,lastN,configuration);
			if ( (*L).second.GetFitness() > d[testConfig].GetFitness() && test == 0)
				{
					return 1;
				}
			YY++;
			}
		}
	return 0;
}

/* ------------- START OF LANDSCAPE ---------------------- */
class Landscape
{
	public:
		Landscape( int inputN, int inputK, bool KeysOnly );
		Landscape();
		~Landscape();
		int getNumPeaks() { return itsNumPeaks; }
		int getNumPeaks2() { return itsNumPeaks2; }
		void outputPayoffs();
		void sumPayoffs(int n, map<string,Payoff> & d, vector < vector < Payoff > > & payoffs, int T, string & numbuf );
		void buildStructure( int T, string & numbuf);
		void fillPartialPayoffs( int T );
		int GetN() { return n; }
		int GetK() { return k; }
		vector < vector < string > > key; // this is a vector of vectors of strings where each vector has 2^(k+1) strings attached to it
		map < string, Payoff > d; // d is the final vector (i: 0 -> 2^n)
	protected:
		vector< vector< Payoff > > payoffs; // 
		int itsNumPeaks;
		int itsNumPeaks2;
		int n;
		int k;
		map < string, unsigned short int > Distribution;	
};


Landscape::Landscape()
{
}

Landscape::Landscape( int initialN, int initialK, bool KeysOnly )
{
	this->n = initialN;
	this->k = initialK;
	string configuration;  // list of positions, 0 means not considered, 1 means not chosen, 2 means chosen
	unsigned short int random_choice = 0;
	unsigned short int T = pow(2,(k+1));
	unsigned short int success;
	string numbuf;
	unsigned short int MM = 0;
	buildStructure(T, numbuf);
	if ( !KeysOnly){
		fillPartialPayoffs(T);
		// a vector of payoffs, where the vector is each decision (d_i), and where each key represents a configuration of choices, and the value is the payoff

		unsigned short int i = 0;  // counter that represents each decision (d_i)
		unsigned short int j = 0;  // counter that represents each other decision given (d_i)(itsTotalPeaks / itsTotalRuns)
		unsigned short int jj = 0;
		success = 0;
		T = pow(2, n);
		sumPayoffs(n, d, payoffs, T, numbuf);
		i=0;	
		MM=0;
		map <string,Payoff>::iterator map_It;
		for (map_It = d.begin() ; map_It != d.end() ; map_It++)
			{
	/* now that we have key/value pairs, and each decision d_i has a value, in order to get the total for the decision configuration, i need to add up all the values for each decision. CRAP, what is stored in the key is the configuration that tells what the relevant other decisions are, but i still would need a vector of the actual decision possibilities (EDIT: this is now the d vector), then i need to use each of those possibilities to get a number from the key vector
	 how to determine peaks? 	
	*/
			configuration = (*map_It).first;
			success = (*map_It).second.CompareNeighbors(d, n, configuration);
			if (!success) {
				MM++;
				}
			}
		itsNumPeaks = MM;
	}
}
/* ------------- END OF LANDSCAPE ---------------------- */
Landscape::~Landscape(){}

void Landscape::buildStructure(int T, string & numbuf)
/* fills the key vector */
{
	string configuration;
	bool choices[20] = {0}; 
	string binaryConfig;
	unsigned short int choices_counter = 0, i = 0, L = 0, numbufLength = 0, decision = 0, X = 0, Z = 0, cc = 0, jj = 0, kk = 0, ii = 0, random_choice = 0;
	float randomTemp;
	while (decision < n){
		cc = 0;
		while (cc < 21)
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
}

void Landscape::sumPayoffs(int n, map<string, Payoff> & d, vector < vector < Payoff > > & payoffs, int T, string & numbuf )
{
	unsigned short int i, jj, kk, ii, numbufLength, success;
	string temp; string temp2; string configuration;
	i=0, jj=0, kk=0, ii=0, numbufLength=0, success=0;
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
		d.insert( pair<string,Payoff> (configuration, Payoff(configuration, 0)));
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
					d[configuration].SetFitness( d[configuration].GetFitness() + payoffs[jj][kk].GetFitness() );
					}
				kk++;
				}
			jj++;
			}
		// d[configuration]=payoffSum;
		d[configuration].SetFitness( d[configuration].GetFitness()/ (double) n ); // the number of vectors that matched should be equal to the total number (N)
		i++;
		} // this loop created a vector, d, filled with strings representing each possible combination (2^n)
} // fills the d vector

void Landscape::outputPayoffs()
{
	cout << "outputPayoffs(" << n << ", " << k << ") called..." << endl;
	unsigned short int i = 0, j=0;
	while ( i < n )
		{
		j=0;
		while ( j < k )
			{
			cout << payoffs[i][j].GetConfig() << " ";
			j++;
			}
		cout << endl;
 		i++;
		}
}

void Landscape::fillPartialPayoffs(int T)
/* this is all the partial payoffs for each thingie */
{
	double randomTemp = 0;
	string configuration;
	unsigned short int i = 0, j = 0;
	while ( (i < n) ){ 
		j = 0;
		payoffs.push_back( vector < Payoff >() );
			while ( j < T )
				{
				configuration = key[i][j+1];
				randomTemp = (double)rand() / ((double)(RAND_MAX)+(double)(1));
				payoffs[i].push_back(Payoff(configuration, randomTemp)); // filling the map with keys from 0 to n^(k+1), and values [0,1]
				j++;
				}
		i++;
		}
}


class JoinedLandscape : public Landscape
{
	public:
		JoinedLandscape( Landscape & LOne, Landscape & LTwo, int k_two );
		~JoinedLandscape();
		void Join( Landscape & LOne, Landscape & LTwo, int k_two );
		void buildJoined ( Landscape & L_Join, int k_input, int decision, vector < vector < string > > & key_new, string binaryConfig, bool * choices );
		void fillPartialPayoffs(int T);
		map < string, Payoff > d_two;
		int Writeout ();
	private:
		vector < vector< Payoff > > payoffs_two;
		vector < vector < string > > key_two;
};

void JoinedLandscape::fillPartialPayoffs(int T)
/* this is an ugly copy/paste of the partial payoffs for each thingie */
{
	double randomTemp = 0;
	string configuration;
	unsigned short int i = 0, j = 0;
	while ( (i < n) ){ 
		j = 0;
		payoffs.push_back( vector < Payoff >() );
			while ( j < T )
				{
				configuration = key[i][j];
				randomTemp = (double)rand() / ((double)(RAND_MAX)+(double)(1));
				payoffs[i].push_back(Payoff(configuration, randomTemp)); // filling the map with keys from 0 to n^(k+1), and values [0,1]
				j++;
				}
		i++;
		}
	randomTemp = 0;
	configuration.clear();
	i = 0, j = 0;
	while ( (i < n) ){ 
		j = 0;
		payoffs_two.push_back( vector < Payoff >() );
			while ( j < T )
				{
				configuration = key_two[i][j];
				randomTemp = (double)rand() / ((double)(RAND_MAX)+(double)(1));
				payoffs_two[i].push_back(Payoff(configuration, randomTemp)); // filling the map with keys from 0 to n^(k+1), and values [0,1]
				j++;
				}
		i++;
		}
}

void JoinedLandscape::buildJoined( Landscape & L_Join, int k_input, int decision, vector < vector < string > > & key_new, string binaryConfig, bool * choices)
{
	string numbuf, configuration, configuration2;
	unsigned short int numbufLength = 0, X = 0, Z = 0, cc = 0, kk = 0, ii = 0, ll = 0;
	float randomTemp;
	while ( (ii < pow(2,k)) ) // loops across all T possible combinations C_i(d_i, d_!i) -- removed k > 0
		{
			numbuf.clear();
			configuration.clear();
			configuration2.clear();
			kk = 0;
			Z = 0;
			binary(X, numbuf);
			numbufLength = numbuf.length();
			binaryConfig.replace((k-numbufLength),numbufLength,numbuf);
			// cout << LOne.key[decision][ii] << endl;
			// configuration.push_back(LOne.key[decision][ii]);
			kk = 0;
			while (kk < n)
			{
				// this loop adds k chars to string key, 0 means not considered, 1 means not chosen, 2 means chosen
				if (choices[kk] == 1)
					{
						if ( binaryConfig[Z] == '1' ){ configuration.push_back('2'); Z++;}
						else { configuration.push_back('1'); Z++;}
					}
				else
					{
						configuration.push_back('0');
					}
				kk++;
			}
			ll = 0;
			while ( ll < pow(2, L_Join.GetK() + 1 ) )
				{
				configuration2.clear();
				configuration2 = L_Join.key[decision][ll+1] + configuration;
				key_new[decision].push_back(configuration2);
				ll++;
				}
			ii++;
			X++;
			}
}

/****************************** START OF JOIN ***************************/
JoinedLandscape::JoinedLandscape( Landscape & LOne, Landscape & LTwo, int k_input ) // relate two landscapes payoff structures so that decisions must be made with respect to both
/*    one approach would be to pass references to the two smaller landscapes then use the vectors to create a new one
 *    map < string, Payoff > d
 *    d is the final vector (i: 0 -> 2^n)
 *    vector< vector< Payoff > > payoffs --- a partial contribution for each possible configuration where payoffs[1][i] is the ith config for the 1th choice
 *    vector < vector < string > > key  ---  shows which set of choices matter for a given choice
 *    the best way to accomplish this might be to have one separate array for the cross-landscape choices that matter
 */
{
	cout << "Landscape initialized... working";
	n = LOne.GetN();
	k = k_input;
	string configuration, configuration2, numbuf;  // list of positions, 0 means not considered, 1 means not chosen, 2 means chosen
	int random_choice = 0, i = 0, j = 0, jj = 0, success = 0, MM = 0;
	int T = pow(2,(k+1));
	// buildStructure(T, numbuf, crosskey);
	// fillPartialPayoffs(T);
	// T = pow(2, n);
	// sumPayoffs(n, d, payoffs, T, numbuf);
	bool choices[20] = {0}; 
	string binaryConfig;
	int choices_counter = 0, L = 0, numbufLength = 0, decision = 0, X = 0, Z = 0, cc = 0, kk = 0, ii = 0, ll = 0;
	float randomTemp;
	while (decision < n ){  // i need to change this so that it creates two distinct sets of vectors
		cc = 0;
		while (cc < 21)
			{
			choices[cc] = 0;
			cc++;
			}
		ii = 0; kk = 0; L = 0; choices_counter = 0; X = 0; Z = 0;  numbufLength = 0; binaryConfig.clear(); numbuf.clear();
		while (choices_counter < k) // this loop selects k random decisions
			{
			random_choice = rand() % (n); // random integer from 0 to n-1 as index
			if (choices[random_choice] != 1)
				{
				choices[random_choice] = 1;
				binaryConfig.push_back('0');
				choices_counter++;
				}
			}
		// now that there is an array showing which choices matter, we need keys for each of the 2^(k+1) choices for those values
		key.resize(decision+1);
		key_two.resize(decision+1);
		buildJoined( LOne, k, decision, key, binaryConfig, choices );
		buildJoined( LTwo, k, decision, key_two, binaryConfig, choices );
		decision++;
	}
	fillPartialPayoffs( pow(2, (LOne.GetK()+k+1) ));
	i = 0; int mmm = 0;
/*	while (i < n){ // test loop
 *		mmm = 0;
 *		while (mmm < pow(2, LOne.k + k + 1)){
 *			cout << " " << payoffs[i][mmm].GetFitness() << " ";
 *			mmm++;
 *		}
 *		cout << endl;
 *		i++;
 *	}
 */
	i = 0;  // counter that represents each decision (d_i)
	success = 0;
	int divisor;
	T = pow(2, n);
	string temp, temp2;

/* need to replace with a function ****************** */
	i=0, jj=0, kk=0, ii=0, numbufLength=0, success=0;
	while (i < pow(2,(2*n)))
		{
		configuration.clear();
		ii=0;
		while ( ii < 2*n )
		{
			configuration.push_back('0');
			ii++;
		}
		numbuf.clear();
		binary(i, numbuf);
		numbufLength = numbuf.length();
		configuration.replace(((2*n)-numbufLength),numbufLength,numbuf);
		d.insert( pair<string,Payoff> (configuration, Payoff(configuration, 0)));
		jj=0;
		divisor = 0;
		while (jj < n)
			{
			kk=0;
			while (kk < pow(2, LOne.GetK() + k+ 1))  // this loop goes through each decision kk, goes to the payoff map, and adds up all the vectors with configs that match
				{
				temp = payoffs[jj][kk].GetConfig();
				temp2 = configuration;
				success = maskTest(temp, temp2, (2*n));
				if ( success ) {
					d[configuration].SetFitness( d[configuration].GetFitness() + payoffs[jj][kk].GetFitness() );
					divisor++;
					}
				kk++;
				}
			jj++;
			}
		// d[configuration]=payoffSum;
		d[configuration].SetFitness( d[configuration].GetFitness()/ (double) (n) ); // the number of vectors that matched should be equal to the total number (N)
		i++;
		}
/* here ends *************************** */
/* need to replace with a function ****************** */
	i=0, jj=0, kk=0, ii=0, numbufLength=0, success=0;
	while (i < pow(2,(2*n)))
		{
		configuration.clear();
		ii=0;
		while ( ii < 2*n )
		{
			configuration.push_back('0');
			ii++;
		}
		numbuf.clear();
		binary(i, numbuf);
		numbufLength = numbuf.length();
		configuration.replace(((2*n)-numbufLength),numbufLength,numbuf);
		d_two.insert( pair<string,Payoff> (configuration, Payoff(configuration, 0)));
		jj=0;
		divisor = 0;
		while (jj < n)
			{
			kk=0;
			while (kk < pow(2, LTwo.GetK() + k+ 1))  // this loop goes through each decision kk, goes to the payoff map, and adds up all the vectors with configs that match
				{
				temp = payoffs[jj][kk].GetConfig();
				temp2 = configuration;
				success = maskTest(temp, temp2, (2*n));
				if ( success ) {
					d_two[configuration].SetFitness( d_two[configuration].GetFitness() + payoffs_two[jj][kk].GetFitness() );
					divisor++;
					}
				kk++;
				}
			jj++;
			}
		// d[configuration]=payoffSum;
		d_two[configuration].SetFitness( d_two[configuration].GetFitness()/ (double) (n) );
		// the number of vectors that matched should be equal to the total number (N)
		i++;
		}
/* here ends *************************** */

	i=0; MM=0;
	int MM_two =0;
/*	map<string,Payoff>::iterator IT;
	for ( IT = d.begin() ; IT != d.end() ; IT++)
		{
		success = (*IT).second.CompareNeighbors(d, 2*n, (*IT).first, 0, n);
		if (!success) {
			MM++;
			}
		}
	itsNumPeaks = MM;
	i=0; MM_two=0;
	for ( IT = d_two.begin() ; IT != d_two.end() ; IT++)
		{
		success = (*IT).second.CompareNeighbors(d_two, 2*n, (*IT).first, 0, n);
		if (!success) {
			MM_two++;
			}
		}
	itsNumPeaks2 = MM_two;
*/	cout << " landscape complete!" << endl;
}
/****************************** END OF JOIN ***************************/

int JoinedLandscape::Writeout ()
{
	ofstream out( "output.txt" );

	if( !out )
	{
		cout << "Couldn't open file."  << endl;
		return 0;
	}
	map < string , Payoff >::iterator IT;
	for ( IT = d.begin() ; IT != d.end() ; IT++ )
	{
		out << (*IT).second.GetConfig() << ":" << (*IT).second.GetFitness() << endl;
	}
	map < string , Payoff >::iterator IT_two;
	for ( IT_two = d_two.begin() ; IT_two != d_two.end() ; IT_two++ )
	{
		out << (*IT_two).second.GetConfig() << ":" << (*IT_two).second.GetFitness() << endl;
	}
	out.close();
	return 1;
}

class Entity
	{
	public:
		Entity ( map< string , Payoff > & d, map< string , Payoff > & d_two, int n);
		~Entity ();
		string checkVicinity_one( map< string , Payoff > & d, map< string , Payoff > & d_two, unsigned short int maxN);
		string checkVicinity_two( map< string , Payoff > & d, map< string , Payoff > & d_two, unsigned short int maxN);
		string checkVicinity_one_blind( map< string , Payoff > & d, map< string , Payoff > & d_two, unsigned short int maxN);
		string checkVicinity_two_blind( map< string , Payoff > & d, map< string , Payoff > & d_two, unsigned short int maxN);
		string checkVicinity_all( map< string , Payoff > & d, map< string , Payoff > & d_two);
		bool Move( string destination, map< string , Payoff > & d, map< string , Payoff > & d_two );
		void Reset( map< string , Payoff > & d, map< string , Payoff > & d_two );
		string GetConfig() { return itsPayoff.GetConfig(); }
		double GetFitness() { return itsPayoff.GetFitness(); }			
	private:
		int itsN;
		Payoff itsPayoff;
};

Entity::Entity ( map< string , Payoff > & d, map< string , Payoff > & d_two, int n)
{
	map <string, Payoff >::iterator IT;
	IT = d.begin(); 
	map <string, Payoff >::iterator IT_TWO;
	IT_TWO = d_two.begin();
	itsPayoff.SetConfig( (*IT).second.GetConfig() );
	itsPayoff.SetFitness( ((*IT).second.GetFitness() + (*IT_TWO).second.GetFitness()) / (double) 2 );
	itsN = n;
}

void Entity::Reset( map< string , Payoff > & d, map< string , Payoff > & d_two )
{
	map <string, Payoff >::iterator IT;
	IT = d.begin(); 
	map <string, Payoff >::iterator IT_TWO;
	IT_TWO = d_two.begin();
	itsPayoff.SetConfig( (*IT).second.GetConfig() );
	itsPayoff.SetFitness( ((*IT).second.GetFitness() + (*IT_TWO).second.GetFitness()) / (double) 2 );
}

/* ********************************************* NONBLIND ************************ */
string Entity::checkVicinity_one( map< string , Payoff > & d, map< string , Payoff > & d_two, unsigned short int maxN)
/* this version keeps landscape 2 choices constant */
{
	Payoff Move;
	Payoff Nil( "EMPTY" , 0 );
	string tempConfig = itsPayoff.GetConfig(), tempBinary, tempConfig_two;
	double currentAverage = (d[ itsPayoff.GetConfig() ].GetFitness() + d_two[ flipConfigString(itsPayoff.GetConfig()) ].GetFitness()) / (double) 2;
	unsigned short int i, j;
	unsigned short int strLength;
	vector <Payoff> averages;
	double tempAverage;
	strLength = tempConfig.length();
	for ( i = 0 ; i < maxN ; i++)
	{
		tempConfig = itsPayoff.GetConfig();
		if ( tempConfig[i] == '0' ) { tempConfig[i] = '1'; }
		else { tempConfig[i] = '0'; };
		tempConfig_two = flipConfigString( tempConfig );	
		tempAverage = (d[tempConfig].GetFitness() + d_two[tempConfig_two].GetFitness()) / (double) 2;
		averages.push_back ( Payoff(tempConfig,tempAverage) );
	}
	Move.SetFitness(0);
	vector <Payoff>::iterator IT;
	for ( IT = averages.begin() ; IT != averages.end() ; IT++ )
	{
		if ( Move.GetFitness() < (*IT).GetFitness() )
			{
			Move.SetConfig( (*IT).GetConfig() );
			Move.SetFitness( (*IT).GetFitness() );
			}
	}
	if ( Move.GetFitness() <= itsPayoff.GetFitness() ) { return "EMPTY"; }
	return Move.GetConfig();
}

string Entity::checkVicinity_two( map< string , Payoff > & d, map< string , Payoff > & d_two, unsigned short int maxN)
/* this version keeps landscape 2 choices constant */
{
	Payoff Move;
	string tempConfig = itsPayoff.GetConfig(), tempBinary, tempConfig_two;
	double currentAverage = (d[ itsPayoff.GetConfig() ].GetFitness() + d_two[ flipConfigString(itsPayoff.GetConfig()) ].GetFitness()) / (double) 2;
	unsigned short int i, j;
	unsigned short int strLength;
	vector <Payoff> averages;
	double tempAverage;
	strLength = tempConfig.length();
	for ( i = maxN ; i < 2*maxN ; i++)
	{
		tempConfig = itsPayoff.GetConfig();
		if ( tempConfig[i] == '0' ) { tempConfig[i] = '1'; }
		else { tempConfig[i] = '0'; };
		tempConfig_two = flipConfigString( tempConfig );	
		tempAverage = (d[tempConfig].GetFitness() + d_two[tempConfig_two].GetFitness()) / (double) 2;
		averages.push_back ( Payoff(tempConfig,tempAverage) );
	}
	Move.SetFitness(0);
	vector <Payoff>::iterator IT;
	for ( IT = averages.begin() ; IT != averages.end() ; IT++ )
	{			
		if ( Move.GetFitness() < (*IT).GetFitness() )
			{
			Move.SetConfig( (*IT).GetConfig() );
			Move.SetFitness( (*IT).GetFitness() );
			}
	}
	if ( Move.GetFitness() <= itsPayoff.GetFitness() ) { return "EMPTY"; }
	return Move.GetConfig();
}

string Entity::checkVicinity_all( map< string , Payoff > & d, map< string , Payoff > & d_two)
/* this version, once complete, will consider both landscapes and make the move that provides the biggest average */
{
	Payoff Move;
	string tempConfig = itsPayoff.GetConfig();
	string tempConfig_two;
	unsigned short int i;
	unsigned short int strLength;
	vector <Payoff> averages;
	double tempAverage;
	strLength = tempConfig.length();
	for ( i = 0 ; i < strLength ; i++)
	{
		tempConfig = itsPayoff.GetConfig();
		if ( tempConfig[i] == '0' ) { tempConfig[i] = '1'; }
		else { tempConfig[i] = '0'; }
		tempConfig_two = flipConfigString( tempConfig );
		tempAverage = (d[tempConfig].GetFitness() + d_two[tempConfig_two].GetFitness()) / (double) 2;
		averages.push_back ( Payoff(tempConfig,tempAverage) );
	}
	Move.SetFitness(0);
	vector <Payoff>::iterator IT;
	for ( IT = averages.begin() ; IT != averages.end() ; IT++)
	{
		if (Move.GetFitness() < (*IT).GetFitness() )
			{
			Move.SetConfig( (*IT).GetConfig() );
			Move.SetFitness( (*IT).GetFitness() );
			}
	}
	if ( Move.GetFitness() <= itsPayoff.GetFitness() ) { return "EMPTY"; }
	return Move.GetConfig();
} 
/* ********************************************* END NONBLIND ************************ */

/* ********************************************* BLIND ************************ */
string Entity::checkVicinity_one_blind( map< string , Payoff > & d, map< string , Payoff > & d_two, unsigned short int maxN )
/* this version keeps landscape 2 choices constant */
{
	Payoff Move;
	string tempConfig = itsPayoff.GetConfig();
	unsigned short int i, j;
	unsigned short int strLength;
	double tempFitness;
	strLength = tempConfig.length();
	tempConfig = itsPayoff.GetConfig();
	i = rand() % (maxN);
	if ( tempConfig[i] == '0' ) { tempConfig[i] = '1'; }
	else { tempConfig[i] = '0'; };
	tempFitness = d[tempConfig].GetFitness();
	Move.SetConfig( tempConfig );
	Move.SetFitness( tempFitness );
	if ( Move.GetFitness() <= d[ itsPayoff.GetConfig() ].GetFitness() ) { return "EMPTY"; }
	return Move.GetConfig();
}

string Entity::checkVicinity_two_blind( map< string , Payoff > & d, map< string , Payoff > & d_two, unsigned short int maxN )
/* this version keeps landscape 2 choices constant */
{
	Payoff Move;
	Payoff NoMove( "EMPTY" , 0 );
	string tempConfig = itsPayoff.GetConfig();
	unsigned short int i, j;
	unsigned short int strLength;
	double tempFitness;
	strLength = tempConfig.length();
	tempConfig = itsPayoff.GetConfig();
	tempConfig = flipConfigString ( tempConfig );
	i = rand() % (maxN);
	if ( tempConfig[i] == '0' ) { tempConfig[i] = '1'; }
	else { tempConfig[i] = '0'; };
	tempFitness = d_two[tempConfig].GetFitness();
	Move.SetConfig( flipConfigString (tempConfig) );
	Move.SetFitness( tempFitness );
	if ( Move.GetFitness() <= d_two[ flipConfigString(itsPayoff.GetConfig()) ].GetFitness() ) { return "EMPTY"; }
	return Move.GetConfig();
}

/* ********************************************* END BLIND ************************ */

bool Entity::Move( string Move, map< string , Payoff > & d, map< string , Payoff > & d_two )
{
	if ( Move == "EMPTY" ) { return 0; }
	cout << "Move from " << itsPayoff.GetConfig() << ":" << itsPayoff.GetFitness() << " to ";
	itsPayoff.SetConfig( Move );
	itsPayoff.SetFitness( (d[Move].GetFitness() + d_two[ flipConfigString(Move) ].GetFitness()) / (double) 2 );
	cout << itsPayoff.GetConfig() << ":" << itsPayoff.GetFitness() << " is successful." << endl;
	return 1;
}

int main()
{
	unsigned short int n=0, k=0, totalruns=0, i=0, k_two=0, walks=0, j=0;
	string search_strategy;
	Multicounter CounterA("A"), CounterB("B"), CounterC("C"), CounterD("D");
	cout << "How many runs? ";
	cin >> totalruns;
	cout << "Select a size: ";
	cin >> n;
	srand ( time(NULL) );
	cout << "Select k_1 (" << n-1 << " or less): ";
	cin >> k;
	cout << "Select k_2 (" << n-1 << " or less): ";
	cin >> k_two;
	cout << "Select number of steps: ";
	cin >> walks;
	cout << endl << "a) Keeps landscape constant (non-blind)"
	     << endl << "b) Optimizes all choices at the same time (non-blind)"
	     << endl << "all) Use all methods" << endl;
	cout << "Select a search strategy[default='all']: " ;
	cin >> search_strategy;
	if ( search_strategy != "a" && search_strategy != "b" )
		{
		search_strategy = "all";
		}
	Landscape * pLandscape = 0;
	Landscape * pLandscape_two = 0;
	JoinedLandscape * pLandscape_join = 0;
	Entity * pEntity = 0;
	string Move;
	bool success = 0;
 	while( i < totalruns)
 	{
 		pLandscape = new Landscape(n,k,1);
		pLandscape_two = new Landscape(n,k,1);
		pLandscape_join = new JoinedLandscape(*pLandscape, *pLandscape_two, k_two);
		pEntity = new Entity( pLandscape_join->d, pLandscape_join->d_two, 2*n);
		if ( search_strategy == "a" || search_strategy == "all")
		{
			cout << "--------Search A--------" << endl;
			for ( j = 0 ; j < walks ; j++)
			{
				Move = pEntity->checkVicinity_one( pLandscape_join->d , pLandscape_join->d_two , n );
				success = pEntity->Move( Move, pLandscape_join->d , pLandscape_join->d_two );
				if ( !success ) { cout << "No move (1)" << endl; }
			}
			for ( j = 0 ; j < walks ; j++)
			{
				Move = pEntity->checkVicinity_two( pLandscape_join->d , pLandscape_join->d_two , n );
				success = pEntity->Move( Move, pLandscape_join->d , pLandscape_join->d_two );
				if ( !success ) { cout << "No move (2)" << endl; }
			}
			cout << "Final: " << pEntity->GetConfig() << ":" << pEntity->GetFitness() << endl;
			CounterA.Add ( pEntity->GetFitness() );
			cout << "------------------------" << endl;
		}
		pEntity->Reset( pLandscape_join->d, pLandscape_join->d_two );
		if ( search_strategy == "b" || search_strategy == "all")
		{	
			cout << "--------Search B--------" << endl;
			for ( j = 0 ; j < walks ; j++)
			{
				Move = pEntity->checkVicinity_all( pLandscape_join->d , pLandscape_join->d_two );
				success = pEntity->Move( Move, pLandscape_join->d , pLandscape_join->d_two );
				if ( !success ) { cout << "No move" << endl; }
			}
			cout << "Final: " << pEntity->GetConfig() << ":" << pEntity->GetFitness() << endl;
			CounterB.Add ( pEntity->GetFitness() );
			cout << "------------------------" << endl;
		}
		pEntity->Reset( pLandscape_join->d, pLandscape_join->d_two );
		if ( 1 )
		{	
			cout << "--------Search C--------" << endl;
			for ( j = 0 ; j < walks ; j++)
			{
				Move = pEntity->checkVicinity_one_blind( pLandscape_join->d , pLandscape_join->d_two , n );
				success = pEntity->Move( Move, pLandscape_join->d , pLandscape_join->d_two );
				if ( !success ) { cout << "No move (1)" << endl; }
			}
			for ( j = 0 ; j < walks ; j++)
			{
				Move = pEntity->checkVicinity_two_blind( pLandscape_join->d , pLandscape_join->d_two , n );
				success = pEntity->Move( Move, pLandscape_join->d , pLandscape_join->d_two );
				if ( !success ) { cout << "No move (2)" << endl; }
			}
			cout << "Final: " << pEntity->GetConfig() << ":" << pEntity->GetFitness() << endl;
			CounterC.Add ( pEntity->GetFitness() );
			cout << "------------------------" << endl;
		}
		pEntity->Reset( pLandscape_join->d, pLandscape_join->d_two );
		if ( 1 )
		{	
			cout << "--------Search D--------" << endl;
			for ( j = 0 ; j < walks ; j++)
			{
				Move = pEntity->checkVicinity_one_blind( pLandscape_join->d , pLandscape_join->d_two , n );
				success = pEntity->Move( Move, pLandscape_join->d , pLandscape_join->d_two );
				if ( !success ) { cout << "No move (1)" << endl; }
				Move = pEntity->checkVicinity_two_blind( pLandscape_join->d , pLandscape_join->d_two , n );
				success = pEntity->Move( Move, pLandscape_join->d , pLandscape_join->d_two );
				if ( !success ) { cout << "No move (2)" << endl; }
			}
			cout << "Final: " << pEntity->GetConfig() << ":" << pEntity->GetFitness() << endl;
			CounterD.Add ( pEntity->GetFitness() );
			cout << "------------------------" << endl;
		}
		// pLandscape_join->Writeout();
 		delete pLandscape, pLandscape_two, pLandscape_join, pEntity;
 		i++;
 	}
	*pLandscape, pLandscape_two, pLandscape_join = 0;
 	CounterA.Show();
 	CounterB.Show();
 	CounterC.Show();
 	CounterD.Show();
	return 0;
}

void binary(int number, string & numbuf)
{
	unsigned short int remainder;
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

int maskTest(string & temp, string & temp2, int n)
{ // checks to see if the first string (i.e. 21012) matches the second (i.e. 10101)
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

string flipConfigString( const string & configuration )
{
	unsigned short int strLength=0;
	string one, two, newConfig;
	strLength = ( configuration.length() + 1 )/ 2;
	one = configuration.substr( 0 , strLength );
	two = configuration.substr( strLength , 2 * strLength );
	newConfig = two + one;
	return newConfig;
}
