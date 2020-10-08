// Arianna Rudawski 2020
// www.ariannarudawski.com

#include "ofApp.h"

void ofApp::setup()
{
	ofSetFrameRate(30);
	ofSetWindowPosition(0, 0);
	ofBackground(255);

	// debug gui

	mainGroup.add(stringToDraw.set("string", "04")); 
	mainGroup.add(drawInnerLines.set("draw inner lines", false));
	mainGroup.add(position.set("position", ofVec2f(0.5f, 0.5f), ofVec2f(0.0f, 0.0f), ofVec2f(1.0f, 1.0f)));
	mainGroup.add(size.set("size", 500, 10, 800));
	mainGroup.add(letterSpacing.set("letter spacing", 1, -1, 3));
	mainGroup.add(spacing.set("spacing", 40, 1, 300));
	mainGroup.add(loopSize.set("loop size", ofVec2f(0.0f, 0.0f), ofVec2f(-1.0f, -1.0f), ofVec2f(1.0f, 1.0f)));
	mainGroup.add(loopDepth.set("loop depth", 25, -100, 100));

	mainGroup.add(numLines.set("num lines", 1, 1, 20));
	mainGroup.add(deltaSize.set("delta loop size", ofVec2f(0.1f, 0.1f), ofVec2f(-1.0f, -1.0f), ofVec2f(1.0f, 1.0f)));

	mainGroup.setName("Inktober 2020");
	gui.setup(mainGroup);

	// listeners

	size.addListener(this, &ofApp::onUpdateSize);

	// load font

	font.loadFont("fonts/ArialBlack.ttf", size, true, true, true);
}

void ofApp::update()
{

}

void ofApp::draw()
{
	DrawString(stringToDraw, ofPoint(ofGetWidth() * position.get().x, ofGetHeight() * position.get().y));

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

void ofApp::DrawString(string word, ofPoint position)
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

				for (int v = 0; v < points.size(); v++)
				{
					ofPoint b = points[(v + 1) % points.size()];
					ofPoint c = points[(v + 2) % points.size()];

					ofPoint bInner = innerPoints[(v + 1) % points.size()];
					ofPoint cInner = innerPoints[(v + 2) % points.size()];

					ofPoint bOuter = outerPoints[(v + 1) % points.size()];
					ofPoint cOuter = outerPoints[(v + 2) % points.size()];

					float sizeX = loopSize.get().x + (l * deltaSize.get().x);
					float sizeY = loopSize.get().y + (l * deltaSize.get().y);

					ofPoint from = b - (b - bInner) * -sizeX;
					ofPoint cp1 = b - (b - bInner) * -sizeY;
					ofPoint cp2 = c - (c - cInner) * -sizeY;
					ofPoint to = c - (c - cInner) * -sizeX;

					bLine.addVertex(from);
					bLine.bezierTo(cp1, cp2, to);

					if (debug)
					{
						ofFill();

						ofSetColor(62, 144, 176, 100);
						ofDrawCircle(bInner, 5);
						ofDrawCircle(bOuter, 5);
						ofDrawLine(bInner, bOuter);

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
