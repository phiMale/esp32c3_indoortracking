# Embedded Systems BLE Indoor Tracking Project
# Position Calculation Module
# ___________________________

from math import sqrt
import blescan as ble

class sig_scenario:
    # pathloss0 = tx_pwr_0 - rssi_0 = 9 - (-70) = 79
    # tx_pwr_0 = transmitter pwr level at 100cm distance = 9dBm
    # rssi_0 = receiver pwr level at 100cm distance ~= -70dBm
    def __init__(self, pathloss0=79, pathloss_exp=6, tx_pwr=9):
        self.pathloss0 = pathloss0
        self.pathloss_exp = pathloss_exp
        self.tx_pwr = tx_pwr
        
    def _dBm_to_cm(self, rssi):
        m = 10**((self.tx_pwr - rssi - self.pathloss0)/(10 * self.pathloss_exp))
        cm = m * 100
        return cm

    def get_coords(self, rssi_vals):
        dist_vals = []
        dist_lst = []
        for rssi_lst in rssi_vals:
            for rssi in rssi_lst:
                d = self._dBm_to_cm(rssi)
                dist_lst.append(d)
            dist_vals.append(dist_lst)
        
        r12 = (dist_vals[0][1] + dist_vals[1][0])/2
        r13 = (dist_vals[0][2] + dist_vals[2][0])/2
        r14 = dist_vals[0][3]
        r23 = (dist_vals[1][2] + dist_vals[2][1])/2
        r24 = dist_vals[1][3]
        r34 = dist_vals[2][3]

        x1 = 0
        y1 = 0
        x2 = r12
        y2 = 0
        x3 = (r13**2 + r12**2 - r23**2)/(2*r12)
        y3 = sqrt(r13**2-x3**2)
        x4 = (r14**2 + r12**2 - r24**2)/(2*r12)
        y4 = (r24**2 - y2**2 - r34**2 - x3**2 + y3**2 - 2*x4*x3)/(2*(y3-y2))

        coords = [[x1, y1], [x2, y2], [x3, y3], [x4, y4]]

        return coords

sce = sig_scenario()

def fetch_pos():
    rssi = ble.get_rssi_vals()
    rpi_rssi = ble.get_rpi_rssi()
    i = 0

    for r in rssi:
        r.append(rpi_rssi[i])
        i+=1

    # prints positions on console for trial & error
    g = sce.get_coords(rssi)
    print(g)
    return g