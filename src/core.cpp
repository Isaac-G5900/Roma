#include <routingEngine/core.hpp>
#include <iostream>
#include <string>
namespace routingEngine{
    //maybe have this return an edge object at somepoint?
    // double Coordinate::computeDistance(const Coordinate& origin, const Coordinate& destination)

    //Graph impl

    Graph::Graph() : nodes () {}

    Graph::Graph(const Graph& other) : nodes(){

        nodes.reserve(other.nodes.size());

        for(const auto& node : other.nodes){
            this->id_to_indexMap[node->id] = addNode(node->coords->latitude, node->coords->longitude, node->id, node->coords->name);
        }

        for(const auto& node : other.nodes){
            for(const auto& neighbor : node->neighborList){
                addEdge(node->id, neighbor->id);
            }
        }
    }

    Graph& Graph::operator=(const Graph& other){
        if(this != &other){
            Graph temp(other);
            *this = std::move(temp);
        }
        return *this;
    }

    Graph::Graph(Graph&& other) noexcept : nodes(std::move(other.nodes)), id_to_indexMap(std::move(other.id_to_indexMap)) {
    }

    Graph& Graph::operator=(Graph&& other) noexcept{
        if(this != &other){
            this->nodes = std::move(other.nodes);
            this->id_to_indexMap = std::move(other.id_to_indexMap);
        }

        return *this;
    }


    //Active operations

    //for utility purposes, successful operation returns the index of the added node.
    size_t Graph::addNode(double latitude, double longitude, size_t id, const std::string& name=""){
        size_t index = this->nodes.size(); //starts as 0 if the first newNode added to Graph
        auto newNode = std::make_unique<GraphNode>(latitude, longitude, id, std::vector<GraphNode*>{}, index);
        if(name != ""){newNode->coords->name = name;}
        nodes.push_back(std::move(newNode));
        id_to_indexMap[id] = index;
        return index;
    };

    bool Graph::addEdge(size_t origin_id, size_t destination_id){
        if(origin_id == destination_id || !hasNode(origin_id)|| !hasNode(destination_id)){
            return false;
        }
        size_t origin_index = id_to_indexMap[origin_id];
        size_t destination_index = id_to_indexMap[destination_id];
        nodes[origin_index]->neighborList.push_back(nodes[destination_index].get());
        return true;
    }

    bool Graph::addArc(size_t first_id, size_t second_id){
        return Graph::addEdge(first_id, second_id) && Graph::addEdge(second_id, first_id);
    }

    // GraphNode* Graph::findNode(size_t node_id){
    //     if(!(hasNode(node_id))){throw std::out_of_range("Invalid node ID given");}
    //     return nodes[node_id].get();
    // };

    // void Graph::removeNode(size_t node_id);

    // void Graph::removeEdge(size_t node_id, size_t other_id);


    //A* search

    // std::vector<size_t> Graph::findPath(size_t start_id, size_t destination_id);

    //getters

    bool Graph::hasNode(size_t node_id) const{
        return id_to_indexMap.find(node_id) != id_to_indexMap.end();
    }

    // bool Graph::hasEdge(size_t node_id, size_t other_id) const{

    // };
    
    size_t Graph::nodeCount() const{
        return nodes.size();
    };

    bool Graph::empty() const{
        return nodes.empty();
    };

    // std::vector<size_t> Graph::getneighborOf(size_t node_id) const{

    // }; 

    // double Graph::getDistance(size_t node_id, size_t other_id);

    std::string Graph::GraphNode::printNode(){
        std::string neighborListStr;
        // if(this->neighborList.size() == 0){
        //     neighborListStr = "[]";
        // }
        //else{
        neighborListStr = "[";
        for(const auto& neighbor : this->neighborList){
            neighborListStr += std::to_string(neighbor->id)+", ";
        }
        //}
        //take off the last comma and space before closing
        // neighborListStr.pop_back();
        // neighborListStr.pop_back();
        neighborListStr += "]";
        std::string nodeStr = "[id: "+std::to_string(this->id)+",\n" + "name: " + this->coords->name + ",\n" + "coords: "
        + "(lon: " + std::to_string(this->coords->longitude) + ", "
        + "lat: " + std::to_string(this->coords->latitude) + ")" + ",\n" + "neighbors: " + neighborListStr + "]";
        return nodeStr;
    }
    void Graph::printGraph(){
        if(nodes.size() == 0){
            std::cout << "[]";
            return;
        }
        std::string nodeList = "[";
        for(int i = 0; i < nodes.size(); ++i){
            nodeList += nodes[i]->printNode() +",\n";
        }
        nodeList.pop_back();
        nodeList.pop_back();
        std::cout << nodeList + "]";
    }

}

