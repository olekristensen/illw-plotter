#pragma once

#include "ofMain.h"
#include "ofxHPGL.h"
#include "ofxGeo.h"
#include "ofxTime.h"
#include "ofxGeographicLib.h"
#include "ofxImGui.h"
#include "ofxJSONRPC.h"
#include "ofxHersheyFont.h"

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

struct Location{
    string name;
    string country;
    string dxcc;
    string continent;
    string illw;
    ofx::Geo::Coordinate coordinate;
};

void to_json(ofJson& j, const Location& l);

void from_json(const ofJson& j, Location& l);

struct LogEntry{
public:
    int number;
    Location source;
    Location destination;
    string notes;
    Poco::DateTime timestamp;
};

void to_json(ofJson& j, const LogEntry& l);

void from_json(const ofJson& j, LogEntry& l);


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
    ofPath path;
    ofxHersheyFont hersheyFont;
    
    void plotText(string str, ofPoint pos=ofPoint(0,0), float size=20, float rotation=0, TextAlignment alignment = LEFT, TextVerticalAlignment valign = BASELINE);

    
    void plotLogEntry(LogEntry e);
    ofPoint startPoint;
    ofPoint endPoint;
    float margin;
    float radius;
    float halfHeight;
    
    // GUI
    
    ofxImGui::Gui gui;
    int guiColumnWidth = 250;
    double makeFakeLogsEverySeconds = 5;
    double nextLogSeconds = 0;
    
    
    // SERVER
    
    // Registered methods.
    void rpc_search(ofx::JSONRPC::MethodArgs& args);
    void rpc_addLogEntry(ofx::JSONRPC::MethodArgs& args);
    void rpc_getLog(ofx::JSONRPC::MethodArgs& args);
    
    /// \brief The server that handles the JSONRPC requests.
    ofx::HTTP::JSONRPCServer server;
    
    /// \brief Performs a search across locations.
    /// \param term the user term to search for.
    vector<Location> search(const std::string& term);
    
    // We use a mutex to protect any variables that can be
    // modified by multiple clients.  In our case, userText must be protected.
    // We mark the mutex as mutable so that it can be used in const functions.
    mutable std::mutex mutex;
    
    LogEntry addLogEntry(const Location loc);


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

