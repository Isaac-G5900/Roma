#! /usr/bin/env python3

import requests
import ijson
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












