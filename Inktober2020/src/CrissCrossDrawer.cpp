// Arianna Rudawski 2020
// www.ariannarudawski.com

#include "CrissCrossDrawer.h"
#include <random>

CrissCrossDrawer::CrissCrossDrawer(ofParameterGroup * mainGroup)
	: LineDrawer(mainGroup)
{
	lineGroup.setName("CRISS CROSS");

	lineGroup.add(shuffle.set("shuffle vertices", false));

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

bool isBetween(float num, float a, float b)
{
	if (num <= a && num >= b)
		return true;
	else if (num <= b && num >= a)
		return true;
	else
		return false;
}

bool hasIntersection(ofPoint a, ofPoint b, ofPoint aWall, ofPoint bWall, ofPoint & intersection)
{
	// Line AB represented as a1x + b1y = c1 
	float a1 = b.y - a.y;
	float b1 = a.x - b.x;
	float c1 = a1 * (a.x) + b1 * (a.y);

	// Line CD represented as a2x + b2y = c2 
	float a2 = bWall.y - aWall.y;
	float b2 = aWall.x - bWall.x;
	float c2 = a2 * (aWall.x) + b2 * (aWall.y);

	float determinant = a1 * b2 - a2 * b1;

	if (determinant == 0)
	{
		return false;
	}
	else
	{
		float x = (b2*c1 - b1 * c2) / determinant;
		float y = (a1*c2 - a2 * c1) / determinant;

		if (isBetween(x, aWall.x, bWall.x))
		{
			intersection.set(x, y);
			return true;
		}
		else
		{
			return false;
		}
	}
}

bool hasLeftIntersection(ofPoint a, ofPoint b, ofPoint & intersection)
{
	return hasIntersection(a, b, ofPoint(0, 0), ofPoint(0, ofGetHeight()), intersection);
}

bool hasRightIntersection(ofPoint a, ofPoint b, ofPoint & intersection)
{
	return hasIntersection(a, b, ofPoint(ofGetWidth(), 0), ofPoint(ofGetWidth(), ofGetHeight()), intersection);
}

bool hasTopIntersection(ofPoint a, ofPoint b, ofPoint & intersection)
{
	return hasIntersection(a, b, ofPoint(0, 0), ofPoint(ofGetWidth(), 0), intersection);
}

bool hasBottomIntersection(ofPoint a, ofPoint b, ofPoint & intersection)
{
	return hasIntersection(a, b, ofPoint(0, ofGetHeight()), ofPoint(ofGetWidth(), ofGetHeight()), intersection);
}

void CrissCrossDrawer::setup(ofApp * app)
{
	LineDrawer::setup(app);

	noiseOn.addListener(this, &CrissCrossDrawer::onToggleNoiseOn);
	noiseOn.addListener(app, &ofApp::onUpdateBool);
}

void CrissCrossDrawer::draw(vector<vector<ofPolyline>> charOutlines, bool debug)
{
	if (!drawLines.get())
		return;

	glm::vec2 top = glm::vec2(ofGetWidth(), 0) - glm::vec2(0, 0);
	glm::vec2 bottom = glm::vec2(ofGetWidth(), ofGetHeight()) - glm::vec2(0, ofGetHeight());
	glm::vec2 left = glm::vec2(0, ofGetHeight()) - glm::vec2(0, 0);
	glm::vec2 right = glm::vec2(ofGetWidth(), ofGetHeight()) - glm::vec2(ofGetWidth(), 0);

	float noiseIndex = 0;

	for (int c = 0; c < charOutlines.size(); c++)
	{
		for (int co = 0; co < charOutlines[c].size(); ++co)
		{
			// get all the verts of this outline

			vector<glm::vec3> verts = charOutlines[c][co].getVertices();

			if (shuffle.get())
			{
				std::shuffle(begin(verts), end(verts), default_random_engine{});
			}

			// create lines to draw through these vertices

			vector<ofPolyline> bLines;

			for (int v = 0; v < verts.size(); ++v)
			{
				ofPolyline line;

				ofPoint a = verts[v];
				ofPoint b = verts[(v + 1) % verts.size()];
				ofPoint intersection;

				if (hasLeftIntersection(a, b, intersection))
				{
					if (noiseOn)
					{
						float noise = ofMap(ofNoise(noiseTime + 100, noiseIndex += noiseResolution), 0, 1, -noiseScale, noiseScale);

						if (isBetween(intersection.y + noise, 0, ofGetHeight()))
						{
							line.addVertex(intersection + ofPoint(0, noise));
						}
					}
					else
					{
						line.addVertex(intersection);
					}
				}

				if (hasRightIntersection(a, b, intersection))
				{
					if (noiseOn)
					{
						float noise = ofMap(ofNoise(noiseTime + 100, noiseIndex += noiseResolution), 0, 1, -noiseScale, noiseScale);

						if (isBetween(intersection.y + noise, 0, ofGetHeight()))
						{
							line.addVertex(intersection + ofPoint(0, noise));
						}
					}
					else
					{
						line.addVertex(intersection);
					}
				}

				if (hasTopIntersection(a, b, intersection))
				{
					if (noiseOn)
					{
						float noise = ofMap(ofNoise(noiseTime + 100, noiseIndex += noiseResolution), 0, 1, -noiseScale, noiseScale);

						if (isBetween(intersection.x + noise, 0, ofGetWidth()))
						{
							line.addVertex(intersection + ofPoint(noise, 0));
						}
					}
					else
					{
						line.addVertex(intersection);
					}
				}

				if (hasBottomIntersection(a, b, intersection))
				{
					if (noiseOn)
					{
						float noise = ofMap(ofNoise(noiseTime + 100, noiseIndex += noiseResolution), 0, 1, -noiseScale, noiseScale);

						if (isBetween(intersection.x + noise, 0, ofGetWidth()))
						{
							line.addVertex(intersection + ofPoint(noise, 0));
						}
					}
					else
					{
						line.addVertex(intersection);
					}
				}

				bLines.push_back(line);
			}

			// draw lines

			ofSetColor(0);

			for (int l = 0; l < bLines.size(); ++l)
			{
				bLines[l].draw();
			}
		}
	}
}

void CrissCrossDrawer::onToggleNoiseOn(bool & newVal)
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
