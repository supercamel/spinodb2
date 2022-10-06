#!/usr/bin/python3

import gi
import json

gi.require_version("Spino", "1.2")
from gi.repository import Spino

db = Spino.Database.new()
col = db.get_collection("test")

for i in range(1000000):
    doc = { "name": "Camel", "score": i }
    col.append(json.dumps(doc))

cursor = col.find("{name: \"Camel\"}").set_limit(1)


while(cursor.has_next()):
    print(cursor.next())
