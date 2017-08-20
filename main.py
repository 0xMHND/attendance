
IN_FILE = "attendance"
OUT_FILE = "temp"
print("Python version to practice")
import sys
import time
import matplotlib.pyplot as plt
import numpy as np

out_sec = []
in_sec = []
dayCnt = 0
try:
    with open(IN_FILE) as f:
        for line in f:
            if line[0] != 'W':
                dayCnt += 1
                line_sp = line.split()
                if len( line_sp ) > 2 :
                    day, inTime, outTime = line_sp
                    inTime_sp = time.strptime(inTime.split()[0],'%H:%M:%S')
                    in_sec.append (inTime_sp.tm_hour*3600 + inTime_sp.tm_min*60 + inTime_sp.tm_sec)
                    outTime_sp = time.strptime(outTime.split()[0],'%H:%M:%S')
                    out_sec.append (outTime_sp.tm_hour*3600 + outTime_sp.tm_min*60 + outTime_sp.tm_sec)
                else:
                    day, inTime = line_sp
                    outTime = None
                    inTime_sp = time.strptime(inTime.split()[0],'%H:%M:%S')
                    in_sec.append (inTime_sp.tm_hour*3600 + inTime_sp.tm_min*60 + inTime_sp.tm_sec)
except IOError:
    print("Cannot open file")
    sys.exit(1)

for i, val in enumerate(in_sec):
    if val == 0:
        in_sec[i] = np.nan

x_axis = range(dayCnt)

print(in_sec)
print(out_sec)
#plt.plot(x_axis, in_sec)
plt.scatter(x_axis, in_sec)
plt.show()
