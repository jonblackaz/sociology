/* 
	STILL NEED TO WORK ON RETURNING WHEN USING SEQUENTIAL, DO TESTING OF WHAT VALUES ARE BEING COMPARED, CHANGING LANDSCAPE, ETC.

 * this version takes a pair of landscapes and creates another that takes the key vector of each
 * and adds a second n-length vector to the end of each key that represents the cross-landscape effects.
 * i have offline/online search implemented, blind/nonblind implemented (to some degree), search by distribution implemented
 * search strategy is a property of each entity
 * Entities can be added at any stage, but currently their starting configuration is either opposite the most popular, or random
 * All this needs to be tested
 */
 
      /////  C and K seem to work, not B or J
     /////
    /////

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
		Multicounter( string name );
		~Multicounter();
		void Add( int n );
		void Add( double added );
		void Replace( double replaced );
		double Average( string which );
		double StdDev() ;
		void Show();
		void Write();
	private:
		string itsName;
		int itsTotalRuns;
		int itsTotalPeaks;
		double itsDouble;
		vector <double> allDoubles;
};

Multicounter::Multicounter ( string name ):
	itsTotalPeaks(0),
	itsTotalRuns(0),
	itsDouble(0),
	itsName( name )
{
	allDoubles.clear();
}

Multicounter::Multicounter ():
	itsTotalPeaks(0),
	itsTotalRuns(0),
	itsDouble(0),
	itsName( "Counter" )
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
	cout << itsName << " - " << itsTotalRuns << " Ttl: " << itsDouble << "  Mean: " << this->Average( "double" ) << "  Std.Dev: " << this->StdDev() << endl;
}

void Multicounter::Write()
{
	ofstream out;
	
	out.open( "output.txt", ios::app );
	
	if( !out )
	{
		cout << "Couldn't open file."  << endl;
	}
	out << itsName << "," << itsTotalRuns << "," << itsDouble << "," << this->Average( "double" ) << "," << this->StdDev() << endl;
	out.close();
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

/**************************** Strategy ********************************/

class Strategy
{
	public:
		Strategy( const char choice, const unsigned short int breakpoint=0 );
		Strategy() {};
		~Strategy() {};
		bool isOnline() { return Online; }
		bool isSequential() { return Sequential; }
		bool isIntegrated() { return Integrated; }
		char isCalled() { return Label; }
		unsigned short int GetBreakpoint() { return Breakpoint; }
		unsigned short int WhichLandscape( unsigned int period, unsigned int subperiod );
	private:
		bool Online;
		bool Sequential;
		bool Integrated;
		char Label;
		unsigned short int Breakpoint;
};

Strategy::Strategy( char choice, const unsigned short int breakpoint ):
Label(choice),
Breakpoint(breakpoint)
{
	switch ( choice ) // this is where the magic happens, and searches are defined
	{
		case 'A': 
			  Online = 1; Sequential = 0; Integrated=0;
			  break;
		case 'B': 
			  Online = 1; Sequential = 0; Integrated=0;
			  break;
		case 'C': 
			  Online = 0; Sequential = 0; Integrated=0;
			  break;
		case 'D':
			  Online = 1; Sequential = 0; Integrated=0;
			  break;
		case 'E':
			  Online = 0; Sequential = 0; Integrated=0;
			  break;
		case 'F':
			  Online = 1; Sequential = 1; Integrated=0;
			  break;
		case 'G':
			  Online = 0; Sequential = 1; Integrated=0;
			  break;
		case 'H':
			  Online = 1; Sequential = 1; Integrated=0;
			  break;
		case 'I':
			  Online = 0; Sequential = 1; Integrated=0;
			  break;
		case 'J':
			  Online = 1; Sequential = 1; Integrated=1;
			  break;
		case 'K': 
			  Online = 0; Sequential = 1; Integrated=1;
			  break;
		case 'L': 
			  Online = 1; Sequential = 0; Integrated=1;
			  break;
		case 'M': 
			  Online = 0; Sequential = 0; Integrated=1;
			  break;
		case 'N': 
			  Online = 1; Sequential = 1; Integrated=1;
			  break;
		case 'O': 
			  Online = 0; Sequential = 1; Integrated=1;
			  break;
	}
}

unsigned short int Strategy::WhichLandscape( unsigned int period, unsigned int subperiod )
{
	if ( isIntegrated() ) { return 0; }
	if ( !isSequential() ) { return subperiod; }
	else
	{
		unsigned short int toReturn;
		period <= Breakpoint
		? toReturn = 1
		: toReturn = subperiod;
		return toReturn;
	}
}

/********************** END STRATEGY **********************************/

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
		Landscape( unsigned short int inputN, unsigned short int inputK, bool fillPayoffs, short int id );
		Landscape();
		~Landscape();
		void sumPayoffs( unsigned short int size, int subN, int kOffset );
		void fillPartialPayoffs( int totalK );
		double GetFitness ( string & configuration );
		unsigned short int GetN() { return itsN; }
		unsigned short int GetK() { return itsK; }
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
		unsigned short int getId() { return itsId; }
	protected:
		map < string, Payoff > d;
		vector< vector< Payoff > > payoffs;
		Keymap key;
		unsigned short int itsN;
		unsigned short int itsK;
		unsigned int TotalEntities;
		unsigned short int itsId;
};


Landscape::Landscape()
{
}

