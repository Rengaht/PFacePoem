#pragma once

#include "ofMain.h"
#include "ofxFaceTracker.h"
#include "FrameTimer.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	
	ofVideoGrabber cam;
	ofxFaceTracker tracker;
	ExpressionClassifier classifier;

	FrameTimer _detect_timer;
	enum PStatus {SLEEP,DETECT};
	PStatus _status;
 
};
