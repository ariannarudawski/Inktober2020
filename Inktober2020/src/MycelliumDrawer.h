// Arianna Rudawski 2020
// www.ariannarudawski.com

#pragma once
#include "LineDrawer.h"

class MycelliumDrawer : public LineDrawer
{
public:

	MycelliumDrawer(ofParameterGroup * mainGroup);

	virtual void setup(ofApp * app);
	virtual void draw(vector<vector<ofPolyline>> charOutlines, bool debug, bool sortedVerts);

private:

	// parameters

	ofParameter<int> numVertsPerRing;
	ofParameter<int> numRingsPerChain;
	ofParameter<float> ringRadius;
	ofParameter<float> deltaRadius;
	ofParameter<float> ringSpacing;
	ofParameter<float> startAngleRad;

	ofParameter<bool> noiseOn;
	ofParameter<float> noiseTime;
	ofParameter<float> noiseScale;
	ofParameter<float> noiseResolution;
	void onToggleNoiseOn(bool & newVal);

	ofPolyline CreateRing(glm::vec2 center, float angleRad, float radius, int numVerts);
};

