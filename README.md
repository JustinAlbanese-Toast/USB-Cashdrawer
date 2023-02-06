## Description
This project covers a potential solution to opening a POS cash drawer withouth the use of a printer in the POS system. The goal is to create a USB to RJ-11 cable that will be compatible with all Cashdrawers currently supported by Toast.

### Current solution
Right now every cash drawer used in Toasts POS systems is connected to a printer via an RJ-11 cable. When a cash payment is initiated a command is sent to the printer telling it to open the cash drawer which enables the 24V line on the RJ-11 cable and activates the solenoid in the cash drawer releasing the locking mechanism and causing the compressed spring to push the drawer open. The Solenoid requires a 24V 1.2A pulse for approximately one second to retract the piston and release the lock. In order to not have to replace every cash drawer in the field and maintain backwards compatability our new proposed solution would need to be able to provide this 24V kick. 

### Proposed Solution
In a printerless system we would look to plug the cash drawer into the next gen Toast hub with a standard USB connection. 
