#include "Monotonate.h"

int Monotonate::chain_idx = 1;
int Monotonate::op_idx = 1;
int Monotonate::mon_idx = 1;

#define CROSS_SINE(v0, v1) ((v0).x * (v1).y - (v1).x * (v0).y)
#define LENGTH(v0) (sqrt((v0).x * (v0).x + (v0).y * (v0).y))
#define DOT(v0, v1) ((v0).x * (v1).x + (v0).y * (v1).y)

#define CROSS(v0, v1, v2) (((v1).x - (v0).x)*((v2).y - (v0).y) - ((v1).y - (v0).y)*((v2).x - (v0).x))

Monotonate::Monotonate(Polygon *polygon, Query* query)
{
	this->polygon = polygon;
	this->query = query;
}


Monotonate::~Monotonate()
{
	
}

int Monotonate::monotonate_trapezoids()
{
	int tr_start = -1;
	int n = polygon->get_total_segments();

	// First locate a trapezoid which lies inside the polygon and which is triangular 
	for (int i = 0; i < query->tr.size(); i++)
	{
		if (inside_polygon(&query->tr[i]))
		{
			tr_start = i;
			break;
		}
	}
	// Initialise the mon data-structure and start spanning all the 
	// trapezoids within the polygon 
	for (int i = 1; i <= n; i++)
	{
		mchain[i].prev = polygon->get_segment(i)->prev;// seg[i].prev;
		mchain[i].next = polygon->get_segment(i)->next;// seg[i].next;
		mchain[i].vnum = i;
		vert[i].pt = polygon->get_segment(i)->v0;// seg[i].v0;
		vert[i].vnext[0] = polygon->get_segment(i)->next;// seg[i].next; // next vertex 
		vert[i].vpos[0] = i;	// locn. of next vertex 
		vert[i].nextfree = 1;
	}

	chain_idx = n;
	mon_idx = 0;
	mon[0] = 1;			// position of any vertex in the first chain  


	// traverse the polygon 
	if (query->tr[tr_start].u0 > 0)
		traverse_polygon(0, tr_start, query->tr[tr_start].u0, TRAVERSE_DIRECTION::TR_FROM_UP);
	else if (query->tr[tr_start].d0 > 0)
		traverse_polygon(0, tr_start, query->tr[tr_start].d0, TRAVERSE_DIRECTION::TR_FROM_DN);

	// return the number of polygons created 
	return newmon();
}


// For each monotone polygon, find the ymax and ymin (to determine the 
// two y-monotone chains) and pass on this monotone polygon for greedy 
// triangulation. 
// Take care not to triangulate duplicate monotone polygons 
void Monotonate::triangulate_monotone_polygons(int nmonpoly)
{
	int i;
	point_t ymax, ymin;
	int p, vfirst, posmax, posmin, v;
	int vcount, processed;
	int nvert = polygon->get_total_segments();

#ifdef DEBUG
	for (i = 0; i < nmonpoly; i++)
	{
		fprintf(stderr, "\n\nPolygon %d: ", i);
		vfirst = mchain[mon[i]].vnum;
		p = mchain[mon[i]].next;
		fprintf(stderr, "%d ", mchain[mon[i]].vnum);
		while (mchain[p].vnum != vfirst)
		{
			fprintf(stderr, "%d ", mchain[p].vnum);
			p = mchain[p].next;
		}
	}
	fprintf(stderr, "\n");
#endif

	op_idx = 0;
	for (i = 0; i < nmonpoly; i++)
	{
		vcount = 1;
		processed = false;
		vfirst = mchain[mon[i]].vnum;
		ymax = ymin = vert[vfirst].pt;
		posmax = posmin = mon[i];
		mchain[mon[i]].marked = true;
		p = mchain[mon[i]].next;
		while ((v = mchain[p].vnum) != vfirst)
		{
			if (mchain[p].marked)
			{
				processed = true;
				break;		/* break from while */
			}
			else
				mchain[p].marked = true;

			if (query->_greater_than(&vert[v].pt, &ymax))
			{
				ymax = vert[v].pt;
				posmax = p;
			}
			if (query->_less_than(&vert[v].pt, &ymin))
			{
				ymin = vert[v].pt;
				posmin = p;
			}
			p = mchain[p].next;
			vcount++;
		}


		if (processed)		// Go to next polygon 
			continue;

		if (vcount == 3)		// already a triangle 
		{
			/*op[op_idx][0] = mchain[p].vnum;
			op[op_idx][1] = mchain[mchain[p].next].vnum;
			op[op_idx][2] = mchain[mchain[p].prev].vnum;
			op_idx++;*/
		}
		else			// triangulate the polygon 
		{
			v = mchain[mchain[posmax].next].vnum;
			if (query->_equal_to(&vert[v].pt, &ymin))
			{			// LHS is a single line 
				triangulate_single_polygon(nvert, posmax, TRI::TRI_LHS); // , op);
			}
			else
				triangulate_single_polygon(nvert, posmax, TRI::TRI_RHS); // , op);
		}
	}

#ifdef DEBUG
	for (i = 0; i < op_idx; i++)
		fprintf(stderr, "tri #%d: (%d, %d, %d)\n", i, op[i][0], op[i][1],
			op[i][2]);
#endif
	
	///////return op_idx;
}



