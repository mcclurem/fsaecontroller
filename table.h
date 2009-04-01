
#include "stdlib.h"
#include <vector>
using namespace std;
class Table
{
	public:
		Table(int size_a, int size_b, double delt_a, double delt_b);
		double lookup(double value_a, double value_b);
	private:
		//vector< vector<double> > * values;
		double values[100][100];
		int length_a;//The length of the a-dimension of the array
		int length_b;//The length of the b-dimension of the array
		double delta_a;//The spacing of the a indices
		double delta_b;//The spacing of the b indices
};
