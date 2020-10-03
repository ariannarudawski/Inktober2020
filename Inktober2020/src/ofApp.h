// Arianna Rudawski 2020
// www.ariannarudawski.com

#pragma once
#include "ofMain.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp
{

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key) {};
	void mouseMoved(int x, int y) {};
	void mouseDragged(int x, int y, int button) {};
	void mousePressed(int x, int y, int button) {};
	void mouseReleased(int x, int y, int button) {};
	void mouseEntered(int x, int y) {};
	void mouseExited(int x, int y) {};
	void windowResized(int w, int h) {};
	void dragEvent(ofDragInfo dragInfo) {};
	void gotMessage(ofMessage msg) {};

private:

	// debug

	bool debug = false;

	ofxPanel gui;
	ofParameterGroup mainGroup;

	// string

	ofTrueTypeFont font;
	ofParameter<string> stringToDraw;
	ofParameter<bool> drawInnerLines;

	ofParameter<ofVec2f> position;
	ofParameter<float> spacing;
	ofParameter<int> size;

	void onUpdateSize(int & newSize);
	void DrawString(string word, ofPoint position);

	// flower petals

	ofParameter<ofVec2f> loopSize;
	ofParameter<float> loopDepth;

	ofParameter<int> numLines;
	ofParameter<ofVec2f> deltaSize;
};
