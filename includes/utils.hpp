#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <tuple>
#include <vector>

#include "cgal_definitions.h"
#include "graph_definitions.h"

using std::string;

namespace utils {
    string simple_or_not(const Polygon_2 & p);

    bool is_obtuse(Point & a, Point & b, Point &c);

    int find_obtuse_angle(Point & a, Point & b, Point &c);

    string coordinate_to_rational(const K::FT& coord);    

    std::tuple<int, int> findOppositeEdge(int vertexIndex);

    bool checkConstraints(const CDT& cdt, const Point& p1, const Point& p2);

    Point findGeometricalMean(Polygon & polygon);

    bool isConvex(Polygon & polygon);

    Point centroid(std::vector<Point> & points);

    int countObtuseTriangles(CDT& cdt);

    template <typename CDT>
    int countObtuseTriangles(const CDT &cdt);

    // index 0: vertex a: edge: bc
    // index 1: vertex b: edge: ac
    // index 2: vertex c: edge: ab
    Face * findNeighbor(Graph & graph, Point& a, Point& b, Point& c, int vertex_index, int & neighbor_vertex_index);


    bool is_convex(const std::vector<Point>& boundary);


}

string to_rational(const K::FT& coord);

#endif
