# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: FilePicker.py - Embeddable python script for file dialog picker
# author: Karl-Mihkel Ott

import tkinter as tk
from tkinter.filedialog import askopenfilename

tk.Tk().withdraw()
filename = askopenfilename()
print(filename)
