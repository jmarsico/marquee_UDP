#include "ofMain.h"
#include "testApp.h"
#include "ofAppGlutWindow.h"
//#ifdef TARGET_OPENGLES
//#include "ofGLProgrammableRenderer.h"
//#endif
//========================================================================
int main( ){
	ofAppGlutWindow window;

    ofSetupOpenGL(&window,1280,768,OF_WINDOW);			// <-------- setup the GL context
    

    //ofSetupOpenGL(1280,700,OF_WINDOW);
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new testApp());

}
