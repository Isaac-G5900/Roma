# Roma Routing Engine - Development Notes

## Project Overview
Building a routing engine using Overpass API data to create a graph of Points of Interest (POIs) connected by road networks for pathfinding algorithms.

## Current Implementation Status

### Graph Structure (`core.cpp`)
- **Graph class** with `std::vector<std::unique_ptr<GraphNode>>` for node storage
- **O(1) node lookup** via `std::unordered_map<size_t, size_t> id_to_indexMap`
- **Edge storage** via `std::unordered_map<std::pair<size_t, size_t>, Edge>` for O(1) edge lookups
- **GraphNode** contains:
  - `id`, `coords` (lat/lon/name), `index`
  - `std::vector<GraphNode*> neighborList` (raw pointers to neighbors)
- **Memory management**: `unique_ptr` owns nodes, raw pointers in neighbor lists are safe (validated with Valgrind - no leaks)
- **Copy/move semantics** implemented for Graph class
- **Edge struct** includes default constructor for STL container compatibility

### Implemented Methods
- `addNode()` - Returns index of added node, handles optional name parameter
- `addEdge()` - Creates directed edge, validates node existence and prevents self-loops
- `addArc()` - Creates bidirectional edge using two `addEdge()` calls
- `hasNode()` - O(1) lookup using `id_to_indexMap`
- `nodeCount()`, `empty()` - Basic graph properties
- `printNode()`, `printGraph()` - Debug output (formatting needs cleanup)

### Missing/Commented Methods
- `findPath()` (A* search)
- `getneighborOf()`, `hasEdge()`, `getDistance()`
- `removeNode()`, `removeEdge()`
- `findNode()` (commented out)

### Copy/Move Constructor Details
- **Copy constructor**: First adds all nodes, then recreates all edges using `addEdge()`
- **Move constructor**: Transfers ownership of `nodes`, `id_to_indexMap`, and `edges`
- **Assignment operators**: Use copy-and-swap idiom for exception safety

## Overpass API Data Structure

### Query Components
```
[bbox:lat_min,lon_min,lat_max,lon_max]
(
    node[amenity];           // POI nodes (restaurants, shops, etc.)
    way[highway=primary];    // Road segments
    node(w);                 // Nodes that make up the ways
);
out geom;
```

### Data Format
**Nodes**: 
- `type: "node"`, `id`, `lat`, `lon`
- `tags: {"amenity": "restaurant"}` (for POIs)

**Ways**:
- `type: "way"`, `id`
- `nodes: [id1, id2, id3]` - sequence of node IDs forming the road
- `geometry: [{lat, lon}, ...]` - coordinates for each node
- `bounds: {minlat, minlon, maxlat, maxlon}` - bounding box
- `tags: {"highway": "primary"}` - road type info

## Edge Construction Strategy

### 1. Way-to-Way Connections
- **Intersection Detection**: When two ways share the same node ID in their `nodes` arrays
- **Example**: Way A `[100, 101, 102]` + Way B `[102, 103, 104]` → intersection at node `102`

### 2. POI-to-Road Connections
- **Distance-based**: Find nearest road node to each POI using haversine distance
- **Threshold**: Connect POIs within ~50m of road nodes
- **Bidirectional**: Create edges both ways (POI ↔ road node)

### 3. Direct Way Traversal
- **Sequential edges**: For way with nodes `[A, B, C]`, create edges A→B and B→C
- **One-way handling**: Check `tags.oneway` for directional restrictions
- **Compound paths**: POI₁ → Way A → intersection → Way B → POI₂

## Planned Improvements

### HTTP Streaming (Large Queries) - IMPLEMENTED
- **Problem**: `overpy` loads entire result into memory
- **Solution**: Use `requests` + `ijson` for incremental JSON parsing
- **Benefits**: Handle city-wide queries without memory issues
- **Status**: Basic streaming client implemented in `tests/overpassClient_stream.py`

### A* Pathfinding - PLANNED
- **Heuristic**: Haversine distance to destination
- **Edge weights**: Real distances between consecutive nodes
- **Return**: Sequence of node IDs forming optimal path
- **Implementation**: Will be in `routingEngine` namespace utilities, separate from graph class

## Technical Considerations

### Memory Management - VALIDATED
- ✅ `unique_ptr` ownership model works for static road networks
- ✅ Raw pointers in neighbor lists safe (no node removal planned)
- ✅ **Valgrind tested**: No memory leaks, 0 errors, clean allocation/deallocation
- ✅ **Edge storage**: Upgraded to `std::unordered_map<std::pair<size_t, size_t>, Edge>` for O(1) lookups
- ✅ **Custom hash**: Implemented for `std::pair<size_t, size_t>` to ensure compiler compatibility

### Current Edge Implementation - COMPLETED
```cpp
struct Edge {
    size_t origin_id;
    size_t destination_id;
    double distance;
    std::string hierarchy;  // Road type metadata
    std::string name;       // Way name
    
    Edge();  // Default constructor for STL containers
    Edge(size_t origin_id, size_t destination_id, double distance, 
         std::string hierarchy="", std::string name="");
    
    static double getDistance(const Coordinate& origin, const Coordinate& destination);
};
```

