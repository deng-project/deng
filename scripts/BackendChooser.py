# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: BackendChooser.py - Embeddable Python script to select a correct backend to use for required program
# author: Karl-Mihkel Ott

import tkinter as tk
import tkinter.messagebox as msgbox
from enum import IntEnum


class ApiType(IntEnum):
    Vulkan = 1
    OpenGL = 2
    DirectX = 3


# Default value
api = ApiType.Vulkan
win = tk.Tk()


# Button handler class
class ApiButtonHandlers:
    pixel_virtual: tk.PhotoImage
    content: tk.Frame
    opengl: tk.Button
    vulkan: tk.Button
    directx: tk.Button

    @staticmethod
    def SelectVulkanBackend():
        global api, win
        api = ApiType.Vulkan
        win.destroy()

    @staticmethod
    def SelectOpenGLBackend():
        global api, win
        api = ApiType.OpenGL
        win.destroy()

    @staticmethod
    def SelectDirectXBackend():
        msgbox.showerror("Error", "DirectX backend is not supported")

    def __init__(self, win: tk.Tk):
        self.pixel_virtual = tk.PhotoImage(width=1, height=1)

        # OpenGL button
        self.opengl = tk.Button(
            win,
            text="OpenGL",
            image=self.pixel_virtual,
            width=50,
            height=20,
            command=ApiButtonHandlers.SelectOpenGLBackend,
            compound='c'
        )

        self.opengl.grid(column=0, row=1)

        # Vulkan button
        self.vulkan = tk.Button(
            win,
            text="Vulkan",
            image=self.pixel_virtual,
            width=50,
            height=20,
            command=ApiButtonHandlers.SelectVulkanBackend,
            compound='c'
        )
        self.vulkan.grid(column=0, row=1)

        # DirectX button
        self.directx = tk.Button(
            win,
            text="DirectX",
            image=self.pixel_virtual,
            width=50,
            height=20,
            command=ApiButtonHandlers.SelectDirectXBackend,
            compound='c'
        )
        self.directx.grid(column=0, row=1)


def Prompt():
    global api, win
    win.title("Select renderer API")
    win.geometry('350x100')
    win.resizable(False, False)

    label = tk.Label(win, text="Select renderer API to use for DENG application")
    btn_handler = ApiButtonHandlers(win)

    # POSITIONS
    label.grid(row=0, column=0, columnspan=3, padx=20, pady=10)
    btn_handler.opengl.grid(row=2)
    btn_handler.vulkan.grid(row=2, column=1)
    btn_handler.directx.grid(row=2, column=2)

    win.mainloop()
    return api
