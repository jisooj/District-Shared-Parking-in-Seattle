#!/usr/local/bin/python3.5
from app import app
app.debug = True #auto reload on code change + debug messages
app.run(host='0.0.0.0', use_reloader=False)

