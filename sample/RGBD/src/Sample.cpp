#include <stddef.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <exception>
#include <unistd.h>
#include "Sample.h"
#include "ZDColorPalette.h"

using namespace aerotap;
using namespace std;

string _video0,_video1;
ZDColotPalette colorPalette;

aeroTAPGLSample::~aeroTAPGLSample()
{
	camera.stop();
}

bool aeroTAPGLSample::init(const std::string& video0,const std::string& video1,int res, bool bMJPG, int nFilter, bool bUSB20, int fps)
{
	_width=640;
	_height = 480;

	camera.setVerbose(true);

	if ( !camera.checkDevice() )
	{
		std::cout << "Error! no aeroTAP camera " << std::endl;
		return false;
	}
	_video0 = video0; 
	_video1 = video1; 
	camera.setFPS(fps);
	camera.useMJPG(bMJPG);
	camera.setUSB20(bUSB20);
	camera.setFilter(nFilter);
	int nPType = camera.getPType();
	switch (res)
	{
		case 0:
		_width=320;
		_height = 240;
			break;
		case 2:
		_width=640;
		_height = 400;
		if ( nPType ==3 ) // G2
		_height = 360;
			break;
		case 3:
		_width=1280;
		_height = 720;
			break;
	}
	bool ret = open();

	if ( ret == false )
	{
		std::cout << "Run with "<< video0 << " and " << video1 << std::endl;
		std::cout << "Error! opening camera " << std::endl;
		return false;
	}
	
	_width = camera.getResWidth();
	_height = camera.getResHeight();	

	_outputMode.xres = _width;
	_outputMode.yres = _height;
	
	_isConnectionLost = false;
	_isInitialized = false;
	return true;
}

bool aeroTAPGLSample::open()
{
	bool ret=true;
	if ( _video0.length() ==0 && _video1.length() ==0 )
	{
		std::cout << "Run with Depth and Color" << std::endl;
		ret =camera.open(NULL, NULL, _width, _height);
	}
	 else
	if ( _video0.length() ==0 && _video1.length() >0 )
	{
		std::cout << "Run with Depth only" << std::endl;
		ret =camera.open(NULL, (char*)_video1.c_str(), _width, _height);
	}
	 else
	if ( _video0.length() >0 && _video1.length() ==0 )
	{
		std::cout << "Run with color only" << std::endl;
		ret =camera.open((char*)_video0.c_str(), NULL, _width, _height);
	}
	 else
		ret =camera.open((char*)_video0.c_str(), (char*)_video1.c_str(), _width, _height);
	
	return ret;
}
bool aeroTAPGLSample::update()
{
	if (!_isInitialized)
	{
		// Create texture by DepthSensor output mode
		initTexture(_width, _height);
#if 0
		for( int i=0;i<2;++i)
		{
		  pColor[i] = (unsigned char*)malloc(1280*720*3); 
		  pDepthW[i] = (unsigned char*)malloc(1280*720*sizeof(uint16_t)); 
		  pDepthRAW[i] = (unsigned char*)malloc(1280*720*sizeof(uint16_t)); 
		}
		camera.setBuffers(pColor,pDepthW,pDepthRAW);
#endif
		try
		{
			camera.start();
			nZDTableLen = camera.getZDTable(zdTable);

		}
		catch (const exception& e)
		{
			release();
			exit(EXIT_FAILURE);
		}
		_isInitialized = true;
		return true;
	}
	if ( _isConnectionLost )
	{
		if ( camera.checkDevice() )
		{
			usleep(100*1000);
		std::cout << "re-openning camera" << std::endl;
			if ( open() )
			{
			_isConnectionLost = false;
				std::cout << "re-openning camera succeeded" << std::endl;
			}
			else
			{
				std::cout << "re-openning camera error" << std::endl;
			}
		}
		else
			usleep(100*1000);
		return true;
	}
	try
	{
		// Wait and update new frame
//		std::cout << "req new frame" << std::endl;
		if ( camera.isNewFrame() )
		{		
#if 0		
		std::cout << "new frame" << std::endl;
#else
//		std::cout << "-got frame" << std::endl;
			onNewRGBFrame( camera.getColorData() );
			onNewGrayFrame( camera.getGrayData() );

			onNewDepthFrame( camera.getDepthData() );
			renderTexture();
#endif
			camera.updateFrame();
		}
		else
		if ( camera.isConnectionLost() )
		{
			camera.stop();
			std::cout << "Camera Connection Lost Detected!" << std::endl;
			_isConnectionLost = true;
		}
		
	}
	catch (const exception& e)
	{
		// Update failed, negative result
		return false;
	}
	usleep(33*1000);
	
	return true;
}

