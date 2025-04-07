//
//  aeroTAP SDK for Linux
//  2022.8.12
//  Copyright nextEDGE Technology
//

//
// Notes 
//
Python library requires Python version 3.8 and up
Set LD_LIBRAR_PATH to use library
i.e for Raspberry pi
export LD_LIBRARY_PATH=/home/pi/aeroTAP/lib/linux_arm

echo "/home/pi/aeroTAP/lib/linux_aarch64" | sudo tee /etc/ld.so.conf.d/aeroTAP-ask.conf
sudo ldconfig
//
// Requirements
//
sudo apt install libusb-dev
sudo apt install libudev-dev freeglut3-dev
sudo apt install libv4l-dev
sudo apt install libpng-dev libjpeg-dev

sudo apt install cmake

// check device
v4l2-ctl --list-formats -d /dev/video0

# Python requirements
pip install opencv-python opencv-contrib-python numpy
pip install --upgrade scipy


//
// Updates 
//
#2025.4.07
- Enable OpenMP
- Fixed Open issue with raspberry Pi4 with GS Camera
- Sample updated
  added lib for Raspi5, raspi4 64bit to aarch64
#2024.5.11
- Added Etron Camera Support
- Added getSN Serian Number 
#2022.8.12
- Includes the latest library
- Added Python library
- Update sample source code

//
// Contents
//

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


// Truble Shooting
1. Error with VIDIC_STREAMON error 28, No space left on device

sudo rmmod uvcvideo
sudo modprobe uvcvideo nodrop=1 quirks=128


2. USB3.0port on ubuntu
use USB2.0 connector




