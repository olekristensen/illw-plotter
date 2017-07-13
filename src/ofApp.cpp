#include "ofApp.h"

/*
 
 Wiring using Roline dsub9 female to dsub 25 male
 
 Roline:
 
    dsub  9 - dsub 25
          1 - 8
          2 - 3
          3 - 2
          4 - 20
          5 - 7
          6 - 6
          7 - 4
          8 - 5
          9 - 22
 
 9 pin converter cable for Roline:
   
              Roline
      plotter - V - cable  - serial port
           20 - 4 - blue   - 1
            2 - 3 - yellow - 2
            3 - 2 - red    - 3
            5 - 8 - brown  - 4
            8 - 1 - green  - 4
            7 - 5 - purple - 5
            4 - 7 - black  - 6
            6 - 6 - orange - 7
           20 - 4 - grey   - 8
                    NC     - 9
 
 */




//--------------------------------------------------------------
void ofApp::setup(){
    
    testFont.load("machtgth.ttf", 8, true, true, true);

    hp.setup( "/dev/tty.usbserial-FT5CHURVB" );
    
    // notice in the main.cpp file that the window dimensions are the same ratio as 11x17
    // if you want to change the input width or height ( defaults to window dimensions) //
    // be sure to call setInputWidth and setInputHeight to the width and height
    // of the input area that you will be using. The defaults are ofGetWidth() and ofGetHeight()
    // see the example below //
    int windowW = 1200;
    int windowH = (float)windowW * (float)(sqrt(2.0));
    ofSetWindowShape( windowH, windowW );
    hp.setInputWidth( windowH );
    hp.setInputHeight( windowW );
    hp.enableCapture();
    hp.setPen(1);
    //hp.setPenVelocity(1.0);
    ofSetVerticalSync(false);
    ofSetFrameRate(120);
    
}

//--------------------------------------------------------------
void ofApp::update(){
    hp.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofSetColor(0, 255);
    testFont.drawString(ofToString(ofGetFrameRate()), 20, 40);
    
    ofPushMatrix();
    hp.draw();
    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if( key == 'c' ) {
        hp.circle(ofRandom(ofGetWidth()), ofRandom(ofGetHeight()), ofRandom(2,500));
    }
    if(key == 'l'){
        float height = hp.getInputHeight();
        float width = hp.getInputWidth();
        float halfHeight = height * 0.5;
        float radius = height / 6.;
        float margin = height / 10.;
        /*
        hp.setPen(1);
        
        hp.line(margin+radius, halfHeight, width-(margin+radius), halfHeight);
        hp.circle(margin+radius, halfHeight,radius);
        hp.circle(width-(margin+radius), halfHeight,radius);
*/
        
        for(int i = 0; i <= 60; i++) {
            float lineWidth = (width - (2*(margin+radius)));
            float step = lineWidth / (60.0);
            float x = margin+radius+(step*i);
            //hp.setPen(1);
            //hp.line(x, halfHeight-10, x, halfHeight+10);
            
            auto chars = testFont.getStringAsPoints(ofToString(i));
            float numberwidth = testFont.getStringBoundingBox(ofToString(i), 0, 0).width;
            hp.setPen(2);
            for (auto c : chars){
                c.rotate(-90, ofVec3f(0,0,1));
                c.translate(ofPoint(x-(numberwidth*0.5), halfHeight+30));
                auto o = c.getOutline();
                for (auto p : o){
                    p.simplify(0.05);
                    hp.polyline(p);
                }
            }
            
        }
    }
    if( key == 'p' ) {
        hp.print();
    }

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
