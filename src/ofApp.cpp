#include "ofApp.h"
#include <boost/algorithm/searching/boyer_moore.hpp>

//TODO:
/*
 
 make websocket clients reconnect...
 
 */

bool compareLocations (Location i,Location j) {
    
    if(i.illw.length() > 1 && j.illw.length() > 1){
        if(i.illw.substr(0,2).compare(j.illw.substr(0,2)) == 0 ){
            return (ofToInt(i.illw.substr(2,i.illw.length())) < ofToInt(j.illw.substr(2,j.illw.length())));
        } else {
            return (i.illw < j.illw);
        }
    } else {
        return (i.name < j.name);
    }
}

void ofApp::setup(){
    
    ofSetLogLevel(OF_LOG_NOTICE);
    
    // GUI
    
    setupGui();
    
    // LIGHTHOUSE DATA
    
    loadLighthouses("wllw_lighthouses.xml");
    
    // DATABASE
    
    std::string exampleDB = ofToDataPath("log.sqlite", true);
        
    try
    {
        // Open a database file in create/write mode.
        db = new SQLite::Database(exampleDB, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        
        ofLogVerbose("ofApp::setup()") << "SQLite database file '" << db->getFilename() << "' opened successfully";
        
        db->exec("CREATE TABLE IF NOT EXISTS log (id INTEGER PRIMARY KEY, source TEXT, destination TEXT, notes TEXT, timestamp TEXT, plotted TEXT)");
        
        // Check the results : expect two row of result
        SQLite::Statement query(*db, "SELECT * FROM log");
        
        ofLogVerbose("ofApp::setup()") << "SELECT * FROM log :";
        
        while (query.executeStep())
        {
            LogEntry l;
            from_query(query, l);
            log.push_back(l);
        }
        
    }
    catch (const std::exception& e)
    {
        ofLogError() << "SQLite exception: " << e.what();
    }

    
    // PLOTTER
    
    plotter.setup( "/dev/tty.usbserial-FT5CHURVB" );
    
    int pageW = ofGetScreenHeight()-600;
    int pageH = (float)pageW * (float)(sqrt(2.0));
    ofSetWindowShape( pageH, pageW );
    plotter.setInputWidth( pageH );
    plotter.setInputHeight( pageW );
    plotter.setPenColor(1, ofColor::darkBlue);
    plotter.setPenColor(2, ofColor::black);
    plotter.setPenColor(3, ofColor::yellow);
    plotter.enableCapture();
    plotter.setPen(1);
    
    float height = plotter.getInputHeight();
    float width = plotter.getInputWidth();
    halfHeight = height * 0.5;
    radius = height / 6.;
    margin = height / 10.;
    startPoint.set(width-(margin+radius), halfHeight);
    endPoint.set(margin+radius, halfHeight);
    
    path.setMode(ofPath::POLYLINES);
    path.setFilled(false);
    
    // SERVER
    
    ofx::HTTP::JSONRPCServerSettings settings;
    settings.setPort(8197);
    
    // Initialize the server.
    server.setup(settings);
    
    // Register RPC methods.
    server.registerMethod("search",
                          "Performs a search across locations.",
                          this,
                          &ofApp::rpc_search);
    
    server.registerMethod("addLogEntry",
                          "Add a location to the log.",
                          this,
                          &ofApp::rpc_addLogEntry);
    
    server.registerMethod("getLog",
                          "get the log.",
                          this,
                          &ofApp::rpc_getLog);
    
    // Start the server.
    server.start();
    
    // Launch a browser with the address of the server.
    ofLaunchBrowser(server.url());
    
}

void ofApp::update(){
    
    if(makeFakeLogs && nextFakeLogSeconds < ofGetElapsedTimef()){
        int locationNumber = ofRandom(0, locations.size()-1);
        addLogEntry(locations[locationNumber]);
        nextFakeLogSeconds = ofGetElapsedTimef() + ofRandom(makeFakeLogsEverySecondsMin,  makeFakeLogsEverySecondsMax);
    }
    
    if(plotterLive) {

        Poco::DateTime now;
        Poco::DateTimeFormatter fmt;

        stringstream ss;
        
        ss << "SELECT * FROM log WHERE";
        ss << " timestamp >= datetime('" + fmt.format(plotterLiveFromTimestamp, Poco::DateTimeFormat::ISO8601_FORMAT) + "')";
        ss << " AND timestamp < datetime('" + fmt.format(plotterLiveToTimestamp, Poco::DateTimeFormat::ISO8601_FORMAT) + "')";
        ss << " AND (plotted IS NULL OR plotted = '')";
        ss << " ORDER BY timestamp ASC LIMIT 1";
        
        ofLogVerbose("ofApp::update()") << ss.str();
        
        try
        {
            
            SQLite::Statement query(*db, ss.str());
            
            int id = -1;
            
            while (query.executeStep())
            {
                id = query.getColumn(0);
                ofLogNotice("ofApp::update()") << query.getColumn(0);
                LogEntry l;
                from_query(query, l);
                plotLogEntry(l);
                break;
            }
            
            if(id > 0){
                
                stringstream ssUpdate;
                
                Poco::DateTimeFormatter fmt;
                
                Poco::Timestamp t;
                
                ssUpdate << "UPDATE log SET plotted =";
                ssUpdate << " datetime('" + fmt.format(t, Poco::DateTimeFormat::ISO8601_FORMAT) + "')";
                ssUpdate << " WHERE id = " + ofToString(id);
                
                ofLogVerbose("ofApp::update()") << ssUpdate.str() ;
                
                db->exec(ssUpdate.str());
                
                
            }
                
            
        }
        catch (const std::exception& e)
        {
            ofLogError() << "SQLite exception: " << e.what();
        }
        
        if(now > plotterLiveToTimestamp){
            savePdf = true;
            plotterLive = false;
        }
        
    }
    
    plotter.update();
}

void ofApp::draw(){
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    ofBackground(233, 255);
    // page
    float scaleView = (ofGetWidth()-guiColumnWidth)*1.0/ofGetWidth();

    ofRectangle page(0,0,plotter.getInputWidth(), plotter.getInputHeight());
    page.scaleTo(ofRectangle( 0, 0, ofGetWidth(), ofGetHeight() ));
    ofRectangle pageAtZero(page);
    pageAtZero.translate(-page.x, -page.y);
    
    if(saveSvg){
        ofPushMatrix();
        ofBeginSaveScreenAsSVG("DocumentRoot/img/latest-plot.svg", false, false, pageAtZero);
        ofSetLineWidth(0.1);
        ofTranslate(-page.x, -page.y);
        ofBackground(255,255);
        ofFill();
        plotter.pushMatrix();
            ofDrawRectangle(0,0, plotter.getInputWidth(), plotter.getInputHeight());
        plotter.popMatrix();
        ofNoFill();
        plotter.draw();
        ofEndSaveScreenAsSVG();
        saveSvg = false;
        ofPopMatrix();
    }

    ofBackground(233, 255);
    ofFill();
    ofSetColor(255,255);

    if(savePdf){
        // got to draw with an origin at 0,0, so the page stuff does that.
        ofPushMatrix();
        ofBeginSaveScreenAsPDF("screenshot-"+ofGetTimestampString()+".pdf", false, false, pageAtZero);
        ofBackground(255,255);
        ofEnableBlendMode(OF_BLENDMODE_MULTIPLY);
        ofNoFill();
        ofSetLineWidth(0.1);
        ofTranslate(-page.x, -page.y);
        plotter.draw();
        ofEndSaveScreenAsPDF();
            savePdf = false;
        ofPopMatrix();
    } else {
        ofPushView();
        ofTranslate(guiColumnWidth, ofGetHeight()/2.0);
        ofScale(scaleView, scaleView);
        ofTranslate(0, -ofGetHeight()/2.0);
        plotter.pushMatrix();
        
        ofDrawRectangle(0,0, plotter.getInputWidth(), plotter.getInputHeight());
        //ofDrawEllipse(plotter.getInputPosFromPrinter(plotter.getPenPosition()), 10, 10);
        plotter.popMatrix();
        
        // plotter
        ofEnableBlendMode(OF_BLENDMODE_MULTIPLY);
        ofNoFill();
        plotter.draw();
        path.draw();
        ofPopView();
    }
    
    // GUI
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    ofFill();
    {
        gui.begin();
        ImGuiWindowFlags window_flags = 0;
        window_flags |= ImGuiWindowFlags_NoTitleBar;
        window_flags |= ImGuiWindowFlags_NoResize;
        window_flags |= ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoCollapse;
        window_flags |= ImGuiWindowFlags_ShowBorders;
        
        ImGui::SetNextWindowPos(ofVec2f(0,0));
        ImGui::SetNextWindowSize(ofVec2f(guiColumnWidth,ofGetHeight()));
        ImGui::Begin("Main###Debug", NULL, window_flags);
        
        // Title
        ImGui::PushFont(ImGuiIO().Fonts->Fonts[2]);
        ImGui::TextUnformatted("ILLW plotter");
        ImGui::PopFont();
        
        ImGui::TextUnformatted("ole kristensen");
        //ImGui::Text("FPS %.3f", ofGetFrameRate());
        
        Poco::LocalDateTime time;
        
        ImGui::TextDisabled("%02d:%02d:%02d.%03d", time.hour(), time.minute(), time.second(), time.millisecond());
        
        ImGui::PushFont(ImGuiIO().Fonts->Fonts[1]);
        ImGui::TextUnformatted("Log");
        ImGui::PopFont();
        
        ImGui::Separator();
        
        static ImGuiTextFilter filter;
        /* ImGui::Text("Filter usage:\n"
         "  \"\"         display all lines\n"
         "  \"xxx\"      display lines containing \"xxx\"\n"
         "  \"xxx,yyy\"  display lines containing \"xxx\" or \"yyy\"\n"
         "  \"-xxx\"     hide lines containing \"xxx\"");*/
        filter.Draw("Search");
        
        Poco::DateTimeFormatter fmt;
        
        int countMatches = 0;
        ImGui::BeginChild("LighthouseList", ImVec2(0,100), false, ImGuiWindowFlags_AlwaysVerticalScrollbar);
        
        // Display every line as a separate entry so we can change their color or add custom widgets. If you only want raw text you can use ImGui::TextUnformatted(log.begin(), log.end());
        // NB- if you have thousands of entries this approach may be too inefficient and may require user-side clipping to only process visible items.
        // You can seek and display only the lines that are visible using the ImGuiListClipper helper, if your elements are evenly spaced and you have cheap random access to the elements.
        // To use the clipper we could replace the 'for (int i = 0; i < Items.Size; i++)' loop with:
        //     ImGuiListClipper clipper(Items.Size);
        //     while (clipper.Step())
        //         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        // However take note that you can not use this code as is if a filter is active because it breaks the 'cheap random-access' property. We would need random-access on the post-filtered list.
        // A typical application wanting coarse clipping and filtering may want to pre-compute an array of indices that passed the filtering test, recomputing this array when user changes the filter,
        // and appending newly elements as they are inserted. This is left as a task to the user until we can manage to improve this example code!
        // If your items are of variable size you may want to implement code similar to what ImGuiListClipper does. Or split your data into fixed height items to allow random-seeking into your list.
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4,1)); // Tighten spacing
        
        for (int i = 0; i < locations.size(); i++){
            string searchStr = locations[i].illw + " " + locations[i].country + " " + " " + locations[i].name;
            if (filter.PassFilter(searchStr.c_str())){
                if(ImGui::Button(searchStr.c_str())){
                    addLogEntry(locations[i]);
                };
                countMatches++;
            }
            if(countMatches>15) break;
        }
        
        ImGui::PopStyleVar();
        ImGui::EndChild();
        ImGui::Checkbox("Fake", &makeFakeLogs);
        ImGui::SameLine();
        
        ImGui::DragFloatRange2("Interval", &makeFakeLogsEverySecondsMin, &makeFakeLogsEverySecondsMax, 1.0, 60.0);
        
        //        ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(0.40f, 0.39f, 0.38f, 0.5));
        ImGui::BeginChild("Log", ImVec2(0,100), false, ImGuiWindowFlags_AlwaysVerticalScrollbar);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4,1)); // Tighten spacing
        
        for (int i = 0; i < log.size(); i++){
            ImGui::Text("%s: %s >> %s", fmt.format(Poco::LocalDateTime(log[i].timestamp), "%H:%M:%S").c_str(), log[i].source.name.c_str(), log[i].destination.name.c_str());
        }
        
        ImGui::SetScrollHere();
        ImGui::PopStyleVar();
        ImGui::EndChild();
        //        ImGui::PopStyleColor();
        
        
        ImGui::Separator();
        
        ImGui::PushFont(ImGuiIO().Fonts->Fonts[1]);
        ImGui::TextUnformatted("Plotter");
        ImGui::PopFont();
        
        ImGui::Separator();
        
        bool plotterConnected = plotter.isConnected();
        ImGui::Checkbox("Connected", &plotterConnected);
        
        bool plotterCapturing = plotter.isCapturing();
        if(ImGui::Checkbox("Capture commands", &plotterCapturing)){
            if(plotterCapturing)
                plotter.enableCapture();
            else
                plotter.disableCapture();
        }
        
        if(ImGui::Checkbox("Live", &plotterLive)){
            if(plotterLive){
                Poco::DateTime timestamp;
                plotterLiveFromTimestamp = ofxTime::Utils::floor(timestamp, Poco::Timespan::HOURS);
                plotterLiveToTimestamp = plotterLiveFromTimestamp + ofxTime::Period::Hour();
            }
        }
        if(plotterLive){
            ImGui::SameLine();
            Poco::LocalDateTime time = plotterLiveFromTimestamp;
            ImGui::Text("from %s", ofxTime::Utils::format(time, "%H:%M").c_str());
        }
        ImGui::Text("%i commands", plotter.getNumCommands());
        if(plotter.isPrinting()){
            ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth()-(43+ImGui::GetStyle().ItemInnerSpacing.x));
            ImGui::ProgressBar(plotter.getProgress(), ImVec2(0.0f,0.0f));
            ImGui::PopItemWidth();
            ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
            if (ImGui::Button("Clear"))
            {
                plotter.clear();
            }
            
        }else{
            if (ImGui::Button("Clear"))
            {
                plotter.clear();
            }
            ImGui::SameLine();
            if (ImGui::Button("PDF"))
            {
                savePdf = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("Print"))
            {
                plotter.print();
            }
        }
        
        if(ImGui::Button("New Page")){
            plotPageBeginning();
        }
        ImGui::PushFont(ImGuiIO().Fonts->Fonts[1]);
        ImGui::TextUnformatted("Interface");
        ImGui::PopFont();
        
        ImGui::Separator();
        static bool guiShowTest;
        ImGui::Checkbox("Show Test Window", &guiShowTest);
        if(guiShowTest)
            ImGui::ShowTestWindow();
        
        
        ImGui::End();
        
        gui.end();
    }
    
    
}

