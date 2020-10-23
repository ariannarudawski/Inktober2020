// Arianna Rudawski 2020
// www.ariannarudawski.com

#include "VertToEdgeDrawer.h"

VertToEdgeDrawer::VertToEdgeDrawer(ofParameterGroup * mainGroup)
	: LineDrawer(mainGroup)
{
	lineGroup.setName("VERTS TO EDGES");
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
				ofPolyline line = findShortestLineToEdge(verts[v]);
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

ofPolyline VertToEdgeDrawer::findShortestLineToEdge(ofPoint vert)
{
	// get possivle edge intersections

	ofPoint left = ofPoint(0, vert.y);
	ofPoint right = ofPoint(ofGetWidth(), vert.y);
	ofPoint up = ofPoint(vert.x, 0);
	ofPoint down = ofPoint(vert.x, ofGetHeight());

	// find edge intersection with the shortest distance

	map<float, ofPoint> distFromVertMap; 
	distFromVertMap[vert.distance(left)] = left;
	distFromVertMap[vert.distance(right)] = right;
	distFromVertMap[vert.distance(up)] = up;
	distFromVertMap[vert.distance(down)] = down;

	// create shortest line

	ofPolyline line;
	line.addVertex(vert);
	line.addVertex(distFromVertMap.begin()->second);

	return line;;
}


