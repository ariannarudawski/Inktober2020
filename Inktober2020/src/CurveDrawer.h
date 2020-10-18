// Arianna Rudawski 2020
// www.ariannarudawski.com

#pragma once
#include "LineDrawer.h"

class CurveDrawer : public LineDrawer
{
public:

	CurveDrawer(ofParameterGroup * mainGroup);

	virtual void setup(ofApp * app);

	virtual void draw(vector<vector<ofPolyline>> charOutlines, bool debug);

private:

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
};

