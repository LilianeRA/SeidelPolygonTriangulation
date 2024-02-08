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
    private:
        Polygon *polygon = nullptr;

        Query* query = nullptr;
        Monotonate* monotonate = nullptr;

};

#endif // TRAPEZOID_H