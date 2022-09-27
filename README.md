# aeroTAP SDK for linux
//  2022.8.12
//  Copyright nextEDGE Technology

This is linux library for aeroTAP 3D USB Camera.


# Notes 

Python library requires Python version 3.8 and up
Set LD_LIBRAR_PATH to use library
i.e for Raspberry pi4
```
export LD_LIBRARY_PATH=/home/pi/aeroTAP/lib/linux_arm
```

# Requirements
```
sudo apt install libusb-dev
sudo apt install libudev-dev freeglut3-dev
sudo apt install libv4l-dev
sudo apt install libpng-dev libjpeg-dev

sudo apt install cmake
```


# Updates 
##2022.8.18
- Updated library for cam2web enhancement

## 2022.8.12
- Includes the latest library
- Added Python library
- Update sample source code

# Contents

/lib
   /linux64  
	Ubuntu 16  x64
   /linux_aarch64		
	Nano Pi, Dragonboard 410c
   /linux_arm.pi3
	Raspberry Pi3
   /linux_arm
	Raspberry Pi4
/include
   aeroTAP_SDK.h

/sample
   Sample code using OpenGL


# Trouble Shooting
1. Error with VIDIC_STREAMON error 28, No space left on device

sudo rmmod uvcvideo
sudo modprobe uvcvideo nodrop=1 quirks=128


2. USB3.0 port on ubuntu
use USB2.0 connector




