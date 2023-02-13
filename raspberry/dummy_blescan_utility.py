import math
import random
import time

x1 = 0
y1 = 0
x2 = 400
y2 = 0
x3 = 400
y3 = 400

x4 = 200
y4 = 200

def get_rssi_vals():
    
    global x4
    global y4
    x4 = x4 + random.randint(-100,100)
    y4 = y4 + random.randint(-100,100)
    r12 = x2
    r21 = r12
    r13 = math.sqrt(x3**2 + y3**2)
    r31 = r13
    r14 = math.sqrt(x4**2 + y4**2)
    r41 = r14
    r23 = math.sqrt((x2-x3)**2 + y3**2)
    r32 = r23
    r24 = math.sqrt((x2-x4)**2 + (y2-y4)**2)
    r42 = r24
    r34 = math.sqrt((x3-x4)**2 + (y3-y4)**2)
    r43 = r34
    dist_vals = [[r12, r13, r14], [r21, r23, r24], [r31, r32, r34], [r41, r42, r43]]

    p_0 = 0.0000000002 # empirical estimation using BT scanner app
    #i = 0
    rssi_vals = []
    rssi_tpl = []
    for dist_tpl in dist_vals:
        for dist in dist_tpl:
            rssi = math.log10((p_0*1000/(dist**2))) # conversion from distance to power
            rssi_tpl.append(rssi)
        rssi_vals.append(rssi_tpl)
        #i += 1
    return rssi_vals


# for i in range(10):
#     a = get_rssi_vals()
#     print(x4, y4)
#     print(a)
#     time.sleep(1)

    


