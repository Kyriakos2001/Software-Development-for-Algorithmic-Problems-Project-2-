#pragma once

// boost
#include "boost_definitions.h"

// Macros for CGAL
#include "cgal_definitions.h"

// Standard C++
#include <iostream>
#include <vector>

// Namespaces
using namespace std;

class JsonLoader {
private:
    boost::property_tree::ptree pt;

    string instance_uid;
    string method;
    int num_points;
    int num_constraints;
    vector<int> points_x, points_y, region_boundary;
    vector<std::pair<int, int>> additional_constraints;
    int L;    

    
public:
    void load(const char* inputfile);

    void print();

    vector<Point> getPoints();

    std::vector<std::pair<int, int>> getConstraints() ;

    std::vector<int> getRegionBoundaries();    

    string getInstance() const;

    string getMethod() const;

    int getL() const;

    float alpha, beta, xi, psi, lambda, kappa;

    float getAlpha() const;

    float getBeta() const;

    float getXi() const;

    float getPsi() const;

    float getLambda() const;
    
    float getKappa() const;
};