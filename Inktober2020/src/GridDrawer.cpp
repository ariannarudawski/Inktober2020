// Arianna Rudawski 2020
// www.ariannarudawski.com

#include "GridDrawer.h"

GridDrawer::GridDrawer(ofParameterGroup * mainGroup)
	: LineDrawer(mainGroup)
{
	lineGroup.setName("GRID SQUARES");
	
	lineGroup.add(columns.set("columns", 20, 1, 50));
	lineGroup.add(rows.set("rows", 20, 1, 50));
	lineGroup.add(bricks.set("bricks", false));
	lineGroup.add(inset.set("inset", ofVec2f(0.05f), ofVec2f(-1.0f), ofVec2f(1.0f)));

	// noise parameters

	lineGroup.add(noiseOn.set("noise on", false));
	noiseTime.set("noise time", 0, 0, 1000);
	noiseScale.set("noise scale", ofVec2f(1), ofVec2f(-100), ofVec2f(100));
	noiseResolution.set("noise resolution", ofVec2f(0.5), ofVec2f(-100), ofVec2f(100));

	if (noiseOn.get())
	{
		lineGroup.add(noiseTime);
		lineGroup.add(noiseScale);
		lineGroup.add(noiseResolution);
	}

	// randomness parameters

	lineGroup.add(randomOn.set("random on", false));
	randomScale.set("random scale", ofVec2f(1), ofVec2f(-100), ofVec2f(100));

	if (randomOn.get())
	{
		lineGroup.add(randomScale);
	}
}

void GridDrawer::setup(ofApp * app)
{
	LineDrawer::setup(app);

	app->stringToDraw.addListener(this, &GridDrawer::onStringParamsChanged);
	app->drawInnerLines.addListener(this, &GridDrawer::onBoolParamsChanged);
	app->respace.addListener(this, &GridDrawer::onBoolParamsChanged);
	app->position.addListener(this, &GridDrawer::onofVec2fParamsChanged);
	app->size.addListener(this, &GridDrawer::onIntParamsChanged);
	app->letterSpacing.addListener(this, &GridDrawer::onFloatParamsChanged);
	app->spacing.addListener(this, &GridDrawer::onIntParamsChanged);

	noiseOn.addListener(this, &GridDrawer::onToggleNoiseOn);
	noiseOn.addListener(this, &GridDrawer::onBoolParamsChanged);
	noiseOn.addListener(app, &ofApp::onUpdateBool);

	noiseTime.addListener(this, &GridDrawer::onFloatParamsChanged);
	noiseScale.addListener(this, &GridDrawer::onofVec2fParamsChanged);
	noiseResolution.addListener(this, &GridDrawer::onofVec2fParamsChanged);

	randomOn.addListener(this, &GridDrawer::onToggleRandomOn);
	randomOn.addListener(this, &GridDrawer::onBoolParamsChanged);
	randomOn.addListener(app, &ofApp::onUpdateBool);

	randomScale.addListener(this, &GridDrawer::onofVec2fParamsChanged);

	columns.addListener(this, &GridDrawer::onIntParamsChanged);
	rows.addListener(this, &GridDrawer::onIntParamsChanged);
	inset.addListener(this, &GridDrawer::onofVec2fParamsChanged);
	bricks.addListener(this, &GridDrawer::onBoolParamsChanged);
}

