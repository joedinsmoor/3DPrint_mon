# 3DPrint_mon

3DPrint_mon is a monitoring program for use in conjunction with a 3d printer, a usb webcam, an I2C temperature sensor, and a raspberry pi. Its main function is to monitor bed temperature as well as capture images at any point during a print.  

## Dependencies
- OpenCV
- WiringPi
- WiringPiI2C

## Functionality
3dPrint_mon starts by initializing both the I2C temperature sensor, as well as the OpenCV interface. It then reads in the current temperature detected by the temperature sensor. Once everything has started, it outputs a menu using a switch function. The switch function has four options. 
- Option 1 will return the current bed temperature in degrees Fahrenheit, at the time the option is selected. 
- Option 2 will capture a realtime image of the print bed, outputting to a filename of the user's choice. If no filename is entered, the default will be image.png. 
- Option 3 is automatic print monitoring. Once a print is started, you will enter the estimated time given by the printer, in seconds. The program will then take that time, capture a picture of the initial layer of the print, and also take a picture of the print partway through. 
- Option 4 is used to exit the program.

## Credits
Joseph Dinsmoor
- EGRE 347
- Spring 2022
- Professor Klenke