#include <iostream>
#include <vector>
#include <string>
using namespace std;

class ORG_FORM
{
  public:
	ORG_FORM();
	ORG_FORM(int sizeX, int sizeY);
	// ORG_FORM (int size, ORG_FORM model);
	~ORG_FORM();
	int SetChoices(vector< vector<int> > model);
	int GetSizeX () const { return itsSizeX ; }
	int GetSizeY () const { return itsSizeY ; }
	int GetChoice(int i, int j) const { return itsForm[i][j] ; }
	bool operator== (const ORG_FORM & comparison) const; // overloading equivalence operator
  private:
	int itsSizeX;
	int itsSizeY;
	vector< vector<int> > itsForm;
};

ORG_FORM::ORG_FORM(int sizeX, int sizeY) // fills a vector of length "size" with 0s
{
	itsSizeX = sizeX;
	itsSizeY = sizeY;
	for (int i=0; i<sizeX; i++)
	{
	    itsForm.push_back(vector <int>(itsSizeY,0));
	}
	cout << sizeX << endl;
}

ORG_FORM::ORG_FORM()
{
	itsSizeX = 0;
	itsSizeY = 0;
}
ORG_FORM::~ORG_FORM()
{
	itsForm.clear();
}

int ORG_FORM::SetChoices(vector< vector<int> > model)
{
	int i = 0;
	while ( i < itsSizeX)
		{
		itsForm[i].assign( model[i].begin(), model[i].end() ); // copy contents from one to other
		}
	return 0;
}

bool ORG_FORM::operator== (const ORG_FORM & comparison) const
{
	if ( (itsSizeX == comparison.GetSizeX()) && (itsSizeY == comparison.GetSizeY()) )
	{
		int i = 0;
		int j = 0;
		while( i < itsSizeX)
			{
			while( j < itsSizeY )
				{
				if (itsForm[i][j] != comparison.GetChoice(i,j))
					{
					return 0;
					}
				}
			}
		return 1;
	}
	else
	{
		return 0;
	}
}

int main()
{
	ORG_FORM formOne;
	ORG_FORM formTwo;
	if (formOne == formTwo){ cout << "Yes" << endl; }
	// formOne.SetChoices(test); // copies the contents of "test" to "formOne"
	int i = formOne.GetSizeX();
	while(i < 5)
	   {
	      // cout << formOne.GetChoice(i) << endl;
	      i++;
	   }
	return 0;
}
