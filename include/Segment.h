#ifndef SEGMENT_H
#define SEGMENT_H

#include "Point.h"

class segment_t
{
    public:
        segment_t();
		virtual ~segment_t();
		
		point_t v0, v1;		// two endpoints 
		int is_inserted;	// inserted in trapezoidation yet ? 
		int root0, root1;	// root nodes in Q 
		int next;			// Next logical segment 
		int prev;			// Previous segment 

};

#endif // SEGMENT_H