void ofApp::keyPressed(int key){
    
}

void ofApp::keyReleased(int key){
    /*
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
     
     plotter.setPen(2);
     plotText(ofToString(60-i), ofPoint(x ,halfHeight-13), 7, -90, LEFT, MIDDLE);
     
     }
     }
     if(key == 't') {
     // test text
     plotter.clear();
     plotter.enableCapture();
     float x = plotter.getInputWidth()/4.0;
     float yinc = plotter.getInputHeight()/5.0;
     float y = yinc;
     plotter.setPen(2);
     plotter.line(x-10,y, x+10, y);
     plotter.line(x,y-10, x, y+10);
     plotter.circle(x, y, 5);
     plotText("this text is left baseline", ofPoint(x,y), 20, 180);
     plotter.setPen(1);
     plotText("this text is left baseline", ofPoint(x,y), 20);
     y+=yinc;
     plotter.setPen(2);
     plotter.line(x-10,y, x+10, y);
     plotter.line(x,y-10, x, y+10);
     plotter.circle(x, y, 5);
     plotText("this text is center baseline", ofPoint(x,y), 40, 180,CENTER);
     plotter.setPen(1);
     plotText("this text is center baseline", ofPoint(x,y), 40, 0,CENTER);
     y+=yinc;
     plotter.setPen(2);
     plotter.line(x-10,y, x+10, y);
     plotter.line(x,y-10, x, y+10);
     plotter.circle(x, y, 5);
     plotText("this text is right baseline", ofPoint(x,y), 10, 180,RIGHT);
     plotter.setPen(1);
     plotText("this text is right baseline", ofPoint(x,y), 10, 0,RIGHT);
     x*=3;
     y=yinc;
     plotter.setPen(2);
     plotter.line(x-10,y, x+10, y);
     plotter.line(x,y-10, x, y+10);
     plotter.circle(x, y, 5);
     plotText("This text is left top", ofPoint(x,y), 10, 180,LEFT, TOP);
     plotter.setPen(1);
     plotText("This text is left top", ofPoint(x,y), 10, 0,LEFT, TOP);
     y+=yinc;
     plotter.setPen(2);
     plotter.line(x-10,y, x+10, y);
     plotter.line(x,y-10, x, y+10);
     plotter.circle(x, y, 5);
     plotText("This text is center middle", ofPoint(x,y), 20, 180,CENTER, MIDDLE);
     plotter.setPen(1);
     plotText("This text is center middle", ofPoint(x,y), 20, 0,CENTER, MIDDLE);
     y+=yinc;
     plotter.setPen(2);
     plotter.line(x-10,y, x+10, y);
     plotter.line(x,y-10, x, y+10);
     plotter.circle(x, y, 5);
     plotText("this text is right bottom", ofPoint(x,y), 40, 180,RIGHT, BOTTOM);
     plotter.setPen(1);
     plotText("this text is right bottom", ofPoint(x,y), 40, 0,RIGHT, BOTTOM);
     
     }
     if ( key == 'n') {
     // north compass
     float r = margin/3.0;
     float c = r/7.0;
     float x = plotter.getInputWidth()-(margin/1.5);
     float y = halfHeight;
     plotter.circle(x, y, r);
     plotter.circle(x,y,r/10.0);
     plotter.line(x, y-c, x+r, y);
     plotter.line(x, y+c, x+r, y);
     plotter.line(x, y-c, x-r, y);
     plotter.line(x, y+c, x-r, y);
     plotText("N", ofPoint(x-r*1.2,y), 15, -90, CENTER);
     }
     */
}

