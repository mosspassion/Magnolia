//
//  Sample.cpp
//  Magnolia
//
//  Created by bdorse on 5/4/16.
//
//

#include "Sample.h"

const unsigned int Sample::bufferSize = BUFFER_SIZE;

Sample::Sample(){
    cout << "fuck me" << endl;
}

Sample::Sample(float vidWidthPercent, float vidHeightPercent) {
    _vidPercent.set(vidWidthPercent, vidHeightPercent);
    _type = sampleType::LIGHTNESS;
    _radius = 10;
    _valsIndex = 0;
    _selected = false;
    _name = "";
//    wtf = "fuckoff";
}

void Sample::update(const ofRectangle& vidRect,
                    const ofVec2f& vidSize,
                    const ofPixels& pixels) {
    
    _pos.set(vidRect.x + (vidRect.width * _vidPercent.x),
             vidRect.y + (vidRect.height * _vidPercent.y));
    
    // parse r, g, b, a values
    ofColor c = pixels.getColor(vidSize.x * _vidPercent.x,
                                vidSize.y * _vidPercent.y);
    
    val[0] = c.r;
    val[1] = c.g;
    val[2] = c.b;
    // http://stackoverflow.com/questions/596216/formula-to-determine-brightness-of-rgb-color
    // https://en.wikipedia.org/wiki/Relative_luminance
    val[3] = int((0.2126 * c.r + 0.7152 * c.g + 0.0722 * c.b)); // luminance
    
    vals[_valsIndex] = val;
    if (_valsIndex > BUFFER_SIZE - 1) {
        _valsIndex = 0;
    } else {
        _valsIndex++;
    }
}

void Sample::draw() {
    ofPushStyle();
    isSelected() ? ofSetColor(255, 255, 0) : ofSetColor(255);
    _drawCrosshair();
    _drawText();
    ofPopStyle();
}

void Sample::mouseDown() {
    
}

void Sample::addCharToName(char c) {
    setEnabled(false);
    _name += c;
}

void Sample::removeCharFromName() {
    setEnabled(false);
    _name = _name.substr(0, _name.size() - 1);
}

void Sample::updateName() {
    if (_name.length() > 0) {
        if (_name[0] != '/') {
            _name.insert(_name.begin(), '/');
        }
        setEnabled(true);
    }
}

void Sample::_drawText() {
    stringstream s;
    if (_name != "") s << _name << endl;
    if (_type == sampleType::RED)       s << "[R]: " << val[0] << ", " ;
    else                                s << "r: " << val[0] << ", ";
    if (_type == sampleType::GREEN)     s << "[G]: " << val[1] << ", \n" ;
    else                                s << "g: " << val[1] << ", \n";
    if (_type == sampleType::BLUE)      s << "[B]: " << val[2] << ", " ;
    else                                s << "b: " << val[2] << ", ";
    if (_type == sampleType::LIGHTNESS) s << "[L]: " << val[3] << ", " ;
    else                                s << "l: " << val[3] << ", ";
   
    ofDrawBitmapString(s.str(), _pos.x + 15, _pos.y + 15);
}

void Sample::_drawCrosshair() {
    ofNoFill();
    ofSetLineWidth(0.5);
    ofDrawCircle(_pos.x, _pos.y, _radius);
    ofDrawLine(_pos.x - 15, _pos.y, _pos.x + 15, _pos.y);
    ofDrawLine(_pos.x, _pos.y - 15, _pos.x, _pos.y + 15);
}