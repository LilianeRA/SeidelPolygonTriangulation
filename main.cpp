#include <iostream>
#include "Polygon.h"


/*
#define SEGSIZE 200
// Segment attributes 

using point_t =  struct {
	double x, y, z;
};
using vector_t =  struct {
	double x, y, z;
};

using segment_t = struct {
	point_t v0, v1;		// two endpoints 
	int is_inserted;		// inserted in trapezoidation yet ? 
	int root0, root1;		// root nodes in Q 
	int next;			// Next logical segment 
	int prev;			// Previous segment 
};

segment_t seg[SEGSIZE];		// Segment table 
static int permute[SEGSIZE];
// Generate a random permutation of the segments 1..n 
int generate_random_ordering(int n)
{
	//struct timeval tval;
	//struct timezone tzone;
	register int i;
	int m, st[SEGSIZE], * p;

	static int choose_idx = 1;
	//gettimeofday(&tval, &tzone);
	//srand48(tval.tv_sec);
	
	//std::srand(std::time(0));
	std::srand(1);

	for (i = 0; i <= n; i++)
		st[i] = i;

	p = st;
	for (i = 1; i <= n; i++, p++)
	{
		m = std::rand() % (n + 1 - i) + 1;
		permute[i] = p[m];
		if (m != 1)
			p[m] = p[1];
		//std::cout << seg[permute[i]].v0.x << " " << seg[permute[i]].v0.y << " " << seg[permute[i]].v0.z << " - ";
		//std::cout << seg[permute[i]].v1.x << " " << seg[permute[i]].v1.y << " " << seg[permute[i]].v1.z << "\n";
	}
	return 0;
}

static int initialise(int n)
{
	register int i;

	for (i = 1; i <= n; i++)
		seg[i].is_inserted = false;

	generate_random_ordering(n);

	return 0;
}

// Read in the list of vertices from infile 
int read_segments(const char *filename, int &genus)
{
	std::ifstream infile(filename);
	int ccount;
	register int i;
	int ncontours, npoints, first, last;

	if (!infile.is_open())
	{
		std::cout << "Error when reading: " << filename << std::endl;
		return -1;
	}

	infile >> ncontours;
	//std::cout << "ncontours " << ncontours << std::endl;
	if (ncontours <= 0)
		return -1;

	// For every contour, read in all the points for the contour. The 
	// outer-most contour is read in first (points specified in 
	// anti-clockwise order). Next, the inner contours are input in 
	// clockwise order 

	ccount = 0;
	i = 1;

	while (ccount < ncontours)
	{
		int j;

		infile >> npoints;
		first = i;
		last = first + npoints - 1;
		for (j = 0; j < npoints; j++, i++)
		{
			infile >> seg[i].v0.x >> seg[i].v0.y >> seg[i].v0.z;
			if (i == last)
			{
				//std::cout << "last " << last << " first " << first << std::endl;
				seg[i].next = first;
				seg[i].prev = i - 1;
				seg[i - 1].v1 = seg[i].v0;
			}
			else if (i == first)
			{
				//std::cout << "first " << first << " last " << last << std::endl;
				seg[i].next = i + 1;
				seg[i].prev = last;
				seg[last].v1 = seg[i].v0;
			}
			else
			{
				seg[i].prev = i - 1;
				seg[i].next = i + 1;
				seg[i - 1].v1 = seg[i].v0;
			}
			//std::cout << seg[i].v0.x << " " << seg[i].v0.y << " " << seg[i].v0.z << " - ";
			//std::cout << seg[i].v1.x << " " << seg[i].v1.y << " " << seg[i].v1.z << "\n";

			seg[i].is_inserted = false;
		}

		ccount++;
	}

	genus = ncontours - 1;
	return i - 1;
}


void print_segments(int n)
{
	for (int i = 1; i <= n; i++)
	{
		int first = i;
		int last = seg[i].prev;
		int npoints = last - first + 1;
		if (npoints != 0) std::cout << "\n";
		std::cout << first << " " << last << " " << npoints << "\t\t";
		std::cout << seg[i].v0.x << " " << seg[i].v0.y << " " << seg[i].v0.z << " - ";
		std::cout << seg[i].v1.x << " " << seg[i].v1.y << " " << seg[i].v1.z << "\n";
	}
}*/

int main(int argc, char *argv[])
{
	std::cout << "Hello Worlds\n";
	Polygon polygon;
	if (!polygon.ReadFile("../../data_1"))
	{
		std::cout << "Polygon not read\n";
		return -1;
	}
	std::cout << "Polygon read\n";
	polygon.Init();
	/*int n, genus;

	if ((argc < 2) || ((n = read_segments(argv[1], &genus)) < 0))
	{
		std::cerr << "usage: triangulate <filename>\n";
		exit(1);
	}

	std::cout << "genus " << genus << " n " << n << std::endl;
	print_segments(n);

	std::cout << "-----------------------------------------------\n";
	*/


	return 0;
}
