// Arianna Rudawski 2020
// www.ariannarudawski.com

#pragma once
#include "LineDrawer.h"

class VertToEdgeDrawer : public LineDrawer
{
public:

	VertToEdgeDrawer(ofParameterGroup * mainGroup);

	virtual void setup(ofApp * app);
	virtual void draw(vector<vector<ofPolyline>> charOutlines, bool debug);

private:

	struct comp
	{
		bool operator() (const float& lhs, const float&rhs) const
		{
			return lhs > rhs;
		}
	};

	ofPolyline findLineToEdge(ofPoint vert);

	// parameters

	ofParameter<bool> shortestLine;
	ofParameter<bool> useCircularEdge;
	ofParameter<float> circularEdgeRadius;
	ofParameter<int> numVertsInCircularEdge;

	void onToggleCircularEdge(bool & newVal);

};

