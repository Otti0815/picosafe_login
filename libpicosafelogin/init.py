#!/usr/bin/python

from __future__ import division
import picosafelogin
from optparse import OptionParser
from time import time
from sys import exit

# picosafelogin stick initialisieren

parser = OptionParser()
parser.add_option("-c", "--company",     action="store", type="string",  dest="company",  help="Set company string (up to 50 chars)", default="")
parser.add_option("-o", "--owner",       action="store", type="string",  dest="owner",    help="Set owner string (up to 50 chars)",   default="")
parser.add_option("-p", "--password",    action="store", type="string",  dest="password", help="Set password (exactly 42 chars)",     default="")
parser.add_option("-d", "--description", action="store", type="string",  dest="desc",     help="Set description (up to 50 chars)",    default="")
parser.add_option("-l", "--lock",        action="store_true",            dest="lock",     help="Set lock (flag)",                     default=False)
parser.add_option("-i", "--id",          action="store", type="int",     dest="id",       help="Set id (integer)",                    default=0)

(options, args) = parser.parse_args()

company  = options.company
owner    = options.owner
desc     = options.desc
seed     = options.password
lock     = options.lock
id       = options.id

def ok(string, bool):
  print string+"... ",
  if bool:
    print "ok"
  else:
    print "fail"
    exit(1)

ok("set_owner",     picosafelogin.set_owner(owner))
ok("set_desc",      picosafelogin.set_description(desc))
ok("set_id",        picosafelogin.set_id(id))
ok("set_company",   picosafelogin.set_company(company))
ok("set_seed",      picosafelogin.set_seed(seed))
ok("set_timestamp", picosafelogin.set_timestamp(int(time()), seed))
#print
#print seed

if lock:
    ok("set_lock", picosafelogin.set_lock())
