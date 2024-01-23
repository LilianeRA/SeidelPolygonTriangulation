#ifndef POLYGON_H
#define POLYGON_H

#include <string>
#include <vector>

class Polygon
{
    public:
        Polygon();
		virtual ~Polygon();

        bool ReadFile(const std::string &filepath);

		void Init();


	private:

		using point_t = struct {
			double x, y, z;
		};
		using vector_t = struct {
			double x, y, z;
		};

		using segment_t = struct {
			point_t v0, v1;		// two endpoints 
			int is_inserted;	// inserted in trapezoidation yet ? 
			int root0, root1;	// root nodes in Q 
			int next;			// Next logical segment 
			int prev;			// Previous segment 
		};
		
		int genus;
		std::vector< segment_t > segments;


		static int choose_idx;

		void Print();
		void generate_random_ordering();
};

#endif // POLYGON_H