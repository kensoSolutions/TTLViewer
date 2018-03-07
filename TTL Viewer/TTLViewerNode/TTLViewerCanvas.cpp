/*
    ------------------------------------------------------------------

    This file is part of the Open Ephys GUI
    Copyright (C) 2018 Open Ephys

    ------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "TTLViewerCanvas.h"


TTLViewerCanvas::TTLViewerCanvas(TTLViewerNode* n) :
processor(n), newSpike(false), labelFont("Default", 11.0f, Font::plain)
{

    viewport = new Viewport();
    ttlViewer = new TTLViewer(this, viewport);
    thresholdCoordinator = new SpikeThresholdCoordinator();
    ttlViewer->registerThresholdCoordinator(thresholdCoordinator);

    viewport->setViewedComponent(ttlViewer, false);
    viewport->setScrollBarsShown(true, false);

    scrollBarThickness = viewport->getScrollBarThickness();

    clearButton = new UtilityButton("Clear plots", Font("Small Text", 13, Font::plain));
    clearButton->setRadius(3.0f);
    clearButton->addListener(this);
    addAndMakeVisible(clearButton);

    lockThresholdsButton = new UtilityButton("Lock Thresholds", Font("Small Text", 13, Font::plain));
    lockThresholdsButton->setRadius(3.0f);
    lockThresholdsButton->addListener(this);
    lockThresholdsButton->setClickingTogglesState(true);
    addAndMakeVisible(lockThresholdsButton);

    invertSpikesButton = new UtilityButton("Invert Spikes", Font("Small Text", 13, Font::plain));
    invertSpikesButton->setRadius(3.0f);
    invertSpikesButton->addListener(this);
    invertSpikesButton->setClickingTogglesState(true);
    invertSpikesButton->setToggleState(false, sendNotification);
    addAndMakeVisible(invertSpikesButton);

	voltageRanges.add("0.1");
	voltageRanges.add("0.25");
	voltageRanges.add("0.5");
	voltageRanges.add("1");
	voltageRanges.add("2");
	voltageRanges.add("5");
	voltageRanges.add("10");
	voltageRanges.add("20");
	voltageRanges.add("50");

	voltageRange = new ComboBox("Voltage Range (mV):");
	voltageRange->setEditableText(false);
	voltageRange->addItemList(voltageRanges, 1);
	voltageRange->setJustificationType(Justification::centredLeft);
	voltageRange->addListener(this);
	voltageRange->setSelectedId(2);
	addAndMakeVisible(voltageRange);

	voltageRangeLabel = new Label("Name", "\nVoltage(mV):");
	voltageRangeLabel->setFont(labelFont);
	voltageRangeLabel->setColour(Label::textColourId, Colours::lightgrey);
	addAndMakeVisible(voltageRangeLabel);


	shownLines.add("1");
	shownLines.add("2");
	shownLines.add("3");
	shownLines.add("4");
	shownLines.add("5");

	linesLabel = new Label("Name", "Lines to show \nat plot:");
	linesLabel->setFont(labelFont);
	linesLabel->setColour(Label::textColourId, Colours::lightgrey);
	addAndMakeVisible(linesLabel);

	linesList = new ComboBox("Lines Shown List");
	linesList->addItemList(shownLines, 1);
	linesList->setEditableText(false);
	linesList->setJustificationType(Justification::centredLeft);
	linesList->addListener(this);
	linesList->setSelectedId(1);
	addAndMakeVisible(linesList);
	linesToShow = 1;

    addAndMakeVisible(viewport);

    setWantsKeyboardFocus(true);


    update();

}

TTLViewerCanvas::~TTLViewerCanvas()
{
    processor->removeSpikePlots();
}

void TTLViewerCanvas::beginAnimation()
{
    std::cout << "TTLViewerCanvas beginning animation." << std::endl;

    startCallbacks();
}

void TTLViewerCanvas::endAnimation()
{
    std::cout << "TTLViewerCanvas ending animation." << std::endl;

    stopCallbacks();
}

void TTLViewerCanvas::update()
{

    std::cout << "Updating TTLViewerCanvas" << std::endl;

    int nPlots = processor->getNumElectrodes();
    processor->removeSpikePlots();

   // if (nPlots != ttlViewer->getNumPlots())
   // {
        ttlViewer->removePlots();

        for (int i = 0; i < nPlots; i++)
        {
            SpikePlot* sp = ttlViewer->addSpikePlot(processor->getNumberOfChannelsForElectrode(i), i,
                                                       processor->getNameForElectrode(i));
            processor->addSpikePlotForElectrode(sp, i);
        }
   // }
    /*else
    {
        for (int i = 0; i < nPlots; i++)
        {
            processor->addSpikePlotForElectrode(ttlViewer->getSpikePlot(i), i);
        }
    }*/

    ttlViewer->resized();
    ttlViewer->repaint();
}


void TTLViewerCanvas::refreshState()
{
    // called when the component's tab becomes visible again
    resized();
}

