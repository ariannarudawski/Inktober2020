// Arianna Rudawski 2020
// www.ariannarudawski.com

#include "UnknownPleasuresDrawer.h"

UnknownPleasuresDrawer::UnknownPleasuresDrawer(ofParameterGroup * mainGroup)
	: LineDrawer(mainGroup)
{
	lineGroup.setName("UNKNOWN PLEASURES");

	lineGroup.add(vertical.set("vertical", false));
	lineGroup.add(stripesDensity.set("stripe density", 100, 1, 100));
	lineGroup.add(stripesSteps.set("stripe step", 100, 1, 100));
	lineGroup.add(pointOffset.set("inside char point offset", ofVec2f(0, 30), ofVec2f(-100, -100), ofVec2f(100, 100)));

	lineGroup.add(stripesNoiseOn.set("use noise", false));
	stripesNoiseTime.set("noise time", 0, 0, 1000);
	stripesNoiseScale.set("noise scale", 1, 0, 100);
	stripesNoiseResolution.set("noise resolution", 0.5, 0, 1);
	if (stripesNoiseOn.get())
	{
		lineGroup.add(stripesNoiseTime);
		lineGroup.add(stripesNoiseScale);
		lineGroup.add(stripesNoiseResolution);
	}

	stripesNoiseOn.addListener(this, &UnknownPleasuresDrawer::onUpdateStripesNoiseFlag);

}

void UnknownPleasuresDrawer::setup(ofApp * app)
{
	LineDrawer::setup(app);

	// curve listeners

	stripesNoiseOn.addListener(this, &UnknownPleasuresDrawer::onUpdateStripesNoiseFlag);
	stripesNoiseOn.addListener(app, &ofApp::onUpdateBool);
}

void UnknownPleasuresDrawer::draw(vector<vector<ofPolyline>> charOutlines, bool debug)
{
	if (!drawLines.get())
		return;

	int numLines = vertical.get() ? ofGetHeight() / stripesDensity : ofGetWidth() / stripesDensity;
	int stripeCheckLength = vertical.get() ? ofGetWidth() : ofGetHeight();

	vector<ofPolyline> bLines;

	for (int line = 0; line < numLines; ++line)
	{
		ofPolyline bLine;
		int changingAxis = (line + 0.5f) * stripesDensity;

		for (int staticAxis = 0; staticAxis <= stripeCheckLength;)
		{
			ofPoint point = vertical ? ofPoint(changingAxis, staticAxis) : ofPoint(staticAxis, changingAxis);

			bool insideChar = false;
			
			for (int c = 0; c < charOutlines.size(); c++)
			{
				if (isInsideChar(point, charOutlines[c]))
				{
					insideChar = true;
					break;
				}
			}
			
			if (insideChar)
			{
				bLine.addVertex(point + pointOffset.get());
			}
			else
			{
				bLine.addVertex(point);
			}

			// make sure this stripe reaches all the way to the edge
			if (staticAxis < stripeCheckLength)
			{
				staticAxis += stripesSteps;

				if (staticAxis > stripeCheckLength)
					staticAxis = stripeCheckLength;
			}
			else
			{
				staticAxis += stripesSteps; 
			}
		}

		bLines.push_back(bLine);
	}

	// draw line

	ofSetColor(0);

	for (int l = 0; l < bLines.size(); ++l)
	{
		bLines[l].draw();
	}
}

void UnknownPleasuresDrawer::onUpdateStripesNoiseFlag(bool & newVal)
{
	if ((newVal) && !lineGroup.contains(stripesNoiseTime.getName()))
	{
		lineGroup.add(stripesNoiseTime);
		lineGroup.add(stripesNoiseScale);
		lineGroup.add(stripesNoiseResolution);
	}
	else
	{
		lineGroup.remove(stripesNoiseTime);
		lineGroup.remove(stripesNoiseScale);
		lineGroup.remove(stripesNoiseResolution);
	}
}

bool UnknownPleasuresDrawer::isInsideChar(ofPoint point, vector<ofPolyline> charOutlines)
{
	if (charOutlines.size() < 1)
	{
		return false;
	}
	else if (charOutlines.size() == 1)
	{
		return charOutlines[0].inside(point);
	}
	else if (!charOutlines[0].inside(point))
	{
		return false;
	}
	else
	{
		ofPolyline mainOutline = charOutlines[0];

		for (int i = 1; i < charOutlines.size(); ++i)
		{
			if (charOutlines[i].inside(point))
				return false;
		}

		return true;
	}
}


