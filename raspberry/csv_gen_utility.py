import csv
import time
import blescan_utility as ble
import math
import dummy_blescan_utility as dummy


def dBm_to_m():
    p_0 = 0.0000000002 # empirical estimation using BT scanner app
    rssi_vals = []
    rssi_vals = dummy.get_rssi_vals()
    dist_vals = []
    dist_tpl = []
    for rssi_tpl in rssi_vals:
        for rssi in rssi_tpl:

            dist = int(math.sqrt(p_0*1000/(10**(rssi/10)))) # conversion from power to distance
            dist_tpl.append(dist)
        dist_vals.append(dist_tpl)
    return dist_vals

def get_coords():
    dist_vals = dBm_to_m()
    r12 = (dist_vals[0][1] + dist_vals[1][0])/2
    r13 = (dist_vals[0][2] + dist_vals[2][0])/2
    r14 = (dist_vals[0][3] + dist_vals[3][0])/2
    r23 = (dist_vals[1][2] + dist_vals[2][1])/2
    r24 = (dist_vals[1][3] + dist_vals[3][1])/2
    r34 = (dist_vals[2][3] + dist_vals[3][2])/2

    x1 = 0
    y1 = 0
    x2 = r12
    y2 = 0
    x3 = (r13**2 + r12**2 - r23**2)/(2*r12)
    y3 = math.sqrt(r13**2-x3**2)
    x4 = (r14**2 + r12**2 - r24**2)/(2*r12)
    y4 = (r24**2 - y2**2 - r34**2 - x3**2 + y3**2 - 2*x4*x3)/(2*(y3-y2))

    coords = ((x1, y1), (x2, y2), (x3, y3), (x4, y4))

    return coords

# for i in range(10):
#     print(get_coords())
#     time.sleep(1)