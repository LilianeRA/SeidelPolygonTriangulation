#include <cmath>
#include "Point.h"

const double C_EPS = 1.0e-7;

point_t::point_t() 
{
	
}


point_t::~point_t()
{
}


// Return the maximum of the two points into the yval structure 
void point_t::_max(point_t* yval, const point_t* v0, const point_t* v1)
{
	if (v0->y > v1->y + C_EPS)
	{
		*yval = *v0;
	}
	else if (std::abs(v0->y - v1->y) <= C_EPS)
	{
		if (v0->x > v1->x + C_EPS)
			*yval = *v0;
		else
			*yval = *v1;
	}
	else
	{
		*yval = *v1;
	}
}

void point_t::_min(point_t* yval, const point_t* v0, const point_t* v1)
{
	if (v0->y < v1->y - C_EPS)
	{
		*yval = *v0;
	}
	else if (std::abs(v0->y - v1->y) <= C_EPS)
	{
		if (v0->x < v1->x)
			*yval = *v0;
		else
			*yval = *v1;
	}
	else
		*yval = *v1;
}

bool point_t::_greater_than(const point_t* v0, const point_t* v1)
{
	if (v0->y > v1->y + C_EPS)
		return true;
	else if (v0->y < v1->y - C_EPS)
		return false;
	else
		return (v0->x > v1->x);
}

bool point_t::_equal_to(const point_t* v0, const point_t* v1)
{
	return (std::abs(v0->y - v1->y) <= C_EPS && std::abs(v0->x - v1->x) <= C_EPS);
}

bool point_t::_equal_y(const point_t* v0, const point_t* v1)
{
	return std::abs(v0->y - v1->y) <= C_EPS;
}

bool point_t::_greater_than_equal_to(const point_t* v0, const point_t* v1)
{
	if (v0->y > v1->y + C_EPS)
		return true;
	else if (v0->y < v1->y - C_EPS)
		return false;
	else
		return (v0->x >= v1->x);
}

bool point_t::_less_than(const point_t* v0, const point_t* v1)
{
	if (v0->y < v1->y - C_EPS)
		return true;
	else if (v0->y > v1->y + C_EPS)
		return false;
	else
		return (v0->x < v1->x);
}







