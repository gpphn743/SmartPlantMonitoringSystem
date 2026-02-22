import time
import busio
import digitalio
# from board import SCK, MOSI, MISO, CE0, D24, D25   # SPI0
from board import D21, D19, D20, D16, D13, D26		 # SPI1

from adafruit_rgb_display import color565
import adafruit_rgb_display.ili9341 as ili9341

from PIL import Image, ImageDraw, ImageFont
import random

import subprocess, json, base64

# config
# CS_PIN = CE0	# SPI0 - CE0
CS_PIN = D16	# SPI1 - CE2
DC_PIN = D26
RESET_PIN = D13
BAUDRATE = 24000000


# SPI config 
# spi = busio.SPI(clock=SCK, MOSI=MOSI, MISO=MISO)	# SPI0
spi = busio.SPI(clock=D21, MOSI=D20, MISO=D19)		# SPI1 

# init screen
display = ili9341.ILI9341(spi, cs=digitalio.DigitalInOut(CS_PIN),
                               dc=digitalio.DigitalInOut(DC_PIN),
                          rst=digitalio.DigitalInOut(RESET_PIN),
                               baudrate=BAUDRATE)


# open image
# img = Image.open("road.jpg").convert("RGB")

# rotate 90 couter clockwise
# img = img.transpose(Image.ROTATE_90)
# img = img.resize((display.width, display.height), Image.BICUBIC)
# display.image(img, x=0, y=0)
font = ImageFont.load_default()

    
def show_params(temp, hum, soil, light):
    canvas = Image.new('RGB', (display.width, display.height), (0, 0, 0))
    draw = ImageDraw.Draw(canvas)
    lines = [
        f"Temp : {temp:.1f} C",
        f"Humi : {hum:.1f} %",
        f"Soil : {soil:.0f}",
        f"Light: {light:.0f}"
    ]
    y = 10
    for line in lines:
        draw.text((10, y), line, font=font, fill=(255, 255, 255))
        y += 12   #font height 11
    display.image(canvas)

# decode lora
proc = subprocess.Popen(
    ['journalctl','-f','-u','single_chan_pkt_fwd','-o','cat'],
    stdout=subprocess.PIPE, text=True
)


for raw in proc.stdout:
    if '"rxpk"' not in raw:
        continue
    try:
        j = json.loads(raw[raw.find('{'):])
        b64 = j['rxpk'][0]['data']
        text = base64.b64decode(b64).decode('utf-8')
        #Temp:..., Humi:..., Soil:..., Light:...
        parts = [p.strip() for p in text.split(',')]
        temp  = float(parts[0].split(':')[1][:-1])
        hum   = float(parts[1].split(':')[1][:-1])
        soil  = float(parts[2].split(':')[1])
        light = float(parts[3].split(':')[1])
        show_params(temp, hum, soil, light)

    except Exception:
        pass
    time.sleep(0.1)

