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

	ofParameter<bool> noiseOn;
	ofParameter<float> noiseTime;
	ofParameter<float> noiseScale;
	ofParameter<float> noiseResolution;

	void onToggleNoiseOn(bool & newVal);

};

