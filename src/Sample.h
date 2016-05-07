//
//  Sample.h
//  Magnolia
//
//  Created by bdorse on 5/4/16.
//
//

#ifndef __Magnolia__Sample__
#define __Magnolia__Sample__
#include "ofMain.h"
#include "SmoothedFloat.h"

#define BUFFER_SIZE 512

class Sample {
public:
    
    static const unsigned int bufferSize;
    
    enum sampleType {
        RED,
        GREEN,
        BLUE,
        LIGHTNESS
    };
    
    Sample();
    Sample(float vidWidthPercent, float vidHeightPercent);
    
    void update(const ofRectangle& vidRect,
                const ofVec2f& vidSize,
                const ofPixels& pixels);
    void draw();
    void mouseDown();
    void addCharToName(char c);
    void removeCharFromName();
    void updateName();
    
    void setSelected(bool sel) { _selected = sel; };
    void enable() { _enabled = true; };
    void setEnabled(bool e) { _enabled = e; };
    void setType(sampleType type) { _type = type; };
    void setSmooth(unsigned int numSamples);
    unsigned int getSmooth() { return _smooth; };
    bool isSelected() { return _selected; };
    bool isEnabled() { return _enabled; };
    const string& getName() { return _name; };
    const vector<string>& getAliases() { return _aliases; };
    sampleType getType() { return _type; };
    
    ofVec4f vals[BUFFER_SIZE];
    //WTF, if vals is declared as the last public variable shit hits the fan
    ofVec4f val;
    
protected:
    
    bool _selected;
    bool _enabled;
    void _drawCrosshair();
    void _drawText();
    int _radius;
    unsigned int _smooth;
    unsigned int _valsIndex;
    ofVec2f _pos;
    ofVec2f _vidPercent;
    string _name;
    vector<string> _aliases;
    sampleType _type;
    vector<SmoothedFloat> _smoothedVals;
    
};


#endif /* defined(__Magnolia__Sample__) */
