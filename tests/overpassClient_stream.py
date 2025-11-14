#! /usr/bin/env python3

import requests
import ijson
import json
import os
from pprint import pprint

url = 'https://overpass.private.coffee/api/interpreter'
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

for item in ijson.items(response.raw, 'elements.item'):
    pprint(item)

# parser = ijson.parse(response.raw)
# graphData = dict()
# for event, type, val in parser:
#     if event == "elements.item.bounds":
#         graphData["bounds"]  = dict()
#         for event, type, val in parser:
#             if event == "elements.item.bounds.maxlat":
#                 graphData["bounds"]["maxlat"] = float(val)
#             elif event == "elements.item.bounds.minlat":
#                 graphData["bounds"]["minlat"] = float(val)
#             elif event == "elements.item.bounds.maxlon":
#                 graphData["bounds"]["maxlon"] = float(val)
#             elif event == "elements.item.bounds.minlon":
#                 graphData["bounds"]["minlon"] = float(val)
#             elif event == "elements.item.bounds" and type == "end_map":
#                 break
#     elif event == "elements.item.id":
#         graphData["id"] = int(val)
#     elif event == "elements.item.nodes.item":
#         graphData["nodes"] = list()
#         for event, type, val in parser:
#             if event == "elements.item.nodes.item":
#                 graphData["nodes"].append(val)
#             elif event == "elements.item.nodes" and  type == "end_array":
#                 break
#         mode = "a" if os.path.exists("test_ways.events") else "w"
#         with open("test_ways.events", mode) as f:
#             json.dump(graphData, f, indent=2)










