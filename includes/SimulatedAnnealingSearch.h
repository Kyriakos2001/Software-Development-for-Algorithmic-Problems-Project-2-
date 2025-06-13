// Standard C++
#include <cmath>
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

template <typename U>
class SimulatedAnnealingSearch {
private:
    float calculateEnergy(float alpha, float beta, int obtuse_triangles, int steiner_points) {
        return alpha * obtuse_triangles + beta * steiner_points;
    }

    float calculateProbability(float e2, float e1, float T) { // dE = e2-e1
        return exp(-(e2 - e1) / T);
    }

public:
    vector<Point> triangulate(vector<steiner_stategies::Strategy>& strategies, Graph& graph, JsonLoader& loader, Polygon& boundaryPolygon, float alpha, float beta) {
        vector<Point> steinerPoints;
        CDT& cdt = *(graph.cdt);

        int MAX_ITERATIONS = loader.getL();
        int obtuse_triangles_initial = utils::countObtuseTriangles(cdt);
        int obtuse_triangles_before = 0;
        int obtuse_triangles_after = 0;
        
        float T = 1; // temperature

        cout << "# Max iterations: " << MAX_ITERATIONS << endl;

        float E = calculateEnergy(alpha, beta, obtuse_triangles_initial, steinerPoints.size());
        int i = 0;

        while (T >= 0) {
            cout << " *** Current Energy: " << E << ", T = " << T << endl;

            int conflicts = 0;

            obtuse_triangles_before = utils::countObtuseTriangles(cdt);
            obtuse_triangles_after = 0;

            float E_current = calculateEnergy(alpha, beta, obtuse_triangles_before, steinerPoints.size());
            float E_next = 0;

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

                cout << " - Iteration: " << i << " Temperature: " << T << ": Checking triangle: " << a << "," << b << "," << c << ", obtuse:" << result << ", obtuse triangles: " << obtuse_triangles_before << endl;

                conflicts++;

                if (result) {
                    // ---------------------------------------------------------
                    int n = strategies.size();
                    int N = rand() % n;

                    steiner_stategies::Strategy& selected_strategy = strategies[N];

                    CDT cdt_copy = cdt;
                    Graph graph_copy;
                    graph_copy.cdt = &cdt_copy;

                    if (selected_strategy == steiner_stategies::Strategy::PERICENTER) { // if max edge is constraint skip ...
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

                    Point* s = steiner_stategies::generateSteinerPoint(graph_copy, a, b, c, selected_strategy);

                    E_next = E_current;

                    if (s != nullptr) {
                        if (boundaryPolygon.bounded_side(*s) == CGAL::ON_BOUNDED_SIDE) {
                            // cout << "Inserting ... " << s << endl;
                            if (selected_strategy == steiner_stategies::Strategy::POLYGON) {
                                cdt_copy.insert(*s);
                            } else {
                                cdt_copy.insert_no_flip(*s);
                            }
                        }

                        int copy_obtuse_triangles_after = utils::countObtuseTriangles(cdt_copy);
                        
                        E_next = calculateEnergy(alpha, beta, copy_obtuse_triangles_after, steinerPoints.size() + 1);

                        cout << "\t";
                        steiner_stategies::printStrategy(selected_strategy);
                        cout << " - New energy: " << E_next << " - Method succeeded " << copy_obtuse_triangles_after << endl;

                        // --------------------------------------------------------- energy 
                        bool accept_strategy = false;

                        if (E_next < E_current) {
                            accept_strategy = true;
                        } else {
                            float prob = exp(-(E_next-E_current)/T);
                            float dice = 0.01f*(rand()%100);

                            if (dice < prob) {
                                accept_strategy = true;
                            }
                        }

                        if (accept_strategy) {
                            cout << "* Energy: " << E_current << " to " << E_next << " - Strategy selected: ";

                            steiner_stategies::printStrategy(selected_strategy);

                            cout << endl;

                            Point* s = steiner_stategies::generateSteinerPoint(graph_copy, a, b, c, selected_strategy);

                            if (s != nullptr && boundaryPolygon.bounded_side(*s) == CGAL::ON_BOUNDED_SIDE) {
                                if (selected_strategy == steiner_stategies::Strategy::POLYGON) {
                                    cdt.insert(*s);
                                } else {
                                    cdt.insert_no_flip(*s);
                                }
                                steinerPoints.emplace_back(*s);
                            } else {
                                // cout << "Steiner point ignored  - outside the boundaries " << endl;
                            }

                            delete s;
                        } else {
                            cout << "* Energy: " << E_current << " to " << E_next << " - Strategy rejetced. " << endl;

                            if (s != nullptr) {
                                delete s;
                            }
                        }
                    } else {
                        cout << "\t";
                        steiner_stategies::printStrategy(selected_strategy);
                        cout << " - New energy: " << E_next << " - Method failed    " << endl;
                    }
                }
            }

            obtuse_triangles_after = utils::countObtuseTriangles(cdt);

            cout << " ### Temperature: " << T << " - Initial: " << obtuse_triangles_initial << ", before: " << obtuse_triangles_before << ", after: " << obtuse_triangles_before << endl;
            // if (obtuse_triangles_after >= obtuse_triangles_before || conflicts == 0 || obtuse_triangles_after == 0) {
            //     break;
            // }

            if (obtuse_triangles_after == 0) {
                break;
            }

            T = T - 1.0f / MAX_ITERATIONS;

            i++;
        }

        cout << "***********************************************************************" << endl;
        cout << " - Initial obtuse triangles: " << obtuse_triangles_initial << endl;
        cout << " - Total obtuse triangles  : " << obtuse_triangles_after << endl;
        cout << " - Total steiner points    : " << steinerPoints.size() << endl;
        cout << " - Energy - Initial        : " << alpha*obtuse_triangles_initial << endl;
        cout << " - Energy - Final          : " << alpha*obtuse_triangles_after + beta*steinerPoints.size() << endl;
        cout << " - Alpha                   : " << alpha << endl;
        cout << " - Beta                    : " << beta << endl;
        cout << "***********************************************************************" << endl;

        return steinerPoints;
    }
};
