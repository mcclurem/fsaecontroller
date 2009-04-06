
#include "stdlib.h"
#include <vector>
using namespace std;
class Table
{
	public:
		Table(int size_x, int size_y, double delt_x, double delt_y);
		void edit(int x, int y, double value);
		double lookup(double value_x, double value_y);
	private:
//		vector< vector<double> > values;
		double **values;
		int length_x;//The length of the a-dimension of the array
		int length_y;//The length of the b-dimension of the array
		double delta_x;//The spacing of the a indices
		double delta_y;//The spacing of the b indices
};
