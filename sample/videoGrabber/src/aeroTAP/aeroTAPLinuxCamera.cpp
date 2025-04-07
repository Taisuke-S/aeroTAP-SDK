
#include "aeroTAPCamera.h"
#include "aeroTAPPlatform3DCamera.h"
#include "aeroTAPLinuxCamera.h"
#include <stdio.h>

using namespace aeroTAP;

#include <exception>
using namespace std;

aeroTAPLinux3DCamera::aeroTAPLinux3DCamera(int colorWidth, int colorHeight, int depthWidth, int depthHeight) :
        aeroTAPPlatform3DCamera(colorWidth, colorHeight, depthWidth, depthHeight),
	m_hCamera1(NULL), m_hCamera2(NULL), m_bUpdateFrameData(false) 
{
	try
	{
                //m_hCamera2 = new ofxUvcCamera("/dev/video1", depthWidth, depthHeight, IO_METHOD_USERPTR);
                m_hCamera2 = new ofxUvcCamera("/dev/video1", depthWidth, depthHeight);

	}
	catch(...)
	{
		throw "Init depth camera";
	}

	m_depthWidth = m_hCamera2->getWidth();
	m_depthHeight = m_hCamera2->getHeight();

	try
	{
		if (colorWidth!=0 && colorHeight!=0)
			m_hCamera1 = new ofxUvcCamera("/dev/video0", colorWidth, colorHeight);
		else 
			m_hCamera1 = new ofxUvcCamera("/dev/video0", m_depthWidth*2, m_depthHeight);
	}
	catch(...)
	{
		throw "Init color camera";
	}

	m_colorWidth = m_hCamera1->getWidth();
	m_colorHeight = m_hCamera1->getHeight();

	m_hCamera1->start_capturing();
	m_hCamera2->start_capturing();
}


aeroTAPLinux3DCamera::~aeroTAPLinux3DCamera(void)
{
	if(m_hCamera1)
	{
		m_hCamera1->stop_capturing();
		delete m_hCamera1;
		m_hCamera1 = NULL;
	}
	if(m_hCamera2)
	{
		m_hCamera2->stop_capturing();
		delete m_hCamera2;
		m_hCamera2 = NULL;
	}
}

inline unsigned char* aeroTAPLinux3DCamera::getFrame(emCamStreamFormat& trackType)
{
	if(!m_bUpdateFrameData)
		return NULL;

	return trackType==emCamStreamFormatColor ?  m_colorRaw : 
		trackType==emCamStreamFormatDepth ? m_depthRaw : NULL;
}

inline void aeroTAPLinux3DCamera::updateFrame()
{
	buffer *color, *depth;
	//printf(" [Debug] Linux3DCamera->updateFrame ... ");
	//if(m_bUpdateFrameData = (((color = m_hCamera1->read_frame())!=NULL) && ((depth = m_hCamera2->read_frame())!=NULL)))
	color = m_hCamera1->read_frame();
	depth = m_hCamera2->read_frame();

	//printf("color: %p, depth: %p\n", color, depth);
	if(m_bUpdateFrameData=(color && depth))
	{
		//printf("ready\n");
		m_colorRaw = (unsigned char*)color->start;
		m_depthRaw = (unsigned char*)depth->start;
	}
	//else
	//	printf("not ready\n");
}

inline int aeroTAPLinux3DCamera::getWidth(emCamStreamFormat& trackType)
{
	if(trackType == emCamStreamFormatColor)
		return m_colorWidth;
	else if(trackType == emCamStreamFormatDepth)
		return m_depthWidth;
	else 
	{
		printf("Unsupported StreamFormat for querying video width\n");
		return -1;
	}
}

inline int aeroTAPLinux3DCamera::getHeight(emCamStreamFormat& trackType)
{
	if(trackType == emCamStreamFormatColor)
		return m_colorHeight;
	else if(trackType == emCamStreamFormatDepth)
		return m_depthHeight;
	else 
	{
		printf("Unsupported StreamFormat for querying video height\n");
		return -1;
	}
}

