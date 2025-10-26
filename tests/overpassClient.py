#!/usr/bin/env python3
import overpy
import json

api = overpy.Overpass()

result = api.query("""
[bbox:33.48,-117.18,33.52,-117.12]
[out:json]
[timeout:90];
(
    node[amenity];
    way[highway=primary];
);
out geom 10;
""")

graphData = dict()

for node in result.nodes:
    if "name" in node.tags:
        graphData[node.id] = (node.tags["name"], (float(node.lat),  float(node.lon)))
    elif "short_name" in node.tags:
         graphData[node.id] = (node.tags["short_name"], (float(node.lat),  float(node.lon)))
    elif "operator" in node.tags:
        graphData[node.id] = (node.tags["operator"], (float(node.lat),  float(node.lon)))
    else:
        graphData[node.id] = (node.tags["amenity"], (float(node.lat),  float(node.lon)))
     


with open("test_json.json", "w") as f:
    json.dump(graphData, f, indent=2)





