// Arianna Rudawski 2020
// www.ariannarudawski.com

#include "MatchsticksDrawer.h"

MatchsticksDrawer::MatchsticksDrawer(ofParameterGroup * mainGroup)
	: LineDrawer(mainGroup)
{
	lineGroup.setName("MATCHSTICKS");
	
	lineGroup.add(matchsticksClockwise.set("clockwise", false));
	lineGroup.add(matchsticksRays.set("num rays", 3, 1, 25));
	lineGroup.add(matchsticksRayDistance.set("ray distance", 5, 0, 500));
	lineGroup.add(matchsticksRaySourceSpacing.set("ray source spacing", 1, 1, 25));
	lineGroup.add(matchstickDepth.set("depth", 1, 1, 100));
	lineGroup.add(matchstickSourceDepth.set("ray source depth", 0.5, 0, 1));
	lineGroup.add(matchstickDestDepth.set("ray dest depth", 0.5, 0, 1));

}

void MatchsticksDrawer::draw(vector<vector<ofPolyline>> charOutlines, bool debug)
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

			vector<glm::vec3> verts = charOutlines[c][co].getVertices();
			for (int v = 0; v < verts.size(); v++)
			{			
				glm::vec3 vertex = verts[v];
				glm::vec3 normal = charOutlines[c][co].getNormalAtIndex(v) * matchstickDepth.get();
			
				points.push_back(verts[v]);
				innerPoints.push_back(verts[v] - normal);
				outerPoints.push_back(verts[v] + normal);
			}

			// create lines

			vector<ofPolyline> bLines;

			for (int v = 0; v < points.size(); v += matchsticksRaySourceSpacing)
			{
				ofPoint aInner = innerPoints[v];
				ofPoint aOuter = outerPoints[v];

				for (int i = 0; i < matchsticksRays.get(); ++i)
				{
					ofPoint bInner = innerPoints[(v + i + matchsticksRayDistance) % points.size()];
					ofPoint bOuter = outerPoints[(v + i + matchsticksRayDistance) % points.size()];

					ofPoint src = aInner.getInterpolated(aOuter, matchstickSourceDepth);
					ofPoint dest = bInner.getInterpolated(bOuter, matchstickDestDepth);

					ofPolyline bLine;
					bLine.addVertex(src);
					bLine.addVertex(dest);
					bLines.push_back(bLine);
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


