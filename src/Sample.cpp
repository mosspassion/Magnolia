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

}

Sample::Sample(float vidWidthPercent, float vidHeightPercent) {
    _vidPercent.set(vidWidthPercent, vidHeightPercent);
    _type = sampleType::LIGHTNESS;
    _radius = 10;
    _valsIndex = 0;
    _selected = false;
    _smooth = 0;
    _name = "";
    _smoothedVals.push_back(SmoothedFloat(1, 0));
    _smoothedVals.push_back(SmoothedFloat(1, 0));
    _smoothedVals.push_back(SmoothedFloat(1, 0));
    _scaleBounds.set(0, 255);
    setSmooth(1);
}

void Sample::update(const ofRectangle& vidRect,
                    const ofVec2f& vidSize,
                    const ofPixels& pixels) {
    
    _pos.set(vidRect.x + (vidRect.width * _vidPercent.x),
             vidRect.y + (vidRect.height * _vidPercent.y));
    
    // parse r, g, b, a values
    ofColor c = pixels.getColor(vidSize.x * _vidPercent.x,
                                vidSize.y * _vidPercent.y);
    
    _smoothedVals[0].addValue(c.r);
    _smoothedVals[1].addValue(c.g);
    _smoothedVals[2].addValue(c.b);
    
    val[0] = int(_smoothedVals[0].getValue());
    val[1] = int(_smoothedVals[1].getValue());
    val[2] = int(_smoothedVals[2].getValue());
    // http://stackoverflow.com/questions/596216/formula-to-determine-brightness-of-rgb-color
    // https://en.wikipedia.org/wiki/Relative_luminance
    val[3] = int((0.2126 * val[0] + 0.7152 * val[1] + 0.0722 * val[2])); // luminance
    
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

void Sample::setSmooth(unsigned int numSamples) {
    _smooth = numSamples;
    for (auto& val : _smoothedVals) {
        val.resizeBuffer(numSamples);
    }
};

// receives empty vector and populates it
void Sample::fillValsByType(vector<float>& vec, sampleType type, unsigned int start, unsigned int end) {
    if (start < BUFFER_SIZE && end < BUFFER_SIZE) {
            while (start < end) {
            switch (type) {
                case sampleType::LIGHTNESS: vec.push_back(vals[start][3]); break;
                case sampleType::RED: vec.push_back(vals[start][0]); break;
                case sampleType::GREEN: vec.push_back(vals[start][1]); break;
                case sampleType::BLUE: vec.push_back(vals[start][2]); break;
            }
            start++;
        }
    }
};

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