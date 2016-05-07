#include "ofApp.h"
// Made with OF v0.9.3, untested w/ other versions.

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0);
    player.load("fingers.mov");
    player.setLoopState(OF_LOOP_NORMAL);
    player.play();
    debug = false;
    selectedSampIndex = -1;
    sender.setup("localhost", 12345);
}

//--------------------------------------------------------------
void ofApp::update(){
    player.update();
    for (auto sample : samples) {
       
        sample->update(vidRect,
                       ofVec2f(player.getWidth(), player.getHeight()),
                       player.getPixels());
        
        if (sample->isEnabled()) {
            ofxOscMessage m;
            m.setAddress(sample->getName());
            int val;
            switch (sample->getType()) {
                case Sample::sampleType::LIGHTNESS: val = sample->val[3]; break;
                case Sample::sampleType::RED: val = sample->val[0]; break;
                case Sample::sampleType::GREEN: val = sample->val[1]; break;
                case Sample::sampleType::BLUE: val = sample->val[2]; break;
            }
            m.addIntArg(val);
            sender.sendMessage(m, false);
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    float width = ofGetWidth();
    float height = ofGetHeight();
    float aspect = width / height;

    // horizontal black bars
    if (aspect <= player.getWidth()/player.getHeight()) {
        float vHeight = (player.getHeight() * width) / player.getWidth();
        vidRect.set(0, (height - vHeight) * 0.5, width, vHeight); // cache video rect
    } else { // vertical black bars
        float vWidth = (player.getWidth() * height) / player.getHeight();
        vidRect.set((width - vWidth) * 0.5, 0, vWidth, height); // cache video rect
    }
    
    if (!debug) {
        player.draw(vidRect);
        for (auto sample : samples) {
            sample->draw();
        }
    } else {
        if (samples.size() > 0) {
            int graphHeight = ofGetHeight() / samples.size();
            int sampNum = 1;
            for (auto sample : samples) {
                float x = 0;
                float sampWidth = float(ofGetWidth()) / float(Sample::bufferSize);
                ofPushStyle();
                ofDrawLine(x, (graphHeight * (sampNum - 1)), ofGetWidth(), (graphHeight * (sampNum - 1)));
                if (sample->isSelected()) ofSetColor(255, 255, 0);
                string type;
                switch (sample->getType()) {
                    case Sample::sampleType::LIGHTNESS: type = " | LIGHTNESS"; break;
                    case Sample::sampleType::RED: type = " | RED"; break;
                    case Sample::sampleType::GREEN: type = " | GREEN"; break;
                    case Sample::sampleType::BLUE: type = " | BLUE"; break;
                }
                string enabled = sample->isEnabled() ? " | ENABLED " : " | DISABLED";
                string name = sample->getName() != "" ? sample->getName() : "NO_ADDRESS";
                string smooth = " | SMOOTH: " + ofToString(sample->getSmooth());
                ofDrawBitmapString(name + type + enabled + smooth, x + 15, (graphHeight * (sampNum - 1)) + 15);
                for (int i = 0; i < Sample::bufferSize; i++) {
                    ofSetColor(255, 0, 0);
                    ofDrawRectangle(x, (graphHeight * sampNum) - ofMap(sample->vals[i][0], 0, 255, 0, graphHeight), sampWidth, 1);
                    ofSetColor(0, 255, 0);
                    ofDrawRectangle(x, (graphHeight * sampNum) - ofMap(sample->vals[i][1], 0, 255, 0, graphHeight), sampWidth, 1);
                    ofSetColor(0, 0, 255);
                    ofDrawRectangle(x, (graphHeight * sampNum) - ofMap(sample->vals[i][2], 0, 255, 0, graphHeight), sampWidth, 1);
                    ofSetColor(255);
                    ofDrawRectangle(x, (graphHeight * sampNum) - ofMap(sample->vals[i][3], 0, 255, 0, graphHeight), sampWidth, 1);
                    ofSetColor(255);
                    x+=sampWidth;
                }
                ofPopStyle();
                sampNum++;
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if (key == ' ') {
        debug = !debug;
    }
    
    if (selectedSampIndex != -1 && selectedSampIndex < samples.size())
    {
        // backspace/delete
        if (key == 127)
        {
            if (ofGetKeyPressed(OF_KEY_SHIFT))
            {
                samples.erase(samples.begin() + selectedSampIndex);
                if (samples.size() > 0)
                {
                    selectedSampIndex = 0;
                    samples[selectedSampIndex]->setSelected(true);
                }
                else
                {
                    selectedSampIndex = -1;
                }
            }
            else
            {
                samples[selectedSampIndex]->removeCharFromName();
            }
        } else if (key == OF_KEY_RIGHT) {
            samples[selectedSampIndex]->setSmooth(samples[selectedSampIndex]->getSmooth() + 1);
        } else if (key == OF_KEY_LEFT && samples[selectedSampIndex]->getSmooth()) {
            samples[selectedSampIndex]->setSmooth(samples[selectedSampIndex]->getSmooth() - 1);
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
    // if there is a selected sample
    if (selectedSampIndex != -1 && selectedSampIndex < samples.size()) {
        // tab key
        if (key == 9) {
            // next selection
            samples[selectedSampIndex]->setSelected(false);
            if (selectedSampIndex == samples.size() - 1) {
                selectedSampIndex = 0;
            } else {
                selectedSampIndex++;
            }
            samples[selectedSampIndex]->setSelected(true);
        }
        
        if (!debug) {
            if (key == '/' ||
                 (key >= 65 && key <= 90) /*A-Z*/ ||
                 (key >= 97 && key <= 122) /*a-z*/ ||
                 (key >= 48 && key <= 57) /*0-9*/) {
                samples[selectedSampIndex]->addCharToName(key);
            } else if (key == 13) { // enter
                samples[selectedSampIndex]->updateName();
            } else if (key == OF_KEY_LEFT) {
                if (samples[selectedSampIndex]->getType() == Sample::sampleType::LIGHTNESS) {
                    samples[selectedSampIndex]->setType(Sample::sampleType::BLUE);
                } else if (samples[selectedSampIndex]->getType() == Sample::sampleType::GREEN) {
                    samples[selectedSampIndex]->setType(Sample::sampleType::RED);
                }
            } else if (key == OF_KEY_RIGHT) {
                if (samples[selectedSampIndex]->getType() == Sample::sampleType::BLUE) {
                    samples[selectedSampIndex]->setType(Sample::sampleType::LIGHTNESS);
                } else if (samples[selectedSampIndex]->getType() == Sample::sampleType::RED) {
                    samples[selectedSampIndex]->setType(Sample::sampleType::GREEN);
                }
            } else if (key == OF_KEY_UP) {
                if (samples[selectedSampIndex]->getType() == Sample::sampleType::BLUE) {
                    samples[selectedSampIndex]->setType(Sample::sampleType::RED);
                } else if (samples[selectedSampIndex]->getType() == Sample::sampleType::LIGHTNESS) {
                    samples[selectedSampIndex]->setType(Sample::sampleType::GREEN);
                }
            } else if (key == OF_KEY_DOWN) {
                if (samples[selectedSampIndex]->getType() == Sample::sampleType::RED) {
                    samples[selectedSampIndex]->setType(Sample::sampleType::BLUE);
                } else if (samples[selectedSampIndex]->getType() == Sample::sampleType::GREEN) {
                    samples[selectedSampIndex]->setType(Sample::sampleType::LIGHTNESS);
                }
            }
        } else { // debug
            if (key == OF_KEY_UP) {
                int type = samples[selectedSampIndex]->getType();
                if (type < 3) {
                    type++;
                    samples[selectedSampIndex]->setType((Sample::sampleType) type);
                } else samples[selectedSampIndex]->setType((Sample::sampleType) 0);
            } else if (key == OF_KEY_DOWN) {
                int type = samples[selectedSampIndex]->getType();
                if (type != 0) {
                    type--;
                    samples[selectedSampIndex]->setType((Sample::sampleType) type);
                } else samples[selectedSampIndex]->setType((Sample::sampleType) 3);
            } else if (key == 'e' || key == 'E') {
                if (samples[selectedSampIndex]->getName() != "") {
                    samples[selectedSampIndex]->setEnabled(!samples[selectedSampIndex]->isEnabled());
                }
            }
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
    
    if (!debug) {
        float vidWidthPercent;
        float vidHeightPercent;
        bool fullWidth = ofGetWidth() <= ofGetHeight();
        
        if (fullWidth) {
            int offset = (ofGetHeight() - vidRect.height) * 0.5;
            vidWidthPercent = float(x) / float(vidRect.width);
            vidHeightPercent = float(y - offset) / float(vidRect.height);
        } else {
            int offset = (ofGetWidth() - vidRect.width) * 0.5;
            vidWidthPercent = float(x - offset) / float(vidRect.width);
            vidHeightPercent = float(y) / float(vidRect.height);
        }
        
        if (selectedSampIndex != -1 && selectedSampIndex < samples.size()) {
            samples[selectedSampIndex]->setSelected(false);
        }
        
        shared_ptr<Sample> samp(new Sample(vidWidthPercent, vidHeightPercent));
        samp->setSelected(true);
        samples.push_back(samp);
        selectedSampIndex = samples.size() - 1;
    }
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
