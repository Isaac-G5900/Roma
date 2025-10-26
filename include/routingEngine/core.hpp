#pragma once
#include <memory>
#include <vector>
#include <unordered_map>
namespace routingEngine{

    struct Coordinate{
        double latitude;
        double longitude;
        std::string name;
        Coordinate(double longitude, double latitude, std::string name="") : latitude(latitude), longitude(longitude), name(name) {}

        // double computeDistance(const Coordinate& origin, const Coordinate& destination);
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

            const Coordinate& getDistanceHelper(const Coordinate& start_coord, const Coordinate& destination_coord);

            std::vector<std::unique_ptr<GraphNode>> nodes;
            std::unordered_map<size_t, size_t> id_to_indexMap;
            
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

            bool hasNode(size_t node_id) const;

            // bool hasEdge(size_t node_id, size_t other_id) const;
            
            size_t nodeCount() const;

            bool empty() const;

            // std::vector<size_t> getneighborOf(size_t node_id) const; 

            // double getDistance(size_t node_id, size_t other_id);

            void printGraph();
    };

}

