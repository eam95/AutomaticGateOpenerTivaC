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

The Qia Chip RF module consit of the remote controller, and receiver that consit of the "ON"/"OFF" button where it will indicate open/close the gate. When command is received to open/close the gate the logic state is read by the MCU to determine whether to open or close the gate. 

![Gate Opener](/docs/images/QiaChipReceiver.png)
*Figure 2: The Qia Chip receiver.*

![Gate Opener](/docs/images/QiaChipRemote.png)
*Figure 3: The Qia Chip remote controller.*
