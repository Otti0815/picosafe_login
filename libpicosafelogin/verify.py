#!/usr/bin/python

from __future__ import division
import picosafelogin
from optparse import OptionParser
import sys
from time import time

# start
parser = OptionParser()
parser.add_option("-s", "--seed",     action="store", type="string", dest="seed",     help="seed to be used",    default="")

(options, args) = parser.parse_args()

seed     = options.seed

try:
  while True:
    line = sys.stdin.readline()
    pad  = line[0:-1]
    print picosafelogin.verify_token(seed, pad)
except KeyboardInterrupt:
  pass
