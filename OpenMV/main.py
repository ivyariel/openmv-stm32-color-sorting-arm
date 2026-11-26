"""OpenMV color detection for the STM32 arm controller.

Calibrate the LAB thresholds and point the camera at the fixed pickup area
before running the arm. OpenMV UART3 TX (P4) connects to STM32 USART3 RX
(PB11); both boards must share ground.
"""

import sensor
import struct
import time
from pyb import UART

# Example LAB thresholds. Replace with values measured in OpenMV IDE.
RED = (20, 100, 15, 127, 15, 127)
BLUE = (0, 90, -20, 40, -128, -10)
GREEN = (20, 100, -128, -10, -10, 127)

# The STM32 firmware expects red=1, blue=2, and green=4.
COLOR_THRESHOLDS = ((1, RED), (2, BLUE), (4, GREEN))
ROI = None  # Set to (x, y, width, height) to watch only the pickup area.
MIN_PIXELS = 150
MIN_AREA = 150
STABLE_FRAMES = 3

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA)
sensor.skip_frames(time=2000)
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)

uart = UART(3, 115200, timeout_char=1000)
last_color = 0
stable_frames = 0

while True:
    image = sensor.snapshot()
    best_blob = None
    best_color = 0

    for color_code, threshold in COLOR_THRESHOLDS:
        blobs = image.find_blobs(
            [threshold],
            roi=ROI,
            pixels_threshold=MIN_PIXELS,
            area_threshold=MIN_AREA,
            merge=True,
        )
        for blob in blobs:
            if best_blob is None or blob.pixels() > best_blob.pixels():
                best_blob = blob
                best_color = color_code

    if best_blob is None:
        last_color = 0
        stable_frames = 0
        continue

    if best_color == last_color:
        stable_frames += 1
    else:
        last_color = best_color
        stable_frames = 1

    if stable_frames < STABLE_FRAMES:
        continue

    # A5 + four little-endian signed 32-bit integers + 5A = 18 bytes.
    # x, y, and h are reserved for future position control; m is color.
    packet = b"\xA5" + struct.pack(
        "<iiii", best_blob.cx(), best_blob.cy(), best_blob.h(), best_color
    ) + b"\x5A"
    uart.write(packet)
    time.sleep_ms(100)
