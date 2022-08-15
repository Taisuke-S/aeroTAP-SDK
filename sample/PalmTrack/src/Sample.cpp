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
	switch (res)
	{
		case 0:
		_width=320;
		_height = 240;
			break;
		case 2:
		_width=640;
		_height = 400;
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
	camera.enablePalmTracker(true);
	camera.setPalmDetectMode(HANDMODE_CLICK);
	
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
		std::cout << "ZDTableLen = " << nZDTableLen << std::endl;

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
			onNewQVGAFrame( camera.getQVGAData() );
			onNewDepthFrame( camera.getDepthData() );
			onNewDepthSmallFrame( camera.getDepthSmallData() );
			renderTexture();

			doPalmTracking();
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
		
		HANDRESULT result[2];
		if ( camera.getPalmTrackResult(result) )
		{
			for(int i=0;i<2;++i)
			{
				if ( !result[i].bUsed )
					continue;
				bool mode = result[i].nObjectType>0?true:false;
				if ( mode )
							std::cout << "Camera detects Close Hand!"  << std::endl;
				else
							std::cout << "Camera detects Hand!" <<  std::endl;
				
				int x = result[i].rect.left;
				int y = result[i].rect.top;
				int w = result[i].rect.right-result[i].rect.left;
				int h = result[i].rect.bottom - result[i].rect.top;

				std::cout << "x: " << result[i].rect.left << ", y: " << result[i].rect.top << ", right: " << result[i].rect.right<< ", bottom: " << result[i].rect.bottom<<   std::endl;
				std::cout << "x: " << result[i].pCenter.x << ", y: " << result[i].pCenter.y << ", z: " << result[i].pCenter.z<<   std::endl;
				renderHand( x, y, w, h, mode);
			}
		}
		else
		{
			std::cout << "...no Hand" <<  std::endl;
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
void aeroTAPGLSample::doPalmTracking()
{
	camera.doPalmTracking();
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
void aeroTAPGLSample::onNewDepthSmallFrame(uint16_t *frame)
{
	if(_viewMode != DEPTHSMALL_SEGMENT_MODE)
		return;

	RGBQUAD	*pColorPalette= colorPalette.ColorPalette;

	uint8_t* texturePtr = _textureBuffer;
	const uint16_t* depthPtr = frame;
	float scaleX =(float)_width/80;
	float scaleY =(float)_height/60;
	
	for (size_t i = 0; i < _height; ++i)
	{		
		for (size_t j = 0; j < _width; ++j, texturePtr += 3)
		{
			int p=(int)(i/scaleY)*80+j/scaleX;
			uint16_t depthValue = frame[p];
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
void aeroTAPGLSample::onNewQVGAFrame(uint8_t*frame)
{
	if(_viewMode != GRAYQVGA_MODE)
		return;

	uint8_t* texturePtr = _textureBuffer;
	uint8_t*colorPtr = frame;
	int p=0;
	float scaleX =(float)_width/320;
	float scaleY =(float)_height/240;
	for(int y=0;y<_height;++y)
	{
		for(int x=0;x<_width;++x)
		{
			int ps = (int)(y/scaleY)*_width/scaleX+x/scaleX;
			texturePtr[p] = colorPtr[ps];
			++p;
		}
	}
}

// Render prepared background texture
void aeroTAPGLSample::renderHand(int x, int y, int w, int h, bool mode)
{
#if 1
//    glMatrixMode( GL_PROJECTION );
//    glLoadIdentity();
    glOrtho( 0, _width, _height, 0, -1, 1);
    
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

//	glTranslatef( 0.5, 0.5, 0 );
	if ( mode )
		glColor3ub( 0, 255, 255 );
	else
		glColor3ub( 255, 0, 0 );
    glBegin(GL_LINE_LOOP);
    glVertex2f( x, y );
    glVertex2f( x+w, y );
    glVertex2f( x+w, y+h );
    glVertex2f( x, y+h );
    glEnd();
#endif
	
}
void aeroTAPGLSample::renderTexture()
{
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glEnable(GL_TEXTURE_2D);
	glColor4f(1, 1, 1, 1);
	
	glBindTexture(GL_TEXTURE_2D, _textureID);
	if ( _viewMode == GRAY_MODE || _viewMode == GRAYQVGA_MODE )
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