void ofApp::plotPageBeginning(){
    
    float r = margin/3.0;
    float c = r/7.0;
    float x = plotter.getInputWidth()-margin;
    float y = halfHeight;
    float step = c / 7;
    
    // compass
    plotter.setPen(2);
    plotter.circle(x, y, r);
    plotter.line(x, y-c, x+r, y);
    plotter.line(x, y+c, x+r, y);
    plotter.setPen(1);
    for(float i = c; i > 0; i-=step){
        plotter.circle(x,y,i);
    }
    plotter.line(x, y, x-r, y);
    for(float i = c; i > 0; i-=step){
        plotter.line(x, y-i, x-r, y);
        plotter.line(x, y+i, x-r, y);
    }
    
    plotText("N", ofPoint(x-r*1.2,y), 7, -90, CENTER);
    
    // signature
    
    plotter.setPen(2);
    plotText("ole kristensen", ofPoint(x+r*1.5,y), 2.5, -90, CENTER);
    plotText("HAMMEREN FYR, BORNHOLM, 2017", ofPoint(4+x+r*1.5,y), 2, -90, CENTER);
    
    // timestamp
    plotter.setPen(3);
    Poco::LocalDateTime timestamp;
    Poco::LocalDateTime thisHourBegan = ofxTime::Utils::floor(timestamp, Poco::Timespan::HOURS);
    Poco::LocalDateTime thisHourEnds = thisHourBegan + ofxTime::Period::Hour();
    plotText(ofxTime::Utils::format(thisHourBegan,"%H:%M"), startPoint+ofPoint(10,0), 10, 180, RIGHT, MIDDLE);
    plotter.line(startPoint.x, startPoint.y, endPoint.x, endPoint.y);
    plotText(ofxTime::Utils::format(thisHourEnds,"%H:%M"), endPoint-ofPoint(10,0), 10, 180, LEFT, MIDDLE);

}

