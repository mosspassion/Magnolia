//
//  SmoothedInt.h
//  Magnolia
//
//  Created by bdorse on 5/7/16.
//
//

#ifndef __Magnolia__SmoothedFloat__
#define __Magnolia__SmoothedFloat__
#include <vector>

using namespace std;
// Translated from here
// http://gamedevelopment.tutsplus.com/tutorials/quick-tip-use-the-ring-buffer-data-structure-to-smooth-jittery-values--gamedev-14373
class SmoothedFloat {
public:
    
    int lastIndex;
    float sum;
    bool enabled; // is the buffer size > 0?
    float val; // used only if not enabled
    vector<float> buffer;
    
    SmoothedFloat():
    lastIndex(0),
    val(0),
    enabled(false),
    sum(0)
    {
    }
    
    SmoothedFloat(int numSamples, float initVal){
        
        lastIndex = 0;
        
        if (numSamples > 0) {
            enabled = true;
            buffer.resize(numSamples);
            buffer.assign(numSamples, initVal);
            reset(initVal);
        } else {
            enabled = false;
        }
    }
    
    void reset(float value) {
        if (enabled) {
            sum = value * buffer.size();
            for (int i = 0; i < buffer.size(); ++i) {
                buffer[i] = value;
            }
        }
    }
    
    void resizeBuffer(int numSamples) {
        if (numSamples > 0) {
            enabled = true;
            int lastVal = buffer[lastIndex];
            //NOTE: Come back and make this transition take into account
            // the lastIndex to not disrupt average on resize. Ok for now.
            // needs to grow
            if (numSamples > buffer.size()) {
                buffer.resize(numSamples, lastVal);
            } else { // needs to shrink
                buffer.resize(numSamples, lastVal);
            }
            reset(lastVal);
        } else {
            enabled = false;
        }
    }
    
    void addValue(float value) {
        
        if (enabled) {
            sum -= buffer[lastIndex]; // subtract the oldest sample from the sum
            sum += value; // add the new sample
            buffer[lastIndex] = value; // store the new sample
            
            // advance the index and wrap it around
            lastIndex += 1;
            if (lastIndex >= buffer.size()) {
                lastIndex = 0;
            }
        }
        
        val = value; // used only for when disabled
    }
    
    float getValue() {
        if (enabled) return sum / buffer.size();
        else return val;
    }
};


#endif /* defined(__Magnolia__SmoothedFloat__) */
