import os

for thread in [1, 2, 4]:
	for pad in range(16):
		cmd = "g++ -o prog prog.cpp -lm -fopenmp -DNUMT=%s -DNUMPAD=%s" % (thread, pad)
		os.system(cmd)
		print "thread: %d\tnumpad: %d" % (thread, pad)
		cmd = 'prog'
		os.system(cmd)
		print

