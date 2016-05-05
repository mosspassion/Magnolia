#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0);
    player.load("fingers.mov");
    player.setLoopState(OF_LOOP_NORMAL);
    player.play();
    debug = false;
}

//--------------------------------------------------------------
void ofApp::update(){
    player.update();
    
    for (auto sample : samples) {
        sample->update(vidRect,
                       ofVec2f(player.getWidth(), player.getHeight()),
                       player.getPixels());
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
                int x = 0;
                int sampWidth = ofGetWidth() / Sample::bufferSize;
                for (int i = 0; i < Sample::bufferSize; i++) {
                    ofPushStyle();
                    ofSetColor(255, 0, 0);
                    ofDrawRectangle(x, (graphHeight * sampNum) - ofMap(sample->vals[i][0], 0, 255, 0, graphHeight), sampWidth, 1);
                    ofSetColor(0, 255, 0);
                    ofDrawRectangle(x, (graphHeight * sampNum) - ofMap(sample->vals[i][1], 0, 255, 0, graphHeight), sampWidth, 1);
                    ofSetColor(0, 0, 255);
                    ofDrawRectangle(x, (graphHeight * sampNum) - ofMap(sample->vals[i][2], 0, 255, 0, graphHeight), sampWidth, 1);
                    ofSetColor(255);
                    ofDrawRectangle(x, (graphHeight * sampNum) - ofMap(sample->vals[i][3], 0, 255, 0, graphHeight), sampWidth, 1);
                    ofPopStyle();
                    x+=sampWidth;
                }
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
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
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
    
    shared_ptr<Sample> samp(new Sample(vidWidthPercent, vidHeightPercent));
    samples.push_back(samp);
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
