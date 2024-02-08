#include "Trapezoid.h"
#include <iostream>

Trapezoid::Trapezoid()
{
	
}


Trapezoid::~Trapezoid()
{
}

void Trapezoid::Init(const point_t &qs_i1_yval, const point_t& qs_i3_yval, const int segnum,
	int &t1, int &t2, int &t3, int &t4, int i6, int i7, int i4, int i2)
{
	t1 = newtrap();		// middle left 
	t2 = newtrap();		// middle right 
	t3 = newtrap();		// bottom-most 
	t4 = newtrap();		// top-most 

	tr.at(t1).hi = tr.at(t2).hi = tr.at(t4).lo = qs_i1_yval;
	tr.at(t1).lo = tr.at(t2).lo = tr.at(t3).hi = qs_i3_yval;
	tr.at(t4).hi.y = (double)(INFINITY);
	tr.at(t4).hi.x = (double)(INFINITY);
	tr.at(t3).lo.y = (double)-1 * (INFINITY);
	tr.at(t3).lo.x = (double)-1 * (INFINITY);
	tr.at(t1).rseg = tr.at(t2).lseg = segnum;
	tr.at(t1).u0 = tr.at(t2).u0 = t4;
	tr.at(t1).d0 = tr.at(t2).d0 = t3;
	tr.at(t4).d0 = tr.at(t3).u0 = t1;
	tr.at(t4).d1 = tr.at(t3).u1 = t2;

	tr.at(t1).sink = i6;
	tr.at(t2).sink = i7;
	tr.at(t3).sink = i4;
	tr.at(t4).sink = i2;

	tr.at(t1).state = tr.at(t2).state = STATE::ST_VALID;
	tr.at(t3).state = tr.at(t4).state = STATE::ST_VALID;
}

