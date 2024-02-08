#include "Segment.h"

segment_t::segment_t() 
{
	
}


segment_t::~segment_t()
{
}
// Retun TRUE if the vertex v is to the left of line segment no.
// segnum. Takes care of the degenerate cases when both the vertices
// have the same y--cood, etc.
bool segment_t::is_left_of(const segment_t* s, const point_t* v)
{
	auto CROSS = [](const point_t& v0, const point_t& v1, const point_t& v2) -> double
		{
			return ((v1.x - v0.x) * (v2.y - v0.y) - (v1.y - v0.y) * (v2.x - v0.x));
		};

	double area;

	if (point_t::_greater_than(&s->v1, &s->v0)) // seg. going upwards 
	{
		if (point_t::_equal_y(&s->v1, v))
		{
			if (v->x < s->v1.x)
				area = 1.0;
			else
				area = -1.0;
		}
		else if (point_t::_equal_y(&s->v0, v))
		{
			if (v->x < s->v0.x)
				area = 1.0;
			else
				area = -1.0;
		}
		else
			area = CROSS(s->v0, s->v1, (*v));
	}
	else	// v0 > v1 
	{
		if (point_t::_equal_y(&s->v1, v))
		{
			if (v->x < s->v1.x)
				area = 1.0;
			else
				area = -1.0;
		}
		else if (point_t::_equal_y(&s->v0, v))
		{
			if (v->x < s->v0.x)
				area = 1.0;
			else
				area = -1.0;
		}
		else
			area = CROSS(s->v1, s->v0, (*v));
	}

	if (area > 0.0)
		return true;
	else
		return false;
}
