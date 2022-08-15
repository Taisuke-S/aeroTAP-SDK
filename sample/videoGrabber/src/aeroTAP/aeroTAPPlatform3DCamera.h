#pragma once

#include "aeroTAPCamera.h"

namespace aeroTAP
{

class aeroTAPPlatform3DCamera
{
protected:
	aeroTAPPlatform3DCamera(int colorWidth, int colorHeight, int depthWidth, int depthHeight);

public:
	virtual ~aeroTAPPlatform3DCamera(void);
	virtual unsigned char* getFrame(emCamStreamFormat&) = 0;
	virtual void updateFrame() = 0;
	virtual int getWidth(emCamStreamFormat&) = 0;
	virtual int getHeight(emCamStreamFormat&) = 0;
	virtual list<emStreamInfo>* getVideoResolutions(emCamStreamFormat&);

protected:
	unsigned char *m_colorRaw;
	unsigned char *m_depthRaw;

	int m_colorWidth;
	int m_colorHeight;
	int m_colorFrameSize;

	int m_depthWidth;
	int m_depthHeight;
	int m_depthFrameSize;

	list<emStreamInfo> m_ColorResolutions;
	list<emStreamInfo> m_DepthResolutions;
};

};
