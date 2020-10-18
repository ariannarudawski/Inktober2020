// Arianna Rudawski 2020
// www.ariannarudawski.com

#pragma once
#include "ofApp.h"

class LineDrawer
{
public:

	LineDrawer(ofParameterGroup * mainGroup_) 
	{
		mainGroup = mainGroup_;
	};

	virtual void setup(ofApp * app)
	{
		drawLines.addListener(this, &LineDrawer::onToggleDrawLines);
		drawLines.addListener(app, &ofApp::onUpdateBool);
	};

	virtual void draw(vector<vector<ofPolyline>> charOutlines, bool debug) = 0;

	string getName()
	{
		return lineGroup.getName();
	}

	ofParameter<bool> drawLines;

	void onToggleDrawLines(bool & newDrawCurves)
	{
		if (newDrawCurves)
		{
			mainGroup->add(lineGroup);
		}
		else
		{
			mainGroup->remove(lineGroup);
		}
	}

protected:

	ofParameterGroup * mainGroup;
	ofParameterGroup lineGroup;
};

