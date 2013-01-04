#!/usr/bin/python

from __future__ import division
import picosafelogin
from optparse import OptionParser
from sys import exit
from random import random

parser = OptionParser()
parser.add_option("-n", "--tries", action="store", type="int",    dest="tries", help="number of tries", default=1000)

(options, args) = parser.parse_args()

tries = options.tries

for i in xrange(tries):
  id_1 = int(random()*2**31)
  picosafelogin.set_id(id_1)
  id_2 = picosafelogin.get_id()

  if id_1 != id_2:
    print "%d != %d (diff: %d)" % (id_1, id_2, id_1-id_2)
    exit(1)
  
  if (i % 10) == 0:
    print "% 2.2f%%" % (i/tries*100)
