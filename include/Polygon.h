#ifndef POLYGON_H
#define POLYGON_H

#include <string>
#include <vector>
#include "Segment.h"

class Polygon
{
    public:
        Polygon();
		virtual ~Polygon();

        bool ReadFile(const std::string &filepath);

		void Init();

		int choose_permutation() const;
		segment_t* get_segment(int segnum);
		int get_total_segments() const;

		void set_segment_inserted(int segnum, bool inserted);
		bool set_root0(int index, int root);
		bool set_root1(int index, int root);

	private:
				
		//int genus;
		std::vector< segment_t > segments;
		std::vector<int> permute;


		static int choose_idx;

		void Print();
		void generate_random_ordering();
};

#endif // POLYGON_H