#pragma once

#include "ofMain.h"
#include "Sample.h"
#include "ofxOSC.h"
#include "ofxTextInputField.h"

// Made with OF v0.9.3, untested w/ other versions.
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
        void updateFieldPositions();
        void onAliasFieldTextChanged(const void * sender, string& text);
        void onWindowFieldTextChanged(const void * sender, string& text);
        Sample::AliasAddress createAliasAddress(string addressText, Sample::sampleType defaultType);
    
        bool debug;
        int selectedSampIndex;
        ofVec4f windowClickPoints; //x1, y1, x2, y2
        vector<shared_ptr<Sample>> samples;
        vector<shared_ptr<ofxTextInputField>> aliasFields;
        vector<shared_ptr<ofxTextInputField>> windowFields;
        ofVideoPlayer player;
        ofRectangle vidRect;
        ofxOscSender sender;
		
};
