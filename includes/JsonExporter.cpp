#include <iostream>
#include <fstream>
#include <string>

#include "JsonExporter.h"

using namespace std;

JsonExporter::JsonExporter(string instance_uid) {
    content_type = "CG_SHOP_2025_Solution";
    this->instance_uid = instance_uid;
}

void JsonExporter::print() const {
    std::cout << "Content Type: " << content_type << std::endl;
    std::cout << "Instance UID: " << instance_uid << std::endl;

    std::cout << "Steiner Points (X): ";
    for (const auto& x : steiner_points_x) {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    std::cout << "Steiner Points (Y): ";
    for (const auto& y : steiner_points_y) {
        std::cout << y << " ";
    }
    std::cout << std::endl;

    std::cout << "Edges: ";
    for (const auto& edge : edges) {
        std::cout << "(" << edge.first << ", " << edge.second << ") ";
    }
    std::cout << std::endl;
}

void JsonExporter::save(const char* outputfile) {
    std::ofstream json_file_out(outputfile);

    for (string temp : steiner_points_x) {
        pt_steiner_points_x.push_back(boost::property_tree::ptree::value_type("", temp));
    }

    for (string temp : steiner_points_y) {
        pt_steiner_points_y.push_back(boost::property_tree::ptree::value_type("", temp));
    }

    for (const auto& edge : edges) {
        boost::property_tree::ptree edge_node;

        std::string x_str = std::to_string(edge.first);
        std::string y_str = std::to_string(edge.second);

        edge_node.push_back(boost::property_tree::ptree::value_type("", x_str));

        edge_node.push_back(boost::property_tree::ptree::value_type("", y_str));

        pt_edges.push_back(boost::property_tree::ptree::value_type("", edge_node));
    }

    pt_root.put("content_type", "CG_SHOP_2025_Solution");
    pt_root.put("instance_uid", this->instance_uid);
    pt_root.add_child("steiner_points_x", pt_steiner_points_x);
    pt_root.add_child("steiner_points_y", pt_steiner_points_y);
    pt_root.add_child("edges", pt_edges);
    
    boost::property_tree::write_json(outputfile, pt_root);
}