import random

import matplotlib
matplotlib.use("TkAgg")
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure
from matplotlib import style
import tkinter as tk
from tkinter import ttk
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
#import csv_gen_utility

plt.style.use('dark_background')



fig = Figure(figsize=(5,5), dpi=100)
subfig = fig.add_subplot(111)



class c_window(tk.Tk):
    
    def __init__(self, *args, **kwargs):
        super(c_window, self).__init__()
        tk.Tk.__init__(self, *args, **kwargs)

        #tk.Tk.wm_title(self, "BLE Indoortracking Project")
        self.title("BLE Indoortracking Project")
        self.geometry("500x500+100+100")
        self.mpl_canvas()

        container = tk.Frame(self)
        container.pack(side="top", fill="both", expand = True)
        container.grid_rowconfigure(0, weight=1)
        container.grid_columnconfigure(0, weight=1)

        self.mpl_canvas()

    def mpl_canvas(self):
        fig = Figure(figsize=(5,5), dpi=100)
        subfig = fig.add_subplot(111)
        subfig.plot([1])        
        canvas = FigureCanvasTkAgg(fig, self)
        canvas.draw()
        #canvas.get_tk_widget().pack(side = BOTTOM, fill = BOTH, expand = True)

def animate(i):
    data = pd.read_csv('positiondata.csv')
    x1 = data("x1")
    y1 = data("y1")
    x2 = data("x2")
    y2 = data("y2")
    x3 = data("x3")
    y3 = data("y3")
    x4 = data("x4")
    y4 = data("y4")

    subfig.clear()

    subfig.plot(x1, y1, 'b.', label='1')
    subfig.plot(x2, y2, 'b.', label='2')
    subfig.plot(x3, y3, 'b.', label='3')
    subfig.plot(x4, y4, 'r.', label='4')



app = c_window()
ani = FuncAnimation(fig, animate, interval=500)
app.mainloop()
