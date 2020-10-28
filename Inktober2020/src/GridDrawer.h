// Arianna Rudawski 2020
// www.ariannarudawski.com

#pragma once
#include "LineDrawer.h"
#include "ofxClipper.h"

class GridDrawer : public LineDrawer
{
public:

	GridDrawer(ofParameterGroup * mainGroup);

	virtual void draw(vector<vector<ofPolyline>> charOutlines, bool debug);
	virtual void setup(ofApp * app);

private:

	struct GridSpace
	{
		ofRectangle gridBounds;
		vector<ofPolyline> lines;
	};

	bool intersectsChar(ofRectangle rect, vector<vector<ofPolyline>> charOutlines);
	bool intersectsChar(ofPoint point, vector<ofPolyline> charOutlines);

	ofParameter<int> columns;
	ofParameter<int> rows;
	ofParameter<ofVec2f> inset;
	ofParameter<bool> bricks;

	vector<ofPolyline> lines;
	bool redoLines = true;

	void onBoolParamsChanged(bool & newVal);
	void onIntParamsChanged(int & newVal);
	void onFloatParamsChanged(float & newVal);
	void onofVec2fParamsChanged(ofVec2f & newVal);
	void onStringParamsChanged(string & newVal);

	// clipper

	ofx::CClipper clipper;

};

