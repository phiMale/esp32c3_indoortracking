import csv
import time
from blescan_utility import get_rssi_vals
import math

import random

#start values
x1 = 0
y1 = 0
x2 = 0
y2 = 0
x3 = 0
y3 = 0
x4 = 0
y4 = 0

def dBm_to_m():
    p_0 = 0.0000000002 # empirical estimation using BT scanner app
    rssi_vals = get_rssi_vals()
    i = 0
    dist_vals = []
    for rssi in rssi_vals:
        dist = math.sqrt(p_0*1000/(10**(rssi/10))) # conversion from power to distance
        dist_vals[i] = dist
        i += 1
    return dist_vals



    

def pos_calc():
    dist_vals = dBm_to_m()
    r12 = (dist_vals[0[1]]+dist_vals[1[0]])/2
    r13 = (dist_vals[0[2]]+dist_vals[2[0]])/2
    r14 = (dist_vals[0[3]]+dist_vals[3[0]])/2
    r23 = (dist_vals[1[2]]+dist_vals[2[1]])/2
    r24 = (dist_vals[1[3]]+dist_vals[3[1]])/2
    r34 = (dist_vals[2[3]]+dist_vals[3[2]])/2

    x2 = r12
    x3 = (r13**2 + r12**2 - r23**2)/(2*r12)
    y3 = math.sqrt(r13**2-x3**2)
    x4 = (r14**2 + r12**2 - r24**2)/(2*r12)
    y4 = (r24**2 - y2**2 - r34**2 - x3**2 + y3**2 - 2*x4*x3)/(2*(y3-y2))

    coords = ((0, 0), (0, y2), (x3, y3), (x4, y4))

    return coords


fieldnames = ["x1", "y1", "x2", "y2", "x3", "y3", "x4", "y4"]

with open('positiondata.csv', 'w') as csv_file:
    csv_writer = csv.DictWriter(csv_file, fieldnames=fieldnames)
    csv_writer.writeheader()

while True:
    with open('positiondata.csv', 'w') as csv_file:
        csv_writer = csv.DictWriter(csv_file, fieldnames=fieldnames)
        data = {
            "x1": x1,
            "y1": y1,
            "x2": x2,
            "y2": y2,
            "x3": x3,
            "y3": y3,
            "x4": x4,
            "y4": y4
        }

        csv_writer.writerow(data)
        #print((x1, y1), (x2, y2), (x3, y3), (x4, y4))

        # update vals here
        

        
    time.sleep(1)
