// Arianna Rudawski 2020
// www.ariannarudawski.com

#include "ofApp.h"

void ofApp::setup()
{
	ofSetFrameRate(30);
	ofSetWindowPosition(0, 0);
	ofBackground(255);
	  
	// string gui

	stringGroup.setName("STRING");

	stringGroup.add(stringToDraw.set("string to draw", "10")); 
	stringGroup.add(drawInnerLines.set("use all letter lines", true));
	stringGroup.add(position.set("center position", ofVec2f(0.5f, 0.5f), ofVec2f(0.0f, 0.0f), ofVec2f(1.0f, 1.0f)));
	stringGroup.add(size.set("font size", 500, 10, 800));
	stringGroup.add(letterSpacing.set("letter spacing", 1, -1, 3));
	stringGroup.add(spacing.set("vertex spacing", 40, 1, 500));

	size.addListener(this, &ofApp::onUpdateSize);

	// drawing lines gui

	stringGroup.add(drawCurves.set("draw curves", false));
	drawCurves.addListener(this, &ofApp::onToggleDrawCurves);

	stringGroup.add(drawMatchsticks.set("draw matchsticks", false));
	drawMatchsticks.addListener(this, &ofApp::onToggleDrawMatchsticks);

	stringGroup.add(drawStripes.set("draw stripes", true));
	drawStripes.addListener(this, &ofApp::onToggleDrawStripes);

	// curves gui

	curvesGroup.setName("CURVES");

	curvesGroup.add(curveSize.set("loop size", ofVec2f(0.0f, 0.0f), ofVec2f(-1.0f, -1.0f), ofVec2f(1.0f, 1.0f)));
	curvesGroup.add(curveDepth.set("loop depth", 25, -100, 100));
	curvesGroup.add(curveNumLines.set("num lines", 1, 1, 20));
	curvesGroup.add(curveDeltaSize.set("delta loop size", ofVec2f(0.1f, 0.1f), ofVec2f(-1.0f, -1.0f), ofVec2f(1.0f, 1.0f)));
	curvesGroup.add(curveDepthNoiseOn.set("use depth noise", false));
	curvesGroup.add(curveSizeNoiseOn.set("use size noise", false));
	curveNoiseTime.set("noise time", 0, 0, 1000);
	curveNoiseScale.set("noise scale", 1, 0, 100);
	curveNoiseResolution.set("noise resolution", 0.5, 0, 1);
	if (curveDepthNoiseOn.get() || curveSizeNoiseOn.get())
	{
		curvesGroup.add(curveNoiseTime);
		curvesGroup.add(curveNoiseScale);
		curvesGroup.add(curveNoiseResolution);
	}
	curveDepthNoiseOn.addListener(this, &ofApp::onUpdateDepthNoiseFlag);
	curveSizeNoiseOn.addListener(this, &ofApp::onUpdateSizeNoiseFlag);

	// matchsticks gui

	matchsticksGroup.setName("MATCHSTICKS");

	matchsticksGroup.add(matchsticksClockwise.set("clockwise", false));
	matchsticksGroup.add(matchsticksRays.set("num rays", 3, 1, 25));
	matchsticksGroup.add(matchsticksRayDistance.set("ray distance", 5, 0, 500));
	matchsticksGroup.add(matchsticksRaySourceSpacing.set("ray source spacing", 1, 1, 25));
	matchsticksGroup.add(matchstickDepth.set("depth", 1, 1, 100));
	matchsticksGroup.add(matchstickSourceDepth.set("ray source depth", 0.5, 0, 1));
	matchsticksGroup.add(matchstickDestDepth.set("ray dest depth", 0.5, 0, 1));

	// stripes gui

	matchsticksGroup.setName("STRIPES");

	stripesGroup.add(stripesFlipByChar.set("flip stripes by char", true));
	stripesGroup.add(stripesOutsideString.set("draw stripes outside string", true));
	stripesGroup.add(stripesDensity.set("stripe density", 100, 1, 100));
	stripesGroup.add(stripesSteps.set("stripe step", 100, 1, 100));
	stripesGroup.add(stripesNoiseOn.set("use noise", false));
	stripesNoiseTime.set("noise time", 0, 0, 1000);
	stripesNoiseScale.set("noise scale", 1, 0, 100);
	stripesNoiseResolution.set("noise resolution", 0.5, 0, 1);
	if (stripesNoiseOn.get())
	{
		stripesGroup.add(stripesNoiseTime);
		stripesGroup.add(stripesNoiseScale);
		stripesGroup.add(stripesNoiseResolution);
	}

	stripesNoiseOn.addListener(this, &ofApp::onUpdateStripesNoiseFlag);

	// tie together gui

	mainGroup.setName("Inktober 2020");
	mainGroup.add(stringGroup);

	if (drawCurves.get())
		mainGroup.add(curvesGroup);

	if (drawMatchsticks.get())
		mainGroup.add(matchsticksGroup);

	if (drawStripes.get())
		mainGroup.add(stripesGroup);

	gui.setup(mainGroup);

	// load font

	font.loadFont("fonts/ArialBlack.ttf", size, true, true, true);
}