void TTLViewerCanvas::resized()
{    viewport->setBounds(0,0,getWidth(),getHeight()-90);

    ttlViewer->setBounds(0,0,getWidth()-scrollBarThickness, ttlViewer->getTotalHeight());

    clearButton->setBounds(10, getHeight()-40, 100,20);

    lockThresholdsButton->setBounds(130, getHeight()-40, 130,20);

    invertSpikesButton->setBounds(270, getHeight()-40, 130,20);

	voltageRange->setBounds(410, getHeight() - 40, 90, 20);

	voltageRangeLabel->setBounds(405, getHeight() - 70, 90, 30);

	/*timebaseRange->setBounds(510, getHeight() - 40, 90, 20);

	timebaseRangeLabel->setBounds(505, getHeight() - 70, 90, 30);*/

	linesList->setBounds(510, getHeight() - 40, 80, 20);

	linesLabel->setBounds(505, getHeight() - 70, 120, 30);

}

void TTLViewerCanvas::comboBoxChanged(ComboBox* cb)
{
	
	if (cb == voltageRange)
	{
		int ID = cb->getSelectedId();
		String value = cb->getItemText(ID - 1);
		voltageValue = value.getFloatValue();
	}
	else if (cb == linesList)
	{
		int ID = cb->getSelectedId();
		//std::cout << ID << std::endl;
		String value = cb->getItemText(ID - 1);
		linesToShow = (int)value.getFloatValue();
	}
	
	ttlViewer->setLimits();
	
	
}

float TTLViewerCanvas::getVoltageValue()
{
	//std::cout << voltageValue << std::endl;
	return voltageValue;
}

int TTLViewerCanvas::getLinesToShowValue()
{
	//std::cout << voltageValue << std::endl;
	return linesToShow;
}

void TTLViewerCanvas::paint(Graphics& g)
{

    g.fillAll(Colours::darkgrey);

}

void TTLViewerCanvas::refresh()
{
    processSpikeEvents();

    repaint();
}


void TTLViewerCanvas::processSpikeEvents()
{

    processor->setParameter(2, 0.0f); // request redraw

}

bool TTLViewerCanvas::keyPressed(const KeyPress& key)
{

    KeyPress c = KeyPress::createFromDescription("c");

    if (key.isKeyCode(c.getKeyCode())) // C
    {
        ttlViewer->clear();

        std::cout << "Clearing display" << std::endl;
        return true;
    }

    return false;

}

void TTLViewerCanvas::buttonClicked(Button* button)
{

    if (button == clearButton)
    {
        ttlViewer->clear();
    }
    else if (button == lockThresholdsButton)
    {
        thresholdCoordinator->setLockThresholds(button->getToggleState());
    }
    else if (button == invertSpikesButton)
    {
        ttlViewer->invertSpikes(button->getToggleState());
    }
}

void TTLViewerCanvas::saveVisualizerParameters(XmlElement* xml)
{

    XmlElement* xmlNode = xml->createNewChildElement("TTLVIEWER");

    xmlNode->setAttribute("LockThresholds",lockThresholdsButton->getToggleState());
    xmlNode->setAttribute("InvertSpikes",invertSpikesButton->getToggleState());
	xmlNode->setAttribute("VoltageRangeId", voltageRange->getSelectedId());

    for (int i = 0; i < ttlViewer->getNumPlots(); i++)
    {
        XmlElement* plotNode = xmlNode->createNewChildElement("PLOT");

        for (int j = 0; j < ttlViewer->getNumChannelsForPlot(i); j++)
        {
            XmlElement* axisNode = plotNode->createNewChildElement("AXIS");
            axisNode->setAttribute("thresh",ttlViewer->getThresholdForWaveAxis(i,j));
           // axisNode->setAttribute("range",ttlViewer->getRangeForWaveAxis(i,j));
        }
    }

}

void TTLViewerCanvas::loadVisualizerParameters(XmlElement* xml)
{
    forEachXmlChildElement(*xml, xmlNode)
    {
        if (xmlNode->hasTagName("TTLVIEWER"))
        {
            ttlViewer->invertSpikes(xmlNode->getBoolAttribute("InvertSpikes"));
            invertSpikesButton->setToggleState(xmlNode->getBoolAttribute("InvertSpikes"), dontSendNotification);
            lockThresholdsButton->setToggleState(xmlNode->getBoolAttribute("LockThresholds"), sendNotification);
			const int voltageId = xmlNode->getIntAttribute("VoltageRangeId");
			voltageRange->setSelectedId(voltageId);
			const float voltageVal = voltageRange->getItemText(voltageId - 1).getFloatValue()*1000.0f;
            int plotIndex = -1;

            forEachXmlChildElement(*xmlNode, plotNode)
            {
                if (plotNode->hasTagName("PLOT"))
                {

                    plotIndex++;

                    std::cout << "PLOT NUMBER " << plotIndex << std::endl;

                    int channelIndex = -1;

                    forEachXmlChildElement(*plotNode, channelNode)
                    {

                        if (channelNode->hasTagName("AXIS"))
                        {
                            channelIndex++;

                            std::cout << "CHANNEL NUMBER " << channelIndex << std::endl;

                            ttlViewer->setThresholdForWaveAxis(plotIndex,
                                                                  channelIndex,
                                                                  channelNode->getDoubleAttribute("thresh"));

                            ttlViewer->setRangeForWaveAxis(plotIndex,
                                                              channelIndex,
                                                              voltageVal);

                        }
                    }


                }
            }

        }
    }
}

// ----------------------------------------------------------------

TTLViewer::TTLViewer(TTLViewerCanvas* sdc, Viewport* v) :
    canvas(sdc), viewport(v), shouldInvert(false), thresholdCoordinator(nullptr)
{

    totalHeight = 1000;

}

TTLViewer::~TTLViewer()
{

}

