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
    
    plotFont.load("machtgth.ttf", 20, true, true, true);

    plotter.setup( "/dev/tty.usbserial-FT5CHURVB" );
    
    // notice in the main.cpp file that the window dimensions are the same ratio as 11x17
    // if you want to change the input width or height ( defaults to window dimensions) //
    // be sure to call setInputWidth and setInputHeight to the width and height
    // of the input area that you will be using. The defaults are ofGetWidth() and ofGetHeight()
    // see the example below //
    int windowW = 1200;
    int windowH = (float)windowW * (float)(sqrt(2.0));
    ofSetWindowShape( windowH, windowW );
    plotter.setInputWidth( windowH );
    plotter.setInputHeight( windowW );
    plotter.setPenColor(1, ofColor::darkBlue);
    plotter.setPenColor(2, ofColor::black);
    plotter.enableCapture();
    plotter.setPen(1);
    //plotter.setPenVelocity(1.0);
    
    float height = plotter.getInputHeight();
    float width = plotter.getInputWidth();
    halfHeight = height * 0.5;
    radius = height / 6.;
    margin = height / 10.;
    startPoint.set(width-(margin+radius), halfHeight);
    endPoint.set(margin+radius, halfHeight);
    
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    plotter.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    ofBackground(255, 255);
    ofSetColor(0, 255);
    ofNoFill();
    plotFont.drawStringAsShapes(ofToString(ofGetFrameRate()), 20, 40);
    ofEnableBlendMode(OF_BLENDMODE_MULTIPLY);
    plotter.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if( key == 'c' ) {
        plotter.circle(ofRandom(ofGetWidth()), ofRandom(ofGetHeight()), ofRandom(2,500));
    }
    if(key == 'l'){
        float height = plotter.getInputHeight();
        float width = plotter.getInputWidth();
        float halfHeight = height * 0.5;
        float radius = height / 6.;
        float margin = height / 10.;
        
        plotter.setPen(1);
        
        plotter.line(margin+radius, halfHeight, width-(margin+radius), halfHeight);
        plotter.circle(margin+radius, halfHeight,radius);
        plotter.circle(width-(margin+radius), halfHeight,radius);
        
        for(int i = 0; i <= 60; i++) {
            float lineWidth = (width - (2*(margin+radius)));
            float step = lineWidth / (60.0);
            float x = margin+radius+(step*i);
            plotter.setPen(1);
            plotter.line(x, halfHeight-10, x, halfHeight+10);
            
            auto chars = plotFont.getStringAsPoints(ofToString(i));
            float numberwidth = plotFont.getStringBoundingBox(ofToString(i), 0, 0).width;
            plotter.setPen(2);
            for (auto c : chars){
                c.rotate(-90, ofVec3f(0,0,1));
                c.translate(ofPoint(x-(numberwidth*0.5), halfHeight+30));
                auto o = c.getOutline();
                for (auto p : o){
                    p.simplify(0.05);
                    plotter.polyline(p);
                }
            }
            
        }
    }
    if(key == 't') {
        plotter.clear();
        plotter.enableCapture();
        float x = plotter.getInputWidth()/4.0;
        float y = 200;
        plotter.setPen(2);
        plotter.line(x-10,y, x+10, y);
        plotter.line(x,y-10, x, y+10);
        plotter.circle(x, y, 5);
        plotText("this text is left baseline", plotFont, ofPoint(x,y), 180);
        plotter.setPen(1);
        plotText("this text is left baseline", plotFont, ofPoint(x,y));
        y+=300;
        plotter.setPen(2);
        plotter.line(x-10,y, x+10, y);
        plotter.line(x,y-10, x, y+10);
        plotter.circle(x, y, 5);
        plotText("this text is center baseline", plotFont, ofPoint(x,y), 180,CENTER);
        plotter.setPen(1);
        plotText("this text is center baseline", plotFont, ofPoint(x,y), 0,CENTER);
        y+=300;
        plotter.setPen(2);
        plotter.line(x-10,y, x+10, y);
        plotter.line(x,y-10, x, y+10);
        plotter.circle(x, y, 5);
        plotText("this text is right baseline", plotFont, ofPoint(x,y), 180,RIGHT);
        plotter.setPen(1);
        plotText("this text is right baseline", plotFont, ofPoint(x,y), 0,RIGHT);
        x*=3;
        y=200;
        plotter.setPen(2);
        plotter.line(x-10,y, x+10, y);
        plotter.line(x,y-10, x, y+10);
        plotter.circle(x, y, 5);
        plotText("This text is left top", plotFont, ofPoint(x,y), 180,LEFT, TOP);
        plotter.setPen(1);
        plotText("This text is left top", plotFont, ofPoint(x,y), 0,LEFT, TOP);
        y+=300;
        plotter.setPen(2);
        plotter.line(x-10,y, x+10, y);
        plotter.line(x,y-10, x, y+10);
        plotter.circle(x, y, 5);
        plotText("This text is center middle", plotFont, ofPoint(x,y), 180,CENTER, MIDDLE);
        plotter.setPen(1);
        plotText("This text is center middle", plotFont, ofPoint(x,y), 0,CENTER, MIDDLE);
        y+=300;
        plotter.setPen(2);
        plotter.line(x-10,y, x+10, y);
        plotter.line(x,y-10, x, y+10);
        plotter.circle(x, y, 5);
        plotText("this text is right bottom", plotFont, ofPoint(x,y), 180,RIGHT, BOTTOM);
        plotter.setPen(1);
        plotText("this text is right bottom", plotFont, ofPoint(x,y), 0,RIGHT, BOTTOM);

    }
    if ( key == 'n') {
        float r = margin/3.0;
        float c = r/10.0;
        float x = plotter.getInputWidth()-(margin/1.5);
        float y = halfHeight;
        plotter.circle(x, y, r);
        plotter.circle(x,y,r/10.0);
        plotter.line(x, y-c, x+r, y);
        plotter.line(x, y+c, x+r, y);
        plotter.line(x, y-c, x-r, y);
        plotter.line(x, y+c, x-r, y);
    }
    if( key == 'p' ) {
        plotter.print();
    }

}

void ofApp::plotText(string str, ofTrueTypeFont font, ofPoint pos, float rotation, TextAlignment alignment, TextVerticalAlignment valign){

    float mHeight = font.getStringBoundingBox("m", 0, 0).height;
    auto chars = font.getStringAsPoints(str);
    auto bb = font.getStringBoundingBox(str, 0, 0);
    for (auto c : chars){
        if(alignment == LEFT){
            // noting to be done
        } else if(alignment == CENTER) {
            c.translate(ofPoint(-bb.width/2.0, 0));
        } else if(alignment == RIGHT){
            c.translate(ofPoint(-bb.width, 0));
        }
        if(valign == TOP){
            c.translate(ofPoint(0, font.getSize()));
        } else if (valign == MIDDLE){
            c.translate(ofPoint(0, mHeight/2.0));
        } else if (valign == BOTTOM){
            c.translate(ofPoint(0, font.getDescenderHeight()));
        } else if (valign == BASELINE){
         // nothing to be done
        }
        
        c.rotate(rotation, ofVec3f(0,0,1));
        
        c.translate(pos);

        auto o = c.getOutline();
        for (auto p : o){
            p.simplify(0.05);
            plotter.polyline(p);
        }
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
