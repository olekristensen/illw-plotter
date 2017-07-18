#pragma once

#include "ofMain.h"
#include "ofxHPGL.h"
#include "ofxGeo.h"
#include "ofxTime.h"
#include "ofxGeographicLib.h"
#include "ofxImGui.h"

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))


class Lighthouse{
public:
    string name;
    string country;
    string dxcc;
    string continent;
    string illw;
    ofxGeo::Coordinate coordinate;
};

class LogEntry{
public:
    Lighthouse source;
    Lighthouse destination;
    string notes;
    Poco::DateTime timestamp;
};


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
    
    ofXml lighthouseXml;
    
    Lighthouse location;
    vector<Lighthouse> lighthouses;
    vector<LogEntry> log;

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
    void plotText(string str, ofTrueTypeFont font, ofPoint pos=ofPoint(0,0), float size=20, float rotation=0, TextAlignment alignment = LEFT, TextVerticalAlignment valign = BASELINE);

    ofPoint startPoint;
    ofPoint endPoint;
    float margin;
    float radius;
    float halfHeight;
    
    ofxImGui::Gui gui;
    int guiColumnWidth = 250;

};

class GuiTheme : public ofxImGui::BaseTheme
{
public:
    
    GuiTheme()
    {
        col_main_text = ofColor(ofFloatColor(0.3,0.3,0.2));
        col_main_head = ofColor(64,255,0);
        col_main_area = ofColor(245,244,245);
        col_win_popup = ofColor::black;
        col_win_backg = ofColor(245,244,245,33);
        
        setup();
        
    }
    
    void setup()
    {
        
    }
    
};

