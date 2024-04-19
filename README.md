# Kodi-Remote-Pilot
The aim is to create a physical pilot which will be operating Kodi server as custom BT keyboard.

# Why?
I absolutely hate missing the keys on a smartphone when looking at a monitor - no tactical feedback, basically unusable for me, thus I need smth what I can use without looking at it.
So the idea of a custom tiny physical keyboard appeared.

# Hardware
The schematics looks like this (image below):
![schematics](kodi_pilot.png)

List of parts needed:
* 1 ESP32 C3 Super Mini (or similar)
* 2 momentary switches
* 1 bistable switch (turn on-off)
* 1 battery 4.2V
* Wires ([AWG28](https://www.powerstream.com/Wire_Size.htm) will be absolutely enough)
* 1 USB charging module for the battery (like [this](https://botland.store/charger-modules-for-li-po-batteries/6944-lipol-charger-tp4056-1s-37v-microusb-with-5904422375355.html))
* Rotary Encoder module (I used [DFRobot SEN0502](https://wiki.dfrobot.com/SKU_SEN0502_Rotary_Encoder_Module_I2C))
* Arduino-compatible Joystick (I used [Iduino ST1079](https://botland.store/joystick/1942-thumb-joystick-5904422356231.html))

Connect according to schematics and put into some nice plastic box

# Software

* The ["ino" file](kodi_remote.ino) in the repo - use Arduino IDE (I used v. 2.3.2 on Linux for reference).
* [DFRobot_VisualRotaryEncoder Library](https://github.com/DFRobot/DFRobot_VisualRotaryEncoder)
* [BleKeyboard library](https://github.com/oden-umaru/ESP32C3-BLE-Keyboard)
* [NimBLE-Arduino library](https://github.com/h2zero/NimBLE-Arduino)

# Result

![View 1](result1.png)
![View 2](result2.png)
![View 3](result3.png)
