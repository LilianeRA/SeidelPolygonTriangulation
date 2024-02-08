#include "Triangulation.h"
#include <iostream>
#include <cmath>

Triangulation::Triangulation()
{
	polygon = new Polygon();
	query = new Query();
}


Triangulation::~Triangulation()
{
	
}

bool Triangulation::ReadFile(const std::string& filepath)
{
	return polygon->ReadFile(filepath);
}

void Triangulation::Init()
{
	polygon->Init();
	query->Init(polygon);
}

// Get log*n for given n 
int math_logstar_n(int n)
{
	double v;
	int i = 0;
	for (v = (double)n; v >= 1; i++)
	{
		v = std::log2(v);
		//printf("v %f\n", v);
	}
	//printf("\n");

	return (i - 1);
}

int math_N(int n, int h)
{
	double v = n;

	for (int i = 0; i < h; i++)
		v = std::log2(v);

	return (int)std::ceil((double)1.0 * n / v);
}

void Triangulation::construct_trapezoids()
{
	int nseg = polygon->get_total_segments();
	for (int h = 1; h <= math_logstar_n(nseg); h++)
	{
		//std::cout << "h " << h << " ";
		//std::cout << "math_N(nseg, h - 1) + 1 = " << math_N(nseg, h - 1) + 1 << ", ";
		//std::cout << "math_N(nseg, h) = " << math_N(nseg, h) << std::endl;
		for (int i = math_N(nseg, h - 1) + 1; i <= math_N(nseg, h); i++)
		{
			int segnum = polygon->choose_permutation();
			//std::cout << "i " << i << " segnum " << segnum << std::endl;
			query->add_segment(segnum);
		}

		//std::cout << "find_new_roots\n";
		// Find a new root for each of the segment endpoints
		for (int i = 0; i < nseg; i++)
			query->find_new_roots(i);
	}

	//std::cout << "math_N(nseg, math_logstar_n(nseg)) + 1 = " << math_N(nseg, math_logstar_n(nseg)) + 1 << std::endl;
	//std::cout << "nseg " << nseg << std::endl;
	for (int i = math_N(nseg, math_logstar_n(nseg)) + 1; i <= nseg; i++)
	{
		int segnum = polygon->choose_permutation();
		//std::cout << "i " << i << " segnum " << segnum << std::endl;
		query->add_segment(segnum);
	}
}