Landscape::Landscape( unsigned short int inputN, unsigned short int inputK, bool fillPayoffs, short int id ):
	itsN( inputN ),
	itsK( inputK ),
	key( inputN, inputK, 1),
	itsId( id)
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
	// cout << "sumPayoffs..." << endl;
	unsigned long int ii, jj, kk, success=0, power;
	string temp, temp2, configuration;
	unsigned int T = pow( 2, size );
	unsigned int T_two = pow( 2, kOffset);
	// cout << T << ":" << T_two << ":" << subN << endl;
	for ( ii = 0; ii < T; ii++ )
	{
		if ( ii % 100000 == 0 ) { cout << ii << endl; }
		configuration.clear();
		genConfig(ii, configuration, size);
		d.insert( pair<string,Payoff> ( configuration, Payoff(configuration, 0) ) );
		// cout << configuration << endl;
		for ( jj = 0 ; jj < subN ; jj++)
		{
			success=0;
			power=kOffset;
			// cout << configuration << "[" << jj << "]" << endl;
			temp = payoffs[jj][0].GetConfig();
			// cout << temp;
			for ( kk = 0; kk < size; kk++ )
			{
				if( temp[kk] != '0' ){
					if ( configuration[kk] == '1' )
					{
						power--;
						success += pow(2,power);
					}
				}
			}
			d[configuration].SetFitness( d[configuration].GetFitness() + payoffs[jj][success].GetFitness() );
			// below, i need to generate the correct number
/*			for ( kk = 0; kk < T_two ; kk++ )
 *			// this loop goes through each decision kk, goes to the payoff map, and adds up all the vectors with configs that match
 *			{
 *				temp = payoffs[jj][kk].GetConfig();
 *				temp2 = configuration;
 *				success = maskTest(temp, temp2, size);
 *				if ( success )
 *				{
 *					d[configuration].SetFitness( d[configuration].GetFitness() + payoffs[jj][kk].GetFitness() );
 *					kk = T_two;
 *					// cout << configuration << ":" << temp << "-->" << payoffs[jj][kk].GetFitness() << endl;
 *				}
 *			}
 */		}
		d[configuration].SetFitness( d[configuration].GetFitness()/ (double) subN );
		// the number of vectors that matched should be equal to the total number (N)
	} // this loop created a vector, d, filled with strings representing each possible combination (2^n)
	// cout << "sumPayoffs complete..." << endl;
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

/* ------------- Move Handler ----------------------- */

class MoveHandler
{
	public:
		MoveHandler ( int n, char strategy );
		MoveHandler () {};
		~MoveHandler() {};
		void Reset();
		short int GetPossible( Landscape & thisLandscape, const string currentConfig, const unsigned int period, const unsigned int subperiod );
		string GetPastLocation( short int whichMove );
	private:
		Strategy itsStrategy;
		bool isStatic; 						// may be unnecessary
		vector <vector <unsigned short int> > possibleMoves; 	// a matrix of all available moves
		vector <short int> landscapeHistory; 			// an array of which landscapes were called (last time, time before, time before)
		vector <string> pastConfigs; 				// an array of last 3 configurations
		vector <short int> pastSuggestions; 			// an arrray of the last ints that were returned
		unsigned short int itsSize;				// How many possible choices?
		unsigned int movesCounter; 				// number times the movehandler was asked for a move
		
};	

MoveHandler::MoveHandler ( int n, char strategy ):
itsSize(n),
itsStrategy(strategy)
{
	if ( itsStrategy.isIntegrated() ){
		itsSize = 2 * itsSize;
		}
	itsSize = itsSize / 2;
	Reset();
}

string MoveHandler::GetPastLocation( short int whichMove)
{
	vector <string>::reverse_iterator ITR;
	ITR = pastConfigs.rbegin();
	string pastMove = (*(ITR - whichMove - 1));
	return pastMove;
}

void MoveHandler::Reset()
{
	vector< vector< unsigned short int> >::iterator ITT;
	
	isStatic = 0;
	// clear possibleMoves
	for ( ITT = possibleMoves.begin(); ITT != possibleMoves.end(); ITT++)
	{
		(*ITT).clear();
	}
	possibleMoves.clear();

	// fill possibleMoves according to lengthOne and lengthTwo
	unsigned short int ii=0, jj=0;
	for ( ii = 0; ii < itsSize; ii++)
	{
		possibleMoves.push_back( vector <unsigned short int>(0,0) );
		for ( jj = 0; jj < itsSize; jj++ )
		{
			possibleMoves[ii].push_back(jj);
		}
	}
}

short int MoveHandler::GetPossible( Landscape & thisLandscape, const string currentConfig, const unsigned int period, const unsigned int subperiod )
// This method is only used for keeping temporary track of which
// moves have been made from a particular location in blind (online)
// searches. It's not a search in and of itself.
{
	// cout << "MoveHandler:GetPossible" << endl;
	// testing ---------------------
	unsigned short int shiznit;
	vector<unsigned short int>::iterator ITP;
	vector<vector<unsigned short int> >::iterator ITU;
	cout << "<----->" << endl;
	for (ITU = possibleMoves.begin(); ITU != possibleMoves.end(); ITU++)
	{
		for (ITP = (*ITU).begin(); ITP != (*ITU).end(); ITP++)
		{
			shiznit = (*ITP);
			cout << "[" << shiznit << "]";	
		}
		cout << endl;
	}
	cout << "O-----O" << endl;
	// end testing -----------------

	// temp variables //
	short int counter = 0;
	short int valueToReturn = -2;
	short int choice;
	short int i, j;
	// ************* //

	// if the vector-vector is empty, return an invalid move
	short int length = possibleMoves.size();
	if (length == 0) { return -1; }
	
	unsigned short int whichLandscape = itsStrategy.WhichLandscape( period, subperiod);
	
	if ( whichLandscape == 1 || whichLandscape == 0)
	{
		vector< vector<unsigned short int> >::iterator ITT;
		// find all the possibilities remaining //
		for (ITT = possibleMoves.begin(); ITT != possibleMoves.end(); ITT++)
		{	
			if( (*ITT).size() > 0 ) { counter++; }
		}

		// counter is now the total number of nonempty vectors in possibleMoves //		
		if ( counter == 0 )
		// well, if there aren't any, then return an invalid move //
		{ 
			isStatic = 1;
			valueToReturn = -1;
		}
		else
		{
			valueToReturn = 0;
			choice = rand() % counter;
			cout << choice << ":" << counter << endl;
			counter = 0;
		
			// this here ugly loop gets the appropriate random vector (choice) //
			for (ITT = possibleMoves.begin(); ITT != possibleMoves.end(); ITT++)
			{
				if( (*ITT).size() == 0 && counter <= choice)
				{
					valueToReturn++;
				}
			
				if( (*ITT).size() > 0 && counter <= choice)
				{
					valueToReturn++;
					counter++;
				}
			}
		
			// the valueToReturn is one great because it starts counting with one... //
			// and we're using an index that starts with 0				 //
			valueToReturn--;
		
			// If sequential, and before breakpoint, get rid of this choice for next //
			if ( ((period <= itsStrategy.GetBreakpoint() ) && (itsStrategy.isSequential())) || whichLandscape == 0 )
			{
				possibleMoves[valueToReturn].clear();
			}
		}
	}
	
	if ( whichLandscape == 2 && !isStatic )
	{
		// what was the last move anyway? //
		vector <short int>::reverse_iterator ITR;
		ITR = pastSuggestions.rbegin();
		i = (*ITR);
		
		// get a random move for landscape 2 //
		j = possibleMoves[i].size();
		choice = rand() % j;
		valueToReturn = possibleMoves[i][choice];
		possibleMoves[i].erase(possibleMoves[i].begin()+choice);
	}

	// record keeping //
	landscapeHistory.push_back(whichLandscape);
	pastConfigs.push_back(currentConfig);
	pastSuggestions.push_back(valueToReturn);
	
	// but not too much record keeping //
	if ( pastConfigs.size() > 3 ) { pastConfigs.erase( pastConfigs.begin() ); }
	if ( landscapeHistory.size() > 3) { landscapeHistory.erase( landscapeHistory.begin() ); }
	if ( pastSuggestions.size () > 3) { pastSuggestions.erase( pastSuggestions.begin() ); }

	cout << "whichLandscape: " << whichLandscape << endl;
	cout << "Static: " << isStatic << endl;
	cout << "valueToReturn: " << valueToReturn << endl;
	return valueToReturn;
}

