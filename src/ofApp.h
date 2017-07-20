#pragma once

#include "ofMain.h"
#include "ofxHPGL.h"
#include "ofxGeo.h"
#include "ofxTime.h"
#include "ofxGeographicLib.h"
#include "ofxImGui.h"
#include "ofxJSONRPC.h"

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

class Location{
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
    Location source;
    Location destination;
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
    
    void setupGui();
    bool loadLighthouses(string xmlFilePath);
    
    // LIGHTHOUSE DATA
    
    ofXml lighthouseXml;
    
    vector<Location> locations;
    vector<LogEntry> log;

    Location lighthouse;

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
    
    // PLOTTER
    
    ofxHPGL plotter;
    
    ofTrueTypeFont plotFont;
    
    void plotText(string str, ofTrueTypeFont font, ofPoint pos=ofPoint(0,0), float size=20, float rotation=0, TextAlignment alignment = LEFT, TextVerticalAlignment valign = BASELINE);

    ofPoint startPoint;
    ofPoint endPoint;
    float margin;
    float radius;
    float halfHeight;
    
    // GUI
    
    ofxImGui::Gui gui;
    int guiColumnWidth = 250;
    
    // SERVER
    
    // We use a mutex to protect any variables that can be
    // modified by multiple clients.  In our case, userText must be protected.
    // We mark the mutex as mutable so that it can be used in const functions.
    mutable std::mutex mutex;


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

