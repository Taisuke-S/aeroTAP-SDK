#pragma once

#include "aeroTAPCamera.h"
#include "aeroTAPPlatform3DCamera.h"

#include "ofxUvcCamera.h"

namespace aeroTAP
{

class aeroTAPLinux3DCamera : public aeroTAPPlatform3DCamera
{
public:
	aeroTAPLinux3DCamera(int colorWidth, int colorHeight, int depthWidth, int depthHeight);
	virtual ~aeroTAPLinux3DCamera(void);

	unsigned char* getFrame(emCamStreamFormat&);
	void updateFrame();
	int getWidth(emCamStreamFormat&);
	int getHeight(emCamStreamFormat&);

protected:
	ofxUvcCamera *m_hCamera1;
	ofxUvcCamera *m_hCamera2;
	bool m_bUpdateFrameData;
};

};

