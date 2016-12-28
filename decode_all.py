#!/usr/bin/python

import os
from os import listdir
from os.path import isfile, join

raw_dir = "./cmds_raw"
decoded_dir = "./cmds"

if not os.path.exists(decoded_dir):
    os.makedirs(decoded_dir)
    
onlyfiles = [f for f in listdir(raw_dir) if isfile(join(raw_dir, f))]

for filename in onlyfiles:
    os.system("./decode "+raw_dir+"/"+filename+" "+decoded_dir+"/"+filename)
