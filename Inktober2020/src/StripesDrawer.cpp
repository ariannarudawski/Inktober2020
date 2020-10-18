// Arianna Rudawski 2020
// www.ariannarudawski.com

#include "StripesDrawer.h"

StripesDrawer::StripesDrawer(ofParameterGroup * mainGroup)
	: LineDrawer(mainGroup)
{
	lineGroup.setName("STRIPES");

	lineGroup.add(stripesFlipByChar.set("flip stripes by char", true));
	lineGroup.add(stripesOutsideString.set("draw stripes outside string", true));
	lineGroup.add(stripesDensity.set("stripe density", 100, 1, 100));
	lineGroup.add(stripesSteps.set("stripe step", 100, 1, 100));
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

	stripesNoiseOn.addListener(this, &StripesDrawer::onUpdateStripesNoiseFlag);

}

void StripesDrawer::setup(ofApp * app)
{
	LineDrawer::setup(app);

	// curve listeners

	stripesNoiseOn.addListener(this, &StripesDrawer::onUpdateStripesNoiseFlag);
	stripesNoiseOn.addListener(app, &ofApp::onUpdateBool);
}

void StripesDrawer::draw(vector<vector<ofPolyline>> charOutlines, bool debug)
{
	if (!drawLines.get())
		return;

	for (int c = 0; c < charOutlines.size(); c++)
	{
		for (int co = 0; co < charOutlines[c].size(); ++co)
		{
			// iterate through the axis and draw lines if they are within the baseline 

			ofPolyline baseLine = charOutlines[c][co];
			vector<ofPolyline> bLines;

			bool horizontal = co == 0;

			if (stripesFlipByChar && c % 2 == 0)
			{
				horizontal = !horizontal;
			}

			int numLines = horizontal ? ofGetHeight() / stripesDensity : ofGetWidth() / stripesDensity;
			int stripeCheckLength = horizontal ? ofGetWidth() : ofGetHeight();
			bool lineOn = false;

			for (int line = 0; line < numLines; ++line)
			{
				int changingAxis = (line + 0.5f) * stripesDensity;

				if (stripesNoiseOn.get())
				{
					float noise = ofMap(ofNoise(((c * 2) + line) * stripesNoiseResolution, stripesNoiseTime + c * 1000), 0, 1, -stripesNoiseScale, stripesNoiseScale);
					changingAxis += noise;
				}

				for (int staticAxis = 0; staticAxis < stripeCheckLength; staticAxis += stripesSteps)
				{
					ofPoint point = horizontal ? ofPoint(changingAxis, staticAxis) : ofPoint(staticAxis, changingAxis);
					bool insideOutline = baseLine.size() >= 3 ? baseLine.inside(point) : false;

					if (!lineOn && ((!stripesOutsideString && insideOutline) || (stripesOutsideString && !insideOutline)))
					{
						bLines.push_back(ofPolyline());
						bLines[bLines.size() - 1].addVertex(point);
						lineOn = true;
					}
					else if (lineOn && ((!stripesOutsideString && !insideOutline) || (stripesOutsideString && insideOutline)))
					{
						bLines[bLines.size() - 1].addVertex(point);
						lineOn = false;
					}
				}

				if (stripesOutsideString && lineOn)
				{
					bLines[bLines.size() - 1].addVertex(horizontal ? ofPoint(changingAxis, ofGetWidth()) : ofPoint(ofGetHeight(), changingAxis));
					lineOn = false;
				}
			}

			// draw line

			ofSetColor(0);

			for (int l = 0; l < bLines.size(); ++l)
			{
				bLines[l].draw();
			}

			if (debug)
			{
				ofSetColor(0, 0, 0, 100);
				baseLine.draw();
			}

			baseLine.clear();
		}
	}
}

void StripesDrawer::onUpdateStripesNoiseFlag(bool & newVal)
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


