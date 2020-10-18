// Arianna Rudawski 2020
// www.ariannarudawski.com

#pragma once
#include "LineDrawer.h"

class MatchsticksDrawer : public LineDrawer
{
public:

	MatchsticksDrawer(ofParameterGroup * mainGroup);

	virtual void draw(vector<vector<ofPolyline>> charOutlines, bool debug);

private:

	ofParameter<bool> matchsticksClockwise;

	ofParameter<int> matchsticksRays;
	ofParameter<int> matchsticksRayDistance;
	ofParameter<int> matchsticksRaySourceSpacing;

	ofParameter<float> matchstickDepth;
	ofParameter<float> matchstickSourceDepth;
	ofParameter<float> matchstickDestDepth;
};

