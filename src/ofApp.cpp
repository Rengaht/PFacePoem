#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

float ofApp::CameraScale=0;

void ofApp::setup(){

	ofSetVerticalSync(true);
	ofHideCursor();

	cam.setup(CAM_WIDTH, CAM_HEIGHT);
	CameraScale=max((float)ofGetWidth()/CAM_WIDTH,(float)ofGetHeight()/CAM_HEIGHT);

	loadXmlSetting();
	classifier.load("expressions");

	cout<<"listening for osc message on port "<<PORT<<"\n";
	_receiver.setup(PORT);

	tracker.setup();
	_timer_detect=FrameTimer(3000);
	_timer_blink=FrameTimer(1000);
	_timer_blink.restart();

	_status=PStatus::SLEEP;

	_last_millis=ofGetElapsedTimeMillis();

	_shader_blur.load("shadersES2/shaderBlurX");
	_shader_glitch.load("shadersES2/glitch");

	_fbo1.allocate(ofGetWidth(),ofGetHeight());
	_fbo2.allocate(ofGetWidth(),ofGetHeight());
	
}

void ofApp::update(){
	
	if(_status!=PStatus::POEM){
		cam.update();
		cam.getPixelsRef().mirror(true,false);
		if(cam.isFrameNew()){
			if(tracker.update(toCv(cam))){
				classifier.classify(tracker);
			}
		}
	}

	float dt_=ofGetElapsedTimeMillis()-_last_millis;
	_last_millis+=dt_;
	_timer_blink.update(dt_);
	if(_timer_blink.val()==1) _timer_blink.restart();

	bool found_=tracker.getFound();

	switch(_status){
		case SLEEP:
			if(found_) setStatus(PStatus::DETECT);
			break;
		case DETECT:
			_timer_detect.update(dt_);
			if(found_){
				_face_pos=tracker.getPosition();
				_mood=classifier.getProbability(0);

				auto mesh_=tracker.getMeanObjectMesh();
				ofVec3f size_=getMeshBounding(mesh_);
				float scale_=tracker.getScale();
				size_*=scale_;

				_face_rect=ofRectangle(_face_pos.x-size_.x/2,_face_pos.y-size_.y/2,size_.x,size_.y);
				if(_timer_detect.val()==1) setStatus(PStatus::POEM);
			}else{
				setStatus(PStatus::SLEEP);
			}
			break;
	}
	updateOsc();

}
void ofApp::setStatus(PStatus set_){
	_status=set_;
	ofLog()<<"set status= "<<set_;
	switch(set_){
		case SLEEP:
			_timer_blink.restart();
			break;
		case DETECT:
			_status=PStatus::DETECT;
			_timer_detect.restart();
			ofLog()<<"Face detect!!!";
			break;
		case POEM:
			sendFace(_mood);
			break;
	}
}
void ofApp::updateOsc(){
	if(_receiver.hasWaitingMessages()){
		ofxOscMessage m;
		_receiver.getNextMessage(m);
		ofLog()<<"get message: "<<m.getAddress();
		if(m.getAddress()=="/reset") setStatus(PStatus::SLEEP);
	}
}

void ofApp::sendFace(float mood_){
	
	if(mood_>0.75) mood_=0;
	else mood_=1.0-mood_;

	ofLog()<<"send face: "<<mood_<<" to "<<_str_ip[0];
	ofxOscSender sender_;
	sender_.setup(_str_ip[0],PORT);

	ofxOscMessage message_;
	message_.setAddress("/face");
	message_.addFloatArg(mood_);

	sender_.sendMessage(message_);
}