void ofApp::plotLogEntry(LogEntry e){
    
    bool outgoing = false;
    
    if(lighthouse.illw.compare(e.source.illw) == 0 && e.source.illw.size() > 0){
        outgoing = true;
    } else if (lighthouse.illw.compare(e.destination.illw) == 0 && e.source.illw.size() > 0) {
        outgoing = false;
    } else {
        return; // not to or from us
    }
    
    // find position within the hour
    
    Poco::DateTime timestamp(e.timestamp);
    
    Poco::DateTime thisHourBegan = ofxTime::Utils::floor(timestamp, Poco::Timespan::HOURS);
    Poco::DateTime thisHourEnds = thisHourBegan + ofxTime::Period::Hour();
    
    ofxTime::Interval hour(thisHourBegan, thisHourEnds);
    
    float normalisedHourlyRatio = hour.map(timestamp.timestamp());
    
    ofLogNotice() << normalisedHourlyRatio << " " << ofxTime::Utils::format(thisHourBegan) << " " << ofxTime::Utils::format(thisHourEnds) << " " << ofxTime::Utils::format(timestamp);
    
    
    ofPoint lighthousePointOnPaper(startPoint.getInterpolated(endPoint, normalisedHourlyRatio));
    
    auto sourceCoordinate = e.source.coordinate;
    auto destinationCoordinate = e.destination.coordinate;
    
    double scaleDivisor = ofxGeo::GeoUtils::EARTH_RADIUS_KM * PI / 2.0; // max distance is diameter/2 (the other side of the earth)
    
    double geoBearing = ofxGeo::GeoUtils::bearingHaversine(sourceCoordinate, destinationCoordinate);
    double geoDistance = ofxGeo::GeoUtils::distanceHaversine(sourceCoordinate, destinationCoordinate);
    float distanceOnPaper = geoDistance * radius / scaleDivisor;
    
    //ofLogNotice() << sourceCoordinate << " " << destinationCoordinate << "bearing: " << geoBearing << " dist: " << geoDistance;
    
    ofPoint outsidePoint(-distanceOnPaper, 0);
    outsidePoint.rotate(geoBearing, ofVec3f(0,0,1));
    ofPoint outsidePointOnPaper = outsidePoint + lighthousePointOnPaper;
    
    // PLOT LINE
    
    plotter.setPen(1);
    
    if(outgoing){
        plotter.circle(lighthousePointOnPaper.x, lighthousePointOnPaper.y, 0.5);
        plotter.line(lighthousePointOnPaper.x, lighthousePointOnPaper.y, outsidePointOnPaper.x, outsidePointOnPaper.y);
        plotter.circle(outsidePointOnPaper.x, outsidePointOnPaper.y, 0.5);
    } else {
        plotter.circle(outsidePointOnPaper.x, outsidePointOnPaper.y, 0.5);
        plotter.line(outsidePointOnPaper.x, outsidePointOnPaper.y, lighthousePointOnPaper.x, lighthousePointOnPaper.y);
        plotter.circle(lighthousePointOnPaper.x, lighthousePointOnPaper.y, 0.5);
    }
    
    
    /*
    plotter.setPen(3);
    plotter.circle(lighthousePointOnPaper.x, lighthousePointOnPaper.y, distanceOnPaper);
    */
    
    // PLOT TEXT
    
    stringstream ss;
    if(outgoing){
        ss << e.destination.illw << " " << e.destination.name  << " " << e.destination.country;
    } else {
        ss << e.source.illw << " " << e.source.name << " " << e.source.country;
    }
    
    iconvpp::converter conv("ascii//TRANSLIT",   // output encoding
                            "utf-8",   // input encoding
                            true,      // ignore errors (optional, default: fasle)
                            1024);     // buffer size   (optional, default: 1024)
    ;
    std::string translitterated;
    conv.convert(ss.str(), translitterated);
    
    plotter.setPen(2);
    
    plotText(translitterated, outsidePoint.getScaled(outsidePoint.length()+3) + lighthousePointOnPaper, 2, fmodf(180+geoBearing, 360), TextAlignment::LEFT, TextVerticalAlignment::MIDDLE);
    
    saveSvg = true;
    
}

