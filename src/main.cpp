#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
    ofGLWindowSettings glWindowSettings;
    glWindowSettings.width = 1280;
    glWindowSettings.height = 720;
    glWindowSettings.setGLVersion(3, 2);
    ofCreateWindow(glWindowSettings);

    // this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}
