#ifndef TRAPEZOID_H
#define TRAPEZOID_H

#include "Point.h"

class trap_t
{
    public:
        trap_t();
		virtual ~trap_t();

		enum class STATE
		{
			ST_VALID = 1, ST_INVALID = 2
		};
		
		int lseg, rseg;		// two adjoining segments 
		point_t hi, lo;		// max/min y-values 
		int u0, u1;
		int d0, d1;
		int sink;			// pointer to corresponding in Q
		int usave, uside;	// I forgot what this means 
		STATE state;
	
};

#endif // TRAPEZOID_H