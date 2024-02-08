#include <iostream>
#include <algorithm>
#include "Query.h"


const int FIRSTPT = 1;		// checking whether pt. is inserted 
const int LASTPT = 2;

Query::Query()
{
	trapezoid_struct = new Trapezoid();
}


Query::~Query()
{
}

void Query::Init(Polygon* polygon)
{
	this->polygon = polygon;
	qs.clear();
	// insert a random segment of the polygon
	int segnum = polygon->choose_permutation();
	const segment_t* s = polygon->get_segment(segnum);

	//			i1
	//		   /  \
	//	   Y i3    i2 S
	//		/ \
	//  S i4   i5 X
	//        /  \
	//	  S i6    i7 S

	// root
	const int i1 = newnode();
	qs.at(i1).nodetype = NODE_TYPE::T_Y;
	point_t::_max(&qs.at(i1).yval, &s->v0, &s->v1);
	// right node S
	const int i2 =  newnode();
	qs.at(i1).right = i2;
	qs.at(i2).nodetype = NODE_TYPE::T_SINK;
	qs.at(i2).parent = i1;
	// left node Y
	const int i3 = newnode();
	qs.at(i1).left = i3;
	qs.at(i3).nodetype = NODE_TYPE::T_Y;
	point_t::_min(&qs.at(i3).yval, &s->v0, &s->v1); 
	qs.at(i3).parent = i1;
	// left node S
	const int i4 = newnode();
	qs.at(i3).left = i4;
	qs.at(i4).nodetype = NODE_TYPE::T_SINK;
	qs.at(i4).parent = i3;
	// right node X
	const int i5 = newnode();
	qs.at(i3).right = i5;
	qs.at(i5).nodetype = NODE_TYPE::T_X;
	qs.at(i5).segnum = segnum;
	qs.at(i5).parent = i3;
	// left node S
	const int i6 = newnode();
	qs.at(i5).left = i6;
	qs.at(i6).nodetype = NODE_TYPE::T_SINK;
	qs.at(i6).parent = i5;
	// right node S
	const int i7 = newnode();
	qs.at(i5).right = i7;
	qs.at(i7).nodetype = NODE_TYPE::T_SINK;
	qs.at(i7).parent = i5;

	// -----------------------------------------------------
	int t1, t2, t3, t4;
	trapezoid_struct->Init(qs.at(i1).yval, qs.at(i3).yval, segnum, t1, t2, t3, t4, i6, i7, i4, i2);

	qs.at(i2).trnum = t4;
	qs.at(i4).trnum = t3;
	qs.at(i6).trnum = t1;
	qs.at(i7).trnum = t2;

	polygon->set_segment_inserted(segnum, true);

	int nseg = polygon->get_total_segments();
	for (int i = 0; i < nseg; i++)
	{
		polygon->set_root0(i, i1);
		polygon->set_root1(i, i1);
	}
}


int Query::newnode()
{
	node_t n;
	qs.push_back(n);

	return qs.size() - 1;
}

// Returns true if the corresponding endpoint of the given segment is 
// already inserted into the segment tree. Use the simple test of 
// whether the segment which shares this endpoint is already inserted 
bool Query::inserted(int segnum, int whichpt)
{
	if (whichpt == FIRSTPT) 
		return polygon->get_segment(polygon->get_segment(segnum)->prev)->is_inserted;
	else 
		return polygon->get_segment(polygon->get_segment(segnum)->next)->is_inserted;
}

