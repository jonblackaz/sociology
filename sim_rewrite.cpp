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
/* Working with lots of strings of length N that need to be combined and compared. The landscape is made up of a set of positions. Each position has an associated fitness level and a set of choices. Fitness levels are calculated by finding the average of each binary choice's individual fitness. Each choice's fitness, in turn, depends on k other choices for that position. 
*/

void genConfigString(unsigned int X, string & binaryString, int length);
void binary(unsigned int number, string & partialBinaryString);
bool isEqual(double d1, double d2); 
int bitcount (unsigned int n);

// This class handles configurations of choices, such as (0, 1, 1, 1, 0)
// The int choices represents a number in binary that corresponds to a position.
// Compare(1, 2) = returns int representing binary value of length n where 0 means agreement of a choice and 1 means disagreement
// Distance(1, 2) = number of choices minimally need to transform one into the other
// Display() = prints out binary string
// Mask(1, 2, 3) = returns true if 1 is equivalent to 2 for choices given by 3

class Configuration {
	public:
		Configuration() {};
		Configuration( string binaryString );
		Configuration( unsigned int n );
		Configuration( unsigned int n, unsigned int positionInt);
		Configuration( unsigned int n, unsigned int k, bool defaultIndex, unsigned int index );
		~Configuration() {};
		unsigned int Compare(Configuration &two);
		unsigned int Distance (Configuration &two);
		bool Display( string &boolString);
		bool Mask(Configuration &Two, Configuration &Mask);
		unsigned int GetChoices() { return choices; };
		unsigned int GetN() { return itsN; };
		unsigned int GetK() { return itsK; };
	private:
		unsigned int choices;
		unsigned int itsN;
		unsigned int itsK;
};

Configuration::Configuration( string binaryString )
{
	itsN = binaryString.size();
	string::iterator iter;
	unsigned int reverseIndex = itsN - 1;
	for ( iter = binaryString.begin(); iter != binaryString.end(); ++iter){
		if ((*iter) == '0')
			choices += pow(2, reverseIndex--);
	}
}

Configuration::Configuration( unsigned int n ):
itsN(n),
itsK(0)
{
	choices = rand() % ( (unsigned int) pow(2,itsN) ) ;
}

Configuration::Configuration( unsigned int n, unsigned int positionInt):
itsN(n),
itsK(0),
choices(positionInt)
{
}

Configuration::Configuration( unsigned int n, unsigned int k, bool defaultIndex, unsigned int index  ):
itsN(n),
itsK(k),
choices(0)
{
	int kk = 0;
	vector<unsigned int> possibleChoices (itsN, 0);
	vector<unsigned int>::iterator iter;
	unsigned int thisChoice = 0;
	
	for(iter = possibleChoices.begin(); iter != possibleChoices.end(); ++iter) {
		*iter = kk++;
	}
	
	if (defaultIndex == true) {
		iter = possibleChoices.begin() + index;
		possibleChoices.erase(iter);
	}

	for (int i = 0; i < itsK; i++) {
		thisChoice = rand() % possibleChoices.size();
		choices += pow(2,possibleChoices[thisChoice]);
		iter = possibleChoices.begin() + thisChoice;
		possibleChoices.erase(iter);
	}
}

unsigned int Configuration::Compare (Configuration &Two) {
	return GetChoices() ^ Two.GetChoices();
}

bool Configuration::Mask (Configuration &Two, Configuration &Mask) {
	unsigned int comparison = Compare(Two);
	return comparison & Mask.GetChoices();
}

unsigned int Configuration::Distance (Configuration &Two) {
	unsigned int differences = Compare(Two);
	return bitcount(differences);
}

bool Configuration::Display( string &result) {
	genConfigString(choices, result, itsN);
}

// misc. functions to handle the output of "binary" strings //

#define TWO(c)     (0x1u << (c))
#define MASK(c)    (((unsigned int)(-1)) / (TWO(TWO(c)) + 1u))
#define COUNT(x,c) ((x) & MASK(c)) + (((x) >> (TWO(c))) & MASK(c))

int bitcount (unsigned int n)  {
	n = COUNT(n, 0) ;
	n = COUNT(n, 1) ;
	n = COUNT(n, 2) ;
	n = COUNT(n, 3) ;
	n = COUNT(n, 4) ;
	  /* n = COUNT(n, 5) ;    for 64-bit integers */
	return n ;
}

void binary(unsigned int number, string & partialBinaryString)
{
	unsigned short int remainder;
	if(number == 0) {
		partialBinaryString.push_back('0');
		return;
	}
	if(number == 1) {
		partialBinaryString.push_back('1');
		return;
	}
	remainder = number%2;
	binary(number >> 1, partialBinaryString);
	if(remainder==0){partialBinaryString.push_back('0');}
	if(remainder==1){partialBinaryString.push_back('1');}
	return;
}

