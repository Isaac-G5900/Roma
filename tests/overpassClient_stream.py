#! /usr/bin/env python3

import requests
import ijson
import json
import os
from pprint import pprint

url = 'https://overpass-api.de/api/interpreter'
query = """
[bbox:33.48,-117.18,33.52,-117.12]
[out:json]
[timeout:90];
(
    node[way];
    way[highway=secondary];
);
out geom 10;
"""
response = requests.post(url, data={'data' : query}, stream=True, headers={'Accept-Encoding' : 'identity'})

# for item in ijson.items(response.raw, 'elements.item'):
#     pprint(item)

parser = ijson.parse(response.raw)
graphData = dict()
with open("test_ways.events", "w") as f:
    f.write('[\n')
for event, type, val in parser:
    #print(event, type, val)
    if event == "elements.item.bounds":
        graphData.setdefault("bounds", dict())
        for event, type, val in parser:
            if event == "elements.item.bounds.maxlat":
                graphData["bounds"]["maxlat"] = float(val)
            elif event == "elements.item.bounds.minlat":
                graphData["bounds"]["minlat"] = float(val)
            elif event == "elements.item.bounds.maxlon":
                graphData["bounds"]["maxlon"] = float(val)
            elif event == "elements.item.bounds.minlon":
                graphData["bounds"]["minlon"] = float(val)
            elif event == "elements.item.bounds" and type == "end_map":
                break
    elif event == "elements.item.id":
        graphData.setdefault("id", int(val))
    elif event == "elements.item.nodes.item":
        graphData.setdefault("nodes", list()) #only initialize if it does not exist yet
        graphData["nodes"].append(val)
        for event, type, val in parser:
            if event == "elements.item.nodes.item":
                graphData["nodes"].append(val)
            elif event == "elements.item.nodes" and  type == "end_array":
                break
        # mode = "a" if os.path.exists("test_ways.events") else "w"
        # with open("test_ways.events", mode) as f:
        #     json.dump(graphData, f, indent=2)
        #     f.write('[\n' if mode == "w" else ',\n')
        #     graphData = {}
    elif event == "elements.item.geometry":
        graphData.setdefault("geometry", list())
        geoCoords = dict()
        for event, type, val in parser:
            if event == "elements.item.geometry.item.lat":
                geoCoords['lat'] = float(val)
            elif event == "elements.item.geometry.item.lon":
                geoCoords['lon'] = float(val)
            elif event == "elements.item.geometry.item" and type == "end_map":
                graphData["geometry"].append(geoCoords)
                geoCoords = dict()
            elif event == "elements.item.geometry" and type == "end_array":
                break 
        with open("test_ways.events", "a") as f:
            if os.path.getsize("test_ways.events") > 2:
                f.write(',\n')
            json.dump(graphData, f, indent=2)
            graphData = {}

with open("test_ways.events", "a") as f:
    f.write("\n]")




# elements.item map_key geometry
# elements.item.geometry start_array None
# elements.item.geometry.item start_map None
# elements.item.geometry.item map_key lat
# elements.item.geometry.item.lat number 33.5168570
# elements.item.geometry.item map_key lon
# elements.item.geometry.item.lon number -117.1689956
# elements.item.geometry.item end_map None
# elements.item.geometry.item start_map None
# elements.item.geometry.item map_key lat
# elements.item.geometry.item.lat number 33.5172372
# elements.item.geometry.item map_key lon
# elements.item.geometry.item.lon number -117.1687703
# elements.item.geometry.item end_map None
# elements.item.geometry.item start_map None
# elements.item.geometry.item map_key lat
# elements.item.geometry.item.lat number 33.5176073
# elements.item.geometry.item map_key lon
# elements.item.geometry.item.lon number -117.1685394
# elements.item.geometry.item end_map None
# elements.item.geometry.item start_map None
# elements.item.geometry.item map_key lat
# elements.item.geometry.item.lat number 33.5177121
# elements.item.geometry.item map_key lon
# elements.item.geometry.item.lon number -117.1684740
# elements.item.geometry.item end_map None
# elements.item.geometry end_array None




