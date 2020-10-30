// Arianna Rudawski 2020
// www.ariannarudawski.com

#include "ClipperDrawer.h"

ClipperDrawer::ClipperDrawer(ofParameterGroup * mainGroup)
	: LineDrawer(mainGroup)
{
	lineGroup.setName("CLIPPINGS");

	// clipped lines

	lineGroup.add(drawClippedLines.set("draw clipped lines", false));

	// offset lines

	lineGroup.add(drawOffsetLines.set("draw offset lines", true));
	numOffsetLines.set("num offset lines", 1, 1, 100);
	deltaOffset.set("offset delta", 20, -500, 500);
	offsetJoinType.set("join type", 1, 0, 2);
	offsetEndType.set("end type", 0, 0, 3);
	miter.set("miter", 2, 0, 30);
	drawPipeOutline.set("draw pipe outline", true);
	indexPipeStart.set("pipe start index", 0, 0, 1000);

	if (drawOffsetLines.get())
	{
		lineGroup.add(numOffsetLines);
		lineGroup.add(deltaOffset);
		lineGroup.add(drawPipeOutline);
		//lineGroup.add(offsetJoinType);
		//lineGroup.add(offsetEndType);
		//lineGroup.add(miter);

		if (drawPipeOutline.get())
		{
			lineGroup.add(indexPipeStart);
		}
	}
	
}

void ClipperDrawer::setup(ofApp * app)
{
	LineDrawer::setup(app);

	app->stringToDraw.addListener(this, &ClipperDrawer::onStringParamsChanged);
	app->drawInnerLines.addListener(this, &ClipperDrawer::onBoolParamsChanged);
	app->respace.addListener(this, &ClipperDrawer::onBoolParamsChanged);
	app->position.addListener(this, &ClipperDrawer::onofVec2fParamsChanged);
	app->size.addListener(this, &ClipperDrawer::onIntParamsChanged);
	app->letterSpacing.addListener(this, &ClipperDrawer::onFloatParamsChanged);
	app->spacing.addListener(this, &ClipperDrawer::onIntParamsChanged);

	drawClippedLines.addListener(this, &ClipperDrawer::onBoolParamsChanged);
	drawOffsetLines.addListener(this, &ClipperDrawer::onBoolParamsChanged);
	drawOffsetLines.addListener(this, &ClipperDrawer::onToggleDrawOffset);
	numOffsetLines.addListener(this, &ClipperDrawer::onIntParamsChanged);
	offsetJoinType.addListener(this, &ClipperDrawer::onIntParamsChanged);
	offsetEndType.addListener(this, &ClipperDrawer::onIntParamsChanged);
	deltaOffset.addListener(this, &ClipperDrawer::onFloatParamsChanged);
	miter.addListener(this, &ClipperDrawer::onFloatParamsChanged);
	drawPipeOutline.addListener(this, &ClipperDrawer::onBoolParamsChanged);
	drawPipeOutline.addListener(this, &ClipperDrawer::onTogglePipeOutline);
	indexPipeStart.addListener(this, &ClipperDrawer::onIntParamsChanged);

	drawOffsetLines.addListener(app, &ofApp::onUpdateBool);
	drawPipeOutline.addListener(app, &ofApp::onUpdateBool);
}

