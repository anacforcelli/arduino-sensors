import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation as anim
import numpy as np

import serial

ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)
samples = 256
history = 100

def getReadings():
    readings = []
    line = ser.readline()
    while(line.isupper() != True): # start of packet is upper 
        line = ser.readline()
        print(line)
    for i in range(128):
        line = ser.readline()
        readings.append(float(line))
    return readings

record = []
for i in range(history):
    record.append(np.zeros(128))

plt.ion() #interactive mode

img = plt.imshow(record)

while(True):
    record.append(getReadings())
    record = record[1:]
    img.set_data(record)
    img.autoscale()
    plt.pause(0.01)