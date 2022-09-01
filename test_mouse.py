#!/usr/bin/env python3
from time import sleep
NULL_CHAR = chr(0)

def write_report(report):
	with open('/dev/hidg0', 'rb+') as fd:
		fd.write(report.encode("ascii"))

# PVID=046D:C092 G102 LIGHTSYNC Gaming Mouse
# {00 00}          {05 00} {00 00} {00}
# btn(bit map)     x       y       wheel

sleep(2)

# move x
write_report(NULL_CHAR*2+chr(10)+NULL_CHAR*4)
sleep(1)
# move y
write_report(NULL_CHAR*4+chr(10)+NULL_CHAR*2)
