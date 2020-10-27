// Arianna Rudawski 2020
// www.ariannarudawski.com

#include "UnknownPleasuresDrawer.h"

UnknownPleasuresDrawer::UnknownPleasuresDrawer(ofParameterGroup * mainGroup)
	: LineDrawer(mainGroup)
{
	lineGroup.setName("UNKNOWN PLEASURES");
	
	lineGroup.add(vertical.set("vertical", false));
	lineGroup.add(stripesDensity.set("stripe density", 100, 1, 300));
	lineGroup.add(stripesSteps.set("stripe step", 100, 1, 100));
	lineGroup.add(numLinesToCheckForIntersection.set("num lines to check for intersection", 3, 1, 100));

	lineGroup.add(noiseTime.set("noise time", 500, 0, 1000));
	lineGroup.add(outsideNoiseScale.set("outside noise scale", -5, -100, 100));
	lineGroup.add(insideNoiseScale.set("inside noise scale", -45, -100, 100));
	lineGroup.add(noiseResolution.set("noise resolution", ofVec2f(0.979, 1), ofVec2f(0.0f, 0.0f), ofVec2f(1.0f, 1.0f)));
	lineGroup.add(curveResolution.set("curve resolution", 20, 1, 100));

	vertical.addListener(this, &UnknownPleasuresDrawer::onBoolParamsChanged);
	stripesDensity.addListener(this, &UnknownPleasuresDrawer::onIntParamsChanged);
	stripesSteps.addListener(this, &UnknownPleasuresDrawer::onIntParamsChanged);

	noiseTime.addListener(this, &UnknownPleasuresDrawer::onFloatParamsChanged);
	outsideNoiseScale.addListener(this, &UnknownPleasuresDrawer::onFloatParamsChanged);
	insideNoiseScale.addListener(this, &UnknownPleasuresDrawer::onFloatParamsChanged);
	noiseResolution.addListener(this, &UnknownPleasuresDrawer::onofVec2fParamsChanged);
	curveResolution.addListener(this, &UnknownPleasuresDrawer::onIntParamsChanged);
}


void UnknownPleasuresDrawer::draw(vector<vector<ofPolyline>> charOutlines, bool debug)
{
	if (!drawLines.get())
		return;

	// redo lines

	if (redoLines)
	{
		int numLines = vertical.get() ? ofGetHeight() / stripesDensity : ofGetWidth() / stripesDensity;
		int stripeCheckLength = vertical.get() ? ofGetWidth() : ofGetHeight();

		// find all curved lines

		vector<ofPolyline> cLines;
		bool lineOn;

		for (int line = 0; line < numLines; ++line)
		{
			ofPolyline cLine;
			lineOn = true;

			int changingAxis = ofGetHeight() - ((line + 0.5f) * stripesDensity);

			// find the curved lines for this line

			cLine.addVertex(vertical ? ofPoint(changingAxis, 0) : ofPoint(0, changingAxis));

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

				// adjust point with noise

				float noise = ofNoise(line * noiseResolution.get().x, staticAxis * noiseResolution.get().y, noiseTime.get())
					* (insideChar ? insideNoiseScale.get() : outsideNoiseScale.get());

				point += vertical.get() ? ofPoint(noise, 0) : ofPoint(0, noise);

				cLine.curveTo(point, curveResolution);

				// iterate the static axis
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

			// add the final vertex to close the line, then add it to the list

			cLine.addVertex(vertical ? ofPoint(changingAxis, stripeCheckLength) : ofPoint(stripeCheckLength, changingAxis));
			cLines.push_back(cLine);
		}

		// find intersections and chop up lines

		choppedLines.clear();

		for (int i = cLines.size() - 1 ; i > 0; --i)
		{
			vector<glm::vec3> lineVerts = cLines[i].getVertices();
			cLines.pop_back();

			bool lineOn = true;
			ofPolyline line;

			for (int v = 0; v < lineVerts.size(); ++v)
			{
				ofPoint point = lineVerts[v];
				bool intersectsExistingLines = pointIntersectsExistingLines(point, cLines);

				if (lineOn)
				{
					if (intersectsExistingLines)
					{
						// chop this line 
						// todo: get the correct next point to add as the final vertex				
						choppedLines.push_back(line);
						line.clear();
						lineOn = false;
					}
					else
					{					
						// continue to draw line
						line.addVertex(point);
					}
				}
				else if (!lineOn && !intersectsExistingLines)
				{
					// restart a chopped line at this point 
					line.addVertex(point);
					lineOn = true;
				}
			}

			choppedLines.push_back(line);
		}

		if (cLines.size() > 0)
			choppedLines.push_back(cLines[0]);

		redoLines = false;
	}

	// draw lines

	ofSetColor(0);

	for (int l = 0; l < choppedLines.size(); ++l)
	{
		choppedLines[l].draw();
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

bool UnknownPleasuresDrawer::pointIntersectsExistingLines(ofPoint point, vector<ofPolyline> lines)
{
	int maxLineToCheck = lines.size() - 1 - numLinesToCheckForIntersection;
	if (maxLineToCheck < 0)
		maxLineToCheck = 0;

	for (int i = lines.size() - 1; i >= maxLineToCheck; --i)
		if (lines[i].inside(point))
			return true;

	return false;
}

void UnknownPleasuresDrawer::onBoolParamsChanged(bool & newVal)
{
	redoLines = true;
}

void UnknownPleasuresDrawer::onIntParamsChanged(int & newVal)
{
	redoLines = true;
}

void UnknownPleasuresDrawer::onFloatParamsChanged(float & newVal)
{
	redoLines = true;
}

void UnknownPleasuresDrawer::onofVec2fParamsChanged(ofVec2f & newVal)
{
	redoLines = true;
}
