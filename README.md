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

## Controlling the system
Below is the main loop that our controller will be running through. When we open a serial connection (handled through Toast) this loop will start running. It checks to see if there is any data in the serial buffer. Once data is detected we evaluate if the information in the buffer matches a command we're looking for. In this instance the two commands I've setup are just looking for "1984" or 1985" in the serial buffer in order to run them. Your commands can be anything but I chose numbers for simplicity sake as your information coming in from the buffer will be a text string and it's easy to convert a text string of numbers to an integer you can evaluate against. 

When we receive "1984" through the buffer we send back a message to the host stating the command was sent and then write GPIO_4 high for one second which activates the solenoid releasing the latch and launching the drawer open. The GPIO then goes back to low de-energizing the solenoid and we go back to waiting for a command. The drawer will remain open until manually closed and if an open command is sent again while open the solenoid will trigger but the latch will just toggle quickly since the drawer has already been released. 

When we receive a "1985" command we read the state of GPIO_2. This pin is connected to GPIO_0 through the limit switch. Since GPIO_0 is always high when the switch is closed (drawer closed) GPIO_2 will read high. When the switch opens (drawer open) the circuit is broken and GPIO_2 will read low. We return this drawer state to the host device over our serial connection. 

```
void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(4, LOW);
  while(Serial.available() > 0){
      static char message[MAX_MESSAGE_LENGTH];
      static unsigned int message_pos = 0;
      char inByte = Serial.read();
      if(inByte != '\n' && (message_pos < MAX_MESSAGE_LENGTH -1))
      {
        message[message_pos] = inByte;
        message_pos++;
      }
      else
      {
        //Add null character to string
        message[message_pos] = '\0';
        //Print the message (or do other things)
        Serial.println(message);
        int(number) = atoi(message);
        if(number == 1984){
          Serial.println("open command sent");
          digitalWrite(4,HIGH);
          delay(1000);
        }else if(number == 1985){
          if(digitalRead(2)==LOW){
            Serial.println("the drawer is open");
          }
          else{
            Serial.println("the drawer is closed");
          }
        }
        //Reset for the next message
        message_pos = 0;
      }
  }
}
```

## Working Demo

https://user-images.githubusercontent.com/50248029/217127493-03d580a1-e0ef-4bdb-aa5f-d92e23f3d24f.mp4



## Possible system block diagram
Below is a diagram of one proposed system. In this scenario we would connect our cable and have a detection method to switch the Vbus line of the USB port from 5V to 24V. The 24V line runs to a buck converter to drop the voltage to 3.3V for the MCU and solenoid which is wired through a power transistor controlled by the MCU. The USB communication lines continue to the MCU (through a USB to Serial converter) and finally we have the detection switch which is wired to the GPIO of the MCU. 

<p align="center">
  <img src="https://user-images.githubusercontent.com/50248029/217107398-357683c8-87c5-4dd5-9760-123aa217347a.png" width="80%" height="80%">
</p>


## Notes
The first thing I thought to check was whether or not 24V is a hard requirement. I stepped down my voltage in 1V increments until I found the point where the solenoid will no longer trigger which was 6V. Stepping back up at 0.1V increments it began triggering again at 6.5V. Since the coil is only briefly energized my powersupply does not update it's current reading fast enough for me to see the actual current draw at 6.5V but It's something I'll have to look into with the power monitor. 
