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

	stringGroup.add(stringToDraw.set("string to draw", "05")); 
	stringGroup.add(drawInnerLines.set("draw inner lines of letters", false));
	stringGroup.add(position.set("center position", ofVec2f(0.5f, 0.5f), ofVec2f(0.0f, 0.0f), ofVec2f(1.0f, 1.0f)));
	stringGroup.add(size.set("font size", 500, 10, 800));
	stringGroup.add(letterSpacing.set("letter spacing", 1, -1, 3));
	stringGroup.add(spacing.set("vertex spacing", 40, 1, 500));

	size.addListener(this, &ofApp::onUpdateSize);

	// curves gui

	curvesGroup.setName("CURVES");

	curvesGroup.add(noiseOn.set("use noise", false));
	curvesGroup.add(loopSize.set("loop size", ofVec2f(0.0f, 0.0f), ofVec2f(-1.0f, -1.0f), ofVec2f(1.0f, 1.0f)));
	curvesGroup.add(loopDepth.set("loop depth", 25, -100, 100));
	curvesGroup.add(numLines.set("num lines", 1, 1, 20));

	deltaSize.set("delta loop size", ofVec2f(0.1f, 0.1f), ofVec2f(-1.0f, -1.0f), ofVec2f(1.0f, 1.0f));
	if (numLines.get() > 1) 
		curvesGroup.add(deltaSize);
	numLines.addListener(this, &ofApp::onUpdateNumCurveLines);

	curveNoiseTime.set("noise time", 0, 0, 1000);
	curveNoiseScale.set("noise scale", 1, 0, 100);
	curveNoiseResolution.set("noise resolution", ofVec2f(0.5, 0.5), ofVec2f(0, 0), ofVec2f(1, 1));
	if (noiseOn.get())
	{
		curvesGroup.add(curveNoiseTime);
		curvesGroup.add(curveNoiseScale);
		curvesGroup.add(curveNoiseResolution);
	}
	noiseOn.addListener(this, &ofApp::onUpdateNoiseFlag);

	// tie together gui

	mainGroup.setName("Inktober 2020");
	mainGroup.add(stringGroup);
	mainGroup.add(curvesGroup);

	gui.setup(mainGroup);

	// load font

	font.loadFont("fonts/ArialBlack.ttf", size, true, true, true);
}

void ofApp::update()
{

}

void ofApp::draw()
{
	DrawCurvesFromString(stringToDraw, ofPoint(ofGetWidth() * position.get().x, ofGetHeight() * position.get().y));

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
				glm::vec3 vertex =  verts[v];
				glm::vec3 normal = baseLine.getNormalAtIndex(v) * loopDepth.get();

				points.push_back(position + vertex);
				innerPoints.push_back(position + ((vertex - normal)));
				outerPoints.push_back(position + ((vertex + normal)));
			}

			// create lines

			vector<ofPolyline> bLines;

			for (int l = 0; l < numLines; ++l)
			{
				ofPolyline bLine;
				bLine.setClosed(true);

				for (int v = 0; v < points.size(); v++)
				{
					ofPoint a = points[(v + 1) % points.size()];
					ofPoint b = points[(v + 2) % points.size()];
					ofPoint c = (a + b) * 0.5f;

					ofPoint aInner = innerPoints[(v + 1) % points.size()];
					ofPoint bInner = innerPoints[(v + 2) % points.size()];

					ofPoint aOuter = outerPoints[(v + 1) % points.size()];
					ofPoint bOuter = outerPoints[(v + 2) % points.size()];

					float sizeX = loopSize.get().x + (l * deltaSize.get().x);
					float sizeY = loopSize.get().y + (l * deltaSize.get().y);

					if (noiseOn.get())
					{
						float x = c.x * curveNoiseResolution.get().x;
						float y = c.y * curveNoiseResolution.get().y;

						float noiseX = ofMap(ofNoise(curveNoiseTime.get(), x, y), 0, 1, -1, 1);
						float noiseY = ofMap(ofNoise(curveNoiseTime.get() + curveNoiseTime.getMax(), y, x), 0, 1, -1, 1);

						sizeX += noiseX * curveNoiseScale.get();
						sizeY += noiseY * curveNoiseScale.get();
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

			for (int l = 0; l < numLines; ++l)
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

void ofApp::onUpdateNumCurveLines(int & newNum)
{
	if (newNum > 1 && !curvesGroup.contains(deltaSize.getName()))
		curvesGroup.add(deltaSize);
	else
		curvesGroup.remove(deltaSize);

	gui.setup(mainGroup);
}

void ofApp::onUpdateNoiseFlag(bool & newVal)
{
	if (newVal && !curvesGroup.contains(curveNoiseTime.getName()))
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