void TTLViewer::clear()
{
    if (spikePlots.size() > 0)
    {
        for (int i = 0; i < spikePlots.size(); i++)
        {
            spikePlots[i]->clear();
        }
    }

}


void TTLViewer::removePlots()
{
    spikePlots.clear();

}

SpikePlot* TTLViewer::addSpikePlot(int numChannels, int electrodeNum, String name_)
{

    std::cout << "Adding new spike plot." << std::endl;

    SpikePlot* spikePlot = new SpikePlot(canvas, electrodeNum, 1000 + numChannels, name_);
    spikePlots.add(spikePlot);
    addAndMakeVisible(spikePlot);
    spikePlot->invertSpikes(shouldInvert);
    if (thresholdCoordinator)
    {
        spikePlot->registerThresholdCoordinator(thresholdCoordinator);
    }

    return spikePlot;
}

SpikePlot* TTLViewer::getSpikePlot(int index)
{
    return spikePlots[index];
}

void TTLViewer::paint(Graphics& g)
{

    g.fillAll(Colours::grey);

}

void TTLViewer::resized()
{
    // this is kind of a mess -- is there any way to optimize it?

    if (spikePlots.size() > 0)
    {

        int w = getWidth();

        int numColumns = 1;
        int column, row;

        int stereotrodeStart = 0;
        int tetrodeStart = 0;

        int singlePlotIndex = -1;
        int stereotrodePlotIndex = -1;
        int tetrodePlotIndex = -1;
        int index = -1;

        float width = 0;
        float height = 0;


        float maxHeight = 0;

        for (int i = 0; i < spikePlots.size(); i++)
        {

            if (spikePlots[i]->nChannels == 1)
            {
                index = ++singlePlotIndex;
				numColumns = (int) 1.0f;//jmax(w / spikePlots[i]->minWidth, 1.0f);
                width = jmin((float) w / (float) numColumns, (float) getWidth());
				height = width *  spikePlots[i]->aspectRatio;

            }
            else if (spikePlots[i]->nChannels == 2)
            {
                index = ++stereotrodePlotIndex;
                numColumns = (int) jmax(w / spikePlots[i]->minWidth, 1.0f);
                width = jmin((float) w / (float) numColumns, (float) getWidth());
                height = width * spikePlots[i]->aspectRatio;

            }
            else if (spikePlots[i]->nChannels == 4)
            {
                index = ++tetrodePlotIndex;
                numColumns = (int) jmax(w / spikePlots[i]->minWidth, 1.0f);
                width = jmin((float) w / (float) numColumns, (float) getWidth());
                height = width * spikePlots[i]->aspectRatio;
            }

            column = index % numColumns;

            row = index / numColumns;

            spikePlots[i]->setBounds(width*column, row*height, width, height);

            maxHeight = jmax(maxHeight, row*height + height);

            if (spikePlots[i]->nChannels == 1)
            {
                stereotrodeStart = (int)(height*(float(row)+1));
            }
            else if (spikePlots[i]->nChannels == 2)
            {
                tetrodeStart = (int)(height*(float(row)+1));
            }

        }


        for (int i = 0; i < spikePlots.size(); i++)
        {

            int x = spikePlots[i]->getX();
            int y = spikePlots[i]->getY();
            int w2 = spikePlots[i]->getWidth();
            int h2 = spikePlots[i]->getHeight();

            if (spikePlots[i]->nChannels == 2)
            {
                spikePlots[i]->setBounds(x, y+stereotrodeStart, w2, h2);
                maxHeight = jmax(maxHeight, (float) y+stereotrodeStart+h2);

            }
            else if (spikePlots[i]->nChannels == 4)
            {
                spikePlots[i]->setBounds(x, y+stereotrodeStart+tetrodeStart, w2, h2);
                maxHeight = jmax(maxHeight, (float) y+stereotrodeStart+tetrodeStart+h2);
            }


        }

        totalHeight = (int) maxHeight + 50;

        // std::cout << "New height = " << totalHeight << std::endl;

        setBounds(0, 0, getWidth(), totalHeight);
    }

}

void TTLViewer::mouseDown(const MouseEvent& event)
{

}


void TTLViewer::invertSpikes(bool shouldInvert_)
{

    shouldInvert = shouldInvert_;

    for (int i = 0; i < spikePlots.size(); i++)
    {
        spikePlots[i]->invertSpikes(shouldInvert_);
    }

    //std::cout << "Invert spikes? " << shouldInvert_ << std::endl;
}

void TTLViewer::setLimits()
{
	for (int i = 0; i < spikePlots.size(); i++)
	{
		spikePlots[i]->setLimitsOnAxes();
	}
}

void TTLViewer::plotSpike(const SpikeEvent* spike, int electrodeNum)
{
    spikePlots[electrodeNum]->processSpikeObject(spike);
}

void TTLViewer::registerThresholdCoordinator(SpikeThresholdCoordinator* stc)
{
    thresholdCoordinator = stc;
    for (int i=0; i < spikePlots.size(); i++)
    {
        spikePlots[i]->registerThresholdCoordinator(stc);
    }
}

int TTLViewer::getNumPlots()
{
    return spikePlots.size();
}

int TTLViewer::getNumChannelsForPlot(int plotNum)
{
    return spikePlots[plotNum]->nChannels;
}

float TTLViewer::getThresholdForWaveAxis(int plotNum, int axisNum)
{
    return spikePlots[plotNum]->getDisplayThresholdForChannel(axisNum);
}

