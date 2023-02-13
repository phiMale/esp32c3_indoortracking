from bluepy.btle import Scanner, DefaultDelegate
from time import sleep
import struct

class ScanDelegate(DefaultDelegate):
    def __init__(self):
        DefaultDelegate.__init__(self) # alternatively: super().__init__(self)

    # def handleDiscovery(self, dev, isNewDev, isNewData):
    #     if isNewDev:
    #         print("Discovered device".format(dev.addr))
    #     elif isNewData:
    #         print("Received new data from".format(dev.addr))

# scanner = Scanner().withDelegate(ScanDelegate()) #scanner (objekt) hat funktion withDelegate mit übergabewert return von scandelgeate
# devices = scanner.scan(10.0)

# for dev in devices:  # dev wird hier instanziert 
#     # print("Device {}, RSSI= {} dB".format(dev.addr, dev.rssi))
#     for (adtype, desc, value) in dev.getScanData(): # for touple in dev.getScanData; needs all 3 elements, if not specified use "_"
#         print("{} = {}".format(desc, value))
#         if desc == "Complete Local Name":
            
#             print(value)

            
espnames = ["ESP11", "ESP22", "ES33", "ESP44"]
espfound = False

espdata = [[], [], [], []]


# to pick up where I left off:
# it generally works to print the data, but the var "espnum" bounces around a bit.
# maybe write a switch or if tower that actually asks for the names and assigns 
# the var a fixed value, e.g. if name is "esp11" -> espnum = 0

# problem: ESP44 won't flash anymore. big bad.

def which_esp(name):
    switch={
        "ESP11": 0,
        "ESP22": 1,
        "ESP33": 2,
        "ESP44": 3
    }
    return switch.get(name, None)

def xstr2ints(xstring, n=3):
    results = ()
    for i in range(n):
        byte = int(xstring[2*i:2*i+2], 16)
        if byte == 0:
            byte = 128
        byte = (byte - 127)* -1
        results += (byte,) # "x:y" -> from x to y-1; "x:" -> from x to end
    return results

scanner = Scanner().withDelegate(ScanDelegate())
#scanner.start()
# scanner.start(passive=False)
# scanner.process()

def get_rssi_vals():
    while True: 
        scanner.clear()
        scanner.start()

        devices = scanner.scan(0.1, passive=True) 

        for dev in devices:
            for (type, desc, value) in dev.getScanData():
                #print(value)
                global espfound
                if espfound == False:
                    if desc == "Complete Local Name":
                        #print(value)
                        espnum = which_esp(value)
                        if espnum != None:
                            espfound = True
                            #rpi_rssi = []
                        
                        

                else:
                    if desc == "16b Service Data": # "16b Service Data" "0x12"
                        #print("{} --> {}".format(espnum, xstr2ints(value)))
                        espdata[espnum] = xstr2ints(value)
                        espfound = False
                        return espdata

for i in range(100):
    print(get_rssi_vals())
    sleep(5)

                                


