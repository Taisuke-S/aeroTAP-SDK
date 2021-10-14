#ifndef AEROTAPSAMPLE_H_
#define AEROTAPSAMPLE_H_

#include <../include/aeroTAP_SDK.h>

#ifdef _WIN32
#include <Windows.h>
#endif

#ifndef ANDROID
#include <GL/gl.h>
#else
#include <GLES/gl.h>
#endif

#include <string>
using namespace aerotap;

typedef enum
{
	DEPTH_SEGMENT_MODE = 0,
	RGB_MODE = 1,
	GRAY_MODE,
	MODES_MAX_COUNT
} ViewMode;


// Main class of the sample
class aeroTAPGLSample
{
public:
	aeroTAPGLSample() {
		_isInitialized=false;
		_modesNumber = MODES_MAX_COUNT;
		for( int i=0;i<2;++i)
		{
		  pColor[i] = pDepthW[i] = pDepthRAW[i] = NULL; 
		}
	};
	~aeroTAPGLSample();
	
	bool init(const std::string& video0,const std::string& video1,int res, bool bMJPG, int nfilter,bool bUSB20, int fps);
	bool open();
	void onNewDepthFrame(uint16_t *frame);
	void onNewRGBFrame(uint8_t*frame);
	void onNewGrayFrame(uint8_t*frame);

	OutputMode getOutputMode() const
	{
		return _outputMode;
	}
	
	bool update();
	void doHandDetect();
	
	// Release all sample resources
	void release();

	void nextViewMode()
	{
		_viewMode = (ViewMode)(((uint32_t)_viewMode + 1) % _modesNumber);
	}

private:
	int _width, _height;
	OutputMode _outputMode;
	
	aeroTAP camera;
	// GL data
	GLuint _textureID;
	uint8_t* _textureBuffer;
	GLfloat _textureCoords[8];
	GLfloat _vertexes[8];
	
	ViewMode _viewMode;
	uint32_t _modesNumber;
	bool _isInitialized;
	bool _isConnectionLost;

// Sample if use UserBuffer
	unsigned char *pColor[2];
	unsigned char *pDepthW[2];
	unsigned char *pDepthRAW[2];

	uint16_t zdTable[2048];
	uint16_t nZDTableLen;
	/**
	 * Draw methods
	 */
	void renderTexture();
	void renderLines();
	void renderHand(int x, int y, int w, int h);
	
	void initTexture(int width, int height);
};

#endif /* AEROTAPGLSAMPLE_H_ */
