
# ArduCamControl

A arduino-project that allows you to control a self made servo platform via IR.


## Features

- Any remote control to control the Platform
- Movement speed can be changed
- Settings can be saved to EEPROM
- Positions can be saved and are callable
- Anti-Overheat: The servos can be disabled when not in use
## What you need for this project

- 2 Servo motors
- IR-Receiver
- IR-Sender (e.g. Remote control from an old TV)
- A case holding the servo motors and combine them with the platform
- Any kind of Arduino (tested with Uno & Nano, but in theory they should all work)
- Arduino IDE
## Installation

1. Download the project and open the ArduCamControl.ino
2. Set the boolean value "checkFrequencies" to 'true' to get the codes from your remote control
3. Upload the code and open the Serial Monitor and write down the given IR-codes into remote.h
4. Disable "checkFrequencies" with 'false' and reupload it to the arduino
## How to use

### Remote Functions:

##### **Shortcuts (0-9):**
- Can store Pan and Tilt positions
- When pressed, stored positions are approached
- When pressed in program mode, positions for this shortcut will be overwritten

##### **Up, Down, Left, Right:**
- Controls the Pan and Tilt positions of the platform
- Endstop for both can be set in the main file

##### **Home:**
- Moves the Servos to the Home position of 90 degrees each
- **[Planned]** Home Position can be set

##### **Disable:**
- Toggles the servo motors as well as the controls of the remote

##### **Speed:**
- Toggles the manual movement speed between 1x/4x
- [Planned] control movement speed of automatic mode (shortcuts)

##### **Program:**
- Sets the arduino into programming mode
- if next input is a number, its content will be overwritten with the current positions
- else if next input is not a number, programming mode will exit
- else if next input is the delete-button, delete mode will be activated

##### **Delete:**
- Can only be activated when in programming mode
- if next input is a number, its content will be deleted and the shortcut is empty
- else if next input is not a number, delete and programming mode will exit