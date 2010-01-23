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
/* Working with lots of strings of length N that need to be combined and compared. The landscape is made up of a set of positions. Each position has an associated fitness level and a set of choices. Fitness levels are calculated by finding the average of each binary choice's individual fitness. Each choice's fitness, in turn, depends on k other choices for that position. */

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
		unsigned int Compare(Configuration &two);
		unsigned short int Distance (Configuration &two);
		void Display();
		bool Mask(Configuration &Two, Configuration &Mask);
		unsigned short int GetChoices() { return choices; };
	private:
		unsigned int choices;
		unsigned short int length;
};

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

void Display() {
	string temp;
	genConfigString(choices, temp, length);
	cout << temp;
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
class Position {
	public:
		Position( unsigned int size);
		~Position();
		
	private:
		Configuration config;
		double fitness;
};

class Landscape {
	public:
		Landscape( unsigned int size );
		~Landscape();
	private:
		unsigned int itsN;
		unsigned int itsK;
		vector<Position> itsPositions;
		
};

Landscape::Landscape ( unsigned int size ) {
		
}


int main () {
	
	return 0;
}