int Query::locate_endpoint(const point_t* v, const point_t* vo, int r)
{
	node_t* rptr = &qs.at(r);

	switch (rptr->nodetype)
	{
	case NODE_TYPE::T_SINK:
		return rptr->trnum;

	case NODE_TYPE::T_Y:
		if (point_t::_greater_than(v, &rptr->yval)) // above 
			return locate_endpoint(v, vo, rptr->right);
		else if (point_t::_equal_to(v, &rptr->yval)) // the point is already inserted.
		{
			if (point_t::_greater_than(vo, &rptr->yval)) // above 
				return locate_endpoint(v, vo, rptr->right);
			else
				return locate_endpoint(v, vo, rptr->left); // below 
		}
		else
			return locate_endpoint(v, vo, rptr->left); // below 

	case NODE_TYPE::T_X:
		//if (_equal_to(v, &seg[rptr->segnum].v0) ||
		//	_equal_to(v, &seg[rptr->segnum].v1))
		if (point_t::_equal_to(v, &polygon->get_segment(rptr->segnum)->v0) ||
			point_t::_equal_to(v, &polygon->get_segment(rptr->segnum)->v1))
		{
			if (point_t::_equal_y(v, vo)) // horizontal segment 
			{
				if (vo->x < v->x)
					return locate_endpoint(v, vo, rptr->left); // left 
				else
					return locate_endpoint(v, vo, rptr->right); // right 
			}

			else if (segment_t::is_left_of(polygon->get_segment(rptr->segnum), vo))
				return locate_endpoint(v, vo, rptr->left); // left 
			else
				return locate_endpoint(v, vo, rptr->right); // right 
		}
		else if (segment_t::is_left_of(polygon->get_segment(rptr->segnum), v))
			return locate_endpoint(v, vo, rptr->left); // left 
		else
			return locate_endpoint(v, vo, rptr->right); // right 

	default:
		std::cout << "Error: Haggu !!!!!\n";
		break;
	}
}

// Add in the new segment into the trapezoidation and update Q and T
// structures. First locate the two endpoints of the segment in the
// Q-structure. Then start from the topmost trapezoid and go down to
// the  lower trapezoid dividing all the trapezoids in between .
void Query::add_segment(int segnum)
{
	segment_t* s = polygon->get_segment(segnum);
	
	int tfirst = -1, tlast = -1;
	int tfirstr = -1, tlastr = -1;
	bool tritop = false, tribot = false;
	bool is_swapped = false;

	if (point_t::_greater_than(&s->v1, &s->v0)) // Get higher vertex in v0 
	{
		std::swap(s->v0, s->v1);
		std::swap(s->root0, s->root1);
		is_swapped = true;
	}

	if ((is_swapped) ? !inserted(segnum, LASTPT) : !inserted(segnum, FIRSTPT))     // insert v0 in the tree 
	{
		int tu = locate_endpoint(&s->v0, &s->v1, s->root0);
		int tl = trapezoid_struct->new_lower_trap(tu, s);

		// Now update the query structure and obtain the sinks for the two trapezoids	
		int i1 = newnode();		// Upper trapezoid sink 
		int i2 = newnode();		// Lower trapezoid sink 
		int sk = trapezoid_struct->get_sink(tu); // tr.at(tu).sink;

		qs.at(sk).nodetype = NODE_TYPE::T_Y;
		qs.at(sk).yval = s->v0;
		qs.at(sk).segnum = segnum;	// not really reqd ... maybe later 
		qs.at(sk).left = i2;
		qs.at(sk).right = i1;

		qs.at(i1).nodetype = NODE_TYPE::T_SINK;
		qs.at(i1).trnum = tu;
		qs.at(i1).parent = sk;

		qs.at(i2).nodetype = NODE_TYPE::T_SINK;
		qs.at(i2).trnum = tl;
		qs.at(i2).parent = sk;

		trapezoid_struct->set_sink(tu, i1);
		trapezoid_struct->set_sink(tl, i2);
		tfirst = tl;
	}
	else // v0 already present 
	{
		// Get the topmost intersecting trapezoid
		tfirst = locate_endpoint(&s->v0, &s->v1, s->root0);
		tritop = true;
	}

	if ((is_swapped) ? !inserted(segnum, FIRSTPT) : !inserted(segnum, LASTPT))     // insert v1 in the tree
	{
		int tu = locate_endpoint(&s->v1, &s->v0, s->root1);
		int tl = trapezoid_struct->new_lower_trap(tu, s);

		// Now update the query structure and obtain the sinks for the two trapezoids	
		int i1 = newnode();		// Upper trapezoid sink 
		int i2 = newnode();		// Lower trapezoid sink 
		int sk = trapezoid_struct->get_sink(tu); // tr.at(tu).sink;

		qs.at(sk).nodetype = NODE_TYPE::T_Y;
		qs.at(sk).yval = s->v1;
		qs.at(sk).segnum = segnum;	// not really reqd ... maybe later 
		qs.at(sk).left = i2;
		qs.at(sk).right = i1;

		qs.at(i1).nodetype = NODE_TYPE::T_SINK;
		qs.at(i1).trnum = tu;
		qs.at(i1).parent = sk;

		qs.at(i2).nodetype = NODE_TYPE::T_SINK;
		qs.at(i2).trnum = tl;
		qs.at(i2).parent = sk;

		trapezoid_struct->set_sink(tu, i1);
		trapezoid_struct->set_sink(tl, i2);
		tlast = tl;
	}
	else // v1 already present 
	{
		// Get the lowermost intersecting trapezoid 
		tlast = locate_endpoint(&s->v1, &s->v0, s->root1);
		tribot = true;
	}

	// Thread the segment into the query tree creating a new X-node 
	// First, split all the trapezoids which are intersected by s into two 

	int t = tfirst;	// topmost trapezoid 
	// traverse from top to bot 
	while ((t > 0) && point_t::_greater_than_equal_to(trapezoid_struct->get_min_y(t), trapezoid_struct->get_min_y(tlast))) 
	{
		int sk = trapezoid_struct->get_sink(t);
		int i1 = newnode();		// left trapezoid sink 
		int i2 = newnode();		// right trapezoid sink 

		qs.at(sk).nodetype = NODE_TYPE::T_X;
		qs.at(sk).segnum = segnum;
		qs.at(sk).left = i1;
		qs.at(sk).right = i2;

		qs.at(i1).nodetype = NODE_TYPE::T_SINK;	// left trapezoid (use existing one) 
		qs.at(i1).trnum = t;
		qs.at(i1).parent = sk;

		qs.at(i2).nodetype = NODE_TYPE::T_SINK;	// right trapezoid (allocate new) 
		qs.at(i2).parent = sk;
		qs.at(i2).trnum = trapezoid_struct->newtrap(Trapezoid::STATE::ST_VALID);
		int tn = qs.at(i2).trnum;

		if (t == tfirst)
			tfirstr = tn;
		if (point_t::_equal_to(trapezoid_struct->get_min_y(t), trapezoid_struct->get_min_y(tlast)))
			tlastr = tn;

		trapezoid_struct->copy(t, tn);
		trapezoid_struct->set_sink(t, i1);
		trapezoid_struct->set_sink(tn, i2);
		int t_sav = t;
		int tn_sav = tn;
		if (!trapezoid_struct->add_segment(polygon, s, t, tn, tlast, tribot, is_swapped))
			break;
	}// end-while 

	// Now combine those trapezoids which share common segments. We can
	// use the pointers to the parent to connect these together. This 
	// works only because all these new trapezoids have been formed 
	// due to splitting by the segment, and hence have only one parent 

	merge_trapezoids(segnum, tfirst, tlast, Trapezoid::SIDE::S_LEFT);
	merge_trapezoids(segnum, tfirstr, tlastr, Trapezoid::SIDE::S_RIGHT);

	polygon->get_segment(segnum)->is_inserted = true;
}