/* ------------- END MOVERS ---------------------------- */

/* ------------- Start of Entity ----------------------- */

class Entity
{
	public:
		Entity ( int size, Landscape & LJoined, unsigned int ID, char itsStrategy );
		Entity () {};
		~Entity () {};
		bool Move( string destination, Landscape & LJoined, int searchPeriod );
		void Reset( Landscape & LJoined );
		string GetConfig() { return itsPayoff.GetConfig(); }
		string GetConfig( short int whichLandscape );
		double GetFitness() { return itsPayoff.GetFitness(); }
		bool SurvivalCheckDist( bool which );
		Payoff ReturnPayoff() { return itsPayoff; }
		bool IsAlive() { return Alive; }
		void Dies();
		void ReturnTo( Landscape & thisLandscape, short int which );
		Strategy itsStrategy;
		MoveHandler itsMoves;
		double GetCumulative ( unsigned short int which);
		void updateCumulative( double LJoinFitness, double LOneFitness, double LTwoFitness );
		bool isCumulative ( unsigned short int which );
		void SetCumulative ( bool value, unsigned short int which );
		unsigned int itsId;

	private:
		Payoff itsPayoff;
		double cumulativeFitness[3]; // 0=joined landscape, 1=sub one, 2=sub two
		bool useCumulative[3];
		unsigned short int updatesCalled;
		int itsSize;
		bool Alive;
};

