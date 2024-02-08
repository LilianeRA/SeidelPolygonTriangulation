#ifndef POINT_H
#define POINT_H

class point_t
{
    public:
        point_t();
		virtual ~point_t();

		double x = -1e10, y = -1e10, z = -1e10;

		// Return the maximum of the two points into the yval structure 
		static void _max(point_t* yval, const point_t* v0, const point_t* v1);
		static void _min(point_t* yval, const point_t* v0, const point_t* v1);
		static bool _greater_than(const point_t* v0, const point_t* v1);
		static bool _equal_to(const point_t* v0, const point_t* v1);
		static bool _equal_y(const point_t* v0, const point_t* v1);
		static bool _greater_than_equal_to(const point_t* v0, const point_t* v1);
		static bool _less_than(const point_t* v0, const point_t* v1);
};

#endif // POINT_H