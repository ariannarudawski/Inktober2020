// Arianna Rudawski 2020
// www.ariannarudawski.com

#include "ofApp.h"

#include "CurveDrawer.h"
#include "MatchsticksDrawer.h"
#include "StripesDrawer.h"
#include "VertToEdgeDrawer.h"
#include "UnknownPleasuresDrawer.h"
#include "GridDrawer.h"

void ofApp::setup()
{
	ofSetFrameRate(30);
	ofSetWindowPosition(0, 0);
	ofBackground(255);
	  
	// string gui

	stringGroup.setName("STRING");

	stringGroup.add(stringToDraw.set("string to draw", "21")); 
	stringGroup.add(drawInnerLines.set("use all letter lines", true));
	stringGroup.add(position.set("center position", ofVec2f(0.5f, 0.5f), ofVec2f(0.0f, 0.0f), ofVec2f(1.0f, 1.0f)));
	stringGroup.add(size.set("font size", 500, 10, 800));
	stringGroup.add(letterSpacing.set("letter spacing", 1, -1, 3));

	stringGroup.add(respace.set("respace vertices", true));
	respace.addListener(this, &ofApp::onUpdateSpacingFlag);
	spacing.set("vertex spacing", 40, 1, 500);

	if (respace.get())
	{
		stringGroup.add(spacing);
	}

	size.addListener(this, &ofApp::onUpdateSize);

	// create drawing lines types

	linesGroup.setName("LINES");

	lineDrawers.push_back(new CurveDrawer(&mainGroup));
	lineDrawers.push_back(new MatchsticksDrawer(&mainGroup));
	lineDrawers.push_back(new StripesDrawer(&mainGroup));
	lineDrawers.push_back(new VertToEdgeDrawer(&mainGroup));
	lineDrawers.push_back(new UnknownPleasuresDrawer(&mainGroup));
	lineDrawers.push_back(new GridDrawer(&mainGroup));

	for (auto & drawer : lineDrawers)
	{
		linesGroup.add(drawer->drawLines.set("DRAW " + drawer->getName(), false));
	}

	// gui

	mainGroup.setName("Inktober 2020");

	mainGroup.add(stringGroup);
	mainGroup.add(linesGroup);

	// setup line types

	for (auto & drawer : lineDrawers)
	{
		drawer->setup(this);
	}

	// start off by activating the last line drawer added (makes my life 0.01% easier)

	LineDrawer * lastDrawerAdded = (*(--lineDrawers.end()));
	lastDrawerAdded->drawLines = true;

	// final gui setup

	gui.setup(mainGroup);

	// load font

	font.loadFont("fonts/ArialBlack.ttf", size, true, true, true);
}

void ofApp::update()
{

}

void ofApp::draw()
{
	// get char outlines

	vector<vector<ofPolyline>> outlines = GetAllCharOutlines();

	// draw all eligible lines

	for (auto & drawer : lineDrawers)
	{
		drawer->draw(outlines, debug);
	}

	// draw debug

	if (debug)
	{
		// char outlines 

		float ptRadius = 0.01f * (ofGetWidth() < ofGetHeight() ? ofGetWidth() : ofGetHeight());

		ofNoFill();
		for (int c = 0; c < outlines.size(); c++)
		{
			for (int co = 0; co < outlines[c].size(); ++co)
			{
				ofSetColor(0, 50);
				outlines[c][co].draw();

				for (int v = 0; v < outlines[c][co].getVertices().size(); ++v)
				{
					ofDrawCircle(outlines[c][co][v], ptRadius);
				}
			}
		}
		ofFill();

		// gui

		gui.draw();

		// mouse cursor

		ofNoFill();
		ofSetColor(209, 89, 8);
		ofDrawCircle(ofGetMouseX(), ofGetMouseY(), 15);
		ofFill();
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

void ofApp::UpdateGUI()
{
	gui.setup(mainGroup);
}

void ofApp::onUpdateBool(bool & newVal)
{
	UpdateGUI();
}

void ofApp::onUpdateInt(int & newVal)
{
	UpdateGUI();
}

void ofApp::onUpdateFloat(float & newVal)
{
	UpdateGUI();
}

void ofApp::onUpdateSpacingFlag(bool & newVal)
{
	if (newVal)
	{
		stringGroup.add(spacing);
	}
	else
	{
		stringGroup.remove(spacing);
	}

	UpdateGUI();
}

void ofApp::onUpdateSize(int & newSize)
{
	font.loadFont("fonts/ArialBlack.ttf", newSize, true, true, true);
}

vector<vector<ofPolyline>> ofApp::GetAllCharOutlines()
{
	// create a vector for char outlines

	vector<vector<ofPolyline>> allCharOutlines;

	// get the full string

	string word = stringToDraw.get();

	// adjust for font size and letter spacing

	if (respace.get())
	{
		font.setLetterSpacing(letterSpacing.get());
	}

	ofPoint pos = ofPoint(position.get().x * ofGetWidth(), position.get().y * ofGetHeight()) - ofPoint(font.stringWidth(word) * 0.5f, font.stringHeight(word) * -0.5f);

	// get points

	vector<ofPath> word_path = font.getStringAsPoints(word, true, false);

	for (int chr = 0; chr < word.size(); chr++)
	{
		// get the original char outlines

		vector<ofPolyline> charOutlines = word_path[chr].getOutline();

		if (!drawInnerLines)
		{
			while (charOutlines.size() > 1)
			{
				charOutlines.pop_back();
			}
		}

		for (int o = 0; o < charOutlines.size(); o++)
		{
			// respace

			if (respace.get())
			{
				// add the first vertex again

				charOutlines[o].addVertex(charOutlines[o].getVertices()[0]);

				// calculate the length
				float length = charOutlines[o].getLengthAtIndex(charOutlines[o].getVertices().size() - 1);

				// resample by the defined density
				float spacingEqualized = length / ((int)(length / spacing));
				charOutlines[o] = charOutlines[o].getResampledBySpacing(spacingEqualized);

				// remove the last point if it's still a copy of the first)

				ofVec2f first = charOutlines[o].getVertices()[0];
				ofVec2f last = charOutlines[o].getVertices()[charOutlines[o].getVertices().size() - 1];
				if (first.distance(last) <= 1)
					charOutlines[o].removeVertex(charOutlines[o].getVertices().size() - 1);
			}

			// adjust by position

			vector<glm::vec3> verts = charOutlines[o].getVertices();
			for (int v = 0; v < verts.size(); v++)
			{
				charOutlines[o][v] = pos + verts[v];
			}
		}

		allCharOutlines.push_back(charOutlines);
	}

	return allCharOutlines;
}
