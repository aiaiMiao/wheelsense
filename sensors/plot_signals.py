# #!/usr/bin/env python2

# plotting code by Pierre Karashchuk

# from pylab import *
import serial
# import numpy as np

s = serial.Serial('/dev/ttyACM0', 115200)

import matplotlib.pyplot as plt
import numpy as np
import time

N_sensors = 3

fig = plt.figure()
ax = fig.add_subplot(111)

data = [[0 for _ in range(100)] for _ in range(N_sensors)]

data[0][0] = 1000

# some X and Y data
x = np.arange(100)
y = data[0]

lis = []

for i in range(N_sensors):
    li, = ax.plot(x, y)
    lis.append(li)

# draw and show it
fig.canvas.draw()
plt.show(block=False)

# loop to update the data
while True:
    try:
        try:
            line = s.readline().strip().split(',')
            print(line)
            xs = [float(l) for l in line]
        except ValueError, IndexError:
            continue

        for i in range(N_sensors):
            data[i].append(xs[i])
            data[i] = data[i][-100:]

            # set the new data
            lis[i].set_ydata(data[i])

        # ax.relim()
        # ax.autoscale_view(True,True,True)
        fig.canvas.draw()

        # time.sleep(0.001)
        plt.pause(0.0001)                       #add this it will be OK.
    except KeyboardInterrupt:
        break