bool Trapezoid::add_segment(Polygon *polygon, const segment_t *s, int &t, int tn, int tlast, bool tribot, bool is_swapped)
{
	// error 
	if ((tr.at(t).d0 <= 0) && (tr.at(t).d1 <= 0)) // case cannot arise 
	{
		std::cout << "Error: Trapezoid::add_segment: case cannot arise \n";
		return false;
	}
	// only one trapezoid below. partition t into two and make the 
	// two resulting trapezoids t and tn as the upper neighbours of 
	// the sole lower trapezoid 
	if ((tr.at(t).d0 > 0) && (tr.at(t).d1 <= 0))
	{
		// Only one trapezoid below 
		if ((tr.at(t).u0 > 0) && (tr.at(t).u1 > 0))
		{
			// continuation of a chain from abv.
			if (tr.at(t).usave > 0) // three upper neighbours
			{
				if (tr.at(t).uside == SIDE::S_LEFT)
				{
					tr.at(tn).u0 = tr.at(t).u1;
					tr.at(t).u1 = -1;
					tr.at(tn).u1 = tr.at(t).usave;

					tr.at(tr.at(t).u0).d0 = t;
					tr.at(tr.at(tn).u0).d0 = tn;
					tr.at(tr.at(tn).u1).d0 = tn;
				}
				else // intersects in the right
				{
					tr.at(tn).u1 = -1;
					tr.at(tn).u0 = tr.at(t).u1;
					tr.at(t).u1 = tr.at(t).u0;
					tr.at(t).u0 = tr.at(t).usave;

					tr.at(tr.at(t).u0).d0 = t;
					tr.at(tr.at(t).u1).d0 = t;
					tr.at(tr.at(tn).u0).d0 = tn;
				}

				tr.at(t).usave = tr.at(tn).usave = 0;
			}
			else		// No usave.... simple case 
			{
				tr.at(tn).u0 = tr.at(t).u1;
				tr.at(t).u1 = tr.at(tn).u1 = -1;
				tr.at(tr.at(tn).u0).d0 = tn;
			}
		}
		else // fresh seg. or upward cusp
		{
			int tmp_u = tr.at(t).u0;
			int td0, td1;
			if (((td0 = tr.at(tmp_u).d0) > 0) &&
				((td1 = tr.at(tmp_u).d1) > 0))
			{
				// upward cusp 
				if ((tr.at(td0).rseg > 0) &&
					!segment_t::is_left_of(polygon->get_segment(tr.at(td0).rseg), &s->v1))
				{
					tr.at(t).u0 = tr.at(t).u1 = tr.at(tn).u1 = -1;
					tr.at(tr.at(tn).u0).d1 = tn;
				}
				else // cusp going leftwards 
				{
					tr.at(tn).u0 = tr.at(tn).u1 = tr.at(t).u1 = -1;
					tr.at(tr.at(t).u0).d0 = t;
				}
			}
			else // fresh segment 
			{
				tr.at(tr.at(t).u0).d0 = t;
				tr.at(tr.at(t).u0).d1 = tn;
			}
		}
		if (point_t::_equal_to(&tr.at(t).lo, &tr.at(tlast).lo) && tribot)
		{		// bottom forms a triangle 
			int tmptriseg = s->next;
			if (is_swapped)
				tmptriseg = s->prev; // seg.at(segnum).prev;

			if ((tmptriseg > 0) && segment_t::is_left_of(polygon->get_segment(tmptriseg), &s->v0))
			{
				// L-R downward cusp 
				tr.at(tr.at(t).d0).u0 = t;
				tr.at(tn).d0 = tr.at(tn).d1 = -1;
			}
			else
			{
				// R-L downward cusp 
				tr.at(tr.at(tn).d0).u1 = tn;
				tr.at(t).d0 = tr.at(t).d1 = -1;
			}
		}
		else
		{
			if ((tr.at(tr.at(t).d0).u0 > 0) && (tr.at(tr.at(t).d0).u1 > 0))
			{
				if (tr.at(tr.at(t).d0).u0 == t) // passes thru LHS 
				{
					tr.at(tr.at(t).d0).usave = tr.at(tr.at(t).d0).u1;
					tr.at(tr.at(t).d0).uside = SIDE::S_LEFT;
				}
				else
				{
					tr.at(tr.at(t).d0).usave = tr.at(tr.at(t).d0).u0;
					tr.at(tr.at(t).d0).uside = SIDE::S_RIGHT;
				}
			}
			tr.at(tr.at(t).d0).u0 = t;
			tr.at(tr.at(t).d0).u1 = tn;
		}
		t = tr.at(t).d0;
	}
	else if ((tr.at(t).d0 <= 0) && (tr.at(t).d1 > 0))
	{
		// Only one trapezoid below 
		if ((tr.at(t).u0 > 0) && (tr.at(t).u1 > 0))
		{
			// continuation of a chain from abv. 
			if (tr.at(t).usave > 0) // three upper neighbours 
			{
				if (tr.at(t).uside == SIDE::S_LEFT)
				{
					tr.at(tn).u0 = tr.at(t).u1;
					tr.at(t).u1 = -1;
					tr.at(tn).u1 = tr.at(t).usave;

					tr.at(tr.at(t).u0).d0 = t;
					tr.at(tr.at(tn).u0).d0 = tn;
					tr.at(tr.at(tn).u1).d0 = tn;
				}
				else		// intersects in the right 
				{
					tr.at(tn).u1 = -1;
					tr.at(tn).u0 = tr.at(t).u1;
					tr.at(t).u1 = tr.at(t).u0;
					tr.at(t).u0 = tr.at(t).usave;

					tr.at(tr.at(t).u0).d0 = t;
					tr.at(tr.at(t).u1).d0 = t;
					tr.at(tr.at(tn).u0).d0 = tn;
				}

				tr.at(t).usave = tr.at(tn).usave = 0;
			}
			else		// No usave.... simple case 
			{
				tr.at(tn).u0 = tr.at(t).u1;
				tr.at(t).u1 = tr.at(tn).u1 = -1;
				tr.at(tr.at(tn).u0).d0 = tn;
			}
		}
		else // fresh seg. or upward cusp 
		{
			int tmp_u = tr.at(t).u0;
			int td0, td1;
			if (((td0 = tr.at(tmp_u).d0) > 0) &&
				((td1 = tr.at(tmp_u).d1) > 0))
			{
				// upward cusp 
				if ((tr.at(td0).rseg > 0) && !segment_t::is_left_of(polygon->get_segment(tr.at(td0).rseg), &s->v1))
				{
					tr.at(t).u0 = tr.at(t).u1 = tr.at(tn).u1 = -1;
					tr.at(tr.at(tn).u0).d1 = tn;
				}
				else
				{
					tr.at(tn).u0 = tr.at(tn).u1 = tr.at(t).u1 = -1;
					tr.at(tr.at(t).u0).d0 = t;
				}
			}
			else		// fresh segment 
			{
				tr.at(tr.at(t).u0).d0 = t;
				tr.at(tr.at(t).u0).d1 = tn;
			}
		}
		if (point_t::_equal_to(&tr.at(t).lo, &tr.at(tlast).lo) && tribot)
		{
			// bottom forms a triangle 
			int tmpseg = 0; // ********************************************************************************
			int tmptriseg = s->next; // seg[segnum).next;
			if (is_swapped)
				tmptriseg = s->prev; // seg.at(segnum).prev;

			if ((tmpseg > 0) && segment_t::is_left_of(polygon->get_segment(tmpseg), &s->v0))
			{
				// L-R downward cusp 
				tr.at(tr.at(t).d1).u0 = t;
				tr.at(tn).d0 = tr.at(tn).d1 = -1;
			}
			else
			{
				// R-L downward cusp 
				tr.at(tr.at(tn).d1).u1 = tn;
				tr.at(t).d0 = tr.at(t).d1 = -1;
			}
		}
		else
		{
			if ((tr.at(tr.at(t).d1).u0 > 0) && (tr.at(tr.at(t).d1).u1 > 0))
			{
				if (tr.at(tr.at(t).d1).u0 == t) // passes thru LHS 
				{
					tr.at(tr.at(t).d1).usave = tr.at(tr.at(t).d1).u1;
					tr.at(tr.at(t).d1).uside = SIDE::S_LEFT;
				}
				else
				{
					tr.at(tr.at(t).d1).usave = tr.at(tr.at(t).d1).u0;
					tr.at(tr.at(t).d1).uside = SIDE::S_RIGHT;
				}
			}
			tr.at(tr.at(t).d1).u0 = t;
			tr.at(tr.at(t).d1).u1 = tn;
		}
		t = tr.at(t).d1;
	}
	// two trapezoids below. Find out which one is intersected by 
	// this segment and proceed down that one 
	else
	{
		int tmpseg = tr.at(tr.at(t).d0).rseg;
		double y0, yt;
		point_t tmppt;
		int tnext;
		bool i_d0 = false, i_d1 = false;

		if (point_t::_equal_y(&tr.at(t).lo, &s->v0))
		{
			if (tr.at(t).lo.x > s->v0.x)
				i_d0 = true;
			else
				i_d1 = true;
		}
		else
		{
			tmppt.y = y0 = tr.at(t).lo.y;
			yt = (y0 - s->v0.y) / (s->v1.y - s->v0.y);
			tmppt.x = s->v0.x + yt * (s->v1.x - s->v0.x);

			if (point_t::_less_than(&tmppt, &tr.at(t).lo))
				i_d0 = true;
			else
				i_d1 = true;
		}
		// check continuity from the top so that the lower-neighbour 
		// values are properly filled for the upper trapezoid 
		if ((tr.at(t).u0 > 0) && (tr.at(t).u1 > 0))
		{
			// continuation of a chain from abv.
			if (tr.at(t).usave > 0) // three upper neighbours 
			{
				if (tr.at(t).uside == SIDE::S_LEFT)
				{
					tr.at(tn).u0 = tr.at(t).u1;
					tr.at(t).u1 = -1;
					tr.at(tn).u1 = tr.at(t).usave;

					tr.at(tr.at(t).u0).d0 = t;
					tr.at(tr.at(tn).u0).d0 = tn;
					tr.at(tr.at(tn).u1).d0 = tn;
				}
				else		// intersects in the right 
				{
					tr.at(tn).u1 = -1;
					tr.at(tn).u0 = tr.at(t).u1;
					tr.at(t).u1 = tr.at(t).u0;
					tr.at(t).u0 = tr.at(t).usave;

					tr.at(tr.at(t).u0).d0 = t;
					tr.at(tr.at(t).u1).d0 = t;
					tr.at(tr.at(tn).u0).d0 = tn;
				}

				tr.at(t).usave = tr.at(tn).usave = 0;
			}
			else		// No usave.... simple case
			{
				tr.at(tn).u0 = tr.at(t).u1;
				tr.at(tn).u1 = -1;
				tr.at(t).u1 = -1;
				tr.at(tr.at(tn).u0).d0 = tn;
			}
		}
		else // fresh seg. or upward cusp 
		{
			int tmp_u = tr.at(t).u0;
			int td0, td1;
			if (((td0 = tr.at(tmp_u).d0) > 0) &&
				((td1 = tr.at(tmp_u).d1) > 0))
			{
				// upward cusp 
				if ((tr.at(td0).rseg > 0) &&
					!segment_t::is_left_of(polygon->get_segment(tr.at(td0).rseg), &s->v1))
				{
					tr.at(t).u0 = tr.at(t).u1 = tr.at(tn).u1 = -1;
					tr.at(tr.at(tn).u0).d1 = tn;
				}
				else
				{
					tr.at(tn).u0 = tr.at(tn).u1 = tr.at(t).u1 = -1;
					tr.at(tr.at(t).u0).d0 = t;
				}
			}
			else		// fresh segment 
			{
				tr.at(tr.at(t).u0).d0 = t;
				tr.at(tr.at(t).u0).d1 = tn;
			}
		}

		if (point_t::_equal_to(&tr.at(t).lo, &tr.at(tlast).lo) && tribot)
		{
			// this case arises only at the lowest trapezoid.. i.e.
			// tlast, if the lower endpoint of the segment is
			// already inserted in the structure =
			tr.at(tr.at(t).d0).u0 = t;
			tr.at(tr.at(t).d0).u1 = -1;
			tr.at(tr.at(t).d1).u0 = tn;
			tr.at(tr.at(t).d1).u1 = -1;

			tr.at(tn).d0 = tr.at(t).d1;
			tr.at(t).d1 = tr.at(tn).d1 = -1;

			tnext = tr.at(t).d1;
		}
		else if (i_d0) // intersecting d0
		{
			tr.at(tr.at(t).d0).u0 = t;
			tr.at(tr.at(t).d0).u1 = tn;
			tr.at(tr.at(t).d1).u0 = tn;
			tr.at(tr.at(t).d1).u1 = -1;

			// new code to determine the bottom neighbours of the
			// newly partitioned trapezoid 

			tr.at(t).d1 = -1;

			tnext = tr.at(t).d0;
		}
		else	// intersecting d1 
		{
			tr.at(tr.at(t).d0).u0 = t;
			tr.at(tr.at(t).d0).u1 = -1;
			tr.at(tr.at(t).d1).u0 = t;
			tr.at(tr.at(t).d1).u1 = tn;

			// new code to determine the bottom neighbours of the 
			// newly partitioned trapezoid 

			tr.at(tn).d0 = tr.at(t).d1;
			tr.at(tn).d1 = -1;

			tnext = tr.at(t).d1;
		}

		t = tnext;
	}

	return true;
}

