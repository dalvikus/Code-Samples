#!/usr/local/bin/python

import sys
import os
import magic
import bson
import re
reHW = re.compile('^.*,\s*(\d+)+\s*x\s*(\d+).*$')
da = []
for file in os.listdir('.'):
    d = dict()
    d['name'] = file
    d['type'] = magic.from_file('./%s' % file, mime = True)
    print d['type']
    s = magic.from_file('./%s' % file)
    print s
    m = reHW.match(s)
    if m:
        print 'W: %d, H: %d' % (int(m.group(1)), int(m.group(2)))
    d['size'] = os.stat('./%s' % file).st_size
    frb = open('./%s' % file)
    d['data'] = bson.binary.Binary(frb.read())
    frb.close()
    assert d['size'] == len(d['data'])
#   print d
    da.append(d)

import sys
exit()
if False:
    print da[1]['name']
    fwb = open('../xxx', 'wb')
    fwb.write(da[1]['data'])
    fwb.close()
    exit()
#import pprint
from pymongo import MongoClient
client = MongoClient('localhost', 27017)
#client = MongoClient('mongodb://bb:bb@ds127391.mlab.com:27391/gre')
#print client
db = client.gre
bbb = db.bbb
#bbb.drop()
bbb.insert_many(da)
