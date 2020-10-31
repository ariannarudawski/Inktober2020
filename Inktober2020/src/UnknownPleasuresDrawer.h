// Arianna Rudawski 2020
// www.ariannarudawski.com

#pragma once
#include "LineDrawer.h"

class UnknownPleasuresDrawer : public LineDrawer
{
public:

	UnknownPleasuresDrawer(ofParameterGroup * mainGroup);

	virtual void draw(vector<vector<ofPolyline>> charOutlines, bool debug, bool sortedVerts);

private:

	ofParameter<bool> vertical;
	ofParameter<int> stripesDensity;
	ofParameter<int> stripesSteps;

	ofParameter<float> noiseTime;
	ofParameter<float> outsideNoiseScale;
	ofParameter<float> insideNoiseScale;
	ofParameter<ofVec2f> noiseResolution;
	ofParameter<int> curveResolution;
	ofParameter<int> numLinesToCheckForIntersection;

	vector<ofPolyline> choppedLines;
	bool redoLines = true;

	bool isInsideChar(ofPoint point, vector<ofPolyline> charOutlines);
	bool pointIntersectsExistingLines(ofPoint, vector<ofPolyline> lines);

	void onBoolParamsChanged(bool & newVal);
	void onIntParamsChanged(int & newVal);
	void onFloatParamsChanged(float & newVal);
	void onofVec2fParamsChanged(ofVec2f & newVal);

};

