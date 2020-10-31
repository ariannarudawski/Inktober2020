// Arianna Rudawski 2020
// www.ariannarudawski.com

#pragma once
#include "LineDrawer.h"
#include "ofxClipper.h"

class ClipperDrawer : public LineDrawer
{
public:

	ClipperDrawer(ofParameterGroup * mainGroup);

	virtual void draw(vector<vector<ofPolyline>> charOutlines, bool debug, bool sortedVerts);
	virtual void setup(ofApp * app);

private:

	// clipped lines

	ofx::CClipper clipper;
	vector<ofPolyline> lines;

	ofParameter<bool> drawClippedLines;

	ofParameter<bool> drawOffsetLines;
	ofParameter<int> numOffsetLines;
	ofParameter<float> deltaOffset;
	ofParameter<float> miter;
	ofParameter<bool> drawPipeOutline;
	ofParameter<int> offsetJoinType;
	ofParameter<int> offsetEndType;
	ofParameter<int> indexPipeStart;


	bool redoLines = true;

	void onBoolParamsChanged(bool & newVal);
	void onIntParamsChanged(int & newVal);
	void onFloatParamsChanged(float & newVal);
	void onofVec2fParamsChanged(ofVec2f & newVal);
	void onStringParamsChanged(string & newVal);

	void onToggleDrawOffset(bool & newVal);
	void onTogglePipeOutline(bool & newVal);

	ofPoint findClosestEdgePoint(ofPoint vert);

};

