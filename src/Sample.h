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

#define BUFFER_SIZE 512

class Sample {
public:
    
    static const unsigned int bufferSize;
    
    enum sampleType {
        RED,
        GREEN,
        BLUE,
        VAL
    } type;
    
    Sample();
    Sample(float vidWidthPercent, float vidHeightPercent);
    
    void update(const ofRectangle& vidRect,
                const ofVec2f& vidSize,
                const ofPixels& pixels);
    void draw();
    void mouseDown();
    
    ofVec4f val;
    ofVec4f vals[BUFFER_SIZE];
    
protected:
    
    void _drawCrosshair();
    void _drawText();
    int _radius;
    unsigned int _valsIndex;
    ofVec3f _pos;
    ofVec2f _vidPercent;
    
};


#endif /* defined(__Magnolia__Sample__) */
