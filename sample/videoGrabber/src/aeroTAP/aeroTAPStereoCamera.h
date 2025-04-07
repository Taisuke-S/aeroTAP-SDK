#pragma once

#include "aeroTAPCamera.h"
#include "aeroTAPPlatform3DCamera.h"

namespace aeroTAP
{

class aeroTAPStereoCamera : public aeroTAPCamera
{
public:
	aeroTAPStereoCamera(int colorWidth, int colorHeight, int depthWidth, int depthHeight, emCamPixelFormat fmt = emCamPixelFormatYUY2);
	virtual ~aeroTAPStereoCamera(void);

	unsigned char* getFrame(emCamStreamFormat);
	void updateFrame();
	int getWidth(emCamStreamFormat);
	int getHeight(emCamStreamFormat);
	list<emStreamInfo>* getVideoResolutions(emCamStreamFormat);

protected:
	aeroTAPPlatform3DCamera *m_camera;
	int m_depthWidth;
	int m_depthHeight;

	list<emStreamInfo> *m_DepthResolutions;
};

};
