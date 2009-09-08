/* 
 * this version takes a pair of landscapes and creates another that takes the key vector of each
 * and adds a second n-length vector to the end of each key that represents the cross-landscape effects.
 * 
 * i have multiple entities implemented with a variety of search functions
 * 
 * 
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
void genConfig ( unsigned long int X, string & config, int length);
int maskTest ( string &, string &, int);
string flipConfigString( const string & configuration );
bool isEqual (double, double);
string zeroHalf ( string );

/************************ Multicounter *******************************/
class Multicounter
	{
	public:
		Multicounter();
		Multicounter( string name ) { itsTotalPeaks = 0; itsTotalRuns = 0; itsName = name; };
		~Multicounter();
		void Add( int n );
		void Add( double added );
		void Replace( double replaced );
		double Average( string which );
		double StdDev() ;
		void Show();
	private:
		string itsName;
		int itsTotalRuns;
		int itsTotalPeaks;
		double itsDouble;
		vector <double> allDoubles;
};

Multicounter::Multicounter ():
	itsTotalPeaks(0),
	itsTotalRuns(0),
	itsDouble(0),
	itsName("Counter")
{
	allDoubles.clear();
}

void Multicounter::Add ( int n )
{
	itsTotalRuns++;
	itsTotalPeaks = itsTotalPeaks + n;
}

void Multicounter::Add ( double added )
{
	itsTotalRuns++;
	allDoubles.push_back(added);
	itsDouble = itsDouble + added;
}

void Multicounter::Replace ( double replaced )
{
	itsTotalRuns++;
	itsDouble = replaced;
	allDoubles.clear();
}

