#ifndef TRAPEZOID_H
#define TRAPEZOID_H

#include "Polygon.h"
#include <vector>

class Trapezoid
{
    public:
		Trapezoid();
		virtual ~Trapezoid();

		enum class STATE
		{
			ST_VALID = 1, ST_INVALID = 2
		};
		enum class SIDE // for merge-direction
		{
			S_LEFT = 1, S_RIGHT = 2, S_NONE = 0
		};
		void Init(const point_t& qs_i1_yval, const point_t& qs_i3_yval, const int segnum,
			int& t1, int& t2, int& t3, int& t4, int i6, int i7, int i4, int i2);

		bool add_segment(Polygon* polygon, const segment_t* s, int &t, int tn, int tlast, bool tribot, bool is_swapped);
		bool merge_trapezoids(int segnum, int& t, SIDE side, int& tr_t_sink, int& tr_tnext_sink);


		void copy(int from_index, int to_index);
		// Return a free trapezoid 
		int newtrap(STATE state = STATE::ST_INVALID);
		int new_lower_trap(int tu, const segment_t* s);
		void set_sink(int index, int sink);

		int get_sink(int index);
		const point_t* get_min_y(int index) const;
	private:


		using trap_t = struct trap
		{
			int lseg = -1, rseg = -1;	// two adjoining segments 
			point_t hi, lo;				// max/min y-values 
			int u0 = 0, u1 = 0;
			int d0 = 0, d1 = 0;
			int sink = 0;				// pointer to corresponding in Q
			int usave = 0;				// I forgot what this means 
			enum SIDE uside = SIDE::S_NONE;	
			enum STATE state = STATE::ST_INVALID;
		};

		std::vector < trap_t > tr;  // Trapezoid structure
	
};

#endif // TRAPEZOID_H