void Query::merge_trapezoids(int segnum, int tfirst, int tlast, Trapezoid::SIDE side)
{
	bool cond = false;
	int tr_t_sink = -1; 
	int tr_tnext_sink = -1;
	// First merge polys on the LHS 
	int t = tfirst;
	while ((t > 0) && point_t::_greater_than_equal_to(trapezoid_struct->get_min_y(t), trapezoid_struct->get_min_y(tlast)))
	{
		if (trapezoid_struct->merge_trapezoids(segnum, t, side, tr_t_sink, tr_tnext_sink))
		{
			// Use the upper node as the new node i.e. t 
			int ptnext = qs.at(tr_tnext_sink).parent;

			if (qs.at(ptnext).left == tr_tnext_sink)
				qs.at(ptnext).left = tr_t_sink;
			else
				qs.at(ptnext).right = tr_t_sink;	// redirect parent
		}
	
	}
}

// Update the roots stored for each of the endpoints of the segment.
// This is done to speed up the location-query for the endpoint when
// the segment is inserted into the trapezoidation subsequently
void Query::find_new_roots(int segnum)
{
	segment_t* s = polygon->get_segment(segnum);

	if (s->is_inserted)
		return;

	s->root0 = locate_endpoint(&s->v0, &s->v1, s->root0);
	s->root0 = trapezoid_struct->get_sink(s->root0); // tr.at(s->root0).sink;

	s->root1 = locate_endpoint(&s->v1, &s->v0, s->root1);
	s->root1 = trapezoid_struct->get_sink(s->root1); // tr.at(s->root1).sink;
}