void ofApp::plotText(string str, ofPoint pos, float size, float rotation, TextAlignment alignment, TextVerticalAlignment valign){
    
    float sizeFactor = (size/21.0)*(plotter.getInputWidth()/1200.0);
    
    auto c = hersheyFont.getPath(str, sizeFactor);
    
    float width = hersheyFont.getWidth(str, sizeFactor);
    float mHeight = 14.0*sizeFactor;
    
    if(alignment == LEFT){
        // noting to be done
    } else if(alignment == CENTER) {
        c.translate(ofPoint(-width/2.0, 0));
    } else if(alignment == RIGHT){
        c.translate(ofPoint(-width, 0));
    }
    if(valign == TOP){
        c.translate(ofPoint(0, size));
    } else if (valign == MIDDLE){
        c.translate(ofPoint(0, mHeight/2.0));
    } else if (valign == BOTTOM){
        c.translate(ofPoint(0, -7.0*sizeFactor));
    } else if (valign == BASELINE){
        // nothing to be done
    }
    
    c.rotate(rotation, ofVec3f(0,0,1));
    c.translate(pos);
    
    auto o = c.getOutline();
    for (auto p : o){
        plotter.polyline(p);
    }
}

void ofApp::mouseMoved(int x, int y ){
    
}

void ofApp::mouseDragged(int x, int y, int button){
    
}