void GridDrawer::draw(vector<vector<ofPolyline>> charOutlines, bool debug)
{
	if (!drawLines.get())
		return;

	// redo lines

	if (redoLines)
	{
		lines.clear();

		float xStep = ofGetWidth() / (bricks ? (float)columns + 0.5f : (float)columns);
		float yStep = ofGetHeight() / (float)rows;

		vector<vector<GridSpace>> grid;

		for (int c = 0; c < columns; ++c)
		{
			grid.push_back(vector<GridSpace>());

			for (int r = 0; r < rows; ++r)
			{
				grid[c].push_back(GridSpace());

				float px = bricks ? ((r%2==0) ? ((c + 0.5f) * xStep) : (c * xStep)) : (c * xStep);
				float py = r * yStep;
				grid[c][r].gridBounds = ofRectangle(px, py, xStep, yStep);

				ofPolyline insetLine;
				insetLine.setClosed(true);
				insetLine.addVertex(grid[c][r].gridBounds.getTopLeft() + glm::vec3(xStep * inset.get().x, yStep * inset.get().y, 0));
				insetLine.addVertex(grid[c][r].gridBounds.getBottomLeft() + glm::vec3(xStep * inset.get().x, -yStep * inset.get().y, 0));
				insetLine.addVertex(grid[c][r].gridBounds.getBottomRight() + glm::vec3(-xStep * inset.get().x, -yStep * inset.get().y, 0));
				insetLine.addVertex(grid[c][r].gridBounds.getTopRight() + glm::vec3(-xStep * inset.get().x, yStep * inset.get().y, 0));
				insetLine.addVertex(grid[c][r].gridBounds.getTopLeft() + glm::vec3(xStep * inset.get().x, yStep * inset.get().y, 0));

				// use clipper to find outline of the overlap between this gridspace and the chars

				clipper.Clear();

				clipper.addPolyline(insetLine, ClipperLib::ptSubject);

				for (int i = 0; i < charOutlines.size(); ++i)
				{
					clipper.addPolylines(charOutlines[i], ClipperLib::ptClip);
				}

				grid[c][r].lines = clipper.getClipped(ClipperLib::ClipType::ctIntersection);

				for (int i = 0; i < grid[c][r].lines.size(); ++i)
				{
					ofPolyline line = grid[c][r].lines[i];

					if (noiseOn || randomOn)
					{
						float x = c * noiseResolution.get().x;
						float y = r * noiseResolution.get().y;

						ofPoint offset = noiseOn 
							? ofPoint(ofMap(ofNoise(x, y, noiseTime.get() + 1000), 0, 1, -1, 1),
								ofMap(ofNoise(y, x, noiseTime.get() - 1000), 0, 1, -1, 1)) * noiseScale.get()
							: ofPoint(ofRandom(-1, 1), ofRandom(-1, 1)) * randomScale.get();

						line.translate(offset);
					}

					lines.push_back(line);
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

bool GridDrawer::intersectsChar(ofRectangle rect, vector<vector<ofPolyline>> charOutlines)
{
	for (int i = 0; i < charOutlines.size(); ++i)
	{
		if ((intersectsChar(rect.getTopLeft(), charOutlines[i]) || intersectsChar(rect.getBottomLeft(), charOutlines[i]))
			|| (intersectsChar(rect.getBottomRight(), charOutlines[i]) || intersectsChar(rect.getTopRight(), charOutlines[i])))
			return true;
	}

	return false;
}

bool GridDrawer::intersectsChar(ofPoint point, vector<ofPolyline> charOutlines)
{
	if (charOutlines.size() < 1)
	{
		return false;
	}
	else if (charOutlines.size() == 1)
	{
		return charOutlines[0].inside(point);
	}
	else if (!charOutlines[0].inside(point))
	{
		return false;
	}
	else
	{
		ofPolyline mainOutline = charOutlines[0];

		for (int i = 1; i < charOutlines.size(); ++i)
		{
			if (charOutlines[i].inside(point))
				return false;
		}

		return true;
	}
}

void GridDrawer::onBoolParamsChanged(bool & newVal)
{
	redoLines = true;
}

void GridDrawer::onIntParamsChanged(int & newVal)
{
	redoLines = true;
}

void GridDrawer::onFloatParamsChanged(float & newVal)
{
	redoLines = true;
}

void GridDrawer::onofVec2fParamsChanged(ofVec2f & newVal)
{
	redoLines = true;
}

void GridDrawer::onStringParamsChanged(string & newVal)
{
	redoLines = true;
}

void GridDrawer::onToggleNoiseOn(bool & newVal)
{
	if (newVal)
	{
		randomOn = false;

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

void GridDrawer::onToggleRandomOn(bool & newVal)
{
	if (newVal)
	{
		noiseOn = false;
		lineGroup.add(randomScale);
	}
	else
	{
		lineGroup.remove(randomScale);
	}
}
