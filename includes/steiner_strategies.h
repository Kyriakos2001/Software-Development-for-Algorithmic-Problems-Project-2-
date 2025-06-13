#pragma once

#include "cgal_definitions.h"
#include "graph_definitions.h"

using std::string;

namespace steiner_stategies {
    enum Strategy {
        MAX_EDGE,
        PERICENTER,
        POLYGON,
        PROJECTION,
        BISECTION,
        ALTITUDE,
        NONE,
    };

    void printStrategy(Strategy strategy);

    Point * generateSteinerPointFromMaxEdge(Graph & graph, Point & a, Point & b, Point &c);

    Point * generateSteinerPointFromPericenter(Graph & graph, Point & a, Point & b, Point &c);

    Point * generateSteinerPointInsideConvexHull(Graph & graph, Point & a, Point & b, Point &c);

    Point * generateSteinerPointProjection(Graph & graph, Point & a, Point & b, Point &c);

    Point * generateSteinerPoint(Graph & graph, Point & a, Point & b, Point &c, Strategy strategy);


    Point * generateSteinerPointBiSector(Graph & graph, Point & a, Point & b, Point &c);

    Point * generateSteinerPointAltitude(Graph & graph, Point & a, Point & b, Point &c);
}