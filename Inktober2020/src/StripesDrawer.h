// Arianna Rudawski 2020
// www.ariannarudawski.com

#pragma once
#include "LineDrawer.h"

class StripesDrawer : public LineDrawer
{
public:

	StripesDrawer(ofParameterGroup * mainGroup);

	virtual void setup(ofApp * app);

	virtual void draw(vector<vector<ofPolyline>> charOutlines, bool debug, bool sortedVerts);

private:

	ofParameter<bool> stripesOutsideString;
	ofParameter<bool> stripesFlipByChar;
	ofParameter<int> stripesDensity;
	ofParameter<int> stripesSteps;
	ofParameter<bool> stripesNoiseOn;
	ofParameter<float> stripesNoiseTime;
	ofParameter<float> stripesNoiseScale;
	ofParameter<float> stripesNoiseResolution;

	void onUpdateStripesNoiseFlag(bool & newVal);
};

