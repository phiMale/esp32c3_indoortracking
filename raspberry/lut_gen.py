# RSSI <--> distance Lookup Table Generator

from bluepy.btle import Scanner, DefaultDelegate
import csv

class ScanDelegate(DefaultDelegate):
    def __init__(self):
        DefaultDelegate.__init__(self) # alternatively: super().__init__(self)


scanner = Scanner().withDelegate(ScanDelegate()) #scanner (objekt) hat funktion withDelegate mit übergabewert return von scandelgeate


# class device:
#     def __init__(self, name, type):
#         if type == 'ESP_TYPE':
#             return esp(name)
#         if type == 'RPI_TYPE':
#             return rpi(name)

# class esp:
#     def __init__(self, name):
#         self.name = name
#         self.espdata = [[], [], [], []]

#     def which_esp(name):
#         switch={
#             "ESP11": 0,
#             "ESP22": 1,
#             "ESP33": 2,
#             "ESP44": 3
#         }
#         return switch.get(name, None)
    
#     def xstr2ints(xstring, n=3):
#         results = ()
#         for i in range(n):
#             byte = int(xstring[2*i:2*i+2], 16)
#             if byte == 0:
#                 byte = 128
#             byte = (byte - 127)* -1
#             results += (byte,) # "x:y" -> from x to y-1; "x:" -> from x to end
#         return results

#     def data(self):
#         scanner.clear()
#         scanner.start()
#         devices = scanner.scan(0.1, passive=True) 

#         for dev in devices: 
#             for (type, desc, value) in dev.getScanData():
#                 #print(value)
#                 global espfound
#                 if espfound == False:
#                     if desc == "Complete Local Name":
#                         #print(value)
#                         espnum = self.__eq__which_esp(value)
#                         if espnum != None:
#                             espfound = True
#                             #rpi_rssi = []
                        
                        

#                 else:
#                     if desc == "Manufacturer":
#                         self.espdata[espnum] = self.xstr2ints(value)
#                         espfound = False
#                         return self.espdata 
                

# class rpi:
#     if dev.addr == "84:f7:03:09:0b:d9":
#     if dev.rssi != 0:
#         return dev.rssi



# esp1 = device("esp1", ESP_TYPE)

# class lut:
#     def __init__(self, dev1, dev2):
#         str1 = "lut_"
#         str2 = dev1.name
#         str2 = "_"
#         str3 = dev2.name
#         filename = str1 + str2 + str3
#         f = open(filename, 'a')
#         self.writer = csv.writer(f)

#     def generate(self):

        
    





# def generate_lut(device1, device2, scanner):
#     filename = device1.name
#     f = open()




def test(scanner):
    f = open('rssi_dist_lut', 'a')
    writer = csv.writer(f)


    distances = [10,20,30,40,50,60,70,80,90,100,110,120,130,140,150,160,170,180,190,200,210,220,230,240,250]
    #distances = [1]
    # idea: create GUI page for visualising the lookup tables


    # initializes "lastval"
    
    n = 0
    total = 0

    for d in distances:
        
        print("Scanning at {} cm now.".format(d))
        input("Ready?")
        for i in range(100):
            scanner.clear()
            scanner.start()
            devices = scanner.scan(0.1, passive=True) 

            for dev in devices:  
                
                if dev.addr == "84:f7:03:09:0b:d9":
                    if dev.rssi != 0:
                        print(dev.rssi)
                        total = total + dev.rssi
                        n = n + 1
    
        row = [d, total/n]
        writer.writerow(row)

    print("All scans complete.")
    f.close()

def test2(scanner, lastval):


    # initializes "lastval"

    scanner.clear()
    scanner.start()
    devices = scanner.scan(0.1, passive=True) 

    for dev in devices:  
        
        if dev.addr == "84:f7:03:09:0b:d9":
            if dev.rssi != 0:
                delta = dev.rssi - lastval
                lastval = dev.rssi
                return dev.rssi

# while True:
#     g = test2(scanner, 0)
#     print(g)
# h = test(scanner)
