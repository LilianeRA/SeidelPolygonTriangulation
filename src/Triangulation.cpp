#include "Triangulation.h"
#include <cmath>

Triangulation::Triangulation()
{
	polygon = new Polygon();
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
	double v;

	for (int i = 0, v = n; i < h; i++)
		v = std::log2(v);

	return (int)std::ceil((double)1.0 * n / v);
}

void Triangulation::construct_trapezoids()
{
	this->query = new Query(polygon);
	int nseg = polygon->get_total_segments();

	// Add the first segment and get the query structure and trapezoid 
	// list initialised 

	int segnum = polygon->choose_permutation();
	int root = query->init_query_structure(segnum, polygon->get_total_segments());

	for (int i = 1; i <= nseg; i++)
	{
		polygon->set_root0(i, root);
		polygon->set_root1(i, root);
		//seg[i].root0 = seg[i].root1 = root;
	}

	for (int h = 1; h <= math_logstar_n(nseg); h++)
	{
		for (int i = math_N(nseg, h - 1) + 1; i <= math_N(nseg, h); i++)
		{
			segnum = polygon->choose_permutation();
			query->add_segment(segnum);
		}

		// Find a new root for each of the segment endpoints
		for (int i = 1; i <= nseg; i++)
			query->find_new_roots(i);
	}

	for (int i = math_N(nseg, math_logstar_n(nseg)) + 1; i <= nseg; i++)
	{
		segnum = polygon->choose_permutation();
		query->add_segment(segnum);
	}

}

void Triangulation::monotonate_trapezoids()
{
	monotonate = new Monotonate(polygon, query);
	total_monotone = monotonate->monotonate_trapezoids();
}

void Triangulation::triangulate_monotone_polygons()
{
	monotonate->triangulate_monotone_polygons(total_monotone);
}
