// Arianna Rudawski 2020
// www.ariannarudawski.com

#pragma once
#include "ofMain.h"
#include "ofxGui.h"

class LineDrawer;

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

	void UpdateGUI();

	void onUpdateBool(bool & newVal);
	void onUpdateInt(int & newVal);
	void onUpdateFloat(float & newVal);

	// debug

	bool debug = true;
	ofxPanel gui;
	ofParameterGroup mainGroup;

	// string

	ofParameterGroup stringGroup;

	ofTrueTypeFont font;
	ofParameter<string> stringToDraw;
	ofParameter<bool> drawInnerLines;
	ofParameter<bool> respace;

	ofParameter<ofVec2f> position;
	ofParameter<int> size;
	ofParameter<float> letterSpacing;
	ofParameter<int> spacing;
	ofParameter<bool> sortVertsByPosition;

	void onUpdateSpacingFlag(bool & newVal);
	void onUpdateSize(int & newSize);
	 
	// line drawing 

	ofParameterGroup linesGroup;

	vector<vector<ofPolyline>> GetAllCharOutlines();
	vector<LineDrawer*> lineDrawers;
};
