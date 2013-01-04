#!/usr/bin/python

import picosafelogin

# informationen ausgeben

company = picosafelogin.get_company()
owner   = picosafelogin.get_owner()
desc    = picosafelogin.get_description()
id      = picosafelogin.get_id()
time    = picosafelogin.get_time()
serial  = picosafelogin.get_serial()

print """
company:     %s
owner:       %s
description: %s
id:          %d
serial:      %s
time:        %d""" % (company, owner, desc, id, serial, time)