void ofApp::mousePressed(int x, int y, int button){
    
}

void ofApp::mouseReleased(int x, int y, int button){
    
}

void ofApp::mouseEntered(int x, int y){
    
}

void ofApp::mouseExited(int x, int y){
    
}

void ofApp::windowResized(int w, int h){
    
}

void ofApp::gotMessage(ofMessage msg){
    
}

void ofApp::dragEvent(ofDragInfo dragInfo){
    
}

void ofApp::setupGui(){
    
    
    ImGui::GetIO().MouseDrawCursor = false;
    
    ImGuiIO& io = ImGui::GetIO();
    
    io.Fonts->Clear();
    io.Fonts->AddFontFromFileTTF(ofToDataPath("OpenSans-Light.ttf", true).c_str(), 16);
    io.Fonts->AddFontFromFileTTF(ofToDataPath("OpenSans-Regular.ttf", true).c_str(), 16);
    io.Fonts->AddFontFromFileTTF(ofToDataPath("OpenSans-Light.ttf", true).c_str(), 32);
    io.Fonts->AddFontFromFileTTF(ofToDataPath("OpenSans-Regular.ttf", true).c_str(), 11);
    io.Fonts->AddFontFromFileTTF(ofToDataPath("OpenSans-Bold.ttf", true).c_str(), 11);
    io.Fonts->Build();
    
    gui.setup(new GuiTheme());
    
    ImGuiStyle & style = ImGui::GetStyle();
    
    style.WindowPadding            = ImVec2(15, 15);
    style.WindowRounding           = .0f;
    style.FramePadding             = ImVec2(8, 3);
    style.FrameRounding            = 3.0f;
    style.ItemSpacing              = ImVec2(12, 5);
    style.ItemInnerSpacing         = ImVec2(8, 6);
    style.IndentSpacing            = 25.0f;
    style.ScrollbarSize            = 15.0f;
    style.ScrollbarRounding        = 9.0f;
    style.GrabMinSize              = 8.0f;
    style.GrabRounding             = 2.0f;
    
    style.Colors[ImGuiCol_Text]                  = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.40f, 0.39f, 0.38f, 0.77f);
    style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.96f, 0.96f, 0.96f, 0.45f);
    style.Colors[ImGuiCol_ChildWindowBg]         = ImVec4(1.00f, 1.00f, 1.00f, 0.35f);
    style.Colors[ImGuiCol_PopupBg]               = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_Border]                = ImVec4(0.84f, 0.83f, 0.80f, 0.42f);
    style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
    style.Colors[ImGuiCol_FrameBg]               = ImVec4(0.03f, 0.00f, 0.00f, 0.02f);
    style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.84f, 0.83f, 0.80f, 0.42f);
    style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.84f, 0.83f, 0.80f, 0.20f);
    style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.84f, 0.83f, 0.80f, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(1.00f, 1.00f, 1.00f, 0.92f);
    style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(1.00f, 1.00f, 1.00f, 0.76f);
    style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.00f, 0.00f, 0.00f, 0.21f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.00f, 0.00f, 0.00f, 0.32f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    style.Colors[ImGuiCol_ComboBg]               = ImVec4(1.00f, 0.98f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.00f, 0.00f, 0.00f, 0.19f);
    style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.00f, 0.00f, 0.00f, 0.14f);
    style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    style.Colors[ImGuiCol_Button]                = ImVec4(0.00f, 0.00f, 0.00f, 0.09f);
    style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.00f, 0.00f, 0.00f, 0.01f);
    style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.00f, 0.00f, 0.00f, 0.12f);
    style.Colors[ImGuiCol_Header]                = ImVec4(0.02f, 0.05f, 0.00f, 0.05f);
    style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.00f, 0.00f, 0.00f, 0.05f);
    style.Colors[ImGuiCol_HeaderActive]          = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_Column]                = ImVec4(0.00f, 0.00f, 0.00f, 0.32f);
    style.Colors[ImGuiCol_ColumnHovered]         = ImVec4(0.00f, 0.00f, 0.00f, 0.40f);
    style.Colors[ImGuiCol_ColumnActive]          = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.00f, 0.00f, 0.00f, 0.04f);
    style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.25f, 1.00f, 0.00f, 0.78f);
    style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_CloseButton]           = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
    style.Colors[ImGuiCol_CloseButtonHovered]    = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
    style.Colors[ImGuiCol_CloseButtonActive]     = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
    style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.25f, 1.00f, 0.00f, 0.57f);
    style.Colors[ImGuiCol_ModalWindowDarkening]  = ImVec4(1.00f, 0.98f, 0.95f, 0.68f);
}

