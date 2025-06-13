// Standard C++
#include <gmp.h>
#include <iostream>
#include <map>
#include <vector>

// Macros and headers for CGAL
#include "cgal_definitions.h"

// Macros and headers for boost
#include "boost_definitions.h"

// Macros and headers for QT
#include "qt_definitions.h"

// Configuration
#include "triangulation_configuration.h"

// Support classes
#include "JsonExporter.h"
#include "JsonLoader.h"
#include "graph_definitions.h"
#include "steiner_strategies.h"
#include "utils.hpp"

// Namespaces
using namespace std;

template <typename T>
class LocalSearch {
public:
    vector<Point> triangulate(vector<steiner_stategies::Strategy>& strategies, Graph& graph, JsonLoader& loader, Polygon& boundaryPolygon) {
        vector<Point> steinerPoints;
        CDT& cdt = *(graph.cdt);

        int MAX_ITERATIONS = loader.getL();
        int obtuse_triangles_initial = utils::countObtuseTriangles(cdt);
        int obtuse_triangles_before = 0;
        int obtuse_triangles_after = 0;

        cout << "# Max iterations: " << MAX_ITERATIONS << endl;

        for (int i = 1; i <= MAX_ITERATIONS; i++) {
            int conflicts = 0;

            obtuse_triangles_before = utils::countObtuseTriangles(cdt);
            obtuse_triangles_after = 0;

            std::vector<CDT::Face_handle> finite_faces;

            for (auto fit = cdt.finite_faces_begin(); fit != cdt.finite_faces_end(); ++fit) {
                finite_faces.push_back(fit);
            }

            //
            // Optimization algorithm
            //

            for (auto fit : finite_faces) {
                Point a = fit->vertex(0)->point();
                Point b = fit->vertex(1)->point();
                Point c = fit->vertex(2)->point();

                bool result = utils::is_obtuse(a, b, c);

                cout << " - Iteration: " << i << " Checking triangle: " << a << "," << b << "," << c << ", obtuse:" << result << ", obtuse triangles: " << obtuse_triangles_before << endl;

                conflicts++;

                if (result) {                    
                    // ---------------------------------------------------------
                    map<steiner_stategies::Strategy, int> options;

                    for (steiner_stategies::Strategy& strategy : strategies) {
                        CDT cdt_copy = cdt;
                        Graph graph_copy;
                        graph_copy.cdt = &cdt_copy;

                        if (strategy == steiner_stategies::Strategy::PERICENTER) { // if max edge is constraint skip ...
                            int i = utils::find_obtuse_angle(a, b, c);             // 0:a, 1:b, 2:c
                            if (i == -1) {
                                cout << "CRITICAL ERROR: find_obtuse_angle failed " << endl;
                                exit(1);
                            }

                            std::tuple<int, int> edge_indices = utils::findOppositeEdge(i); // a:1,2 b:0,2 c:0,1

                            Point& p1 = fit->vertex(std::get<0>(edge_indices))->point();
                            Point& p2 = fit->vertex(std::get<1>(edge_indices))->point();

                            bool is_constraint = utils::checkConstraints(cdt, p1, p2);

                            if (is_constraint) {
                                continue;
                            }
                        }

                        Point* s = steiner_stategies::generateSteinerPoint(graph_copy, a, b, c, strategy);

                        if (s != nullptr) {
                            if (boundaryPolygon.bounded_side(*s) == CGAL::ON_BOUNDED_SIDE) {
                                // cout << "Inserting ... " << s << endl;
                                if (strategy == steiner_stategies::Strategy::POLYGON) {
                                    cdt_copy.insert(*s);
                                } else {
                                    cdt_copy.insert_no_flip(*s);
                                }
                            } else {
                                // cout << "Steiner point ignored  - outside the boundaries " << endl;
                            }

                            delete s;

                            int copy_obtuse_triangles_after = utils::countObtuseTriangles(cdt_copy) ;

                            options[strategy] = copy_obtuse_triangles_after;

                            cout << "\t";
                            steiner_stategies::printStrategy(strategy);
                            cout << " - Method succeeded " << copy_obtuse_triangles_after << endl;
                        } else {
                            cout << "\t";
                            steiner_stategies::printStrategy(strategy);
                            cout << " - Method failed    " << endl;
                        }

                    }
                    // ---------------------------------------------------------
                    int min_value = std::numeric_limits<int>::max();
                    steiner_stategies::Strategy strategy = steiner_stategies::Strategy::NONE;

                    for (const auto& [key, value] : options) {
                        if (value < min_value) {
                            min_value = value;
                            strategy = key;
                        }
                    }

                    if (strategy != steiner_stategies::Strategy::NONE) {                        
                        Point* s = steiner_stategies::generateSteinerPoint(graph, a, b, c, strategy);

                        cout << "*Strategy selected: " ;

                        steiner_stategies::printStrategy(strategy);

                        cout << endl;

                        if (boundaryPolygon.bounded_side(*s) == CGAL::ON_BOUNDED_SIDE) {
                            if (strategy == steiner_stategies::Strategy::POLYGON) {
                                cdt.insert(*s);
                            } else {
                                cdt.insert_no_flip(*s);
                            }
                            steinerPoints.emplace_back(*s);
                        } else {
                            // cout << "Steiner point ignored  - outside the boundaries " << endl;
                        }

                        delete s;
                    }
                }
            }

            obtuse_triangles_after = utils::countObtuseTriangles(cdt);

            cout << " ### Initial: " << obtuse_triangles_initial << ", before: " << obtuse_triangles_before << ", after: " << obtuse_triangles_before << endl;
            // if (obtuse_triangles_after >= obtuse_triangles_before || conflicts == 0 || obtuse_triangles_after == 0) {
            //     break;
            // }

            if (obtuse_triangles_after == 0) {
                break;
            }
        }

        cout << "***********************************************************************" << endl;
        cout << " - Initial obtuse triangles  : " << obtuse_triangles_initial << endl;
        cout << " - Total obtuse triangles    : " << obtuse_triangles_after << endl;
        cout << " - Total steiner points      : " << steinerPoints.size() << endl;
        cout << "***********************************************************************" << endl;

        return steinerPoints;
    }
};