// Thread in the segment into the existing trapezoidation. The
// limiting trapezoids are given by tfirst and tlast (which are the
// trapezoids containing the two endpoints of the segment. Merges all
// possible trapezoids which flank this segment and have been recently
// divided because of its insertion
bool Trapezoid::merge_trapezoids(int segnum, int &t, SIDE side, int &tr_t_sink, int &tr_tnext_sink)
{
	int tnext = -1;
	tr_t_sink = -1;
	tr_tnext_sink = -1;
	bool cond = ((((tnext = tr.at(t).d0) > 0) && (tr.at(tnext).lseg == segnum)) ||
				 (((tnext = tr.at(t).d1) > 0) && (tr.at(tnext).lseg == segnum)));
	if (side == SIDE::S_LEFT)
		cond = ((((tnext = tr.at(t).d0) > 0) && (tr.at(tnext).rseg == segnum)) ||
				(((tnext = tr.at(t).d1) > 0) && (tr.at(tnext).rseg == segnum)));

	if (cond)
	{
		if ((tr.at(t).lseg == tr.at(tnext).lseg) &&
			(tr.at(t).rseg == tr.at(tnext).rseg)) // good neighbours, merge them 
		{
			tr_tnext_sink = tr.at(tnext).sink;
			tr_t_sink = tr.at(t).sink;

			// Change the upper neighbours of the lower trapezoids 
			if ((tr.at(t).d0 = tr.at(tnext).d0) > 0)
				if (tr.at(tr.at(t).d0).u0 == tnext)
					tr.at(tr.at(t).d0).u0 = t;
				else if (tr.at(tr.at(t).d0).u1 == tnext)
					tr.at(tr.at(t).d0).u1 = t;

			if ((tr.at(t).d1 = tr.at(tnext).d1) > 0)
				if (tr.at(tr.at(t).d1).u0 == tnext)
					tr.at(tr.at(t).d1).u0 = t;
				else if (tr.at(tr.at(t).d1).u1 == tnext)
					tr.at(tr.at(t).d1).u1 = t;

			tr.at(t).lo = tr.at(tnext).lo;
			tr.at(tnext).state = STATE::ST_INVALID; // invalidate the lower trapezium 
		}
		else // not good neighbours
			t = tnext;
	}
	else // do not satisfy the outer if 
		t = tnext;

	return cond;
}

