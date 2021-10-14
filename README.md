//
//  aeroTAP SDK for Linux
//  2019.12.23
//
//

// Update



/lib
   /linux64  
	Ubuntu 16  x64
   /linux_aarch64		
	Dragonboard 410c
   /linux_arm
	Raspberry Pi3,4
/include
   aeroTAP_SDK.h

/sample
   Sample code using OpenGL ( not support Raspberry Pi4 )


// Truble Shooting
1. Error with VIDIC_STREAMON error 28, No space left on device

sudo rmmod uvcvideo
sudo modprobe uvcvideo nodrop=1 quirks=128


2. USB3.0port on ubuntu
use USB2.0 connector