Entity::Entity ( int size, Landscape & LJoined, unsigned int ID, char strategy ):
	Alive(1),
	itsSize(size),
	itsId(ID),
	itsStrategy(strategy),
	updatesCalled(0),
	itsMoves(size,strategy)
{
	useCumulative[0] = 0;
	useCumulative[1] = 0;	
	useCumulative[2] = 0;
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

string Entity::GetConfig( short int whichLandscape )
{
	if (whichLandscape == 1)
	{
		string temp = itsPayoff.GetConfig();
		temp = temp.substr(0,itsSize/2);
		return temp;
	}
	else if (whichLandscape == 2)
	{
		string temp = itsPayoff.GetConfig();
		temp = temp.substr(itsSize,itsSize/2);
		return temp;
	}
	else
	{
		return itsPayoff.GetConfig();
	}
	
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

void Entity::updateCumulative( double LJoinFitness, double LOneFitness, double LTwoFitness )
{
	cumulativeFitness[0] = (cumulativeFitness[0] * (double) updatesCalled + LJoinFitness) / double (updatesCalled + 1);
	cumulativeFitness[1] = (cumulativeFitness[1] * (double) updatesCalled + LOneFitness) / double (updatesCalled + 1);
	cumulativeFitness[2] = (cumulativeFitness[2] * (double) updatesCalled + LTwoFitness) / double (updatesCalled + 1);
	updatesCalled++;
}

void Entity::ReturnTo( Landscape & thisLandscape, short int which)
{
	if ( Alive == 1 ){ 
		string previous = itsPayoff.GetConfig();
		thisLandscape.SubFromDistribution( previous );
		// cout << "Move from " << itsPayoff.GetConfig() << ":" << itsPayoff.GetFitness() ;
		string LastConfig = itsMoves.GetPastLocation( which );
		itsPayoff.SetConfig( LastConfig );
		itsPayoff.SetFitness( thisLandscape.GetFitness(LastConfig) ) ;
		// cout << "to " << itsPayoff.GetConfig() << ":" << itsPayoff.GetFitness() << endl;
		thisLandscape.AddToDistribution( LastConfig );
	}
}

bool Entity::isCumulative ( unsigned short int which )
{
	return useCumulative[which];
}

double Entity::GetCumulative ( unsigned short int which )
{
	return cumulativeFitness[which];
}

void Entity::SetCumulative ( bool value, unsigned short int which )
{
	useCumulative[which] = value;
}

bool Entity::Move( string destination, Landscape & LJoined, int searchPeriod )
{
	if ( Alive == 0 ) { return 0; }
	if ( destination == "EMPTY" ) { return 0; }
	string previous = itsPayoff.GetConfig();
	double previousFitness = itsPayoff.GetFitness();
	LJoined.SubFromDistribution( previous );
	cout << "Move(" << searchPeriod << " ID-" << itsId << "(" << itsStrategy.isCalled() << ") from "  << itsPayoff.GetConfig() << ":" << itsPayoff.GetFitness() << " to ";
	itsPayoff.SetConfig( destination );
	itsPayoff.SetFitness( LJoined.d[destination].GetFitness() ) ;
	cout << itsPayoff.GetConfig() << ":" << itsPayoff.GetFitness() << endl;
	LJoined.AddToDistribution( destination );
	return 1;
}

/* ------------- End of Entity ------------------------- */

/* ------------- Beginning of population --------------- */




/* ------------- End of population --------------------- */

/* ------------- JoinedLandscape ----------------------- */
class JoinedLandscape : public Landscape
/* derived from a Landscape, also contains 2 landscapes */
{
	public:
		friend class Search;
		JoinedLandscape( unsigned short int inputN, unsigned short int inputK, unsigned short int inputK_two, unsigned short int numEntities, char initialStrategy, string searchString );
		~JoinedLandscape() {}
		int WriteOut ();
		void genJoinedKeys( Landscape & LJoin );
		unsigned short int GetDistribution ( string configuration, int which );
		void AddToDistribution( string & itsConfig );
		void SubFromDistribution( string & itsConfig );
		void genDVector();
		void SetUpDistribution();
		void Kill( Entity & thisEntity );
		void AddEntity( string method, int number );
		void AdjustPayoffs ( double amount, char how, unsigned short int which );
		string strategies;
		void SwitchCumulativeOff( unsigned short int which );
		void SwitchCumulativeOn( unsigned short int which );
		unsigned short int GetN() { return itsN; }
		unsigned short int GetK() { return itsK; }
	private:
		vector < vector< Payoff > > payoffs_joined;
		Landscape LOne;
		Landscape LTwo;
		unsigned short int itsN;
		unsigned short int itsK;
		unsigned short int itsK_two;
		vector <Entity> Entities;
};

JoinedLandscape::JoinedLandscape( unsigned short int inputN, unsigned short int inputK, unsigned short int inputK_two, unsigned short int numEntities, char initialStrategy, string searchString ):
	itsK ( inputK ),
	itsN ( 2 * inputN ),
	itsK_two ( inputK_two),
	LOne( inputN, inputK, 0, 1 ),
	LTwo( inputN, inputK, 0, 2 ),
	strategies (searchString)
{
	itsId = 0;
	unsigned short int counter, searchStringLength, i;
	cout << "Landscape formed" << endl;
	genJoinedKeys ( LOne ); // create a list of what choices "matter" to each other
	genJoinedKeys ( LTwo );
	LOne.fillPartialPayoffs ( itsK + itsK_two ); // assign values to each of those possible choices
	LTwo.fillPartialPayoffs ( itsK + itsK_two );
	LOne.sumPayoffs ( itsN , itsN / 2, itsK + itsK_two + 1); // find the fitness of each possible configuration
	LTwo.sumPayoffs ( itsN , itsN / 2, itsK + itsK_two + 1);
	genDVector();
	if (initialStrategy != 'X'){
		for ( counter = 0 ; counter < numEntities ; counter++ )
		{ 
			Entities.push_back( Entity( (int) itsN, * this, counter, initialStrategy ));
		}
	}
	if (initialStrategy == 'X'){
	counter = 0 ;
		while ( counter < numEntities )
		{ 
			searchStringLength = searchString.length();
			for ( i = 0; i != searchStringLength ; i++ )
			{
				Entities.push_back( Entity( (int) itsN, * this, counter++, searchString[i] ));
			}
		}
	}
	SetUpDistribution();
	cout << "Landscape complete" << endl;
}

unsigned short int JoinedLandscape::GetDistribution ( string configuration, int which )
{
	if ( which == 0 ) { return d[configuration].GiveDistribution(); } // joined = 0
	if ( which == 1 ) { return LOne.GetDistribution(configuration); } // sub one = 1
	if ( which == 2 ) { return LTwo.GetDistribution(configuration); } // sub two = 2
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

void JoinedLandscape::AddEntity (string method, int number)
{
	unsigned short int maximum;
	string target;
	if (method == "O")
	{
		vector <Entity>::iterator IT;
		for (IT = Entities.begin(); IT != Entities.end(); IT++)
		{
			if ( maximum < GetDistribution( (*IT).GetConfig(), 0 ) )
				{
				maximum = GetDistribution( (*IT).GetConfig(), 0 );
				}
		}
	} // I should move this to another, separate function
	int id = Entities.size();
	for (int i=0; i < number; i++)
	{
		Entities.push_back( Entity( (int) itsN, * this, id++, 'A' ) );
		Entities.back().Move( target, *this, 2 ); // sets this newly-created entity's configuration to "target"
	}
}

void JoinedLandscape::Kill( Entity & thisEntity )
{
	string itsConfig = thisEntity.GetConfig();
	this->SubFromDistribution( itsConfig );
	thisEntity.Dies();
}

void JoinedLandscape::AdjustPayoffs ( double amount, char how, unsigned short int which )
{
	// i need to do 2 things here:
	// create a way of adjusting by distribution and create a way of adjusting randomly
	// overall levels of fitness (across the landscape) should probably remain constant?
	// combine this with the idea that fitness is cumulative (network externalities)
	// 
	// Other things to consider - should the niche be based on each decision, a position as a whole
	// or all adjacent positions?
	/* *********** Adjust by distribution *************** */
	map <string, Payoff>::iterator IT;
	for ( IT = this->d.begin() ; IT != this->d.end(); IT++)
	{
		
	}
	// GetDistribution;
	/* ************************************************** */
}

void JoinedLandscape::SwitchCumulativeOff( unsigned short int which )
{
	vector <Entity>::iterator IT;
	for ( IT=Entities.begin(); IT != Entities.end(); IT++)
	{
		(*IT).SetCumulative( 0, which );
	}
}

void JoinedLandscape::SwitchCumulativeOn( unsigned short int which )
{
	vector <Entity>::iterator IT;
	for ( IT=Entities.begin(); IT != Entities.end(); IT++)
	{
		(*IT).SetCumulative( 1, which );
	}
}

/* ------------- End of JoinedLandscape ---------------- */

/* ------------- Start of Search ----------------------- */

class Search
{
	public:
		Search( JoinedLandscape & thisLandscape, char strategy, float strength, unsigned short int period, unsigned short int totalSearches);
		~Search() {};
		string checkVicinityNonrandom(JoinedLandscape & thisLandscape, Entity & thisEntity);
		string localSearch(JoinedLandscape & thisLandscape, Entity & thisEntity);
		string checkVicinityDist(JoinedLandscape & thisLandscape, Entity & thisEntity);
		string checkVicinityIntegrated(JoinedLandscape & thisLandscape, Entity & thisEntity);
		string returnLegitimate( JoinedLandscape & thisLandscape, Entity & thisEntity);
		void findLegitimate( JoinedLandscape & thisLandscape, int whichLandscape );
		void SurvivalCheck(JoinedLandscape & thisLandscape, Entity & thisEntity, int whichLandscape);
		void SetMaxFitness(JoinedLandscape & thisLandscape, int whichLandscape);
		void returnSequentials(JoinedLandscape & thisLandscape, unsigned short int period);
		void ShowData();
		void WriteData();	
	private:
		short int thisPeriod;
		short int thisSubperiod;
		unsigned short int attemptNumber;
		short int TotalEntities;
		vector < double> ChoiceDistributions;
		Multicounter Counter;
		Multicounter CorpseCounter;
		vector <Multicounter> SubCounters;
		Payoff tempPayoff;
		Payoff maxPayoff;
		float selectionStrength;
		Landscape *pLandscape;
		JoinedLandscape &thisLandscape;
};

Search::Search ( JoinedLandscape & passedLandscape, char strategy, float strength, unsigned short int period, unsigned short int totalSearches):
/* I should change this so that instead of a single char, this takes a pattern of chars,
 * i.e., AB or AA, or BA, or whatever. I can get the length of that string, then
 * can do that number of those searches in a repeating pattern.
 */
	selectionStrength(strength),
	thisPeriod(period),
	Counter(),
	CorpseCounter("Failed"),
	ChoiceDistributions(passedLandscape.GetN(), 0.5),
	thisLandscape(passedLandscape)
{
	cout << thisPeriod << endl;
	// ***Temporary variables********** //
	string (Search::*pSearchFunc) ( JoinedLandscape &, Entity & ) = 0;
	string move;
	string tempConfig, tempConfigFlipped;
	string temp, pattern;
	unsigned short int jj=0, strLength=0, counter=0, multiplier=1; // parallel search by default
	bool success;
	unsigned short int breakpoint = 30;
	bool sequentialChange = false;
	vector <Entity>::iterator IT;
	// ******************************** //
	findLegitimate(thisLandscape, 0);
	if (period > 1 ) returnSequentials(thisLandscape, thisPeriod);

	if ( strategy == 'X' )
	{
		/*
		 *  This generates the counters which keep track of population and subpopulation data.
		 */		
		
		pattern = thisLandscape.strategies;
		strLength = pattern.length();
		for ( jj = 0 ; jj < strLength; jj++ )
		{
			temp = "CounterA_main";
			temp[7] = pattern[jj];
			SubCounters.push_back(Multicounter (temp));
			temp = "CounterA_LOne";
			temp[7] = pattern[jj];
			SubCounters.push_back(Multicounter (temp));
			temp = "CounterA_LTwo";
			temp[7] = pattern[jj];
			SubCounters.push_back(Multicounter (temp));
		}
	
	}

	// move all entities, once for each subperiod //
	for ( thisSubperiod = 1; thisSubperiod <= 2 ; thisSubperiod++ )
	{
		for ( IT = thisLandscape.Entities.begin() ; IT != thisLandscape.Entities.end() ; IT++ )
		{
			switch ( (*IT).itsStrategy.isCalled() ) // searche functions are defined here
			{
				case 'A': pSearchFunc = &Search::checkVicinityNonrandom;
					  break;
				case 'B': pSearchFunc = &Search::localSearch;
					  break;
				case 'C': pSearchFunc = &Search::localSearch;
					  break;
				case 'D': pSearchFunc = &Search::checkVicinityDist;
					  break;
				case 'E': pSearchFunc = &Search::checkVicinityDist;
					  break;
				case 'F': pSearchFunc = &Search::localSearch;
					  break;
				case 'G': pSearchFunc = &Search::localSearch;
					  break;
				case 'H': pSearchFunc = &Search::checkVicinityDist;
					  break;
				case 'I': pSearchFunc = &Search::checkVicinityDist;
					  break;
				case 'J': pSearchFunc = &Search::localSearch;
					  break;
				case 'K': pSearchFunc = &Search::localSearch;
					  break;
				case 'L': pSearchFunc = &Search::returnLegitimate;
					  break;
				case 'M': pSearchFunc = &Search::returnLegitimate;
					  break;
				case 'N': pSearchFunc = &Search::returnLegitimate;
					  break;
				case 'O': pSearchFunc = &Search::returnLegitimate;
					  break;
			}

			/* this adds appropriate output data, BEFORE selection */
			if ( strategy == 'X' && (*IT).IsAlive() )
			{

				tempConfig = (*IT).GetConfig();
				tempConfigFlipped = flipConfigString( tempConfig );

				for ( jj = 0 ; jj < strLength; jj++ )
				{
					if( (*IT).itsStrategy.isCalled() == pattern[jj] ) {
						counter = jj * 3;
						SubCounters[counter++].Add ( (*IT).GetFitness() ); 
						SubCounters[counter++].Add ( thisLandscape.LOne.GetFitness( tempConfig ) ); 
						SubCounters[counter++].Add ( thisLandscape.LTwo.GetFitness( tempConfigFlipped ) );
					}
				}

			}	

			if ( (*IT).itsStrategy.isSequential() && sequentialChange == true)
			{
				(*IT).itsMoves.Reset();
			}
				
			/*  search attempt */
			move = (*this.*pSearchFunc) ( thisLandscape , (*IT));
			success = (*IT).Move( move, thisLandscape, thisSubperiod );
		}
	}
	// these are necessary for the selection step
	SetMaxFitness(thisLandscape, 0); 
	thisLandscape.UpdateTotal();
	for ( IT = thisLandscape.Entities.begin() ; IT != thisLandscape.Entities.end() ; IT++ )
	{
		tempConfig = (*IT).GetConfig();
		tempConfigFlipped = flipConfigString( tempConfig );

		//  Updates cumulative fitness for the entity (*IT)
		(*IT).updateCumulative( (*IT).GetFitness(), thisLandscape.LOne.GetFitness ( tempConfig ), thisLandscape.LTwo.GetFitness ( tempConfigFlipped) );

		/* selection */
		SurvivalCheck(thisLandscape, (*IT), 0);
		/* ********* */
		
		/* 
		 *  This loop helps keep track of population-level statistics.
		 */ 
		
		if ( (*IT).IsAlive() )
		{
			Counter.Add ( (*IT).GetFitness() );
		}else
		{
			CorpseCounter.Add ( (*IT).GetFitness() );
		}
	}
}

void Search::ShowData()
{
	Counter.Show();
	CorpseCounter.Show();
	vector <Multicounter>::iterator IT;
	for ( IT = SubCounters.begin() ; IT != SubCounters.end() ; IT++ )
	{
		(*IT).Show();
	}
}

void Search::WriteData()
{
	Counter.Write();
	CorpseCounter.Write();
	vector <Multicounter>::iterator IT;
	for ( IT = SubCounters.begin() ; IT != SubCounters.end() ; IT++ )
	{
		(*IT).Write();
	}
}

void Search::returnSequentials(JoinedLandscape & thisLandscape, unsigned short int period)
// This function moves entities back to their locations last round 
// if their current fitness is lower than it was previously
{
	string oldConfig, currentConfig;
	vector <Entity>::iterator ITT;
	for ( ITT = thisLandscape.Entities.begin(); ITT != thisLandscape.Entities.end(); ITT++)
	{
		if( (*ITT).itsStrategy.isOnline() )
		{
			currentConfig = (*ITT).GetConfig();
			if ( (*ITT).itsStrategy.isIntegrated() )
			{
				pLandscape = &thisLandscape;
				oldConfig = (*ITT).itsMoves.GetPastLocation(-2);
			}			
			else
			{
				// First, check and see if the most recent move was better. If so, return there. //
				period < (*ITT).itsStrategy.GetBreakpoint()
					? pLandscape = &thisLandscape.LOne
					: pLandscape = &thisLandscape.LTwo;
				oldConfig = (*ITT).itsMoves.GetPastLocation(-1);

				if ( ( pLandscape->GetFitness(currentConfig) <= pLandscape->GetFitness(oldConfig) )
				|| !( isEqual (pLandscape->GetFitness(currentConfig), pLandscape->GetFitness(oldConfig)) ) )
					(*ITT).ReturnTo(thisLandscape, -1);
				
				// at this point, we check and see if the next farther back location was better
				pLandscape = &thisLandscape.LOne;
				currentConfig = (*ITT).GetConfig(); // refresh the location
				oldConfig = (*ITT).itsMoves.GetPastLocation(-2); 						
			}
			if ( ( pLandscape->GetFitness(currentConfig) <= pLandscape->GetFitness(oldConfig) )
			|| !( isEqual (pLandscape->GetFitness(currentConfig), pLandscape->GetFitness(oldConfig)) ) )
			{
				(*ITT).ReturnTo(thisLandscape, -2);
			}
		}
	}
}

string Search::checkVicinityNonrandom( JoinedLandscape & thisLandscape, Entity & thisEntity)
/* this version finds the move with the greatest improvement choices constant */
{
	tempPayoff.SetConfig ( thisEntity.GetConfig() ); // tempPayoff is current value for thisEntity
	if ( !thisEntity.IsAlive() ) { return "EMPTY"; }
	string configuration = tempPayoff.GetConfig(); // configuration is a temporary string to be returned as the potential move
	unsigned short int i, j, strLength, searchLength;
	unsigned short int whichLandscape = thisEntity.itsStrategy.WhichLandscape(thisPeriod, thisSubperiod);
	double tempAverage = 0;
	vector <Payoff> averages;
	strLength = configuration.length();
	

	switch (whichLandscape)
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
	if (whichLandscape == 2) { configuration = flipConfigString (configuration); } // ugly hack to make the second landscape work
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
	if ( !thisEntity.itsStrategy.isOnline() ){
		if ( tempAverage <= tempPayoff.GetFitness() ) { return "EMPTY"; } // if less, don't recommend a move
		if ( isEqual ( tempAverage, tempPayoff.GetFitness() ) ) { return "EMPTY"; } // if equal, no reason to move
	}
	if ( whichLandscape == 2 ) { return flipConfigString ( configuration ); } // flip the string back for the potential move
	return configuration;
}

string Search::localSearch( JoinedLandscape & thisLandscape, Entity & thisEntity)
/* randomly selects one move */
{
	if ( !thisEntity.IsAlive() ) { return "EMPTY"; }

	unsigned short int whichLandscape = thisEntity.itsStrategy.WhichLandscape(thisPeriod, thisSubperiod);
	short int i;
	double tempFitness = 0;
	string configuration = thisEntity.GetConfig();

	switch (whichLandscape)
	{
		case 0: pLandscape = &thisLandscape;
			break;
		case 1: pLandscape = &thisLandscape.LOne;
			break;
		case 2: pLandscape = &thisLandscape.LTwo;
			configuration = flipConfigString( configuration );
			break;
	}
	
	i = thisEntity.itsMoves.GetPossible( (*pLandscape), thisEntity.GetConfig(), thisPeriod, thisSubperiod );

	if ( i < 0 ) { return "EMPTY"; }
	
	if ( configuration[i] == '0' ) { configuration[i] = '1'; }
	else { configuration[i] = '0'; }
	
	tempFitness = (*pLandscape).d[configuration].GetFitness(); // new value
	
	if ( !thisEntity.itsStrategy.isOnline() ){
		if ( tempFitness <= thisEntity.GetFitness() ) { return "EMPTY"; } // if less, don't recommend a move
		if ( isEqual (tempFitness, thisEntity.GetFitness() ) ) { return "EMPTY"; } // if equal, no reason to move
	}

	if ( whichLandscape == 2 ) { return flipConfigString ( configuration ); } // flip the string back for the potential move
	return configuration;
}

string Search::checkVicinityDist( JoinedLandscape & thisLandscape, Entity & thisEntity)
/* randomly selects one move weighted by distribution
 * it only considers possibilities within one move of itself
 * 
 */
{
	unsigned short int whichLandscape = thisEntity.itsStrategy.WhichLandscape(thisPeriod, thisSubperiod);
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
	switch (whichLandscape)
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
	if ( !thisEntity.itsStrategy.isOnline() ){
		if ( tempFitness <= tempPayoff.GetFitness() ) { return "EMPTY"; } // if less, don't recommend a move
		if ( isEqual (tempFitness, tempPayoff.GetFitness() ) ) { return "EMPTY"; } // if equal, no reason to move
	}
	if ( whichLandscape == 2 )
	{
		return flipConfigString ( configuration );
	} // flip the string back for the potential move
	return configuration;
}

void Search::findLegitimate( JoinedLandscape & thisLandscape, int whichLandscape )
/* Finds most popular choices, moves there */
{
	int length = thisLandscape.GetN();
	int i;
	vector < vector < int > > totals ( length, vector <int>(2, 0));
	string temp;
	vector <Entity>::iterator IT;
	for (IT = thisLandscape.Entities.begin(); IT != thisLandscape.Entities.end(); IT++)
	{
		if( (*IT).IsAlive() ){
			temp = (*IT).GetConfig();
			for ( i = 0; i != length; i++ )
			{
				totals[i][0]++;
				if ( temp[i] == '1' )
					{ totals[i][1]++; }
			}
		}
	}
	vector < vector < int > >::iterator ITT;
	i = 0;
	for ( ITT = totals.begin(); ITT != totals.end(); ITT++ )
	{
		if ( ( (*ITT)[0] > 0 ) && ( (*ITT)[1] >= 0 ) )
			{ ChoiceDistributions[i] = (double) (*ITT)[1] / (*ITT)[0]; } 
		else	{ ChoiceDistributions[i] = .5; }
		// cout << (*ITT)[1] << "/" << (*ITT)[0] << " = " << ChoiceDistributions[i] << endl;
		i++;
	}
	totals.clear();
}

string Search::returnLegitimate( JoinedLandscape & thisLandscape, Entity & thisEntity )
{
	tempPayoff.SetConfig ( thisEntity.GetConfig() ); // tempPayoff is current value for thisEntity
	if ( !thisEntity.IsAlive() ) { return "EMPTY"; }
	unsigned short int whichLandscape = thisEntity.itsStrategy.WhichLandscape(thisPeriod, thisSubperiod);

	string configuration = tempPayoff.GetConfig(); // configuration is a temporary string to be returned as the potential move
	string temp;
	short int i;
	bool setToOne = false;
	unsigned short int strLength, searchLength;
	double tempFitness = 0;
	double randomChoice = 0;
	strLength = configuration.length();
	tempPayoff.SetFitness ( thisLandscape.d[configuration].GetFitness() ); // current value (to be compared)

	switch (whichLandscape)
	{
		case 0: pLandscape = &thisLandscape;
			searchLength = strLength;
			break;
		case 1: pLandscape = &thisLandscape.LOne;
			searchLength = strLength/2;
			break;
		case 2: pLandscape = &thisLandscape.LTwo;
			searchLength = strLength/2;
			configuration = flipConfigString( configuration );
			tempPayoff.SetConfig ( flipConfigString ( tempPayoff.GetConfig() ) );
			break;
	}


	i = thisEntity.itsMoves.GetPossible( ( *pLandscape), thisEntity.GetConfig(), thisPeriod, thisSubperiod );
	if ( i < 0 ) { return "EMPTY"; }			
	
	short int offset = 0;
	if (whichLandscape == 2)
	{
		offset = searchLength;
	}
	
	randomChoice = (double)rand() / ((double)(RAND_MAX)+(double)(1));
	// cout << ChoiceDistributions[i] << ":" << randomChoice << endl;
	if ( ChoiceDistributions[i+offset] > randomChoice || isEqual(ChoiceDistributions[i+offset], randomChoice) )
	{
		setToOne = true;
	}
	if (setToOne == true) { configuration[i] = '1'; }
	else { configuration[i] = '0'; }
	
	if (tempPayoff.GetConfig() == configuration) { return "EMPTY"; }
	
	if (whichLandscape == 2) { configuration = flipConfigString (configuration); }
	tempFitness = thisLandscape.d[configuration].GetFitness();       // new value
	if ( !thisEntity.itsStrategy.isOnline() ){
		if ( tempFitness <= tempPayoff.GetFitness() ) { return "EMPTY"; } // if less, don't recommend a move
		if ( isEqual (tempFitness, tempPayoff.GetFitness() ) ) { return "EMPTY"; } // if equal, no reason to move
	}
	if ( whichLandscape == 2 ) { return flipConfigString ( configuration ); } // flip the string back for the potential move

	return configuration;
	
}  

void Search::SetMaxFitness( JoinedLandscape & thisLandscape, int whichLandscape )
{
	Payoff tempFitness(" ",0);
	vector <Entity>::iterator IT;
	for (IT = thisLandscape.Entities.begin(); IT != thisLandscape.Entities.end(); IT++)
	{
		if( (*IT).IsAlive() ){
			if( (*IT).GetFitness() > tempFitness.GetFitness() && !isEqual( (*IT).GetFitness(), tempFitness.GetFitness() ) )
			{
				if( thisLandscape.GetDistribution ( (*IT).GetConfig(), whichLandscape ) > 0 )
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

void Search::SurvivalCheck(JoinedLandscape & thisLandscape, Entity & thisEntity, int whichLandscape)
{

	if ( thisEntity.IsAlive() )
	{
		double max = maxPayoff.GetFitness();
		double current = 0;
		string tempConfig = thisEntity.GetConfig();
		string tempConfigFlipped = flipConfigString( tempConfig );

		// Below is somewhat painful logic for figuring out the Entity's proper fitness to test against.
		// If survival is based on cumulative fitness for this landscape, use that for this landscape. Otherwise, check
		// to ses if we are on the joined landscape. If not, check to see if we are on landscape sub 1. If so, use
		// that current fitness, otherwise, use the sub 2 current fitness
		// Otherwise, if this landscape is the joined landscape, check to see if each of the other
		// fitnesses used is supposed to be cumulative or not. Add the proper values together, and divide by 2.
		// "checkthis" ? "do if true" : "do if false"
		current = (thisEntity.isCumulative(whichLandscape) ? ( thisEntity.GetCumulative(whichLandscape) )
			: ( whichLandscape != 0 ?
				( whichLandscape == 1 ? thisLandscape.LOne.GetFitness ( tempConfig )
						      : thisLandscape.LTwo.GetFitness ( tempConfigFlipped )
				)
			 	:
			 	(( thisEntity.isCumulative(1) ? thisEntity.GetCumulative(1)
			 				     : thisLandscape.LOne.GetFitness (tempConfig) )
				+
				( thisEntity.isCumulative(2) ? thisEntity.GetCumulative(2)
			 				     : thisLandscape.LTwo.GetFitness (tempConfigFlipped) )) / (double) 2	     
			  ));

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
		bool WriteToFile;
		vector <float> selectionStrength;
};

Session::Session( bool defaults ):
	n(5),
	k(3),
	k_two(3),
	walks(100),
	totalEntities(200),
	WriteToFile(0)
{
	/* various temp variables* */
	unsigned short int i=0, j=0, L=0, M=0, totalRuns=0;
	int random_seed;
	char searchChar;
	string searchString;
	string Move;
	bool Offline = 1;
	bool success = 0;
	bool usecumulative = 0;
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
		cout << "How many total entities? ";
		cin >> totalEntities;
		cout << "Random seed (use same number for equal landscape): ";
		cin >> random_seed;
		srand (random_seed);
		// srand ( time(NULL) );
		while ( k > (n - 1)  || k < 0 ) {
			cout << "Select k_1 (" << n-1 << " or less): ";
			cin >> k;
		}
		while ( k_two > n  || k_two < 0 ){
			cout << "Select k_2 (" << n << " or less): ";
			cin >> k_two;
		}
		while ( walks <= 0 ){
			cout << "Select number of steps: ";
			cin >> walks;
		}
		cout << "Use cumulative: ";
		cin >> usecumulative;
		cout << "Write to file (0 for no | 1 for yes): ";
		cin >> WriteToFile;
	}
	/* ************************************************* */
	cout << endl << "A) Find greatest potential payoff (non-blind)"
	     << endl << "B) Offline parallel"
	     << endl << "C) Online parallel (blind)"
	     << endl << "D) Offline parallel by local distribution"
	     << endl << "E) Online parallel by local distribution (blind)" 
	     << endl << "F) Offline sequential"
	     << endl << "G) Online sequential (blind)"
	     << endl << "H) Offline sequential by local distribution"
	     << endl << "I) Online sequential by local distribution (blind)"
	     << endl << "J) Offline integrated (sequential)"
	     << endl << "K) Online integrated (blind/sequential)"
	     << endl << "L) Offline parallel by distribution"
	     << endl << "M) Online parallel by distribution"
	     << endl << "N) Offline sequential by distribution"
	     << endl << "O) Online sequential by distribution"
	     << endl << "X) Define a distribution*" << endl;
	cout << "Select a search strategy: " ;
	cin >> searchChar;
/*	char searchChar = 'A';
	for ( i = 1; (i < searchStrategy) && (i != 6); i++)
	{
		searchChar++;
	}
*/	
	if (searchChar == 'X')
	{
		cout << "Enter distribution (i.e., ACEG means 1/4 each of A, C, E, and G)." << endl;
		cout << "Pattern: ";
		cin >> searchString;
	}
	for ( i = 0; i < walks ; i++ )
	{
		selectionStrength.push_back(.025);
	}
	JoinedLandscape * pLandscape = 0;
	Search * pSearch = 0;
	vector <Search * > Searches;
 	for( i = 0; i < totalRuns; i++)
 	{
		pLandscape = new JoinedLandscape(n, k, k_two, totalEntities, searchChar, searchString);
		if (usecumulative == 1) { pLandscape->SwitchCumulativeOn(1); }
		for ( j = 0 ; j < walks ; j++ )
		{
			pSearch = new Search( * pLandscape, searchChar, selectionStrength[j], j+1, walks );
			Searches.push_back( pSearch ) ;
			// if( ((2*j) % walks) == 0 ) { SwitchCumulativeOff() }
			if (WriteToFile) {Searches[j]->WriteData();}
		}
		pLandscape->WriteDistribution();
		Searches[walks-1]->ShowData();
		vector <Search * >::iterator ITT;
		for (ITT = Searches.begin(); ITT != Searches.end(); ITT++)
		{
			delete (*ITT);
		}
		Searches.clear();
		delete pLandscape;
		pLandscape = 0;
		pSearch = 0;
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
