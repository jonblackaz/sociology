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

// This class handles configurations of choices, such as (0, 1, 1, 1, 0)
// The int choices represents a number in binary that corresponds to a position.
// Compare(1, 2) = returns int representing binary value of length n where 1 means agreement of a choice and 0 means disagreement
// Distance(1, 2) = number of choices minimally need to transform one into the other
// Display() = prints out binary string
// Mask(1, 2, 3) = returns true if 1 is equivalent to 2 for choices given by 3

class Configuration {
	public:
		unsigned int Compare(Configuration &one, Configuration &two);
		unsigned short int Distance (Configuration one, Configuration two);
		void Display();
		bool Mask(Configuration &One, Configuration &Two, Configuration &Mask);
		unsigned short int GetChoices() { return choices; };
	private:
		unsigned int choices;
		unsigned short int length;
};

unsigned int Configuration::Compare (Configuration &One, Configuration &Two) {
	return One.GetChoices() ^ Two.GetChoices();
}

void Display() {
	string temp;
	genConfigString(choices, temp, length);
	cout << temp;
}

/* misc. functions to handle the output of "binary" strings */

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


/* end misc. functions */

class Position {
	public:
		Position();
		~Position();
		bool Compare(Position Other);
	private:
		Configuration itsConfig;
		double itsFitness; 
};

bool Position::Compare(Position Other) {
	
}

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
	unsigned int test;
	test = 12 ^ 4;
	string a, b, c;
	genConfigString(12, a, 4);
	genConfigString(4, b, 4);
	genConfigString(test, c, 4);
	cout << c << ": " << a << " ^ " << b << endl;
	return 0;
}
