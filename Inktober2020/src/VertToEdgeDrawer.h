// Arianna Rudawski 2020
// www.ariannarudawski.com

#pragma once
#include "LineDrawer.h"

class VertToEdgeDrawer : public LineDrawer
{
public:

	VertToEdgeDrawer(ofParameterGroup * mainGroup);

	virtual void draw(vector<vector<ofPolyline>> charOutlines, bool debug);

private:

	struct comp
	{
		bool operator() (const float& lhs, const float&rhs) const
		{
			return lhs > rhs;
		}
	};

	ofPolyline findShortestLineToEdge(ofPoint vert);
};

