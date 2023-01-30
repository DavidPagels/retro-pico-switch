# N64 -> Raspberry Pi Pico -> Nintendo Switch

## Table of Contents

- [Description](#description)
- [Inspiration](#inspiration)
- [Setup](#setup)
- [3D Printed Enclosure/Plug](#3d-printed-enclosureplug)
- [Credits](#credits)

---

## Description

This code allows a Raspberry Pi Pico to control a Nintendo Switch using button and joystick inputs from an original N64 controller. This is a spiritual successor to my [N64-Arduino-Switch](https://github.com/DavidPagels/n64-arduino-switch) project.

## Inspiration

It's been a year since I published the [N64-Arduino-Switch](https://github.com/DavidPagels/n64-arduino-switch) project. Since then, the wireless N64 Nintendo Switch controllers have still been largely out of stock and scalped. One of the pieces of feedback from the last project is that it looked intimidating to setup. I've since discovered the Raspberry Pi Pico and decided to redo this project for the Pico. This has 2 advantages over the Arduino project:
  
- Programming involves plugging in a Micro USB cable while holding a button, dragging and dropping a file, and you're done! No more installing a Device Firmware Update (DFU) programmer, installing extra libraries, or shorting pins to get the device into a DFU mode.
- The Pico only costs $4 instead of $10 for an off-brand, slower Arduino!

Also, when playing through The Legend of Zelda: Ocarina of Time last year on the Arduino version, it was annoying not having a way to go to the Switch's Home screen using the N64 controller. Since I didn't rely on a library to handle N64 controller communication this time, I found out that there is a special 'Reset' bit that's set when L + R + Start are held at the same time. This project maps that input to the Home button on the Switch! 

## Setup

To program the Raspberry Pi Pico

1. Hold down the boot sel button on the Pico, and while holding, plug it into your PC via Micro USB cable.
2. Open the Pico in your File Explorer.
3. Download the .uf2 file from [this repository's most recent release](https://github.com/davidpagels/n64-pico-switch/releases).
4. Drag and drop the .uf2 file to the Pico folder from step 2.
5. Done!

Once these steps are done, [wire up your N64 controller](https://github.com/pothos/arduino-n64-controller-library/blob/master/README.md#wireing). The pre-built .uf2 file assumes the data pin is on the Pico's [GP18 pin](https://datasheets.raspberrypi.com/pico/Pico-R3-A4-Pinout.pdf).

## 3D Printed Enclosure/Plug

I've also designed a small enclosure to house the Pico and act as a plug for a switch controller! More details and files are available [here](https://www.thingiverse.com/thing:5823446)

<a href="https://www.thingiverse.com/thing:5823446">
  <img width="400" src="resources/N64%20Male%20Connector.jpg"/>
  <img width="400" src="resources/Pico%20Enclosure.jpg"/>
</a>

## Credits

This project would have taken a lot longer without the work done by everyone credited in the original [N64-Arduino-Switch](https://github.com/DavidPagels/n64-arduino-switch) project along with the creators and contributors of [TinyUsb](https://github.com/hathach/tinyusb), the [GP2040-CE](https://github.com/OpenStickCommunity/GP2040-CE) project, and the [MPG](https://github.com/OpenStickCommunity/MPG) project. If you're into developing this sort of stuff, definitely check their stuff out!