void Trapezoid::copy(int from_index, int to_index)
{
	if (from_index >= 0 && from_index < tr.size())
		if (to_index >= 0 && to_index < tr.size())
			tr.at(to_index) = tr.at(from_index);
}

int Trapezoid::new_lower_trap(int tu, const segment_t* s)
{
	int tmp_d = -1;

	int tl = newtrap();		// tl is the new lower trapezoid 
	tr.at(tl).state = STATE::ST_VALID;
	tr.at(tl) = tr.at(tu);
	tr.at(tu).lo.y = tr.at(tl).hi.y = s->v1.y;
	tr.at(tu).lo.x = tr.at(tl).hi.x = s->v1.x;
	tr.at(tu).d0 = tl;
	tr.at(tu).d1 = 0;
	tr.at(tl).u0 = tu;
	tr.at(tl).u1 = 0;

	if (((tmp_d = tr.at(tl).d0) > 0) && (tr.at(tmp_d).u0 == tu))
		tr.at(tmp_d).u0 = tl;
	if (((tmp_d = tr.at(tl).d0) > 0) && (tr.at(tmp_d).u1 == tu))
		tr.at(tmp_d).u1 = tl;

	if (((tmp_d = tr.at(tl).d1) > 0) && (tr.at(tmp_d).u0 == tu))
		tr.at(tmp_d).u0 = tl;
	if (((tmp_d = tr.at(tl).d1) > 0) && (tr.at(tmp_d).u1 == tu))
		tr.at(tmp_d).u1 = tl;

	return tl;
}

void Trapezoid::set_sink(int index, int sink)
{
	if (index >= 0 && index < tr.size())
		tr.at(index).sink = sink;
}

int Trapezoid::get_sink(int index)
{
	if (index < 0 || index >= tr.size())
		return -1;
	return tr.at(index).sink;
}

const point_t* Trapezoid::get_min_y(int index) const
{
	if (index < 0 || index >= tr.size())
		return nullptr;
	return &tr.at(index).lo;
}

// Return a free trapezoid 
int Trapezoid::newtrap(STATE state)
{
	trap_t new_trap;
	new_trap.lseg = -1;
	new_trap.rseg = -1;
	new_trap.state = state; // STATE::ST_VALID;

	tr.push_back(new_trap);
	return tr.size() - 1;
}