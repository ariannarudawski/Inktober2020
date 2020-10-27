// Arianna Rudawski 2020
// www.ariannarudawski.com

#pragma once
#include "LineDrawer.h"

class CrissCrossDrawer : public LineDrawer
{
public:

	CrissCrossDrawer(ofParameterGroup * mainGroup);

	virtual void setup(ofApp * app);
	virtual void draw(vector<vector<ofPolyline>> charOutlines, bool debug);

private:

	ofParameter<bool> shuffle;

	ofParameter<float> crissCrossScale;

	ofParameter<bool> noiseOn;
	ofParameter<float> noiseTime;
	ofParameter<float> noiseScale;
	ofParameter<float> noiseResolution;

	ofPolyline GetLineForPoints(ofPoint vert, ofPoint towards, float & noiseIndex);

	void onToggleNoiseOn(bool & newVal);

};

