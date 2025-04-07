#pragma once

#include <list>
using namespace std;

namespace aeroTAP
{
enum emCamPixelFormat
{
	emCamPixelFormatUnknown = 0, 
	emCamPixelFormatYUY2 = 1, 
};

enum emCamStreamFormat
{
	emCamStreamFormatUnknown = 0, 
	emCamStreamFormatColor, 
	emCamStreamFormatDepth, 
	emCamStreamFormatInfrad, 
	emCamStreamFormatSkeleton, 
};

enum emCamStreamCodecFormat
{
	emCamStreamCodec_Unknown = 0, 
	emCamStreamCodec_YUY2, 
	emCamStreamCodec_MJPG, 
};

struct emStreamInfo
{
	int width;
	int height;
	emCamStreamCodecFormat codec;
};

class aeroTAPCamera
{
protected:
	aeroTAPCamera(void);

	int m_imageWidth;
	int m_imageHeight;
	list<emStreamInfo> *m_ColorResolutions;

public:
	virtual ~aeroTAPCamera(void);

	virtual unsigned char* getFrame(emCamStreamFormat) = 0;
	virtual void updateFrame() = 0;
	virtual int getWidth(emCamStreamFormat) = 0;
	virtual int getHeight(emCamStreamFormat) = 0;
	virtual list<emStreamInfo>* getVideoResolutions(emCamStreamFormat) = 0;
};

//	aeroTAPCamera* CreateAEROCamera(aeroTAPCameraName);
aeroTAPCamera* CreateAEROStereoCamera(int ColorWidth, int ColorHeight, int DepthWidth, int DepthHeight, 
	emCamPixelFormat pixelFormat = emCamPixelFormatYUY2);

};
