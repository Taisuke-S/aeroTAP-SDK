#include "aeroTAPPlatform3DCamera.h"

#include <stdio.h>

using namespace aeroTAP;

aeroTAPPlatform3DCamera::aeroTAPPlatform3DCamera
	(int colorWidth, int colorHeight, int depthWidth, int depthHeight) : 
	m_colorWidth(colorWidth), m_colorHeight(colorHeight), 
	m_depthWidth(depthWidth), m_depthHeight(depthHeight), 
	m_colorRaw(NULL), m_depthRaw(NULL)
{
	try
	{
		m_colorRaw = new unsigned char[m_colorHeight * m_colorWidth << 2];
		m_depthRaw = new unsigned char[m_depthHeight * m_depthWidth << 2];
	}
	catch(int e)
	{
		printf("Memory allocation for %s\n", m_colorRaw ? "Color Stream" : "Depth Stream");
		throw e;
	}
}


aeroTAPPlatform3DCamera::~aeroTAPPlatform3DCamera(void)
{
	if(m_colorRaw)
	{
		delete [] m_colorRaw;
		m_colorRaw = NULL;
	}

	if(m_depthRaw)
	{
		delete [] m_depthRaw;
		m_depthRaw = NULL;
	}

	m_ColorResolutions.clear();
	m_DepthResolutions.clear();
}

list<emStreamInfo>* aeroTAPPlatform3DCamera::getVideoResolutions(emCamStreamFormat& trackType)
{
	if(trackType == emCamStreamFormatColor)
		return &m_ColorResolutions;
	else if(trackType == emCamStreamFormatDepth)
		return &m_DepthResolutions;
	else
	{
		printf("Unsupported StreamFormat for Video resolutions\n");
		return NULL;
	}
}
