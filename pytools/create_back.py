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


canvas = Image.new("RGB", (width, height + 10), (0, 0, 0))
draw = ImageDraw.Draw(canvas)

# Draw outer arc
start = coord_2_screen(-stick_length * 3, stick_length * 3)
end = coord_2_screen(stick_length * 3, -stick_length * 3)
draw.arc([start, end], -90 - deg / 2, -90 + deg / 2,
         fill=(200, 200, 200))

# Draw inner arc
start = coord_2_screen(-stick_length * 2, stick_length * 2)
end = coord_2_screen(stick_length * 2, -stick_length * 2)
draw.arc([start, end], -90 - deg / 2, -90 + deg / 2,
         fill=(200, 200, 200))

# Draw left bound
start = polar_2_rect(90 + deg / 2, stick_length * 2)
start = coord_2_screen(start[0], start[1])
end = polar_2_rect(90 + deg / 2, stick_length * 3)
end = coord_2_screen(end[0], end[1])
draw.line((start, end), fill=(200, 200, 200))

# Draw right bound
start = polar_2_rect(90 - deg / 2, stick_length * 2)
start = coord_2_screen(start[0], start[1])
end = polar_2_rect(90 - deg / 2, stick_length * 3)
end = coord_2_screen(end[0], end[1])
draw.line((start, end), fill=(200, 200, 200))

# Draw x y axis
start = coord_2_screen(+width, 0)
end = coord_2_screen(-width, 0)
draw.line((start, end), fill=(200, 200, 0))
start = coord_2_screen(0, -10)
end = coord_2_screen(0, +stick_length * 2)
draw.line((start, end), fill=(200, 200, 0))


for dd in range(48):
    delta = (90 + deg / 2 - (deg / 49)) - (dd * deg / 49)
    for i in range(16):
        r = (stick_length * 2 + 5) + i * 10
        XY = polar_2_rect(delta, r)
        WH = coord_2_screen(XY[0], XY[1])
        pixel = canvas.getpixel(WH)
        print("0X%X, 0X%X, 0X%X, " % (pixel[0], pixel[1], pixel[2]), end='')
        # draw.ellipse([WH[0] - 2, WH[1] - 2, WH[0] + 2, WH[1] + 2])
    print()

canvas.save("pp.bmp")
# canvas.show()