void genConfigString(unsigned int X, string & binaryString, int length)
{
	binaryString.clear();
	if (length > 0)
	{
		for ( int i = 0; i < length; i++)
		{
			binaryString.push_back('0');
		}
		string partialConfig;
		binary(X, partialConfig); // returns ii in binary
		int partialConfigLength = partialConfig.length();
		binaryString.replace( (length - partialConfigLength) , partialConfigLength, partialConfig);
	}
}

bool isEqual(double d1, double d2) 
{
        if(abs(d1-d2) < 0.00000001)
                return true;
        return false;
}
// end misc. functions //

// deals with which choices depend upon which other chocies
class Dependencies {
	public:
		Dependencies( unsigned int n, unsigned int k);
		~Dependencies() {};
		void Show();
	private:
		vector< Configuration > itsDependencies;
};

Dependencies::Dependencies( unsigned int n, unsigned int k ) {
	for ( unsigned int choices = 0; choices < n; choices++) {
		itsDependencies.push_back(Configuration(n, k, true, choices));
	}
}

void Dependencies::Show() {
	vector<Configuration>::iterator iter;
	string temp;
	for (iter = itsDependencies.begin(); iter != itsDependencies.end(); ++iter) {
		(*iter).Display(temp);
		cout << temp << endl;
	}
}

class Choices {
	public:
		Choices( unsigned int choiceIndex, unsigned int itsK, Configuration &mask);
		~Choices() {};
	private:
		unsigned int choiceIndex;
		map<unsigned int, double> itsFitness;
		Configuration configMask;
};

Choices::Choices ( unsigned int choiceIndex, unsigned int itsK, Configuration &mask ) {
	unsigned int itsN = mask.GetN();
	int kk = 0;
	unsigned int choiceInt = 0;
	vector<unsigned int> possibleChoices (itsN, 0);
	vector<unsigned int>::iterator iter;
	unsigned int thisChoice = 0;
	
	for(iter = possibleChoices.begin(); iter != possibleChoices.end(); ++iter) {
		*iter = kk++;
	}
	
	/*
	iter = possibleChoices.begin() + index;
	possibleChoices.erase(iter);
	*/
	
	string temp;
	unsigned int totalCombinations = pow(2, itsK + 1);
	
	mask.Display(temp);
	
	// get rid of the choices that don't matter
	for (unsigned int pp = (itsN); pp  > 0; pp--) {
		if (temp[pp] == '0') {
			iter = possibleChoices.begin() + pp;
			possibleChoices.erase(iter);
		}
	}
	
	// now create the vector that contains 2^(k+1) fitness levels
	unsigned int tempChoice = 0;
	unsigned int testValue;
	unsigned int counter = 0;
	
	for( unsigned int mm = 0; mm < (itsK + 1); mm++) {
	
		tempChoice = 0;
		counter = 0;
		for( iter = possibleChoices.begin(); iter != possibleChoices.end(); ++iter) {
			testValue = pow(2, itsK - counter - 1);
			if ( mm & testValue == testValue ) {
				tempChoice += *iter;
			}
			counter++;
		}
		
		genConfigString(tempChoice, temp, itsN);
		cout << temp << endl;
	}
	
	/*
	for (int i = 0; i < itsK; i++) {
		thisChoice = possibleChoices.size();
		choiceInt += pow(2,possibleChoices[thisChoice]);
		iter = possibleChoices.begin() + thisChoice;
		possibleChoices.erase(iter);
	}
	*/ 
	
	double fitness = (double)rand() / ((double)(RAND_MAX)+(double)(1));
	
}

   /* 
   the individual fitness contributions of each 'choice' are averaged
   to give the 'position' fitness level. 
   */
class Position {
	public:
		Position( unsigned int size);
		Position( unsigned int size, unsigned int positionInt);
		~Position() {};
		double GetFitness() { return fitness; };
		void SetFitness( double f ) { fitness = f; };
	private:
		Configuration config;
		double fitness;
};

Position::Position ( unsigned int size):
config(size),
fitness(0)
{
}

Position::Position ( unsigned int size, unsigned int positionInt):
config(size, positionInt),
fitness(0)
{
}

class Strategy {
	public: 
		Strategy( string options);
		~Strategy();
	private:
		
};

class Landscape {
	public:
		Landscape( unsigned int size, unsigned int K );
		~Landscape();
	private:
		unsigned int itsN;
		unsigned int itsK;
		vector<Position> itsPositions;
		Dependencies itsDeps;
		bool ComputePayoff();
		
};

Landscape::Landscape ( unsigned int size, unsigned int K ):
	itsDeps(size, K),
	itsN(size),
	itsK(K)
{
	
}

class Entity {
	public:
		Entity();
		~Entity();
	private:
		bool Move();
		Position * pPosition;
		Strategy * pStrategy;
		double fitness;
		bool alive;
};

class Multiscape : public Landscape {
	public:
		Multiscape ( unsigned int size, unsigned int K, unsigned int crossK, unsigned int count);
		~Multiscape ();
};

int main () {
	srand( time(NULL) );
	Dependencies test(5,3);
	Configuration mask (7 );
	Choices test2( 1, 3, mask );
	test.Show();
	return 0;
}
