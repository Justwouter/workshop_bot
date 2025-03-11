Welcome to the Acrobot Workshop bot

This is a small version of the [Acrobot](https://Acrobot.nl), a simple but expressive robot acrobat with 4 joints. It is easy to animate by using [Bottango](https://www.bottango.com/).

The project was developed by [Daniel Simu](https://danielsimu.com), with funding and support from [Creative Coding Utrecht](https://creativecodingutrecht.nl/).
Additional materials were sponsored by [DFRobot](https://www.dfrobot.com/) and [Fillamentum](https://fillamentum.com/)

# Required materials

For a workshop you need 3 things:
- Workshop bot
- ESP-NOW dongle
- Computer with Bottango software
  
Typically 2 - 3 participants can use 1 robot kit.

## Bottango

- The Bottango project file can be found in this repo.
- The ESP-NOW dongle is flashed with the BottangoArduinoDriver in parent mode, and connected to the computer running Bottango
- The ESP32 on the robot is running the BottangoArduinoDriver in child  mode

Make sure to update the child code with the mac address of the parent, and in Bottango enter the mac address of the child and the COM port in the hardware settings.

## Building the robot

3d model files, schematics and pcb gerber files are in the repo.

- The body consists of 2 3d printed halves, in PLA
- The shoulders, hips, hands and feet are best printed of TPU98A from [Fillamentum](https://fillamentum.com/)
- The motors are ideally 6Kg 300° Clutch Servos from [DFRobot](https://www.dfrobot.com/) 
- The ideal ESP32 for the robot is this [TTGO-T18](https://github.com/LilyGO/TTGO-T-Cell), which comes with a battery holder and a button. You can get these from [Tinytronics](https://www.tinytronics.nl/en/development-boards/microcontroller-boards/with-wi-fi/lilygo-ttgo-t-cell-esp32-with-3-way-button-and-18650-battery-holder)
  - I recommend squeezing a protected 18650 cell into the battery holder, the holder terminals may need to be bent to fit this.
- The dongle can be any standard ESP32, I use a Pico-d4 with a usb plug connector mounted on it.
- You can solder the electronics together on a perfboard, or fabricate the pcb.
- You will need 2 small screws to hold the ESP(2x6mm works well), and 3 M3 nuts and M3x30mm bolts to close the case.


# Todo

- Add pictures
- Write detailed instructions about how to fabricate and assemble the motors
- Write detailed BOM
- Write workshop instructions
- Improve robot design

# License

This project is shared under the Creative Commons Attribution 4.0 International license. 

We're open to consider other licenses that are better suited for open hardware design.
