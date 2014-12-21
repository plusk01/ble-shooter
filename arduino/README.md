BLE Shooter Hardware
====================

Original BYU Competition - [Angry Engineers](http://www.et.byu.edu/~bmazzeo/LTR/2014/).

- 2 Servos
- 1 Ultrasonic Sensor (HC-SR04)
- Teensy 3.1
- Projectile Shooter (from an RC Helicopter)
- 1 BJT
- Vector board, header pins, and wire
- Adafruit Bluefruit LE module
- 9v Battery, with clip
- 5v Voltage Regulator

--------------------------------------------------------------

### C++ Code ###

Instead of using the vanilla Arduino IDE, I use the [Arduino-Makefile Project](https://github.com/sudar/Arduino-Makefile) so that I can write all the code in pure C++ in Sublime Text, and then use my terminal to `make` the binary and upload to the teensy.
