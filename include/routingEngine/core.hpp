#pragma once
#define _USE_MATH_DEFINES
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <utility>
#include <functional>
#include <math.h>

//custom hash operator overload for origin, dest id pair mapping to edge object
//for edges in graph class
namespace std {
    template <>
    struct hash<std::pair<size_t, size_t>> {
        std::size_t operator()(const std::pair<size_t, size_t>& k) const {
            return std::hash<size_t>()(k.first) ^ (std::hash<size_t>()(k.second) << 1);
        }
    };
}

namespace routingEngine{

    struct Coordinate{
        double latitude;
        double longitude;
        std::string name;
        Coordinate(double longitude, double latitude, std::string name="") : latitude(latitude), longitude(longitude), name(name) {}
    };

    struct Edge{

        size_t origin_id;
        size_t destination_id;
        double distance;
        //size_t mph;
        // double travel_time; <For future use when ETA's need to be taken into account>
        std::string hierarchy; //What is the layer of this particular network? 
                               //e.g. Highways, primary, secondary, etc.
        std::string name;

        //default constructor for edges unordered_map required for resize, insertion, etc.
        //value types are required to be default constructable
        Edge() : origin_id(0), destination_id(0), distance(0.0), hierarchy(""), name("") {}
        Edge(size_t origin_id, size_t destination_id, double distance, std::string hierarchy="", std::string name="") : origin_id(origin_id), destination_id(destination_id), distance(distance), hierarchy(hierarchy), name(name) {}

        static double getDistance(const Coordinate& origin, const Coordinate& destination);

    };

    class Graph{
        private:
            struct GraphNode{
                std::unique_ptr<Coordinate> coords;
                std::vector<GraphNode*> neighborList;
                size_t id;
                size_t index;
                GraphNode(double longitude, double latitude, size_t id, std::vector<GraphNode*> neighborList = {}, size_t index = 0) : index(index), id(id),
                coords(std::make_unique<Coordinate>(longitude, latitude)), neighborList(neighborList) {}
                
                std::string printNode();
            };

            std::vector<std::unique_ptr<GraphNode>> nodes;
            std::unordered_map<size_t, size_t> id_to_indexMap;
            std::unordered_map<std::pair<size_t, size_t>, Edge> edges; //origin_id, dest_id : Edge object that connects them
            
        public:


            //RAII
            Graph();

            Graph(const Graph& other); 

            Graph& operator=(const Graph& other);

            Graph(Graph&& other) noexcept;

            Graph& operator=(Graph&& other) noexcept;

             ~Graph() = default;

            //Active operations
            
            size_t addNode(double latitude, double longitude, size_t id, const std::string& name);

            bool addEdge(size_t origin_id, size_t destination_id);
            
            bool addArc(size_t first_id, size_t second_id);

            // GraphNode* findNode(size_t node_id);

            // void removeNode(size_t node_id);

            // void removeEdge(size_t node_id, size_t other_id);


            //A* search

            // std::vector<size_t> findPath(size_t start_id, size_t destination_id);

            //getters

            inline bool hasNode(size_t node_id) const;

            // bool hasEdge(size_t node_id, size_t other_id) const;
            
            inline size_t nodeCount() const;

            inline bool empty() const;

            // std::vector<size_t> getneighborOf(size_t node_id) const; 

            // double getDistance(size_t node_id, size_t other_id);

            void printGraph();
    };

}

