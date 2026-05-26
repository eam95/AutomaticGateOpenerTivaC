# Automatic Gate Opener: TIVA C

## Project Overview

This project is a prototyped automatic gate opener using the TIVA C microcontroller from Texas Instrument. The project was completed in December of 2024, as coursework that explored MCU programming at a bare metal programming and its application. The gate opener uses a linear actuator that will simulate a simple prototype of the gate along with an LCD display, printing the status and state of the gate. To operate the gate opener a remote controller with a QiaChip Receiver RF Sensor is integrated, with a transmitter that either opens or closes the gate. When implemented in practice, people or objects may be in the path of the gate when it is in the closing state.

## Installation Requirements and Sub-Projects

The project has the gate opener firmware as shown in the repo or linked in the table below. In the subfolder rf_Integrated, goes into further detail on program functionality, and schematic of the entire project.

| Folder | Description | Software | Version |
|--------|-------------|----------|---------|
| [`rf_Integrated/`](rf_Integrated/) | TM4C123GLX Gate Opener firmware — sensors, motor control, wireless TX | Code Composer Studio | 12.3.0 |

## System Architecture
The architecture setup for the project is shown in the block diagram at a high-level overview. Note that motor driver used was the L298N not VNH7070BAS as shown in the block diagram.

![Gate Opener](/docs/images/HighLevelGateOpener.png)
*Figure 1: High-level overview of the project functionality.*

The Qia Chip RF module consists of the receiver, and remote controller with "ON"/"OFF" button where it will indicate open/close the gate. When command is received to open/close the gate, the receiver sends a logic state to the MCU to determine whether to open or close the gate. 

![Gate Opener](/docs/images/QiaChipReceiver.png)
*Figure 2: The Qia Chip receiver.*

![Gate Opener](/docs/images/QiaChipRemote.png)
*Figure 3: The Qia Chip remote controller.*

To indicate the status and state of the gate opener an LCD display prints a message when the gate is open, opening, closed, closing, or obstacle. These states will be determined based on the remote controller and the ultrasonic sensor readings(measured in all states) on the gate path. The ultra sonic sensor is used to measure the distance from the end effector of the gate to the wall to determine if its either fully open/closed, or it is opening/closing. During the opening and closing state the distance measured is displayed on the status LCD. To determine an obstacle has moved into the gate path, the difference between last measured position and current position is calculated, where a small displacement indicates on object is in the path whereas a large displacement indicates an obstacle is in the path. The setup of the gate and ultra sonic sensor is shown below along with the range of the states the gate is in.

![Gate Opener](/docs/images/obstacleDetector.png)
*Figure 4: The boundaries of the gate opener and includes the range of the gate opener states.*


## Test Run

A small 4 minute video demonstrating the gate opener in action.

[Gate Opener](https://youtu.be/Qj_LQ_2-MLc)


## Key Challenges

Most of the time the gate opener was able to respond to the respond based on the remote controller logic state, and readings from the ultra-sonic. However, there were some challenges encountered as it will be discuss on each section below.

### Stationary Object Detection

Although the system is able to detect an object momentarily and reopen the gate until it object is removed, the algorithm setup suffers on stationary objects. When an object is placed over a longer period of time, the displacement algorithm will incorrectly determine there is no object ahead due to the displacement measurement being small where it assumes that there is no object while the gate is closing. A potential solution is place a magnet at the wall and the hall effect sensor at the end of the gate, for detection of the wall. To safety proof it, the gate must be in the open state to check if the distance measured is equivalent to the distance when the gate is fully open as reference. While the gate is closing, it can record a few points to track the position of the actuator so that when an object is stationary it can detect but compare the distance it recorded and then retract the gate until the object is removed.


### LCD Display Ground Loop
The status LCD Display uses SPI protocol in order to reduce the amount of wires/pins required. A shift register is used to convert the serial data received from the MCU to parallel data pin that will be send to the pins of the LCD display. In the test video, there are two LCD displays, where one is built-in on the EDU board from CSUF, and attempted version on the bread board. At random or when there is jolt from the linear actuator, it will send a corrupt message as shown below, and is resolved by pressing the reset button to reinitialize the LCD display.

![Gate Opener](/docs/images/lcdDisplayGroundIssue.png)
*Figure 5: The LCD Display on the EDU board (board from CSUF) vs the attempted version on breadboard with corrupted message.*

The potential cause for the corruption of the LCD display message could be the grounding for the LCD display, on the bread board along with the L298N motor driver. When the MCU is mounted to the EDU board as shown in the picture/video, it does not display and corrupted message from a jolt received from the motor driver. To resolve the issue a perf board can have the MCU attached and the LCD display soldered onto it to reduce the ground loop.  