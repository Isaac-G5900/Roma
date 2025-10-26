#include <routingEngine/core.hpp>
#include <iostream>
#include <fstream>
#include <other/json.hpp>
using json = nlohmann::json;

int main(){

//    routingEngine::Graph A;

//     size_t node0 = A.addNode(-122.4194, 37.7749);
//     size_t node1 = A.addNode(-122.4094, 37.7849);

//     A.addArc(node0, node1);

//     routingEngine::Graph B = A;

//     std::cout << "Original Graph Count: " << A.nodeCount() << std::endl;
//     std::cout << "Copy Count : " << B.nodeCount() << std::endl;
//     A.printGraph();
//     std::cout << "\n______________________________________________________________________________________\n";
//     B.printGraph();
//      std::cout << "\n______________________________________________________________________________________\n";
//     routingEngine::Graph G;
//     G.printGraph();
//     std::cout << "\n";

//     size_t node2 = A.addNode(-142.4194, 77.7749);
//     size_t node3 = B.addNode(-200.4094, 37.7849);
//     size_t node4 = G.addNode(-204.2938, 51.9825);

//     std::cout << A.addEdge(node2, node4) << std::endl;
//     std::cout << B.addEdge(node3, node2) << std::endl;
//     std::cout << G.addEdge(node4, node2) << std::endl;

    // A.printGraph();
    // std::cout << "\n______________________________________________________________________________________\n";
    // B.printGraph();
    //  std::cout << "\n______________________________________________________________________________________\n";
    // G.printGraph();
    // std::cout << "\n";
    try{
        std::ifstream input_json("test_json.json");
        json j = json::parse(input_json);
        routingEngine::Graph J;
        
        // std::cout << "JSON parsed successfully!" << std::endl;
        // std::cout << "Number of items in JSON: " << j.size() << std::endl;
        
        for(const auto& [key, value] : j.items()){
            // std::cout << "Processing OSM ID: " << key << std::endl;
            // std::cout << "Name: " << value[0] << std::endl;
            // std::cout << "Lat: " << value[1][0] << ", Lon: " << value[1][1] << std::endl;
            
            J.addNode(value[1][0], value[1][1], std::stol(key), value[0]);
           // std::cout << "Added node, total count: " << J.nodeCount() << std::endl;
        }
        
        std::cout << "\nFinal graph:" << std::endl;
        J.printGraph();
    }catch(const json::parse_error &e){
        std::cerr << "Failed to parse the input Json with" << std::endl
        << "message: " << e.what() << std::endl << "exception id: " << e.id <<
        "byte position of error: " << e.byte << std::endl;
    }




    return 0;
}