bool ofApp::loadLighthouses(string xmlFilePath){
    if(lighthouseXml.load(xmlFilePath)){
        ofLogNotice() << " lighthouses loaded";
    } else {
        return false;
    }
    
    auto lhNodes = lighthouseXml.getChild("lights").getChildren();
    
    ofxGeo::Coordinate hammerenFyr(55.286849, 14.759447);
    
    for(auto node : lhNodes){
        Location l;
        l.illw = node.getChild("illw").getValue();
        l.searchStringLowercase = ofToLower(l.illw);
        l.name = node.getChild("name").getValue();
        l.searchStringLowercase += " " + ofToLower(l.name);
        l.country = node.getChild("country").getValue();
        l.searchStringLowercase += " " + ofToLower(l.country);
        l.dxcc = node.getChild("dxcc").getValue();
        l.searchStringLowercase += " " + ofToLower(l.dxcc);
        l.continent = node.getChild("continent").getValue();
        l.searchStringLowercase += " " + ofToLower(l.continent);
        
        string coords = node.getChild("geo").getValue();
        
        if (!(ofIsStringInString(coords, "Museum")||ofIsStringInString(coords, "museum"))){
            
            if(coords[0] == '@'){
                coords = coords.substr(1, coords.find_last_of(','));
            }
            ofStringReplace(coords, "<br>", " ");
            
            try
            {
                // Miscellaneous conversions
                GeographicLib::GeoCoords c(coords);
                ofxGeo::Coordinate cOfx(c.Latitude(), c.Longitude());
                
                l.coordinate = cOfx;
                
                ofLogVerbose() << ofxGeo::GeoUtils::distanceHaversine(hammerenFyr, cOfx) << "km til " << l.country << " " << l.name << " retning " << ofxGeo::GeoUtils::bearingHaversine(hammerenFyr, cOfx);
                
                
            }
            catch (const std::exception& e) {
                ofLogError() << "\t" << l.country << " " << l.name << endl << "\t\t\t" <<  coords << endl << "\t\t\t" << "Caught exception: " << e.what() << endl;
            }
        }
        
        locations.push_back(l);
        if(ofIsStringInString(l.country, "Denmark") && ofIsStringInString(l.name, "Hammer")){
            lighthouse = l;
        }
    }
    
    std::sort (locations.begin(), locations.end(), compareLocations);
    
    return true;
    
}

void ofApp::rpc_search(ofx::JSONRPC::MethodArgs& args)
{
    // Set the result equal to the substring.
    // std::unique_lock<std::mutex> lock(mutex);
    
    ofLogVerbose("ofApp::rpc_search") << args.params.dump(4);
    
    vector<Location> result = search(args.params);
    
    args.result = result;
    
    ofLogVerbose("ofApp::rpc_search") << args.result.dump(4);
}

vector<Location> ofApp::search(const std::string& term)
{
    auto needles = ofSplitString(ofToLower(ofTrim(term)), " ");
    
    vector<Location> matches;
    
    /*
     map<std::string, boost::algorithm::boyer_moore<std::string::const_iterator> > searches;
    
    for(const std::string n : needles){
        searches.insert(make_pair(n, boost::algorithm::make_boyer_moore ( n ) ));
    }
     */
    
    for(const auto l : locations){
        
        const std::string haystack(l.searchStringLowercase);
        
        const std::string illwLowercase(ofToLower(l.illw));
        
        int foundTerms = 0;
        bool foundIllw = false;
        
        for(const std::string n : needles){
            
            //boost::algorithm::boyer_moore<std::string::const_iterator> s = searches.at(n);
        
            // search all fields
//            if(s(haystack.begin(), haystack.end()) != std::make_pair(haystack.end (), haystack.end ())){
            if(boost::algorithm::boyer_moore_search( haystack.begin(), haystack.end(), n.begin(), n.end()) != std::make_pair(haystack.end (), haystack.end ())){
                foundTerms++;
            } else {
                // do an extra expensive check on illw codes regardless of zeros and spaces
                if(n.length() > 2 && (illwLowercase.substr(0,2).compare(n.substr(0,2)) == 0)){
                    if(ofToInt(illwLowercase.substr(2,illwLowercase.length())) == ofToInt(n.substr(2,n.length()))){
                        foundIllw = true;
                        break;
                    }
                    
                }
            }
        }
        
        if(foundIllw || foundTerms == needles.size()){
            matches.push_back(l);
        }
    }
    
    ofLogVerbose("ofApp::search") << matches.size();
    
    return matches;
}


