/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CsvtoMidiAudioProcessorEditor::CsvtoMidiAudioProcessorEditor (CsvtoMidiAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 200);

	addAndMakeVisible(btnLoadCSV);
	btnLoadCSV.setButtonText("Load CSV");
	btnLoadCSV.addListener(this);

	addAndMakeVisible(btnPlay);
	btnPlay.setButtonText("Play");
	btnPlay.addListener(this);
	btnPlay.setEnabled(false);
	//btnPlay.setClickingTogglesState(true);

	addAndMakeVisible(btnClear);
	btnClear.setButtonText("Clear Bins");
	btnClear.addListener(this);
}

CsvtoMidiAudioProcessorEditor::~CsvtoMidiAudioProcessorEditor()
{
	btnLoadCSV.removeListener(this);
}

//==============================================================================
void CsvtoMidiAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    //g.setColour (Colours::white);
    //g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), Justification::centred, 1);
}

void CsvtoMidiAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
	btnLoadCSV.setBounds(0, 10, getWidth() / 2, getWidth() / 4);
	btnPlay.setBounds(getWidth() / 2, 10, getWidth() / 2, getWidth() / 4);
	btnClear.setBounds(0, getWidth() / 4 + 10, getWidth() / 2, getWidth() / 4);
}

void CsvtoMidiAudioProcessorEditor::buttonClicked(Button * button)
{
	if (button == &btnLoadCSV) {
		FileChooser chooser("Select a CSV file", File::nonexistent, "*.csv");
		if (chooser.browseForFileToOpen()) {
			auto file = chooser.getResult();
			if (processor.loadCSV(file.getFullPathName())) {
				btnPlay.setEnabled(true);
			}
		}
	}
	else if (button == &btnPlay) {
		if (processor.togglePlay()) {
			btnPlay.setButtonText("Stop");
		}
		else btnPlay.setButtonText("Play");
	}
	else if (button == &btnClear) {
		processor.resetBins();
	}
}