**Key Features:**
- Default constructor enables use in STL containers
- Static Haversine distance calculation method
- Supports road hierarchy and name metadata
- Stored in `std::unordered_map<std::pair<size_t, size_t>, Edge>` for O(1) access

### Query Optimization
- **Current**: Small bounding box (~4km × 6km) - manageable
- **Future**: Geographic chunking or HTTP streaming for larger areas
- **API limits**: Overpass has timeout/size limits

### Data Processing Order
1. Process POI nodes → add to graph
2. Process ways → extract road network nodes
3. Create edges along ways (consecutive nodes)
4. Connect ways at intersections (shared node IDs)
5. Connect POIs to nearest road nodes

## Recent Design Analysis and Next Steps

### Space and Performance Considerations
- The graph is designed to be lightweight, only loading nodes and edges within a relevant bounding box for each query.
- For city-scale routing, this approach is efficient and keeps memory usage low.
- For larger regions, consider streaming data from Overpass and incremental graph construction to avoid loading everything into memory.
- ✅ **Edge storage**: Uses `std::unordered_map<std::pair<size_t, size_t>, Edge>` for O(1) edge lookups and scalability.

### Edge and Path Construction
- Edges are constructed between consecutive nodes in ways, storing origin/destination IDs and computed distance (Haversine formula).
- Not all OSM ways have names; always check for the "name" tag and handle missing names gracefully.
- For pathfinding, only the minimal graph (nodes, edges, weights) is loaded and used.
- Path abstraction (as a sequence of edges) will be important for routing and can be added when implementing algorithms.

### Pathfinding Strategy
- Pathfinding (A*) operates on the cached subgraph within the bounding box.
- If a query goes outside the cached region, update the cache before running pathfinding.
- For performance, avoid storing unnecessary metadata or geometry unless needed for advanced features.
- Use `neighborList` for fast neighbor access and `edges` map for O(1) edge metadata lookup.

### Next Immediate Steps
1. ✅ **COMPLETED**: Refactor edge storage to use unordered_map for scalability
2. ✅ **COMPLETED**: Memory validation with Valgrind - no leaks detected
3. 🔄 **IN PROGRESS**: Enhance the client script to stream and parse ways data from Overpass
4. **TODO**: Implement POI-to-way connectivity logic using bounding box or proximity checks
5. **TODO**: Design pathfinding utilities in routingEngine namespace (separate from graph class)

### Architectural Note
- The graph loader and data ingestion logic should be implemented as a separate routing engine service, not as part of the core graph class. This separation keeps the graph implementation focused on in-memory structure and algorithms, while the service handles data acquisition, parsing, and region management.
- The routing engine service will be responsible for:
  - Querying and streaming Overpass data
  - Parsing ways, nodes, and POIs
  - Managing bounding box regions and cache updates
  - Invoking the core graph API to build/update the graph for pathfinding
- This modular design improves maintainability, scalability, and testability of the overall system.

## Graph Structure and Edge Storage Update - COMPLETED

- ✅ **IMPLEMENTED**: Edge storage upgraded from `std::vector<Edge>` to `std::unordered_map<std::pair<size_t, size_t>, Edge>`
- ✅ **PERFORMANCE**: O(1) access to edge metadata, simplified edge existence checks, updates, and removals
- ✅ **ARCHITECTURE**: Graph class focuses on representing nodes, neighbor relationships, and edge metadata
- ✅ **SEPARATION**: Search/pathfinding algorithms planned for `routingEngine` namespace utilities

## Separation of Concerns - DESIGNED

- **Graph Loader/Service:** Data ingestion (fetching, parsing, streaming from Overpass) handled by separate service
- **Pathfinding Algorithms:** Search algorithms (A*, Dijkstra, bidirectional search) implemented as utilities in the `routingEngine` namespace
- **Core Graph:** Focused on in-memory structure and basic operations (add/remove nodes/edges, neighbor access)
- This modular design improves maintainability, scalability, and testability.

## POI-to-Way Connectivity Logic - PLANNED

- Connect POIs to ways by checking if POI coordinates fall within the bounding box of a way's geometry, or by proximity threshold
- Create edges to nearby ways, then build routes by traversing intersecting ways
- Use hierarchy filtering (e.g., start with secondary roads) for efficiency
- Bidirectional A* to expand from both origin and destination

## Implementation Status Summary

✅ **COMPLETED**:
- Core graph data structure with smart pointer management
- Edge storage with O(1) lookup performance
- Memory leak validation with Valgrind
- Basic streaming client infrastructure
- Custom hash implementation for edge keys

🔄 **IN PROGRESS**:
- Overpass data streaming and parsing
- POI-to-way connectivity logic

**TODO**:
- Pathfinding algorithm implementation
- Graph loader service
- Performance testing with real-world data

---
