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
    windowClickPoints.set(-1);
}

//--------------------------------------------------------------
void ofApp::update(){
    player.update();
    for (auto sample : samples) {
       
        sample->update(vidRect,
                       ofVec2f(player.getWidth(), player.getHeight()),
                       player.getPixels());
        
        if (sample->isEnabled()) {
            
            // send messages to main address/name
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
            m.addFloatArg(ofMap(val, 0, 255, 0, 1));
            sender.sendMessage(m, false);
            
            // send messages to aliases
            for (const auto& alias: sample->getAliases()) {
                m.clear();
                m.setAddress(alias.address);
                switch (alias.type) {
                    case Sample::sampleType::LIGHTNESS: val = sample->val[3]; break;
                    case Sample::sampleType::RED: val = sample->val[0]; break;
                    case Sample::sampleType::GREEN: val = sample->val[1]; break;
                    case Sample::sampleType::BLUE: val = sample->val[2]; break;
                }
                m.addIntArg(val);
                m.addFloatArg(ofMap(val, 0, 255, 0, 1));
                sender.sendMessage(m, false);
            }
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
                if (sample->isSelected()) ofSetColor(255, 255, 0);
                ofDrawBitmapString("ALIASES:", 15, graphHeight * (sampNum - 1) + 30);
                ofDrawBitmapString("WINDOW:", ofGetWidth()/2 - 40, graphHeight * (sampNum - 1) + 15);
                ofSetColor(255);
//                ofNoFill();
//                ofDrawRectangle(aliasFields[sampNum - 1]->bounds);
                ofFill();
                aliasFields[sampNum - 1]->draw();
                windowFields[sampNum - 1]->draw();
                ofPopStyle();
                sampNum++;
            }
            if (windowClickPoints.x != -1 && windowClickPoints.y != -1 &&
                windowClickPoints.z == -1 && windowClickPoints.w == -1) {
                cout << "in here" << endl;
                ofPushStyle();
                ofSetColor(255, 80);
                int graphHeight = ofGetHeight() / samples.size();
                int sampleIndex = windowClickPoints.y / graphHeight;
                ofDrawRectangle(windowClickPoints.x,
                                graphHeight * sampleIndex,
                                mouseX - windowClickPoints.x,
                                graphHeight);
                ofPopStyle();
            }
        }
    }
}

void ofApp::updateFieldPositions() {
    int graphHeight = ofGetHeight() / aliasFields.size();
    for (int i = 0; i < aliasFields.size(); i++) {
        int y = graphHeight * i + 15;;
        aliasFields[i]->bounds.y = y;
        windowFields[i]->bounds.y = y - 15;
        windowFields[i]->bounds.x = ofGetWidth()/2 + 15;
    }
}

void ofApp::onAliasFieldTextChanged(const void * sender, string& text) {
    for (int i = 0; i < aliasFields.size(); i++) {
        // what is the index of this sample?
        if (((ofxTextInputField*) sender) == &(*aliasFields[i])) {
            samples[i]->clearAliases();
            ofStringReplace(text, "\n", " ");
            
            auto aliases = ofSplitString(text, " ");
            for (string& a : aliases) {
                auto alias = createAliasAddress(a, samples[i]->getType());
                if (alias.address.length() > 0) {
                    samples[i]->addAlias(alias);
                }
            }
        }
    }
}

void ofApp::onWindowFieldTextChanged(const void * sender, string& text) {
    for (int i = 0; i < windowFields.size(); i++) {
        // what is the index of this sample?
        if (((ofxTextInputField*) sender) == &(*windowFields[i])) {
            auto window = createAliasAddress(text, samples[i]->getType());
            if (window.address.length() > 0) {
                samples[i]->setWindow(window);
                cout << "set window: " << window.address << " | " << window.type << endl;
            }
        }
    }
}

