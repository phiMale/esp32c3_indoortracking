# Embedded Systems BLE Indoor Tracking Project
# Graphical User Interface Module
# ___________________________

import matplotlib
matplotlib.use("TkAgg") # backend
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure
import matplotlib.animation as animation
from matplotlib import style

import pos_calc
import random
from bluepy.btle import Scanner, DefaultDelegate

import tkinter as tk
from tkinter import ttk



LARGE_FONT = ("Verdana", 12)
style.use("ggplot")

f = Figure(figsize=(5,5), dpi=100)
a = f.add_subplot(111)

# for dummy animation
x4 = 200
y4 = 200

sce = pos_calc.sig_scenario()



def dummy_animate(i):
    
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

    a.clear()
    a.plot(x1, y1, 'bo')
    a.plot(x2, y2, 'go')
    a.plot(x3, x3, 'yo')    
    a.plot(x4, y4, 'ro')

def animate(i):
    coords = pos_calc.fetch_pos()


    x1 = coords[0][0]
    y1 = coords[0][1]
    x2 = coords[1][0]
    y2 = coords[1][1]
    x3 = coords[2][0]
    y3 = coords[2][1]
    x4 = coords[3][0]
    y4 = coords[3][1]

    a.clear()
    a.plot(x1, y1, 'bo')
    a.plot(x2, y2, 'go')
    a.plot(x3, x3, 'yo')    
    a.plot(x4, y4, 'ro')

class gui(tk.Tk): 
    def __init__(self, *args, **kwargs): 
        
        tk.Tk.__init__(self, *args, **kwargs)

        tk.Tk.wm_title(self, "ESP Indoor Tracking Project")
        
        container = tk.Frame(self) 

        container.pack(side="top", fill="both", expand = True) 
        container.grid_rowconfigure(0, weight=1) 
        container.grid_columnconfigure(0, weight=1) 

        self.frames = {}

        for F in (StartPage, PageOne):
            frame = F(container, self)
            self.frames[F] = frame
            frame.grid(row=0, column = 0, sticky="nsew") 
        
        self.show_frame(StartPage)

    def show_frame(self, cont):
        frame = self.frames[cont]
        frame.tkraise()

class StartPage(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        label = tk.Label(self, text="HOME", font=LARGE_FONT)
        label.pack(pady=10, padx=10)

        button1 = ttk.Button(self, text="Live Position Tracking", 
                            command=lambda: controller.show_frame(PageOne)) 
        button1.pack()



class PageOne(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        label = tk.Label(self, text="Live Positions", font=LARGE_FONT)
        label.pack(pady=10, padx=10) 

        button1 = ttk.Button(self, text="Home", 
                            command=lambda: controller.show_frame(StartPage)) 
        button1.pack()

        canvas = FigureCanvasTkAgg(f, self)
        canvas.draw()
        canvas.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand = True)



class ScanDelegate(DefaultDelegate):
    def __init__(self):
        DefaultDelegate.__init__(self)


scanner = Scanner().withDelegate(ScanDelegate())

app = gui() 
ani = animation.FuncAnimation(f, animate, interval=200)
app.mainloop()