void Multicounter::Show()
{
	cout << itsName << " - " << itsTotalRuns << " Total: " << itsDouble << "  Average: " << this->Average( "double" ) << "  Std. Dev: " << this->StdDev() << endl;
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

double Multicounter::StdDev ()
{
	double ss = 0, sd = 0, average = 0;
	if (allDoubles.size() > 0)
	{
		average = this->Average("double");
		vector <double>::iterator IT;
		for (IT = allDoubles.begin() ; IT != allDoubles.end() ; IT++)
		{
			ss = ss + pow( ((*IT) - average) , 2 ); // adds all the squared deviations
		}
		sd = sqrt ( ss / (itsTotalRuns - 1) );
		return sd;
	}else
	{
	return 0;
	}
}
/***********************************************************************/

/**************************** Payoff **********************************/
class Payoff
	{
	public:
		Payoff (string configuration, double fitness);
		Payoff () { distribution = 0; }
		~Payoff () {};
		string GetConfig() { return itsConfiguration; }
		double GetFitness() { return itsFitness; }
		void SetConfig(string configuration) { itsConfiguration = configuration; }
		void SetFitness(double fitness) { itsFitness = fitness; }
		void AddToDist();
		void SubFromDist();
		void SetDist( unsigned short int value );
		unsigned short int GiveDistribution();
		void DistributionReset();
	private:
		string itsConfiguration;
		double itsFitness;
		bool peak;
		unsigned short int distribution;
};

Payoff::Payoff (string configuration, double fitness)
{
	itsConfiguration = configuration;
	itsFitness = fitness;
	distribution = 0;
}

void Payoff::AddToDist()
{
	distribution++;
}

void Payoff::SetDist(unsigned short int value)
{
	distribution = value;
}

void Payoff::SubFromDist()
{
	if (distribution > 0){ distribution--; }
}

unsigned short int Payoff::GiveDistribution()
{
	return distribution;
}

void Payoff::DistributionReset()
{
	distribution = 0;
}

/* ------------- Start of keymap ------------------------- */

class Keymap
/*  A vector of vectors of keys. For key[x][y], x is the position of
 *  each choice d_, and y is a vector of all possible configurations
 *  that 'matter' for the fitness contribution of d_.
 *  In other words, a vector of vectors like this: key[1][3] = "02102102"
 *  for n = 8, k = 5, where the 2nd, 3rd, 5th, 6th, and 8th choices matter
 */
{
	public:
		Keymap( int inputN, int inputK, bool countSelf);
		Keymap();
		~Keymap();
		string Get( unsigned short int indexOne, unsigned short int indexTwo ) { return key[indexOne][indexTwo]; }
		void Replace( vector < vector <string> > & replacement);
	private:
		vector < vector < string > > key;
		int itsN;
		int itsK;
		vector <bool> choices;
};

Keymap::Keymap(){}

Keymap::~Keymap(){}

Keymap::Keymap ( int inputN, int inputK, bool countSelf):
	itsN( inputN ),
	itsK( inputK ),
	choices( inputN, 0 ),
	key( inputN , vector <string> () )
{
	// cout << "Keymap called..." << endl;
	// Temporary variables below //
	unsigned short int T = pow( 2, itsK + 1);
	unsigned long int ii = 0, jj = 0, ll = 0;
	unsigned short int choiceNum = 0, randomChoice = 0, counter = 0, binaryLength = itsK + 1;
	string binaryConfig, configuration;
	float randomTemp;
	// ************************* //
	if (!countSelf)
	{
		binaryLength = binaryLength -1;
		T = pow( 2, itsK);
	}
	for ( choiceNum = 0; choiceNum < itsN; choiceNum++ )
	{
		for ( jj = 0; jj < itsN; jj++ )
		// setting all choices[] to 0
		{
			choices[jj] = 0;
		}
		if ( countSelf )
		{
			choices[choiceNum] = 1; // will become clear later
		}
		jj = 0;
		while ( jj < itsK )
		// randomly filling choices with 1s and 0s
		{
			randomChoice = rand() % (itsN);
			if ( choices[randomChoice] != 1 )
			// only set choices if it isn't already
			{
				choices[randomChoice] = 1;
				jj++;
			}
		}
		// there's now a vector, choices[], showing which choices "matter"
		for ( ii = 0; ii < T; ii++ )
		// 2^(k+1) is the total number of 'permutations'
		{
			configuration.clear(); // key vector - 0 - not considered, 2 - chosen, 1 - not chosen
			counter = 0; // number of iterations
			genConfig(ii, binaryConfig, binaryLength); // returns ii in binary
			// now binaryConfig looks like i.e. 0010101 (if n=7)
			for ( ll = 0; ll < itsN; ll++ )
			// this loops across 'choices' and if it is set, puts either a 2 or a 1 in configuration in the corresponding position
			{
				if (choices[ll] == 1)
				{
					if (binaryConfig[counter] == '1') // if choice 'll' matters, see whether it is selected
					{
						configuration.push_back('2');
						counter++;
					}
					else
					{
						configuration.push_back('1');
						counter++;
					}
				}
				else
				{
					configuration.push_back('0');
					// counter not incremented, because binaryConfig applies only to choices that 'matter' (k of them)
				}
			}
			key[choiceNum].push_back(configuration);
			// cout << "key[" << choiceNum << "][" << ii << "]" << key[choiceNum][ii] << endl;
		}
	}
	// cout << "Keymap finished..." << endl;
}

void Keymap::Replace( vector < vector <string> > & replacement)
{
	vector < vector <string> >::iterator IT_outer;
	vector < string >::iterator IT_inner;
	for (IT_outer = key.begin() ; IT_outer != key.end() ; IT_outer++)
	{
		for (IT_inner = (*IT_outer).begin() ; IT_inner != (*IT_outer).end() ; IT_inner++)
		{
			(*IT_outer).erase(IT_inner);
		}
	}
	key = replacement;
}

/* ------------- End of Keymap --------------------------- */

/* ------------- Start of Period ------------------------- */
class Timeframe
{
	public:
		Timeframe();
		~Timeframe();
	private:
		unsigned int itsPeriod;
		unsigned int entityPopulation;
};
/* ------------- End of period --------------------------- */

/* ------------- START OF LANDSCAPE ---------------------- */
class Landscape
{
	public:
		friend class Entity;
		friend class Timeframe;
		friend class Search;
		Landscape( int inputN, int inputK, bool fillPayoffs );
		Landscape();
		~Landscape();
		void sumPayoffs( unsigned short int size, int subN, int kOffset );
		void fillPartialPayoffs( int totalK );
		double GetFitness ( string & configuration );
		int GetN() { return itsN; }
		int GetK() { return itsK; }
		string getKey( unsigned short int indexOne, unsigned short int indexTwo ) { return key.Get( indexOne, indexTwo ); }
		void ReplaceKeys( vector < vector <string> > & replacement);
		void ClearDistribution();
		unsigned short int SumDistribution();
		void WriteDistribution ();
		unsigned short int GetDistribution( string itsConfig);
		void UpdateTotal();
		virtual void AddToDistribution( string & itsConfig );
		virtual void SubFromDistribution( string & itsConfig );
		/* need some accessor functions */
		void Write();
	protected:
		map < string, Payoff > d;
		vector< vector< Payoff > > payoffs;
		Keymap key;
		int itsN;
		int itsK;
		unsigned int TotalEntities;
};


Landscape::Landscape()
{
}

Landscape::Landscape( int inputN, int inputK, bool fillPayoffs ):
	itsN( inputN ),
	itsK( inputK ),
	key( inputN, inputK, 1)
{
	if ( fillPayoffs )
	{
		fillPartialPayoffs( itsK );
		sumPayoffs( itsN, itsN, itsK );
	}
}

Landscape::~Landscape(){}

void Landscape::fillPartialPayoffs( int kTotal )
/* fills the "payoffs" vector, which is all the individual choices' fitness levels */
{
	// cout << "fillPartialPayoffs called..." << endl;
	double randomTemp = 0;
	string configuration;
	unsigned short int ii = 0, jj = 0;
	for ( ii = 0 ; ii < itsN ; ii++ ){
		payoffs.push_back( vector < Payoff >() ); // adds an empty vector
		for ( jj = 0 ; jj < pow(2,kTotal+1) ; jj++ )
		{
			configuration = key.Get(ii, jj); // gets the key corresponding to d_ii, for configuration jj
			randomTemp = (double)rand() / ((double)(RAND_MAX)+(double)(1)); // random float bewteen 0 and 1
			payoffs[ii].push_back(Payoff(configuration, randomTemp)); // filling the map with keys from 0 to n^(k+1), and values [0,1]l;
			// cout << "payoffs[" << ii << "][" << jj << "]: " << configuration << "," << randomTemp << ":" << payoffs[ii][jj].GetFitness() << endl;
		}
	}
	// cout << "fillPartialPayoffs finished..." << endl;
}

double Landscape::GetFitness ( string & configuration)
{
	return d[configuration].GetFitness();
}

void Landscape::sumPayoffs( unsigned short int size, int subN, int kOffset )
{
	cout << "sumPayoffs..." << endl;
	unsigned long int ii, jj, kk, success;
	string temp, temp2, configuration;
	unsigned int T = pow( 2, size );
	unsigned int T_two = pow( 2, kOffset);
	cout << T << ":" << T_two << ":" << subN << endl;
	for ( ii = 0; ii < T; ii++ )
	{
		if ( ii % 100000 == 0 ) { cout << ii << endl; }
		configuration.clear();
		genConfig(ii, configuration, size);
		d.insert( pair<string,Payoff> ( configuration, Payoff(configuration, 0) ) );
		// cout << configuration << endl;
		for ( jj = 0 ; jj < subN ; jj++)
		{
			// cout << configuration << "[" << jj << "]" << endl;
			for ( kk = 0; kk < T_two ; kk++ )
			// this loop goes through each decision kk, goes to the payoff map, and adds up all the vectors with configs that match
			{
				temp = payoffs[jj][kk].GetConfig();
				temp2 = configuration;
				success = maskTest(temp, temp2, size);
				if ( success )
				{
					d[configuration].SetFitness( d[configuration].GetFitness() + payoffs[jj][kk].GetFitness() );
					kk = T_two;
					// cout << configuration << ":" << temp << "-->" << payoffs[jj][kk].GetFitness() << endl;
				}
			}
		}
		d[configuration].SetFitness( d[configuration].GetFitness()/ (double) subN );
		// the number of vectors that matched should be equal to the total number (N)
	} // this loop created a vector, d, filled with strings representing each possible combination (2^n)
	cout << "sumPayoffs complete..." << endl;
}

void Landscape::ReplaceKeys( vector < vector <string> > & replacement)
{
	key.Replace(replacement);
}

void Landscape::Write()
{
	// ofstream out( "output.txt" );

/*	if( !out )
	{
		cout << "Couldn't open file."  << endl;
		return 0;
	}
*/	map < string , Payoff >::iterator IT;
	for ( IT = d.begin() ; IT != d.end() ; IT++ )
	{
		cout << (*IT).second.GetConfig() << ":" << (*IT).second.GetFitness() << endl;
	}
	// out.close();
}

void Landscape::ClearDistribution ()
{
	map <string,Payoff>::iterator IT;
	for (IT = d.begin() ; IT != d.end() ; IT++)
	{	
		(*IT).second.DistributionReset();
	}
}

void Landscape::UpdateTotal ()
{
	unsigned short int total = 0;
	map <string,Payoff>::iterator IT;
	for (IT = d.begin() ; IT != d.end() ; IT++)
	{	
		total = total + (*IT).second.GiveDistribution();
	}
	TotalEntities = total;
}

unsigned short int Landscape::GetDistribution( string itsConfig )
{
	return d[itsConfig].GiveDistribution(); 
}

void Landscape::AddToDistribution( string & itsConfig )
{
	d[itsConfig].AddToDist();
}

void Landscape::SubFromDistribution( string & itsConfig )
{
	d[itsConfig].SubFromDist();
}

void Landscape::WriteDistribution ()
{
	unsigned short int number;
	map <string,Payoff>::iterator IT;
	cout << "Final distribution -------" << endl;
	for (IT = d.begin() ; IT != d.end() ; IT++)
	{	
		number = (*IT).second.GiveDistribution();
		if ( number > 0)
		{
			cout << (*IT).second.GetConfig() << "(" << (*IT).second.GetFitness() << "): "<< number << endl;
		}
	}
	cout << "--------------------------" << endl;
}

/* ------------- END OF LANDSCAPE ---------------------- */

/* ------------- Start of Entity ----------------------- */

class Entity
{
	public:
		Entity ( int size, Landscape & LJoined, unsigned int ID);
		Entity () {};
		~Entity () {};
		bool Move( string destination, Landscape & LJoined );
		void Reset( Landscape & LJoined );
		string GetConfig() { return itsPayoff.GetConfig(); }
		double GetFitness() { return itsPayoff.GetFitness(); }
		bool SurvivalCheckDist( bool which );
		Payoff ReturnPayoff() { return itsPayoff; }
		bool IsAlive() { return Alive; }
		void Dies();
	private:
		Payoff itsPayoff;
		int itsSize;
		unsigned int movesTaken;
		bool Alive;
		string Strategy;
		unsigned int itsID;
};

Entity::Entity ( int size, Landscape & LJoined, unsigned int ID ):
	movesTaken(0),
	Alive(1),
	itsSize(size),
	itsID(ID)
{
	/* temporary variables */
	unsigned int randomInt = 0;
	unsigned int divisor = pow( 2 , itsSize );
	string configuration = "";
	randomInt = rand() % ( divisor );
	/* ******************* */
	genConfig ( randomInt , configuration, itsSize );
	LJoined.d[ configuration ].AddToDist();
	itsPayoff.SetConfig( configuration );
	itsPayoff.SetFitness( LJoined.d[configuration].GetFitness() );
}

void Entity::Dies()
{
	if( Alive == 1)
	{
		Alive = 0;
	}else
	{
		cout << "Beating a dead entity" << endl;
	}
}

void Entity::Reset( Landscape & LJoined )
{
	map <string, Payoff >::iterator IT;
	IT = LJoined.d.begin(); 
	itsPayoff.SetConfig( (*IT).second.GetConfig() );
	itsPayoff.SetFitness( (*IT).second.GetFitness() );
}

bool Entity::Move( string destination, Landscape & LJoined )
{
	if ( Alive == 0 ) { return 0; }
	if ( destination == "EMPTY" ) { return 0; }
	string previous = itsPayoff.GetConfig();
	LJoined.SubFromDistribution( previous );
	// cout << "Move from " << itsPayoff.GetConfig() << ":" << itsPayoff.GetFitness() << " to ";
	itsPayoff.SetConfig( destination );
	itsPayoff.SetFitness( LJoined.d[destination].GetFitness() ) ;
	// cout << "to " << itsPayoff.GetConfig() << ":" << itsPayoff.GetFitness() << endl;
	LJoined.AddToDistribution( destination );
	movesTaken++;
	return 1;
}
/* ------------- End of Entity ------------------------- */

/* ------------- JoinedLandscape ----------------------- */
class JoinedLandscape : public Landscape
/* derived from a Landscape, also contains 2 landscapes */
{
	public:
		friend class Search;
		JoinedLandscape( int inputN, int inputK, int inputK_two, unsigned short int numEntities );
		~JoinedLandscape() {}
		int WriteOut ();
		void genJoinedKeys( Landscape & LJoin );
		unsigned short int GetDistribution ( string configuration, int which );
		void AddToDistribution( string & itsConfig );
		void SubFromDistribution( string & itsConfig );
		void genDVector();
		void SetUpDistribution();
		void Kill( Entity & thisEntity );
	private:
		vector < vector< Payoff > > payoffs_joined;
		Landscape LOne;
		Landscape LTwo;
		unsigned short int itsN;
		unsigned short int itsK;
		unsigned short int itsK_two;
		vector <Entity> Entities;
};

JoinedLandscape::JoinedLandscape( int inputN, int inputK, int inputK_two, unsigned short int numEntities):
	itsK ( inputK ),
	itsN ( 2 * inputN ),
	itsK_two ( inputK_two),
	LOne( inputN, inputK, 0 ),
	LTwo( inputN, inputK, 0 )
{
	unsigned short int counter;
	cout << "Landscape formed" << endl;
	genJoinedKeys ( LOne ); // create a list of what choices "matter" to each other
	genJoinedKeys ( LTwo );
	LOne.fillPartialPayoffs ( itsK + itsK_two ); // assign values to each of those possible choices
	LTwo.fillPartialPayoffs ( itsK + itsK_two );
	LOne.sumPayoffs ( itsN , itsN / 2, itsK + itsK_two + 1); // find the fitness of each possible configuration
	LTwo.sumPayoffs ( itsN , itsN / 2, itsK + itsK_two + 1);
	genDVector();
	for ( counter = 0 ; counter < numEntities ; counter++ )
	{ 
		Entities.push_back( Entity( (int) itsN, * this, counter ));
	}
	SetUpDistribution();
}

unsigned short int JoinedLandscape::GetDistribution ( string configuration, int which )
{
	if ( which == 0 ) { return d[configuration].GiveDistribution(); }
	if ( which == 1 ) { return LOne.GetDistribution(configuration); }
	if ( which == 2 ) { return LTwo.GetDistribution(configuration); }
}

void JoinedLandscape::genJoinedKeys( Landscape &LJoin )
{
	Keymap tempKeymap(LJoin.GetN(), itsK_two, 0); // create a keymap to add to the other
	unsigned int T_one = pow (2, itsK + 1);
	unsigned int T_two = pow (2, itsK_two );
	vector < vector < string > > tempKeys ( LJoin.GetN() , vector <string>() );
	unsigned int ii, jj, ll;
	for( ii = 0 ; ii < LJoin.GetN() ; ii++ )
	{
		for( jj = 0 ; jj < T_one ; jj++ )
		{
			for( ll = 0 ; ll < T_two ; ll++ )
			{
			// Generate 2^k_2 partial keys and add them to the end of the tempKeymap
			tempKeys[ii].push_back( LJoin.getKey( ii, jj) + tempKeymap.Get( ii, ll) );
			}
		}
   	}
	LJoin.ReplaceKeys(tempKeys);
}

int JoinedLandscape::WriteOut ()
{
	LOne.Write();
	LTwo.Write();
	return 1;
}

void JoinedLandscape::AddToDistribution( string & itsConfig )
{
	unsigned short int strLength = itsConfig.length() / 2;
	d[itsConfig].AddToDist();
	string first = itsConfig;
	first.replace ( strLength, strLength*2, strLength, '0' );
	string second = itsConfig;
	second.replace ( 0, strLength, strLength, '0' );
	LOne.AddToDistribution(first);
	LTwo.AddToDistribution(second);
}

void JoinedLandscape::SubFromDistribution( string & itsConfig )
{
	unsigned short int strLength = itsConfig.length() / 2;
	d[itsConfig].SubFromDist();
	string first = itsConfig;
	first.replace ( strLength, strLength*2, strLength, '0' );
	string second = itsConfig;
	second.replace ( 0, strLength, strLength, '0' );
	LOne.SubFromDistribution(first);
	LTwo.SubFromDistribution(second);
}

void JoinedLandscape::genDVector()
{
	int counter;
	int T = pow( 2, itsN );
	double average = 0;
	string configuration;
	string configuration2;
	for ( counter = 0 ; counter < T ; counter++ )
	{
		genConfig( counter, configuration, itsN);
		configuration2 = flipConfigString ( configuration );
		average = (LOne.GetFitness( configuration ) + LTwo.GetFitness( configuration2 ) ) / (double) 2;
		d.insert( pair<string,Payoff> ( configuration, Payoff(configuration, average) ) ); 
	}
}

void JoinedLandscape::SetUpDistribution()
{
	int counter;
	string tempConfig, tempConfig2;
	vector < Entity >::iterator IT;
	for ( IT = Entities.begin() ; IT < Entities.end() ; IT++ )
	{
		(*IT).GetConfig();
		tempConfig = (*IT).GetConfig();
		tempConfig2 = zeroHalf(tempConfig);
		LOne.AddToDistribution(tempConfig2);
		tempConfig2 = flipConfigString(tempConfig);
		tempConfig2 = zeroHalf(tempConfig2);
		LTwo.AddToDistribution(tempConfig2);
	}
}

void JoinedLandscape::Kill( Entity & thisEntity )
{
	string itsConfig = thisEntity.GetConfig();
	this->SubFromDistribution( itsConfig );
	thisEntity.Dies();
}

/* ------------- End of JoinedLandscape ---------------- */

/* ------------- Start of Search ----------------------- */

class Search
{
	public:
		Search( JoinedLandscape & thisLandscape, char strategy, float strength );
		~Search() {};
		string checkVicinityNonrandom(JoinedLandscape & thisLandscape, Entity & thisEntity, bool Sequential, int WhichLandscape);
		string checkVicinityBlind(JoinedLandscape & thisLandscape, Entity & thisEntity, bool Sequential, int WhichLandscape);
		string checkVicinityDist(JoinedLandscape & thisLandscape, Entity & thisEntity, bool Sequential, int WhichLandscape);
		void SurvivalCheck(JoinedLandscape & thisLandscape, Entity & thisEntity, bool Sequential, int WhichLandscape);
		void SetMaxFitness(JoinedLandscape & thisLandscape, int WhichLandscape);
		void ShowData();	
	private:
		unsigned short int attemptNumber;
		bool Blind;
		bool Sequential;
		bool Offline;
		short int TotalEntities;
		Multicounter Counter;
		Multicounter CorpseCounter;
		Payoff tempPayoff;
		Payoff maxPayoff;
		float selectionStrength;
		Landscape *pLandscape;
};

Search::Search ( JoinedLandscape & thisLandscape, char strategy, float strength ):
/* I should change this so that instead of a single char, this takes a pattern of chars,
 * i.e., AB or AA, or BA, or whatever. I can get the length of that string, then
 * can do that number of those searches in a repeating pattern.
 */
	Sequential(0),
	selectionStrength(strength)
{
	/* Temporary variables ************ */
	string (Search::*pSearchFunc) ( JoinedLandscape &, Entity &, bool, int ) = 0;
	string move;
	bool success;
	vector <Entity>::iterator IT;
	/* ******************************** */
	switch (strategy)
	{
		case 'A': pSearchFunc = &Search::checkVicinityNonrandom;
			  break;
		case 'B': pSearchFunc = &Search::checkVicinityBlind;
			  break;
		case 'C': pSearchFunc = &Search::checkVicinityDist;
			  break;
	}
	for ( IT = thisLandscape.Entities.begin() ; IT != thisLandscape.Entities.end() ; IT++ )
	{
		thisLandscape.UpdateTotal();
		/* first search attempt */
		move = (*this.*pSearchFunc) ( thisLandscape , (*IT), Sequential, 1 );
		success = (*IT).Move( move, thisLandscape );
		/* second search attempt */
		move = (*this.*pSearchFunc) ( thisLandscape , (*IT), Sequential, 2 );
		success = (*IT).Move( move, thisLandscape );
	}
	SetMaxFitness(thisLandscape, 0);
	for ( IT = thisLandscape.Entities.begin() ; IT != thisLandscape.Entities.end() ; IT++ )
	{
		thisLandscape.UpdateTotal();
		/* selection */
		SurvivalCheck(thisLandscape, (*IT), 0, 0);
		/* ********* */
		if ( (*IT).IsAlive() )
		{
		Counter.Add ( (*IT).GetFitness() );
		}
		CorpseCounter.Add ( (*IT).GetFitness() );
	}
}

void Search::ShowData()
{
	Counter.Show();
}

string Search::checkVicinityNonrandom( JoinedLandscape & thisLandscape, Entity & thisEntity, bool Sequential, int WhichLandscape )
/* this version finds the move with the greatest improvement choices constant */
{
	tempPayoff.SetConfig ( thisEntity.GetConfig() ); // tempPayoff is current value for thisEntity
	if ( !thisEntity.IsAlive() ) { return "EMPTY"; }
	string configuration = tempPayoff.GetConfig(); // configuration is a temporary string to be returned as the potential move
	unsigned short int i, j, strLength, searchLength;
	double tempAverage = 0;
	vector <Payoff> averages;
	strLength = configuration.length();

	switch (WhichLandscape)
	{
		case 0: pLandscape = &thisLandscape;
			searchLength = strLength;
			break;
		case 1: pLandscape = &thisLandscape.LOne;
			searchLength = strLength / 2;
			break;
		case 2: pLandscape = &thisLandscape.LTwo;
			configuration = flipConfigString( configuration );
			tempPayoff.SetConfig ( flipConfigString ( tempPayoff.GetConfig() ) );
			searchLength = strLength / 2;
			break;
	}
	tempPayoff.SetFitness ( (*pLandscape).d[configuration].GetFitness() );
	for ( i = 0 ; i < searchLength ; i++)
	{
		configuration = tempPayoff.GetConfig(); // reset configuration
		if ( configuration[i] == '0' ) { configuration[i] = '1'; } // flip one decision
		else { configuration[i] = '0'; };	
		tempAverage = (*pLandscape).d[configuration].GetFitness(); // get the new value for the flipped string
		averages.push_back ( Payoff(configuration, tempAverage) ); // add to the list of possibilties
	}

	configuration = tempPayoff.GetConfig(); // resetting configuration to the Entity's current value
	if (WhichLandscape == 2) { configuration = flipConfigString (configuration); } // ugly hack to make the second landscape work
	tempAverage = tempPayoff.GetFitness(); // resetting fitness to the Entity's current value

	vector <Payoff>::iterator IT;
	for ( IT = averages.begin() ; IT != averages.end() ; IT++ )
	{
		if ( tempAverage < (*IT).GetFitness() ) // if one of the averages is greater
			{
			configuration = (*IT).GetConfig(); // set the move information
			tempAverage = (*IT).GetFitness(); // similar
			}
	} // the move information should be the greatest possible
	if ( tempAverage <= tempPayoff.GetFitness() ) { return "EMPTY"; } // if less, don't recommend a move
	if ( isEqual ( tempAverage, tempPayoff.GetFitness() ) ) { return "EMPTY"; } // if equal, no reason to move
	if ( WhichLandscape == 2 ) { return flipConfigString ( configuration ); } // flip the string back for the potential move
	return configuration;
}

string Search::checkVicinityBlind( JoinedLandscape & thisLandscape, Entity & thisEntity, bool Sequential, int WhichLandscape )
/* randomly selects one move */
{
	tempPayoff.SetConfig ( thisEntity.GetConfig() ); // tempPayoff is current value for thisEntity
	if ( !thisEntity.IsAlive() ) { return "EMPTY"; }
	string configuration = tempPayoff.GetConfig(); // configuration is a temporary string to be returned as the potential move
	unsigned short int i, strLength, searchLength;
	double tempFitness = 0;
	strLength = configuration.length();

	switch (WhichLandscape)
	{
		case 0: pLandscape = &thisLandscape;
			searchLength = strLength;
			break;
		case 1: pLandscape = &thisLandscape.LOne;
			searchLength = strLength / 2;
			break;
		case 2: pLandscape = &thisLandscape.LTwo;
			configuration = flipConfigString( configuration );
			tempPayoff.SetConfig ( flipConfigString ( tempPayoff.GetConfig() ) );
			searchLength = strLength / 2;
			break;
	}

	i = rand() % (searchLength); // get a random number of appropriate length
	tempPayoff.SetFitness ( (*pLandscape).d[configuration].GetFitness() ); // current value (to be compared)

	if ( configuration[i] == '0' ) { configuration[i] = '1'; }
	else { configuration[i] = '0'; };

	tempFitness = (*pLandscape).d[configuration].GetFitness();       // new value
	if ( tempFitness <= tempPayoff.GetFitness() ) { return "EMPTY"; } // if less, don't recommend a move
	if ( isEqual (tempFitness, tempPayoff.GetFitness() ) ) { return "EMPTY"; } // if equal, no reason to move
	if ( WhichLandscape == 2 ) { return flipConfigString ( configuration ); } // flip the string back for the potential move

	return configuration;
}

string Search::checkVicinityDist( JoinedLandscape & thisLandscape, Entity & thisEntity, bool Sequential, int WhichLandscape )
/* randomly selects one move weighted by distribution
 * Doesn't work right now because the sublandscapes don't have their distributions set properly by the Entity constructor.
 * 
 */
{
	tempPayoff.SetConfig ( thisEntity.GetConfig() );
	if ( !thisEntity.IsAlive() ) { return "EMPTY"; }
	string configuration = tempPayoff.GetConfig(), partialConfig; // configuration is the string that will ultimately be returned as a move
	/* Temporary variables ***** */
	int i, j;
	unsigned short int strLength = configuration.length(), searchLength;
	double tempFitness; // holds the fitness level of the value to be returned
	map <string,int> chance;
	vector <string> Configs;
	/*****************************/
	switch (WhichLandscape)
	{
		case 0: pLandscape = &thisLandscape;
			searchLength = strLength;
			break;
		case 1: pLandscape = &thisLandscape.LOne;
			searchLength = strLength / 2;
			partialConfig = configuration.substr(searchLength, searchLength);
			tempPayoff.SetFitness ( (*pLandscape).d[configuration].GetFitness() );
			break;
		case 2: pLandscape = &thisLandscape.LTwo;
			configuration = flipConfigString( configuration );
			tempPayoff.SetConfig ( flipConfigString ( tempPayoff.GetConfig() ) );		
			searchLength = strLength / 2;
			partialConfig = configuration.substr(searchLength, searchLength);
			tempPayoff.SetFitness ( (*pLandscape).d[configuration].GetFitness() );
			break;
	}
	Configs.push_back( zeroHalf(configuration) );
	for ( i = 0 ; i < searchLength ; i++ )
	{
		configuration = tempPayoff.GetConfig();
		if ( configuration[i] == '0' ) { configuration[i] = '1'; }
		else { configuration[i] = '0'; }
		Configs.push_back( zeroHalf(configuration) );
	}	
	unsigned int total = 0;
	unsigned int tempDist;
	vector <string>::iterator IT;
	for ( IT = Configs.begin() ; IT != Configs.end() ; IT++ )
	{
		tempDist = (*pLandscape).d[(*IT)].GiveDistribution();
		chance.insert( pair<string,int> ( (*IT), tempDist ) );
		total += tempDist;
	}
	if (total > 0) { i = rand() % total; }
	map <string,int>::reverse_iterator IT_TWO;
	for ( IT_TWO = chance.rbegin() ; IT_TWO != chance.rend() ; IT_TWO++ ) // this random number might be 1 off
	{
		i = i-(*IT_TWO).second;
		configuration = (*IT_TWO).first;
		if (i < 0) { break; }
	}
	configuration.replace(searchLength, strLength, partialConfig);
	tempFitness = (*pLandscape).d[configuration].GetFitness();
	if ( tempFitness <= tempPayoff.GetFitness() ) { return "EMPTY"; } // if less, don't recommend a move
	if ( isEqual (tempFitness, tempPayoff.GetFitness() ) ) { return "EMPTY"; } // if equal, no reason to move
	if ( WhichLandscape == 2 )
	{
		return flipConfigString ( configuration );
	} // flip the string back for the potential move
	return configuration;
}

void Search::SetMaxFitness( JoinedLandscape & thisLandscape, int WhichLandscape )
{
	Payoff tempFitness(" ",0);
	vector <Entity>::iterator IT;
	for (IT = thisLandscape.Entities.begin(); IT != thisLandscape.Entities.end(); IT++)
	{
		if( (*IT).IsAlive() ){
			if( (*IT).GetFitness() > tempFitness.GetFitness() && !isEqual( (*IT).GetFitness(), tempFitness.GetFitness() ) )
			{
				if( thisLandscape.GetDistribution ( (*IT).GetConfig(), WhichLandscape ) > 0 )
				{
					tempFitness.SetConfig( (*IT).GetConfig() );
					tempFitness.SetFitness( (*IT).GetFitness() );
				}
			}
		}
	}
	maxPayoff.SetFitness( tempFitness.GetFitness() );
	maxPayoff.SetConfig ( tempFitness.GetConfig() );
}

void Search::SurvivalCheck(JoinedLandscape & thisLandscape, Entity & thisEntity, bool Sequential, int WhichLandscape)
{
	if ( thisEntity.IsAlive() )
	{
		double max = maxPayoff.GetFitness();
		double current = thisEntity.GetFitness();
		double random = (double)rand() / ((double)(RAND_MAX)+(double)(1));
		double survival = pow ( current / max , selectionStrength );
		if ( survival < random )
		{
			thisLandscape.Kill(thisEntity);
		}
	}
}


/* ------------- End of Search ------------------------- */

/* ----------------- Start of Session ------------------ */

class Session
{
	public:
		Session( bool defaults );
		~Session() {};
	private:
		unsigned short int totalRuns;
		unsigned short int totalEntities;
		unsigned short int searchStrategy;
		unsigned short int walks;
		unsigned short int n;
		unsigned short int k;
		unsigned short int k_two;
		vector <float> selectionStrength;
};

Session::Session( bool defaults ):
	n(5),
	k(3),
	k_two(3),
	walks(10),
	totalEntities(25)
{
	/* various temp variables* */
	unsigned short int i=0, j=0, L=0, M=0, totalRuns=0;;
	int random_seed;
	string letter = "A";
	string Move;
	bool success = 0;
	srand ( time(NULL) );
	while ( totalRuns <= 0 ){
		cout << "How many runs? ";
		cin >> totalRuns;
	}
	/* *********************** */
	/* If not using defaults, get the appropriate parameters */
	if (!defaults)
	{
		n=0; k=666; k_two=666; walks=0; totalEntities = 200;
		while ( n <= 0 || n >=11 ){
			cout << "Select a size: ";
			cin >> n;
		}
		cout << "Random seed (use same number for equal landscape): ";
		cin >> random_seed;
		srand (random_seed);
		// srand ( time(NULL) );
		while ( k > (n - 1)  || k < 0 ) {
			cout << "Select k_1 (" << n-1 << " or less): ";
			cin >> k;
		}
		while ( k_two > n - 1  || k_two < 0 ){
			cout << "Select k_2 (" << n-1 << " or less): ";
			cin >> k_two;
		}
		while ( walks <= 0 ){
			cout << "Select number of steps: ";
			cin >> walks;
		}
	}
	/* ************************************************* */
	cout << endl << "1) Keeps landscape constant (non-blind)"
	     << endl << "2) Optimizes all choices at the same time (non-blind)"
	     << endl << "3) Sequential and blind"
	     << endl << "9) Use all methods" << endl;
	cout << "Select a search strategy: " ;
	cin >> searchStrategy;
	char searchChar = 'A';
	for ( i = 1; (i < searchStrategy) && (i != 3); i++)
	{
		searchChar++;
	}
	for ( i = 0; i < walks ; i++ )
	{
		selectionStrength.push_back(1);
	}
	JoinedLandscape * pLandscape = 0;
	vector <Search> Searches;
 	for( i = 0; i < totalRuns; i++)
 	{
		pLandscape = new JoinedLandscape(n, k, k_two, totalEntities);
		for ( j = 0 ; j < walks ; j++ )
		{
			Searches.push_back( Search( * pLandscape, searchChar, selectionStrength[j] )) ;
		}
		pLandscape->WriteDistribution();
		Searches[walks-1].ShowData();
		delete pLandscape;
		pLandscape = 0;
 	}
}

/* ----------------- End of Session -------------------- */

int main()
{
	int defaults;
	for ( defaults = 2 ; (defaults != 0) && (defaults != 1) ; cin >> defaults )
	{
		cout << "Defaults (0=no, 1=yes): ";
	}
	Session mySession( (bool) defaults );
	return 0;
}

/* ------------- OTHER FUNCTIONS  ---------------------- */

void genConfig(unsigned long int X, string & binaryConfig, int length)
{
	binaryConfig.clear();
	if (length > 0)
	{
		for ( int i = 0; i < length; i++)
		{
			binaryConfig.push_back('0');
		}
		string partialConfig;
		binary(X, partialConfig); // returns ii in binary
		int partialConfigLength = partialConfig.length();
		binaryConfig.replace( (length - partialConfigLength) , partialConfigLength, partialConfig);
	}
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

bool isEqual(double d1, double d2) 
{
        if(abs(d1-d2) < 0.00000001)
                return true;
        return false;
}

string zeroHalf ( string configuration )
{
	unsigned short int strLength;
	strLength = configuration.length() / 2;
	configuration.replace(strLength, strLength * 2, strLength, '0');
	return configuration;
}
