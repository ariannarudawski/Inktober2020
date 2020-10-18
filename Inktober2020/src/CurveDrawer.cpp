// Arianna Rudawski 2020
// www.ariannarudawski.com

#include "CurveDrawer.h"

CurveDrawer::CurveDrawer(ofParameterGroup * mainGroup)
	: LineDrawer(mainGroup)
{
	lineGroup.setName("CURVES");

	// general parameters

	lineGroup.add(curveSize.set("loop size", ofVec2f(0.0f, 0.0f), ofVec2f(-1.0f, -1.0f), ofVec2f(1.0f, 1.0f)));
	lineGroup.add(curveDepth.set("loop depth", 25, -100, 100));
	lineGroup.add(curveNumLines.set("num lines", 1, 1, 20));
	lineGroup.add(curveDeltaSize.set("delta loop size", ofVec2f(0.1f, 0.1f), ofVec2f(-1.0f, -1.0f), ofVec2f(1.0f, 1.0f)));
	lineGroup.add(curveDepthNoiseOn.set("use depth noise", false));
	lineGroup.add(curveSizeNoiseOn.set("use size noise", false));

	// noise parameters

	curveNoiseTime.set("noise time", 0, 0, 1000);
	curveNoiseScale.set("noise scale", 1, 0, 100);
	curveNoiseResolution.set("noise resolution", 0.5, 0, 1);

	if (curveDepthNoiseOn.get() || curveSizeNoiseOn.get())
	{
		lineGroup.add(curveNoiseTime);
		lineGroup.add(curveNoiseScale);
		lineGroup.add(curveNoiseResolution);
	}
}

void CurveDrawer::setup(ofApp * app)
{
	LineDrawer::setup(app);

	// curve listeners

	curveDepthNoiseOn.addListener(this, &CurveDrawer::onUpdateDepthNoiseFlag);
	curveDepthNoiseOn.addListener(app, &ofApp::onUpdateBool);

	curveSizeNoiseOn.addListener(this, &CurveDrawer::onUpdateSizeNoiseFlag);
	curveSizeNoiseOn.addListener(app, &ofApp::onUpdateBool);
}

void CurveDrawer::draw(vector<vector<ofPolyline>> charOutlines, bool debug)
{
	if (!drawLines.get())
		return;

	vector<glm::vec3> points;
	vector<glm::vec3> innerPoints;
	vector<glm::vec3> outerPoints;
	vector<float> pointNoise;

	for (int c = 0; c < charOutlines.size(); c++)
	{
		for (int co = 0; co < charOutlines[c].size(); ++co)
		{
			// turn all baseline verts into reference points

			points.clear();
			innerPoints.clear();
			outerPoints.clear();
			pointNoise.clear();

			vector<glm::vec3> verts = charOutlines[c][co].getVertices();
			for (int v = 0; v < verts.size(); v++)
			{
				pointNoise.push_back(ofMap(ofNoise(((c * 2) + v) * curveNoiseResolution, curveNoiseTime.get()), 0, 1, 0, curveNoiseScale));
				glm::vec3 normal = charOutlines[c][co].getNormalAtIndex(v) * curveDepth.get();

				if (curveDepthNoiseOn)
					normal *= pointNoise[v];

				points.push_back(verts[v]);
				innerPoints.push_back(verts[v] - normal);
				outerPoints.push_back(verts[v] + normal);
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
		}
	}
}

void CurveDrawer::onUpdateDepthNoiseFlag(bool & newVal)
{
	if (newVal) curveSizeNoiseOn = false;

	if ((newVal) && !lineGroup.contains(curveNoiseTime.getName()))
	{
		lineGroup.add(curveNoiseTime);
		lineGroup.add(curveNoiseScale);
		lineGroup.add(curveNoiseResolution);
	}
	else
	{
		lineGroup.remove(curveNoiseTime);
		lineGroup.remove(curveNoiseScale);
		lineGroup.remove(curveNoiseResolution);
	}
}

void CurveDrawer::onUpdateSizeNoiseFlag(bool & newVal)
{
	if (newVal) curveDepthNoiseOn = false;

	if ((newVal) && !lineGroup.contains(curveNoiseTime.getName()))
	{
		lineGroup.add(curveNoiseTime);
		lineGroup.add(curveNoiseScale);
		lineGroup.add(curveNoiseResolution);
	}
	else
	{
		lineGroup.remove(curveNoiseTime);
		lineGroup.remove(curveNoiseScale);
		lineGroup.remove(curveNoiseResolution);
	}
}
