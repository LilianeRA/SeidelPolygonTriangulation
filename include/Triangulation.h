#ifndef TRIANGULATION_H
#define TRIANGULATION_H

#include "Polygon.h"
#include "Query.h"
#include "Monotonate.h"

class Triangulation
{
    public:
        Triangulation();
		virtual ~Triangulation();

        bool ReadFile(const std::string& filepath);

        void Init();
        void construct_trapezoids();
        void monotonate_trapezoids();
        void triangulate_monotone_polygons();
    private:
        Polygon *polygon = nullptr;
        Query* query = nullptr;
        Monotonate* monotonate = nullptr;

        int total_monotone = 0;

};

#endif // TRAPEZOID_H