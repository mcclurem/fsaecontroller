
#include "table.h"

struct point
{
		double x,y,z;
};


Table::Table(int size_x, int size_y, double delt_x, double delt_y)
{
		delta_x = delt_x;
		delta_y = delt_y;
		length_x = size_x;
		length_y = size_y;
		
//Initialize the values array
		values = (double **)calloc(sizeof(double *), length_x);
		values[0] = (double *)calloc(sizeof(double), length_x * length_y);
		for(int i = 1; i < length_x; i++)
			values[i] = values[0] + i * length_y;


}

void Table::edit(int x, int y, double value)
{
		values[x][y] = value;
}

double Table::lookup(double value_x, double value_y)
{
		//*********NOTE**********
		//We have to be certain that these catch statements are all
		//inclusive - a seg-fault on this code will be FATAL!!!

		if(value_x < 0.)
				value_x = 0.;
		if(value_x > (length_x - 1) * delta_x)
				value_x = (length_x - 1) * delta_x;
		if(value_y < 0.)
				value_y = 0.;
		if(value_y > (length_y - 1) * delta_y)
				value_y = (length_y - 1) * delta_y;

		
		//Casting to an int will truncate and get our next closest value
		//below our actual (then subtract 1 because array starts at zero)
		int index_x1 = (int)(value_x / delta_x);
		//Catch small values
		//if(index_x1 < 0)
		//	index_x1 = 0;
		
		int index_x2 = index_x1 + 1; //The next highest value
		//Catch large values
		if(index_x2 > length_x - 1)
			index_x2 = length_x - 1;


		//Casting to an int will truncate and get our next closest value
		//below our actual (then subtract 1 because array starts at zero)
		int index_y1 = (int)(value_y / delta_y);
		//Catch small values
		//if(index_y1 < 0)
		//	index_y1 = 0;
		int index_y2 = index_y1 + 1; //The next highest value
		//Catch large values
		if(index_y2 > length_y - 1)
			index_y2 = length_y - 1;


		printf("\n\n\nIndex a1 = %d, Index b1 = %d\n", index_x1, index_y1);

		double value_x0y0 = values[index_x1][index_y1];
		double value_x0y1 = values[index_x1][index_y2];
		double value_x1y0 = values[index_x2][index_y1];
		double value_x1y1 = values[index_x2][index_y2];

		double midval = (value_x0y0 + value_x0y1 + value_x1y0 + value_x1y1)/4.;

		//We need to solve the plane eq
		//Ax+By+Cz=D
		//We are working in a left handed coord system
		
		double x = 1.- (((double)(index_x2)*delta_x - value_x)/delta_x);
		double y = 1.- (((double)(index_y2)*delta_y - value_y)/delta_y);

		printf("X=%lf, Y=%lf\n",x,y);
		point interpoint, point1, point2, point3;
		if( y <= x )
		{//upper right
			printf("UR\n");
			point1.x = 1.;
			point1.y = 0.;
			point1.z = value_x1y0;
		}else
		{//lower left
			printf("LL\n");
			point1.x = 0.;
			point1.y = 1.;
			point1.z = value_x0y1;
		}
		
		if( y <= (-x + 1) )
		{//upper left
			printf("UL\n");
			point2.x = 0.;
			point2.y = 0.;
			point2.z = value_x0y0;
		}else
		{//lower right
			printf("LR\n");
			point2.x = 1.;
			point2.y = 1.;
			point2.z = value_x1y1;
		}

		point3.x = 0.5;
		point3.y = 0.5;
		point3.z = midval;
		
		interpoint.x = x;
		interpoint.y = y;


		printf("point 1=%lf, point2=%lf 2midval = %lf\n", point1.z, point2.z, midval);
		//Calculate determinants
		double A,B,C,D;
		A = (point1.y * (point2.z - point3.z)) + (point2.y * (point3.z - point1.z)) + (point3.y * (point1.z - point2.z));
		B = (point1.z * (point2.x - point3.x)) + (point2.z * (point3.x - point1.x)) + (point3.z * (point1.x - point2.x));
		C = (point1.x * (point2.y - point3.y)) + (point2.x * (point3.y - point1.y)) + (point3.x * (point1.y - point2.y));
		D = (point1.x * (point2.y * point3.z - point3.y * point2.z))
			+ (point2.x * (point3.y * point1.z - point1.y * point3.z))
			+ (point3.x * (point1.y * point2.z - point2.y * point1.z));

		printf("A=%lf, B=%lf, C=%lf, D=%lf\n",A,B,C,D);
		interpoint.z = ( D - ( interpoint.x * A )-( interpoint.y * B))/C;












		printf("%lf\t%lf\n%lf\t%lf\n", value_x0y0, value_x1y0, value_x0y1, value_x1y1);
		
		return interpoint.z;
}
