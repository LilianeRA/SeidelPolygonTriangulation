#ifndef MONOTONATE_H
#define MONOTONATE_H

#include "Query.h"

class Monotonate
{
    public:
        Monotonate(Polygon *polygon, Query *query);
		virtual ~Monotonate();

        int monotonate_trapezoids();
        void triangulate_monotone_polygons(int nmonpoly);
    private:

        enum class TRAVERSE_DIRECTION
        {
            TR_FROM_UP = 1, TR_FROM_DN = 2
        };

        enum class SPLITTING_TRAPEZOID
        {
            SP_ERROR = 0,
            SP_SIMPLE_LRUP = 1,
            SP_SIMPLE_LRDN = 2,
            SP_2UP_2DN = 3,
            SP_2UP_LEFT = 4,
            SP_2UP_RIGHT = 5,
            SP_2DN_LEFT = 6,
            SP_2DN_RIGHT = 7,
            SP_NOSPLIT = -1
        };

        enum class TRI
        { 
            TRI_LHS = 1, TRI_RHS = 2
        };

        using monchain_t = struct {
            int vnum;
            int next;			// Circularly linked list  
            int prev;			// describing the monotone 
            int marked;			// polygon 
        };


        using vertexchain_t = struct {
            point_t pt;
            int vnext[4];			// next vertices for the 4 chains 
            int vpos[4];			// position of v in the 4 chains 
            int nextfree;
        };

        static int chain_idx, op_idx, mon_idx;

        Query* query = nullptr;
        Polygon* polygon = nullptr;

        std::vector< monchain_t > mchain;
        std::vector< vertexchain_t > vert;
        std::vector< int > mon; // contains position of any vertex in the monotone chain for the polygon
        std::vector< bool > visited; 

        bool inside_polygon(trap_t* t);
        Monotonate::SPLITTING_TRAPEZOID traverse_polygon(int mcur, int trnum, int from, TRAVERSE_DIRECTION dir);
        int newmon();
        int new_chain_element();
        int make_new_monotone_poly(int mcur, int v0, int v1);
        void get_vertex_positions(int v0, int v1, int& ip, int& iq);
        double get_angle(point_t* vp0, point_t* vpnext, point_t* vp1);

        int triangulate_single_polygon(int nvert, int posmax, TRI side);
};

#endif // MONOTONATE_H