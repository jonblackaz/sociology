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
unsigned int bitcount(unsigned int n);

// This class handles configurations of choices, such as (0, 1, 1, 1, 0)
// The int choices represents a number in binary that corresponds to a position.
// Compare(1, 2) = returns int representing binary value of length n where 0 means agreement of a choice and 1 means disagreement
// Distance(1, 2) = number of choices minimally need to transform one into the other
// Display() = prints out binary string
// Mask(1, 2, 3) = returns true if 1 is equivalent to 2 for choices given by 3

class Configuration {
	public:
		Configuration( unsigned int n );
		Configuration( unsigned int n, unsigned int positionIndex, unsigned int k );
		~Configuration();
		unsigned int Compare(Configuration &two);
		unsigned short int Distance (Configuration &two);
		void Display( String &boolString);
		bool Mask(Configuration &Two, Configuration &Mask);
		unsigned short int GetChoices() { return choices; };
	private:
		unsigned int choices;
		unsigned short int length;
};

Configuration::Configuration( unsigned int n ) {
	this.length = n;
	this.choices = rand () % pow(2,n) ;
}

Configuration::Configuration( unsigned int n, unsigned int positionIndex) {
	this.length = n;
	this.choices = positionIndex;
}

Configuration::Configuration( unsigned int n, unsigned int k, bool doesNothing ) {
	this.length = n;
	vector<unsigned int> = possibleChoices (n, 1);
	unsigned int thisChoice = 0;
	for (int i = 0; i < k; i++) {
		thisChoice = rand () % possibleChoices.size();
		this.choices += pow(2,possibleChoices[thisChoice]);
		possibleChoices.erase(thisChoice, thisChoice);
	}
}

unsigned int Configuration::Compare (Configuration &Two) {
	return this.GetChoices() ^ Two.GetChoices();
}

bool Configuration::Mask (Configuration &Two, Configuration &Mask) {
	unsigned int comparison = this.Compare(&Two);
	return comparison & Mask.GetChoices();
}

unsigned int Configuration::Distance (Configuration &Two) {
	unsigned int differences = this.Compare(&Two);
	return bitcount(differences);
}

void Display( String &result) {
	genConfigString(this.choices, result, this.length);
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
		~Dependencies();
	private:
		vector<vector<Configuration>> itsDependencies;
};

Dependencies:Dependencies( unsigned int n, unsigned int k ) {
	for ( int choices = 0; choices < n; choices++) {
		itsDependencies.push_back( vector < Configuration >() );
		for (int interactions = 0; interactions <= k; interactions++) {
			itsDependencies[choices].push_back(Configuration(n));
		}
	}
}

class Choices {
	public:
		Choices( unsigned int choiceIndex, Configuration mask);
		~Choices();
	private:
		unsigned int choiceIndex;
		double fitness;
		Configuration itsConfiguration;
};

Choices::Choices ( unsigned int choiceIndex, Configuration mask ) {
	fitness = (double)rand() / ((double)(RAND_MAX)+(double)(1));
}

   /* 
   the individual fitness contributions of each 'choice' are averaged
   to give the 'position' fitness level. 
   */
class Position {
	public:
		Position( unsigned int size);
		~Position();
		
	private:
		Configuration config;
		double fitness;
};

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
		bool ComputePayoff();
		
};

Landscape::Landscape ( unsigned int size ) {
	
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
}

class Multiscape : public Landscape {
	public:
		Multiscape ( unsigned int size, unsigned int K, unsigned int crossK, unsigned int count);
		~Multiscape ();
};

void main () {
	
	return 0;
}
