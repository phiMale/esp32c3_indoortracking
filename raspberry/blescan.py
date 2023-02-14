# Embedded Systems BLE Indoor Tracking Project
# BLE Scanner Module
# ___________________________

from bluepy.btle import Scanner, DefaultDelegate
from time import sleep


class ScanDelegate(DefaultDelegate):
    def __init__(self):
        DefaultDelegate.__init__(self)

            
espnames = ["ESP11", "ESP22", "ES33"]
espfound = False

rssidata = [[], [], []]
rpi_rssi = [0,0,0]
espnum = 0


def which_esp(name):
    switch={
        "ESP11": 0,
        "ESP22": 1,
        "ESP33": 2,
    }
    return switch.get(name, None)


def xstr2ints(xstring, n=2):
    results = []
    for i in range(n):
        byte = int(xstring[2*i:2*i+2], 16)
        if byte == 0:
            byte = 128
        byte = (byte - 127)* -1
        results.append(byte) 
    return results

scanner = Scanner().withDelegate(ScanDelegate())

def get_rpi_rssi():
    while True:
        scanner.clear()
        scanner.start()

        devices = scanner.scan(0.1, passive=True)

        for dev in devices:
            for (type, desc, value) in dev.getScanData():
                global espfound
                if espfound == False:
                    if desc == "Complete Local Name":
                        espnum = which_esp(value)
                        if espnum != None:
                            espfound = True
                else:
                    rpi_rssi[espnum] = dev.rssi + 10
                    return rpi_rssi

def get_rssi_vals():
    while True: 
        scanner.clear()
        scanner.start()

        devices = scanner.scan(0.1, passive=True) 

        for dev in devices:
            for (type, desc, value) in dev.getScanData():
                global espfound
                global espnum
                if espfound == False:
                    if desc == "Complete Local Name":
                        espnum = which_esp(value)
                        if espnum != None:
                            espfound = True
                        
                else:
                    if desc == "16b Service Data": 
                        rssidata[espnum] = xstr2ints(value)
                        espfound = False
                        return rssidata