void ofApp::update()
{

}

void ofApp::draw()
{
	// draw lines

	if (drawCurves.get())
		DrawCurvesFromString(stringToDraw, ofPoint(ofGetWidth() * position.get().x, ofGetHeight() * position.get().y));

	if (drawMatchsticks.get())
		DrawMatchsticksFromString(stringToDraw, ofPoint(ofGetWidth() * position.get().x, ofGetHeight() * position.get().y));

	if (drawStripes.get())
		DrawStripesFromString(stringToDraw, ofPoint(ofGetWidth() * position.get().x, ofGetHeight() * position.get().y));

	// draw mouse 
	if (debug)
	{
		gui.draw();

		ofNoFill();
		ofSetColor(209, 89, 8);
		ofDrawCircle(ofGetMouseX(), ofGetMouseY(), 15);
	}
}

void ofApp::keyPressed(int key)
{
	switch (key)
	{
	case 'd':
	case 'D':
		debug = !debug;
		break;
	case ' ':
		bool resetDrawDebug = debug;
		debug = false;
		ofBeginSaveScreenAsSVG("../../../../svgs/Inktober2020_" + stringToDraw.get() + ".svg");
		draw();
		ofEndSaveScreenAsSVG();
		debug = resetDrawDebug;
		break;
	}
}

void ofApp::onUpdateSize(int & newSize)
{
	font.loadFont("fonts/ArialBlack.ttf", newSize, true, true, true);
}