float TTLViewer::getRangeForWaveAxis(int plotNum, int axisNum)
{
    return spikePlots[plotNum]->getRangeForChannel(axisNum);
}

void TTLViewer::setThresholdForWaveAxis(int plotNum, int axisNum, float range)
{
    if (spikePlots.size() > plotNum)
        return spikePlots[plotNum]->setDisplayThresholdForChannel(axisNum, range);
}

void TTLViewer::setRangeForWaveAxis(int plotNum, int axisNum, float range)
{
    if (spikePlots.size() > plotNum)
        return spikePlots[plotNum]->setRangeForChannel(axisNum, range);
}

// ----------------------------------------------------------------

SpikePlot::SpikePlot(TTLViewerCanvas* sdc, int elecNum, int p, String name_) :///////////////////////////////////////////////////////////////////////////////////////////
    canvas(sdc), isSelected(false), electrodeNumber(elecNum),  plotType(p),
    limitsChanged(true), name(name_)

{

    font = Font("Default", 15, Font::plain);

    switch (p)
    {
        case SINGLE_PLOT:
            // std::cout<<"SpikePlot as SINGLE_PLOT"<<std::endl;
            nWaveAx = 1;
            nProjAx = 0;
            nChannels = 1;
            minWidth = 500;
            aspectRatio = 0.1f;
            break;
        case STEREO_PLOT:
            //  std::cout<<"SpikePlot as STEREO_PLOT"<<std::endl;
            nWaveAx = 2;
            nProjAx = 1;
            nChannels = 2;
            minWidth = 300;
            aspectRatio = 0.5f;
            break;
        case TETRODE_PLOT:
            // std::cout<<"SpikePlot as TETRODE_PLOT"<<std::endl;
            nWaveAx = 4;
            nProjAx = 6;
            nChannels = 4;
            minWidth = 400;
            aspectRatio = 0.5f;
            break;
            //        case HIST_PLOT:
            //            nWaveAx = 1;
            //            nProjAx = 0;
            //            nHistAx = 1;
            //            break;
        default: // unsupported number of axes provided
            std::cout << "SpikePlot as UNKNOWN, defaulting to SINGLE_PLOT" << std::endl;
            nWaveAx = 1;
            nProjAx = 0;
            plotType = SINGLE_PLOT;
            nChannels = 1;
    }

    initAxes();

   /* for (int i = 0; i < nChannels; i++)
    {
        UtilityButton* rangeButton = new UtilityButton("250", Font("Small Text", 10, Font::plain));
        rangeButton->setRadius(3.0f);
        rangeButton->addListener(this);
        addAndMakeVisible(rangeButton);

        rangeButtons.add(rangeButton);
    }*/

}

SpikePlot::~SpikePlot()
{
    if (thresholdCoordinator)
    {
        thresholdCoordinator->unregisterSpikePlot(this);
    }
}

void SpikePlot::paint(Graphics& g)
{

    //const MessageManagerLock mmLock;

    g.setColour(Colours::white);
    g.drawRect(0,0,getWidth(),getHeight());

    g.setFont(font);

    g.drawText(name,10,0,800,20,Justification::left,false);

}

void SpikePlot::processSpikeObject(const SpikeEvent* s)
{
    // std::cout << "ElectrodePlot::processSpikeObject()" << std::endl;

    // first, check if it's above threshold
    // bool aboveThreshold = false;

    // for (int i = 0; i < nWaveAx; i++)
    // {
    //     aboveThreshold = aboveThreshold | wAxes[i]->checkThreshold(s);
    // }

    // if (aboveThreshold)
    // {
    for (int i = 0; i < nWaveAx; i++)
        wAxes[i]->updateSpikeData(s);

    for (int i = 0; i < nProjAx; i++)
        pAxes[i]->updateSpikeData(s);
    // }

    //     if (aboveThreshold && isRecording)
    //     {
    //         // write spike to disk
    //       writeSpike(s);
    //     }

}

void SpikePlot::select()
{
    isSelected = true;
}

void SpikePlot::deselect()
{
    isSelected = false;
}

void SpikePlot::initAxes()
{
    initLimits();
	float vValue = canvas->getVoltageValue();
	int lValue = canvas->getLinesToShowValue();
    for (int i = 0; i < nWaveAx; i++)
    {
        WaveAxes* wAx = new WaveAxes(WAVE1 + i, lValue);////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
        wAxes.add(wAx);
        addAndMakeVisible(wAx);
        ranges.add(vValue*1000.0f); // default range is 250 microvolts
    }

    for (int i = 0; i < nProjAx; i++)
    {
        ProjectionAxes* pAx = new ProjectionAxes(PROJ1x2 + i);
        pAxes.add(pAx);
        addAndMakeVisible(pAx);
    }

    setLimitsOnAxes(); // initialize the ranges
}



