// Arianna Rudawski 2020
// www.ariannarudawski.com

#include "VertToEdgeDrawer.h"

VertToEdgeDrawer::VertToEdgeDrawer(ofParameterGroup * mainGroup)
	: LineDrawer(mainGroup)
{
	lineGroup.setName("VERTS TO EDGES");

	lineGroup.add(shortestLine.set("use shortest line", true));
	lineGroup.add(drawTopTwoLines.set("draw all the way through point", false));
	lineGroup.add(drawSlices.set("draw slices", true));

	lineGroup.add(useCircularEdge.set("circular edge", true));
	circularEdgeRadius.set("radius", 0.85, 0, 1);
	numVertsInCircularEdge.set("num verts", 365, 2, 365);

	if (useCircularEdge.get())
	{
		lineGroup.add(circularEdgeRadius);
		lineGroup.add(numVertsInCircularEdge);
	}

	// noise parameters

	lineGroup.add(noiseOn.set("noise on", false));
	noiseTime.set("noise time", 0, 0, 1000);
	noiseScale.set("noise scale", 1, 0, 100);
	noiseResolution.set("noise resolution", 0.5, 0, 1);

	if (noiseOn.get())
	{
		lineGroup.add(noiseTime);
		lineGroup.add(noiseScale);
		lineGroup.add(noiseResolution);
	}
}

void VertToEdgeDrawer::setup(ofApp * app)
{
	LineDrawer::setup(app);

	useCircularEdge.addListener(this, &VertToEdgeDrawer::onToggleCircularEdge);
	useCircularEdge.addListener(app, &ofApp::onUpdateBool);

	noiseOn.addListener(this, &VertToEdgeDrawer::onToggleNoiseOn);
	noiseOn.addListener(app, &ofApp::onUpdateBool);
}

void VertToEdgeDrawer::draw(vector<vector<ofPolyline>> charOutlines, bool debug, bool sortedVerts)
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

			if (drawSlices)
			{
				for (int v = 0; v < verts.size(); v += 2)
				{
					ofPolyline line = findSliceToEdge(verts[v], verts[(v + 1) % verts.size()]);
					bLines.push_back(line);
				}
			}
			else
			{
				for (int v = 0; v < verts.size(); ++v)
				{
					ofPolyline line = findLineToEdge(verts[v]);
					bLines.push_back(line);
				}
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
		float noiseX = 0;
		float noiseY = 0;

		// use the window edges

		ofPoint left = ofPoint(0, vert.y + noiseY);
		ofPoint right = ofPoint(ofGetWidth(), vert.y + noiseY);
		ofPoint up = ofPoint(vert.x + noiseX, 0);
		ofPoint down = ofPoint(vert.x + noiseX, ofGetHeight());

		distFromVertMap[vert.distance(left)] = left;
		distFromVertMap[vert.distance(right)] = right;
		distFromVertMap[vert.distance(up)] = up;
		distFromVertMap[vert.distance(down)] = down;
	}

	// create the shortest line

	ofPolyline line;
	if (shortestLine.get())
	{
		line.addVertex(distFromVertMap.begin()->second);
		line.addVertex(vert);

		if (drawTopTwoLines)
		{
			line.addVertex((++distFromVertMap.begin())->second);
		}
	}
	else
	{
		line.addVertex((--distFromVertMap.end())->second);
		line.addVertex(vert);

		if (drawTopTwoLines)
		{
			line.addVertex((----distFromVertMap.end())->second);
		}
	}

	return line;
}

ofPolyline VertToEdgeDrawer::findSliceToEdge(ofPoint vA, ofPoint vB)
{
	// find edge intersection with the shortest distance

	ofPoint vMid = (vA + vB) * 0.5f;

	map<float, ofPoint> distFromVertMap_A;
	//map<float, ofPoint> distFromVertMap_B;

	if (useCircularEdge.get())
	{
		float minWidth = ofGetWidth() < ofGetHeight() ? ofGetWidth() : ofGetHeight();
		ofPoint center(ofGetWidth() * 0.5f, ofGetHeight() * 0.5f);

		for (int v = 0; v < numVertsInCircularEdge.get(); ++v)
		{
			float angle = v * TWO_PI / (float)numVertsInCircularEdge.get();
			float radius = minWidth * circularEdgeRadius.get() * 0.5f;

			ofPoint point = center + ofPoint(radius * cos(angle), radius * sin(angle));

			//distFromVertMap_A[vA.distance(point)] = point;
			//distFromVertMap_B[vB.distance(point)] = point;
			
			distFromVertMap_A[vMid.distance(point)] = point;
		}
	}
	else
	{
		float noiseX = 0;
		float noiseY = 0;

		// use the window edges

		ofPoint left = ofPoint(0, vMid.y + noiseY);
		ofPoint right = ofPoint(ofGetWidth(), vMid.y + noiseY);
		ofPoint up = ofPoint(vMid.x + noiseX, 0);
		ofPoint down = ofPoint(vMid.x + noiseX, ofGetHeight());

		distFromVertMap_A[vMid.distance(left)] = left;
		distFromVertMap_A[vMid.distance(right)] = right;
		distFromVertMap_A[vMid.distance(up)] = up;
		distFromVertMap_A[vMid.distance(down)] = down;

		//left = ofPoint(0, vB.y + noiseY);
		//right = ofPoint(ofGetWidth(), vB.y + noiseY);
		//up = ofPoint(vB.x + noiseX, 0);
		//down = ofPoint(vB.x + noiseX, ofGetHeight());

		//distFromVertMap_B[vB.distance(left)] = left;
		//distFromVertMap_B[vB.distance(right)] = right;
		//distFromVertMap_B[vB.distance(up)] = up;
		//distFromVertMap_B[vB.distance(down)] = down;
	}

	// create the shortest line

	ofPolyline line;
	//line.addVertex(vMid);
	line.addVertex(vA);

	if (shortestLine.get())
	{
		line.addVertex(distFromVertMap_A.begin()->second);
		//line.addVertex(distFromVertMap_B.begin()->second);
	}
	else
	{
		line.addVertex((--distFromVertMap_A.end())->second);
		//line.addVertex((--distFromVertMap_B.end())->second);
	}

	//line.addVertex(vMid);

	line.addVertex(vB);
	line.addVertex(vA);

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

void VertToEdgeDrawer::onToggleNoiseOn(bool & newVal)
{
	if (newVal)
	{
		lineGroup.add(noiseTime);
		lineGroup.add(noiseScale);
		lineGroup.add(noiseResolution);
	}
	else
	{
		lineGroup.remove(noiseTime);
		lineGroup.remove(noiseScale);
		lineGroup.remove(noiseResolution);
	}
}
