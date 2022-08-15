#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <fstream>
#include "Sample.h"
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <GL/glut.h>
//#include "../../include/aeroTAP.h"
#include <fcntl.h>
#include <iostream>
#include <libudev.h>


using namespace std;
using namespace aerotap;
aeroTAPGLSample sample;


// Keyboard handler
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	// On Esc key press
	case 27:
	{
		sample.release();
		glutDestroyWindow(glutGetWindow());
		exit(EXIT_FAILURE);
	}

	default:
	{
		// Do nothing otherwise
		break;
	}
	}
}

void mouseClick(int button, int state, int x, int y)
{
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		sample.nextViewMode();
	}
}

// Update tracking data and visualize it
void display()
{
	// Delegate this action to example's main class
	bool update = sample.update();

	if (!update)
	{
		// End the work if update failed
		sample.release();
		glutDestroyWindow(glutGetWindow());
		exit(EXIT_FAILURE);
	}

	// Do flush and swap buffers to update viewport
	glFlush();
	glutSwapBuffers();
}

void idle()
{
	glutPostRedisplay();
}

void showHelpInfo()
{
	std::cout << "Usage: aeotap_RGB -v/dev/video0 -v/dev/video1 \n"
				 "Press Esc to close window." << std::endl;
	std::cout << "Options: -j MJPG,  -u uSB2.0, -f Enable Post Filter \n"
				 "Resolution: -r0 qvga, -r1 VGA, -r2 Wvga, -r3 HD\n" << std::endl;
}



int main(int argc, char* argv[])
{
	showHelpInfo();
	string video[2]={"",""};
	int nFilter = 0;
	bool bMJPG = false;
	bool bUSB20 = false;
	int res =1; //vga 
	int fps =30;
	// Prepare sample to work
	int c=0;
    for(int i=0;i<argc;++i)
    {
		if ( argv[i][0]=='-' || argv[i][0]=='/' )
		{
			if ( (argv[i][1]=='f' || argv[i][1]=='F') && ( argv[i][2]=='p' || argv[i][2]=='P' ) && ( argv[i][3]=='s' || argv[i][3]=='S' ) )
			{
					std::cout << argv[i] << std::endl;
				if ( strlen( &argv[i][4])>0 )
				{
					std::cout << argv[i][4] << std::endl;
					fps=atoi( &argv[i][4]);
					if ( fps<1 )
						fps =1;
				}
			}
			else
			if ( argv[i][1]=='f' || argv[i][1]=='F' )
				nFilter =1;
			else
			if ( argv[i][1]=='j' || argv[i][1]=='J' )
				bMJPG =true;
			else
			if ( argv[i][1]=='u' || argv[i][1]=='U' )
				bUSB20 =true;
			else
			if ( argv[i][1]=='r' || argv[i][1]=='R' )
			{
				if ( argv[i][2]=='0' )
					res=0;
				if ( argv[i][2]=='1' )
					res=1;
				if ( argv[i][2]=='2' )
					res=2;
				if ( argv[i][2]=='3' )
					res=3;
			}
			else
			if ( argv[i][1]=='v' )
			{	
				if (i>0 && c<2)
				{
					std::cout << "---- " << argv[i] << std::endl;
					video[c]=&argv[i][2];
					++c;
				}
			}
		}
		
	}
	if ( nFilter >0 )
		std::cout << "- Depth PostProcess Filter is enabled"<< std::endl;
	if ( bMJPG )
		std::cout << "- Use MJPG for color streaming"<< std::endl;
	

	sample.init(video[0],video[1],res, bMJPG,nFilter,bUSB20, fps);

	OutputMode outputMode = sample.getOutputMode();

	// Initialize GLUT window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(outputMode.xres, outputMode.yres);
	glutCreateWindow("aeroTAP Sample (aeroTAP API)");
	//glutSetCursor(GLUT_CURSOR_NONE);

	// Connect GLUT callbacks
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutMouseFunc(mouseClick);

	// Setup OpenGL
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);


	glOrtho(0, outputMode.xres, outputMode.yres, 0, -1.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	
	// Start main loop
	glutMainLoop();

	return 0;
}
