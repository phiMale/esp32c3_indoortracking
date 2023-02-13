import tkinter as tk
import matplotlib

matplotlib.use("TkAgg") #backend; sagt mpl wie die bilder verarbeitet werden
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

from matplotlib.figure import Figure
import matplotlib.patches as mpt
import matplotlib.pyplot as plt
import numpy as np
#import liveplot_utility

print(matplotlib.get_backend())

class c_window(tk.Tk):     # class window erbt eigenschaften von class Tk (aus tk), aktuell nur kopie)
                        #zusätzl param werden unten drangepappt       
    def __init__(self):     #init der class wird kopiert von super
        super(c_window, self).__init__() #self: referenz zu selbst;

        self.title("Indoortracking Map") # property: funktion als variable
        self.geometry("500x500+100+100")

        self.mpl_canvas()

    def mpl_canvas(self):
        fig = Figure(figsize=(5,5), dpi=100)
        a = fig.add_subplot(111)
        a.plot([1])        
        canvas = FigureCanvasTkAgg(fig, self)
        canvas.draw()
        #canvas.get_tk_widfet().pack(side = BOTTOM, fill = BOTH, expand = True)




# interate through the coords list and read the coordinates and the text of each tuple



if __name__ == '__main__':
    # req for tkinter
    window = c_window()
    window.mainloop()


        
                    
