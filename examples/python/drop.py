#!/usr/bin/python3

import gi
import json

gi.require_version("Spino", "1.2")
from gi.repository import Spino

db = Spino.Database.new()
col = db.get_collection("test")
col.create_index("name")
col.create_index("species")

print("appending documents")

doc = { "name": "Camel", "species": "Human" }
col.append(json.dumps(doc))

doc = { "name": "Bruce", "species": "Shark" }
col.append(json.dumps(doc))

print("executing command")

db.execute(json.dumps({
    "cmd": "drop", 
    "collection": "test", 
    "query": "{$and:[{name:\"Bruce\"},{species:\"Shark\"}]}", 
    "limit": 1 
    }))

print("execute completed")

cursor = col.find("{}")

while(cursor.has_next()):
    print(cursor.next())
