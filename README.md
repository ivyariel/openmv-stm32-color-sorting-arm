# Color-Sorting Robotic Arm

This project contains STM32 firmware and an OpenMV script for a fixed-position robotic arm. OpenMV detects red, blue, and green objects and sends the detected color to the STM32 controller over UART. The controller then runs predefined servo movements to pick up and place each object.

## Code Structure

- `OpenMV/main.py`: Detects color blobs and sends an 18-byte UART frame containing the object's position, height, and color code.
- `Bsp/openmv.c` and `Bsp/openmv.h`: Receive and decode OpenMV data on the STM32.
- `App/control.c` and `App/control.h`: Define the pickup sequence and color-specific placement sequences.
- `Bsp/bus_servo.c` and `Bsp/bus_servo.h`: Send position commands to the bus servos.
- `Core/`, `System/`, and `Drivers/`: Provide STM32 startup, peripheral initialization, timing, HAL, and CMSIS support.
- `MDK-ARM/` and `4.3.2.ioc`: Contain the Keil project and STM32CubeMX configuration.

## How It Works

1. OpenMV detects a colored object and sends its data through UART.
2. The STM32 receives the frame and reads the color code.
3. The arm moves through fixed pickup and placement positions for that color.

The arm uses predefined servo positions. The transmitted image coordinates are available in the frame but are not used for dynamic positioning. Color thresholds and the camera's detection area can be adjusted in `OpenMV/main.py`.
