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

	bool debug = true;
	ofxPanel gui;
	ofParameterGroup mainGroup;

	// string

	ofParameterGroup stringGroup;

	ofTrueTypeFont font;
	ofParameter<string> stringToDraw;
	ofParameter<bool> drawInnerLines;

	ofParameter<ofVec2f> position;
	ofParameter<int> size;
	ofParameter<float> letterSpacing;
	ofParameter<int> spacing;

	void onUpdateSize(int & newSize);

	// types of lines

	ofParameter<bool> drawCurves;
	void DrawCurvesFromString(string word, ofPoint position);
	void onToggleDrawCurves(bool & drawCurves);

	// curves

	ofParameterGroup curvesGroup;

	ofParameter<bool> depthNoiseOn;
	ofParameter<bool> sizeNoiseOn;
	ofParameter<ofVec2f> loopSize;
	ofParameter<float> loopDepth;
	ofParameter<int> numLines;
	ofParameter<ofVec2f> deltaSize;
	ofParameter<float> curveNoiseTime;
	ofParameter<float> curveNoiseScale;
	ofParameter<float> curveNoiseResolution;

	void onUpdateDepthNoiseFlag(bool & newVal);
	void onUpdateSizeNoiseFlag(bool & newVal);
};
