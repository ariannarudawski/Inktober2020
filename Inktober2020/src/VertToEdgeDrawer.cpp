// Arianna Rudawski 2020
// www.ariannarudawski.com

#include "VertToEdgeDrawer.h"

VertToEdgeDrawer::VertToEdgeDrawer(ofParameterGroup * mainGroup)
	: LineDrawer(mainGroup)
{
	lineGroup.setName("VERTS TO EDGES");

	lineGroup.add(shortestLine.set("use shortest line", true));
	lineGroup.add(useCircularEdge.set("circular edge", true));
	circularEdgeRadius.set("radius", 0.75, 0, 1);
	numVertsInCircularEdge.set("num verts", 2, 2, 365);

	if (useCircularEdge.get())
	{
		lineGroup.add(circularEdgeRadius);
		lineGroup.add(numVertsInCircularEdge);
	}
}

void VertToEdgeDrawer::setup(ofApp * app)
{
	useCircularEdge.addListener(this, &VertToEdgeDrawer::onToggleCircularEdge);
	useCircularEdge.addListener(app, &ofApp::onUpdateBool);
}

void VertToEdgeDrawer::draw(vector<vector<ofPolyline>> charOutlines, bool debug)
{
	if (!drawLines.get())
		return;

	vector<glm::vec3> points;
	vector<glm::vec3> innerPoints;
	vector<glm::vec3> outerPoints;

	for (int c = 0; c < charOutlines.size(); c++)
	{
		for (int co = 0; co < charOutlines[c].size(); ++co)
		{
			// turn all baseline verts into reference points

			points.clear();
			innerPoints.clear();
			outerPoints.clear();

			vector<ofPolyline> bLines;
			vector<glm::vec3> verts = charOutlines[c][co].getVertices();

			for (int v = 0; v < verts.size(); v++)
			{			
				ofPolyline line = findLineToEdge(verts[v]);
				bLines.push_back(line);
			}

			// draw line

			ofSetColor(0);

			for (int l = 0; l < bLines.size(); ++l)
			{
				bLines[l].draw();
			}
		}
	}
}

ofPolyline VertToEdgeDrawer::findLineToEdge(ofPoint vert)
{
	// find edge intersection with the shortest distance

	map<float, ofPoint> distFromVertMap;

	if (useCircularEdge.get())
	{
		float minWidth = ofGetWidth() < ofGetHeight() ? ofGetWidth() : ofGetHeight();
		ofPoint center(ofGetWidth() * 0.5f, ofGetHeight() * 0.5f);

		for (int v = 0; v < numVertsInCircularEdge.get(); ++v)
		{
			float angle = v * TWO_PI / (float)numVertsInCircularEdge.get();
			float radius = minWidth * circularEdgeRadius.get() * 0.5f;
			ofPoint point = center + ofPoint(radius * cos(angle), radius * sin(angle));

			distFromVertMap[vert.distance(point)] = point;
		}
	}
	else
	{
		// use the window edges

		ofPoint left = ofPoint(0, vert.y);
		ofPoint right = ofPoint(ofGetWidth(), vert.y);
		ofPoint up = ofPoint(vert.x, 0);
		ofPoint down = ofPoint(vert.x, ofGetHeight());

		distFromVertMap[vert.distance(left)] = left;
		distFromVertMap[vert.distance(right)] = right;
		distFromVertMap[vert.distance(up)] = up;
		distFromVertMap[vert.distance(down)] = down;
	}

	// create the shortest line

	ofPolyline line;
	line.addVertex(vert);

	if (shortestLine.get())
		line.addVertex(distFromVertMap.begin()->second);
	else
		line.addVertex((--distFromVertMap.end())->second);

	return line;
}


void VertToEdgeDrawer::onToggleCircularEdge(bool & newVal)
{
	if (newVal)
	{
		lineGroup.add(circularEdgeRadius);
		lineGroup.add(numVertsInCircularEdge);
	}
	else
	{
		lineGroup.remove(circularEdgeRadius);
		lineGroup.remove(numVertsInCircularEdge);
	}
}