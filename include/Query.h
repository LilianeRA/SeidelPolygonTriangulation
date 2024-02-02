#ifndef QUERY_H
#define QUERY_H

#include "Polygon.h"
#include "Trapezoid.h"

class Query
{
    public:
        Query(Polygon* polygon);
		virtual ~Query();

        int init_query_structure(int segnum, int total_segments);
        void add_segment(int segnum);
        void find_new_roots(int segnum);

        std::vector < trap_t > tr;      // Trapezoid structure

        bool _equal_to(const point_t* v0, const point_t* v1);
        bool _greater_than(const point_t* v0, const point_t* v1);
        bool _less_than(const point_t* v0, const point_t* v1);
    private:
        Polygon* polygon = nullptr;
        // Node attributes for every node in the query structure 

        using node_t = struct {
            int nodetype;			// Y-node or S-node 
            int segnum;
            point_t yval;
            int trnum;
            int parent;			    // doubly linked DAG 
            int left, right;		// children 
        };


        int q_idx = 0;
        int tr_idx = 0;

        std::vector < node_t > qs;	    // Query structure 

        int newnode();
        int newtrap();

        void _max(point_t* yval, const point_t* v0, const point_t* v1);
        void _min(point_t* yval, const point_t* v0, const point_t* v1);

        bool _greater_than_equal_to(const point_t* v0, const point_t* v1);
        bool inserted(int segnum, int whichpt);
        bool is_left_of(int segnum, const point_t* v);

        int locate_endpoint(point_t* v, const point_t* vo, int r);


        void merge_trapezoids(int segnum, int tfirst, int tlast, int side);

};

#endif // QUERY_H