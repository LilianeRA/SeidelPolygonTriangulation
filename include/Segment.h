#ifndef SEGMENT_H
#define SEGMENT_H

#include "Point.h"

class segment_t
{
    public:
        segment_t();
		virtual ~segment_t();
		
		point_t v0, v1;				// two endpoints 
		int is_inserted = false;	// inserted in trapezoidation yet ? 
		int root0 = -1, root1 = -1;	// root nodes in Q 
		int next = -1;	// Next logical segment 
		int prev = -1;	// Previous segment 

		//segment_t* next = nullptr;	// Next logical segment 
		//segment_t *prev = nullptr;	// Previous segment 

		static bool is_left_of(const segment_t* s, const point_t* v);
};

#endif // SEGMENT_H