void ofApp::DrawCurvesFromString(string word, ofPoint position)
{
	if (word.length() <= 0 || word == " ")
		return;

	// get points for this string

	vector<glm::vec3> points;
	vector<glm::vec3> innerPoints;
	vector<glm::vec3> outerPoints;
	vector<float> pointNoise;

	// adjust for font size and letter spacing
		
	font.setLetterSpacing(letterSpacing.get());
	position -= ofPoint(font.stringWidth(word) * 0.5f, font.stringHeight(word) * -0.5f);

	// get points

	vector<ofPath> word_path = font.getStringAsPoints(word, true, false);

	for (int word_index = 0; word_index < word.size(); word_index++) 
	{
		vector<ofPolyline> outline = word_path[word_index].getOutline();

		for (int o = 0; o < (drawInnerLines ? outline.size() : 1); o++)
		{
			// clear points for next iteration

			points.clear();
			innerPoints.clear();
			outerPoints.clear();
			pointNoise.clear();

			// get the baseline from the outline

			ofPolyline baseLine = outline[o];

			// add the first vertex again

			baseLine.addVertex(baseLine.getVertices()[0]);

			// calculate the length
			float length = baseLine.getLengthAtIndex(baseLine.getVertices().size() - 1);

			// resample by the defined density
			float spacingEqualized = length / ((int)(length/spacing));
			baseLine = baseLine.getResampledBySpacing(spacingEqualized);

			// remove the last point if it's still a copy of the first)

			ofVec2f first = baseLine.getVertices()[0];
			ofVec2f last = baseLine.getVertices()[baseLine.getVertices().size() - 1];
			if (first.distance(last) <= 1)
				baseLine.removeVertex(baseLine.getVertices().size() - 1);

			// turn all baseline verts into reference points

			vector<glm::vec3> verts = baseLine.getVertices();
			for (int v = 0; v < verts.size(); v++)
			{
				pointNoise.push_back(ofMap(ofNoise(((word_index * 2) + v) * curveNoiseResolution, curveNoiseTime.get()), 0, 1, 0, curveNoiseScale));

				glm::vec3 vertex =  verts[v];
				glm::vec3 normal = baseLine.getNormalAtIndex(v) * curveDepth.get();

				if (curveDepthNoiseOn)
					normal *= pointNoise[v];

				points.push_back(position + vertex);
				innerPoints.push_back(position + ((vertex - normal)));
				outerPoints.push_back(position + ((vertex + normal)));
			}

			// create lines

			vector<ofPolyline> bLines;

			for (int l = 0; l < curveNumLines; ++l)
			{
				ofPolyline bLine;
				bLine.setClosed(true);

				for (int v = 0; v < points.size(); v++)
				{
					ofPoint a = points[v];
					ofPoint b = points[(v + 1) % points.size()];
					ofPoint c = (a + b) * 0.5f;

					ofPoint aInner = innerPoints[v];
					ofPoint bInner = innerPoints[(v + 1) % points.size()];

					ofPoint aOuter = outerPoints[v];
					ofPoint bOuter = outerPoints[(v + 1) % points.size()];

					float sizeX = curveSize.get().x + (l * curveDeltaSize.get().x);
					float sizeY = curveSize.get().y + (l * curveDeltaSize.get().y);

					if (curveSizeNoiseOn.get())
					{
						sizeX += pointNoise[v];
						sizeY += pointNoise[v];
					}

					ofPoint from = a - (a - aInner) * -sizeX;
					ofPoint cp1 = a - (a - aInner) * -sizeY;
					ofPoint cp2 = b - (b - bInner) * -sizeY;
					ofPoint to = b - (b - bInner) * -sizeX;

					bLine.addVertex(from);
					bLine.bezierTo(cp1, cp2, to);

					if (debug)
					{
						ofFill();

						ofSetColor(62, 144, 176, 100);
						ofDrawCircle(aInner, 5);
						ofDrawCircle(aOuter, 5);
						ofDrawLine(aInner, aOuter);

						ofSetColor(235, 162, 89, 100);
						ofDrawCircle(from, 5);
						ofDrawCircle(cp1, 5);
						ofDrawCircle(cp2, 5);
						ofDrawCircle(to, 5);
						ofDrawLine(from, cp1);
						ofDrawLine(cp1, cp2);
					}
				}

				bLines.push_back(bLine);
			}

			// draw line

			ofSetColor(0);

			for (int l = 0; l < curveNumLines; ++l)
			{
				bLines[l].draw();
			}

			if (debug)
			{
				ofSetColor(0, 0, 0);
				for (int p = 0; p < points.size(); ++p)
				{
					ofDrawCircle(points[p], 5);
				}
			}
		}
	}
}

void ofApp::onToggleDrawCurves(bool & newDrawCurves)
{
	if (newDrawCurves)
	{
		mainGroup.add(curvesGroup);
	}
	else
	{
		mainGroup.remove(curvesGroup);
	}

	gui.setup(mainGroup);
}