void to_json(ofJson& j, const Location& l) {
    j = ofJson{{"name", l.name}, {"country", l.country}, {"dxcc", l.dxcc}, {"continent", l.continent}, {"illw", l.illw}, {"coordinate", { {"lat", l.coordinate.getLatitude()}, {"lon", l.coordinate.getLongitude() } } }};
};

void from_json(const ofJson& j, Location& l) {
    
    l.name = j.at("name").get<std::string>();
    l.country = j.at("country").get<std::string>();
    l.dxcc = j.at("dxcc").get<std::string>();
    l.continent = j.at("continent").get<std::string>();
    l.illw = j.at("illw").get<std::string>();
    l.coordinate = ofxGeo::Coordinate(j.at("coordinate").at("lat").get<double>(),  j.at("coordinate").at("lon").get<double>() );
};

void to_json(ofJson& j, const LogEntry& l) {
    Poco::DateTimeFormatter fmt;
    std::string timestamp = fmt.format(l.timestamp, Poco::DateTimeFormat::HTTP_FORMAT);
    j = ofJson{{"number", l.number}, {"source", l.source}, {"destination", l.destination}, {"notes", l.notes}, {"timestamp", timestamp }, {"distance", ofx::Geo::GeoUtils::distanceHaversine(l.source.coordinate, l.destination.coordinate) }, {"bearing", ofx::Geo::GeoUtils::bearingHaversine(l.source.coordinate, l.destination.coordinate)  } };
};

void from_json(const ofJson& j, LogEntry& l) {
    l.number = j.at("number").get<int>();
    l.source = j.at("source").get<Location>();
    l.destination = j.at("destination").get<Location>();
    l.notes = j.at("notes").get<std::string>();
    Poco::DateTimeParser parser;
    int tz;
    l.timestamp = parser.parse(j.at("timestamp").get<std::string>(), tz);
};

void from_query(SQLite::Statement& q, LogEntry& l) {
    
    ofLogVerbose("from_query:") << q.getColumn(0).getString() << " " << q.getColumn(1).getString() << " " << q.getColumn(2).getString() << " " << q.getColumn(3).getString() << " " << q.getColumn(4).getString();

    ofJson sourceJson = ofJson::parse(q.getColumn(1).getString());
    ofJson destinationJson = ofJson::parse(q.getColumn(2).getString());

    l.number = q.getColumn(0).getInt();
    l.source = sourceJson.get<Location>();
    l.destination = destinationJson.get<Location>();
    l.notes = q.getColumn(3).getString();
    
    Poco::DateTimeParser parser;
    int tz;
    l.timestamp = parser.parse(q.getColumn(4).getString(), tz);
    
};

void ofApp::rpc_addLogEntry(ofx::JSONRPC::MethodArgs& args)
{
    // Set the result equal to the substring.
    std::unique_lock<std::mutex> lock(mutex);
    
    ofLogVerbose("ofApp::rpc_addLogEntry") << args.params.dump(4);
    
    Location newLocation = args.params;
    
    LogEntry newLog = addLogEntry(newLocation);
    
    args.result = newLog;
    
    ofLogVerbose("ofApp::rpc_addLogEntry") << args.result.dump(4);
}

void ofApp::rpc_getLog(ofx::JSONRPC::MethodArgs& args)
{
    ofLogVerbose("ofApp::rpc_getLogEntries") << args.params.dump(4);
    vector<LogEntry> result;
    int startFrom = args.params;
    for(int i = startFrom; i < log.size(); i++){
        result.push_back(log.at(i));
    }
    args.result = result;
    ofLogVerbose("ofApp::rpc_getLogEntries") << args.result.dump(4);
}

LogEntry ofApp::addLogEntry(const Location loc){
    LogEntry l;
    l.source = lighthouse;
    l.destination = loc;
    ofx::HTTP::WebSocketFrame frame("refreshLog");
    server.webSocketRoute().broadcast(frame);
    
    try
    {

        ofJson j = l;
        
        stringstream ss;

        // (id INTEGER PRIMARY KEY, source TEXT, destination TEXT, notes TEXT, timestamp TEXT, plotted TEXT);

        Poco::DateTimeFormatter fmt;
        
        ss << "INSERT INTO log (source, destination, timestamp) VALUES (";
        ss << "'" << j.at("source").dump() << "',";
        ss << "'" << j.at("destination").dump() + "',";
        ss << + "datetime('" + fmt.format(l.timestamp, Poco::DateTimeFormat::ISO8601_FORMAT) + "'))";
        
        ofLogVerbose("addLogEntry:") << ss.str() ;
        
        db->exec(ss.str());
        
    }
    catch (const std::exception& e)
    {
        ofLogError("addLogEntry:") << "SQLite exception: " << e.what();
    }
    

    
    log.push_back(l);
    return l;
}

