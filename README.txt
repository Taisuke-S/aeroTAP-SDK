//
//  aeroTAP SDK for Linux
//  2021.10.13
//
//

// 2021.1013 Update
- Now support aeroTAP 3D G2 camera
- Now detects USB mode ( 2.0 or 3.0 )
- Now detects appropriate /dev/video? for color image and depth image
- Now load ZDTable from camera


/lib
   /linux64  
	Ubuntu 16  x64
   /linux_aarch64		
	Dragonboard 410c, Nano Pi NEO3
   /linux_arm
	Raspberry Pi3,4
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




