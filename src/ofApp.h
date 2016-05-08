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
        void updateAliasFieldPositions();
        void onAliasFieldTextChanged(const void * sender, string& text);
    
        bool debug;
        int selectedSampIndex;
        vector<shared_ptr<Sample>> samples;
        vector<shared_ptr<ofxTextInputField>> aliasFields;
        ofVideoPlayer player;
        ofRectangle vidRect;
        ofxOscSender sender;
		
};