// return a new mon structure from the table
int Monotonate::newmon()
{
	mon.push_back(0);

	return mon.size() - 1;
}
// return a new chain element from the table
int Monotonate::new_chain_element()
{
	monchain_t m;
	mchain.push_back(m);

	return mchain.size() - 1;
}

// Function returns TRUE if the trapezoid lies inside the polygon 
bool Monotonate::inside_polygon(trap_t* t)
{
	int rseg = t->rseg;

	if (t->state == trap_t::STATE::ST_INVALID)
		return false;

	if ((t->lseg <= 0) || (t->rseg <= 0))
		return false;

	if (((t->u0 <= 0) && (t->u1 <= 0)) || ((t->d0 <= 0) && (t->d1 <= 0))) // triangle 
		return (query->_greater_than(&polygon->get_segment(rseg)->v1, &polygon->get_segment(rseg)->v0));
		//return (query->_greater_than(&seg[rseg].v1, &seg[rseg].v0));

	return false;
}

/* recursively visit all the trapezoids */
Monotonate::SPLITTING_TRAPEZOID Monotonate::traverse_polygon(int mcur, int trnum, int from, TRAVERSE_DIRECTION dir)
{
	trap_t* t = &query->tr[trnum];
	int howsplit, mnew;
	int v0, v1, v0next, v1next;
	SPLITTING_TRAPEZOID retval;
	int tmp;
	int do_switch = false;

	if ((trnum <= 0) || visited[trnum])
		return SPLITTING_TRAPEZOID::SP_ERROR;

	visited[trnum] = true;

	// We have much more information available here. 
	// rseg: goes upwards   
	// lseg: goes downwards 

	// Initially assume that dir = TR_FROM_DN (from the left) 
	// Switch v0 and v1 if necessary afterwards 


	// special cases for triangles with cusps at the opposite ends. 
	// take care of this first

	if ((t->u0 <= 0) && (t->u1 <= 0))
	{
		if ((t->d0 > 0) && (t->d1 > 0)) // downward opening triangle 
		{
			v0 = query->tr[t->d1].lseg;
			v1 = t->lseg;
			if (from == t->d1)
			{
				do_switch = true;
				mnew = make_new_monotone_poly(mcur, v1, v0);
				traverse_polygon(mcur, t->d1, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
				traverse_polygon(mnew, t->d0, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
			}
			else
			{
				mnew = make_new_monotone_poly(mcur, v0, v1);
				traverse_polygon(mcur, t->d0, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
				traverse_polygon(mnew, t->d1, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
			}
		}
		else
		{
			retval = SPLITTING_TRAPEZOID::SP_NOSPLIT;	// Just traverse all neighbours
			traverse_polygon(mcur, t->u0, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
			traverse_polygon(mcur, t->u1, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
			traverse_polygon(mcur, t->d0, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
			traverse_polygon(mcur, t->d1, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
		}
	}

	else if ((t->d0 <= 0) && (t->d1 <= 0))
	{
		if ((t->u0 > 0) && (t->u1 > 0)) // upward opening triangle
		{
			v0 = t->rseg;
			v1 = query->tr[t->u0].rseg;
			if (from == t->u1)
			{
				do_switch = true;
				mnew = make_new_monotone_poly(mcur, v1, v0);
				traverse_polygon(mcur, t->u1, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
				traverse_polygon(mnew, t->u0, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
			}
			else
			{
				mnew = make_new_monotone_poly(mcur, v0, v1);
				traverse_polygon(mcur, t->u0, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
				traverse_polygon(mnew, t->u1, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
			}
		}
		else
		{
			retval = SPLITTING_TRAPEZOID::SP_NOSPLIT;	/* Just traverse all neighbours */
			traverse_polygon(mcur, t->u0, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
			traverse_polygon(mcur, t->u1, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
			traverse_polygon(mcur, t->d0, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
			traverse_polygon(mcur, t->d1, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
		}
	}


	else if ((t->u0 > 0) && (t->u1 > 0))
	{
		if ((t->d0 > 0) && (t->d1 > 0)) // downward + upward cusps 
		{
			v0 = query->tr[t->d1].lseg;
			v1 = query->tr[t->u0].rseg;
			retval = SPLITTING_TRAPEZOID::SP_2UP_2DN;
			if (((dir == TRAVERSE_DIRECTION::TR_FROM_DN) && (t->d1 == from)) ||
				((dir == TRAVERSE_DIRECTION::TR_FROM_UP) && (t->u1 == from)))
			{
				do_switch = true;
				mnew = make_new_monotone_poly(mcur, v1, v0);
				traverse_polygon(mcur, t->u1, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
				traverse_polygon(mcur, t->d1, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
				traverse_polygon(mnew, t->u0, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
				traverse_polygon(mnew, t->d0, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
			}
			else
			{
				mnew = make_new_monotone_poly(mcur, v0, v1);
				traverse_polygon(mcur, t->u0, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
				traverse_polygon(mcur, t->d0, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
				traverse_polygon(mnew, t->u1, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
				traverse_polygon(mnew, t->d1, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
			}
		}
		else			// only downward cusp 
		{
			//if (query->_equal_to(&t->lo, &seg[t->lseg].v1))
			if (query->_equal_to(&t->lo, &polygon->get_segment(t->lseg)->v1))
			{
				v0 = query->tr[t->u0].rseg;
				v1 = polygon->get_segment(t->lseg)->next; // seg[t->lseg].next;

				retval = SPLITTING_TRAPEZOID::SP_2UP_LEFT;
				if ((dir == TRAVERSE_DIRECTION::TR_FROM_UP) && (t->u0 == from))
				{
					do_switch = true;
					mnew = make_new_monotone_poly(mcur, v1, v0);
					traverse_polygon(mcur, t->u0, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
					traverse_polygon(mnew, t->d0, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
					traverse_polygon(mnew, t->u1, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
					traverse_polygon(mnew, t->d1, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
				}
				else
				{
					mnew = make_new_monotone_poly(mcur, v0, v1);
					traverse_polygon(mcur, t->u1, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
					traverse_polygon(mcur, t->d0, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
					traverse_polygon(mcur, t->d1, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
					traverse_polygon(mnew, t->u0, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
				}
			}
			else
			{
				v0 = t->rseg;
				v1 = query->tr[t->u0].rseg;
				retval = SPLITTING_TRAPEZOID::SP_2UP_RIGHT;
				if ((dir == TRAVERSE_DIRECTION::TR_FROM_UP) && (t->u1 == from))
				{
					do_switch = true;
					mnew = make_new_monotone_poly(mcur, v1, v0);
					traverse_polygon(mcur, t->u1, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
					traverse_polygon(mnew, t->d1, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
					traverse_polygon(mnew, t->d0, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
					traverse_polygon(mnew, t->u0, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
				}
				else
				{
					mnew = make_new_monotone_poly(mcur, v0, v1);
					traverse_polygon(mcur, t->u0, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
					traverse_polygon(mcur, t->d0, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
					traverse_polygon(mcur, t->d1, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
					traverse_polygon(mnew, t->u1, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
				}
			}
		}
	}
	else if ((t->u0 > 0) || (t->u1 > 0)) // no downward cusp
	{
		if ((t->d0 > 0) && (t->d1 > 0)) // only upward cusp
		{
			//if (query->_equal_to(&t->hi, &seg[t->lseg].v0))
			if (query->_equal_to(&t->hi, &polygon->get_segment(t->lseg)->v0))
			{
				v0 = query->tr[t->d1].lseg;
				v1 = t->lseg;
				retval = SPLITTING_TRAPEZOID::SP_2DN_LEFT;
				if (!((dir == TRAVERSE_DIRECTION::TR_FROM_DN) && (t->d0 == from)))
				{
					do_switch = true;
					mnew = make_new_monotone_poly(mcur, v1, v0);
					traverse_polygon(mcur, t->u1, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
					traverse_polygon(mcur, t->d1, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
					traverse_polygon(mcur, t->u0, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
					traverse_polygon(mnew, t->d0, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
				}
				else
				{
					mnew = make_new_monotone_poly(mcur, v0, v1);
					traverse_polygon(mcur, t->d0, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
					traverse_polygon(mnew, t->u0, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
					traverse_polygon(mnew, t->u1, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
					traverse_polygon(mnew, t->d1, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
				}
			}
			else
			{
				v0 = query->tr[t->d1].lseg;
				v1 = polygon->get_segment(t->rseg)->next;// seg[t->rseg].next;

				retval = SPLITTING_TRAPEZOID::SP_2DN_RIGHT;
				if ((dir == TRAVERSE_DIRECTION::TR_FROM_DN) && (t->d1 == from))
				{
					do_switch = true;
					mnew = make_new_monotone_poly(mcur, v1, v0);
					traverse_polygon(mcur, t->d1, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
					traverse_polygon(mnew, t->u1, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
					traverse_polygon(mnew, t->u0, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
					traverse_polygon(mnew, t->d0, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
				}
				else
				{
					mnew = make_new_monotone_poly(mcur, v0, v1);
					traverse_polygon(mcur, t->u0, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
					traverse_polygon(mcur, t->d0, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
					traverse_polygon(mcur, t->u1, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
					traverse_polygon(mnew, t->d1, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
				}
			}
		}
		else			// no cusp 
		{
			//if (query->_equal_to(&t->hi, &seg[t->lseg].v0) &&
			//	  query->_equal_to(&t->lo, &seg[t->rseg].v0))
			if (query->_equal_to(&t->hi, &polygon->get_segment(t->lseg)->v0) &&
				query->_equal_to(&t->lo, &polygon->get_segment(t->rseg)->v0))
			{
				v0 = t->rseg;
				v1 = t->lseg;
				retval = SPLITTING_TRAPEZOID::SP_SIMPLE_LRDN;
				if (dir == TRAVERSE_DIRECTION::TR_FROM_UP)
				{
					do_switch = true;
					mnew = make_new_monotone_poly(mcur, v1, v0);
					traverse_polygon(mcur, t->u0, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
					traverse_polygon(mcur, t->u1, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
					traverse_polygon(mnew, t->d1, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
					traverse_polygon(mnew, t->d0, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
				}
				else
				{
					mnew = make_new_monotone_poly(mcur, v0, v1);
					traverse_polygon(mcur, t->d1, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
					traverse_polygon(mcur, t->d0, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
					traverse_polygon(mnew, t->u0, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
					traverse_polygon(mnew, t->u1, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
				}
			}
			//else if (query->_equal_to(&t->hi, &seg[t->rseg].v1) &&
			//		   query->_equal_to(&t->lo, &seg[t->lseg].v1))
			else if (query->_equal_to(&t->hi, &polygon->get_segment(t->rseg)->v1) &&
					 query->_equal_to(&t->lo, &polygon->get_segment(t->lseg)->v1))
			{
				v0 = polygon->get_segment(t->rseg)->next;// seg[t->rseg].next;
				v1 = polygon->get_segment(t->lseg)->next;// seg[t->lseg].next;

				retval = SPLITTING_TRAPEZOID::SP_SIMPLE_LRUP;
				if (dir == TRAVERSE_DIRECTION::TR_FROM_UP)
				{
					do_switch = true;
					mnew = make_new_monotone_poly(mcur, v1, v0);
					traverse_polygon(mcur, t->u0, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
					traverse_polygon(mcur, t->u1, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
					traverse_polygon(mnew, t->d1, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
					traverse_polygon(mnew, t->d0, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
				}
				else
				{
					mnew = make_new_monotone_poly(mcur, v0, v1);
					traverse_polygon(mcur, t->d1, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
					traverse_polygon(mcur, t->d0, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
					traverse_polygon(mnew, t->u0, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
					traverse_polygon(mnew, t->u1, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
				}
			}
			else			/* no split possible */
			{
				retval = SPLITTING_TRAPEZOID::SP_NOSPLIT;
				traverse_polygon(mcur, t->u0, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
				traverse_polygon(mcur, t->d0, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
				traverse_polygon(mcur, t->u1, trnum, TRAVERSE_DIRECTION::TR_FROM_DN);
				traverse_polygon(mcur, t->d1, trnum, TRAVERSE_DIRECTION::TR_FROM_UP);
			}
		}
	}

	return retval;
}

// v0 and v1 are specified in anti-clockwise order with respect to
// the current monotone polygon mcur. Split the current polygon into
// two polygons using the diagonal (v0, v1)
int Monotonate::make_new_monotone_poly(int mcur, int v0, int v1)
{
	int p, q, ip, iq;
	int mnew = newmon();
	int i, j, nf0, nf1;
	vertexchain_t* vp0, * vp1;

	vp0 = &vert[v0];
	vp1 = &vert[v1];

	get_vertex_positions(v0, v1, ip, iq);

	p = vp0->vpos[ip];
	q = vp1->vpos[iq];

	// At this stage, we have got the positions of v0 and v1 in the 
	// desired chain. Now modify the linked lists 

	i = new_chain_element();	// for the new list 
	j = new_chain_element();

	mchain[i].vnum = v0;
	mchain[j].vnum = v1;

	mchain[i].next = mchain[p].next;
	mchain[mchain[p].next].prev = i;
	mchain[i].prev = j;
	mchain[j].next = i;
	mchain[j].prev = mchain[q].prev;
	mchain[mchain[q].prev].next = j;

	mchain[p].next = q;
	mchain[q].prev = p;

	nf0 = vp0->nextfree;
	nf1 = vp1->nextfree;

	vp0->vnext[ip] = v1;

	vp0->vpos[nf0] = i;
	vp0->vnext[nf0] = mchain[mchain[i].next].vnum;
	vp1->vpos[nf1] = j;
	vp1->vnext[nf1] = v0;

	vp0->nextfree++;
	vp1->nextfree++;

	/*
#ifdef DEBUG
	fprintf(stderr, "make_poly: mcur = %d, (v0, v1) = (%d, %d)\n",
		mcur, v0, v1);
	fprintf(stderr, "next posns = (p, q) = (%d, %d)\n", p, q);
#endif
	*/

	mon[mcur] = p;
	mon[mnew] = i;
	return mnew;
}

// (v0, v1) is the new diagonal to be added to the polygon. Find which 
// chain to use and return the positions of v0 and v1 in p and q 
void Monotonate::get_vertex_positions(int v0, int v1, int& ip, int& iq)
{
	vertexchain_t* vp0, * vp1;
	int i;
	double angle, temp;
	int tp, tq;

	vp0 = &vert[v0];
	vp1 = &vert[v1];

	// p is identified as follows. Scan from (v0, v1) rightwards till 
	// you hit the first segment starting from v0. That chain is the 
	// chain of our interest 

	angle = -4.0;
	for (i = 0; i < 4; i++)
	{
		if (vp0->vnext[i] <= 0)
			continue;
		if ((temp = get_angle(&vp0->pt, &(vert[vp0->vnext[i]].pt),
			&vp1->pt)) > angle)
		{
			angle = temp;
			tp = i;
		}
	}

	ip = tp;

	// Do similar actions for q 

	angle = -4.0;
	for (i = 0; i < 4; i++)
	{
		if (vp1->vnext[i] <= 0)
			continue;
		if ((temp = get_angle(&vp1->pt, &(vert[vp1->vnext[i]].pt),
			&vp0->pt)) > angle)
		{
			angle = temp;
			tq = i;
		}
	}

	iq = tq;
}

double Monotonate::get_angle(point_t* vp0, point_t* vpnext, point_t* vp1)
{
	point_t v0, v1;

	v0.x = vpnext->x - vp0->x;
	v0.y = vpnext->y - vp0->y;

	v1.x = vp1->x - vp0->x;
	v1.y = vp1->y - vp0->y;

	if (CROSS_SINE(v0, v1) >= 0)	/* sine is positive */
		return DOT(v0, v1) / LENGTH(v0) / LENGTH(v1);
	else
		return (-1.0 * DOT(v0, v1) / LENGTH(v0) / LENGTH(v1) - 2);
}


// A greedy corner-cutting algorithm to triangulate a y-monotone polygon in O(n) time.
// Joseph O-Rourke, Computational Geometry in C.
int Monotonate::triangulate_single_polygon(int nvert, int posmax, TRI side) //, int op[][3])
{
	int v;
	int ri = 0;	// reflex chain 
	//int rc[SEGSIZE];
	std::vector<int> rc;
	int endv, tmp, vpos;

	if (side == TRI::TRI_RHS)		// RHS segment is a single segment 
	{
		rc[0] = mchain[posmax].vnum;
		tmp = mchain[posmax].next;
		rc[1] = mchain[tmp].vnum;
		ri = 1;

		vpos = mchain[tmp].next;
		v = mchain[vpos].vnum;

		if ((endv = mchain[mchain[posmax].prev].vnum) == 0)
			endv = nvert;
	}
	else	// LHS is a single segment 
	{
		tmp = mchain[posmax].next;
		rc[0] = mchain[tmp].vnum;
		tmp = mchain[tmp].next;
		rc[1] = mchain[tmp].vnum;
		ri = 1;

		vpos = mchain[tmp].next;
		v = mchain[vpos].vnum;

		endv = mchain[posmax].vnum;
	}

	while ((v != endv) || (ri > 1))
	{
		if (ri > 0)	// reflex chain is non-empty 
		{
			if (CROSS(vert[v].pt, vert[rc[ri - 1]].pt,
				vert[rc[ri]].pt) > 0)
			{		// convex corner: cut if off 
				//op[op_idx][0] = rc[ri - 1];
				//op[op_idx][1] = rc[ri];
				//op[op_idx][2] = v;
				//op_idx++;
				ri--;
			}
			else    // non-convex 
			{		// add v to the chain 
				ri++;
				rc[ri] = v;
				vpos = mchain[vpos].next;
				v = mchain[vpos].vnum;
			}
		}
		else		// reflex-chain empty: add v to the 
		{			// reflex chain and advance it  
			rc[++ri] = v;
			vpos = mchain[vpos].next;
			v = mchain[vpos].vnum;
		}
	} // end-while 

	// reached the bottom vertex. Add in the triangle formed
	//op[op_idx][0] = rc[ri - 1];
	//op[op_idx][1] = rc[ri];
	//op[op_idx][2] = v;
	//op_idx++;
	ri--;

	return 0;
}


