#include "aeroTAPStereoCamera.h"

using namespace aeroTAP;

#include <stdio.h>

#include "aeroTAPLinuxCamera.h"

aeroTAPStereoCamera::aeroTAPStereoCamera(int colorWidth, int colorHeight, int depthWidth, int depthHeight, emCamPixelFormat fmt /* = emCamPixelFormatYUY2 */) :
	m_camera(NULL), m_depthWidth(depthWidth), m_depthHeight(depthHeight), m_DepthResolutions(NULL)
{
	m_imageWidth = colorWidth;
	m_imageHeight = colorHeight;

	try
	{
                m_camera = new aeroTAPLinux3DCamera(m_imageWidth, m_imageHeight, m_depthWidth, m_depthHeight);
	}
	catch(char* e)
	{
		throw e;
	}

}


aeroTAPStereoCamera::~aeroTAPStereoCamera(void)
{
	if(m_camera)
	{
		delete m_camera;
		m_camera = NULL;
	}
	m_DepthResolutions = NULL;
}

unsigned char* aeroTAPStereoCamera::getFrame(emCamStreamFormat trackType)
{
	return m_camera->getFrame(trackType);
}

void aeroTAPStereoCamera::updateFrame()
{
	m_camera->updateFrame();
}

int aeroTAPStereoCamera::getWidth(emCamStreamFormat trackType)
{
	return m_camera->getWidth(trackType);
}

int aeroTAPStereoCamera::getHeight(emCamStreamFormat trackType)
{
	return m_camera->getHeight(trackType);
}

inline list<emStreamInfo>* aeroTAPStereoCamera::getVideoResolutions(emCamStreamFormat trackType)
{
	return m_camera->getVideoResolutions(trackType);
}

