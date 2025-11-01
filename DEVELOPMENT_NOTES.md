# Roma Routing Engine - Development Notes

## Project Overview
Building a routing engine using Overpass API data to create a graph of Points of Interest (POIs) connected by road networks for pathfinding algorithms.

## Current Implementation Status

### Graph Structure (`core.cpp`)
- **Graph class** with `std::vector<std::unique_ptr<GraphNode>>` for node storage
- **O(1) node lookup** via `std::unordered_map<size_t, size_t> id_to_indexMap`
- **GraphNode** contains:
  - `id`, `coords` (lat/lon/name), `index`
  - `std::vector<GraphNode*> neighborList` (raw pointers to neighbors)
- **Memory management**: `unique_ptr` owns nodes, raw pointers in neighbor lists are safe (no node removal planned)
- **Copy/move semantics** implemented for Graph class

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
- **Copy constructor**: First adds all nodes, then recreates all edges
- **Move constructor**: Transfers ownership of `nodes` and `id_to_indexMap`
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

### Edge Struct
```cpp
struct Edge {
    size_t from_id;
    size_t to_id;
    double weight;          // distance/time
    std::string road_name;  // optional metadata
};
```

### HTTP Streaming (Large Queries)
- **Problem**: `overpy` loads entire result into memory
- **Solution**: Use `requests` + `ijson` for incremental JSON parsing
- **Benefits**: Handle city-wide queries without memory issues

### A* Pathfinding
- **Heuristic**: Haversine distance to destination
- **Edge weights**: Real distances between consecutive nodes
- **Return**: Sequence of node IDs forming optimal path

## Technical Considerations

### Memory Management
- ✅ `unique_ptr` ownership model works for static road networks
- ✅ Raw pointers in neighbor lists safe (no node removal)
- ⚠️ Potential dangling pointers if node removal implemented later

### Dangling Reference Risk Solutions

#### Option 1: Node IDs (Safest, Medium Refactor)
```cpp
std::vector<size_t> neighborList;  // Store IDs instead of pointers
```
**Pros**: 
- Zero dangling pointer risk
- Works with node removal
- Memory efficient

**Cons**: 
- Need to refactor `addEdge()`, copy constructor, `printNode()`
- Extra lookup required: `nodes[id_to_indexMap[neighbor_id]]`

#### Option 2: `std::weak_ptr` (Safe, Major Refactor)
```cpp
std::vector<std::weak_ptr<GraphNode>> neighborList;
```
**Pros**: 
- No dangling pointers (`expired()` check)
- Pointer semantics preserved

**Cons**: 
- Need to change `nodes` to `std::vector<std::shared_ptr<GraphNode>>`
- Major refactor required
- More memory overhead

#### Option 3: Keep Current + Safety Checks (Minimal Change)
```cpp
// Add validation methods
bool isValidNeighbor(GraphNode* neighbor) const {
    return id_to_indexMap.find(neighbor->id) != id_to_indexMap.end();
}
```
**Pros**: 
- Minimal code changes
- Fast access

**Cons**: 
- Still unsafe if you implement node removal
- Runtime checks needed

**Recommendation**: Use Node IDs (Option 1) for long-term robustness.

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
- Edge storage is currently a vector, but for fast lookups and scalability, an unordered_map keyed by node pairs or way IDs may be used in the future.

### Edge and Path Construction
- Edges are constructed between consecutive nodes in ways, storing origin/destination IDs and computed distance (Haversine formula).
- Not all OSM ways have names; always check for the "name" tag and handle missing names gracefully.
- For pathfinding, only the minimal graph (nodes, edges, weights) is loaded and used.
- Path abstraction (as a sequence of edges) will be important for routing and can be added when implementing algorithms.

### Pathfinding Strategy
- Pathfinding (A*) operates on the cached subgraph within the bounding box.
- If a query goes outside the cached region, update the cache before running pathfinding.
- For performance, avoid storing unnecessary metadata or geometry unless needed for advanced features.
- Use adjacency lists for neighbor lookups and consider unordered_map for edge storage if needed.

### Next Immediate Steps
1. Enhance the client script to stream and parse ways data from Overpass, keeping memory usage minimal.
2. Update the graph loader to build edges from ways and only load relevant data for the current query region.
3. Prepare for pathfinding by designing a minimal API for running A* on the loaded subgraph and handling POIs/intersections.

### Architectural Note
- The graph loader and data ingestion logic should be implemented as a separate routing engine service, not as part of the core graph class. This separation keeps the graph implementation focused on in-memory structure and algorithms, while the service handles data acquisition, parsing, and region management.
- The routing engine service will be responsible for:
  - Querying and streaming Overpass data
  - Parsing ways, nodes, and POIs
  - Managing bounding box regions and cache updates
  - Invoking the core graph API to build/update the graph for pathfinding
- This modular design improves maintainability, scalability, and testability of the overall system.

## Graph Structure and Edge Storage Update

- The core graph data structure is solid for in-memory routing and connectivity.
- For scalability and fast edge lookups, switch from `std::vector<Edge>` to `std::unordered_map<EdgeKey, Edge>`, where `EdgeKey` is a struct of origin/destination node IDs.
- This allows O(1) access to edge metadata and simplifies edge existence checks, updates, and removals.
- The graph class should focus on representing nodes, neighbor relationships, and edge metadata, but not on search/pathfinding algorithms or data loading.

## Separation of Concerns

- **Graph Loader/Service:** Data ingestion (fetching, parsing, streaming from Overpass) should be handled by a separate loader or routing engine service, not by the graph class itself.
- **Pathfinding Algorithms:** Search algorithms (A*, Dijkstra, bidirectional search) should be implemented as utilities in the `routingEngine` namespace, operating on the graph but not embedded within it.
- This modular design keeps the graph class lean, focused on structure, and improves maintainability and testability.

## POI-to-Way Connectivity Logic

- When building the graph, connect POIs to ways by checking if the POI coordinates fall within the bounding box of a way's geometry, or by proximity threshold.
- For each POI, create edges to nearby ways, then build routes by traversing intersecting ways, always moving closer to the destination.
- Use hierarchy filtering (e.g., start with secondary roads) to keep the graph relevant and efficient for the query region.

## Next Steps Recap

1. Refactor edge storage in the graph class to use an unordered_map for scalability.
2. Keep search/pathfinding logic outside the graph class, as utilities in the routing engine namespace.
3. Enhance the loader/service to stream and parse ways and POIs, then populate the graph.
4. Implement POI-to-way connectivity logic using bounding box or proximity checks.
5. Prepare for bidirectional A* pathfinding using the updated graph structure.

---
