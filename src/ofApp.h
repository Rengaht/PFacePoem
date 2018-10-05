#pragma once

#define CAM_WIDTH 320
#define CAM_HEIGHT 240

#define PORT 12345

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxOsc.h"
#include "ofxFaceTracker.h"
#include "FrameTimer.h"

#include "ofxFaceTrackerThreaded.h"

class ofApp : public ofBaseApp {
public:
	static float CameraScale;

	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void exit();
	
	ofVideoGrabber cam;
	//ofxFaceTracker tracker;
	ofxFaceTrackerThreaded tracker;

	ExpressionClassifier classifier;


	float _last_millis;
	FrameTimer _timer_detect,_timer_blink;

	enum PStatus {SLEEP,DETECT,POEM};
	PStatus _status;
	void setStatus(PStatus set_);	

	ofVec2f _face_pos;
	ofRectangle _face_rect;
	ofVec3f getMeshBounding(const ofMesh& mesh);
	
	ofxOscReceiver _receiver;
	void updateOsc();
	void sendFace(float mood_);
	
	vector<string> _str_ip;
	void loadXmlSetting();

	float _mood;

	ofShader _shader_blur,_shader_glitch;
	ofFbo _fbo1,_fbo2;

};