void ClipperDrawer::draw(vector<vector<ofPolyline>> charOutlines, bool debug)
{
	if (!drawLines.get())
		return;

	// redo lines

	if (redoLines)
	{
		// get perimeter

		ofPolyline perimeter;
		perimeter.setClosed(true);

		perimeter.addVertex(0, 0);
		perimeter.addVertex(ofGetWidth(), 0);
		perimeter.addVertex(ofGetWidth(), ofGetHeight());
		perimeter.addVertex(0, ofGetHeight());

		// set up clipper based on parameters

		clipper.Clear();

		clipper.addPolyline(perimeter, ClipperLib::ptSubject);

		for (int i = 0; i < charOutlines.size(); ++i)
		{
			clipper.addPolylines(charOutlines[i], ClipperLib::ptClip);
		}

		// get clipped lines

		lines.clear();

		vector<ofPolyline> clippedLines = clipper.getClipped(ClipperLib::ClipType::ctIntersection);

		// if drawing pipe outline, add nearest border vertices to clipped lines

		if (drawOffsetLines && drawPipeOutline)
		{
			for (int i = 0; i < clippedLines.size(); ++i)
			{
				for (int j = 0; j < indexPipeStart; ++j)
				{
					ofPoint first = clippedLines[i][0];
					clippedLines[i].removeVertex(0);
					clippedLines[i].addVertex(first);
				}

				ofPoint startClosest = findClosestEdgePoint(clippedLines[i][0]);
				ofPoint endClosest = findClosestEdgePoint(clippedLines[i][clippedLines[i].size() - 1]);

				clippedLines[i].insertVertex(startClosest, 0);
				clippedLines[i].addVertex(endClosest);

			}
		}

		// draw clipped line

		if (drawClippedLines)
		{
			for (int i = 0; i < clippedLines.size(); ++i)
			{
				lines.push_back(clippedLines[i]);
			}
		}

		// draw offset lines

		if (drawOffsetLines)
		{
			for (int o = 1; o <= numOffsetLines; ++o)
			{
				vector<ofPolyline> offsets = clipper.getOffsets(
					clippedLines, 
					deltaOffset.get() * o, 
					(ClipperLib::JoinType)offsetJoinType.get(), 
					drawPipeOutline ? ClipperLib::EndType::etOpenButt : (ClipperLib::EndType)offsetEndType.get(), miter.get());

				clipper.Clear();
				clipper.addPolyline(perimeter, ClipperLib::ptSubject);
				clipper.addPolylines(offsets, ClipperLib::ptClip);

				vector<ofPolyline> clippedOffset = clipper.getClipped(ClipperLib::ClipType::ctIntersection);

				for (int i = 0; i < clippedOffset.size(); ++i)
				{
					lines.push_back(clippedOffset[i]);
				}
			}
		}


		redoLines = false;
	}

	// draw lines

	ofSetColor(0);

	for (int l = 0; l < lines.size(); ++l)
	{
		lines[l].draw();
	}
}

void ClipperDrawer::onBoolParamsChanged(bool & newVal)
{
	redoLines = true;
}

void ClipperDrawer::onIntParamsChanged(int & newVal)
{
	redoLines = true;
}

void ClipperDrawer::onFloatParamsChanged(float & newVal)
{
	redoLines = true;
}

void ClipperDrawer::onofVec2fParamsChanged(ofVec2f & newVal)
{
	redoLines = true;
}

void ClipperDrawer::onStringParamsChanged(string & newVal)
{
	redoLines = true;
}

void ClipperDrawer::onToggleDrawOffset(bool & newVal)
{
	if (newVal)
	{
		lineGroup.add(numOffsetLines);
		lineGroup.add(deltaOffset);
		lineGroup.add(drawPipeOutline);
		//lineGroup.add(offsetJoinType);
		//lineGroup.add(offsetEndType);
		//lineGroup.add(miter);
	}
	else
	{
		lineGroup.remove(numOffsetLines);
		lineGroup.remove(deltaOffset);
		lineGroup.remove(drawPipeOutline);
		//lineGroup.remove(offsetJoinType);
		//lineGroup.remove(offsetEndType);
		//lineGroup.remove(miter);

		drawPipeOutline = false; 
	}
}

void ClipperDrawer::onTogglePipeOutline(bool & newVal)
{
	if (newVal)
	{
		lineGroup.add(indexPipeStart);
	}
	else
	{
		lineGroup.remove(indexPipeStart);
	}
}

ofPoint ClipperDrawer::findClosestEdgePoint(ofPoint vert)
{
	// find edge intersection with the shortest distance

	map<float, ofPoint> distFromVertMap;

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

	return distFromVertMap.begin()->second;
}
