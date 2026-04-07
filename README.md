# IR power switch for mini PC

This project aims to power on mini PC by TV remote.

![IR switch outline image](resources/irswitch-outline.png "IR switch outline image")

NucBox G5 expose power switch pins as indicated by red arrow in below image (2nd from right most pin is the power on pin).

![NucBox G5 power switch pins](resources/power-switch-pins.jpg "NucBox G5 power switch pins")

Create a connector like below image.

![custom made connector](resources/power-pin-connector.jpg "custom made connector")

I soldered 4 wires unthinkingly, but 1 wire would be enough.

Then tuck the connector to the space between mother board and fan platform.

![Connector connected](resources/power-pin-connector-connected.jpg "Connector connected")

Use multimeter to ensure pins are not short circuited.

Connect the other end of the connector to MCU with IR receiver and it's done.

You can use the USB port backside of G5 as a power source.

Now my G5 looks like this.

![NucBox G5 with IR switch](resources/g5-ir-switch.jpg "NucBox G5 with IR switch")

See [circuit/irswitch-sch.pdf](circuit/irswitch-sch.pdf) for the schematic.
