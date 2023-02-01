#!/bin/python3
import sys
def msb2lsb(s,r):
	if s=='':
		return r
	return msb2lsb(s[0:-2],r+s[-2::])
lll=0x80000000
for line in sys.stdin:
	line=line[0:48].split()
	if len(line)>0 and line[0].startswith('0x'):
		for i in range(4):
			if len(line)>i+1:
				print(msb2lsb(line[i+1],''),end=' ')
		print(end='\n')
