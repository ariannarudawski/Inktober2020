// Arianna Rudawski 2020
// www.ariannarudawski.com

#pragma once
#include "LineDrawer.h"

class UnknownPleasuresDrawer : public LineDrawer
{
public:

	UnknownPleasuresDrawer(ofParameterGroup * mainGroup);

	virtual void setup(ofApp * app);

	virtual void draw(vector<vector<ofPolyline>> charOutlines, bool debug);

private:

	ofParameter<bool> vertical;
	ofParameter<int> stripesDensity;
	ofParameter<int> stripesSteps;
	ofParameter<ofVec2f> pointOffset;

	ofParameter<bool> stripesNoiseOn;
	ofParameter<float> stripesNoiseTime;
	ofParameter<float> stripesNoiseScale;
	ofParameter<float> stripesNoiseResolution;

	void onUpdateStripesNoiseFlag(bool & newVal);

	bool isInsideChar(ofPoint point, vector<ofPolyline> charOutlines);
};

