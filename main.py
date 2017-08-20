
IN_FILE = "attendance"
OUT_FILE = "temp"
print("Python version to practice")
import sys
import time
import matplotlib.pyplot as plt
from matplotlib.dates import DateFormatter
import datetime
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
                    outTime = time.localtime()
                    inTime_sp = time.strptime(inTime.split()[0],'%H:%M:%S')
                    in_sec.append (inTime_sp.tm_hour*3600 + inTime_sp.tm_min*60 + inTime_sp.tm_sec)
                    out_sec.append (outTime_sp.tm_hour*3600 + outTime_sp.tm_min*60 + outTime_sp.tm_sec)

except IOError:
    print("Cannot open file")
    sys.exit(1)



#in_time = [datetime.timedelta(seconds=sec) for sec in in_sec]  
#ax = plt.subplot()
#ax = plt.gca()
#ax.set_yticks(in_sec)
#ax.yaxis.set_major_formatter(DateFormatter('%H:%M'))

''' to avoid 0 values in the plot '''
x_axis_days = [datetime.datetime(2017,7,23) + datetime.timedelta(days=day) for day in np.arange(float(dayCnt))]  
new_cnt = 0
new_in_sec = []
new_out_sec = []
new_x_axis_days = []
for i, val in enumerate(in_sec):
    if val != 0:
        new_in_sec.append(in_sec[i])
        new_x_axis_days.append(x_axis_days[i])
        new_cnt += 1
for i, val in enumerate(out_sec):
    if val != 0:
        new_out_sec.append(out_sec[i])

now_time = datetime.datetime.now()
new_out_sec.append (now_time.hour*3600 + now_time.minute*60 + now_time.second)
print(len(new_in_sec),len(new_out_sec))
in_time = [ datetime.datetime(2000,1,1) + datetime.timedelta(seconds=sec) for sec in new_in_sec]  
out_time = [ datetime.datetime(2000,1,1) + datetime.timedelta(seconds=sec) for sec in new_out_sec]  

#for i in np.arange(new_cnt):
#    in_time[i] += new_x_axis_days[i] 

plt.grid(True, which='both')
plt.grid(b=True, which='major', color='k', linestyle='-')
plt.grid(b=True, which='minor', color='k', linestyle='-', linewidth=0.1)
plt.xticks(rotation='horizontal',fontsize='small')
plt.minorticks_on()
plt.margins(0.01)

plt.plot_date(x=new_x_axis_days, y=in_time, color="red", linestyle="--", markersize=4)
plt.plot_date(x=new_x_axis_days, y=out_time, color="blue", linestyle="--", markersize=4)
plt.gca().yaxis.set_major_formatter(DateFormatter('%H:%M'))
plt.gca().xaxis.set_major_formatter(DateFormatter('%m-%d'))
plt.ylabel("Time")
plt.tight_layout()

plt.show()
