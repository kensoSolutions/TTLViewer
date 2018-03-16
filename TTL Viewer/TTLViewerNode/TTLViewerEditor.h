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

#ifndef TTLVIEWEREDITOR_H_
#define TTLVIEWEREDITOR_H_

#include <VisualizerEditorHeaders.h>
#include <RecordingLib.h>
#include "TTLViewerNode.h"
#include "TTLViewerCanvas.h"


#define MAX_N_SUB_CHAN 8
class Visualizer;
class UtilityButton;

/**

  User interface for the TTLViewerNode sink.

  @see TTLViewerNode, TTLViewerCanvas

*/

class TTLViewerEditor : public VisualizerEditor
{
public:
	TTLViewerEditor(GenericProcessor*);
    ~TTLViewerEditor();

    void buttonEvent(Button* button);
    void startRecording();
    void stopRecording();

    // void updateSettings();
    // void updateVisualizer();

    Visualizer* createNewCanvas();

private:

    UtilityButton* panUpBtn;
    UtilityButton* panDownBtn;
    UtilityButton* zoomInBtn;
    UtilityButton* zoomOutBtn;
    UtilityButton* clearBtn;
    UtilityButton* saveImgBtn;
	
    Label* panLabel;
    Label* zoomLabel;
	Label* voltageRangeLabel;

    UtilityButton* allSubChansBtn;

	
	ComboBox* voltageRange;

    int nSubChannels;
    Label* subChanLabel;
    UtilityButton* subChanBtn[MAX_N_SUB_CHAN];
    bool subChanSelected[MAX_N_SUB_CHAN];

    void initializeButtons();


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TTLViewerEditor);

};

#endif  // TTLVIEWEREDITOR_H_