void SpikePlot::resized()
{

    float width = getWidth()-10;
    float height = getHeight()-25;

    float axesWidth = 0;
    float axesHeight = 0;

    // to compute the axes positions we need to know how many columns of proj and wave axes should exist
    // using these two values we can calculate the positions of all of the sub axes
    int nProjCols = 0;
    int nWaveCols = 0;

    switch (plotType)
    {
        case SINGLE_PLOT:
            nProjCols = 0;
            nWaveCols = 1;
            axesWidth = width;
            axesHeight = height;
            break;

        case STEREO_PLOT:
            nProjCols = 1;
            nWaveCols = 2;
            axesWidth = width/2;
            axesHeight = height;
            break;
        case TETRODE_PLOT:
            nProjCols = 3;
            nWaveCols = 2;
            axesWidth = width/4;
            axesHeight = height/2;
            break;
    }

    for (int i = 0; i < nWaveAx; i++)
    {
        wAxes[i]->setBounds(5 + (i % nWaveCols) * axesWidth/nWaveCols, 20 + (i/nWaveCols) * axesHeight, axesWidth/nWaveCols, axesHeight);
       /* rangeButtons[i]->setBounds(8 + (i % nWaveCols) * axesWidth/nWaveCols,
                                   20 + (i/nWaveCols) * axesHeight + axesHeight - 18,
                                   25, 15);*/
    }

    for (int i = 0; i < nProjAx; i++)
        pAxes[i]->setBounds(5 + (1 + i%nProjCols) * axesWidth, 20 + (i/nProjCols) * axesHeight, axesWidth, axesHeight);


}

/*void SpikePlot::buttonClicked(Button* button)
{
    UtilityButton* buttonThatWasClicked = (UtilityButton*) button;

    int index = rangeButtons.indexOf(buttonThatWasClicked);
    String label;

    if (ranges[index] == 250.0f)
    {
        ranges.set(index, 500.0f);
        label = "500";
    }
    else if (ranges[index] == 500.0f)
    {
        ranges.set(index, 100.0f);
        label = "100";
    }
    else if (ranges[index] == 100.0f)
    {
        ranges.set(index, 250.0f);
        label = "250";
    }

    buttonThatWasClicked->setLabel(label);

    setLimitsOnAxes();

}*/

void SpikePlot::setLimitsOnAxes()
{
    //std::cout<<"SpikePlot::setLimitsOnAxes()"<<std::endl;
	float vValue = canvas->getVoltageValue();
	int lValue = canvas->getLinesToShowValue();
	for (int i = 0; i < nWaveAx; i++)
	{
		wAxes[i]->setRange(vValue*1000.0f);
		wAxes[i]->setLines(lValue);
	}
		

    // Each projection sets its limits using the limits of the two waveform dims it represents.
    // Convert projection number to indices, and then set the limits using those indices
    int j1, j2;
    for (int i = 0; i < nProjAx; i++)
    {
        pAxes[i]->n2ProjIdx(pAxes[i]->getType(), &j1, &j2);
        pAxes[i]->setRange(ranges[j1], ranges[j2]);
    }
}

void SpikePlot::initLimits()
{
    for (int i = 0; i < nChannels; i++)
    {
        limits[i][0] = 1209;//-1*pow(2,11);
        limits[i][1] = 11059;//pow(2,14)*1.6;
    }

}

void SpikePlot::getBestDimensions(int* w, int* h)
{
    switch (plotType)
    {
        case TETRODE_PLOT:
            *w = 4;
            *h = 2;
            break;
        case STEREO_PLOT:
            *w = 2;
            *h = 1;
            break;
        case SINGLE_PLOT:
            *w = 1;
            *h = 1;
            break;
        default:
            *w = 1;
            *h = 1;
            break;
    }
}

void SpikePlot::clear()
{
    std::cout << "SpikePlot::clear()" << std::endl;

    for (int i = 0; i < nWaveAx; i++)
        wAxes[i]->clear();
    for (int i = 0; i < nProjAx; i++)
        pAxes[i]->clear();
}

float SpikePlot::getDisplayThresholdForChannel(int i)
{
    return wAxes[i]->getDisplayThreshold();
}

void SpikePlot::setDisplayThresholdForChannel(int i, float thresh)
{
    std::cout << "Setting threshold to " << thresh << std::endl;
    wAxes[i]->setDisplayThreshold(thresh);
}

float SpikePlot::getRangeForChannel(int i)
{
    return wAxes[i]->getRange();
}

void SpikePlot::setRangeForChannel(int i, float range)
{
    std::cout << "Setting range to " << range << std::endl;
    wAxes[i]->setRange(range);
    //rangeButtons[i]->setLabel(String(int(range)));
}

void SpikePlot::setDetectorThresholdForChannel(int i, float t)
{
    // std::cout << "Setting threshold to " << t << std::endl;
    wAxes[i]->setDetectorThreshold(t);
}

void SpikePlot::registerThresholdCoordinator(SpikeThresholdCoordinator* stc)
{
    thresholdCoordinator = stc;
    stc->registerSpikePlot(this);
    for (int i=0; i < wAxes.size(); i++)
    {
        wAxes[i]->registerThresholdCoordinator(stc);
    }
}

void SpikePlot::setAllThresholds(float displayThreshold, float range)
{
    String label;
    for (int i=0; i< nWaveAx; i++)
    {
        ranges.set(i,range);
        wAxes[i]->setDisplayThreshold(displayThreshold);
    }

    if (range == 100)
    {
        label = "100";
    }
    else if (range == 250)
    {
        label = "250";
    }
    else
    {
        label = "500";
    }

    for (int i=0; i < rangeButtons.size(); i++)
    {
        rangeButtons[i]->setLabel(label);
    }

    setLimitsOnAxes();
}

void SpikePlot::invertSpikes(bool shouldInvert)
{
    for (int i = 0; i < nWaveAx; i++)
    {
        wAxes[i]->invertSpikes(shouldInvert);
    }
}

// --------------------------------------------------


