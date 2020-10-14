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
	void onToggleDrawCurves(bool & newDrawCurves);

	ofParameter<bool> drawMatchsticks;
	void DrawMatchsticksFromString(string word, ofPoint position);
	void onToggleDrawMatchsticks(bool & newDrawMatchsticks);

	ofParameter<bool> drawStripes;
	void DrawStripesFromString(string word, ofPoint position);
	void onToggleDrawStripes(bool & newDrawMatchsticks);

	// curves

	ofParameterGroup curvesGroup;

	ofParameter<bool> curveDepthNoiseOn;
	ofParameter<bool> curveSizeNoiseOn;
	ofParameter<ofVec2f> curveSize;
	ofParameter<float> curveDepth;
	ofParameter<int> curveNumLines;
	ofParameter<ofVec2f> curveDeltaSize;
	ofParameter<float> curveNoiseTime;
	ofParameter<float> curveNoiseScale;
	ofParameter<float> curveNoiseResolution;

	void onUpdateDepthNoiseFlag(bool & newVal);
	void onUpdateSizeNoiseFlag(bool & newVal);

	// matchsticks

	ofParameterGroup matchsticksGroup;

	ofParameter<bool> matchsticksClockwise;
	ofParameter<int> matchsticksRays;
	ofParameter<int> matchsticksRayDistance;
	ofParameter<int> matchsticksRaySourceSpacing;
	ofParameter<float> matchstickDepth;
	ofParameter<float> matchstickSourceDepth;
	ofParameter<float> matchstickDestDepth;

	// stripes

	ofParameterGroup stripesGroup;

	ofParameter<int> stripesDensity;
	ofParameter<int> stripesSteps;

};
