#pragma once

#include "ofMain.h"
#include "ofxHPGL.h"

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
        
    ofxHPGL plotter;
    ofPath path;

    enum TextAlignment {
        LEFT = 0,
        CENTER,
        RIGHT
    };
    
    enum TextVerticalAlignment {
        TOP = 0,
        MIDDLE,
        BOTTOM,
        BASELINE
    };
    
    ofTrueTypeFont plotFont;
    void plotText(string str, ofTrueTypeFont font, ofPoint pos=ofPoint(0,0), float rotation=0, TextAlignment alignment = LEFT, TextVerticalAlignment valign = BASELINE);

    ofPoint startPoint;
    ofPoint endPoint;
    float margin;
    float radius;
    float halfHeight;
};

class logEntry{
    
    
    
};