WaveAxes::WaveAxes(int channel, int linesToShow) : GenericAxes(channel),
	bufferSize(linesToShow),
    drawGrid(true),
    displayThresholdLevel(0.0f),
    detectorThresholdLevel(0.0f),
    spikesReceivedSinceLastRedraw(0),
    spikeIndex(0),
    
    range(250.0f),
    isOverThresholdSlider(false),
    isDraggingThresholdSlider(false),
    thresholdCoordinator(nullptr),
    spikesInverted(false)

{

    addMouseListener(this, true);

    thresholdColour = Colours::red;

    font = Font("Small Text",10,Font::plain);
    for (int n = 0; n < bufferSize; n++)
    {
        spikeBuffer.add(nullptr);
    }
}

void WaveAxes::setRange(float r)
{

    std::cout << "Setting range to " << r << std::endl;

    range = r;

    repaint();
}

void WaveAxes::setLines(int l)
{

	std::cout << "Setting lines to show to " << l << std::endl;

	bufferSize = l;

	repaint();
}

void WaveAxes::paint(Graphics& g)
{
    g.setColour(Colours::black);
    g.fillRect(0,0,getWidth(), getHeight());

    // int chan = 0;

    // draw the grid lines for the waveforms

    if (drawGrid)
        drawWaveformGrid(g);

    // draw the threshold line and labels
    drawThresholdSlider(g);
    //drawBoundingBox(g);

    // if no spikes have been received then don't plot anything
    if (!gotFirstSpike)
    {
        return;
    }


    for (int spikeNum = 0; spikeNum < bufferSize; spikeNum++)
    {

        if (spikeNum != spikeIndex)
        {
            g.setColour(Colours::grey);
            plotSpike(spikeBuffer[spikeNum], g);
        }

    }

    
    plotSpike(spikeBuffer[spikeIndex], g);


    spikesReceivedSinceLastRedraw = 0;

}

void WaveAxes::plotSpike(const SpikeEvent* s, Graphics& g)
{
	if (!s) return;

    float h = getHeight();

    //compute the spatial width for each waveform sample
	int nSamples = s->getChannelInfo()->getTotalSamples();
	float dx = getWidth() /  float(nSamples);

    //TODO: check for special metadata for this
	//if (s.sortedId > 0)
    //   g.setColour(Colour(s.color[0],s.color[1],s.color[2]));
    //else
       g.setColour(Colours::white);

    // type corresponds to channel so we need to calculate the starting
    // sample based upon which channel is getting plotted
    int sampIdx = nSamples*type; //spikeBuffer[0].nSamples * type; //
	
    int dSamples = 1;

    float x = 0.0f;
	const float* data = s->getDataPointer();

	for (int i = 0; i < nSamples - 1; i++)
	{
		//std::cout << data[sampIdx] << std::endl;

		float s1, s2;

		if (spikesInverted)
		{
			s1 = h / 2 + data[sampIdx] / range * h;
			s2 = h / 2 + data[sampIdx + 1] / range * h;
		}
		else
		{
			s1 = h / 2 - data[sampIdx] / range * h;
			s2 = h / 2 - data[sampIdx + 1] / range * h;

		}
		g.drawLine(x,
			s1,
			x + dx,
			s2);




		sampIdx += dSamples;
		x += dx;
	}

}

void WaveAxes::drawThresholdSlider(Graphics& g)
{

    // draw display threshold (editable)
    float h;

    if (spikesInverted)
        h = getHeight()*(0.5f - displayThresholdLevel/range);
    else
        h = getHeight()*(0.5f + displayThresholdLevel/range);

    g.setColour(thresholdColour);
	g.drawLine(0, h, getWidth(), h);

    g.drawText(String(roundFloatToInt(displayThresholdLevel)),2,h,25,10,Justification::left, false);

    // draw detector threshold (not editable)
    if (spikesInverted)
        h = getHeight()*(0.5f - detectorThresholdLevel/range);
    else
        h = getHeight()*(0.5f + detectorThresholdLevel/range);

    g.setColour(Colours::orange);
    g.drawLine(0, h, getWidth(), h);
}

void WaveAxes::drawWaveformGrid(Graphics& g)
{

    float h = getHeight();
    float w = getWidth();

    g.setColour(Colours::darkgrey);

    for (float y = -range/2; y < range/2; y += 25.0f)
    {
        if (y == 0)
            g.drawLine(0,h/2 + y/range*h, w, h/2+ y/range*h,2.0f);
        else
            g.drawLine(0,h/2 + y/range*h, w, h/2+ y/range*h);
    }

}

bool WaveAxes::updateSpikeData(const SpikeEvent* s)
{
    if (!gotFirstSpike)
    {
        gotFirstSpike = true;
    }

    if (spikesReceivedSinceLastRedraw < bufferSize)
    {

        SpikeEvent* newSpike = new SpikeEvent(*s);

        spikeIndex++;
        spikeIndex %= bufferSize;

        spikeBuffer.set(spikeIndex, newSpike);

        spikesReceivedSinceLastRedraw++;

    }

    return true;

}

bool WaveAxes::checkThreshold(const SpikeEvent* s)
{
	int nSamples = s->getChannelInfo()->getTotalSamples();
    int sampIdx = nSamples*type;
	const float* data = s->getDataPointer();

    for (int i = 0; i < nSamples-1; i++)
    {

        //if (data[sampIdx] > displayThresholdLevel)
        //{
            return true;
        //}

        sampIdx++;
    }

    return false;

}