void ofApp::DrawMatchsticksFromString(string word, ofPoint position)
{
	if (word.length() <= 0 || word == " ")
		return;

	// get points for this string

	vector<glm::vec3> points;
	vector<glm::vec3> innerPoints;
	vector<glm::vec3> outerPoints;
	vector<float> pointNoise;

	// adjust for font size and letter spacing

	font.setLetterSpacing(letterSpacing.get());
	position -= ofPoint(font.stringWidth(word) * 0.5f, font.stringHeight(word) * -0.5f);

	// get points

	vector<ofPath> word_path = font.getStringAsPoints(word, true, false);

	for (int word_index = 0; word_index < word.size(); word_index++)
	{
		vector<ofPolyline> outline = word_path[word_index].getOutline();

		for (int o = 0; o < (drawInnerLines ? outline.size() : 1); o++)
		{
			// clear points for next iteration

			points.clear();
			innerPoints.clear();
			outerPoints.clear();
			pointNoise.clear();

			// get the baseline from the outline

			ofPolyline baseLine = outline[o];

			// add the first vertex again

			baseLine.addVertex(baseLine.getVertices()[0]);

			// calculate the length
			float length = baseLine.getLengthAtIndex(baseLine.getVertices().size() - 1);

			// resample by the defined density
			float spacingEqualized = length / ((int)(length / spacing));
			baseLine = baseLine.getResampledBySpacing(spacingEqualized);

			// remove the last point if it's still a copy of the first)

			ofVec2f first = baseLine.getVertices()[0];
			ofVec2f last = baseLine.getVertices()[baseLine.getVertices().size() - 1];
			if (first.distance(last) <= 1)
				baseLine.removeVertex(baseLine.getVertices().size() - 1);

			// turn all baseline verts into reference points

			vector<glm::vec3> verts = baseLine.getVertices();
			for (int v = 0; v < verts.size(); v++)
			{
				pointNoise.push_back(ofMap(ofNoise(((word_index * 2) + v) * curveNoiseResolution, curveNoiseTime.get()), 0, 1, 0, curveNoiseScale));

				glm::vec3 vertex = verts[v];
				glm::vec3 normal = baseLine.getNormalAtIndex(v) * matchstickDepth.get();

				if (curveDepthNoiseOn)
					normal *= pointNoise[v];

				points.push_back(position + vertex);
				innerPoints.push_back(position + ((vertex - normal)));
				outerPoints.push_back(position + ((vertex + normal)));
			}

			// create lines

			vector<ofPolyline> bLines;

			for (int l = 0; l < curveNumLines; ++l)
			{
				for (int v = 0; v < points.size(); v += matchsticksRaySourceSpacing)
				{
					ofPoint aInner = innerPoints[v];
					ofPoint aOuter = outerPoints[v];

					for (int i = 0; i < matchsticksRays.get(); ++i)
					{
						ofPoint bInner = innerPoints[(v + i + matchsticksRayDistance) % points.size()];
						ofPoint bOuter = outerPoints[(v + i + matchsticksRayDistance) % points.size()];

						ofPoint src = aInner.getInterpolated(aOuter, matchstickSourceDepth);
						ofPoint dest = bInner.getInterpolated(bOuter, matchstickDestDepth);

						ofPolyline bLine;
						bLine.addVertex(src);
						bLine.addVertex(dest);
						bLines.push_back(bLine);
					}
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
				ofSetColor(0, 0, 0);
				for (int p = 0; p < points.size(); ++p)
				{
					ofDrawCircle(points[p], 5);
				}
			}
		}
	}
}

void ofApp::onToggleDrawMatchsticks(bool & newDrawMatchsticks)
{
	if (newDrawMatchsticks)
	{
		mainGroup.add(matchsticksGroup);
	}
	else
	{
		mainGroup.remove(matchsticksGroup);
	}

	gui.setup(mainGroup);
}

void ofApp::DrawStripesFromString(string word, ofPoint position)
{
	if (word.length() <= 0 || word == " ")
		return;

	// get points for this string

	ofPolyline baseLine;
	baseLine.setClosed(true);

	// adjust for font size and letter spacing

	font.setLetterSpacing(letterSpacing.get());
	position -= ofPoint(font.stringWidth(word) * 0.5f, font.stringHeight(word) * -0.5f);

	// get points

	vector<ofPath> word_path = font.getStringAsPoints(word, true, false);

	for (int word_index = 0; word_index < word.size(); word_index++)
	{
		vector<ofPolyline> outline = word_path[word_index].getOutline();

		for (int o = 0; o < (drawInnerLines ? outline.size() : 1); o++)
		{
			// add the first vertex again

			outline[o].addVertex(outline[o].getVertices()[0]);

			// calculate the length
			float length = outline[o].getLengthAtIndex(outline[o].getVertices().size() - 1);

			// resample by the defined density
			float spacingEqualized = length / ((int)(length / spacing));
			outline[o] = outline[o].getResampledBySpacing(spacingEqualized);

			// remove the last point if it's still a copy of the first)

			ofVec2f first = outline[o].getVertices()[0];
			ofVec2f last = outline[o].getVertices()[outline[o].getVertices().size() - 1];
			if (first.distance(last) <= 1)
				outline[o].removeVertex(outline[o].getVertices().size() - 1);

			// turn all baseline verts into reference points

			vector<glm::vec3> verts = outline[o].getVertices();
			for (int v = 0; v < verts.size(); v++)
			{
				glm::vec3 vertex = verts[v];
				baseLine.addVertex(position + vertex);
			}

			// iterate through the axis and draw lines if they are within the baseline 

			vector<ofPolyline> bLines;

			bool horizontal = o == 0;

			if (stripesFlipByChar && word_index % 2 == 0)
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
					float noise = ofMap(ofNoise(((word_index * 2) + line) * stripesNoiseResolution, stripesNoiseTime), 0, 1, -stripesNoiseScale, stripesNoiseScale);
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

void ofApp::onToggleDrawStripes(bool & newDrawStripes)
{
	if (newDrawStripes)
	{
		mainGroup.add(stripesGroup);
	}
	else
	{
		mainGroup.remove(stripesGroup);
	}

	gui.setup(mainGroup);
}

void ofApp::onUpdateDepthNoiseFlag(bool & newVal)
{
	if (newVal) curveSizeNoiseOn = false;

	if ((newVal) && !curvesGroup.contains(curveNoiseTime.getName()))
	{
		curvesGroup.add(curveNoiseTime);
		curvesGroup.add(curveNoiseScale);
		curvesGroup.add(curveNoiseResolution);
	}
	else
	{
		curvesGroup.remove(curveNoiseTime);
		curvesGroup.remove(curveNoiseScale);
		curvesGroup.remove(curveNoiseResolution);
	}

	gui.setup(mainGroup);
}

void ofApp::onUpdateSizeNoiseFlag(bool & newVal)
{
	if (newVal) curveDepthNoiseOn = false;

	if ((newVal) && !curvesGroup.contains(curveNoiseTime.getName()))
	{
		curvesGroup.add(curveNoiseTime);
		curvesGroup.add(curveNoiseScale);
		curvesGroup.add(curveNoiseResolution);
	}
	else
	{
		curvesGroup.remove(curveNoiseTime);
		curvesGroup.remove(curveNoiseScale);
		curvesGroup.remove(curveNoiseResolution);
	}

	gui.setup(mainGroup);
}

void ofApp::onUpdateStripesNoiseFlag(bool & newVal)
{
	if ((newVal) && !stripesGroup.contains(stripesNoiseTime.getName()))
	{
		stripesGroup.add(stripesNoiseTime);
		stripesGroup.add(stripesNoiseScale);
		stripesGroup.add(stripesNoiseResolution);
	}
	else
	{
		stripesGroup.remove(stripesNoiseTime);
		stripesGroup.remove(stripesNoiseScale);
		stripesGroup.remove(stripesNoiseResolution);
	}

	gui.setup(mainGroup);
}
