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
    
    //setup gui
    
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
    
    
    if(lighthouseXml.load("wllw_lighthouses.xml")){
        ofLogNotice() << lighthouseXml.getNumChildren() << " lighthouses loaded";
    };
    
    auto lhNodes = lighthouseXml.getPocoElement()->childNodes();
    
    ofLogNotice() << lhNodes->length() << " lighthouses parsed";
    
    ofxGeo::Coordinate hammerenFyr(55.286849, 14.759447);
    
    for(int i = 0; i < lhNodes->length() ; i++){
        if(lhNodes->item(i)->hasChildNodes()){
            
            Lighthouse l;
            l.country = lhNodes->item(i)->getNodeByPath("country")->innerText();
            l.name = lhNodes->item(i)->getNodeByPath("name")->innerText();
            l.dxcc = lhNodes->item(i)->getNodeByPath("dxcc")->innerText();
            l.continent = lhNodes->item(i)->getNodeByPath("continent")->innerText();
            l.illw = lhNodes->item(i)->getNodeByPath("illw")->innerText();

            string coords = lhNodes->item(i)->getNodeByPath("geo")->innerText();
            
            if (!(ofIsStringInString(coords, "Museum")||ofIsStringInString(coords, "museum"))){
                
                if(coords[0] == '@'){
                    coords = coords.substr(1, coords.find_last_of(','));
                }
                ofStringReplace(coords, "<br>", " ");
                //ofLogNotice() << coords;
                try
                {
                    // Miscellaneous conversions
                    GeographicLib::GeoCoords c(coords);
                    ofxGeo::Coordinate cOfx(c.Latitude(), c.Longitude());
                    
                    l.coordinate = cOfx;
                    
                    ofLogNotice() << ofxGeo::Utils::distanceHaversine(hammerenFyr, cOfx) << "km til " << lhNodes->item(i)->getNodeByPath("country")->innerText() << " " << lhNodes->item(i)->getNodeByPath("name")->innerText() << " retning " << ofxGeo::Utils::bearingHaversine(hammerenFyr, cOfx);
                    
                    
                }
                catch (const std::exception& e) {
                    ofLogError() << "\t" << lhNodes->item(i)->getNodeByPath("country")->innerText() << " " << lhNodes->item(i)->getNodeByPath("name")->innerText() << endl << "\t\t\t" <<  lhNodes->item(i)->getNodeByPath("geo")->innerText() << endl << "\t\t\t" << "Caught exception: " << e.what() << endl;
                }
            }

            lighthouses.push_back(l);
            if(ofIsStringInString(l.country, "Denmark") && ofIsStringInString(l.name, "Hammer")){
                location = l;
            }
        }
    }
    
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
    
    // PLOTTER
    ofNoFill();
    ofSetColor(0, 255);
    ofEnableBlendMode(OF_BLENDMODE_MULTIPLY);
    plotter.draw();
    /*
    ofSetColor(255,0,255,200);
    plotter.pushMatrix();
    ofDrawEllipse(plotter.getInputPosFromPrinter(plotter.getPenPosition()), 10, 10);
    plotter.popMatrix();
    */
    
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
        
        for (int i = 0; i < lighthouses.size(); i++){
            string searchStr = lighthouses[i].illw + " " + lighthouses[i].country + " " + " " + lighthouses[i].name;
            
            
            if (filter.PassFilter(searchStr.c_str())){
                if(ImGui::Button(searchStr.c_str())){
                    LogEntry l;
                    l.source = location;
                    l.destination = lighthouses[i];
                    log.push_back(l);
                    ///   * %d - days
                    ///   * %H - hours	 (00 .. 23)
                    ///   * %h - total hours (0 .. n)
                    ///   * %M - minutes (00 .. 59)
                    ///   * %m - total minutes (0 .. n)
                    ///   * %S - seconds (00 .. 59)
                    ///   * %s - total seconds (0 .. n)
                    ///   * %i - milliseconds (000 .. 999)
                    ///   * %c - centisecond (0 .. 9)
                    ///   * %F - fractional seconds/microseconds (000000 - 999999)
                    ///   * %% - percent sign
                    ofLogNotice() << fmt.format(Poco::LocalDateTime(l.timestamp), "%H:%M:%S") << " " << l.source.name << " >> " << l.destination.name;
                };
                countMatches++;
            }
//            if(countMatches>15) break;
        }
        
        ImGui::PopStyleVar();
        ImGui::EndChild();


        ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(1.0,1.0,1.0,1.0));
        ImGui::BeginChild("Log", ImVec2(0,100), false, ImGuiWindowFlags_AlwaysVerticalScrollbar);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4,1)); // Tighten spacing
        
        for (int i = 0; i < log.size(); i++){
            ImGui::Text("%s: %s >> %s", fmt.format(Poco::LocalDateTime(log[i].timestamp), "%H:%M:%S").c_str(), log[i].source.name.c_str(), log[i].destination.name.c_str());
        }
        
        ImGui::SetScrollHere();
        ImGui::PopStyleVar();
        ImGui::EndChild();
        ImGui::PopStyleColor();


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
            if (ImGui::Button("Print"))
            {
                plotter.print();
            }
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
            
            plotter.setPen(2);
            plotText(ofToString(i), plotFont, ofPoint(x ,halfHeight-13), 7, -90, LEFT, MIDDLE);
            
        }
    }
    if(key == 't') {
        // test text
        plotter.clear();
        plotter.enableCapture();
        float x = plotter.getInputWidth()/4.0;
        float y = 200;
        plotter.setPen(2);
        plotter.line(x-10,y, x+10, y);
        plotter.line(x,y-10, x, y+10);
        plotter.circle(x, y, 5);
        plotText("this text is left baseline", plotFont, ofPoint(x,y), 20, 180);
        plotter.setPen(1);
        plotText("this text is left baseline", plotFont, ofPoint(x,y), 20);
        y+=300;
        plotter.setPen(2);
        plotter.line(x-10,y, x+10, y);
        plotter.line(x,y-10, x, y+10);
        plotter.circle(x, y, 5);
        plotText("this text is center baseline", plotFont, ofPoint(x,y), 40, 180,CENTER);
        plotter.setPen(1);
        plotText("this text is center baseline", plotFont, ofPoint(x,y), 40, 0,CENTER);
        y+=300;
        plotter.setPen(2);
        plotter.line(x-10,y, x+10, y);
        plotter.line(x,y-10, x, y+10);
        plotter.circle(x, y, 5);
        plotText("this text is right baseline", plotFont, ofPoint(x,y), 10, 180,RIGHT);
        plotter.setPen(1);
        plotText("this text is right baseline", plotFont, ofPoint(x,y), 10, 0,RIGHT);
        x*=3;
        y=200;
        plotter.setPen(2);
        plotter.line(x-10,y, x+10, y);
        plotter.line(x,y-10, x, y+10);
        plotter.circle(x, y, 5);
        plotText("This text is left top", plotFont, ofPoint(x,y), 10, 180,LEFT, TOP);
        plotter.setPen(1);
        plotText("This text is left top", plotFont, ofPoint(x,y), 10, 0,LEFT, TOP);
        y+=300;
        plotter.setPen(2);
        plotter.line(x-10,y, x+10, y);
        plotter.line(x,y-10, x, y+10);
        plotter.circle(x, y, 5);
        plotText("This text is center middle", plotFont, ofPoint(x,y), 20, 180,CENTER, MIDDLE);
        plotter.setPen(1);
        plotText("This text is center middle", plotFont, ofPoint(x,y), 20, 0,CENTER, MIDDLE);
        y+=300;
        plotter.setPen(2);
        plotter.line(x-10,y, x+10, y);
        plotter.line(x,y-10, x, y+10);
        plotter.circle(x, y, 5);
        plotText("this text is right bottom", plotFont, ofPoint(x,y), 40, 180,RIGHT, BOTTOM);
        plotter.setPen(1);
        plotText("this text is right bottom", plotFont, ofPoint(x,y), 40, 0,RIGHT, BOTTOM);
        
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
        plotText("N", plotFont, ofPoint(x-r*1.2,y), 15, -90, CENTER);
    }
}

void ofApp::plotText(string str, ofTrueTypeFont font, ofPoint pos, float size, float rotation, TextAlignment alignment, TextVerticalAlignment valign){
    
    float sizeFactor = size/font.getSize();
    
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
        c.scale(sizeFactor, sizeFactor);
        
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
