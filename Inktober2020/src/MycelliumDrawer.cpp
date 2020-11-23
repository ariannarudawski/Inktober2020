// Arianna Rudawski 2020
// www.ariannarudawski.com

#include "MycelliumDrawer.h"

MycelliumDrawer::MycelliumDrawer(ofParameterGroup * mainGroup)
	: LineDrawer(mainGroup)
{
	lineGroup.setName("MYCELLIUM");

	// mycellium parameters

	lineGroup.add(numVertsPerRing.set("num verts per ring", 3, 1, 100));
	lineGroup.add(numRingsPerChain.set("num rings per chain", 5, 1, 100));
	lineGroup.add(ringRadius.set("ring radius", 10, 1, 100));
	lineGroup.add(ringSpacing.set("ring spacing", 0.75, -2, 2));
	lineGroup.add(startAngleRad.set("start angle radians", 0, 0, TWO_PI));

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

void MycelliumDrawer::setup(ofApp * app)
{
	LineDrawer::setup(app);

	noiseOn.addListener(this, &MycelliumDrawer::onToggleNoiseOn);
	noiseOn.addListener(app, &ofApp::onUpdateBool);
}

void MycelliumDrawer::draw(vector<vector<ofPolyline>> charOutlines, bool debug, bool sortedVerts)
{
	if (!drawLines.get())
		return;

	for (int c = 0; c < charOutlines.size(); c++)
	{
		for (int co = 0; co < charOutlines[c].size(); ++co)
		{
			// turn all baseline verts into mycellium chains

			vector<ofPolyline> bLines;
			vector<glm::vec3> verts = charOutlines[c][co].getVertices();

			for (int v = 0; v < verts.size(); ++v)
			{
				glm::vec2 center = verts[v];
				float radius = ringRadius.get();
				int numVerts = numVertsPerRing.get();

				glm::vec2 travelDirection = charOutlines[c][co].getNormalAtIndex(v);
				float travelDirectionRad = atan2(travelDirection.y, travelDirection.x) + startAngleRad;
				travelDirection = glm::vec2(cos(travelDirectionRad), sin(travelDirectionRad));

				for (int r = 0; r < numRingsPerChain; ++r)
				{
					ofPolyline line = CreateRing(center, travelDirectionRad, ringRadius, numVerts);
					bLines.push_back(line);

					center += travelDirection * radius * ringSpacing.get();
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

void MycelliumDrawer::onToggleNoiseOn(bool & newVal)
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

ofPolyline MycelliumDrawer::CreateRing(glm::vec2 center, float startAngle, float radius, int numVerts)
{
	ofPolyline line;
	line.setClosed(true);

	for (int i = 0; i < numVerts; ++i)
	{
		float angle = startAngle + ((float)i / (float)numVerts) * TWO_PI;
		ofPoint vert(center.x + radius * cos(angle), center.y + radius * sin(angle));

		if ((vert.x < 0 || vert.x > ofGetWidth()) || (vert.y < 0 || vert.y > ofGetHeight()))
		{
			line.clear();
			break;
		}

		line.addVertex(vert);
	}

	return line;
}
