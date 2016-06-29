#!/usr/bin/python3

import subprocess
subprocess.Popen(["./processradio.py"]) #Server for GNURadio to connect to
subprocess.Popen(["./top_block.py"]) #Compiled GNURadio Script