void WaveAxes::clear()
{

    spikeBuffer.clear();
    spikeIndex = 0;

    for (int n = 0; n < bufferSize; n++)
    {
        spikeBuffer.add(nullptr);
    }

    repaint();
}

void WaveAxes::mouseMove(const MouseEvent& event)
{

    // Point<int> pos = event.getPosition();

    float y = event.y;

    float h = getHeight()*(0.5f - displayThresholdLevel/range);

    if (spikesInverted)
        h = getHeight()*(0.5f - displayThresholdLevel/range);
    else
        h = getHeight()*(0.5f + displayThresholdLevel/range);

    // std::cout << y << " " << h << std::endl;

    if (y > h - 10.0f && y < h + 10.0f && !isOverThresholdSlider)
    {
        thresholdColour = Colours::yellow;

        //  std::cout << "Yes." << std::endl;

        repaint();

        isOverThresholdSlider = true;

        // cursorType = MouseCursor::DraggingHandCursor;

    }
    else if ((y < h - 10.0f || y > h + 10.0f) && isOverThresholdSlider)
    {

        thresholdColour = Colours::red;
        repaint();

        isOverThresholdSlider = false;

        //   cursorType = MouseCursor::NormalCursor;

    }


}

void WaveAxes::mouseDown(const MouseEvent& event)
{
    // if (isOverThresholdSlider)
    // {
    //     cursorType = MouseCursor::DraggingHandCursor;
    // }
}

void WaveAxes::mouseDrag(const MouseEvent& event)
{
   /* if (isOverThresholdSlider)
    {

        float thresholdSliderPosition =  float(event.y) / float(getHeight());

        if (spikesInverted)
        {
            if (thresholdSliderPosition > 0.5f)
                thresholdSliderPosition = 0.5f;
            else if (thresholdSliderPosition < 0.0f)
                thresholdSliderPosition = 0.0f;

            displayThresholdLevel = (0.5f - thresholdSliderPosition) * range;
        }
        else
        {
            if (thresholdSliderPosition < 0.5f)
                thresholdSliderPosition = 0.5f;
            else if (thresholdSliderPosition > 1.0f)
                thresholdSliderPosition = 1.0f;

            displayThresholdLevel = (thresholdSliderPosition-0.5f) * range;
        }

        //std::cout << "Threshold = " << thresholdLevel << std::endl;

        if (thresholdCoordinator)
        {
            thresholdCoordinator->thresholdChanged(displayThresholdLevel,range);
        }

        repaint();
    }*/
}

// MouseCursor WaveAxes::getMouseCursor()
// {
//     MouseCursor c = MouseCursor(cursorType);

//     return c;
// }

void WaveAxes::mouseExit(const MouseEvent& event)
{
    if (isOverThresholdSlider)
    {
        isOverThresholdSlider = false;
        thresholdColour = Colours::red;
        repaint();
    }
}

float WaveAxes::getDisplayThreshold()
{
    return displayThresholdLevel;
}

void WaveAxes::setDetectorThreshold(float t)
{
    detectorThresholdLevel = t;
}

void WaveAxes::registerThresholdCoordinator(SpikeThresholdCoordinator* stc)
{
    thresholdCoordinator = stc;
}

void WaveAxes::setDisplayThreshold(float threshold)
{
    displayThresholdLevel = threshold;

    repaint();
}

// --------------------------------------------------

ProjectionAxes::ProjectionAxes(int projectionNum) : GenericAxes(projectionNum), imageDim(500),
    rangeX(250), rangeY(250), spikesReceivedSinceLastRedraw(0)
{
    projectionImage = Image(Image::RGB, imageDim, imageDim, true);

    clear();
    //Graphics g(projectionImage);
    //g.setColour(Colours::red);
    //g.fillEllipse(20, 20, 300, 200);

    n2ProjIdx(projectionNum, &ampDim1, &ampDim2);


}

void ProjectionAxes::setRange(float x, float y)
{
    rangeX = (int) x;
    rangeY = (int) y;

    //std::cout << "Setting range to " << x << " " << y << std::endl;

    repaint();
}

void ProjectionAxes::paint(Graphics& g)
{
    //g.setColour(Colours::orange);
    //g.fillRect(5,5,getWidth()-5, getHeight()-5);

    g.drawImage(projectionImage,
                0, 0, getWidth(), getHeight(),
                0, imageDim-rangeY, rangeX, rangeY);
}

bool ProjectionAxes::updateSpikeData(const SpikeEvent* s)
{
    if (!gotFirstSpike)
    {
        gotFirstSpike = true;
    }

    int idx1, idx2;
    calcWaveformPeakIdx(s, ampDim1, ampDim2, &idx1, &idx2);

    // add peaks to image
    Colour col;

	//Again, fix this adding proper metadata check
    //if (s.sortedId > 0)
    //    col = Colour(s.color[0], s.color[1], s.color[2]);
    //else
        col = Colours::white;

	const float* data = s->getDataPointer();
    updateProjectionImage(data[idx1], data[idx2], 1, col);

    return true;
}

void ProjectionAxes::updateProjectionImage(float x, float y, float gain, Colour col)
{
    Graphics g(projectionImage);

    // h/2 + float(s.data[sampIdx]-32768)/float(*s.gain)*1000.0f / range * h;

    if (gain != 0)
    {
		float xf = x;
        float yf = float(imageDim) - y; // in microvolts

        g.setColour(col);
        g.fillEllipse(xf,yf,2.0f,2.0f);
    }

}

