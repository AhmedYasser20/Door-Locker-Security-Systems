# Door Locker Security System


## Introduction

This GitHub repository contains the source code for a Door Locker Security System project. The system is designed to unlock a door using a password. It is implemented using two ATmega32 Microcontrollers with a frequency of 8MHz. The project is based on a layered architecture model, with two main components:

- **HMI_ECU (Human Machine Interface)**: Responsible for interacting with the user using a 2x16 LCD and a 4x4 keypad.
- **CONTROL_ECU**: Responsible for processing and making decisions in the system, such as password checking, opening the door, and activating the system alarm. It includes EEPROM, a buzzer, and a DC motor.

## System Sequence

The system follows a specific sequence of steps:

### Step 1: Create a System Password

- The LCD displays "Please Enter Password."
- The user enters a password consisting of 5 numbers, with '*' displayed for each number.
- The user presses the enter button.
- The system asks the user to re-enter the same password for confirmation, displaying the message "Please re-enter the same Pass."
- The user enters the password again.
- HMI_ECU sends the two passwords to the CONTROL_ECU through UART.
- If the two passwords match, the system has a password and saves it inside the EEPROM. Then, it proceeds to Step 2.
- If the two passwords do not match, the user must repeat Step 1.

### Step 2: Main Options

- The LCD always displays the main system options.

### Step 3: Open Door (+)

- The LCD displays "Please Enter Password."
- The user enters the password and presses the enter button.
- HMI_ECU sends the password to the CONTROL_ECU, which compares it with the one saved in the EEPROM.
- If the two passwords match:
  - The system rotates the motor for 15 seconds clockwise and displays the message "Door is Unlocking."
  - The system holds the motor for 3 seconds.
  - The system rotates the motor for 15 seconds counterclockwise and displays the message "Door is Locking."

### Step 4: Change Password (-)

- The LCD displays "Please Enter Password."
- The user enters the password and presses the enter button.
- HMI_ECU sends the password to the CONTROL_ECU, which compares it with the one saved in the EEPROM.
- If the two passwords match, the system repeats Step 1.

### Step 5

- If the two passwords are unmatched in Step 3 (+: Open Door) or Step 4 (-: Change Password):
- The user is asked for the password one more time.
- If the two passwords match, the system either opens the door or changes the password in Steps 3 and 4.
- If the two passwords are not matched again, the following actions are taken:
  - The system activates the buzzer for 1 minute.
  - An error message is displayed on the LCD for 1 minute.
  - The system is locked, and no inputs from the keypad will be accepted during this time.
- The system then goes back to Step 2, displaying the main options.

## Driver and Hardware 

The project uses several drivers and hardware components, including:

- GPIO driver: Used for general-purpose input/output operations.
- LCD driver: Utilizes a 2x16 LCD display.
- Keypad driver: Interfaces with a 4x4 keypad.
- DC Motor driver: Controls a DC motor, which is connected to the CONTROL_ECU.
- EEPROM driver: Manages external EEPROM using I2C communication.
- I2C driver: Used for TWI communication in the CONTROL_ECU.
- UART driver: Handles serial communication between the two ECUs.
- Timer driver: Utilizes Timer1 for timing and interrupts.
- Buzzer driver: Controls a buzzer for generating sound signals.

## Code Files

This repository contains two main code files for the HMI_ECU and CONTROL_ECU. These files implement the functionality described in the system sequence.

- `HMI_ECU/main.c`: Contains the code for the Human Machine Interface ECU.
- `CONTROL_ECU/main.c`: Contains the code for the Control ECU.

## ScreenShot

