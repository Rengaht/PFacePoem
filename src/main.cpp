#include "ofApp.h"


int main() {
//    ofSetupOpenGL(800, 480, OF_WINDOW);
//    return ofRunApp(std::make_shared<ofApp>());

	ofGLESWindowSettings settings;
	settings.glesVersion=2;
	ofCreateWindow(settings);

	ofRunApp(new ofApp());

}