void ProjectionAxes::calcWaveformPeakIdx(const SpikeEvent* s, int d1, int d2, int* idx1, int* idx2)
{

    float max1 = -1*pow(2.0,15);
    float max2 = max1;
	int nSamples = s->getChannelInfo()->getTotalSamples();
	const float* data = s->getDataPointer();

    for (int i = 0; i < nSamples; i++)
    {
        if (data[d1*nSamples + i] > max1)
        {
            *idx1 = d1*nSamples+i;
            max1 = data[*idx1];
        }
        if (data[d2*nSamples+i] > max2)
        {
            *idx2 = d2*nSamples+i;
            max2 = data[*idx2];
        }
    }
}



void ProjectionAxes::clear()
{
    projectionImage.clear(Rectangle<int>(0, 0, projectionImage.getWidth(), projectionImage.getHeight()),
                          Colours::black);

    repaint();
}

void ProjectionAxes::n2ProjIdx(int proj, int* p1, int* p2)
{
    int d1, d2;
    if (proj==PROJ1x2)
    {
        d1 = 0;
        d2 = 1;
    }
    else if (proj==PROJ1x3)
    {
        d1 = 0;
        d2 = 2;
    }
    else if (proj==PROJ1x4)
    {
        d1 = 0;
        d2 = 3;
    }
    else if (proj==PROJ2x3)
    {
        d1 = 1;
        d2 = 2;
    }
    else if (proj==PROJ2x4)
    {
        d1 = 1;
        d2 = 3;
    }
    else if (proj==PROJ3x4)
    {
        d1 = 2;
        d2 = 3;
    }
    else
    {
        std::cout<<"Invalid projection:"<<proj<<"! Cannot determine d1 and d2"<<std::endl;
        *p1 = -1;
        *p2 = -1;
        return;
    }
    *p1 = d1;
    *p2 = d2;
}

// --------------------------------------------------

GenericAxes::GenericAxes(int t)
    : gotFirstSpike(false), type(t)
{
    ylims[0] = 0;
    ylims[1] = 1;

    xlims[0] = 0;
    xlims[1] = 1;

    font = Font("Default", 12, Font::plain);

}

GenericAxes::~GenericAxes()
{

}

bool GenericAxes::updateSpikeData(const SpikeEvent* newSpike)
{
    if (!gotFirstSpike)
    {
        gotFirstSpike = true;
    }

  //  s = newSpike;
    return true;
}

void GenericAxes::setYLims(double ymin, double ymax)
{

    //std::cout << "setting y limits to " << ymin << " " << ymax << std::endl;
    ylims[0] = ymin;
    ylims[1] = ymax;
}
void GenericAxes::getYLims(double* min, double* max)
{
    *min = ylims[0];
    *max = ylims[1];
}
void GenericAxes::setXLims(double xmin, double xmax)
{
    xlims[0] = xmin;
    xlims[1] = xmax;
}
void GenericAxes::getXLims(double* min, double* max)
{
    *min = xlims[0];
    *max = xlims[1];
}


void GenericAxes::setType(int t)
{
    if (t < WAVE1 || t > PROJ3x4)
    {
        std::cout<<"Invalid Axes type specified";
        return;
    }
    type = t;
}

int GenericAxes::getType()
{
    return type;
}

int GenericAxes::roundUp(int numToRound, int multiple)
{
    if (multiple == 0)
    {
        return numToRound;
    }

    int remainder = numToRound % multiple;
    if (remainder == 0)
        return numToRound;
    return numToRound + multiple - remainder;
}


void GenericAxes::makeLabel(int val, int gain, bool convert, char* s)
{
    if (convert)
    {
		double volt = ad16ToUv(val, gain);// / 1000.;
        if (abs(val)>1e6)
        {
            //val = val/(1e6);
            sprintf(s, "%.2fV", volt);
        }
        else if (abs(val)>1e3)
        {
            //val = val/(1e3);
            sprintf(s, "%.2fmV", volt);
        }
        else
            sprintf(s, "%.2fuV", volt);
    }
    else
    {
        sprintf(s,"%d", (int)val);
    }
}

double GenericAxes::ad16ToUv(int x, int gain)
{
    int result = (double)(x * 20e6) / (double)(gain * pow(2.0,16));
    return result;
}

SpikeThresholdCoordinator::SpikeThresholdCoordinator() : lockThresholds(false) {}

SpikeThresholdCoordinator::~SpikeThresholdCoordinator()
{
    masterReference.clear();
}

void SpikeThresholdCoordinator::registerSpikePlot(SpikePlot* sp)
{
    registeredPlots.addIfNotAlreadyThere(sp);
}

void SpikeThresholdCoordinator::unregisterSpikePlot(SpikePlot* sp)
{
    registeredPlots.removeAllInstancesOf(sp);
}

void SpikeThresholdCoordinator::setLockThresholds(bool en)
{
    lockThresholds = en;
}

bool SpikeThresholdCoordinator::getLockThresholds()
{
    return lockThresholds;
}

void SpikeThresholdCoordinator::thresholdChanged(float displayThreshold, float range)
{
    if (lockThresholds)
    {
        for (int i = 0; i < registeredPlots.size(); i++)
        {
            registeredPlots[i]->setAllThresholds(displayThreshold,range);
        }
    }
}