from PIL import Image
from PIL import ImageDraw
import math


deg = 68.
stick_length = 160
width = stick_length * 4
height = stick_length * 3


def rect_2_polar(x, y):
    r = math.sqrt(x * x + y * y)
    deg = math.atan(y / x)
    deg = deg * 180 / math.pi
    return (deg, r)


def polar_2_rect(deg, r):
    x = r * math.cos(math.radians(deg))
    y = r * math.sin(math.radians(deg))
    return (x, y)


def coord_2_screen(x, y):
    w = x + width / 2
    h = height - y
    return (w, h)


def screen_2_coord(w, h):
    x = w - width / 2
    y = height - h
    return (x, y)


canvas = Image.open("pp.bmp")

for dd in range(48):
    delta = (90 + deg / 2 - (deg / 49)) - (dd * deg / 49)
    for i in range(16):
        r = (stick_length * 2 + 5) + i * 10
        XY = polar_2_rect(delta, r)
        WH = coord_2_screen(XY[0], XY[1])
        pixel = canvas.getpixel(WH)
        print("0X%02X, 0X%02X, 0X%02X, " % (pixel[0], pixel[1], pixel[2]), end='')
        # draw.ellipse([WH[0] - 2, WH[1] - 2, WH[0] + 2, WH[1] + 2])
    print()
