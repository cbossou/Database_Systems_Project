#!/bin/python

# This script takes in a filename (excluding extension) and then uses the pilot generated results to plot info about it

# got help from: https://stackoverflow.com/questions/13545388/plot-data-from-csv-file-with-matplotlib

import sys
import os
import numpy as np
import matplotlib as mp
import matplotlib.pyplot as plt

argv = sys.argv;

if len(argv) < 2:
	print "Usage: <file>"
	exit(1)

if not os.path.isfile(argv[1]):
	print "File %s doesn't exist" % (argv[1])
	exit(1)

#print "Couldn't plot %s because system is not yet ready" % (argv[1])
#exit(1)

data = np.genfromtxt(argv[1], delimiter=',', skip_header=1,\
		names=['size', 'mean', 'variance'])

print "here it is:", data['size'], data['mean'], data['variance']

#int_data = {'size': int(data['size']), 'mean': int(data['mean']), \
#		'variance': int(data['variance'])}

plt.figure()
plt.errorbar(data['size'], data['mean'], yerr=data['variance'])
# assumes something like seq_hit_read_<stuff>.csv and would get you just seq_hit_read
#fig.title("_".join(argv[1].split(".")[0].split("_")[2:0]))

#ax = plt.add_subplot(111)
ax = plt.gca()
# assumes something like seq_hit_read_<stuff>.csv and would get you just seq_hit_read
ax.set_title("_".join(argv[1].split(".")[0].split("_")[2:0]))
ax.set_xlabel('size [blocks]')
ax.set_ylabel('average time for $size operations')

ax.plot()

plt.savefig(argv[1].split(".")[0] + ".png")