void ofApp::draw(){

	float offx=5;
	float offy=10;
	float line_=10;

	_fbo1.begin();
	ofClear(0);


	ofPushMatrix();
	ofTranslate(ofGetWidth()*.5-CameraScale*CAM_WIDTH*.5,ofGetHeight()*.5-CameraScale*CAM_HEIGHT*.5);
	ofScale(CameraScale,CameraScale);

	
	ofSetColor(255);

	cam.draw(0,cam.getHeight(),cam.getWidth(),-cam.getHeight());
	

	/* detect frame */
	ofPushStyle();
	ofSetColor(255,255.0*_timer_blink.valFade());
	ofNoFill();
	//ofSetLineWidth(5);
	if(tracker.getFound()){ 
		ofPushMatrix();
		ofTranslate(_face_rect.getTopLeft());
			ofDrawRectangle(0,0,_face_rect.width,_face_rect.height);
		ofPopMatrix();
	}else{
		if(_status==PStatus::SLEEP){
			float p_=_timer_blink.valFade();
			p_=(p_*.2+.8)*CAM_HEIGHT/5.0;
			ofPushMatrix();
			ofTranslate(CAM_WIDTH/2,CAM_HEIGHT/2);
				ofDrawRectangle(-p_/2,-p_/2,p_,p_);
			ofPopMatrix();
		}
	}

	ofPopStyle();
	ofPopMatrix();

	_fbo1.end();

	_fbo2.begin();
	ofClear(0);
	_shader_glitch.begin();	
	_shader_glitch.setUniform1f("amount",3);
	_shader_glitch.setUniform1f("phi",ofRandom(-100,100));
	_shader_glitch.setUniform1f("angle",PI*sin(ofGetFrameNum()%40/40.0+ofRandom(-10,10)));
	_shader_glitch.setUniform1f("windowWidth",ofGetWidth());
	_shader_glitch.setUniform1f("windowHeight",ofGetHeight());
	_shader_glitch.setUniformTexture("tex0",_fbo1.getTexture(),0);
		_fbo1.draw(0,0);
	_fbo2.end();
	_fbo1.begin();
	ofClear(0);
	_shader_blur.begin();
	_shader_blur.setUniform1f("windowWidth",ofGetWidth());
	_shader_blur.setUniform1f("windowHeight",ofGetHeight());
	_shader_blur.setUniformTexture("tex0",_fbo2.getTexture(),0);
		_fbo2.draw(0,0);
	_shader_blur.end();
	_fbo1.end();
	_fbo1.draw(0,0);

	string hint_="";
	switch(_status){
		case SLEEP:
			hint_="search for face...";
			break;
		case DETECT:
			hint_="processing...";
			break;
		case POEM:
			hint_="on display...";
			break;
	}


	ofPushStyle();
	ofSetColor(255);
		ofDrawBitmapString(ofToString(ofGetFrameRate()),offx,offy+=line_);
		ofDrawBitmapString(hint_,offx,offy+=line_);
		ofDrawBitmapString("mood= "+ofToString(_mood),offx,offy+=line_);
	/*if(tracker.getFound()){
	ofPushMatrix();
	ofTranslate(ofGetWidth()*.5-CameraScale*CAM_WIDTH*.5,ofGetHeight()*.5-CameraScale*CAM_HEIGHT*.5);
	ofTranslate(_face_rect.getBottomLeft()*CameraScale);
		int n = classifier.size();
		ofDrawBitmapString("pos= "+ofToString(_face_pos),0,line_);		
		for(int i=0;i<n;++i){
			ofDrawBitmapString(classifier.getDescription(i)+"= "+ofToString(classifier.getProbability(i)),0,(i+2)*line_);
		}
	ofPopMatrix();
	}*/
	ofPopStyle();

}

void ofApp::keyPressed(int key) {
/*	if(key == 'r') {
		tracker.reset();
		classifier.reset();
	}
	if(key == 'e') {
		classifier.addExpression();
	}
	if(key == 'a') {
		classifier.addSample(tracker);
	}
	if(key == 's') {
		classifier.save("expressions");
	}
	if(key == 'l') {
		classifier.load("expressions");
	}*/
	switch(key){
		case 'r':
		case 'R':
			setStatus(PStatus::SLEEP);
			break;
	}

}

ofVec3f ofApp::getMeshBounding(const ofMesh& mesh){
	auto xExtreme=minmax_element(mesh.getVertices().begin(),mesh.getVertices().end(),
					[](const ofPoint& lhs,const ofPoint& rhs){
						return lhs.x<rhs.x;
					});
	auto yExtreme=minmax_element(mesh.getVertices().begin(),mesh.getVertices().end(),
					[](const ofPoint& lhs,const ofPoint& rhs){
						return lhs.y<rhs.y;
					});
	auto zExtreme=minmax_element(mesh.getVertices().begin(),mesh.getVertices().end(),
					[](const ofPoint& lhs,const ofPoint& rhs){
						return lhs.z<rhs.z;
					});
	return ofVec3f(xExtreme.second->x-xExtreme.first->x,
			yExtreme.second->y-yExtreme.first->y,
			zExtreme.second->z-zExtreme.first->z);
	
}

void ofApp::exit(){
	tracker.stopThread();
	tracker.waitForThread();
}

void ofApp::loadXmlSetting(){
	ofxXmlSettings param_;
	param_.loadFile("Pdata.xml");
	
	_str_ip.push_back(param_.getValue("IP_BROADCAST",""));
	/*_str_ip.push_back(param_.getValue("IP_FACE",""));
	_str_ip.push_back(param_.getValue("IP_POEM",""));
	_str_ip.push_back(param_.getValue("IP_DISPLAY1",""));
	_str_ip.push_back(param_.getValue("IP_DISPLAY2",""));
	_str_ip.push_back(param_.getValue("IP_DISPLAY3",""));
	*/
}
