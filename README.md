## Description
This project covers a potential solution to opening a POS cash drawer withouth the use of a printer in the POS system. The goal is to create a USB to RJ-11 cable that will be compatible with all Cashdrawers currently supported by Toast.

### Current solution
Right now every cash drawer used in Toasts POS systems is connected to a printer via an RJ-11 cable. When a cash payment is initiated a command is sent to the printer telling it to open the cash drawer which enables the 24V line on the RJ-11 cable and activates the solenoid in the cash drawer releasing the locking mechanism and causing the compressed spring to push the drawer open. The Solenoid requires a 24V 1.2A pulse for approximately one second to retract the piston and release the lock. In order to not have to replace every cash drawer in the field and maintain backwards compatability our new proposed solution would need to be able to provide this 24V kick. 

### Proposed Solution
In a printerless system we would look to plug the cash drawer into the next gen Toast hub with a standard USB connection. Using a small micro controller and some transistor logic we can open a serial connection with the host device and await the command to open the cash drawer. Once detected we can enable a 24V line for the solenoid kick. This would allow us to retain use of cash drawers currently in the field while also minimizing the types of ports on the new hub. Below is just a simple diagram of the cable connections.

<p align="center">
  <img src="https://user-images.githubusercontent.com/50248029/217106735-018c5260-9605-44cb-a2d0-6e458d9fd4d4.png" width="80%" height="80%">
</p>

## Proof of concept
The internals of the cash drawer consist of just a few simple components. We have our 24V solenoid, a limit switch, a locking mechanism and a spring. This drawer had six connections wired into the RJ-11 connector. For my proof of concept I only used found which included the positive and negative wires for the solenoid and two switch wires. 

<p align="center">
  <img src="https://user-images.githubusercontent.com/50248029/217114270-3b13e62d-0c7a-4aa2-ab03-d140c802c3f1.jpg" width="60%" height="60%">
</p>

A wiring diagramn of my test circuit can be found below. I'm using a raspberry Pi Pico for my development board which uses an RP2040 as the MCU and has an onboard USB to serial converter and 3.3V regulator. We'll be using a TIP120 NPN power transistor to act as the "switch" for our 24V line. This transistor was chosen partially because it's what I had on hand but also because it can support up to 5A and we will be drawing at least 1A through it. I'm using a bench top power supply to provide the 24V to the solenoid. One of the solenoid wires is connected to 24V and the other is connected to the collector of the TIP120. The base of the TIP120 is tied to GPIO_4 of the PI Pico, this pin works as our switch. When we set GPIO_4 to high we the 24V to flow from the power supply through the solenoid and back to ground completing the circuit. With the GPIO set low (or unpowered) the circuit remains open. The emitter of the TIP120 is tied to a common ground with the power supply and Pico ground connections. 

To repurpose our limit switch I tied it to GPIOs 0 and 2. GPIO_0 is setup as an output and always set to high meaning it will output 3.3V. GPIO_2 is setup as an input. By reading the state of GPIO_2 we can tell wether the switch is open (low) or closed (high). 

<p align="center">
  <img src="https://user-images.githubusercontent.com/50248029/217115863-f20e2546-2580-4d5b-9415-9c38e6ebac7c.png" width="60%" height="60%">
</p>

## Possible system block diagram
Below is a diagram of one proposed system. In this scenario we would connect our cable and have a detection method to switch the Vbus line of the USB port from 5V to 24V. The 24V line runs to a buck converter to drop the voltage to 3.3V for the MCU and solenoid which is wired through a power transistor controlled by the MCU. The USB communication lines continue to the MCU (through a USB to Serial converter) and finally we have the detection switch which is wired to the GPIO of the MCU. 

<p align="center">
  <img src="https://user-images.githubusercontent.com/50248029/217107398-357683c8-87c5-4dd5-9760-123aa217347a.png" width="80%" height="80%">
</p>
