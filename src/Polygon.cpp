#include "Polygon.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>

int Polygon::choose_idx = 1;

Polygon::Polygon() 
{
	
}


Polygon::~Polygon()
{
	segments.clear();
}

bool Polygon::ReadFile(const std::string& filepath)
{
	std::ifstream infile(filepath);
	int ccount;
	register int i;
	int ncontours, npoints, first, last;

	if (!infile.is_open())
	{
		std::cout << "Error when reading: " << filepath << std::endl;
		return false;
	}

	infile >> ncontours;
	//std::cout << "ncontours " << ncontours << std::endl;
	if (ncontours <= 0)
	{
		infile.close();
		return false;
	}

	// For every contour, read in all the points for the contour. The 
	// outer-most contour is read in first (points specified in 
	// anti-clockwise order). Next, the inner contours are input in 
	// clockwise order 

	ccount = 0;
	i = 1;

	while (ccount < ncontours)
	{
		infile >> npoints;
		first = i;
		last = first + npoints - 1;
		for (int j = 0; j < npoints; j++)
		{
			segment_t seg;
			segments.push_back(seg);
		}
		for (int j = 0; j < npoints; j++, i++)
		{
			segment_t& seg = segments.at(i-1); // i starts at 1
			infile >> seg.v0.x >> seg.v0.y >> seg.v0.z;
			if (i == last)
			{
				seg.next = first;
				seg.prev = i - 1;
				//seg[i - 1].v1 = seg[i].v0;
			}
			else if (i == first)
			{
				seg.next = i + 1;
				seg.prev = last;
				//seg[last].v1 = seg[i].v0;
			}
			else
			{
				seg.prev = i - 1;
				seg.next = i + 1;
				//seg[i - 1].v1 = seg[i].v0;
			}
			segments.at(seg.prev - 1).v1 = seg.v0;
			seg.is_inserted = false;
		}

		ccount++;
	}

	genus = ncontours - 1;

	infile.close();

	//Print();
	return (segments.size() > 0); // i - 1;
}

void Polygon::Print()
{
	for (int i = 1; i <= segments.size(); i++)
	{
		int first = i;
		int last = segments.at(i-1).prev;
		int npoints = last - first + 1;
		if (npoints != 0) std::cout << "\n";
		std::cout << first << " " << last << " " << npoints << "\t\t";
		std::cout << segments.at(i - 1).v0.x << " " << segments.at(i - 1).v0.y << " " << segments.at(i - 1).v0.z << " - ";
		std::cout << segments.at(i - 1).v1.x << " " << segments.at(i - 1).v1.y << " " << segments.at(i - 1).v1.z << "\n";
	}
}

void Polygon::Init()
{
	for(auto &seg : segments)
		seg.is_inserted = false;

	generate_random_ordering();

}

// Generate a random permutation of the segments 1..n 
void Polygon::generate_random_ordering()
{
	std::vector<int> permute;

	Polygon::choose_idx = 1;

	register int i; // It's a hint to the compiler that the variable will be heavily used and that you recommend it be kept in a processor register if possible.
	for (i = 1; i <= segments.size(); i++)
	{
		permute.push_back(i);
	}

	std::random_device rd;
	std::mt19937 g(rd()); 
	g.seed(1);

	std::shuffle(permute.begin(), permute.end(), g);

	//std::copy(permute.begin(), permute.end(), std::ostream_iterator<int>(std::cout, " "));
	//std::cout << '\n';
}