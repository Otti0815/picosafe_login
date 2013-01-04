#!/usr/bin/python

from __future__ import division
import picosafelogin
from optparse import OptionParser
from sys import exit
from random import random
from time import sleep

parser = OptionParser()
parser.add_option("-n", "--tries", action="store", type="int",    dest="tries", help="number of tries", default=1000)
parser.add_option("-s", "--seed",  action="store", type="string", dest="seed",  help="Set password (exactly 42 chars)",     default="")

(options, args) = parser.parse_args()

tries = options.tries
seed  = options.seed

y2010 = 1293840000
y2030 = 1893456000

for i in xrange(tries):
  timestamp_1 = y2010+int(random()*(y2030-y2010))
  picosafelogin.set_timestamp(timestamp_1, seed)
  timestamp_2 = picosafelogin.get_time()

  if timestamp_1 != timestamp_2:
    print "%d != %d (diff: %d)" % (timestamp_1, timestamp_2, timestamp_1-timestamp_2)
  
  if (i % 10) == 0:
    print "% 2.2f%%" % (i/tries*100)
