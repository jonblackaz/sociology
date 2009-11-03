#include <iostream>
using namespace std;

class Thing {
	public:
		float exist(int x, int y);
	private:
		vector<vector<float>> Existence;	
}

class Decision {
	private:
		int induce(float, int, int);
		int infer(int);
		int deduce(int);
		int commit(int);
		vector<vector<float>> Particles;
		bool active;
		
	public: 
		int assert(int);	
};

int Decision::induce(float data, int x, int y) {
	
}

int main ()
{
	cout << "You betcha!\n";
	return 0;
}
