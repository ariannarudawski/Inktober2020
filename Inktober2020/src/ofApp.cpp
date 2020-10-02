// Arianna Rudawski 2020
// www.ariannarudawski.com

#include "ofApp.h"

void ofApp::setup()
{
	ofSetFrameRate(30);
	ofSetWindowPosition(0, 0);
	ofBackground(255);

	// debug gui

	mainGroup.add(position.set("position", ofVec2f(0.5f, 0.5f), ofVec2f(0.0f, 0.0f), ofVec2f(1.0f, 1.0f)));
	mainGroup.add(size.set("size", 500, 10, 800));
	mainGroup.add(spacing.set("spacing", 30, 10, 100));
	mainGroup.add(loopSize.set("loop size", ofVec2f(0.0f, 0.0f), ofVec2f(-1.0f, -1.0f), ofVec2f(1.0f, 1.0f)));

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
		ofBeginSaveScreenAsSVG("svgs/Inktober2020_" + stringToDraw + ".svg");
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
	vector<float> noise;
	ofPolyline line;

	// adjust position for font size 

	position -= ofPoint(font.stringWidth(word) * 0.5f, font.stringHeight(word) * -0.5f);

	// get points

	vector<ofPath> word_path = font.getStringAsPoints(word, true, false);
	for (int word_index = 0; word_index < word.size(); word_index++) {

		vector<ofPolyline> outline = word_path[word_index].getOutline();

		for (int o = 0; o < outline.size(); o++)
		{
			// get points and char center

			outline[o] = outline[o].getResampledBySpacing(spacing);
			vector<glm::vec3> verts = outline[o].getVertices();

			ofPoint charCenter(0, 0);

			for (int v = 0; v < verts.size(); v++)
			{
				glm::vec3 vertex = position + glm::vec2(verts[v]);

				charCenter += vertex;
				points.push_back(vertex);
			}

			charCenter /= points.size();

			// create line

			ofPolyline bLine;

			for (int v = 0; v < points.size(); v++)
			{
				//// get vertices from the char  

				ofPoint a = points[(v)];
				ofPoint b = points[(v + 1) % points.size()];

				ofPoint from = a - (a - charCenter) * -loopSize.get().x;
				ofPoint cp1 = a - (a - charCenter) * -loopSize.get().y;
				ofPoint cp2 = b - (b - charCenter) * -loopSize.get().y;
				ofPoint to = b - (b - charCenter) * -loopSize.get().x;

				bLine.addVertex(from);
				bLine.bezierTo(cp1, cp2, to);

				if (debug)
				{
					ofFill();

					ofSetColor(0, 0, 0, 128);
					ofDrawCircle(a, 5);

					ofSetColor(235, 204, 91, 128);
					ofDrawCircle(from, 5);
					ofDrawCircle(to, 5);

					ofSetColor(235, 162, 89, 128);
					ofDrawCircle(cp1, 5);
					ofDrawCircle(cp2, 5);
				}
			}

			// draw line

			ofSetColor(0);
			bLine.draw();

			// draw points debug

			if (debug)
			{
				ofFill();

				// draw points in black
				ofSetColor(0);
				for (int p = 0; p < points.size(); ++p)
				{
					ofDrawCircle(points[p], 5);
				}
			}

			// clear points for next iteration

			points.clear();
		}
	}
}

