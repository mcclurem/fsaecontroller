
#include "table.h"

Table::Table(int size_a, int size_b, double delt_a, double delt_b)
{
		//values = new vector< vector<double> >;
		//values = (double*)malloc(sizeof(double[100])*100);
		values[0][0] = 0.;
}

double Table::lookup(double value_a, double value_b)
{
		//*********NOTE**********
		//We have to be certain that these catch statements are all
		//inclusive - a seg-fault on this code will be FATAL!!!

		//Casting to an int will truncate and get our next closest value
		//below our actual (then subtract 1 because array starts at zero)
		int index_a1 = (int)(value_a / delta_a) - 1;
		//Catch small values
		if(index_a1 < 0)
			index_a1 = 0;
		
		int index_a2 = index_a1 + 1; //The next highest value
		//Catch large values
		if(index_a2 > length_a - 1)
			index_a2 = length_a - 1;

		//Casting to an int will truncate and get our next closest value
		//below our actual (then subtract 1 because array starts at zero)
		int index_b1 = (int)(value_b / delta_b) - 1;
		//Catch small values
		if(index_b1 < 0)
			index_b1 = 0;
		int index_b2 = index_b1 + 1; //The next highest value
		//Catch large values
		if(index_b2 > length_b - 1)
			index_b2 = length_b - 1;



		//We are now going to determine where the index would be fractionally
		double percent_a1 = (value_a - (double)(index_a1*delta_a))/delta_a;
		double percent_a2 = 1. - percent_a1;

		double percent_b1 = (value_b - (double)(index_b1*delta_b))/delta_b;
		double percent_b2 = 1. - percent_b1;

		double value_a1b1 = values[index_a1][index_b1];
		double value_a1b2 = values[index_a1][index_b2];
		double value_a2b1 = values[index_a2][index_b1];
		double value_a2b2 = values[index_a2][index_b2];

		//Now we do a weighted average of all the edges and divide by 4 and sum
		double val_top = value_a1b1 * percent_a1 + value_a2b1 * percent_a2;//Top segment
		double val_bottom = value_a1b2 * percent_a1 + value_a2b2 * percent_a2;//Bottom segment
		double val_left = value_a1b1 * percent_b1 + value_a1b2 * percent_b2;//Left Segment
		double val_right =value_a2b1 * percent_b1 + value_a2b2 * percent_b2;//Right Segment

		return ((val_top + val_bottom + val_left + val_right)/4.);
}
