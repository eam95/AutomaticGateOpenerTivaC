# Automatic Gate Opener: TIVA C

## Project Overview

This project is a prototyped automatic gate opener using the TIVA C microcontroller from Texas Instrument. The project was complete in December of 2024, which is the coursework that explores MCU programming at a bare metal programming and its application. The gate opener uses a linear actuator that will simulate a simple prototype of the gate along with an LCD display, printing the status and state of the gate. To operate the gate opener a remote controller with a QiaChip Receiver RF Sensor is integrated, with a transmitter that either opens or closes the gate. When implemented in practice, people or objects can be in the path of the gate when it is in the closing state.

## Installation Requirements and Sub-Projects

The project has the gate opener firmware as shown in the repo or linked in the table below. In the subfolder rf_Integrated, will go further in detail on program functionality, and schematic of the entire project.

| Folder | Description | Software | Version |
|--------|-------------|----------|---------|
| [`rf_Integrated/`](rf_Integrated/) | TM4C123GLX Gate Opener firmware — sensors, motor control, wireless TX | Code Composer Studio | 12.3.0 |

## System Architecture
The Architecture setup for the project is shown in the block diagram at a high-level overview.

![Gate Opener](/docs/images/HighLevelGateOpener.png)
*Figure 1: High-level overview of the project functionality.*

The Qia Chip RF module consist of the receiver, and remote controller with "ON"/"OFF" button where it will indicate open/close the gate. When command is received to open/close the gate, the receiver sends a logic state to the MCU to determine whether to open or close the gate. 

![Gate Opener](/docs/images/QiaChipReceiver.png)
*Figure 2: The Qia Chip receiver.*

![Gate Opener](/docs/images/QiaChipRemote.png)
*Figure 3: The Qia Chip remote controller.*

To indicate the status and state of the gate opener a LCD display prints a message when a the gate is open, opening, closed, closing, or obstacle. These states will be determined based on the remote controller and the ultra sonic sensor readings(measured in all states) on the gate path. The ultra sonic sensor is used to measure the distance from the end effector of the gate to the wall to determine if its either fully open/closed, or it is opening/closing. During the opening and closing state the distance measured is displayed on the status LCD. To determine an obstacle has moved into the get path, the difference between last measured position and current position is calculated, where a small displacement indicates on object is in the path whereas a large displacement indicate an obstacle is in the path. The setup of the gate and ultra sonic sensor is shown below along with the range of the states the gate is in.

![Gate Opener](/docs/images/obstacleDetector.png)
*Figure 4: The boundaries of the gate opener and includes the range of the gate opener states.*


## Test Run

A small 4 minute video demonstrating the gate opener in action.

Will upload soon....



## Key Challenges

Most of the time the gate opener was able to respond to the respond based on the remote controller logic state, and readings from the ultra-sonic. However, there were some challenges encountered as it will be discuss on each section below.

**Stationary Object Detection**
    Although it is able to detect an object momentarily and reopen the gate until it object is removed, the algorithm setup suffers on stationary objects. When an object is placed over a longer period of time, the displacement algorithm will think there is no object ahead due to the displacement measurement being small where it assumes that there is no object while the gate is closing. A potential solution is place a magnet at the wall and the hall effect sensor at the end of the gate, for detection of an object or wall. To safety proof it, it must be in the open state in order to check if the distance measured is equivalent to the distance when the gate is fully open as reference. 


