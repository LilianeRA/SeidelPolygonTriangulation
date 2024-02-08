#ifndef QUERY_H
#define QUERY_H

#include "Polygon.h"
#include "Trapezoid.h"

class Query
{
    public:
        Query();
		virtual ~Query();

        void Init(Polygon *polygon);

        void add_segment(int segnum);
        void find_new_roots(int segnum);

    private:
        enum class NODE_TYPE
        {
            T_X = 1, T_Y = 2, T_SINK = 3, T_ERROR = 0
        };

        // Node attributes for every node in the query structure 
        using node_t = struct node{
            int segnum = -1; // segment number
            int trnum = -1;  // trapezoid number
            NODE_TYPE nodetype = NODE_TYPE::T_ERROR; // Y-node or S-node 
            point_t yval;

            int parent = -1;  // doubly linked DAG 
            int left = -1;    // children
            int right = -1;
        };

        Polygon* polygon = nullptr;
        Trapezoid* trapezoid_struct = nullptr;
        std::vector < node_t > qs;  // Query structure 


        int newnode();
        bool inserted(int segnum, int whichpt);
        int locate_endpoint(const point_t* v, const point_t* vo, int r);

        void merge_trapezoids(int segnum, int tfirst, int tlast, Trapezoid::SIDE side);
};

#endif // QUERY_H