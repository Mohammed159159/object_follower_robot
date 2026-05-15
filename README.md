# Object-Follower Robot

An autonomous robot built with **Mbed OS** that follows objects using an ultrasonic sensor and a motor driver.

## Project Overview

This project implements a simple but effective object-following behavior. The robot measures the distance to an object in front of it using an HC-SR04 ultrasonic sensor. Depending on the distance, it uses a bang-bang controller with hysteresis to decide whether to move forward or stop.

### Key Features
- **Hysteresis Control:** Prevents "chattering" at the threshold distance by using two different setpoints for starting and stopping.
- **PWM Speed Control:** Uses Pulse Width Modulation to smoothly drive the motors via an L298N motor driver.
- **Mbed OS 6:** Leverages modern Mbed features like `ThisThread::sleep_for` and `chrono` durations.
- **Serial Debugging:** Real-time feedback of distance and movement status over USB.

## Hardware Configuration

| Component | Pin Assignment | Function |
|-----------|----------------|----------|
| **HC-SR04 TRIG** | D11 | Trigger pulse |
| **HC-SR04 ECHO** | D10 | Echo pulse |
| **L298N IN1** | PB_0 | Motor forward |
| **L298N IN2** | PB_1 | Motor backward |
| **L298N ENA** | D3 | PWM speed control |
| **LED1** | Built-in | Driving indicator |

## Demonstration

<video src="assets/object_follower_demo.mp4" width="600" controls>
  Your browser does not support the video tag.
</video>

## How to Build

1. Clone the repository with submodules:
   ```bash
   git clone --recursive https://github.com/Mohammed159159/object_follower_robot.git
   ```
2. Configure the Mbed environment and build using `mbed-tools` or the Mbed Studio.

## License
This project is licensed under the Apache-2.0 License - see the [LICENSE](LICENSE) file for details.