void aeroTAPGLSample::release()
{	
	// Release aerotap and remove all modules
	try
	{
		camera.stop();
	}
	catch (const exception& e)
	{
//		std::cerr << "aerotap release failed (ExceptionType: " << e.type() << ")" << std::endl;
	}

	_isInitialized = false;
	// Free texture buffer
	if (_textureBuffer)
	{
		delete[] _textureBuffer;
		_textureBuffer = 0;
	}
#if 1	
		for( int i=0;i<2;++i)
		{
		  if (pColor[i] )
			free(pColor[i]);
		  if (pDepthW[i] )
			free(pDepthW[i]);
		  if (pDepthRAW[i] )
			free(pDepthRAW[i]);
		}
#endif	
}
// Copy depth frame data, received from camera, to texture to visualize
void aeroTAPGLSample::onNewDepthFrame(uint16_t *frame)
{
	if(_viewMode != DEPTH_SEGMENT_MODE)
		return;

	RGBQUAD	*pColorPalette= colorPalette.ColorPalette;

	uint8_t* texturePtr = _textureBuffer;
	const uint16_t* depthPtr = frame;
	
	for (size_t i = 0; i < _height; ++i)
	{		
		for (size_t j = 0; j < _width; ++j, texturePtr += 3)
		{
			uint16_t depthValue = *depthPtr;
			if ( depthValue<=10000 )
			{
				texturePtr[0] = pColorPalette[depthValue].rgbRed;
				texturePtr[1] = pColorPalette[depthValue].rgbGreen;
				texturePtr[2] = pColorPalette[depthValue].rgbBlue;
			}
			else
			{
//		std::cout << "depth data error" << depthValue << std::endl;
				
				texturePtr[0] = 0;
				texturePtr[1] = 0;
				texturePtr[2] = 0;
			}
			++depthPtr;
		}
	}
}

// Copy color frame data, received from camera, to texture to visualize
void aeroTAPGLSample::onNewRGBFrame(uint8_t*frame)
{
	if(_viewMode != RGB_MODE)
		return;

	uint8_t* texturePtr = _textureBuffer;
	uint8_t*colorPtr = frame;
	memcpy(texturePtr,colorPtr,_height*_width*3);
}
void aeroTAPGLSample::onNewGrayFrame(uint8_t*frame)
{
	if(_viewMode != GRAY_MODE)
		return;

	uint8_t* texturePtr = _textureBuffer;
	uint8_t*colorPtr = frame;
	memcpy(texturePtr,colorPtr,_height*_width);
}

// Render prepared background texture
void aeroTAPGLSample::renderTexture()
{
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glEnable(GL_TEXTURE_2D);
	glColor4f(1, 1, 1, 1);
	
	glBindTexture(GL_TEXTURE_2D, _textureID);
	if ( _viewMode == GRAY_MODE )
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _width, _height, GL_LUMINANCE, GL_UNSIGNED_BYTE, _textureBuffer);
	else
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _width, _height, GL_RGB, GL_UNSIGNED_BYTE, _textureBuffer);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0,GL_RGB, GL_UNSIGNED_BYTE, _textureBuffer);
#if 1	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glVertexPointer(2, GL_FLOAT, 0, _vertexes);
	glTexCoordPointer(2, GL_FLOAT, 0, _textureCoords);
	
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glDisable(GL_TEXTURE_2D);
    		
#endif	

}

void aeroTAPGLSample::initTexture(int width, int height)
{
	glGenTextures(1, &_textureID);
		
	if (_textureBuffer != 0)
		delete[] _textureBuffer;
	
	_textureBuffer = new uint8_t[width * height * 3];
	memset(_textureBuffer, 0, sizeof(uint8_t) * width * height * 3);
	
	glBindTexture(GL_TEXTURE_2D, _textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	// Set texture coordinates [0, 1] and vertexes position
	{
		_textureCoords[0] = (float) _width / width;
		_textureCoords[1] = (float) _height / height;
		_textureCoords[2] = (float) _width / width;
		_textureCoords[3] = 0.0;
		_textureCoords[4] = 0.0;
		_textureCoords[5] = 0.0;
		_textureCoords[6] = 0.0;
		_textureCoords[7] = (float) _height / height;
		
		_vertexes[0] = _width;
		_vertexes[1] = _height;
		_vertexes[2] = _width;
		_vertexes[3] = 0.0;
		_vertexes[4] = 0.0;
		_vertexes[5] = 0.0;
		_vertexes[6] = 0.0;
		_vertexes[7] = _height;
	}
}
