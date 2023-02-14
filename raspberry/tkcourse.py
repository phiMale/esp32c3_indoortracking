import matplotlib
matplotlib.use("TkAgg") # backend

from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure
import matplotlib.animation as animation
from matplotlib import style

import random

import dummy_blescan_utility as dummy
from lut_gen import test2
from bluepy.btle import Scanner, DefaultDelegate


import tkinter as tk
from tkinter import ttk

LARGE_FONT = ("Verdana", 12)
style.use("ggplot")

f = Figure(figsize=(5,5), dpi=100)
a = f.add_subplot(111)

g = Figure(figsize=(5,5), dpi=100)
b = g.add_subplot(111)

x = 0 # for test graph

x4 = 200
y4 = 200

def animate(i): # i for interval
    pullData = open("sampleData.txt", "r").read()
    dataList = pullData.split('\n')

    xList = []
    yList = []
    
    x1 = 0
    y1 = 0
    x2 = 400
    y2 = 0
    x3 = 400
    y3 = 400
    global x4
    x4 = x4 + random.randint(-20,20)
    if x4 > 400 or x4 < 0:
        x4 = 200
    global y4
    y4 = y4 + random.randint(-20,20)
    if y4 > 400 or y4 < 0:
        y4 = 200


    xList.extend([x1, x2, x3, x4])
    yList.extend([y1, y2, y3, y4])

    a.clear()
    a.plot(xList, yList, 'bo')
    
# to continue where I left off: I simply wanted to plot the new rssi rate of change I get with test() to see if it was useful. 
# turns out I can't even print some simple data. anyways, will experiment with the rssi delta a bit. rssi itself seems not suited;
# the antenna continuously adjusts its power when the distance is changed. yikes.

x = 0
lastval = 0
xList = []
yList = []


def animate2(i):
    global x
    x = x + 1
    y = test2(scanner, lastval)
    #lastval = y

    xList.append(x)
    yList.append(y)

    b.clear()
    b.plot(xList, yList, 'b-')



# this class is the foundation of the tkinter application you're making. all the pages and windows will be based on this. setup engineering.
class c_itGUI(tk.Tk): # class c_itGUI taking traits from the class Tk within tk
    def __init__(self, *args, **kwargs): # __init__ as the initialization of the class that always runs when calling the class. self as conventional first param of a method. args and keword args as placeholders for anything passed on
        
        tk.Tk.__init__(self, *args, **kwargs) # initializes tkinter2
        
        # to add a title:

        tk.Tk.wm_title(self, "ESP Indoor Tracking Project")

        # to add an icon: 
        # tk.Tk.iconbitmap(self, default="somefile.ico")
        
        container = tk.Frame(self) # frame is the borders of the window. we create an object "container" and give it the class traits of Frame.

        # notes on pack: very quick way to pack stuff into a window. doesn'T give you much control
        container.pack(side="top", fill="both", expand = True) # fill: fills in the space alloted to the pack. expand: goes beyond the defined limits
        container.grid_rowconfigure(0, weight=1) # 
        container.grid_columnconfigure(0, weight=1) # 

        self.frames = {}

        for F in (StartPage, PageOne, PageTwo, PageThree):

            frame = F(container, self)

            self.frames[F] = frame

            frame.grid(row=0, column = 0, sticky="nsew") # sticky = north south east west; aligns and stretches the frame to the border
        
        self.show_frame(StartPage)

    def show_frame(self, cont):

        frame = self.frames[cont] # cont = control
        frame.tkraise() # raises the window to the front

class StartPage(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        label = tk.Label(self, text="HOME", font=LARGE_FONT) # text: this is how you add text to a tkinter window.
        label.pack(pady=10, padx=10) # padding

        button1 = ttk.Button(self, text="Live Position Tracking", 
                            command=lambda: controller.show_frame(PageOne)) # a btn needs a text and a command (function). lambda: prohibits the calling of the function when assigning "command" when you need to pass a param.
        button1.pack()

        button2 = ttk.Button(self, text="The Code", 
                            command=lambda: controller.show_frame(PageTwo)) # a btn needs a text and a command (function). lambda: prohibits the calling of the function when assigning "command" when you need to pass a param.
        button2.pack()

        button3 = ttk.Button(self, text="Github Repo", 
                            command=lambda: controller.show_frame(PageThree)) # a btn needs a text and a command (function). lambda: prohibits the calling of the function when assigning "command" when you need to pass a param.
        button3.pack()


class PageOne(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        label = tk.Label(self, text="Live Positions", font=LARGE_FONT) # text: this is how you add text to a tkinter window.
        label.pack(pady=10, padx=10) # padding

        button1 = ttk.Button(self, text="Home", 
                            command=lambda: controller.show_frame(StartPage)) # a btn needs a text and a command (function). lambda: prohibits the calling of the function when assigning "command" when you need to pass a param.
        button1.pack()

        canvas = FigureCanvasTkAgg(f, self)
        canvas.draw()
        canvas.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand = True)

class PageTwo(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        label = tk.Label(self, text="Test Graph", font=LARGE_FONT) # text: this is how you add text to a tkinter window.
        label.pack(pady=10, padx=10) # padding

        button1 = ttk.Button(self, text="Back to Home", 
                            command=lambda: controller.show_frame(StartPage)) # a btn needs a text and a command (function). lambda: prohibits the calling of the function when assigning "command" when you need to pass a param.
        button1.pack()

        canvas = FigureCanvasTkAgg(g, self)
        canvas.draw()
        canvas.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand = True)

class PageThree(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        label = tk.Label(self, text="Graph Page", font=LARGE_FONT) # text: this is how you add text to a tkinter window.
        label.pack(pady=10, padx=10) # padding

        button1 = ttk.Button(self, text="Back to Home", 
                            command=lambda: controller.show_frame(StartPage)) # a btn needs a text and a command (function). lambda: prohibits the calling of the function when assigning "command" when you need to pass a param.
        button1.pack()


class ScanDelegate(DefaultDelegate):
    def __init__(self):
        DefaultDelegate.__init__(self) # alternatively: super().__init__(self)


scanner = Scanner().withDelegate(ScanDelegate()) #scanner (objekt) hat funktion withDelegate mit übergabewert return von scandelgeate

app = c_itGUI() # creating the app object from class
ani = animation.FuncAnimation(f, animate, interval=1000)
ani2 = animation.FuncAnimation(g, animate2, interval=400)
app.mainloop() # tkinter code to initialize the window