Sample::AliasAddress ofApp::createAliasAddress(string addressText, Sample::sampleType defaultType) {
    
    Sample::AliasAddress alias;
    
    if (addressText.length() > 1 && addressText[1] == ':') {
        switch (addressText[0]) {
            case 'r': case 'R':
                alias.type = Sample::sampleType::RED;
                break;
            case 'g': case 'G':
                alias.type = Sample::sampleType::GREEN;
                break;
            case 'b': case 'B':
                alias.type = Sample::sampleType::BLUE;
                break;
            case 'l': case 'L':
                alias.type = Sample::sampleType::LIGHTNESS;
                break;
            default:
                alias.type = defaultType;
                break;
        }
        addressText.erase(0, 2);
    } else {
        alias.type = defaultType;
    }
    
    if (addressText.length() > 0) {
        if (addressText[0] != '/') addressText.insert(addressText.begin(), '/');
        alias.address = addressText;
    }
    
    return alias;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if (key == OF_KEY_ALT) {
        debug = !debug;
        
        for (auto field : aliasFields) {
            debug ? field->enable() : field->disable();
        }
        
        for (auto field : windowFields) {
            debug ? field->enable() : field->disable();
        }
    }
    
    if (selectedSampIndex != -1 && selectedSampIndex < samples.size())
    {
        // backspace/delete
        if (key == 127)
        {
            if (ofGetKeyPressed(OF_KEY_SHIFT))
            {
                samples.erase(samples.begin() + selectedSampIndex);
                ofRemoveListener(aliasFields[selectedSampIndex]->textChanged,
                                 this,
                                 &ofApp::onAliasFieldTextChanged);
                aliasFields.erase(aliasFields.begin() + selectedSampIndex);
                ofRemoveListener(windowFields[selectedSampIndex]->textChanged,
                                 this,
                                 &ofApp::onWindowFieldTextChanged);
                windowFields.erase(windowFields.begin() + selectedSampIndex);
                updateFieldPositions();
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
            else if (!debug)
            {
                samples[selectedSampIndex]->removeCharFromName();
            }
        } else if (key == OF_KEY_RIGHT &&
                   !aliasFields[selectedSampIndex]->getIsEditing() &&
                   !windowFields[selectedSampIndex]->getIsEditing()) {
            samples[selectedSampIndex]->setSmooth(samples[selectedSampIndex]->getSmooth() + 1);
        } else if (key == OF_KEY_LEFT &&
                   samples[selectedSampIndex]->getSmooth() != 0 &&
                   !aliasFields[selectedSampIndex]->getIsEditing() &&
                   !windowFields[selectedSampIndex]->getIsEditing()) {
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
            if (key == OF_KEY_UP &&
                !aliasFields[selectedSampIndex]->getIsEditing() &&
                !windowFields[selectedSampIndex]->getIsEditing()) {
                int type = samples[selectedSampIndex]->getType();
                if (type < 3) {
                    type++;
                    samples[selectedSampIndex]->setType((Sample::sampleType) type);
                } else samples[selectedSampIndex]->setType((Sample::sampleType) 0);
            } else if (key == OF_KEY_DOWN &&
                       !aliasFields[selectedSampIndex]->getIsEditing() &&
                       !windowFields[selectedSampIndex]->getIsEditing()) {
                int type = samples[selectedSampIndex]->getType();
                if (type != 0) {
                    type--;
                    samples[selectedSampIndex]->setType((Sample::sampleType) type);
                } else samples[selectedSampIndex]->setType((Sample::sampleType) 3);
            } else if ((key == 'e' || key == 'E') &&
                       !aliasFields[selectedSampIndex]->getIsEditing() &&
                       !windowFields[selectedSampIndex]->getIsEditing()) {
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
        
        if (x != 0 && y != 0) {
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
            
            // add the alias input text fields
            shared_ptr<ofxTextInputField> field(new ofxTextInputField);
            field->setup();
            field->multiline = true;
            field->text = "";
            field->disable();
            field->bounds.x = 80;
            field->bounds.width = ofGetWidth() / 2.5;
            field->bounds.height = 20;
            ofAddListener(field->textChanged, this, &ofApp::onAliasFieldTextChanged);
            aliasFields.push_back(field);
            
            shared_ptr<ofxTextInputField> field2(new ofxTextInputField);
            field2->setup();
            field2->multiline = false;
            field2->text = "";
            field2->disable();
            field2->bounds.x = ofGetWidth()/2 + 15;
            field2->bounds.width = ofGetWidth() / 2.5;
            field2->bounds.height = 20;
            ofAddListener(field2->textChanged, this, &ofApp::onWindowFieldTextChanged);
            windowFields.push_back(field2);
            
            updateFieldPositions();
        }
    } else { // debug
        int windowBoxIndex = y / (ofGetHeight() / samples.size());
        if (selectedSampIndex != -1 && selectedSampIndex < samples.size() &&
            !aliasFields[windowBoxIndex]->bounds.inside(x, y) &&
            !windowFields[windowBoxIndex]->bounds.inside(x, y)) {
            windowClickPoints.x = x;
            windowClickPoints.y = y;
        }
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    if (debug) {
        if (windowClickPoints.x != -1 && windowClickPoints.y != -1)
        if (selectedSampIndex != -1 && selectedSampIndex < samples.size()) {
            windowClickPoints.z = x;
            windowClickPoints.w = y;
            int i = windowClickPoints.y / (ofGetHeight() / samples.size());
            if (samples[i]->getWindow().address.length() > 0) {
                vector<float> vec;
                samples[i]->fillValsByType(
                    vec, samples[i]->getType(),
                    ofMap(windowClickPoints.x, 0, ofGetWidth(), 0, Sample::bufferSize),
                    ofMap(windowClickPoints.z, 0, ofGetWidth(), 0, Sample::bufferSize)
                );
                ofxOscMessage m;
                m.setAddress(samples[i]->getWindow().address);
                for (float& v : vec) {
                    m.addIntArg(v);
                }
                sender.sendMessage(m);
                
            }
            windowClickPoints.set(-1);
        }
    }
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    updateFieldPositions();
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
